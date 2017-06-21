/* $Id: t1framer.c,v 1.18 1996/10/10 23:45:25 jchlin Exp $
 * $Source: /cscka/t1_neat/src/t1/t1framer.c,v $
 *------------------------------------------------------------------
 * t1framer.c  NEAT framer programming routines
 *
 * October, 1995. Joe Liu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: t1framer.c,v $
 * Revision 1.18  1996/10/10 23:45:25  jchlin
 * o       Set the framer with default value of SF and AMI during the
 *         initialization of framer.
 * o       Enable the robbed bit interrupt mask in framer chip.
 * o       Change the timer timeout process so that it can process
 *         the timeout timer in the iteration of certain number
 *         before it releases CPU control.
 *
 * Revision 1.17  1996/07/13  05:36:34  towu
 * o Added rbs framing status.
 *
 * Revision 1.16  1996/07/10  06:44:24  jliu
 *     	o Created another timer(250ms) for framing process and one second
 *           DSX1 performance data statistics in order to pass ACCUNET test
 * 	o Code changed for both AT&T and ANSI fdl test
 *         o Removed frame_mode in T1InfoType by using FrameType in frm_info
 *
 * Revision 1.15  1996/06/07  21:40:32  jliu
 *     o fixed controller was up but loss of signal was detected problem
 *     o added new sub-controller command for esf fdl exchange setting
 *
 * Revision 1.14  1996/05/12  23:50:18  towu
 * Fixed bugs for ct1 rbs features.
 *
 * Revision 1.13  1996/05/09  21:25:55  towu
 * Added new feature Robbed bit signals.
 *
 * Revision 1.12  1996/05/06  06:01:06  jliu
 *     o Cleaned some code for LED on and off
 *     o Set DSX1_XMT_AIS in dsx1LineStatus after sending AIS
 *
 * Revision 1.11  1996/04/29  03:35:08  jliu
 *     o clean up some code
 *     o updated loopback bit of the line status when local loopback
 *       is being performed
 *
 * Revision 1.9  1996/04/14  20:49:52  jliu
 *     o clean up some comments
 *     o made some changes foe phone digits collection
 *
 * Revision 1.8  1996/04/08  18:17:57  jliu
 *     o cleanup some code
 *     o modified code for digits collection
 *
 * Revision 1.7  1996/03/19  03:54:46  jliu
 *    o change the Tx clock source to internal by default
 *    o always use XBCKI for slip/transmit buffer clock which is derived
 *      from TDM bus 2.044 Mhz
 *    o when external network loopback is detected, need to change the transmit
 *      clock to line and change it back to internal when that loopback
 *      request goes away
 *    o need to initialize Rx PCM control registers
 *    o modify some internal registers setup for tone generation
 *
 * Revision 1.6  1996/03/06  00:40:16  jliu
 *     o Put a T1_CFG_REG read right after T1_CFG_REG write. There is a weird
 *       problem on T1_CFG_REG write. A read after write seems to write
 *       correct data to that register
 *
 * Revision 1.5  1996/03/02  03:51:23  towu
 * The basecode to support both mailbox and message queue for ipc.
 *
 * Revision 1.4  1996/03/02  03:01:48  jliu
 *     o need to add some cycles between two consecutive read or write
 *       from/to framer registers
 *
 * Revision 1.3  1996/02/28  18:35:54  jliu
 *     o changed the T1_CTRL_REG setup to select 256-bit mode and frame sync
 *       for transmit and slip buffer
 *     o removed framer_remote_alarm function vector from T1InfoType
 *
 *     o need to clear TRI_RLOOP bit in LIU_control when local loopback is
 *       enabled and re-set when it is disabled
 *
 * Revision 1.2  1996/02/22  22:27:48  jliu
 *     o update thge current T1 firmware for starting channelized T1 coding
 *
 * Revision 1.1.1.1  1996/02/10  03:22:52  towu
 * Create
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include        "t1_headers.h"


void bt8360_framing (T1InfoType *t1, short frametype )
{
    volatile ushort *framer;
    ushort          old, tempread;
    dsx1MIBDataType *e;

    e = t1->Mib;
    framer = t1->Fr.framer;
    old = bt8360_rd_freg(framer, T1_CFG_REG);

    if ( frametype == DSX1_ESF ) {
        /*
         * Enable ESF mode
         */
	t1->Fr.FrameType = DSX1_ESF;	
        old = bt8360_rd_freg(framer,T1_CFG_REG);
        old |= T1_CFG_ESF;
        bt8360_wr_freg(framer,T1_CFG_REG, old);

        for (old = 0; old < 10; old++);
        old = bt8360_rd_freg(framer,T1_FRAMCTRL_REG);
        old |= T1_FRAMCTRL_CRCCHK;
        bt8360_wr_freg(framer,T1_FRAMCTRL_REG, old);

        for (old=0;old < 10; old++);
        old = bt8360_rd_freg(framer,T1_ALARM_REG);
        old &= ~T1_ALARM_SFYELLOW;
        bt8360_wr_freg(framer,T1_ALARM_REG, old);
        tempread = bt8360_rd_freg(framer, T1_ALARM_REG);

        bt8360_wr_freg(framer, T1_TXCTRL_REG, T1_TXCTRL_IDLE);
        tempread = bt8360_rd_freg(framer, T1_TXCTRL_REG);
        if (t1->fdl_std == DSX1_FDL_ATT) {
            bt8360_wr_freg(framer, T1_RXCTRL_REG, 
                           (T1_RXCTRL_STACKINT | T1_RXCTRL_MOPINT));
        } else if (t1->fdl_std == DSX1_FDL_ANSI) {
            bt8360_wr_freg(framer, T1_RXCTRL_REG, 
                           (T1_RXCTRL_STACKINT | T1_RXCTRL_MOPINT | T1_RXCTRL_BOPINT));
        } else {
           bt8360_wr_freg(framer, T1_RXCTRL_REG, T1_RXCTRL_STACKINT);
        }
    } else {
        /*
         * Enable SF mode
         * In SF mode T1_TXCTRL_REG register should be programmed with the
         * Transmit Idle bit high to prevent transmit data link interrupt
         */
	t1->Fr.FrameType = DSX1_SF;
        bt8360_wr_freg(framer,T1_TXCTRL_REG, T1_TXCTRL_IDLE);
        tempread = bt8360_rd_freg(framer,T1_TXCTRL_REG);
        bt8360_wr_freg(framer, T1_RXCTRL_REG, T1_RXCTRL_STACKINT);
        old = bt8360_rd_freg(framer,T1_CFG_REG);
        old &= ~T1_CFG_ESF;
        bt8360_wr_freg(framer, T1_CFG_REG, old);

        for (old=0;old < 10; old++);
        old = bt8360_rd_freg(framer, T1_FRAMCTRL_REG);
        old &= ~T1_FRAMCTRL_CRCCHK;
        bt8360_wr_freg(framer, T1_FRAMCTRL_REG, old);

        /*
         * This will put a timer on the received yellow alarms
         */
        for (old=0; old < 10; old++);
        old = bt8360_rd_freg(framer,T1_ALARM_REG);
        old |= T1_ALARM_SFYELLOW;
        bt8360_wr_freg(framer,T1_ALARM_REG, old);

    }  

    e->Config.dsx1LineType = frametype;

    for (old = 0; old < 10; old++);
    old = bt8360_rd_freg(framer,T1_FRAMCTRL_REG);
    old |= T1_FRAMCTRL_FORCE;
    bt8360_wr_freg(framer,T1_FRAMCTRL_REG, old);

}

