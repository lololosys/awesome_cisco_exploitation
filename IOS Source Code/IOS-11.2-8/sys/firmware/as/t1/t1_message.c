/* $Id: message.c,v 1.3.6.2 1997/02/21 07:41:05 jliu Exp $
 * $Source: /cscka/t1_neat/src/t1/message.c,v $
 *------------------------------------------------------------------
 * message.c    Messages exchange between NEAT processor and core CPU
 *
 * October, 1995. Joe Liu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: message.c,v $
 * Revision 1.3.6.2  1997/02/21 07:41:05  jliu
 *    o Resolved CSCdi85835 bug and fix a problem in make.rule file
 *    o Resolved CSCdi85835 bug and fix a problem in make.rule file
 *
 * Revision 1.3.6.1  1997/01/17 23:50:59  jliu
 * 	o Fixed potential race condition between IOS and firmware
 *
 * Revision 1.3  1996/10/10 23:45:23  jchlin
 * o       Set the framer with default value of SF and AMI during the
 *         initialization of framer.
 * o       Enable the robbed bit interrupt mask in framer chip.
 * o       Change the timer timeout process so that it can process
 *         the timeout timer in the iteration of certain number
 *         before it releases CPU control.
 *
 * Revision 1.2  1996/06/07  21:40:24  jliu
 *     o fixed controller was up but loss of signal was detected problem
 *     o added new sub-controller command for esf fdl exchange setting
 *
 * Revision 1.1.1.1  1996/02/10  03:22:53  towu
 * Create
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include        "t1_headers.h"

/*
 * send_message_to_MPU
 * Send and clear all the data associated with the message
 * Note: send a level 4 interrupt to MPU only there are messages
 *       from either T1 interface or both to be sent to core CPU
 */
void send_message_to_MPU (void)
{
    T1InfoType         *t1;
    neat2mpu_mail_type *mptr;
    int                ix;
    uchar              send_int4 = FALSE;
    ushort             sr;


    for ( ix =0; ix < MAX_T1; ix++) {
         /*
          * See if there are any messages waiting for sending back to MPU 
          */
         t1 = &Interface[ix];
         if (t1->MsgToSendToMPU == NEAT_MSG_NONE)
             continue;

         mptr = t1->MailToMPU;
         /*
          * See if the MPU has picked up the last message
          */
         if (mptr->message_type != NEAT_MSG_NONE)
             continue;

         if (t1->MsgToSendToMPU & NEAT_MSG_FULL) {
             mptr->board_status = t1->Mib->Config.dsx1LineStatus;
             mptr->other_status = t1->Fr.OtherErrors;
             mptr->BPVcounter = t1->Fr.BPVErrs;
             mptr->PCVcounter = t1->Fr.PCVErrs;
             mptr->CRCcounter = t1->Fr.CRCErrs;
             mptr->FBEcounter = t1->Fr.FBEErrs;

             t1->Fr.OtherErrors = 0;
             t1->Fr.BPVErrs = 0;
             t1->Fr.PCVErrs = 0;
             t1->Fr.CRCErrs = 0;
             t1->Fr.FBEErrs = 0;
         }
         sr = set_ipl(PRIO_FDL);
         if (t1->MsgToSendToMPU & NEAT_MSG_LPBK) {
            mptr->loopback_status = t1->LoopbackAct;
            t1->LoopbackAct = NEAT_MSG_NONE;
         }
         mptr->message_type = t1->MsgToSendToMPU;
         t1->MsgToSendToMPU = NEAT_MSG_NONE;
         send_int4 = TRUE;
         reset_ipl(sr);
    }

    if (send_int4 && (!MPU_reload))
       int4_mpu();
}

/*
 * Check to see if anything has changed in the mailbox from the MPU
 * Process that information
 */
