/* $Id: message.h,v 1.11.6.1 1997/02/21 07:41:06 jliu Exp $
 * $Source: /cscka/t1_neat/src/t1/message.h,v $
 *------------------------------------------------------------------
 * message.h    Messages exchange between NEAT processor and core CPU
 *
 * October, 1995. Joe Liu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: message.h,v $
 * Revision 1.11.6.1  1997/02/21 07:41:06  jliu
 *    o Resolved CSCdi85835 bug and fix a problem in make.rule file
 *    o Resolved CSCdi85835 bug and fix a problem in make.rule file
 *
 * Revision 1.11  1996/09/25 18:25:25  jliu
 *   o Neat bus error could occurred due to level 6 interrupt pending
 *     before neat memory got cleared. So main CPU could read undeterminated
 *     data from Neat memory since system code running before firmware
 *     download
 *
 * Revision 1.10  1996/06/07  21:40:26  jliu
 *     o fixed controller was up but loss of signal was detected problem
 *     o added new sub-controller command for esf fdl exchange setting
 *
 * Revision 1.9  1996/05/09  21:25:52  towu
 * Added new feature Robbed bit signals.
 *
 * Revision 1.8  1996/04/20  02:37:43  jliu
 *     o Changed timer inetrrupt resolution from 50ms to 1 ms
 *
 * Revision 1.7  1996/04/14  20:49:51  jliu
 *     o clean up some comments
 *     o made some changes foe phone digits collection
 *
 * Revision 1.6  1996/04/08  18:17:51  jliu
 *     o cleanup some code
 *     o modified code for digits collection
 *
 * Revision 1.5  1996/03/19  03:54:43  jliu
 *    o change the Tx clock source to internal by default
 *    o always use XBCKI for slip/transmit buffer clock which is derived
 *      from TDM bus 2.044 Mhz
 *    o when external network loopback is detected, need to change the transmit
 *      clock to line and change it back to internal when that loopback
 *      request goes away
 *    o need to initialize Rx PCM control registers
 *    o modify some internal registers setup for tone generation
 *
 * Revision 1.4  1996/03/06  00:42:46  jliu
 *     o The NEAT_INT register is write only. So the code doesn't need to
 *       do read-modify-write
 *
 * Revision 1.3  1996/03/02  03:51:20  towu
 * The basecode to support both mailbox and message queue for ipc.
 *
 * Revision 1.2  1996/02/26  06:21:53  jliu
 *     o Extended the LIU reset cycle to guarantee reset successfully
 *
 * Revision 1.1.1.1  1996/02/10  03:22:52  towu
 * Create
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef HAVE_MESSAGE_H
#define HAVE_MESSAGE_H

#define NULL            0x0
#define MESSAGE_ID      0xff
#define NEW_REQUEST     0x8000
/*
 * The unit of the timer tick is 1ms. So TC_1ST_DIGIT is set for 5s
 * and TC_ALL_DIGIT is set for 2.5s.
 */
#define TC_1ST_DIGIT    100  /* Maximum timer to allow for first digit rcv */
#define TC_ALL_DIGIT    50   /* Maximum timer to allow for the rest of the */
                             /* digits to be received */
#define OnOff_period    10   /* 500 ms for time period to trun on and off */
                             /* busy tone data to TDM bus */
/*
 * These definitions are used for remote loopback test result
 */
#define REM_LP_PASS     0x01
#define REM_LP_FAIL     0x02
#define LINE_LP_NET     0x04
#define NO_LINE_LP_NET  0x08
#define PAYLD_LP_NET    0x10
#define NO_PAYLD_LP_NET 0x20

/*
 * These definitions reflect the latest T1 carrier status
 */
#define CARRIER_UP     0x01
#define CARRIER_DOWN   0x02
#define BUS_ERROR      0x80000000
#define ILLEGAL        0x40000000

/*
 * Messaging from NEAT to MPU.  Read through MailToMPU vector in
 * T1InfoType. For T1 0 the mailbox is at 0x1b000. For T1 1 the
 * mailbox is at 0x1b100. The following bits definition is for message_type
 * in neat2mpu_mail_type data structure. The NEAT_DTMFx is shown to MPU
 * which DTMF has message for MPU. Then MPU should go to DTMF0_TO_MPU
 * location which is 0x1b900 to get DTMF message.
 *
 * In order to consolidate message from T1 and DTMF, currently all DTMF 
 * mail exchange go through T1 0 mailbox. So MPU should check all DTMF 
 * mail from MAIL0_TO_MPU(0x1b000) location. T1 1 mailbox doesn't have
 * DTMF information.
 */
#define NEAT_MSG_NONE         0x00
#define NEAT_MSG_FULL         0x01
#define NEAT_MSG_LPBK         0x02
#define NEAT_MSG_ERROR        0x04
#define NEAT_MSG_CARRIER      0x10	
#define NEAT_DTMF0            0x100
#define NEAT_DTMF1            0x200

