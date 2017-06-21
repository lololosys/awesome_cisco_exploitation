/* $Id: if_as5200_modem.c,v 3.1.66.8 1996/09/10 03:57:07 elem Exp $
 * $Source: /release/112/cvs/Xsys/les/if_as5200_modem.c,v $
 *------------------------------------------------------------------
 * if_as5200_modem.c Modem Board Software
 *
 * Nov 95, Eddy Lem
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_as5200_modem.c,v $
 * Revision 3.1.66.8  1996/09/10  03:57:07  elem
 * CSCdi67595:  OOB port assignment incorrect when only one modem card
 * installed
 * Branch: California_branch
 * The OOB ports should be allocated to the modems based on the physical
 * ports on the carrier card, and not the total of number of modems
 * found.  Plus some minor clean up for missing modem modules.
 *
 * Revision 3.1.66.7  1996/09/10  03:28:59  elem
 * CSCdi54417:  modem busyout generates report as cleanup pending.
 * Delaying DTR
 * Branch: California_branch
 * The setting of the MODEM_AUTOCONFIG_2 flag to hold off the modem
 * state machine is moved off to modem_lock_tty().  This will prevent
 * the cleanup pending message in all situations when the CSM needs
 * to lock the tty line -- not just during modem maintainence activity.
 *
 * Revision 3.1.66.6  1996/09/07  02:07:51  snyder
 * CSCdi68409:  declare more things const
 *              572 out of data, 12 image
 * Branch: California_branch
 *
 * Revision 3.1.66.5  1996/08/08  17:57:35  sjackson
 * CSCdi65288:  Modem autoconfig should always redetect and attempt to
 * lock speed
 * Branch: California_branch
 * For modem autoconfig the type is constant, populate the registry.
 *
 * Revision 3.1.66.4  1996/08/07  09:56:57  snyder
 * CSCdi65343:  more things that should be declared const
 *              remove an unnecessary extern thats in the way.
 * Branch: California_branch
 *
 * Revision 3.1.66.3  1996/07/14  18:27:36  elem
 * CSCdi61142:  Upon reload modems do not respond to polling and CTS is low
 * Branch: California_branch
 * Main problem was some modems failed to initialize, and was marked bad.
 * Cause is PPP dedicated mode putting cirrus chip in hardware ppp mode,
 * and resets tty line during startup.  For now extended the number of
 * retries during modem startup.
 * Fixed flash_copy_file_name call which dropped one argument for
 * CSCdi52582.
 * Also some minor code clean up.
 *
 * Revision 3.1.66.2  1996/06/28  23:23:42  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.1.66.1  1996/06/16  21:14:52  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/11  21:36:32  elem
 * Place holder for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include <ciscolib.h>
#include "sys_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "logger.h"
#include "ttysrv.h"
#include "globs.h"
#include "if_les.h"
#include "../ts/nonvol_x2444.h"
#include "parser_defs_modem.h"
#include "../dev/flash_defines.h"
#include "packet.h"
#include "../src-68-as5200/as5200.h"
#include "if_as5200_tdm.h"
#include "isdn_bri.h"
#include "if_as5200_modem.h"
#include "modem_mgmt.h"
#include "../h/cs_registry.regh"
#include "../ts/as5200_cd2430.h"
#include "../ts/async_ppp.h"
#include "../wan/ppp.h"

modem_pool_t *modem_pool;

static modem_info_t    *modem_oob_req_modem;
static int 	       *modem_oob_req_status;
static watched_boolean *modem_oob_req_event;
static paktype         **modem_oob_rcv_packet = NULL;
static paktype         *modem_oob_free_packet = NULL;

static const unsigned char ppp_lcp_escapemap[256] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 00 - 07 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 08 - 0f */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 10 - 17 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 18 - 1f */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 20 - 27 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 28 - 2f */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 30 - 37 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 38 - 3f */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 40 - 47 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 48 - 4f */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 50 - 57 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 58 - 5f */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 60 - 67 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 68 - 6f */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 70 - 77 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, /* 78 - 7f */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 80 - 87 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 88 - 8f */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 90 - 97 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 98 - 9f */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* a0 - a7 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* a8 - af */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* b0 - b7 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* b8 - bf */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* c0 - c7 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* c8 - cf */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* d0 - d7 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* d8 - df */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* e0 - e7 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* e8 - ef */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* f0 - f7 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  /* f8 - ff */
};

#define ISON(map,ix) ((map)[(ix)])

/*
 * FCS lookup table as calculated by genfcstab.
 */

#define MODEM_POOL_MAJVERSION  1
#define MODEM_POOL_MINVERSION  0
#define MODEM_POOL_EDITVERSION 1

/*
 * modem_lock_tty
 *    -	Grab the tty line.
 */
void modem_lock_tty (modem_info_t *modem_info)
{
    if (modem_info->modem_tty) {
	CLAIM_TTY(modem_info->modem_tty);
	modem_info->modem_tty->statbits2 |= MODEM_AUTOCONFING_2;
    }
}

