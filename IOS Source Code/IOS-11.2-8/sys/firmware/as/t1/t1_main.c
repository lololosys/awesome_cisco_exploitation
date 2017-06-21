/* $Id: main.c,v 1.17.6.2 1997/02/21 07:41:04 jliu Exp $
 * $Source: /cscka/t1_neat/src/t1/main.c,v $
 *------------------------------------------------------------------
 * main.c  NEAT main routine
 *
 * October, 1995. Joe Liu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: main.c,v $
 * Revision 1.17.6.2  1997/02/21 07:41:04  jliu
 *    o Resolved CSCdi85835 bug and fix a problem in make.rule file
 *    o Resolved CSCdi85835 bug and fix a problem in make.rule file
 *
 * Revision 1.17.6.1  1997/01/17 23:50:58  jliu
 * 	o Fixed potential race condition between IOS and firmware
 *
 * Revision 1.17  1996/07/13 05:24:51  towu
 * o Enable RBS feature.
 *
 * Revision 1.16  1996/07/10  06:44:21  jliu
 *     	o Created another timer(250ms) for framing process and one second
 *           DSX1 performance data statistics in order to pass ACCUNET test
 * 	o Code changed for both AT&T and ANSI fdl test
 *         o Removed frame_mode in T1InfoType by using FrameType in frm_info
 *
 * Revision 1.15  1996/05/12  23:50:13  towu
 * Fixed bugs for ct1 rbs features.
 *
 * Revision 1.14  1996/05/12  22:24:52  jchlin
 * o       Add rbs_process() in the main process loop.
 * o       Change the message stream sent form neat to MPU
 *         from one to two. One is high priority and the other
 *         is low priority.
 * o       Add one more status STATUS_CONNECTED so that the state
 *         machine doesn't need to wait for the modem going off-ook
 *         before get the incoming call connected.
 * o       Make sure the NO_CIRCUIT_AVAILABLE is the same as ISDN cause
 *         number.
 *
 * Revision 1.13  1996/05/09  21:25:52  towu
 * Added new feature Robbed bit signals.
 *
 * Revision 1.12  1996/05/06  06:01:05  jliu
 *     o Cleaned some code for LED on and off
 *     o Set DSX1_XMT_AIS in dsx1LineStatus after sending AIS
 *
 * Revision 1.11  1996/04/29  03:35:06  jliu
 *     o clean up some code
 *     o updated loopback bit of the line status when local loopback
 *       is being performed
 *
 * Revision 1.10  1996/04/20  02:37:40  jliu
 *     o Changed timer inetrrupt resolution from 50ms to 1 ms
 *
 * Revision 1.9  1996/04/16  00:05:06  jliu
 *     o clean up some debug code
 *
 * Revision 1.8  1996/04/14  20:49:50  jliu
 *     o clean up some comments
 *     o made some changes foe phone digits collection
 *
 * Revision 1.7  1996/04/08  18:14:52  jliu
 *     o Made some configuration changes to get tone data generation
 *       works
 *     o Implemented transmit underrun error interrupt
 *
 * Revision 1.6  1996/03/19  03:54:42  jliu
 *    o change the Tx clock source to internal by default
 *    o always use XBCKI for slip/transmit buffer clock which is derived
 *      from TDM bus 2.044 Mhz
 *    o when external network loopback is detected, need to change the transmit
 *      clock to line and change it back to internal when that loopback
 *      request goes away
 *    o need to initialize Rx PCM control registers
 *    o modify some internal registers setup for tone generation
 *
 * Revision 1.5  1996/03/02  03:51:19  towu
 * The basecode to support both mailbox and message queue for ipc.
 *
 * Revision 1.4  1996/03/01  22:20:49  jliu
 *     o unit in t1 data structure need to be initialized
 *
 * Revision 1.3  1996/02/28  18:30:11  jliu
 *     o Truned on tone data generation
 *
 * Revision 1.2  1996/02/22  22:27:46  jliu
 *     o update thge current T1 firmware for starting channelized T1 coding
 *
 * Revision 1.1.1.1  1996/02/10  03:22:52  towu
 * Create
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define	DECLARE_GLOBS
#include	"t1_headers.h"

static timer_t tasks_50ms;

/*
 * initialize registers in 68360 to have 1ms timeout interrupt periodically. 
 * This 1ms timer is mainly used for 2 Bt8360 framing process and digit
 * collection process 
 */