/*
 * initialize all FDL reply message buffers by putting zero to in_process
 */
static inline void fdl_rlyQ_init (T1InfoType *t1)
{
        uint         ix;
        frm_info     *frm;

        frm = &t1->Fr;
        frm->rly_free_head = frm->rly_message_head = 0x0;

        for (ix = 0; ix < MAX_RLY_MESSAGE; ix++) {
             frm->DL_Reply[ix].in_process = 0;
             frm->DL_Reply[ix].has_message = 0;
             frm->DL_Reply[ix].which_half = 0;
             frm->DL_Reply[ix].length = 0;
        }
}

/*
 * initialize all FDL request message buffers by putting zero to in_process
 * and has_message
 */
static inline void fdl_reqQ_init (T1InfoType *t1)
{
        uint           ix;
        frm_info       *frm;

        frm = &t1->Fr;
        frm->req_free_head = frm->req_message_head = 0x0;

        for (ix = 0; ix < MAX_REQ_MESSAGE; ix++) {
             frm->DL_Request[ix].in_process = 0;
             frm->DL_Request[ix].has_message = 0;
             frm->DL_Request[ix].length = 0;
        }
}

/*
 *		Initialize Bt8360 framer
 */
void t1_init_framer (T1InfoType *t1)
{
    int            indx;
    frm_info       *frm;
    volatile ushort *framer, tempread;

    frm = &t1->Fr;
    framer = t1->Fr.framer;
    tempread = 0x0;

    /* set a default line long haul with 0db, Local alarm led */
    *t1->LIU_Control &= (~EC4_1);

    bt8360_wr_freg(framer,T1_CFG_REG, T1_CFG_INTTXCLK);
    tempread = bt8360_rd_freg(framer,T1_CFG_REG);
    tempread = (T1_CTRL_BMC_256 | T1_CTRL_INFREQ_1 | T1_CTRL_OUTFREQ_1 | 
               T1_CTRL_TXSYNC | T1_CTRL_SLIPSYNC);
    bt8360_wr_freg(framer,T1_CTRL_REG, tempread);
    bt8360_wr_freg(framer,T1_FRAME_REG, 0);
    bt8360_wr_freg(framer,T1_SIGCTRL_REG, 0);
    bt8360_wr_freg(framer,T1_FRAMCTRL_REG, T1_FRAMCTRL_2OF5);
    bt8360_wr_freg(framer,T1_SLIP_REG, (T1_SLIP_SLPMVIP | T1_SLIP_TXMVIP));
    bt8360_wr_freg(framer,T1_ALARM_REG, T1_ALARM_ZERO);
    /*
     * No interrupts allowed at this moment until we are in signal/frame
     */
    bt8360_wr_freg(framer, T1_ALRMINT_REG, 0);
	
    /*
     * Don't interrupt on any overflow errors
     */
    bt8360_wr_freg(framer,T1_ERRINT_REG, 0);
    
    /* the next two deal with the Facilities Data Link */
    bt8360_wr_freg(framer,T1_TXCTRL_REG, T1_TXCTRL_IDLE); /* send idle */
    bt8360_wr_freg(framer,T1_RXCTRL_REG, 0);

    /* loop codes for remote CSU */
    bt8360_wr_freg(framer, T1_LOOPGD_REG,
		    (T1_LOOPGD_DEACTDET_6 | T1_LOOPGD_ACTDET_5));
    bt8360_wr_freg(framer, T1_LOOPCG_REG, 0);

    /* 5 bit code 10000 */
    bt8360_wr_freg(framer, T1_LOOPCODE_ACT_REG, 0x80);
    /* 6 bit code 100100 */
    bt8360_wr_freg(framer, T1_LOOPCODE_DEACT_REG, 0x90);
    /*
     * the below is to make sure that the transmit frame synch aligns
     * okay.
     */
    for ( indx = 1; indx <= NUM_T1_CHANNELS; indx++ ) {
	bt8360_wr_fram(framer, (T1_RATE_CONTROL + indx),
			     T1_RATE_TS_ACTIVE);
	tempread = bt8360_rd_fram(framer,(T1_RATE_CONTROL + indx));
    }

    /*
     * send idle byte from framer to M32 for unused channels 
     */
    for ( indx = 1; indx < NUM_UNUSED_CHANNELS; indx++ ) {
        bt8360_wr_fram(framer, (T1_RATE_CONTROL + NUM_T1_CHANNELS + indx),
                             T1_RATE_RX_PCM_IDLE);
        tempread = bt8360_rd_fram(framer,(T1_RATE_CONTROL + NUM_T1_CHANNELS + 
                                                                   indx));
    }

    bt8360_wr_fram(framer, T1_RATE_CONTROL, T1_RATE_RX_PCM_IDLE);
    tempread = bt8360_rd_fram(framer,T1_RATE_CONTROL);

    /*
     * Also init the TX PCM control registers - for signalling
     */
    for ( indx = 0; indx < NUM_T1_CHANNELS; indx++ ) {
	bt8360_wr_fram(framer,T1_TX_PCM_CONTROL + indx, 0x0);
    }

    for ( indx = 0; indx < NUM_T1_CHANNELS; indx++ ) {
	bt8360_wr_fram(framer,T1_RX_PCM_CONTROL + indx, 0x0);
    }
    /*
     * Dummy read of all counters - should get out initial shock
     */
    tempread = bt8360_rd_freg(framer,T1_BPV_LSB);
    tempread = bt8360_rd_freg(framer,T1_BPV_MSB);
    tempread = bt8360_rd_freg(framer,T1_FBE_CTR);
    tempread = bt8360_rd_freg(framer,T1_CRC_CTR);
    tempread = bt8360_rd_freg(framer,T1_COFA_CTR);
    
    fdl_rlyQ_init(t1);
    fdl_reqQ_init(t1);

    /*
     * Make sure the initial default setting is SF/AMI...
     * Please don't remove the following two function calls
     * setting the default configuration of T1 line
     */
    bt8360_framing(t1, DSX1_SF);
    bt8360_linecode(t1, DSX1_AMI);
}

