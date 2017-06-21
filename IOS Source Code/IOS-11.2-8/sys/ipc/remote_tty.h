/* $Id$
 * $Source$
 *------------------------------------------------------------------
 * Include file used for remote tty master-slave interface
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

/*----------------------------------------------------------------------------*
 * A bunch of useful defines that are needed for the client/server stuff
 *----------------------------------------------------------------------------*/
#define RTTY_OUTPUT_MAX			2048
#define RTTYC_INPUT_MAX			RTTY_OUTPUT_MAX

#define RTTY_SERVER_MASTER_PORT_NAME	"Remote TTY Server Port"
#define RTTY_CLIENT_SLAVE_PORT_NAME	"Slave RSP: Remote TTY Client Port"

#define OUTPUT_TIMEOUT			1
 
/*----------------------------------------------------------------------------*
 * Definitions of the message type for the remote tty
 * client/server interface.
 *----------------------------------------------------------------------------*/
typedef enum rtty_msg_type_ {
    RTTY_OUTPUT_REQ,
    RTTYC_ATTACH_REQ,
    RTTYC_DETACH_REQ,
    RTTYC_INPUT_REQ,
 
    RTTY_OUTPUT_RSP,
    RTTYC_ATTACH_RSP,
    RTTYC_DETACH_RSP,
    RTTYC_INPUT_RSP
} rtty_msg_type;

typedef enum rtty_error_type_ {
    RTTY_NO_ERROR = 0,
    RTTY_BOGUS_REMOTE_TTYNUM,
    RTTY_ALREADY_ATTACHED,
    RTTY_NOT_ATTACHED
} rtty_error_type;

/*----------------------------------------------------------------------------*
 * Definitions for IPC messages used in remote tty
 *
 * These are paired sets of request/response message types.  The
 * requests end with "_req", and the responses end with "_rsp".
 *
 *----------------------------------------------------------------------------*/

typedef struct rtty_output_req_ {
    int server_ttynum;
    char buffer[RTTY_OUTPUT_MAX];
} rtty_output_req_t;
 
typedef struct rtty_output_rsp_ {
    int return_code;
} rtty_output_rsp_t;

/* Client handler */

typedef struct rttyc_attach_req_ {
    int remote_ttynum;
    ipc_port_id server_port;
    int server_ttynum;
} rttyc_attach_req_t;
 
typedef struct rttyc_attach_rsp_ {
    int return_code;
} rttyc_attach_rsp_t;

typedef struct rttyc_detach_req_ {
    int remote_ttynum;
} rttyc_detach_req_t;
 
typedef struct rttyc_detach_rsp_ {
    int return_code;
} rttyc_detach_rsp_t;

typedef struct rttyc_input_req_ {
    int remote_ttynum;
    char buffer[RTTYC_INPUT_MAX];
} rttyc_input_req_t;
 
typedef struct rttyc_input_rsp_ {
    int return_code;
} rttyc_input_rsp_t;

int RTTYS_init (boolean master);

int RTTYC_init (boolean is_slave_RSP);
void RTTYC_reassign_tty (int server_ttynum);
void RTTYC_restore_tty (void);
void RTTYC_putc (tt_soc *tty, int c);
boolean RTTYC_startoutput (tt_soc *tty);

static inline tt_soc *number2tt (int line)
{
    if (line < 0 || line >= PLATFORM_MAXLINES)
	return console;
    return MODEMS[line] ? MODEMS[line] : console;
}