void mail_from_mpu (T1InfoType *t1)
{
    ushort          xor, read;
    dsx1MIBDataType *mib;
    volatile ushort *framer;



    mib = t1->Mib;
    framer = t1->Fr.framer;
    read  = *t1->MailFromMPU;
    xor = t1->MsgReceivedFromMPU ^ read;
    if (xor) {
        if (xor & MPU_MSG_DISABLED)
            bt8360_tx_ais(t1, ((read & MPU_MSG_DISABLED) ? TRUE : FALSE));

        if (xor & MPU_MSG_FRAME)
            bt8360_framing(t1, ((read & MPU_MSG_FRAME) ? DSX1_ESF : DSX1_SF));

        if (xor & MPU_MSG_LINECODE)
            bt8360_linecode(t1,
                          ((read & MPU_MSG_LINECODE) ? DSX1_B8ZS : DSX1_AMI));

        if (xor & MPU_MSG_LPBK_LOC) {
            if (read & MPU_MSG_LPBK_LOC)
                bt8360_local_up(t1);
            else
                bt8360_no_loop(t1);
        }

        if (xor & MPU_MSG_LPBK_REM) {
            if (read & MPU_MSG_LPBK_REM)
                bt8360_loopup_code(t1);
            else
                bt8360_loopdown_code(t1);
        }

        if (xor & MPU_MSG_CLK_SOURCE) {
            if (read & MPU_MSG_CLK_SOURCE)
                bt8360_clock_source(t1, DSX1_LOCAL_CLOCK);
            else
                bt8360_clock_source(t1, DSX1_REC_CLOCK);
        }

       if ((read & MPU_MSG_FDL) != ((t1->MsgReceivedFromMPU) & MPU_MSG_FDL)) {
            if ((read & MPU_MSG_FDL) == MPU_MSG_FDL_ANSI) {
                 t1->fdl_std = DSX1_FDL_ANSI;
                 mib->Config.dsx1Fdl = DSX1_FDL_ANSI;
                 bt8360_wr_freg(framer, T1_RXCTRL_REG,
                        (T1_RXCTRL_STACKINT | T1_RXCTRL_MOPINT | T1_RXCTRL_BOPINT));
             } else if ((read & MPU_MSG_FDL) == MPU_MSG_FDL_ATT) {
                 t1->fdl_std = DSX1_FDL_ATT;
                 mib->Config.dsx1Fdl = DSX1_FDL_ATT;
                 bt8360_wr_freg(framer, T1_RXCTRL_REG, 
                                (T1_RXCTRL_STACKINT |T1_RXCTRL_MOPINT));
             } else if ((read & MPU_MSG_FDL) == 0x0) {
                 t1->fdl_std = DSX1_FDL_NONE;
                 mib->Config.dsx1Fdl = DSX1_FDL_NONE;
                 bt8360_wr_freg(framer, T1_RXCTRL_REG, T1_RXCTRL_STACKINT);
             }
        }

        /*
         * Save the byte
         */
        t1->MsgReceivedFromMPU = read;
    }
}

/*
 * Need to convert hexadecimal code to ASCII code
 */
uchar convert_to_ascii (uchar code)
{
	uchar   digit=0;

            if ((code >= 0x01) && (code <= 0x09)) 
                 digit = code + '0';
        
            switch (code) {
            case 0x0a:
                 digit = '0';
                 break;
            case 0x0b:
                 digit = 0x2a;
                 break;
            case 0x0c:
                 digit = 0x23;
                 break;
            case 0x0d:
                 digit = 0x41;
                 break;
            case 0x0e:
                 digit = 0x42;
                 break;
            case 0x0f:
                 digit = 0x43;
                 break;
            case 0x00:
                 digit = 0x44;
                 break;
            default:
                 break;
            }
            return(digit);
}

/*
 * Store each phone number digit to digit[0] in dtmfmail_to_mpu. When it
 * complete, send them back to core CPU. Here is the digit hex code table
 *
 *                     	   Output Code
 *         Digit          D8  D4  D2  D1                
 *         -----          --------------
 *           1            0   0   0   1
 *           2            0   0   1   0
 *           3            0   0   1   1
 *           4            0   1   0   0
 *           5            0   1   0   1
 *           6            0   1   1   0
 *           7            0   1   1   1
 *           8            1   0   0   0
 *           9            1   0   0   1
 *           0            1   0   1   0
 *           *            1   0   1   1
 *           #            1   1   0   0
 *           A            1   1   0   1
 *           B            1   1   1   0
 *           C            1   1   1   1
 *           D            0   0   0   0
 */
void store_digit (dtmfmail_to_mpu *dtmf_mail, uint unit, uchar digit_col_reg) 
{
        T1InfoType *t1;

        t1 = &Interface[0];
        digit_col_reg &= D4_1;
        dtmf_mail->digit[dtmf_mail->num] = convert_to_ascii(digit_col_reg);
        dtmf_mail->num += 1;
        if (dtmf_mail->num > MAX_DIGITS) {
            t1->MsgToSendToMPU |= NEAT_MSG_ERROR;
            t1->MailToMPU->error |= MSG_DIG_MAX_ERR;
        } 
}
