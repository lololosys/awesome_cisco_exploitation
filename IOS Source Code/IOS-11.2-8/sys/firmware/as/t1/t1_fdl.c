/* $Id: t1fdl.c,v 1.9.6.1 1997/02/21 07:41:07 jliu Exp $
 * $Source: /cscka/t1_neat/src/t1/t1fdl.c,v $
 *------------------------------------------------------------------
 * t1fdl.c  Facility Data Link Message handling routine
 *
 * October, 1995. Joe Liu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: t1fdl.c,v $
 * Revision 1.9.6.1  1997/02/21 07:41:07  jliu
 *    o Resolved CSCdi85835 bug and fix a problem in make.rule file
 *    o Resolved CSCdi85835 bug and fix a problem in make.rule file
 *
 * Revision 1.9  1996/07/10 06:44:22  jliu
 *     	o Created another timer(250ms) for framing process and one second
 *           DSX1 performance data statistics in order to pass ACCUNET test
 * 	o Code changed for both AT&T and ANSI fdl test
 *         o Removed frame_mode in T1InfoType by using FrameType in frm_info
 *
 * Revision 1.8  1996/06/07  21:40:29  jliu
 *     o fixed controller was up but loss of signal was detected problem
 *     o added new sub-controller command for esf fdl exchange setting
 *
 * Revision 1.7  1996/04/29  03:35:07  jliu
 *     o clean up some code
 *     o updated loopback bit of the line status when local loopback
 *       is being performed
 *
 * Revision 1.6  1996/04/08  18:17:53  jliu
 *     o cleanup some code
 *     o modified code for digits collection
 *
 * Revision 1.5  1996/03/19  03:54:44  jliu
 *    o change the Tx clock source to internal by default
 *    o always use XBCKI for slip/transmit buffer clock which is derived
 *      from TDM bus 2.044 Mhz
 *    o when external network loopback is detected, need to change the transmit
 *      clock to line and change it back to internal when that loopback
 *      request goes away
 *    o need to initialize Rx PCM control registers
 *    o modify some internal registers setup for tone generation
 *
 * Revision 1.4  1996/03/06  00:37:06  jliu
 *     o fixed typing error to include more than one statement in "if" check
 *
 * Revision 1.3  1996/03/02  03:51:21  towu
 * The basecode to support both mailbox and message queue for ipc.
 *
 * Revision 1.2  1996/02/22  22:27:47  jliu
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

/************************************************************************
 * Here is how the code handles FDL messages from network.
 * - there are two initialization routines (fdl_rlyQ_init & fdl_reqQ_init)
 *   in t1framer.c to allocate 30 (MAX_REQ_MESSAGE & MAX_RLY_MESSAGE) buffers
 *   for each request and reply queue. The size of the request buffer is the
 *   size of the FDLRequestType and the size of the reply buffer is 246.
 *
 * - Each request and reply buffer has in_process and has_message to indicate
 *   the status of the buffer. has_message means the buffer contains message.
 *   in_process means the message in that buffer is being processed. 
 *
 * - Each queue has free pointer to point to the next available buffer and 
 *   message pointer to point to the buffer which has message being processed
 *   or waiting for process. Those pointers should wrap around when reach 
 *   MAX_REQ_MESSAGE or MAX_RLY_MESSAGE value. The free pointer should not
 *   overrun message pointer in either queue.
 *
 * - The request message is queued up in request queue in FDL interrupt 
 *   routine and the has_message status is set. The request message is replied
 *   in main loop routine and the in_process status is set. 
 *
 * - The in_process and has_message in request buffer are cleared after 
 *   the corresponding reply buffer contains the necessary information
 *   from request buffer and is in reply queue or is being processed.
 * 
 * - The has_message in reply buffer is set when the buffer has all reply
 *   data and set the in_process if the data in this reply buffer is being
 *   sent to external network.
 *
 * - The in_process and has_message in reply buffer are cleared after the
 *   data in this buffer is sent completely.
 ****************************************************************************/

/*
 * bzero
 * Zero some memory, takes address and byte count
 */