/*
 * The following bits definition is for error in neat2mpu_mail_type 
 * data structure.
 */
#define MSG_DIG_MAX_ERR		0x01
#define MSG_BADFCS_ERR		0x02
#define MSG_ABORT_ERR		0x04
#define MSG_OVERRUN_ERR		0x08
#define MSG_RECEIVED		0x10
#define MSG_SENT		0x20

/*
 * The following bits definition is for other_status in neat2mpu_mail_type 
 * data structure.
 */
#define MSG_ES_OCCURRED         0x01
#define MSG_BES_OCCURRED        0x02
#define MSG_SES_OCCURRED        0x04
#define MSG_CSS_OCCURRED        0x08
#define MSG_FR_LOSS_OCCURRED    0x10
#define MSG_LES_OCCURRED        0x20

/*
 * The following messages from MPU are contained in 1 word.  The other messages
 * are defined seperately due to some parameters with those messages. Read 
 * through MailFromMPU in T1InfoType. For T1 0 the mailbox is at 0x1b200. For
 * T1 1 the mailbox is at 0x1b600. The default value is the value after power up
 * which is 0x0.
 */
#define MPU_MSG_DISABLED        0x01  /*enable = 0, disable = 1*/
#define MPU_MSG_FRAME           0x02  /*SF = 0, ESF =1*/
#define MPU_MSG_LINECODE        0x04  /*AMI = 0, B8ZS = 1*/
#define MPU_MSG_LPBK_LOC        0x08  /*disable = 0, enable = 1*/
#define MPU_MSG_LPBK_REM        0x10  /*disable = 0, enable = 1*/
#define MPU_MSG_CLK_SOURCE      0x20  /*external = 0, internal = 1*/
#define MPU_MSG_FDL             0xc0  /*bit 7 & 6 represent fdl type*/
#define MPU_MSG_FDL_ATT         0x40  /*ATT = 01*/
#define MPU_MSG_FDL_ANSI        0x80  /*ANSI = 10*/

/*
 * This DTMF message bits are store at DTMF0_TO_MPU & DTMF1_TO_MPU to
 * tell MPU what kind of the message DTMF sends.
 */
#define NEAT_MSG_RDY_DIGCOL  0x01
#define NEAT_MSG_1STDIG_TO   0x02
#define NEAT_MSG_1STDIG_RCV  0x04
#define NEAT_MSG_DIG_DONE    0x08

/*
 * LIU control register for T1 0 is at 0x84050 and for T1 1 is at 0x84060.
 * Bits definition of the LIU control register
 */
#define   EC4_1         0x0f   /* EC4-1 for pulse equalization and Line */
                               /* Build Out */
#define   RLOOP         0x10   /* 1 for remote loopback */
#define   LLOOP         0x20   /* 1 for local loopback */
#define   TAOS          0x40   /* 1 for transmit all ones */
#define   TRI_RLOOP     0x80   /* tri-state RLOOP to enable NLOOP detection */
#define   ONE_T1_CYCLE  0x20   /* The NEAT CPU speed is 20Mhz, in order to */
                               /* get at least one T1 cycle, the ONE_T1_CYCLE */
                               /* should be 16 * 1.544Mhz > 20Mhz */

/*
 * LIU status register for both T1 0 & 1 is at 0x84090. 
 * Since LIU01_STATUS register shared bt T1 0 & 1, it doesn't put in 
 * T1InfoType data structure. The following are the bits definition.
 */
#define LIU01_STATUS (*(unsigned volatile short *)LIU01_STAT_ADDR)

#define LIU0_DFM        0x01   /* LIU 0 Driver Fail Monitor */
#define LIU0_NLOOP      0x02   /* LIU 0 NLOOP */
#define LIU0_LOS        0x04   /* LIU 0 LOSS of signal */
#define LIU1_DFM        0x08   /* LIU 1 Driver Fail Monitor */
#define LIU1_NLOOP      0x10   /* LIU 1 NLOOP */
#define LIU1_LOS        0x20   /* LIU 1 LOSS of signal */

/* for dsx1TransmitClockSource */
#define DSX1_REC_CLOCK          0
#define DSX1_LOCAL_CLOCK        1

static inline void int4_mpu (void)
{
        NEAT_INT = LEVEL_4;
}

/*
 * carrier_state_to_MPU
 * Reports carrier state changed to core CPU which includes line status
 */
static inline void carrier_state_to_MPU (T1InfoType *t1, ushort state,
                                  carrier_mail_type *carrier)
{
        carrier->carrier_state = state;
        carrier->board_status = t1->Mib->Config.dsx1LineStatus;
        NEAT_INT = LEVEL_6;
}

extern void store_digit(dtmfmail_to_mpu *dtmf_mail, 
                           uint unit, uchar digit_col_reg);
extern void send_message_to_MPU(void);
extern void mail_from_mpu(T1InfoType *t1);
extern void dtmf_mail_set(uint unit);
#endif   /* HAVE_MESSAGE_H */
