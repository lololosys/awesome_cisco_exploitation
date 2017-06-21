/* $Id: cas_rbs.c,v 1.7 1996/10/10 23:45:22 jchlin Exp $
 * $Source: /cscka/t1_neat/src/common/cas_rbs.c,v $
 *------------------------------------------------------------------
 * Channelized T1 supporting routines.
 *
 * March 1996, Tony Wu
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cas_rbs.c,v $
 * Revision 1.7  1996/10/10 23:45:22  jchlin
 * o       Set the framer with default value of SF and AMI during the
 *         initialization of framer.
 * o       Enable the robbed bit interrupt mask in framer chip.
 * o       Change the timer timeout process so that it can process
 *         the timeout timer in the iteration of certain number
 *         before it releases CPU control.
 *
 * Revision 1.6  1996/09/10  02:53:06  jchlin
 * Add thw following new features:
 * o       busy dsx0 back to CO trunk when there's no free modem available
 *         in the system
 * o       Support E&M feature group D, FXS loop start and ground start,
 *         and SAS loop start and ground start.
 *
 * Revision 1.5  1996/07/13  05:19:30  towu
 * o removed debugging messages.
 * o Added rbs transmit status report routine.
 *
 * Revision 1.4  1996/06/19  23:03:31  jliu
 *     o There was a sever bug found from devtest. This bug caused framer registers
 *       content changed. The reason was the stack area stepped over code space
 *     o we should use FrameType instead of frame_mode
 *
 * Revision 1.3  1996/05/12  23:50:14  towu
 * Fixed bugs for ct1 rbs features.
 *
 * Revision 1.2  1996/05/09  21:38:49  towu
 * Support Robbed bits signals.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef T1_FIRMWARE
#include "t1_headers.h"
#else
#include "e1_headers.h"
#endif

static void t1_rbs_timer (timer_t *timer, void *para1, void *para2);
static timer_t rbstimer;


/*
 * ct1_enable_handler -
 * Enable/Disable channelized T1 rbs timeslots
 */
  
void ct1_enable_rbs_handler ( void *p)
{
	msg_ct1_channels    *c;
  	volatile ushort     *framer;
 	ushort		    ix;
 	uchar 		    *chan_sig;
 	uchar	            sig_type;
  	uint chan_mask = 0x80000000;
  
  	c = p;
  	framer = Interface[c->unit].Fr.framer;
  
 	for ( ix = 0, chan_sig = c->sig_type; ix < NUM_T1_CHANNELS; ix++) {
 	    /* grep the signal type first */
 	    if (ix & 1) {
 	        sig_type = (*chan_sig & 0x0F);
                 chan_sig++; 	/* bump the pointer */
             }
 	    else
 	        sig_type = ((*chan_sig >> 4) & 0x0F);
 
 	    if (!(Interface[c->unit].ct1_map & chan_mask)) {
 	    /* if the timeslot is not enable yet */
 	        if (sig_type) {
 	            bt8360_wr_fram(framer,T1_RX_PCM_CONTROL + ix, 
 		          	T1_RX_PCM_SIG_ACT);
 		    /* stop tranmit rbs from control register */
                     t1_rbs_transmit(c->unit, ix, 0);
 	            Interface[c->unit].ct1_map |= chan_mask;
  		    rbs_enable_signal_channel(c->unit, ix, sig_type);
 	        }
 	    }
 	    else {
 	        /* The timeslot already enabled */
 	        if (!sig_type) {
                    /* disable receiving rbs */
                    bt8360_wr_fram(framer,T1_RX_PCM_CONTROL + ix, 0);
                    rbs_disable_signal_channel(c->unit, ix);
                    /* stop transfer rbs from local insert state */
                    bt8360_wr_fram(framer,T1_TX_PCM_CONTROL + ix, 0);
                    Interface[c->unit].ct1_map &= ~chan_mask;
  		} else
                    rbs_enable_signal_channel(c->unit, ix, sig_type);
 	    }
 	    chan_mask >>= 1;
  	}
 	/* Force do the rbs report */
 	Interface[c->unit].rx_rbs_report = TRUE;
  	Interface[c->unit].rx_rbs_changed = FALSE;
}

/*
 *  t1_rx_rbs_report -
 *	returns all the receiving timeslots rbs status to mou if needed.
 */
