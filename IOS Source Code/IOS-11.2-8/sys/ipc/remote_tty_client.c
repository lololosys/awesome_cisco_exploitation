/* $Id$
 * $Source$
 *------------------------------------------------------------------
 * Remote tty client side -- runs on the slave RSP
 *
 * February 1997, Derek Taubert
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log$
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "config.h"
#include "types.h"
#include "logger.h"
#include "sys_registry.h"
#include "../h/ttysrv.h"
#include "../os/async.h"
#include "remote_tty.h"
#include "../ipc/ipc_util.h"
#include "../ipc/ipc.h"
#include "../ipc/ipc_private.h"
#include "../ipc/ipc_message.h"
#include "../ipc/ipc_seats.h"
#include "../ipc/ipc_ports.h"

static char *RTTYC_ipc_port_name;

static void RTTYC_handler (ipc_message *req_msg, void *context, ipc_error_code ec)
{
    ipc_message *rsp_msg;
    ipc_message_header *req_hdr;
 
    req_hdr = req_msg->header;
    switch (req_hdr->type) {
      case RTTYC_ATTACH_REQ:
	rsp_msg = ipc_get_message(sizeof(rttyc_attach_rsp_t), 0,
				  RTTYC_ATTACH_RSP);
	if (!rsp_msg) {
	    /* post an error */
	} else {
	    rttyc_attach_req_t *req = req_msg->data;
	    rttyc_attach_rsp_t *rsp = rsp_msg->data;
	    tt_soc *tty = number2tt(req->remote_ttynum);

	    if (tty->RTTY_server_port) {
		rsp->return_code = RTTY_ALREADY_ATTACHED;
	    } else {
		tty->RTTY_server_port = req->server_port;
		tty->server_ttynum = req->server_ttynum;
		rsp->return_code = RTTY_NO_ERROR;
	    }
	    ipc_send_rpc_reply(req_msg, rsp_msg);
	}
	break;
      case RTTYC_DETACH_REQ:
	rsp_msg = ipc_get_message(sizeof(rttyc_detach_rsp_t), 0,
				  RTTYC_DETACH_RSP);
	if (!rsp_msg) {
	    /* post an error */
	} else {
	    rttyc_detach_req_t *req = req_msg->data;
	    rttyc_detach_rsp_t *rsp = rsp_msg->data;
	    tt_soc *tty = number2tt(req->remote_ttynum);

	    if (tty->RTTY_server_port) {
		tty->RTTY_server_port = 0;
		rsp->return_code = RTTY_NO_ERROR;
	    } else {
		rsp->return_code = RTTY_NOT_ATTACHED;
	    }
	    ipc_send_rpc_reply(req_msg, rsp_msg);
	}
	break;
      case RTTYC_INPUT_REQ: {
	rttyc_input_req_t *req = req_msg->data;
	tt_soc *tty = number2tt(req->remote_ttynum);
	char *pbuf;
	leveltype SR;

	for (pbuf = req->buffer; *pbuf; pbuf++) {
	    if (special_chars(tty, *pbuf) >= 0) {
		SR = raise_interrupt_level(ALL_DISABLE);
		/*
		 * Handle buffer wrap
		 */
		if (tty->intail >= tty->inend)
		    tty->intail = tty->instart;
		/*
		 * Handle overflow
		 */
		if ((tty->intail == tty->inhead) && tty->incount) {
		    tty->overflow++;
		    if (modem_debug)
			ttybug(tty,"Input buffer overflow");
		    reset_interrupt_level(SR);
		    break;
		}
		/*
		 * Add character to end of buffer
		 */
		*tty->intail++ = *pbuf;
		++tty->incount;
		reset_interrupt_level(SR);
	    }
	}
	ipc_return_message(req_msg);
	break;
      }
    }
}

static void RTTYC_reg_handler_port (void)
{
    ipc_port_id handler_port;
    ipc_error_code retcode;
 
    /*
     * By now the port may have been renumber,
     * so we need to get it again by the name.
     */
    handler_port = ipc_locate_port(RTTYC_ipc_port_name);
 
    retcode = ipc_register_port(handler_port);
    if (retcode != IPC_OK) {
        /* post an error */
        ;
    }
}