/*
 * The CheckAvailableAndSend() routine is called when the carrier state 
 * change comes that may enable the receiver again.  It also send level 6
 * interrupt to core CPU to notify the carrier state changed.
 */
void CheckAvailableAndSend (T1InfoType *t1)
{
    carrier_mail_type *carrier;

    carrier = t1->Carrier;
    if (( t1->Fr.UnavailableState == FALSE )
        && ( t1->Fr.RxingRemAlarm == FALSE )) {
        t1->Available = TRUE;
        carrier_state_to_MPU(t1, CARRIER_UP, carrier);
        rbs_t1_framing_up(t1->unit);
        }

}

/*
 * SendRemoteAlarm
 * set/clear the remote alarm, and all the flags referring to it
 */
void SendRemoteAlarm (T1InfoType *t1, bool Flag)
{
    dsx1MIBDataType *mib;

    mib = t1->Mib;

    if (Flag == FALSE) {
        if (t1->Fr.TxingRemAlarm) {
            /* Turn off Remote alarm */
            ClearMIBLineStatus(&mib->Config.dsx1LineStatus, DSX1_XMT_REM_ALM);
            t1->Fr.TxingRemAlarm = FALSE;
            bt8360_tx_remotealarm(t1, Flag);
        }
    } else {
        if (!(t1->Fr.TxingRemAlarm)) {
            /* Turn on remote alarm */
            SetMIBLineStatus(&mib->Config.dsx1LineStatus, DSX1_XMT_REM_ALM);
            t1->Fr.TxingRemAlarm = TRUE;
            bt8360_tx_remotealarm(t1, Flag);
        }
    }
}

