/* $Id: tacacs_plus.h,v 3.2.62.1 1996/04/19 15:29:40 che Exp $
 * $Source: /release/112/cvs/Xsys/servers/tacacs_plus.h,v $
 *------------------------------------------------------------------
 * tacacs_plus.h -- header file for TACACS+ subsystem
 *
 * September 1994, Dave Carrel
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tacacs_plus.h,v $
 * Revision 3.2.62.1  1996/04/19  15:29:40  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.1.6.3  1996/04/02  19:32:49  carrel
 * cvs fixes
 *
 * Revision 2.1.6.2  1996/02/28  06:15:12  lol
 * Branch: IosSec_branch
 * TACACS+ processing.
 *
 * Revision 2.1.6.1  1995/11/17  17:39:39  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.2  1995/11/17  18:51:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:12:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/18  00:56:56  billw
 * CSCdi36940:  AAA should keep cumulative statistics
 * This patch does accounting stats only...
 *
 * Revision 2.1  1995/06/07  22:44:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *void tacacs_plus_init(void);
 *void tacacs_plus_closeconn(void *handle);
 *boolean tacacs_plus_start_login(userstruct *ustruct, authenstruct *astruct,
 *				int action);
 *boolean tacacs_plus_cont_login(userstruct *ustruct, authenstruct *astruct);
 */

#define TAC_PLUS_MAJOR_VER	0xc0
#define TAC_PLUS_MAJOR_VER_MASK	0xf0

#define TAC_PLUS_MINOR_VER_0	0x00
#define TAC_PLUS_MINOR_VER_1	0x01
#define TAC_PLUS_MINOR_VER_CUR	TAC_PLUS_MINOR_VER_1
#define TAC_PLUS_MINOR_VER_MASK	0x0f

#define TAC_PLUS_VER		(TAC_PLUS_MAJOR_VER | TAC_PLUS_MINOR_VER_CUR)
#define TAC_PLUS_VER_0		(TAC_PLUS_MAJOR_VER | TAC_PLUS_MINOR_VER_0)


#define TAC_PLUS_GET_MINOR_PAK_VER(p) (((p)->version)&(TAC_PLUS_MINOR_VER_MASK))

/* Number of times to restart a TCP read if data appears corrupt */
#define TAC_PLUS_NUM_RESTARTS		3

/*
 * WARNING/NOTE: Many of the enums below are part of external specifications.
 * DO NOT change values unless you know what you are doing and you consult
 * the specs.
 */

enum TACACS_PLUS_PAK_TYPES {
    TAC_PLUS_AUTHEN	= 0x01,
    TAC_PLUS_AUTHOR	= 0x02,
    TAC_PLUS_ACCT	= 0x03
};

/*
 * WARNING/NOTE: This enum is matched to the AAA AUTHEN_STATUS enum
 * in h/aaa.h
 */
enum TACACS_PLUS_STATUS {
    TAC_PLUS_STATUS_PASS	= AAA_AUTHEN_STATUS_PASS,
    TAC_PLUS_STATUS_FAIL	= AAA_AUTHEN_STATUS_FAIL,
    TAC_PLUS_STATUS_GETDATA	= AAA_AUTHEN_STATUS_GETDATA,
    TAC_PLUS_STATUS_GETUSER	= AAA_AUTHEN_STATUS_GETUSER,
    TAC_PLUS_STATUS_GETPASS	= AAA_AUTHEN_STATUS_GETPASS,
    TAC_PLUS_STATUS_RESTART	= AAA_AUTHEN_STATUS_RESTART,
    TAC_PLUS_STATUS_ERROR	= AAA_AUTHEN_STATUS_ERROR,
    TAC_PLUS_STATUS_FOLLOW	= 0x21	/* Not matched to AAA */
};

enum TACACS_PLUS_ENCRYPTION {
    TAC_PLUS_ENCRYPTED		= 0x00,
    TAC_PLUS_CLEAR		= 0x01
};

typedef struct tacacs_plus_pkt_hdr_ {
    uchar version;
    uchar type;
    uchar seq_no;
    uchar encrypted;
    int session_id;
    int length;
    /* followed by "length" bytes of encrypted data */
} tacacs_plus_pkt_hdr;
#define TACACS_PLUS_HDR_SIZ 12

typedef struct authen_start_body_ {
    uchar action;
    uchar priv_lvl;
    uchar authen_type;
    uchar service;
    uchar user_len;
    uchar port_len;
    uchar rem_addr_len;
    uchar data_len;
} authen_start_body;
#define AUTHEN_START_BODY_SIZ 8