void bzero (void *dummy, int count)
{
    ulong *lp;
    int   lcount;
    uchar *address = dummy;

    /* First, do a few bytes so that we're word aligned. */
    for (; count && ((int)address & 0x3); count--)
        *address++ = 0;

    /* Now do the longs. */
    lp = (ulong *) address;
    lcount = count >> 2;
    for (; lcount > 16; lcount -= 16) {
        *lp++ = 0;
        *lp++ = 0;
        *lp++ = 0;
        *lp++ = 0;
        *lp++ = 0;
        *lp++ = 0;
        *lp++ = 0;
        *lp++ = 0;
        *lp++ = 0;
        *lp++ = 0;
        *lp++ = 0;
        *lp++ = 0;
        *lp++ = 0;
        *lp++ = 0;
        *lp++ = 0;
        *lp++ = 0;
    }
    while (lcount) {
        *lp++ = 0;
        lcount--;
    }
    address = (uchar *) lp;
    count &= 0x3;

    /* Now do what's left. */
    for (; count; address++, count--)
        *address = 0;
}

void payloadloop_pro( T1InfoType *t1, bool flag )
{
     vushort   *bt8360;
     uchar     temp;
 
     bt8360 = t1->Fr.framer;
     temp = bt8360_rd_freg(bt8360, T1_FRAME_REG);

     if (flag == TRUE) {
         temp |= T1_FRAME_PYLDLOOP;
     } else {
         temp &= (~T1_FRAME_PYLDLOOP);
     }
     bt8360_wr_freg(bt8360, T1_FRAME_REG, temp);
}
         
void t1_nloop (T1InfoType *t1, bool flag)
{
     	vushort   *bt8360;
        ushort    temp, old;

	bt8360 = t1->Fr.framer;
        old = bt8360_rd_freg(bt8360,T1_CFG_REG);
        temp = bt8360_rd_freg(bt8360, T1_FRAME_REG);
        if (flag) {
             temp |= T1_FRAME_LINELOOP;
             old &= ~T1_CFG_INTTXCLK;
        } else {
             temp &= (~T1_FRAME_LINELOOP);
             old |= T1_CFG_INTTXCLK;
        } 
        bt8360_wr_freg(t1->Fr.framer, T1_CFG_REG, old);
        old = bt8360_rd_freg(bt8360, T1_CFG_REG);
        bt8360_wr_freg(bt8360, T1_FRAME_REG, temp);
}

void next_request_message ( FDLRequestType *req, frm_info *frm )
{
    /*
     * clear the in_process bit since the request message has been replied
     */
    req->in_process = 0x0;
    req->has_message = 0x0;
    req->length = 0x0;
    frm->req_message_head += 1;
    if (frm->req_message_head >= MAX_REQ_MESSAGE)
        frm->req_message_head = 0x0;
}

/*
 * Here we start sending a new reply message to network after check reply
 * message queue. The rule to stuff transmit data link buffer in framer is
 * 1. if less than or equal to 16 reply message byte need to be sent, fill
 *    the buffer and set T1_TXCTRL_MOP & T1_TXCTRL_FCS then wait interrupt.
 *    When interrupt occurs, set T1_TXCTRL_MOP & T1_TXCTRL_IDLE. This causes
 *    the TDL circuit to send the 2-octet FCS and one flag after this last 
 *    blobk of data.
 *
 * 2. if more than 16 reply message byte need to sent, fill the lower half
 *    buffer, set which_half to 0x80 and set T1_TXCTRL_MOP only then wait
 *    interrupt. When it occurs, fill the upper half and switch which_half
 *    to 0x0. If the remainning bytes less than or equal to 0x8, set
 *    T1_TXCTRL_FCS as well then wait interrupt. If it occurs, fill the 
 *    remainning bytes and set T1_TXCTRL_IDLE. 
 * 3. If the remainning bytes is greater than 0x08 then fill either lower
 *    or upper half accordingly until it is less than or equal to 0x08. 
 *    In this case, set T1_TXCTRL_FCS then wait interrupt. If it occurs, 
 *    fill the remainning bytes and set T1_TXCTRL_IDLE.
 */