static vectortype RTTYvector;

int RTTYC_init (boolean is_slave_RSP)
{
    ipc_error_code ec;
    ipc_port_id handler_port;
    vectortype *vector;

    if (is_slave_RSP)
	RTTYC_ipc_port_name = strdup(RTTY_CLIENT_SLAVE_PORT_NAME);
    if (!RTTYC_ipc_port_name)
	return -1;

    ec = ipc_create_named_port(RTTYC_ipc_port_name, &handler_port, 0);
    if (ec != IPC_OK) {
	/* post an error */
	return -1;
    } else {
	ec = ipc_set_receive_callback(handler_port, 0, RTTYC_handler);
    }

    /*
     * Can not register our handler port right now as IPC
     * may have not been initialized yet. Register an init
     * function for later invocation.
     */
    ipc_register_delayed_func(IPC_LEVEL_FILESYS, RTTYC_reg_handler_port);

    vector = &RTTYvector;
    vector_init(vector);

    vector->vgetc = serial_getc;
    vector->vinputpending = serial_inputpending;
    vector->vputc = RTTYC_putc;
    vector->vstartoutput = RTTYC_startoutput;

    return 0;
}

void RTTYC_putc (tt_soc *tty, int c)
{
    rtty_output_req_t *req;

    if (!tty->RTTY_server_port)
	return;

    if (!tty->RTTY_req_msg) {
	tty->RTTY_req_msg = ipc_get_message(sizeof(rtty_output_req_t),
				tty->RTTY_server_port, RTTY_OUTPUT_REQ);
	if (tty->RTTY_req_msg == NULL) {
	    /* post an error */
	    return;
	}
	tty->outcount = 0;
    }
    req = tty->RTTY_req_msg->data;
    req->buffer[tty->outcount++] = c;
    if (c == 10 || tty->outcount == RTTY_OUTPUT_MAX-1) {
	startoutput(tty);
    }
}

boolean RTTYC_startoutput (tt_soc *tty)
{
    rtty_output_req_t *req;
    ipc_message *req_msg;
    ipc_message *rsp_msg;
    ipc_error_code ec;

    if (!tty->RTTY_server_port)
	return FALSE;

    req_msg = tty->RTTY_req_msg;
    if (!req_msg)
	return TRUE;
    tty->RTTY_req_msg = NULL;

    req = req_msg->data;
    req->server_ttynum = tty->server_ttynum;
    req->buffer[tty->outcount] = 0;
    rsp_msg = ipc_send_rpc(req_msg, &ec);
    if (ec != IPC_OK) {
	/* post an error */
    } else {
	ipc_return_message(rsp_msg);
    }
    return TRUE;
}

void RTTYC_reassign_tty (int server_ttynum)
{
    tt_soc *vty;

    vty = tty_allocate(0, TRUE);
    if (!vty) /* ??? */ return;

    vty->ivector = vty->ovector = vty->nvector = &RTTYvector;

    serial_setbuffers(vty, MAXINCHARS, 0);
    vty->modem_bits &= ~(255);
    vty->modem_bits |= MODEMBIT_DTR;
    vty->tty_exectimeout = 0;

    vty->RTTY_server_port = ipc_locate_port(RTTY_SERVER_MASTER_PORT_NAME);
    vty->server_ttynum = server_ttynum;
    vty->RTTY_req_msg = NULL;

    /* re-assign stdio to the vty */
    process_set_ttysoc(THIS_PROCESS, vty);
    process_get_pid(&vty->tiptop);
}

void RTTYC_restore_tty (void)
{
    tt_soc *vty;

    /* release IPC buffer we might have left */
    startoutput(stdio);

    /*
     * until process_kill() supports death() correctly,
     * WE will have to deallocate the vty
     */
    vty = stdio;
    if (vty != console) {
	process_set_ttysoc(THIS_PROCESS, console);
	vty->tiptop = -1;
	tty_deallocate(vty);
    }
}