typedef struct authen_cont_body_ {
    ushort msg_len;
    ushort data_len;
    uchar flags;
} authen_cont_body;
#define AUTHEN_CONT_BODY_SIZ 5

#define TAC_PLUS_CONT_FLAG_ABORT	0x01

typedef struct authen_response_body_ {
    uchar status;
    uchar flags;
    ushort msg_len;
    ushort data_len;
} authen_response_body;
#define AUTHEN_RESPONSE_BODY_SIZ 6

#define TAC_PLUS_RESP_FLAG_NOECHO	AAA_RESP_FLAG_NOECHO

typedef struct author_request_body_ {
    uchar authen_method;
    uchar priv_lvl;
    uchar authen_type;
    uchar service;
    uchar user_len;
    uchar port_len;
    uchar rem_addr_len;
    uchar arg_cnt;
} author_request_body;
#define AUTHOR_REQUEST_BODY_SIZ 8

typedef struct author_response_body_ {
    uchar status;
    uchar arg_cnt;
    ushort msg_len;
    ushort data_len;
} author_response_body;
#define AUTHOR_RESPONSE_BODY_SIZ 6

/*
 * WARNING/NOTE: This enum is matched to the AAA AUTHOR_STATUS enum
 * in h/aaa.h
 */
enum TACACS_PLUS_AUTHOR_STATUS {
    TAC_PLUS_AUTHOR_STATUS_PASS_ADD	= AAA_AUTHOR_STATUS_PASS_ADD,
    TAC_PLUS_AUTHOR_STATUS_PASS_REPL	= AAA_AUTHOR_STATUS_PASS_REPL,
    TAC_PLUS_AUTHOR_STATUS_FAIL		= AAA_AUTHOR_STATUS_FAIL,
    TAC_PLUS_AUTHOR_STATUS_ERROR	= AAA_AUTHOR_STATUS_ERROR,
    TAC_PLUS_AUTHOR_STATUS_FOLLOW	= 0x21 /* Not matched to AAA */
};

typedef struct acct_request_body_ {
    uchar flags;
    uchar authen_method;
    uchar priv_lvl;
    uchar authen_type;
    uchar service;
    uchar user_len;
    uchar port_len;
    uchar rem_addr_len;
    uchar arg_cnt;
} acct_request_body;
#define ACCT_REQUEST_BODY_SIZ 9

typedef struct acct_response_body_ {
    ushort msg_len;
    ushort data_len;
    uchar status;
} acct_response_body;
#define ACCT_RESPONSE_BODY_SIZ 5

/*
 * WARNING/NOTE: This enum is matched to the AAA ACCT_STATUS enum
 * in h/aaa.h
 */
enum TACACS_PLUS_ACCT_STATUS {
    TAC_PLUS_ACCT_STATUS_SUCCESS	= AAA_ACCT_STATUS_SUCCESS,
    TAC_PLUS_ACCT_STATUS_ERROR		= AAA_ACCT_STATUS_ERROR,
    TAC_PLUS_ACCT_STATUS_FOLLOW		= 0x21 /* Not matched to AAA */
};


/*
 * External function prototypes.
 */
void tacacs_plus_command(parseinfo *);

enum SERVER_TYPE {
    STAY_OPEN_SERVER,
    STAY_CLOSED_SERVER
};

extern char *tacacs_plus_key;

extern void
close_handle (void **handle);

extern boolean
open_handle (addrtype *addr, void **handle);

extern char *
get_server_key (addrtype *servaddr);

extern int
get_server_port (addrtype *servaddr);

extern int
get_server_timeout (addrtype *servaddr);

extern boolean
stay_open_server(addrtype *addr, void *handle);

extern tacacs_plus_pkt_hdr *
copy_tac_plus_packet (tacacs_plus_pkt_hdr *pak);

extern boolean
tacacs_plus_proc_send_receive (tcbtype *tcb,
			       tacacs_plus_pkt_hdr *out, 
			       tacacs_plus_pkt_hdr **in,
			       addrtype *addr,
			       uint reply);

extern boolean
tacacs_plus_encrypt(addrtype *server, tacacs_plus_pkt_hdr *tac_pak);

#define TAC_PLUS_EXPECT_REPLY 1
#define TAC_PLUS_EXPECT_NO_REPLY 2