static void t1_fdl_send (T1InfoType *t1,  FDLReplyType *reply)
{
    uint      index, ix;
    vushort   *bt8360;
    uchar     *start;


    bt8360 = t1->Fr.framer;
    start = &reply->address;
    /*
     * If we are sending remote alarm, stop sending remote alarm so the fdl
     * reply messages can be sent but should keep 1 second minimum to send
     * another fdl reply message
     */
    if (t1->Fr.TxingRemAlarm == TRUE) {
        bt8360_tx_remotealarm(t1, FALSE);
        /*
         * This delay is necessary in order to pass ACCUNET test
         */
        for (ix=0; ix < 50000; ix++)
                  ;
        t1->fdl_resend_RAI = TRUE;
    }

    if (reply->length <= 16) {
        for (index = 0; index < reply->length; index++) {
            bt8360_wr_fram(bt8360, (T1_TX_DL_BUFFER + index), *start++);
        }
        bt8360_wr_freg(bt8360, T1_TXCTRL_REG, (((reply->length - 1) << 4) |
                         (T1_TXCTRL_MOP | T1_TXCTRL_FCS)));
        reply->length = 0;
    } else {
        for (index = 0; index < 8; index++) {
            bt8360_wr_fram(bt8360, (T1_TX_DL_BUFFER + index), *start++);
        }
        reply->length -= 8;
        reply->data_start = &reply->data[0];
        reply->which_half = 0x80;    /* indicate which half of buffer */
        bt8360_wr_freg(bt8360, T1_TXCTRL_REG, (0xf0 | T1_TXCTRL_MOP));
    }
}

static inline void reply_next_buf (frm_info *frm)
{
    uint         index;

    index = frm->rly_free_head;
    index += 1; 
    if (index >= MAX_RLY_MESSAGE)
        index = 0x0;
    frm->rly_free_head = index;
}

void fdl_reply_process (T1InfoType *t1, frm_info *frm)
{
    FDLReplyType *current;

    current = &frm->DL_Reply[frm->rly_message_head];
    /*
     * If has_message is zero, that means rly_message_head is equal to
     * rly_free_head and no reply message need to be sent.
     */
    if (!(current->has_message))
          return;
    if (!(current->in_process)) {
        current->in_process = 0x01;
        t1_fdl_send( t1, current );
    }
}

static void common_fdl_reply (LineStatus lsp, FDLReplyType *reply,
                                 FDLRequestType *req)
{
    uchar temp;

    reply->address = 0x81;
    reply->control = 0x08;
    reply->cmd = req->cmd;
    reply->status = 0x0;
    reply->org_unit_addr = req->org_unit_addr;
    reply->unit_addr = req->unit_addr;
    reply->request = req->request;
    temp = 0;
    if (lsp & DSX1_LOOPED)
        temp |= 0x82;
    if ((lsp & DSX1_NO_ALARM) == 0)
         temp |= 0xc0;
    reply->cur_status = temp;
}

static void confirmation_reply (T1InfoType *t1, uchar status,
                               FDLRequestType *req)
{
    frm_info     *frm;
    FDLReplyType *reply;

    frm = &t1->Fr;
    reply = &frm->DL_Reply[frm->rly_free_head];
    reply_next_buf(frm);
    common_fdl_reply(t1->Mib->Config.dsx1LineStatus, reply, req);

    reply->status = status;
    reply->length = 8;
    reply->has_message = 0x01;

    fdl_reply_process(t1, frm);
}

#define WRITE_ESF_DL_SHORT(reply, data) *reply++ = data & 0xff;\
                                        *reply++ = data >> 8;

static inline void WRITE_ESF_DL_SHORT_NO_INC (uchar *reply, ushort data)
{
    *reply = data & 0xff;
    *(reply + 1) = data >> 8;
}

static void t1_esf_dl_crc_reply (T1InfoType *t1, FDLRequestType *req)
{
    frm_info     *frm;
    FDLReplyType *reply;
    uchar        *reply_data;

    frm = &t1->Fr;
    reply = &frm->DL_Reply[frm->rly_free_head];
    reply_next_buf(frm);
    common_fdl_reply(t1->Mib->Config.dsx1LineStatus, reply, req);
    reply_data = &reply->data[0];
    WRITE_ESF_DL_SHORT(reply_data, t1->crc_count);
    reply->length = 10;
    reply->has_message = 0x01;

    fdl_reply_process(t1, frm);
}