void init_1ms_timer( )
{

    QuiccInternalRegsType *qir_p = (QuiccInternalRegsType *)REGB_ADDR;

    /*
     *  CPM interrupt mask register (7-380)
     *  Masked timer 2 interrupts
     */
    qir_p->cpic_cimr &= ~QUICC_CIMR_TIMER2;

    /*
     * Timer global configuration register (7-20)
     * Reset the corresponding timer - timer 2 in this case
     */
    qir_p->tmr_tgcr &= ~QUICC_TGCR_TMR2_EN;

    /*
     * Timer registers (7-21)
     * tmr_tmr2 - 0xff for prescaler, internal system clock divided by 16,
     *            enable interrupt upon reaching the reference value.
     * tmr_tcn2 - cleared the timer counter.
     * tmr_trr2 - set the reference to 244, so 1ms timer can be obtained
     * tmr_ter2 - Cleared reference and capture events
     */
    qir_p->tmr_tmr2 = TMR2_PRESCALE | TMR2_ICLK_DIV16 | TMR_INT_EN;
    qir_p->tmr_tcn2 = 0x0;          /* clear the count */
    qir_p->tmr_trr2 = TMR2_TIME_1MS;
    qir_p->tmr_ter2 = TMR_TER_CAP_EVENT | TMR_TER_REF_EVENT;

    /*
     * Enable interrupt and start timer 2 in Timer Global Config register
     */
    qir_p->cpic_cimr |= QUICC_CIMR_TIMER2;
    qir_p->tmr_tgcr |= QUICC_TGCR_TMR2_EN;
    /*
     * initialize timer counter
     */
     rbs_report_timer = 0;
}

/*
 * initialize registers in 68360 to have 1ms timeout interrupt periodically.
 * This 1ms timer is mainly used for 2 Bt8360 framing process and digit
 * collection process
 */
void init_250ms_timer( )
{

    QuiccInternalRegsType *qir_p = (QuiccInternalRegsType *)REGB_ADDR;

    /*
     *  CPM interrupt mask register (7-380)
     *  Masked timer 1 interrupts
     */
    qir_p->cpic_cimr &= ~QUICC_CIMR_TIMER1;

    /*
     * Timer global configuration register (7-20)
     * Reset the corresponding timer - timer 1 in this case
     */
    qir_p->tmr_tgcr &= ~QUICC_TGCR_TMR1_EN;

    /*
     * Timer registers (7-21)
     * tmr_tmr1 - 0xff for prescaler, internal system clock divided by 16,
     *            enable interrupt upon reaching the reference value.
     * tmr_tcn1 - cleared the timer counter.
     * tmr_trr1 - set the reference to 244, so 1ms timer can be obtained
     * tmr_ter1 - Cleared reference and capture events
     */
    qir_p->tmr_tmr1 = TMR1_PRESCALE | TMR1_ICLK_DIV1 | TMR_INT_EN;
    qir_p->tmr_tcn1 = 0x0;          /* clear the count */
    qir_p->tmr_trr1 = TMR1_TIME_250MS;
    qir_p->tmr_ter1 = TMR_TER_CAP_EVENT | TMR_TER_REF_EVENT;

    /*
     * Enable interrupt and start timer 1 in Timer Global Config register
     */
    qir_p->cpic_cimr |= QUICC_CIMR_TIMER1;
    qir_p->tmr_tgcr |= QUICC_TGCR_TMR1_EN;

}

/*
 * Initialize dial tone data path to TDM bus through MC68360 TDM output
 */