/*
 * DeclareFrameLoss
 * Indicates to the world that we've lost frame.  Will send
 * a message to the MPU, sends the remote alarm, and set/clear
 * all appropriate bits.
 */
void DeclareFrameLoss (T1InfoType *t1)
{
    carrier_mail_type *carrier;

    carrier = t1->Carrier;
    if ( t1->Fr.FrameAcquired ) {
        t1->Fr.FrameLossOccurred = TRUE;
    }
    /*
     * Note: with channels defined, putting the send remote-alarm
     * here might screw up the time for the T1 to come back
     * in 500 usec.
     */
    SendRemoteAlarm( t1, TRUE );
    if ( t1->Fr.HasFrame ) {
        t1->Fr.HasFrame = FALSE;

        CLEAR_COUNTER(t1->Fr.CountsUntilFrame);
        CLEAR_COUNTER(t1->Fr.CountsUntilLossClear);
        if (t1->Fr.RxingRemAlarm) {
            ClearRxingRemoteAlarm(t1);
        }
        if (!t1->unit) { 
            NEAT_LED &= ~REM_ALARM_0;
            NEAT_LED |= LOCAL_ALARM_0;
        } else {
            NEAT_LED &= ~REM_ALARM_1;
            NEAT_LED |= LOCAL_ALARM_1;
        }

        SetMIBLineStatus( &t1->Mib->Config.dsx1LineStatus,
                         DSX1_FRAME_LOSS );

        if ( t1->Available ) {
            t1->Available = FALSE;
            carrier_state_to_MPU(t1, CARRIER_DOWN, carrier);
            rbs_t1_framing_down(t1->unit);
        }
    }
}