static void t1_one_hr_reply (T1InfoType *t1, FDLRequestType *req)
{
    frm_info     *frm;
    FDLReplyType *reply;
    uchar        *reply_data;
    uint         index, ix;

    frm = &t1->Fr;
    reply = &frm->DL_Reply[frm->rly_free_head];
    reply_next_buf(frm);
    common_fdl_reply(t1->Mib->Config.dsx1LineStatus, reply, req);
    reply_data = &reply->data[0];
    WRITE_ESF_DL_SHORT(reply_data, t1->Mib->Config.dsx1TimeElapsed);
    WRITE_ESF_DL_SHORT(reply_data, t1->Mib->CurrentData.dsx1ESs);
    WRITE_ESF_DL_SHORT(reply_data, t1->Mib->CurrentData.dsx1UASs);
    *reply_data++ = t1->Mib->Config.dsx1ValidIntervals;
    WRITE_ESF_DL_SHORT(reply_data, t1->Mib->TotalData.dsx1ESs);
    WRITE_ESF_DL_SHORT(reply_data, t1->Mib->TotalData.dsx1UASs);
    index = (t1->Mib->CurrentIndex) - 1;
    for (ix = 0; ix < 4; ix++) {
        WRITE_ESF_DL_SHORT_NO_INC(reply_data, t1->Mib->Intervals[index].dsx1ESs);
        WRITE_ESF_DL_SHORT_NO_INC(reply_data + 8,
                                  t1->Mib->Intervals[index].dsx1UASs);
        reply_data += sizeof(ushort);
        if (index == 0) index = QUARTER_HOURS_IN_DAY - 1;
        else index--;
    }
    reply->length = 35;
    reply->has_message = 0x01;

    fdl_reply_process(t1, frm);
}

static void t1_reset_perform_reply (T1InfoType *t1, FDLRequestType *req)
{
    frm_info     *frm;
    FDLReplyType *reply;
    uint         index, ix, total_intervals;

    frm = &t1->Fr;
    reply = &frm->DL_Reply[frm->rly_free_head];
    reply_next_buf(frm);
    common_fdl_reply(t1->Mib->Config.dsx1LineStatus, reply, req);
    t1->Mib->Config.dsx1TimeElapsed = 0x0;
    t1->Mib->CurrentData.dsx1ESs = 0x0;
    t1->Mib->CurrentData.dsx1UASs = 0x0;
    t1->Mib->CurrentData.dsx1BESs = 0x0;
    t1->Mib->CurrentData.dsx1SESs = 0x0;
    t1->Mib->CurrentData.dsx1CSSs = 0x0;
    t1->Mib->CurrentData.dsx1LOFCs = 0x0;
    total_intervals = t1->Mib->Config.dsx1ValidIntervals;
    t1->Mib->Config.dsx1ValidIntervals = 0x0;
    t1->Mib->TotalData.dsx1ESs = 0x0;
    t1->Mib->TotalData.dsx1UASs = 0x0;
    t1->Mib->TotalData.dsx1BESs = 0x0;
    t1->Mib->TotalData.dsx1SESs = 0x0;
    t1->Mib->TotalData.dsx1CSSs = 0x0;
    t1->Mib->TotalData.dsx1LOFCs = 0x0;
    index = (t1->Mib->CurrentIndex) - 1;
    for (ix = 0; ix < total_intervals; ix++) {
         t1->Mib->Intervals[index].dsx1ESs = 0x0;
         t1->Mib->Intervals[index].dsx1UASs = 0x0;
         t1->Mib->Intervals[index].dsx1BESs = 0x0;
         t1->Mib->Intervals[index].dsx1SESs = 0x0;
         t1->Mib->Intervals[index].dsx1CSSs = 0x0;
         t1->Mib->Intervals[index].dsx1LOFCs = 0x0;
         if (index == 0) index = QUARTER_HOURS_IN_DAY - 1;
         else index--;
    }
    reply->length = 8;
    reply->has_message = 0x01;

    fdl_reply_process(t1, frm);
}

    
static void t1_esf_dl_24hr_reply (T1InfoType *t1, FDLRequestType *req,
                                    bool es)
{
    frm_info     *frm;
    FDLReplyType *reply;
    uchar        *reply_data;
    uint         index, ix;

    index = 0x0;
    frm = &t1->Fr;
    reply = &frm->DL_Reply[frm->rly_free_head];
    reply_next_buf(frm);
    common_fdl_reply(t1->Mib->Config.dsx1LineStatus, reply, req);
    reply_data = &reply->data[0];

    WRITE_ESF_DL_SHORT(reply_data, t1->Mib->Config.dsx1TimeElapsed);
    WRITE_ESF_DL_SHORT(reply_data, t1->Mib->CurrentData.dsx1ESs);
    WRITE_ESF_DL_SHORT(reply_data, t1->Mib->CurrentData.dsx1UASs);
    *reply_data++ = t1->Mib->Config.dsx1ValidIntervals;
    WRITE_ESF_DL_SHORT(reply_data, t1->Mib->TotalData.dsx1ESs);
    WRITE_ESF_DL_SHORT(reply_data, t1->Mib->TotalData.dsx1UASs);

    index = (t1->Mib->CurrentIndex) - 1;
    for (ix = 0; ix < QUARTER_HOURS_IN_DAY; ix++) {
        if (es) {
            WRITE_ESF_DL_SHORT(reply_data,
                               t1->Mib->Intervals[index].dsx1ESs);
        } else {
            WRITE_ESF_DL_SHORT(reply_data,
                               t1->Mib->Intervals[index].dsx1UASs);
        }
        if (index == 0) index = QUARTER_HOURS_IN_DAY - 1;
        else index--;
    }
    reply->length = 211;
    reply->has_message = 0x01;

    fdl_reply_process(t1, frm);
}