/*
 * modem_unlock_tty
 *    -	Release the tty line.
 */
void modem_unlock_tty (modem_info_t *modem_info)
{
    if (modem_info->modem_tty) {
	RELEASE_TTY(modem_info->modem_tty);
	modem_info->modem_tty->statbits2 &= ~MODEM_AUTOCONFING_2;
    }
}

/*
 * modem_set_codec_type
 *    -	Function to be called from T1/E1 software, via a registry,
 *	to inform the modem board of the proper A-law/u-law setting.
 */
static void modem_set_codec_type (boolean encoding)
{
    modem_pool->encoding = encoding ? A_LAW_ENCODING : U_LAW_ENCODING ;
}

/*
 * modem_release_override
 *    -	This function will be called, via a registry, to inform the
 *	modem board software that the tty line is being released.
 *	In here, the modem board software may instantly grab the
 *	tty line.
 */
static boolean modem_release_override (tt_soc *tty)
{
    /* 
     * Run in the interrupt context, return immediately after the action.
     * Cannot block the execution.
     */
    modem_info_t *modem_info;
    leveltype     SR;
    boolean       override;
  
    modem_info = modem_ttynum_to_modem_info(tty->ttynum);
    if (modem_info == NULL) {
        return (FALSE);
    }

    SR = raise_interrupt_level(NETS_DISABLE);
 
    override = csm_do_pending_request(modem_info);

    reset_interrupt_level(SR);
    return (override);
}

/*
 * modem_hardware_in_use
 *    -	This is called to let others know that the modem board
 *	software is using the tty line.
 */
static boolean modem_hardware_in_use (tt_soc *tty)
{
    modem_info_t *modem_info;
    leveltype     SR;
    boolean       in_use;

    modem_info = modem_ttynum_to_modem_info(tty->ttynum);
    if (modem_info == NULL) {
	return (FALSE);
    }

    SR = raise_interrupt_level(NETS_DISABLE);

    in_use = CSM_IS_LOCKED(modem_info);

    reset_interrupt_level(SR);
    return (in_use);
}

/*
 * modem_busyout_tty
 *    -	For systems with modem boards, the busyout is done through
 *	the modem board software.  This is called to initiate the
 *	modem busyout.
 */
static boolean modem_busyout_tty (tt_soc *tty, boolean sense)
{
    modem_info_t        *modem_info;
 
    modem_info = modem_ttynum_to_modem_info(tty->ttynum);
    if (modem_info) {
	mai_busyout_a_modem(modem_info, sense);
	/* indicate that busyout action is taken */
	return (TRUE);
    }
    return (FALSE);
}

/*
 * modem_busyout_present
 *    -	This is used to indicate that the modem board will provide
 *	the mechanism modem busyout for this tty.
 */
static boolean modem_busyout_present (tt_soc *tty, boolean printerror)
{
    modem_info_t        *modem_info;
 
    modem_info = modem_ttynum_to_modem_info(tty->ttynum);
    if (modem_info) {
        return (TRUE);
    }
    return (FALSE);   
}

/*
 * modem_pool_type
 *
 * Return a description of the modem pool
 */
static char *modem_pool_type (tt_soc *tty)
{
    if (modem_ttynum_to_modem_info(tty->ttynum)) {
	return ("microcom_hdms");
    }
    return (NULL);
}

/*
 * modem_shutdown
 *    -	This will initiate the modem shutdown.
 */
boolean modem_shutdown (modem_info_t *modem_info, boolean sense)
{
    if (!modem_info) {
        return (FALSE);
    }
    
    if (sense) {
        return (csm_set_shutdown_status(modem_info));
    } else {
        return (csm_clear_shutdown_status(modem_info));
    }
}

/*
 * modem_bad
 *    -	This will be used to mark the modem bad.
 */
boolean modem_bad (modem_info_t *modem_info, boolean sense)
{
    if (!modem_info) {
        return (FALSE);
    }
 
    if (sense) {
        return (csm_set_bad_modem_status(modem_info));
    } else {
	csm_clear_download_fail_status(modem_info);
        return (csm_clear_bad_modem_status(modem_info));
    }
}

/*
 * modem_reset
 *    -	This is used to initiate the modem reset.
 */
boolean modem_reset (modem_info_t *modem_info,
                     boolean sense, boolean wait_for_reset)
{
    if (!modem_info) {
        return (FALSE);
    }
 
    /* Make sure not doing other maintenance (download or back-to-back) */
    if (!mai_clear_maintns(modem_info)) {
	return (FALSE);
    }

    if (sense) {
	/*
	 * Place the modem in reset state
	 */
	csm_set_busy_status(modem_info);

        /* Tell modem mgmt that oob port is off now */
	if (csm_set_modem_reset_status(modem_info)) {
	    reg_invoke_modem_mgmt_oob_operation((void *)modem_info,
					OOB_PORT_OFF);
	    return (TRUE);
	}
	return (FALSE);
    } else {
        /*
         * Take modem out of reset state
         */
 
	if (CSM_IS_MODEM_RESET(modem_info)) {
	    if (csm_clear_modem_reset_status(modem_info, wait_for_reset)) {
		mai_restart_modem(modem_info);
	    } else {
		return (FALSE);
	    }
	}
    }
    return (TRUE);
}

