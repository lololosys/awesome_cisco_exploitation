/* $Id: interrupt.c,v 1.16.6.1 1997/02/21 07:41:03 jliu Exp $
 * $Source: /cscka/t1_neat/src/t1/interrupt.c,v $
 *------------------------------------------------------------------
 * interrupt.c  all NEAT interrupt service routines
 *
 * October, 1995. Joe Liu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: interrupt.c,v $
 * Revision 1.16.6.1  1997/02/21 07:41:03  jliu
 *    o Resolved CSCdi85835 bug and fix a problem in make.rule file
 *    o Resolved CSCdi85835 bug and fix a problem in make.rule file
 *
 * Revision 1.16  1996/09/25 18:25:24  jliu
 *   o Neat bus error could occurred due to level 6 interrupt pending
 *     before neat memory got cleared. So main CPU could read undeterminated
 *     data from Neat memory since system code running before firmware
 *     download
 *
 * Revision 1.15  1996/07/13  05:22:03  towu
 * o code cleanup.
 *
 * Revision 1.14  1996/07/10  06:44:20  jliu
 *     	o Created another timer(250ms) for framing process and one second
 *           DSX1 performance data statistics in order to pass ACCUNET test
 * 	o Code changed for both AT&T and ANSI fdl test
 *         o Removed frame_mode in T1InfoType by using FrameType in frm_info
 *
 * Revision 1.13  1996/06/07  21:40:22  jliu
 *     o fixed controller was up but loss of signal was detected problem
 *     o added new sub-controller command for esf fdl exchange setting
 *
 * Revision 1.12  1996/05/12  23:56:23  towu
 * Fixed bugs for ct1 rbs features.
 *
 * Revision 1.11  1996/05/12  22:57:03  jliu
 *     o Added bus error and illegal intruction handling routine
 *
 * Revision 1.10  1996/05/09  21:25:50  towu
 * Added new feature Robbed bit signals.
 *
 * Revision 1.9  1996/04/29  03:35:05  jliu
 *     o clean up some code
 *     o updated loopback bit of the line status when local loopback
 *       is being performed
 *
 * Revision 1.8  1996/04/20  02:37:37  jliu
 *     o Changed timer inetrrupt resolution from 50ms to 1 ms
 *
 * Revision 1.7  1996/04/14  20:49:49  jliu
 *     o clean up some comments
 *     o made some changes foe phone digits collection
 *
 * Revision 1.6  1996/04/08  18:14:51  jliu
 *     o Made some configuration changes to get tone data generation
 *       works
 *     o Implemented transmit underrun error interrupt
 *
 * Revision 1.5  1996/03/19  03:54:39  jliu
 *    o change the Tx clock source to internal by default
 *    o always use XBCKI for slip/transmit buffer clock which is derived
 *      from TDM bus 2.044 Mhz
 *    o when external network loopback is detected, need to change the transmit
 *      clock to line and change it back to internal when that loopback
 *      request goes away
 *    o need to initialize Rx PCM control registers
 *    o modify some internal registers setup for tone generation
 *
 * Revision 1.4  1996/03/02  03:51:19  towu
 * The basecode to support both mailbox and message queue for ipc.
 *
 * Revision 1.3  1996/02/28  18:29:01  jliu
 *     o removed interrupt level mask set in status register
 *
 * Revision 1.2  1996/02/22  22:27:45  jliu
 *     o update thge current T1 firmware for starting channelized T1 coding
 *
 * Revision 1.1.1.1  1996/02/10  03:22:53  towu
 * Create
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include	"t1_headers.h"

#define RBS_REPORT_PERIOD	20*50

static inline void next_reply_message (FDLReplyType *reply, frm_info *frm)
{
      reply->has_message = 0x0;
      reply->in_process = 0x0;
      reply->length = 0x0;
      frm->rly_message_head += 1;
      if (frm->rly_message_head >= MAX_RLY_MESSAGE)
          frm->rly_message_head = 0x0;
}

void fdl_int (void)
{
    register int        ix;
    register T1InfoType *t1;
    volatile ushort     *framer;
    ushort              src, cause;
    frm_info            *frm;

    /*
     * Here we handle FDL interrupt from two T1 interface. The interrupts 
     * could be transmit or receive or both.
     */
    for(ix = 0; ix < MAX_T1 ; ix++ ) {
        t1 = &Interface[ix];
        frm = &t1->Fr;
        framer = t1->Fr.framer;

        src = bt8360_rd_freg(framer, T1_INTSRC_REG);

        if(src & T1_INTSRC_RXDATA) {
            /*
             * Receive data link interrupt occurred, read receive data link 
             * status register to clear the associated interrupt.
             */
            cause = bt8360_rd_freg(framer, T1_RXSTATUS_REG);
            switch (cause & T1_RXSTATUS_MASK) {
            case T1_RXSTATUS_ABORT_IDLE:
                break;
            case T1_RXSTATUS_BADFCS:
                break;
            case T1_RXSTATUS_MOP_IDLE:
                /* Done with BOP - nothing for now */
                break;
            case T1_RXSTATUS_MOP_BOP:

                if ((cause & T1_RXSTATUS_MSGINDEX) == 0) {
                    /* have BOP */
                    ansi_fdl_info   *ansi;
                    uint    from;
                    uchar   bop_req;

                    /*
                     * If ANSI T1.403 was not selected the ignore this int
                     */
                    if (t1->fdl_std != DSX1_FDL_ANSI)
                        break;
                    ansi = &t1->Ansi;
                    from = (cause & T1_RXSTATUS_MSGSTART) >> 4;
                    from += T1_RX_DL_BUFFER;
                    bop_req = bt8360_rd_fram(framer, from);
                    switch (bop_req) {
                    case LINELOOP_ACT:
                         if (t1->ansi_lineloop == FALSE) {
                             t1->ansi_lineloop = TRUE;
                             netloop_pro(t1, TRUE);
                         }
                         break;
                    case LINELOOP_DEACT:
                         if (t1->ansi_lineloop == TRUE) {
                             t1->ansi_lineloop = FALSE;
                             netloop_pro(t1, FALSE);
                         }
                         break;
                    case PAYLOOP_ACT:
                         ansi->message1_T |= LB;
                         SetMIBLineStatus(&t1->Mib->Config.dsx1LineStatus, 
                                                                DSX1_LOOPED);
                         t1->Mib->Config.dsx1LoopbackConfig = DSX1_PAYLOAD_LOOP;
                         payloadloop_pro(t1, TRUE);
                         t1->LoopbackAct |= PAYLD_LP_NET;
                         t1->MsgToSendToMPU |= NEAT_MSG_LPBK;
                         if (t1->unit) {
                             NEAT_LED |= LOOPBACK_1;
                         } else {
                             NEAT_LED |= LOOPBACK_0;
                         }
                         break;
                    case PAYLOOP_DEACT:
                         ansi->message1_T &= ~LB;
                         ClearMIBLineStatus(&t1->Mib->Config.dsx1LineStatus, 
                                                                 DSX1_LOOPED);
                         t1->Mib->Config.dsx1LoopbackConfig = DSX1_NO_LOOP;
                         payloadloop_pro(t1, FALSE);
                         t1->LoopbackAct |= NO_PAYLD_LP_NET;
                         t1->MsgToSendToMPU |= NEAT_MSG_LPBK;
                         if (t1->unit) {
                             NEAT_LED &= (~LOOPBACK_1);
                         } else {
                             NEAT_LED &= (~LOOPBACK_0);
                         }
                         break;
                    default:
                         break;
                    }
                }
                /* fall through */
            case T1_RXSTATUS_ENDMOP:
                {
                    ushort         bufcnt, ix;
                    uchar          *to;
                    uint           from, index;
                    FDLRequestType *req;

                    /* Copy from buffer to a req buffer */
                    index = frm->req_free_head;
                    req = &frm->DL_Request[index];
                    if (req->in_process || req->has_message) {
                        /* this could be free pointer overrun message pointer */
                        break;
                    } else {
                        index += 1;
                        if (index >= MAX_REQ_MESSAGE)
                            index = 0;
                        frm->req_free_head = index;
                    }
                    /* the start address to store incoming fdl request message */
                    to = &req->address;
                    bufcnt = ((cause & T1_RXSTATUS_MSGINDEX) >> 4);
                    from = (cause & T1_RXSTATUS_MSGSTART) >> 4;
                    from += T1_RX_DL_BUFFER;
                    for (ix = 0; ix <= bufcnt; ix++) {
                        *to++ =(uchar)bt8360_rd_fram(framer, from);
                        from++;
                        req->length += 0x01;
                    }
                    req->has_message = 0x01;
                }
                break;
            default:
                break;
            }

        }

        if(src & T1_INTSRC_TXDATA) {
            FDLReplyType *reply;
            uchar        temp = 0;
            uint         index, ix;

            frm = &t1->Fr;
            reply = &frm->DL_Reply[frm->rly_message_head];

            if (t1->fdl_std == DSX1_FDL_ANSI) {
                temp =  bt8360_rd_freg(frm->framer, T1_TXCTRL_REG);
                temp &= T1_TXCTRL_MSGPTR;
                temp |= (T1_TXCTRL_MOP | T1_TXCTRL_IDLE);
                bt8360_wr_freg(frm->framer, T1_TXCTRL_REG, temp);
                /*
                 * If fdl_resend_RAI is true then we need to resend RAI
                 * after one second ANSI PRM was sent
                 */
                if (t1->fdl_resend_RAI == TRUE) {
                    t1->fdl_resend_RAI = FALSE;
                    t1->RAI_resend = TRUE;
                }
                continue;
            }

            if (reply->length == 0) {
                temp =  bt8360_rd_freg(frm->framer, T1_TXCTRL_REG);
                temp &= T1_TXCTRL_MSGPTR;
                temp |= (T1_TXCTRL_MOP | T1_TXCTRL_IDLE);
                bt8360_wr_freg(frm->framer, T1_TXCTRL_REG, temp);
                next_reply_message(reply, frm);
                /*
                 * If fdl_resend_RAI is true then we need to resend RAI
                 * after AT&T reply message was sent
                 */
                if (t1->fdl_resend_RAI == TRUE) {
                    t1->fdl_resend_RAI = FALSE;
                    t1->RAI_resend = TRUE;
                }
                continue;
            }

            if (reply->length <= 8) {
                if (reply->which_half == 0)
                    index = 0;
                else
                    index = 8;

                for (ix = 0; ix < reply->length; ix++) {
                     bt8360_wr_fram(frm->framer, (T1_TX_DL_BUFFER + index), 
                                                   *reply->data_start++);
                     index++;
                }
                temp |= reply->which_half | ((reply->length - 1) << 4) |
                               (T1_TXCTRL_MOP | T1_TXCTRL_IDLE);
                bt8360_wr_freg(frm->framer, T1_TXCTRL_REG, temp);
                next_reply_message(reply, frm);
                /*
                 * If fdl_resend_RAI is true then we need to resend RAI
                 * after AT&T reply message was sent
                 */
                if (t1->fdl_resend_RAI == TRUE) {
                    t1->fdl_resend_RAI = FALSE;
                    t1->RAI_resend = TRUE;
                }
            } else {
                if (reply->which_half == 0) {
                    reply->which_half = 0x80;
                    index = 0;
                } else {
                    reply->which_half = 0;
                    index = 8;
                }
                for (ix = 0; ix < 0x8; ix++) {
                    bt8360_wr_fram(frm->framer, (T1_TX_DL_BUFFER + index), 
                                                     *reply->data_start++);
                    index++;
                }
                reply->length -= 0x8;
                temp = reply->which_half | ((0x08 - 1) << 4) | T1_TXCTRL_MOP;
                if (reply->length <= 8)
                    temp |= T1_TXCTRL_FCS;
                bt8360_wr_freg(frm->framer, T1_TXCTRL_REG, temp);
            }
        }
    }
}