void init_dialtone ( QuiccInternalRegsType *qir_p )
{
    SCCTransParamType *stp = (SCCTransParamType *)SCC1_PARAM_ADDR;
    QuiccBdType *tbd;

    tbd = (QuiccBdType *)(DPRAM_ADDR + SCC1_TxBD_OFFSET);

    /*
     * Write TBASE in the SCC 1 parameter RAM to point to the Tx BD in the
     * dual-port RAM
     */
    stp->tbase = SCC1_TxBD_OFFSET;

    /*
     * SCC function code register (7-125)
     * Motorola byte ordering, FC3-FC0 = 0110 to identify this SDMA
     * channel access as a DMA-type access since we use the same SRAM to
     * store code and tone data
     */
    stp->tfcr = 0x16;

    /*
     * Command register (7-5)
     * INIT TX PARAMETERS=1, Channel Number=SCC1, Command Semaphore Flag=1
     * This command causes the TBPTR parameter of the serial channel to be
     * updated with the new values just programmed into TBASE
     */
    qir_p->cp_cr = 0x0201;

    /*
     * Transparent transmit buffer descriptor (7-230)
     * Ready = 1, Wrap = 1, Interrupt = 0, Last in Message = 0,
     * Transmit CRC = 0, Continuous Mode = 1
     */
    tbd->status = 0xa200;
    /*
     * There are total 4k bytes of data for dial tone
     */
    tbd->length = 0xfa0;
    /*
     * The transmit buffer pointer which points to the first byte of the
     * 4k dial tone data block in external SRAM
     */
    tbd->buf_ptr = (volatile uchar *)DIALTONE_ADDR;

    /*
     * Transparent event and mask registers (7-232)
     * Clear any previous events, mask all corresponding interrupts in the
     * event register, mask SCC1 interrupt in CIMR
     */
    qir_p->scc[0].scc_scce = 0xffff;
    qir_p->scc[0].scc_sccm = 0x10;
    qir_p->cpic_cimr &= ~QUICC_CIMR_SCC1;

    /*
     * General SCC mode register (7-111)
     * Enable transparent operation on transmitter, need to set ENT bit in
     * GSMRL to enable transmitter hardware state machine for SCC1 after
     * transmit routing in TSA has been programmed
     */
    qir_p->scc[0].scc_gsmrl = 0x00000000;
    qir_p->scc[0].scc_gsmrh = 0x00003f80; /* set TTX for transparent mode */
}

/*
 * Initialize busy tone data path to TDM bus through MC68360 TDM output
 */
void init_busytone ( QuiccInternalRegsType *qir_p )
{
    SCCTransParamType *stp = (SCCTransParamType *)SCC2_PARAM_ADDR;
    QuiccBdType *tbd;

    tbd = (QuiccBdType *)(DPRAM_ADDR + SCC2_TxBD_OFFSET);

    /*
     * Write TBASE in the SCC 2 parameter RAM to point to the Tx BD in the
     * dual-port RAM
     */
    stp->tbase = SCC2_TxBD_OFFSET;

    /*
     * Motorola byte ordering, FC3-FC0 = 0110 to identify this SDMA
     * channel access as a DMA-type access since we use the same SRAM to
     * store code and tone data
     */
    stp->tfcr = 0x16;

    /*
     * INIT TX PARAMETERS=1, Channel Number=SCC2, Command Semaphore Flag=1
     * This command causes the TBPTR parameter of the serial channel to be
     * updated with the new values just programmed into TBASE
     */
    qir_p->cp_cr = 0x0241;

    /*
     * Ready = 1, Wrap = 1, Interrupt = 0, Last in Message = 0,
     * Transmit CRC = 0, Continuous Mode = 1
     */
    tbd->status = 0xa200;
    /*
     * There are total 4k bytes of data for busy tone
     */
    tbd->length = 0xfa0;
    /*
     * The transmit buffer pointer which points to the first byte of the
     * 4k busy tone data block in external SRAM
     */
    tbd->buf_ptr = (volatile uchar *)BUSYTONE_ADDR;

    /*
     * Clear any previous events, mask all corresponding interrupts in the
     * event register, mask SCC2 interrupt in CIMR
     */
    qir_p->scc[1].scc_scce = 0xffff;
    qir_p->scc[1].scc_sccm = 0x00;
    qir_p->cpic_cimr &= ~QUICC_CIMR_SCC2;

    /*
     * Enable transparent operation on transmitter, need to set ENT bit in
     * GSMRL to enable transmitter hardware state machine for SCC2 after
     * transmit routing in TSA has been programmed
     */
    qir_p->scc[1].scc_gsmrl = 0x00000000;
    qir_p->scc[1].scc_gsmrh = 0x00003f80; /* set TTX for transparent mode */
}