/*
 * modem_init_async_idb_info
 *    -	This function will tie the hardware idb to the modem_info.
 *	It should be called, via a registry, whenever a hardware idb
 *	is created for a tty line.
 */
static void modem_init_async_idb_info (hwidbtype *hwidb)
{
    modem_info_t *modem;
    hwidb_sb_t   sb_type;

    sb_type = HWIDB_SB_MODEM_INFO;
    modem = modem_ttynum_to_modem_info(hwidb->idb_tty->ttynum);
    idb_add_hwsb(hwidb, &sb_type, modem);

    if (modem) {
	hwidb->slot = CSM_MODEM_SLOT(modem);
	hwidb->subunit = CSM_MODEM_PORT(modem);
    }
}


/*
 * modem_oob_request_reset
 *    -	Called to abort the last oob request, if not yet completed.
 *	Any current data being received is trashed.
 */
void modem_oob_request_reset(modem_info_t *modem)
{
    pppContext *ctx;
    leveltype SR;

    /* Lockout tty interrupts*/
    SR = raise_interrupt_level(TTY_DISABLE);
    clearinput(modem->mgmt_tty);
    clearoutput(modem->mgmt_tty);
    ctx = modem->mgmt_tty->appp_context;
    if (ctx->inpak) {
	modem_oob_free_packet = ctx->inpak;
	ctx->inpak = NULL;
    }
    reset_interrupt_level(SR);
}

/*
 * modem_oob_request_send
 *    -	This is used to send a oob message to modem, and then set up to
 *	receive any response that may come back.
 */
int modem_oob_request_send (modem_info_t *modem, paktype *req_buf,
                        paktype **rsp_buf,  int *status,
                        watched_boolean **event)
{
    pppContext *ctx;
    leveltype SR;

    ctx = modem->mgmt_tty->appp_context;

    /* save the requests */
    modem_oob_req_modem = modem;
    modem_oob_req_status = status;
    modem_oob_req_event = *event;
    modem_oob_rcv_packet = rsp_buf;
    if (ctx->inpak == NULL) {
	if (modem_oob_free_packet) {
	    ctx->inpak = modem_oob_free_packet;
	    modem_oob_free_packet = NULL;
	} else {
	    /* Lockout tty interrupts*/
	    SR = raise_interrupt_level(TTY_DISABLE); 
	    if (!appp_new_inpak(ctx)) {
		reset_interrupt_level(SR);
		return (!MGMT_STATUS_NO_ERROR);
	    }
	    reset_interrupt_level(SR);
	}
    }
    *rsp_buf = NULL;
    ctx->inpak->datagramsize = 0;
    ctx->inpak->dataptr = ctx->inpak->datagramstart;
    ctx->outpak = req_buf;
    /* Initially, send an end of frame */
    ctx->outstate = APPP_FRAME_END;
    ctx->outchar = -1;		/* No output character yet */
    ctx->inpak->unspecified[0] = PPPINITFCS;

    *modem_oob_req_status = MGMT_STATUS_NO_DATA;
    /* ask cd2430 start transfer data */
    startoutput(modem->mgmt_tty);
    return (MGMT_STATUS_NO_ERROR);
}

/*
 * modem_oob_txservice
 *    -	TX service routine which handles the sending of messages to the modem
 *	on the oob bus, in PPP framing.
 */