/*
 * backplane_int - backplane interrupt routine for system reload notice
 * Use level 5 interrupt to NEAT processor.
 *
 */
void backplane_int (void)
{
        MPU_reload = TRUE;
	neat_ipc_msg_mgmt_init();
	t1_rbs_reset();
}

/*
 * transmitter underrun error
 * transmit underrun error occurs on SCC1 DMA channel. We used to have 
 * this error in debug phase. It caused by setting L1RCKLA(7-358) on
 * port A to output and have a contention with hardware setup which
 * has 2.0544Mhz clock input to this pin.
 */

void txe_int (void)
{
       ushort temp;

       QuiccInternalRegsType *qir_p = (QuiccInternalRegsType *)REGB_ADDR;

       temp = qir_p->scc[0].scc_scce;
            
       qir_p->scc[0].scc_scce = 0xffff;
}

/*
 * timer1_int - 250ms timer interrupt routine for digit collection timeout,
 * busy tone data on and off and frame process & one second update. 
 * Use level 4 interrupt to NEAT processor.
 * The way to get the address of the timer 1 interrupt handler is here:
 * VBA2-0 = 5 in CICR and five LSBs for timer 1 interrupt is 0x19. So the
 * 8-bit vector is 10111001b. multiply by 4 to get the offset address of the
 * vector in vectore table. Thus, the offset address is 1011100100b = 0x2E4.
 * So the "_timer1_int" should be put at 185 vectore table in c68k.S file
 */