void bt8360_linecode (T1InfoType *t1, short codetype)
{
    ushort old;
    dsx1MIBDataType *e;

    e = t1->Mib;
    if ( codetype != DSX1_B8ZS && codetype != DSX1_AMI )
         return;

    old = bt8360_rd_freg(t1->Fr.framer, T1_CFG_REG);
    old &= ~(T1_CFG_ZBTSI | T1_CFG_B8ZS_TX | T1_CFG_B8ZS_RX );
    
    if ( codetype == DSX1_B8ZS ) {
	old |= (T1_CFG_B8ZS_TX | T1_CFG_B8ZS_RX);
    } 
    
    bt8360_wr_freg(t1->Fr.framer, T1_CFG_REG, old);
    old = bt8360_rd_freg(t1->Fr.framer, T1_CFG_REG);
    e->Config.dsx1LineCoding = codetype;

}

void bt8360_clock_source (T1InfoType *t1, short clocktype)
{
    t1->clk_source = clocktype;
}

void bt8360_tx_remotealarm( T1InfoType *t1, bool Flag )
{
    ushort old;

    old = bt8360_rd_freg(t1->Fr.framer,T1_ALARM_REG);

    if ( Flag == FALSE ) {
	old &= ~T1_ALARM_TXYELLOW;
	bt8360_wr_freg(t1->Fr.framer,T1_ALARM_REG, old);
    } else {
	old |= T1_ALARM_TXYELLOW;
	bt8360_wr_freg(t1->Fr.framer,T1_ALARM_REG,old);
    }
}
		

void bt8360_tx_ais ( T1InfoType *t1, bool Set ) 
{
    ushort read;
    dsx1MIBDataType *mib;

    mib = t1->Mib;
    if ( Set ) {
	if ( t1->Fr.TxingAis == FALSE ) {
	    t1->Fr.TxingAis = TRUE;
	    read = bt8360_rd_freg(t1->Fr.framer,T1_ALARM_REG);
            read |= T1_ALARM_TXAIS;
	    bt8360_wr_freg(t1->Fr.framer,T1_ALARM_REG, read);
            SetMIBLineStatus(&mib->Config.dsx1LineStatus, DSX1_XMT_AIS);
	}
    } else {
	if ( t1->Fr.TxingAis ) {
	    t1->Fr.TxingAis = FALSE;
	    read = bt8360_rd_freg(t1->Fr.framer,T1_ALARM_REG);
	    read = read & ~T1_ALARM_TXAIS;
	    T1_FRAMER_WAIT;
	    bt8360_wr_freg(t1->Fr.framer,T1_ALARM_REG, read);
            ClearMIBLineStatus(&mib->Config.dsx1LineStatus, DSX1_XMT_AIS);
	}
    }
}
	
void bt8360_local_up (T1InfoType *t1)
{
    ushort tempread, delay, old;
    /*
     * For local loopback, do a dual loopback at LIU by setting both
     * RLOOP & LLOOP to one. This will just give a basic loopback.
     */
    tempread = *t1->LIU_Control;
    tempread |= (RLOOP | LLOOP);
    tempread &= ~TRI_RLOOP;
    *t1->LIU_Control = tempread; 

    for (delay = 0; delay < 10; delay++);
    old = bt8360_rd_freg(t1->Fr.framer,T1_FRAMCTRL_REG);
    old |= T1_FRAMCTRL_FORCE;
    bt8360_wr_freg(t1->Fr.framer,T1_FRAMCTRL_REG, old);

    if (t1->unit) {
        NEAT_LED |= LOOPBACK_1;
    } else {
        NEAT_LED |= LOOPBACK_0;
    }
    SetMIBLineStatus(&t1->Mib->Config.dsx1LineStatus, DSX1_LOOPED);
    t1->Mib->Config.dsx1LoopbackConfig = DSX1_OTHER_LOOP;
}

void bt8360_no_loop (T1InfoType *t1)
{
    ushort tempread;

    tempread = *t1->LIU_Control;
    tempread &= (~RLOOP & ~LLOOP);
    tempread |= TRI_RLOOP;
    *t1->LIU_Control = tempread;

    if (t1->unit) {
        NEAT_LED &= (~LOOPBACK_1);
    } else {
        NEAT_LED &= (~LOOPBACK_0);
    }
    ClearMIBLineStatus(&t1->Mib->Config.dsx1LineStatus, DSX1_LOOPED);
    t1->Mib->Config.dsx1LoopbackConfig = DSX1_NO_LOOP;
}