static uchar *t1_esf_dl_enh_setup (T1InfoType *t1, uchar *data)
{
    WRITE_ESF_DL_SHORT(data, t1->Mib->Config.dsx1TimeElapsed);
    WRITE_ESF_DL_SHORT(data, t1->Mib->CurrentData.dsx1ESs);
    WRITE_ESF_DL_SHORT(data, t1->Mib->CurrentData.dsx1UASs);
    WRITE_ESF_DL_SHORT(data, t1->Mib->CurrentData.dsx1BESs);
    WRITE_ESF_DL_SHORT(data, t1->Mib->CurrentData.dsx1SESs);
    *data++ = t1->Mib->Config.dsx1ValidIntervals;
    WRITE_ESF_DL_SHORT(data, t1->Mib->TotalData.dsx1ESs);
    WRITE_ESF_DL_SHORT(data, t1->Mib->TotalData.dsx1UASs);
    WRITE_ESF_DL_SHORT(data, t1->Mib->TotalData.dsx1BESs);
    WRITE_ESF_DL_SHORT(data, t1->Mib->TotalData.dsx1SESs);
    return(data);
}

static void t1_esf_dl_enh_1hr_reply (T1InfoType *t1, FDLRequestType *req)
{
    frm_info     *frm;
    FDLReplyType *reply;
    uchar        *reply_data;
    uint         index, ix;

    frm = &t1->Fr;
    reply = &frm->DL_Reply[frm->rly_free_head];
    reply_next_buf(frm);
    common_fdl_reply(t1->Mib->Config.dsx1LineStatus, reply, req);
    reply_data = &reply->data[0];
    reply_data = t1_esf_dl_enh_setup(t1, reply_data);
    index = (t1->Mib->CurrentIndex) - 1;
    for (ix = 0; ix < 4; ix++) {
        WRITE_ESF_DL_SHORT_NO_INC(reply_data, t1->Mib->Intervals[index].dsx1ESs);
        WRITE_ESF_DL_SHORT_NO_INC(reply_data + 8,
                                  t1->Mib->Intervals[index].dsx1UASs);
        WRITE_ESF_DL_SHORT_NO_INC(reply_data + 16,
                                  t1->Mib->Intervals[index].dsx1BESs);
        WRITE_ESF_DL_SHORT_NO_INC(reply_data + 24,
                                  t1->Mib->Intervals[index].dsx1SESs);
        /* Do CSS and LOFC */
        *(reply_data + 36) = t1->Mib->Intervals[index].dsx1CSSs;
        *(reply_data + 37) = t1->Mib->Intervals[index].dsx1LOFCs;
        reply_data += sizeof(ushort);
        if (index == 0) index = QUARTER_HOURS_IN_DAY - 1;
        else index--;
    }
    *(reply_data + 24) = t1->Mib->CurrentData.dsx1CSSs;
    *(reply_data + 25) = t1->Mib->CurrentData.dsx1LOFCs;
    *(reply_data + 26) = t1->Mib->TotalData.dsx1CSSs;
    *(reply_data + 27) = t1->Mib->TotalData.dsx1LOFCs;
    PUTSHORT((ushort *)(reply_data + 36), 0xabcd);  /* 0's at end */
    reply->length = 73;
    reply->has_message = 0x01;

    fdl_reply_process(t1, frm);
}