uint modem_oob_txservice (tt_soc *tty, uchar *p, int max)
{
    int ch, len = 0;
    pppContext *ctx = tty->appp_context;

    while (len < max) {
	/* Fetch any saved character */
	ch = ctx->outchar;
	/* Saved output character? */	
	if (ch >= 0) {
	    /* Yes, clear it for next time */
	    ctx->outchar = -1;
	} else {
	    switch (ctx->outstate) {
	    /* Output a byte of data */
	    case APPP_DATA:

		/* Fetch the character */
		ch = *ctx->outptr++;
		/* Accumulate the FCS */
		FCS_ACCUMULATE(ctx->outfcs, ch);

		/* End of the packet? */
		if (--ctx->outlen == 0) {
		    /* Yes, change state */
		    ctx->outstate = APPP_FCS_LSB;
		    ctx->outpak = NULL;
		}
		break;
		
	    /* Handle LSB of FCS */
	    case APPP_FCS_LSB:
		/* Pick up the LSB */
		ch = (~ctx->outfcs) & 0xFF;
		ctx->outstate = APPP_FCS_MSB;
		break;
		
	    /* Handle MSB of FCS */
	    case APPP_FCS_MSB:
		/* Pick up the MSB */
		ch = ((~ctx->outfcs) >> 8) & 0xFF;
		/* Send frame mark next */
		ctx->outstate = APPP_FRAME_END;
		break;

	    /* Handle end of frame */
	    case APPP_FRAME_END:
		/*
		 * Get next packet to send
		 */
		ctx->ActiveEscapeMap = ppp_lcp_escapemap;

		/*
		 * If have a packet, set it up as the next buffer to
		 * be sent via txservice.
		 */
		if (ctx->outpak) {
    	            ctx->outptr = ctx->outpak->datagramstart;
                    ctx->outlen = ctx->outpak->datagramsize;
                    ctx->outfcs = PPPINITFCS;
		    /* Change the state */
		    ctx->outstate = APPP_DATA; 

		} else {
		    /* Mark line as idle */
		    ctx->outstate = APPP_IDLE;
		    /* prepare to receive packet */
		    ctx->inescape = FALSE;
		}
		*p++ = PPP_FRAME_END;
		len += 1;
		continue;

	    case APPP_IDLE:		/* Idle state, nothing to do */
		return(len);
	    }

	    /*
	     * At this point, we have a character to be output, with
	     * possible quoting. If the character is to be quoted, we
	     * save the escaped character for next time, and send the
	     * escape now.
	     */
	    if (ISON(ctx->ActiveEscapeMap, ch)) {
		ctx->outchar = PPP_ESCAPE_MECHANISM(ch);
		ch = PPP_FRAME_ESC;
	    }
	}

	/*
	 * Add character to the output buffer
	 */
	*p++ = ch;
	len += 1;
    }
    return(len);
}

static void modem_oob_rcv_done (tt_soc *tty, paktype *pak, int status)
{
    *modem_oob_req_status = status;
    *modem_oob_rcv_packet = pak;
    process_set_boolean(modem_oob_req_event, TRUE);
}

/*
 * modem_oob_rxservice
 *    -	RX service routine which handles the reception of messages from the
 *	modem on the oob bus, in PPP framing.
 */
static void modem_oob_rxservice (tt_soc *tty, uchar *p, int len)
{
    pppContext *ctx = tty->appp_context;
    uchar ch;
    paktype *pak;

    pak = ctx->inpak;
    if (!pak) {
	return;
    }

    while (len-- > 0) {
	ch = *p++;
	/* Should we escape this character */
	if (ctx->inescape) {
	    /* Yes, get out of escape state */
	    ctx->inescape = FALSE;
	    if (ch == PPP_FRAME_END) {
		modem_oob_rcv_done(tty, NULL, MGMT_STATUS_DATA_ERROR);
		modem_oob_free_packet = pak;
		ctx->inpak = NULL;
		return;
	    }
	    /* Unescape the character */
	    ch = PPP_ESCAPE_MECHANISM(ch);
	} else {
	    switch (ch) {
	      case PPP_FRAME_END:
		if (pak->datagramsize == 0) {
		    continue;
		}
		if ((pak->unspecified[0] & 0xffff) != PPPGOODFCS) {
		   modem_oob_rcv_done(tty, pak, MGMT_STATUS_CRC_ERROR);
		   modem_oob_free_packet = pak;
		   ctx->inpak = NULL;
		   return;
                }
                pak->datagramsize -= PPP_FCS_SIZE;
		modem_oob_rcv_done(tty, pak, MGMT_STATUS_GOOD_DATA);
		ctx->inpak = NULL;
                ctx->inpaks++;
                return;

	      case PPP_FRAME_ESC:
		ctx->inescape = TRUE;
		continue;
	    }
	}

        pak->datagramsize++;
        *pak->dataptr++ = ch;   /* Store the character */
	FCS_ACCUMULATE(pak->unspecified[0], ch);       /* Accumulate CRC */

    }
}

/*
 * modem_oob_ppp_startup
 *    -	Initialize the oob port tty line for ppp.
 */
static boolean modem_oob_ppp_startup (tt_soc *tty)
{
    pppContext *ctx;
    uchar ch;

    ctx = malloc_named(sizeof(pppContext), "Modem OOB Port");
    if (!ctx) {
      return(FALSE);		/* Can't get a context block */
    }
    for (ch = 0; ch < 0x20; ch++) {
	  appp_remove_quoted_char(tty, ch);
    }
    tty_pppmap(tty, 0L, 0L);
    /* modify appp context to fit in the as5200 modem management */
    ctx->idb = NULL;
    tty->appp_context = ctx;
    ctx->iqueue = NULL;
    ctx->ioffset = 0;
    ctx->imax = 512;
    /* No tx data yet */
    ctx->outstate = APPP_IDLE;
    ctx->inpak = NULL;
    ctx->outpak = NULL;
    clearoutput(tty);
    /* turn on software ppp packet encode/decode service routine */
    tty_servicehook(tty, modem_oob_txservice, modem_oob_rxservice);
    setspeed(tty, ASYNC_DEFSPEED, ASYNC_DEFSPEED);
    setstopbits(tty, STOPBITS1);
    setdatabits(tty, 8);
    setparity(tty, PARITY_NO);
    /* hookup private transmit and receive service in the interrupt routine */
    tty_portenable(tty);
    return(TRUE);
}