/*
 * Description:
 *      Set SIRAM format to use a static map.  All part are set for
 *      transmit. The values below are derived using the following 
 *      format.  This map is for dial and busy tone data generation.
 *
 *      SWTR  SSEL   CSEL  CNT   BYT  LST  Description
 *
 *       0    0000   001   0000   1    0   1 byte from SCC1 for dial tone
 *       0    0000   010   0000   1    1   1 byte from SCC2 for busy tone
 */
void init_siram ( QuiccInternalRegsType *qir_p)
{
    int             ix;
    volatile ushort *siram;

    siram = qir_p->si_siram;
    /*
     * SI routing RAM entries (7-72)
     * CSEL = 001 means the byte group is routed to SCC1,  
     * CNT = 0 & BYT = 1 means one byte in this group, 
     * LST = 0 means this is not the last entry
     */
    for (ix = 0x0; ix < QUICC_SIRAM_TX ; ix++)
         siram[QUICC_SIRAM_RX + ix] = 0x0;

    siram[QUICC_SIRAM_TX + 0] = SCC1_DIALBYTE;
    /*
     * CSEL = 010 means the byte group is routed to SCC2,
     * CNT = 0 & BYT = 1 means one byte in this group,
     * LST = 1 means this is the last entry
     */
    siram[QUICC_SIRAM_TX + 1] = SCC2_BUSYBYTE;

    for (ix = 0x2; ix < QUICC_SIRAM_TX; ix++)
         siram[QUICC_SIRAM_TX + ix] = 0x0;

    /*
     * SI global mode register (7-77)
     * Tell the system that we are going to have one map in routing RAM
     * for a single TDM channel.
     */
    qir_p->si_sigmr = QUICC_SIGMR_ONE_STATIC;

    /*
     * SI mode regsietr (7-78)
     * Initalize the mode register. 
     * CRTx = 1 - common rx and tx pins for TDM A
     * CEx = 1 & DSCx = 0 - data is transmitted on the falling edge of the clk
     *                      and received on the rising edge
     * FEx = 1 - frame sync edge is sampled on the rising edge 
     * GMx = 1 - IDL mode for TDM A or B grant mechanism
     */
    qir_p->si_simode = 0x05;

    /*
     * SI clock route register (7-86)
     * SC1 & SC2 = 1, SCC1&2 are connected to the multiplexed SI.
     */
    qir_p->si_sicr |= PORT_SCC1_SCC2;

    /*
     * SI command register (7-87)
     * The receiver and transmitter shadow RAM are not valid for TDM a & b
     */
    qir_p->si_sicmr = 0x00;

    /*
     * Enable TDM channel a
     */
    qir_p->si_sigmr |= QUICC_SIGMR_ENABLE_A;

}

/*
 * Setup dial & busy tone data path to TDM bus through MC68360 TDM output
 */
void init_tonedata ( )
{
    QuiccInternalRegsType *qir_p = (QuiccInternalRegsType *)REGB_ADDR;

    /*
     * Initialize serial interface routing RAM for dial and busy tone data
     */
    init_siram(qir_p);

    init_dialtone(qir_p);

    init_busytone(qir_p);

}

/*
 * Enable transmitter hardware state machine to start sending tone data
 * from external SRAM to TDM bus.
 */
void send_tonedata ( )
{
    QuiccInternalRegsType *qir_p = (QuiccInternalRegsType *)REGB_ADDR;

    /*
     * set ENT bit in general SCC mode register (7-111)
     */
    qir_p->cpic_cimr |= QUICC_CIMR_SCC1;
    qir_p->scc[0].scc_gsmrl = 0x00000010;
    qir_p->scc[1].scc_gsmrl = 0x00000010;
}

/*
 * This routine is used to process all situations occurred during digit
 * collection.
 * Note: The digit is only valid when low to high transition on DV bit
 *       is detected. After store the digit, the software need to poll
 *       the DV bit until the high to low transition is detected then 
 *       software start to polling DV bit for next arrive digit.
 */