void timer1_int (void)
{
        ushort          temp;
        T1InfoType      *t1;
        int             ix;

        QuiccInternalRegsType *qir_p = (QuiccInternalRegsType *)REGB_ADDR;

        /*
         * Reset & stop timer1 and mask interrupt in Timer Global Config reg
         */
        qir_p->cpic_cimr &= ~QUICC_CIMR_TIMER1;
        qir_p->tmr_tgcr &= ~QUICC_TGCR_TMR1_EN;

        /*
         * Clear reference event in TER1 and timer1 in-service interrupt
         */
        temp = qir_p->tmr_ter1;
        qir_p->tmr_ter1 = temp;

        /*
         * The timer interrupt occurs every 1ms, so FRAME_PROCESS x 1ms
         * should be equal to 250ms for frame process.
         * Set frame_pro_need TRUE in T1InfoType if 250ms expired, so
         * the T1_Framing_Process function will be called in the main loop.
         */
        for (ix = 0; ix < MAX_T1; ix++) {
             t1 = &Interface[ix];
             t1->frame_pro_need = TRUE;
        }

        /*
         * Must clear timer1 bit after servicing this timer interrupt is complete
         */
        qir_p->cpic_cipr = QUICC_CIMR_TIMER1;
        qir_p->cpic_cisr = QUICC_CIMR_TIMER1;
        /*
         * Start timer1 and enable interrupt in Timer Global Config register
         */
        qir_p->tmr_tmr1 = TMR1_PRESCALE | TMR1_ICLK_DIV1 | TMR_INT_EN;
        qir_p->tmr_trr1 = TMR1_TIME_250MS;
        qir_p->tmr_tgcr |= QUICC_TGCR_TMR1_EN;
        qir_p->cpic_cimr |= QUICC_CIMR_TIMER1;

}