/*
 * modem_x2444_io
 *    -	Generic utility to interface to all x2444 serial eeproms
 *	on the modem board.
 */
static ulong modem_x2444_io (
     ushort *reg,
     ushort chipselect,
     ushort sclock,
     ushort datain,
     ushort dataout,
     int cmd,
     int readbits,
     int writebits)
{
    int ix;
    ulong value = 0;

    /*
     * Enable the X2444
     */

    /* Clear all lines first */
    *reg = 0;
    /* And delay for one millisecond */

    DELAY(1);
    /* Clear clock and data in */
    *reg &= ~(sclock | datain);
    /* Set chip enable */
    *reg |= chipselect;
    /* And delay for one millisecond */
    DELAY(1);

    /*
     * Give the command to the X2444
     */

    for (ix = 0; ix < readbits; ix++) {
	if (cmd & 0x1) {
	  /* Set the data in bit */
	  *reg |= datain;
	} else {
	  /* Clear the data in bit */
	  *reg &= ~datain;
	}
	/* Delay one millisecond */
	DELAY(1);
	/* Set the clock bit */
	*reg |= sclock;
	/* Delay one millisecond */
	DELAY(1);
	/* Reset the clock bit */
	*reg &= ~sclock;
	/* Delay one millisecond */
	DELAY(1);
	/* Shift right one bit */
	cmd >>= 1;
    }
    /* Set data in bit */
    *reg |= datain;
    /* Delay one millisecond */
    DELAY(1);

    /*
     * Read the response from the X2444
     */

    for (ix = 0; ix < writebits; ix++) {
	/* If the data bit is set */
	if (*reg & dataout) {
	    /* Set the appropriate value bit */
	    value |= (1 << ix);
	}
	/* Set the clock bit */
	*reg |= sclock;
	/* Delay one millisecond */
	DELAY(1);
	/* Clear the clock bit */
	*reg &= ~sclock;
	/* Delay one millisecond */
	DELAY(1);
    }

    /*
     * Disable the X2444
     */

    /* Clear chip enable and DI*/
    *reg &= ~(chipselect | datain);
    /* And delay */
    DELAY(1);

    /*
     * We're done
     */
    return(value);
}

/*
 * modem_read_modem_cookies
 *    -	This will read all the cookies on the modem board (carrier and
 *	modem module).
 */
static void modem_read_modem_cookies (modem_slot_t *slotinfo)
{
    int	ix, jx, nmodules, ncarrier_words, nmodule_words;
    modem_cookie_t *modem_cookie;
    modem_carrier_cookie_t tmp_carrier;
    modem_module_cookie_t *module_cookie;
    uchar chipsel;
    ushort *reg, *wordptr;
 
    if (slotinfo->modem_cookie != NULL) {
	return;
    }

    ncarrier_words = sizeof(modem_carrier_cookie_t)/2;
    if (ncarrier_words > MAX_WORDS_PER_X2444) {
	ncarrier_words = MAX_WORDS_PER_X2444; 
    }
    nmodule_words = sizeof(modem_module_cookie_t)/2;
    if (nmodule_words > MAX_WORDS_PER_X2444) {
	nmodule_words = MAX_WORDS_PER_X2444; 
    }

    /* get serial prom access register */
    reg = (ushort *)&(slotinfo->modem_reg)->mdb_eepm;

    /* read carrier card cookie in to tmp first */
    modem_x2444_io(reg, X2444_CE_CARRIER, MODEM_X2444_SK, MODEM_X2444_DI,
				MODEM_X2444_DO, X2444_CMD_RCL, 8, 0);
    wordptr = (ushort *)&tmp_carrier;
    for (ix = 0; ix < ncarrier_words; ix++) {
	*(wordptr+ix) = modem_x2444_io(reg, X2444_CE_CARRIER, MODEM_X2444_SK,
	   			MODEM_X2444_DI, MODEM_X2444_DO,
				X2444_CMD_READ + (ix * 2), 
				X2444_NUM_CMDBITS_OUT, 
				X2444_NUM_DATABITS_IN);
    }

    nmodules = tmp_carrier.max_modules;

    if (nmodules > MAX_MODEM_MODULE_PER_SLOT) {
	nmodules = tmp_carrier.max_modules = MAX_MODEM_MODULE_PER_SLOT;
	printf("\n%WARNING: Maximum number of modem modules exceeded. ");
    }
    modem_cookie = slotinfo->modem_cookie 
        = malloc_named(sizeof(modem_carrier_cookie_t) +
			nmodules * sizeof(modem_module_cookie_t),
			"Modem Pool Cookie");
    if (modem_cookie == NULL) {
	crashdump(0);
    }

    bzero(modem_cookie, sizeof(modem_carrier_cookie_t) +
                        nmodules * sizeof(modem_module_cookie_t));
    bcopy(&tmp_carrier, &modem_cookie->carrier, sizeof(modem_carrier_cookie_t));

    /* read modem modules */
    chipsel = X2444_CE_MODEMBD1;
    module_cookie = modem_cookie->module;
    for (jx = 0; jx < nmodules; jx++, chipsel += X2444_CE_CARRIER,
							module_cookie++) {
	modem_x2444_io(reg, chipsel, MODEM_X2444_SK,
					MODEM_X2444_DI, MODEM_X2444_DO,
					X2444_CMD_RCL, X2444_NUM_CMDBITS_OUT, 
					X2444_NO_DATABITS_IN);
	wordptr = (ushort *)module_cookie;
	for (ix = 0; ix < nmodule_words; ix++) {
	    *(wordptr+ix) = modem_x2444_io(reg, chipsel, MODEM_X2444_SK,
				       	MODEM_X2444_DI, MODEM_X2444_DO,
				       	X2444_CMD_READ + (ix * 2), 
					X2444_NUM_CMDBITS_OUT, 
					X2444_NUM_DATABITS_IN);
	}
    }
}