void digit_pro ( DTMFInfoType *dtmf, uint unit )
{
    uint 	number;
    uchar       temp;

    if (unit == 0)
        temp = (uchar) (DIGIT_COL0 & 0xff);
    else
        temp = (uchar) (DIGIT_COL1 & 0xff);

    if (dtmf->digit_col_req) {
        if (!(dtmf->digit_1st_received)) {
            if (dtmf->digit_1st_timeout) {
                dtmf->digit_1st_timeout = FALSE;
                dtmf->digit_col_req = FALSE;
                dtmf->DTMFToMPU->message = NEAT_MSG_1STDIG_TO;
                dtmf_mail_set(unit);
            } else {
              if (dtmf->dv_search) {
                 if (temp & DV) {
                     dtmf->dv_search = FALSE;
                     dtmf->digit_1st_received = TRUE;
                     store_digit(dtmf->DTMFToMPU, unit, temp);
                     dtmf->DTMFToMPU->total_digits += 0x1;
                     dtmf->digit_rx_done_timer = 0; /*start timer for the rest*/
                                                    /*of digits collection*/
                     dtmf->DTMFToMPU->message = NEAT_MSG_1STDIG_RCV;
                     dtmf_mail_set(unit);
                 }
              }
           }
       } else {
           if (dtmf->digit_rx_done) {
               dtmf->digit_col_req = FALSE;
               dtmf->digit_1st_received = FALSE;
               number = dtmf->DTMFToMPU->num;
               dtmf->DTMFToMPU->digit[number] = NULL;
               dtmf->DTMFToMPU->total_digits += 0x1; /* include NULL byte */
               dtmf->DTMFToMPU->message = NEAT_MSG_DIG_DONE;
               dtmf_mail_set(unit);
           } else {
               if (dtmf->dv_search) {
                  if (temp & DV) {
                     dtmf->dv_search = FALSE;
                     store_digit(dtmf->DTMFToMPU, unit, temp);
                     dtmf->DTMFToMPU->total_digits += 0x1;
                     dtmf->digit_rx_done_timer = 0;
                  }
               } else {
                 if (!(temp & DV))
                     dtmf->dv_search = TRUE;
              }
          }
      }
   }
}

/*
 * This routine is used to handle network loopback 
 */
void netloop_pro ( T1InfoType *t1, bool flag )
{

      t1_nloop(t1, flag);
      if (flag == TRUE) {
          SetMIBLineStatus(&t1->Mib->Config.dsx1LineStatus, DSX1_LOOPED);
          t1->Mib->Config.dsx1LoopbackConfig = DSX1_LINE_LOOP;
          t1->LoopbackAct |= LINE_LP_NET;
          if (t1->unit) {
              NEAT_LED |= LOOPBACK_1;
          } else {
              NEAT_LED |= LOOPBACK_0;
          }
      } else {
          ClearMIBLineStatus(&t1->Mib->Config.dsx1LineStatus, DSX1_LOOPED);
          t1->Mib->Config.dsx1LoopbackConfig = DSX1_NO_LOOP;
          t1->LoopbackAct |= NO_LINE_LP_NET;
          if (t1->unit) {
              NEAT_LED &= (~LOOPBACK_1);
          } else {
              NEAT_LED &= (~LOOPBACK_0);
          }
      }
      t1->MsgToSendToMPU |= NEAT_MSG_LPBK;
}

/* 
 * Check to see the system side finished the reload 
 */
void  check_reload ( void )
{
      if (MPU_RELOAD_DONE == 0xabcd) {
          MPU_reload = FALSE;
          MPU_RELOAD_DONE = 0x0;
          set_ipl(ALL_INTERRUPTS);
      }
}
          
/*
 * 50ms timer timeout, this 50ms timeout is used  for digit collection,
 * frame process and busy tone generation
 */
