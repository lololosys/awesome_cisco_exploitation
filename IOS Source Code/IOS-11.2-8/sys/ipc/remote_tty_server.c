/* $Id$
 * $Source$
 *------------------------------------------------------------------
 * Remote tty server side -- runs on the master RSP image
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
#include "remote_tty.h"
#include "../ipc/ipc_util.h"
#include "../ipc/ipc.h"
#include "../ipc/ipc_private.h"
#include "../ipc/ipc_message.h"
#include "../ipc/ipc_seats.h"
#include "../ipc/ipc_ports.h"

static char *RTTY_ipc_port_name;

static void RTTYS_handler (ipc_message *req_msg, void *context, ipc_error_code ec)
{
    ipc_message *rsp_msg;
    ipc_message_header *req_hdr;

    req_hdr = req_msg->header;
    switch (req_hdr->type) {
    case RTTY_OUTPUT_REQ:
	rsp_msg = ipc_get_message(sizeof(rtty_output_rsp_t), 0,
						RTTY_OUTPUT_RSP);
	if (rsp_msg == NULL) {
	    /* post an error */
	} else {
	    rtty_output_rsp_t *rsp = rsp_msg->data;
	    rtty_output_req_t *req = req_msg->data;
	    tt_soc *tty = number2tt(req->server_ttynum);
	    char *pbuf;

	    for (pbuf = req->buffer; *pbuf; pbuf++) {
		putc(tty, *pbuf);
	    }

	    startoutput(tty);
	    rsp->return_code = 0;
	    ipc_send_rpc_reply(req_msg, rsp_msg);
	}
	break;
    }
}

static void RTTYS_reg_handler_port (void)
{
    ipc_port_id handler_port;
    ipc_error_code retcode;

    /*
     * By now the port may have been renumber,
     * so we need to get it again by the name.
     */
    handler_port = ipc_locate_port(RTTY_ipc_port_name);
    
    retcode = ipc_register_port(handler_port);
    if (retcode != IPC_OK) {
	/* post an error */
	;
    }
}

int RTTYS_init (boolean master)
{
    ipc_error_code ec;
    ipc_port_id handler_port;

    if (!master) {
	RTTYC_init(TRUE);
	return 0;
    }

    /*
     * Create the server port and set the server call-back function to
     * handle messages that arrive.
     */

    RTTY_ipc_port_name = strdup(RTTY_SERVER_MASTER_PORT_NAME);
    if (RTTY_ipc_port_name == NULL)
	return -1;
    ec = ipc_create_named_port(RTTY_ipc_port_name, &handler_port, 0);
    if (ec != IPC_OK) {
	/* post an error */
	return -1;
    } else {
	ec = ipc_set_receive_callback(handler_port, 0, RTTYS_handler);
    }

    /*
     * Can not register our handler port right now as IPC
     * may have not been initialized yet. Register an init
     * function for later invocation.
     */
    ipc_register_delayed_func(IPC_LEVEL_FILESYS, RTTYS_reg_handler_port);

    return 0;
}