/*
 * modem_pool_init
 *    -	This will size up the modem board and determine how many modems
 *	there are, and where they are on the board.   It then allocates
 *	the modem_infos for them and do some basic initializations.
 *	Then it calls others to do their initializations.
 */
static void modem_pool_init (modem_slot_t *slotinfo, int slot)
{   
    modem_info_t *modem;
    modem_carrier_cookie_t *carrier;
    modem_module_cookie_t *module;
    modem_vendor_defs_t *vendorinfo;
    int total_modules;	/* maximum number of modules supported by carrier */
    int total_ports;	/* maximum number of async ports supported by carrier */
    int avg_ports;	/* number of async lines going to each modem module */
    int num;		/* number of modems on a given modem module */
    int module_num, port, base;
    ushort rten_mask, led_mask;

    carrier = &slotinfo->modem_cookie->carrier;
    
    /*
     * Based on the number of ports and modules supported by this
     * carrier card, determine how many ports go to each module.
     * This assumes that all async ports are equally distributed
     * to each of the modem modules.
     */
    total_ports = carrier->number_of_ports;
    if (total_ports > slotinfo->total_tty_ports) {
	total_ports = slotinfo->total_tty_ports;
    }
    total_modules = carrier->max_modules;
    if (total_ports == 0 || total_modules == 0) {
	return;
    }
    if (total_ports > MAX_MODEM_PER_SLOT) {
	total_ports = MAX_MODEM_PER_SLOT;
    }
    avg_ports = total_ports / total_modules;
    if (total_ports % total_modules) {
	/* not balanced, then just make it so by rounding total down */
	total_ports = total_modules * avg_ports;
    }

    module = slotinfo->modem_cookie->module;
    rten_mask = RTEN_MDB1_ENABLE_20HZ_RING_PULSE;
    led_mask = MDB_CNTL1_MDB1_LED;
    for (module_num = 0, base = 0; base < total_ports; 
         module_num++, base+=avg_ports, module++, rten_mask<<=1, led_mask<<=1) {
	num = module->number_of_modems;
	if (num == 0xFF) {
	    /* if read all one's, then the modem module is */
	    /* absent and we skip a chunk of modem info's */
	    slotinfo->modem_reg->mdb_control1 &= ~led_mask;
	    continue;
	}
        /*
         * Turn on/off the 20Hz ring indication signal for
         * different manufacturers.
         */
	module->vendor_banner[VENDOR_BANNER_CHARLEN] = 0;
	for (vendorinfo = mai_modem_vendor_specific_table;
				*vendorinfo->banner; vendorinfo++) {
	    if (strncmp(vendorinfo->banner, module->vendor_banner,
			strlen(vendorinfo->banner)) == 0) {
		break;
	    }
	}

	if (vendorinfo->ring_tone_enabled) {
	    slotinfo->modem_reg->rten |= rten_mask;
	} else {
	    slotinfo->modem_reg->rten &= ~rten_mask;
	}

	/*
	 * If actual number of modems are less than supported by the carrier
	 * card then there will be holes in the slotinfo->modem_info[].
	 * But if there are more modems than the carrier can support, then
	 * then extra ones are ignored.
	 */
	if (num > avg_ports) {
	    num = avg_ports;
	}
	slotinfo->good_modem_count[module_num] = num;
	for (port = base, num += base; port < num; port++) {
	    slotinfo->modem_info[port] = modem = malloc_named(
				sizeof(modem_info_t),
				"Modem Pool Modem Info");
	    if (modem == NULL) {
		crashdump(0);
	    }
	    bzero(modem, sizeof(modem_info_t));
	    modem->module_num = module_num;
	    modem->cookie = module;
	    modem->vendor_info = vendorinfo;
	    CSM_MODEM_SLOT(modem) = slot;
	    CSM_MODEM_PORT(modem) = port;
	}
	slotinfo->total_modem_num = port;
    }

    mai_modem_pool_init(slotinfo);

    reg_invoke_csm_interface_init(CSM_INIT_INTERFACE, slot, 0, 0);

}