static void t1_rx_rbs_report (ushort unit)
{
    volatile ushort     *framer;
    ushort	ix;
    uint shift, chan_mask = 0x80000000;
    ulong ct1_map;
    msg_show_rbs_resp rbs;
    uchar	signals, *s;

    framer = Interface[unit].Fr.framer;
    ct1_map = Interface[unit].ct1_map;
	    
    bzero(&rbs, sizeof(msg_show_rbs_resp));

    if (Interface[unit].Fr.FrameType == DSX1_ESF) {
	    /* ESF mode */
	   for ( ix = 1, s = rbs.signals; ix <= NUM_T1_CHANNELS; ix++) {
		    if (ct1_map & chan_mask) {
			    signals = bt8360_rd_fram(framer,T1_RX_SIG_BUFFER + ix);
			    signals &= T1_RX_SIG_NEW_MASK;
		    }
		    else {
			    signals = 0;
		    }
		    if (( ix & 1) == 0)
			    signals >>= 4;
		    s[(ix-1)>>1] |= signals;

		    chan_mask >>= 1;
	    }
			    
    }
    else {  /* SF mode */
	    for ( ix = 1, s = rbs.signals; ix <= NUM_T1_CHANNELS; ix++) {
		    if (ct1_map & chan_mask) {
			    signals = bt8360_rd_fram(framer,T1_RX_SIG_BUFFER + ix);
			    signals &= T1_RX_SIG_NEW_SF_MASK;
		    }
		    else {
			    signals = 0;
		    }
		    shift = (ix-1)&3;
		    if (shift == 1)
			    signals >>= 2;
		    else if (shift == 2)
			    signals >>= 4;
		    else if (shift == 3)
			    signals >>= 6;

		    s[(ix-1)>>2] |= signals;

		    chan_mask >>= 1;
	    }
    }
    rbs.unit = unit;
    rbs.chan_map = ct1_map;
    neat_ipc_message_send(MSG_NEAT_RX_RBS_DATA, (uchar *)&rbs, 
				sizeof(msg_show_rbs_resp));
    Interface[unit].rx_rbs_report = FALSE;
}

/*
 *  t1_tx_rbs_report -
 *	returns all the transmiting timeslots rbs status to mou if needed.
 */
static void t1_tx_rbs_report (ushort unit)
{
    volatile ushort     *framer;
    ushort	ix;
    uint shift, chan_mask = 0x80000000;
    ulong ct1_map;
    msg_show_rbs_resp rbs;
    uchar	signals, *s;

    framer = Interface[unit].Fr.framer;
    ct1_map = Interface[unit].ct1_map;
	    
    bzero(&rbs, sizeof(msg_show_rbs_resp));

    if (Interface[unit].Fr.FrameType == DSX1_ESF) {
	    /* ESF mode */
	   for ( ix = 1, s = rbs.signals; ix <= NUM_T1_CHANNELS; ix++) {
		    if (ct1_map & chan_mask) {
			signals = bt8360_rd_fram(framer,T1_TX_SIG_BUFFER + ix);
		 	signals &= T1_TX_SIG_MASK;

		    }
		    else {
			    signals = 0;
		    }
		    if  (ix & 1) 
			    signals <<= 4;
		    s[(ix-1)>>1] |= signals;

		    chan_mask >>= 1;
	    }
			    
    }
    else {  /* SF mode */
	    for ( ix = 1, s = rbs.signals; ix <= NUM_T1_CHANNELS; ix++) {
		    if (ct1_map & chan_mask) {
			signals = bt8360_rd_fram(framer,T1_TX_SIG_BUFFER + ix);
			signals &= T1_TX_SIG_SF_MASK;
		    }
		    else {
			    signals = 0;
		    }
		    shift = (ix-1)&3;

		    if (shift == 0)
			    signals <<= 4;
		    else if (shift == 1)
			    signals <<= 2;
		    else if (shift == 3)
			    signals >>= 2;

		    s[(ix-1)>>2] |= signals;

		    chan_mask >>= 1;
	    }
    }
    rbs.unit = unit;
    rbs.chan_map = ct1_map;
    neat_ipc_message_send(MSG_NEAT_TX_RBS_DATA, (uchar *)&rbs, 
				sizeof(msg_show_rbs_resp));
    Interface[unit].tx_rbs_report = FALSE;
}
/*
 *  t1_rbs_report -
 *   returns all the rbs status to mpu if needed.
 */
void t1_rbs_report ( void )
{
	ushort unit;

	for (unit = 0; unit < MAX_DSX1; unit++) {
	    if (Interface[unit].rx_rbs_report == TRUE)
	    	t1_rx_rbs_report(unit);
	    if (Interface[unit].tx_rbs_report == TRUE)
		t1_tx_rbs_report(unit);
	}
}

/*
 * t1_rbs_int -
 * 	Interrupt handling routine for Robbed bit signnals change state
 */