static void t1_esf_dl_enh_24hr_reply (T1InfoType *t1, FDLRequestType *req,
                                        ushort type)
{
    frm_info     *frm;
    FDLReplyType *reply;
    uchar        *reply_data;
    uint         index, ix;

    frm = &t1->Fr;
    reply = &frm->DL_Reply[frm->rly_free_head];
    reply_next_buf(frm);
    common_fdl_reply(t1->Mib->Config.dsx1LineStatus, reply, req);
    reply_data = &reply->data[0];
    reply_data = t1_esf_dl_enh_setup(t1, reply_data);
    index = (t1->Mib->CurrentIndex) - 1;
    if (type == TYPE_ES)
        for (ix = 0; ix < QUARTER_HOURS_IN_DAY; ix++) {
            WRITE_ESF_DL_SHORT(reply_data,
                               t1->Mib->Intervals[index].dsx1ESs);
            if (index == 0) index = QUARTER_HOURS_IN_DAY - 1;
            else index--;
        }
    else if (type == TYPE_UAS)
        for (ix = 0; ix < QUARTER_HOURS_IN_DAY; ix++) {
            WRITE_ESF_DL_SHORT(reply_data,
                               t1->Mib->Intervals[index].dsx1UASs);
            if (index == 0) index = QUARTER_HOURS_IN_DAY - 1;
            else index--;
        }
    else if (type == TYPE_BES)
        for (ix = 0; ix < QUARTER_HOURS_IN_DAY; ix++) {
            WRITE_ESF_DL_SHORT(reply_data,
                               t1->Mib->Intervals[index].dsx1BESs);
            if (index == 0) index = QUARTER_HOURS_IN_DAY - 1;
            else index--;
        }
    else if (type == TYPE_SES)
        for (ix = 0; ix < QUARTER_HOURS_IN_DAY; ix++) {
            WRITE_ESF_DL_SHORT(reply_data,
                               t1->Mib->Intervals[index].dsx1SESs);
            if (index == 0) index = QUARTER_HOURS_IN_DAY - 1;
            else index--;
        }
    else
        for (ix = 0; ix < QUARTER_HOURS_IN_DAY; ix++) {
            *reply_data++ = t1->Mib->Intervals[index].dsx1CSSs;
            *reply_data++ = t1->Mib->Intervals[index].dsx1LOFCs;
            if (index == 0) index = QUARTER_HOURS_IN_DAY - 1;
            else index--;
        }

    *reply_data++ = t1->Mib->CurrentData.dsx1CSSs;
    *reply_data++ = t1->Mib->CurrentData.dsx1LOFCs;
    *reply_data++ = t1->Mib->TotalData.dsx1CSSs;
    *reply_data++ = t1->Mib->TotalData.dsx1LOFCs;
    reply->length = 223;
    reply->has_message = 0x01;

    fdl_reply_process(t1, frm);
}

static void t1_esf_dl_ci_reply (T1InfoType *t1, FDLRequestType *req)
{
    frm_info     *frm;
    FDLReplyType *reply;
    uchar        *reply_data;

    frm = &t1->Fr;
    reply = &frm->DL_Reply[frm->rly_free_head];
    reply_next_buf(frm);
    common_fdl_reply(t1->Mib->Config.dsx1LineStatus, reply, req);
    reply_data = &reply->data[0];
    *reply_data++ = 2; /* indicate we do CSS */
    bzero(reply_data, 10);
    reply->length = 19;
    reply->has_message = 0x01;

    fdl_reply_process(t1, frm);
}