void t1_50ms_task (timer_t *timer, void *para1, void *para2)
{ 
        int             ix;
        DTMFInfoType    *dtmf;

        QuiccInternalRegsType *qir_p = (QuiccInternalRegsType *)REGB_ADDR;

        /*
         * The timer interrupt occurs every 1ms, so TC_1ST_DIGIT x 1ms
         * is the time to check first digit received or not? Also,
         * TC_ALL_DIGIT x 1ms is the time to figure out all digits are
         * collected if timeout occurs after first digit received.
         */
        for (ix = 0; ix <MAX_DTMF; ix++) {
             dtmf = &DTMF[ix];
             if (dtmf->digit_col_req) {
                 if (!dtmf->digit_1st_received) {
                     dtmf->digit_1st_timer += 0x01;
                     if (dtmf->digit_1st_timer > TC_1ST_DIGIT)
                         dtmf->digit_1st_timeout = TRUE;
                 } else {
                     if (!dtmf->digit_rx_done) {
                         dtmf->digit_rx_done_timer += 0x01;
                         if (dtmf->digit_rx_done_timer > TC_ALL_DIGIT)
                             dtmf->digit_rx_done = TRUE;
                     }
                 }
             }
        }
 
        BusyToneTimer += 0x01;
        if (BusyToneTimer == OnOff_period) {
            /*
             * reset the BusyToneTimer after reach to 500ms
             */
            BusyToneTimer = 0x0;
            if (BUSYTONE_ON == FALSE) {
                /*
                 * Refer to (7-140) for SCC transmitter full sequence
                 * need to trun off busy tone data to TDM bus for 500ms
                 * a. issue a stop transmit command to CP for SCC2
                 * b. clear the ENT bit in the GSMR
                 */
                qir_p->cp_cr = QUICC_CPCR_STOPTX | QUICC_CPCR_SCC2 |
                                  QUICC_CPCR_SEMA;
                qir_p->scc[1].scc_gsmrl = 0x00000000;
 
                BUSYTONE_ON = TRUE;
             } else {
                /*
                 * need to trun off busy tone data to TDM bus for 500ms
                 * a. issue a restart transmit command to CP for SCC2
                 * b. enable the ENT bit in the GSMR
                 */
                qir_p->cp_cr = QUICC_CPCR_RESTART | QUICC_CPCR_SCC2 |
                                  QUICC_CPCR_SEMA;
                qir_p->scc[1].scc_gsmrl = 0x00000010;
 
                BUSYTONE_ON = FALSE;
            }
        }

        timer_start_one_shot(timer, 50, t1_50ms_task, 0, 0);
}

/*
 * This is the main loop which checks framing process, commands from core
 * CPU, network loopback request from external network, digits collection
 * from modem cards if request has been made from core CPU and FDL request
 * message from external network
 */ 
void main_code ( void )
{
    
    T1InfoType      *t1;
    DTMFInfoType    *dtmf;
    ushort          status;
    int             ix, iy;

    /*
     * check each T1 FDL request messages queue to see any request message
     * need to be replied.
     */
    for ( ix =0; ix < MAX_T1; ix++) {
          fdl_req_process(&Interface[ix]);
          fdl_reply_process(&Interface[ix], &Interface[ix].Fr);
    }

    /* process low priority messages */
    neat_ipc_msg_process();  

    for ( ix =0; ix < MAX_T1; ix++) {
          t1 = &Interface[ix];
          if (t1->RAI_resend == TRUE) {
              t1->RAI_resend = FALSE;
              if (t1->Fr.TxingRemAlarm != TRUE)
                  continue;
              /*
               * the delay is necessary in order to pass ACCUNET test
               */
              for (iy=0; iy < 50000; iy++)
                          ;
              bt8360_tx_remotealarm(t1, TRUE);
           }
    }

    /*
     * check network loopback request from LIU status register. If loopback
     * request activate is detected, set att_lineloop status in Interface data
     * structure and jump to netloop_pro subroutine to enable the loopback.
     * If loopback request deactivate is detected, reset att_lineloop status and
     * goes to netloop_pro subroutine to disable the loopback.
     */
    status = LIU01_STATUS;
    if (!Interface[0].att_lineloop) {
        if (status & LIU0_NLOOP) {
            Interface[0].att_lineloop = TRUE;
            netloop_pro(&Interface[0], TRUE);
        }
    } else {
        if (!(status & LIU0_NLOOP)) {
            Interface[0].att_lineloop = FALSE;
            netloop_pro(&Interface[0], FALSE);
        }
    }

    if (!Interface[1].att_lineloop) {
        if (status & LIU1_NLOOP) {
            Interface[1].att_lineloop = TRUE;
            netloop_pro(&Interface[1], TRUE);
        }
    } else {
        if (!(status & LIU1_NLOOP)) {
            Interface[1].att_lineloop = FALSE;
            netloop_pro(&Interface[1], FALSE);
        }
    }
	
    /*
     * If 250ms time has elapsed, run framing process for that T1 interface
     */
    for ( ix =0; ix < MAX_T1; ix++) {
          t1 = &Interface[ix];
          if (t1->frame_pro_need) {
              t1->frame_pro_need = FALSE;
              T1_Framing_Process(t1, ix);
          }
    }
      
    /*
     * When system side is doing reload, all digits 
     * collection and messages exchange between core CPU and NEAT processor
     * should be ceased but keep FDL activity, network loopback monitoring
     * and framing process going without any interrupt from system side.
     */
    if (!(MPU_reload)) {
        /*
         * check digit_1st_received first. If false, that means we just start 
         * collecting first digit then check digit_1st_timer to see it timeout 
         * before first digit received. If true, notify core CPU about this. If 
         * not true, check data valid bit. If data valid is true, store the 
         * first digit and start digit_rx_done_timer to collect the remainning 
         * digits
         */
        for ( ix=0; ix < MAX_DTMF; ix++) {
              dtmf = &DTMF[ix];
              digit_pro(dtmf, ix);
        }
    } else {
       check_reload();
    }

    if (!(MPU_reload)) {
        /*
         * Here we send all pending messages to core CPU and process all
         * commands from core CPU for each T1 interface at one time.
         */
        if (Have1Sec) {
            send_message_to_MPU();
            Have1Sec = FALSE;
        }
    } else {
       check_reload();
    }

    for ( ix =0; ix < MAX_T1; ix++) {
          mail_from_mpu(&Interface[ix]);
    }

    if (!(MPU_reload))
       t1_rbs_report();
    else
       check_reload();
}