void t1_rbs_int (T1InfoType *t1)
{
    	volatile ushort *framer;
	uchar	stack, sig_from_stack, sig_from_pcm, chan;

	framer = t1->Fr.framer;

	if (ct1_enable == 0)
		return;
	if (t1->Fr.FrameType == DSX1_ESF) {
		do {
		  stack = bt8360_rd_freg(framer, T1_SIGCHG_REG);
		    /* Read Signaling Change Stack  Register */
		    chan = (stack & T1_SIGCHG_CHAN);
		    /* ignore the noise */
		    if (chan > 24)
			break;
		    sig_from_pcm = bt8360_rd_fram(framer, T1_RX_PCM_CONTROL 
		    + (chan - 1));
		    sig_from_pcm &= T1_RX_PCM_SIG_MASK;
		    rbs_rx_abcd_bit_signal(t1->unit, chan-1, sig_from_pcm);
		  } while (stack & T1_SIGCHG_MORE); 
	}
	else {
		do {
		    /* Read Signaling Change Stack  Register */
		    stack = bt8360_rd_freg(framer, T1_SIGCHG_REG);

		    chan = (stack & T1_SIGCHG_CHAN);
		    if (chan > 24)
			break;
		    sig_from_stack = (stack&T1_SIGCHG_MASK) >> 5;
		    sig_from_pcm = bt8360_rd_fram(framer, 
				T1_RX_PCM_CONTROL + (chan-1));
		    /* The most current  A,B signals is in  C, D in PCM reg */
		    sig_from_pcm = (sig_from_pcm & T1_RX_PCM_SIGCD_MASK);	
		    if (sig_from_stack == sig_from_pcm)
			rbs_rx_ab_bit_signal(t1->unit, chan-1, sig_from_stack <<2);
		  } while (stack & T1_SIGCHG_MORE); 
	}
	t1->rx_rbs_changed = TRUE;
}

/*
 * t1_rbs_transmit -
 *	transmit rbs through control register.
 */
void t1_rbs_transmit (ushort unit, uchar channel, uchar signals)
{
    	volatile ushort *framer;
	uchar	temp;
        
	framer = Interface[unit].Fr.framer;
	if (Interface[unit].Fr.FrameType == DSX1_SF) {
	/* duplicate AB signals to CD bits */
		temp = signals >> 2;
		signals |= temp;
	}
	bt8360_wr_fram(framer, T1_TX_PCM_CONTROL + channel, 
		signals | T1_TX_PCM_SIG_LOCAL | T1_TX_PCM_INSERT_SIG);
	Interface[unit].tx_rbs_changed = TRUE;
}


void t1_rbs_init(void)
{
	int ix;

	for (ix = 0; ix < MAX_DSX1; ix++) {
		Interface[ix].rx_rbs_changed = FALSE;
		Interface[ix].rx_rbs_report = FALSE;
		Interface[ix].tx_rbs_changed = FALSE;
		Interface[ix].tx_rbs_report = FALSE;
	}
	if (ct1_enable == 0)
		return;
	neat_ipc_message_register(MSG_MPU_SET_CT1_CHANNELS, ct1_enable_rbs_handler,
		NEAT_IPC_MSG_PRI_HI);
	
	timer_init(&rbstimer);
        timer_start_one_shot(&rbstimer, 1000, t1_rbs_timer, 0, 0);
	rbs_init_signal_info();
}

/*
 * Deactivate rbs channels
 */
void t1_rbs_reset ( void )
{
	ushort ix, jx;
	msg_ct1_channels c;
		   
		    
	if (ct1_enable == 0)
	    return;
	for (ix = 0; ix < MAX_DSX1; ix++) {
	    c.unit = ix;
	    for (jx = 0; jx < NUM_T1_CHANNELS/2; jx++)
		c.sig_type[jx] =  0;
	    ct1_enable_rbs_handler(&c);
        }
}


static void t1_rbs_timer (timer_t *timer, void *para1, void *para2)
{
	int ix;

	if (ct1_enable) {
		for ( ix = 0; ix < MAX_DSX1; ix++ ) {
			if ( Interface[ix].rx_rbs_changed  == TRUE) {
				Interface[ix].rx_rbs_changed  = FALSE;
				Interface[ix].rx_rbs_report  = TRUE;
			}
			if ( Interface[ix].tx_rbs_changed  == TRUE) {
				Interface[ix].tx_rbs_changed  = FALSE;
				Interface[ix].tx_rbs_report  = TRUE;
			}
		}
        	timer_start_one_shot(timer, 1000, t1_rbs_timer, 0, 0);
	}
}