void fdl_req_process (T1InfoType *t1)
{
    FDLRequestType *req;
    frm_info       *frm;
            int  ix=0;

    frm = &t1->Fr;
    req = &frm->DL_Request[frm->req_message_head];

    /*
     * Check if there are any request messages waiting for reply
     */
    if (req->has_message > 0) {
        /*
         * if AT&T TR54016 was not selected then ignore this request message
         */
        if (t1->fdl_std != DSX1_FDL_ATT) {
            next_request_message(req, frm);
            return;
        }

        /*
         * if the request message is being replied then wait until the reply
         * message sent completely. This should not happen, since code process
         * request message until either reply completely or put in reply queue
         */
        if (req->in_process == 0)
            req->in_process = 0x01;
        else
            return;

        /* Message oriented protocol */
        if (req->address != 0xc3) {
            next_request_message(req, frm); /* Not AT&T */
            return;
        }

        if (req->control != 0x08) {
            next_request_message(req, frm);
            return;
        }

        if (((req->unit_addr != 0x41) && (req->unit_addr != 'Z')) 
                                        && (req->unit_addr != 0x80)) {
             next_request_message(req, frm);
             return;
        }

        /*
         * Now do the reply
         */
        switch (req->request) {
        case 1:
            SetMIBLineStatus(&t1->Mib->Config.dsx1LineStatus, DSX1_LOOPED);
            t1->Mib->Config.dsx1LoopbackConfig = DSX1_PAYLOAD_LOOP;
            confirmation_reply(t1, ESF_DL_RESPONSE_GOOD_CMD, req);
            /*
             * This delay is necessary in order to pass ACCUNET test
             */
            for (ix=0; ix < 30000; ix++)
                      ;
            payloadloop_pro(t1, TRUE);
            t1->LoopbackAct |= PAYLD_LP_NET;
            t1->MsgToSendToMPU |= NEAT_MSG_LPBK;
            if (t1->unit) {
                NEAT_LED |= LOOPBACK_1;
            } else {
                NEAT_LED |= LOOPBACK_0;
            }
            break;
        case 2:
            ClearMIBLineStatus(&t1->Mib->Config.dsx1LineStatus, DSX1_LOOPED);
            t1->Mib->Config.dsx1LoopbackConfig = DSX1_NO_LOOP;
            confirmation_reply(t1, ESF_DL_RESPONSE_GOOD_CMD, req);
            /*
             * This delay is necessary in order to pass ACCUNET test
             */
            for (ix=0; ix < 30000; ix++)
                      ;
            payloadloop_pro(t1, FALSE);
            t1->LoopbackAct |= NO_PAYLD_LP_NET;
            t1->MsgToSendToMPU |= NEAT_MSG_LPBK;
            if (t1->unit) {
                NEAT_LED &= (~LOOPBACK_1);
            } else {
                NEAT_LED &= (~LOOPBACK_0);
            }
            break;
        case 64:
        case 128: /* 1 hour performance request */
            t1_one_hr_reply(t1, req);
            break;
        case 66:
        case 130:
        case 194: /* Reset Performance Monitoring counter */
            t1_reset_perform_reply(t1, req);
            break;
        case 67:
        case 131: /* Errored ESF Data counter */
            t1_esf_dl_crc_reply(t1, req);
            break;
        case 68:
        case 132:
        case 196: /* reset Errored ESF Data counter */
            t1->crc_count = 0x0;
            confirmation_reply(t1, ESF_DL_RESPONSE_GOOD_CMD, req);
            break;
        case 100:
        case 164: /* 24 hour ES data */
            t1_esf_dl_24hr_reply(t1, req, TRUE);
            break;
        case 101:
        case 165: /* 24 hour UAS data */
            t1_esf_dl_24hr_reply(t1, req, FALSE);
            break;
        case 103: /* enhanced 1 hour performance data */
            t1_esf_dl_enh_1hr_reply(t1, req);
            break;
        case 107: /* enhanced 24 hour ES performance data */
            t1_esf_dl_enh_24hr_reply(t1, req, TYPE_ES);
            break;
        case 108: /* enhanced 24 hour UAS performance data */
            t1_esf_dl_enh_24hr_reply(t1, req, TYPE_UAS);
            break;
        case 109: /* enhanced 24 hour BES performance data */
            t1_esf_dl_enh_24hr_reply(t1, req, TYPE_BES);
            break;
        case 110: /* enhanced 24 hour SES performance data */
            t1_esf_dl_enh_24hr_reply(t1, req, TYPE_SES);
            break;
        case 111: /* enhanced 24 hour LOFC performance data */
            t1_esf_dl_enh_24hr_reply(t1, req, TYPE_CSS_LOFC);
            break;
        case 231: /* enhanced CI configuration data */
            t1_esf_dl_ci_reply(t1, req);
            break;
        default:
            confirmation_reply(t1, ESF_DL_RESPONSE_BAD_CMD, req);
            break;
        }
        next_request_message(req, frm);
    }
}