/*
 * modem_pool_line_init
 *    -	This will perform the final line initializations.
 *	The first thing is to do the tty line initializations so that
 *	the tty line is ready and we can start talking to the modems.
 *	At this point we have all the tty lines and all the modems,
 *	now we need to start linking them up.
 *	Then get others to do their line initializations.
 */
static void modem_pool_line_init (void)
{
    modem_slot_t *slotinfo;
    modem_info_t *modem;
    modem_module_cookie_t *module;
    int slot, port, base, ttybase, modembase, oob_ports, modems_per_oob, ix;
    tt_soc *tty = NULL;
    /*
     * Do the async driver line init first
     */
    tty_as5200_cd2430_line_init();

    /* tty base s/b set by cd2430 init code */
    modembase = ttybase = modem_pool->ttybase;

    /*
     * Now change the line defaults to as5200 specific values.
     * i.e. set them to 115200,N,8,1,flowcontrol hardware
     */
    platformdef_asyncspeed[TTY_LINE] = ASYNC_MAXSPEED;
    platformdef_stopbits[TTY_LINE] = STOPBITS1;
    platformdef_capabilities[TTY_LINE] = (HFLOWIN | HFLOWOUT);

    for (base = ttybase, ix = 0; ix < nttylines; ix++, base++) {
        tty = MODEMS[base];
        setspeed(tty, ASYNC_MAXSPEED, ASYNC_MAXSPEED);
        setstopbits(tty, STOPBITS1);
        setdatabits(tty, 8);
        setparity(tty, PARITY_NO);
        tty->def_capabilities = tty->capabilities =
	                      TTY_PLATFORM_DEF_CAPABILITIES(tty); 
        setflow(tty);
    }
   
    modem_pool->tty2modem =  malloc_named(nttylines * sizeof(modem_info_t *),
					"Modem Pool tty2modem");
    if (modem_pool->tty2modem == NULL) {
	crashdump(0);
    }
    bzero(modem_pool->tty2modem, nttylines * sizeof(modem_info_t *));

    base = ttybase;
    for (slot = 0; slot < AS5200_MAX_SLOTS; slot++) {
	slotinfo = modem_pool->modem_slot[slot];
	if (slotinfo == NULL) {
	    continue;
	}

	/*
	 * Set the codec u/A-law bit.
	 */
	/* First find a valid modem module cookie */
	for (ix = 0, module = slotinfo->modem_cookie->module; 
			ix < slotinfo->modem_cookie->carrier.max_modules;
			ix++, module++) {
	    if (module->number_of_modems != 0xFF) {
		break;
	    }
	}
	if (ix < slotinfo->modem_cookie->carrier.max_modules) {
	    if (((modem_pool->encoding == A_LAW_ENCODING) &&
			!(module->option_bits & VENDOR_CODEC_U_LAW_HIGH)) ||
			((modem_pool->encoding != A_LAW_ENCODING) &&
			(module->option_bits & VENDOR_CODEC_U_LAW_HIGH))) {
		slotinfo->modem_reg->mdb_control1 |= MDB_CNTL1_CODEC_AU_LAW;
	    } else {
		slotinfo->modem_reg->mdb_control1 &=~ MDB_CNTL1_CODEC_AU_LAW;
	    }
	}

        /*
	 * link the tty structure into the modem_info structure
	 */
	oob_ports = slotinfo->modem_cookie->carrier.max_oob_ports;
	modems_per_oob = slotinfo->total_modem_num;
	if (oob_ports) {
	    /* get number modems per oob port */
	    if (oob_ports > OOB_PORTS_PER_SLOT) {
		oob_ports = OOB_PORTS_PER_SLOT;
	    }
	    modems_per_oob = slotinfo->modem_cookie->carrier.number_of_ports /
								oob_ports;
	}
	for (port = 0; port < slotinfo->total_modem_num; port++) {
	    modem = slotinfo->modem_info[port];
	    if (modem) {
		modem->modem_tty = MODEMS[base];
		/* setup modem management tty pointer */
		modem->mgmt_tty = slotinfo->mgmt_oob_tty[port/modems_per_oob];
		modembase = base - modem_pool->ttybase;
		if (modembase < nttylines) {
		    (*modem_pool->tty2modem)[modembase] = modem;
		}
                base++;
	    }
	}

 	/* setup ppp mode for oob ports */
	for (ix = 0; ix < oob_ports; ix++) {
	    if (slotinfo->mgmt_oob_tty[ix] != NULL) {
		modem_oob_ppp_startup (slotinfo->mgmt_oob_tty[ix]); 
	    }
	}
    }

    /* additional line initializations */
    mai_line_init();
    /* modem_mgmt_init(); */
    reg_invoke_modem_mgmt_inits();

}