/*
 * timer2_int - 1ms timer interrupt routine for digit collection timeout,
 * busy tone data on and off and frame process & one second update.
 * Use level 4 interrupt to NEAT processor.
 * The way to get the address of the timer 2 interrupt handler is here:
 * VBA2-0 = 5 in CICR and five LSBs for timer 2 interrupt is 0x12. So the
 * 8-bit vector is 10110010b. multiply by 4 to get the offset address of the
 * vector in vectore table. Thus, the offset address is 1011001000b = 0x2C8.
 * So the "_timer2_int" should be put at 176 vectore table in c68k.S file
 */
void timer2_int (void)
{
        ushort          temp;

        QuiccInternalRegsType *qir_p = (QuiccInternalRegsType *)REGB_ADDR;

        /*
         * Reset & stop timer2 and mask interrupt in Timer Global Config reg
         */
        qir_p->cpic_cimr &= ~QUICC_CIMR_TIMER2;
        qir_p->tmr_tgcr &= ~QUICC_TGCR_TMR2_EN;

        /*
         * Clear reference event in TER2 and timer2 in-service interrupt
         */
        temp = qir_p->tmr_ter2;
        qir_p->tmr_ter2 = temp;

        timer_ticks();

        /*
         * Must clear timer2 bit after servicing this timer interrupt is complete
         */
        qir_p->cpic_cipr = QUICC_CIMR_TIMER2;
        qir_p->cpic_cisr = QUICC_CIMR_TIMER2;
        /*
         * Start timer2 and enable interrupt in Timer Global Config register
         */
        qir_p->tmr_tmr2 = TMR2_PRESCALE | TMR2_ICLK_DIV16 | TMR_INT_EN;
        qir_p->tmr_trr2 = TMR2_TIME_1MS;
        qir_p->tmr_tgcr |= QUICC_TGCR_TMR2_EN;
        qir_p->cpic_cimr |= QUICC_CIMR_TIMER2;

}

/*
 * framer_int - framer interrupt routine
 * Loss of signal, loss of frame, remote loopback activate and deactivate
 * are the sources of this interrupt.
 */