/*
 *	The main system code.
 */

void Main ( void )
{
    T1InfoType    *t1;
    DTMFInfoType  *dtmf;
    int		  t1_id;

    /* Turn off the channelized T1 at this time */
    ct1_enable = 1;
    ipc = (ipc_section *)IPC_AREA;
    neat_ipc_msg_mgmt_init();
    for (t1_id = 0; t1_id < MAX_T1; t1_id++) {
	t1 = &Interface[t1_id];
	t1->unit = t1_id;
	t1->Fr.framer = (unsigned volatile short *)(FRAMER_BASE_ADDR 
                                                      + FRAMER_GAP*t1_id);
	t1->MailFromMPU = (vushort *)MAIL_FROM_MPU(t1_id);
	t1->MailToMPU = (neat2mpu_mail_type *)MAIL_TO_MPU(t1_id);
	t1->Carrier = (carrier_mail_type *)CARRIER_TO_MPU(t1_id);
	t1->LIU_Control = (vushort *)(LIU_CONT_BASE_ADDR + LIU_CONT_GAP*t1_id);
	CLEAR_COUNTER(OneSecTimer[t1_id]);
	interface_init( t1 );

    	dtmf = &DTMF[t1_id];
    	dtmf->DTMFFromMPU = (dtmfmail_from_mpu *)DTMF_FROM_MPU(t1_id);
    	dtmf->DTMFToMPU = (dtmfmail_to_mpu *)DTMF_TO_MPU(t1_id);
    	dtmf->digit_col_req = FALSE;
        dtmf->dv_search = FALSE;
    }

    init_tonedata();
    send_tonedata();

    /*
     * MPU_reload will be set TRUE in backplane interrupt to indicate
     * system side is doing reload and ask NEAT processor to stop any
     * activity with core CPU which includes digits collection, send message,
     * receive messages to/from core CPU and framing process.
     *
     * MPU_RELOAD_DONE will be set to non-zero when core CPU finishes the
     * reload and NEAT processor will keep polling it until non-zero occurs.
     */
    MPU_reload = TRUE;
    MPU_RELOAD_DONE = 0x0;

   /*
    * This timer(500ms) is used for busy tone generation.
    * 0.5 sec on and 0.5 sec off
    */
    BusyToneTimer = 0x0;
    BUSYTONE_ON = FALSE;
    Have1Sec = FALSE;

    t1_rbs_init();    

    timer_init(&tasks_50ms);
    timer_start_one_shot(&tasks_50ms, 50, t1_50ms_task, 0, 0);
    
    init_1ms_timer();
    init_250ms_timer();
    set_ipl(ALL_INTERRUPTS);
    /*
     *	OK, now we're ready to go.
     */
    for(;;) {
	main_code();
        rbs_process();
    }
}