/*
 * modem_analyze_modem_boards
 *    -	This is called to analyze the board in the given slot.
 *	It will create the modem_slot structure, and call others to create
 *	and initialize the relevant tty line and modem data structures.
 */
static void modem_analyze_modem_boards (int slot)
{
    modem_slot_t *slotinfo;
    static boolean first_time=TRUE;

    if (!BRASIL_CARD_PRESENT(slot) ||
   		BRASIL_BOARD_ID(slot) != BRASIL_24_ASYNC_PLUS_MODEMS) {
	return;
    }

    if (first_time) {
        reg_add_line_init(modem_pool_line_init, "modem_pool");
        reg_add_modem_hardware_override(modem_release_override, 
                                        "modem_release_override");
        reg_add_modem_busyout(modem_busyout_tty, "modem_busyout");
        reg_add_modem_busyout_allowed(modem_busyout_present, 
                                        "modem_busyout_present");
        reg_add_modem_async_idb_inits(modem_init_async_idb_info,
					"modem_async_idb_inits");
        reg_add_modem_hardware_in_use(modem_hardware_in_use,
                                      "modem_hardware_in_use");

	/* initialize as5200 modem parser commands */
	modem_parser_init();

        first_time = FALSE;
    }

    slotinfo = modem_pool->modem_slot[slot] = malloc_named(
				sizeof(modem_slot_t), "Modem Pool Slot Info");
    if (slotinfo == NULL) {
	crashdump(0);
    }

    bzero(slotinfo, sizeof(modem_slot_t));
    slotinfo->modem_reg = (modem_reg_t *)GET_SLOT_BASE(slot);
    /* enable the board and light up any modem module LEDs */
    slotinfo->modem_reg->mdb_control1 |= (MDB_CNTL1_MDB_ENABLE |
				MDB_CNTL1_MDB1_LED | MDB_CNTL1_MDB2_LED);

    modem_read_modem_cookies(slotinfo);

    tty_as5200_cd2430_init(slotinfo, slot);

    modem_pool_init(slotinfo, slot);

}

/*
 * modem_ttynum_to_modem_info
 *    -	Map tty line number to modem_info.
 */
modem_info_t *modem_ttynum_to_modem_info (int ttynum)
{
    int modembase;

    if (modem_pool->tty2modem == NULL) {
	return (NULL);
    }
    modembase = ttynum - modem_pool->ttybase;
    if (modembase >= 0 && modembase < nttylines) {
	return ((*modem_pool->tty2modem)[modembase]);
    }
    return (NULL);
}

/*
 * modem_get_channelidb
 * return the hwidb controlling the input side of this tty, if it's connected
 * to an internal modem card.
 */

static boolean
modem_get_channelidb (tt_soc *tty, hwidbtype **hwidb)
{
    modem_info_t *modem;

    if (tty->type != TTY_LINE)
	return(FALSE);

    modem = modem_ttynum_to_modem_info(tty->ttynum);

    if (modem) {
	*hwidb = modem->bchan_idb;
	return(TRUE);
    }
    return(FALSE);
}


/*
 * modem_pool_subsystem_init
 *    -	Modem subsystem initializations.
 */
static void modem_pool_subsystem_init (subsystype *subsys)
{
    modem_pool = malloc_named(sizeof(modem_pool_t), "Modem Pool");
    if (modem_pool == NULL) {
	crashdump(0);
        return;
    }
    bzero(modem_pool, sizeof(modem_pool_t));

    modem_pool->max_slots = AS5200_MAX_SLOTS;

    reg_add_modem_set_codec_type(modem_set_codec_type, "modem_set_codec_type");

    tty_cd2430_subsystem_init();
    reg_add_analyze_interface(BRASIL_NIM_ASYNC_MODEM,
			modem_analyze_modem_boards,
			"modem_analyze_modem_boards");
    reg_add_modem_type_desired(modem_pool_type, "modem_pool_type");
    reg_add_channel_hwidb(modem_get_channelidb, "modem_get_channelidb");
}

SUBSYS_HEADER(modem_pool, MODEM_POOL_MAJVERSION, 
              MODEM_POOL_MINVERSION,
              MODEM_POOL_EDITVERSION, 
              modem_pool_subsystem_init,
	      SUBSYS_CLASS_DRIVER, "seq: modem_mgmt", NULL);