void framer_int (void)
{
    register int ix;
    register T1InfoType *t1;
    volatile ushort *framer;
    ushort    src, cause, temp;

    for(ix = 0; ix < MAX_T1 ; ix++ ) {
        t1 = &Interface[ix];
        framer = t1->Fr.framer;

        src = bt8360_rd_freg(framer, T1_INTSRC_REG);
          if ( src ) {
              if(src & T1_INTSRC_ALS) {
                 /* alarm / loopback / signaling */
                 cause = bt8360_rd_freg(framer, T1_ALSSTAT_REG);
                 if (cause & T1_ALSSTAT_LOS) {
                     t1->Fr.IntrptLOS = TRUE;
                     /*
                      * Disable this interrupt.  There's a bug in the
                      * Bt8360 chip that will give a constant interrupt.
                      * Try to avoid that here
                      */
                     temp = bt8360_rd_freg(framer, T1_ALRMINT_REG);
                     temp &= ~T1_ALRMINT_LOS;
                     bt8360_wr_freg(framer, T1_ALRMINT_REG, temp);
                }
                if (cause & T1_ALSSTAT_OOF) {
                    t1->Fr.IntrptOOF = TRUE;
                    /*
                     * Disable this interrupt.  There's a bug in the
                     * Bt8360 chip that will give a constant interrupt.
                     * Try to avoid that here
                     */
                    temp = bt8360_rd_freg(framer, T1_ALRMINT_REG);
                    temp &= ~T1_ALRMINT_OOF;
                    bt8360_wr_freg(framer, T1_ALRMINT_REG, temp);
                }
                /*
                 * Do we have a loop-up ?  If we were searching for
                 * it, stop sending the loop code, and indicate
                 * that we have sucessfully looped the remote unit
                 */
                if ( cause & T1_ALSSTAT_LOOPACT ) {
                    if ( t1->RemoteLoopState == REM_LOOP_ON_SRCH_CODE) {
                        bt8360_stoploop_code( t1 );
                        t1->RemoteLoopState = REM_LOOP_IN_LOOP;
                        t1->InRemLoop = TRUE;
                        t1->LoopbackAct |= REM_LP_PASS;
                        t1->MsgToSendToMPU |= NEAT_MSG_LPBK;
                    }
                }
                /*
                 * Do we have a loop-down ?  If we were searching for
                 * it, indicate that we have received it.  We will
                 * then wait until we stop receiving it.
                 */
                if (( cause & T1_ALSSTAT_LOOPDEACT )
                    && (t1->RemoteLoopState == REM_LOOP_OFF_SRCH_CODE )) {
                    t1->RemoteLoopState = REM_LOOP_OFF_WAIT_NO_CODE;
                }
                if ( cause & T1_ALSSTAT_SIGSTACK )
			t1_rbs_int(t1);
            }
      }
   }
}

/*
 * Initialize DTMF data structure everytime a new digit collection request
 * is received from core CPU
 */
void init_dtmf(DTMFInfoType *dtmf)
{

     dtmf->digit_1st_received = FALSE;
     dtmf->digit_1st_timeout = FALSE;
     dtmf->digit_1st_timer = 0;
     dtmf->digit_rx_done_timer = 0;
     dtmf->digit_rx_done = FALSE;
     dtmf->DTMFToMPU->message = 0x0;
     dtmf->DTMFToMPU->total_digits = 0x0;
     dtmf->DTMFToMPU->num = 0x0;
     dtmf->DTMFFromMPU->current_mid = dtmf->DTMFFromMPU->new_req_mid;
     dtmf->dv_search = TRUE;
     dtmf->digit_col_req = TRUE;
}

/*
 * Set the DTMF message bit in T1 0's MsgToSendToMPU to notify MPU
 * to process phone digits collection
 */
void dtmf_mail_set (uint unit)
{
       if (unit == 0)
           Interface[0].MsgToSendToMPU |= NEAT_DTMF0;
       else
           Interface[0].MsgToSendToMPU |= NEAT_DTMF1;
}

/*
 * mpu_int - core CPU interrupt routine for digit collection request
 * Use level 2 interrupt to NEAT processor.
 */
void mpu_int (void)
{
        DTMFInfoType      *dtmf;
	dtmfmail_from_mpu *dtmf_mail;
        uint              unit;

        for (unit = 0; unit < MAX_DTMF ; unit++) {
	     dtmf = &DTMF[unit];
             dtmf_mail = dtmf->DTMFFromMPU;
	     /*
              * If there is new request for this DTMF from core CPU, restart this
              * request by clearing all status in DTMFInfoType data structure and
              * start timer for first digit collection.
              */
             if (dtmf_mail->new_req_mid & NEW_REQUEST) {

                 init_dtmf(dtmf);

                 /*
                  * After update new request, need to clear new request bit to
                  * distinguish from next new arrive request and notify core
                  * CPU the DTMF is ready to collect the phone digits.
                  */
                 dtmf_mail->new_req_mid = 0x0;
                 dtmf->DTMFToMPU->message = NEAT_MSG_RDY_DIGCOL;
                 dtmf_mail_set(unit);
             }
        }

	/* process the ipc message */
	neat_ipc_msgrcv();
	/*
         * Clear interrupt from core CPU after finished the interrupt
         * service routine.
         */
	NEAT_INT |= CLR_NEAT_INT; 
}