/*
 * And now, a short dissertation on remote loopback.  Basically
 * what's supposed to happen is that we send the remote loopback
 * code to the remote CSU.  It will loop up (transmitting the code
 * back to us), and then we will detect the code coming back.
 * We also have a process counting up to 7 seconds, which will send
 * an error message to the core CPU if we don't get the code back.
 * For some reason, the framer chip goes out of frame and
 * does not detect the code or frame until the pattern stops.  So,
 * I had to add code that when we get a frame loss, we see if we
 * are sending remote loopback.  If we are, we will stop sending
 * the pattern until frame is detected again.  Then we begin
 * sending the remote loopback pattern again.  This time, the
 * framer chip will detect the pattern, and we will have sucess.
 *
 * Basically, this is like sending the pattern twice.
 */
void bt8360_loopup_code (T1InfoType *t1)
{
    ushort read;
    /*
     * enable the interrupt for detecting loop up codes
     */
    read = bt8360_rd_freg(t1->Fr.framer,T1_LOOPCODE_ACT_REG);
    read |= T1_LOOPCODE_ACT_INT;
    bt8360_wr_freg(t1->Fr.framer, T1_LOOPCODE_ACT_REG, read);
    /*
     * Now send the loopback code
     */
    bt8360_wr_freg(t1->Fr.framer,T1_LOOPCG_REG, 0x80);
    read = bt8360_rd_freg(t1->Fr.framer,T1_LOOPGD_REG);
    read &= ~T1_LOOPGD_CODELEN;
    read |= (T1_LOOPGD_CODELEN_5 | T1_LOOPGD_CODEGEN);
    bt8360_wr_freg(t1->Fr.framer, T1_LOOPGD_REG, read);
    /*
     * Indicate that we are searching for a loop-up code
     */
    t1->RemoteLoopState = REM_LOOP_ON_SRCH_CODE;
    t1->RemoteLoopCount = 0;
    t1->Mib->Config.dsx1LoopbackConfig = DSX1_OTHER_LOOP;
}

/*
 * Here's how the remote loopback down command works.  We send
 * the remote CSU loopback down command.  We detect that the
 * pattern comes back.  Then we verify that the pattern goes
 * away after a while.  For some reason, we don't have the same
 * problem that we have with the remote loopback up pattern.
 */
void bt8360_loopdown_code (T1InfoType *t1)
{
    ushort read;
    /*
     * enable the interrupt for detecting loop down codes
     */
    read = bt8360_rd_freg(t1->Fr.framer, T1_LOOPCODE_DEACT_REG);
    read |= T1_LOOPCODE_DEACT_INT;
    bt8360_wr_freg(t1->Fr.framer, T1_LOOPCODE_DEACT_REG, read);
    /*
     * Now send the loop down code
     */
    bt8360_wr_freg(t1->Fr.framer, T1_LOOPCG_REG, 0x90);
    read = bt8360_rd_freg(t1->Fr.framer, T1_LOOPGD_REG);
    read &= ~T1_LOOPGD_CODELEN;
    read |= (T1_LOOPGD_CODELEN_6 | T1_LOOPGD_CODEGEN);
    bt8360_wr_freg(t1->Fr.framer, T1_LOOPGD_REG, read);
    /*
     * Indicate that we are searching for a loop-down code
     */
    t1->RemoteLoopState = REM_LOOP_OFF_SRCH_CODE;
    t1->RemoteLoopCount = 0;
    
}

void bt8360_stoploop_code (T1InfoType *t1)
{
    ushort read;
    
    /* Stop sending the code */
    read = bt8360_rd_freg(t1->Fr.framer, T1_LOOPGD_REG);
    read &= ~T1_LOOPGD_CODEGEN;
    bt8360_wr_freg(t1->Fr.framer, T1_LOOPGD_REG, read);
    
    /* Stop interrupting on both the loop-up and loop-down detections */
    read = bt8360_rd_freg(t1->Fr.framer,T1_LOOPCODE_DEACT_REG);
    read &= ~T1_LOOPCODE_DEACT_INT;
    bt8360_wr_freg(t1->Fr.framer, T1_LOOPCODE_DEACT_REG, read);
    
    read = bt8360_rd_freg(t1->Fr.framer,T1_LOOPCODE_ACT_REG);
    read &= ~T1_LOOPCODE_ACT_INT;
    bt8360_wr_freg(t1->Fr.framer,T1_LOOPCODE_ACT_REG,read);

}

