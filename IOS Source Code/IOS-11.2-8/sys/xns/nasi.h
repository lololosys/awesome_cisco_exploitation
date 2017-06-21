/* $Id: nasi.h,v 3.2.62.1 1996/06/07 01:17:41 irfan Exp $
 * $Source: /release/112/cvs/Xsys/xns/nasi.h,v $
 *------------------------------------------------------------------
 * NASI.h Netware Asynchronous Services Interface (Header include)
 *
 * Sept 1995, Syed Mohammad Irfan Ashraf
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nasi.h,v $
 * Revision 3.2.62.1  1996/06/07  01:17:41  irfan
 * CSCdi55494:  NASI server does not work after reload
 * Branch: California_branch
 * Don't reopen NASI's SPX socket if already opened.
 *
 * Revision 3.2  1995/11/17  19:22:20  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:58:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:38:46  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/26  12:13:03  irfan
 * Netware Asynchronous Services Interface: holding area
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __NASI_H__
#define __NASI_H__
#

/*
 * Some constants
 */

#define NASI_SERVICE_SOCKET  NOVELL_SOCK_NCS_SRV  /* from novell.h */
#define NASI_SERVICE_NAME_LEN 60  /* Pvt. service name array size */
#define NASI_MAX_HOSTNAME_LEN (NASI_SERVICE_NAME_LEN - 5)
#define NASI_PORT_NAME_MAXLEN 60  /* Pvt. port name array size */
#define NASI_CMD_START 1

#define NASI_PER_CON_PAK_COUNT 10  /* # of pkts to process per wake-up */
#define NASI_IO_BUFSIZE        400 /* Buf size for NASI tty I/O */
#define NASI_MAX_SEND          250 /* Max size that we write to net */

#define NASI_SRVSTRING_LEN    8    /* Server name size */
#define NASI_USTRING_LEN     16    /* User string size */
#define NASI_PWSTRING_LEN    25    /* PW string size */
#define NASI_GRPSTRING_SIZE   8    /* port group-name string size */
#define NASI_PORTSTRING_SIZE 14    /* port specific-name string size */
#define NASI_LONGSTRING_SIZE 48    /* some long strings */

#define NASI_PW_HASH_BYTE    0xAB  /* PW hash magic key */


/*
 * NASI wildcard strings
 */
#define NASI_ANYNAME8  "????????"
#define NASI_ANYNAME12 "????????????"

/*
 * NASI special characters
 */
#define NASI_FILLER_CHAR '_'
#define NASI_ANY_CHAR '?'
#define NASI_NULL_CHAR '\0'


/*
 * NASI speed index table
 */
#define NASI_MIN_TTY_SPEED     0
#define NASI_MAX_TTY_SPEED     115200
#define NASI_MAX_SPEED_INDEX   19

/*
 * NASI word length index table
 */
#define NASI_MIN_TTY_WORDLEN   5
#define NASI_MAX_TTY_WORDLEN   8
#define NASI_MAX_WORDLEN_INDEX 4

/*
 * NASI stopbits index table
 */
#define NASI_MIN_TTY_STOPBITS    STOPBITS1
#define NASI_MAX_TTY_STOPBITS    STOPBITS2
#define NASI_MAX_STOPBITS_INDEX  3

/*
 * NASI parity index table
 */
#define NASI_MIN_TTY_PARITY   PARITY_NO
#define NASI_MAX_TTY_PARITY   PARITY_MARK   /* internally mark > space */
#define NASI_MAX_PARITY_INDEX 5

/*
 * NASI flow types
 */
#define NASI_FLOW_NONE  0
#define NASI_FLOW_SOFT  1
#define NASI_FLOW_HARD  2
#define NASI_FLOW_BOTH  3     /* We don't do this */

/*
 * NASI port status
 */
#define NASI_PORT_STATUS_IDLE      0   /* available to any application */
#define NASI_PORT_STATUS_ALLOCATED 1   /* allocated (to your application */
#define NASI_PORT_STATUS_CONNECTED 2   /* connected */
#define NASI_PORT_STATUS_ONHOLD    3   /* on hold */

/*
 * Maximum Nasi reads/writes at a time
 */
#define NASI_READ_SIZE 240

/*
 * NASI will wait this much b/w the 2 responses for init_port
 */
#define NASI_INITPORT_DELAY (ONESEC/50)

/*
 * NASI will wait this much before closing connection
 */
#define NASI_CLOSECON_DELAY (ONESEC/2)

/*
 * csb->which values for "show nasi" top level exec command options
 */
enum NASI_SHOW_TYPES {
    SHOW_NASI_CON,
    SHOW_NASI_ALL
};

/*
 * csb->which values for "nasi" line sub-command options
 */
enum NASI_LINE_COMMAND_TYPES {
    LINE_NASI_AUTH,
    LINE_NASI_ENABLE
};

/*
 * bits in NASI flags field in NASI data structure per connection
 */
#define NASI_FLAG_NEED_CLOSE 0x00000001
#define NASI_FLAG_NEED_FLUSH 0x00000002

/*
 * bits in NASI flags word for the NASI process
 * kept in nasi_status_flags
 */
#define NASI_MAIN_RUNNING  0x000000001
#define NASI_MAIN_EXIT     0x000000002


/*
 * Identified NASI packet types
 */
#define NASI_OP_SRV_DISCONNECT    0x4200  /* Server disconnected */
#define NASI_OP_SRV_DATA          0x4300  /* Read function - on remote */
#define NASI_OP_SRV_DATA_GOOD_ACK 0x4481  /* some kind of good ack */
#define NASI_OP_SRV_DATA_BAD_ACK  0x44C1  /* some kind of bad ack */
#define NASI_OP_QNAME_FFIRST      0x2100  /* NasiQueryNameService - FFirst */
#define NASI_OP_QNAME_FNEXT       0x2101  /* NasiQueryNameService - FNext */ 
#define NASI_OP_CHECK_SERVER      0x4F00  /* ??? Not sure */
#define NASI_OP_INFO_SERVER       0x4F01  /* ??? Not much clue */
#define NASI_OP_SET_SERVICE_NAME  0x1300  /* Set Service name */
#define NASI_OP_GET_SERVICE_NAME  0x1301  /* Get Service name */
#define NASI_OP_GET_SERVER_NAME   0x1302  /* Get Server name */
#define NASI_OP_GET_GENERAL_NAME  0x1303  /* Get General name */
#define NASI_OP_GET_SPECIFIC_NAME 0x1304  /* Get Specific name */
#define NASI_OP_INITIALIZE_PORT   0x1600  /* NasiInitialize - on remote */
#define NASI_OP_RELEASE_PORT      0x1700  /* NasiClose ?? */
#define NASI_OP_CLIENT_DATA       0x1800  /* Write function - on remote */
#define NASI_OP_FLUSH_RXBUF       0x1c00  /* Flush Rx Buffes */
#define NASI_OP_FLUSH_TXBUF       0x1d00  /* Flush Tx Buffes */
#define NASI_OP_BREAK_OFF         0x1e00  /* Stop sending break */
#define NASI_OP_BREAK_ON          0x1e02  /* Start sending break */
#define NASI_OP_DISCONNECT        0x1e04  /* Disconnect req from client */
#define NASI_OP_HOLD              0x1e06  /* Put port on hold */
#define NASI_OP_SET_SFLOW         0x1e08  /* Enable SW flow on port */
#define NASI_OP_RESET_SFLOW       0x1e0a  /* Disable SW flow on port */
#define NASI_OP_DEFINE_SFLOW      0x1e0c  /* Define XON, XOFF characters */

/*
 * UnIdentified NASI packet types
 */
#define NASI_OP_UNKNOWN_1         0x4500  /* ??? */

/*
 * Some structures
 */

/*
 * NASI query port name packet type
 */
#define nasi_foo4 \
    ushort code_subcode;\
    ushort ret_ax;\
    ushort ret_cx;\
    ushort ret_flags;\
    ushort ret_word1;\
    ushort ret_word2;\
    uchar zero1;\
    uchar srv_name[NASI_SRVSTRING_LEN + 1];\
    uchar p_name[NASI_PORTSTRING_SIZE];\
    uchar unused1;\
    uchar g_name[NASI_GRPSTRING_SIZE];\
    uchar zero;\
    uchar port_num;\
    uchar port_status;\
    uchar inc_byte;\
    uchar port_params[11];\
    uchar unused2;\
    uchar zero2[NASI_LONGSTRING_SIZE];\
    uchar sess_srv_name[NASI_LONGSTRING_SIZE];\
    uchar sess_cl_addr[NASI_LONGSTRING_SIZE];\
    uchar sess_cl_user[NASI_LONGSTRING_SIZE+1];\
    uchar sess_cl_pw_hash[NASI_PWSTRING_LEN];\
    uchar sess_cl_user2[NASI_USTRING_LEN];\
    uchar unused3;\
    uchar word_field3[2];\
    uchar word_field4[2];

struct nasi_query_port_struct_ {
    nasi_foo4
};

typedef struct nasi_query_port_struct_ nasi_qport_hdr;
#define sizeofnasi_qport_hdr 300

/*
 * NASI port initialization request packet type
 */
#define nasi_foo5 \
    ushort code_subcode;\
    ushort ret_ax;\
    ushort ret_cx;\
    ushort ret_flags;\
    ushort ret_word1;\
    ushort ret_word2;\
    ushort ret_word3;\
    uchar zero1;\
    uchar port_num;\
    uchar port_status;\
    uchar port_params[11];\
    uchar unused1;\
    uchar zero3[96];\
    uchar sess_cl_addr[NASI_LONGSTRING_SIZE];\
    uchar sess_srv_name[NASI_SRVSTRING_LEN];\
    uchar unused2;\
    uchar p_name[NASI_PORTSTRING_SIZE];\
    uchar unused3;\
    uchar g_name[NASI_GRPSTRING_SIZE];\
    uchar unused4;\
    uchar zero4;\
    uchar field1;\
    uchar sess_uname1[NASI_LONGSTRING_SIZE];\
    uchar unused5;\
    uchar sess_pw_hash[NASI_PWSTRING_LEN];\
    uchar sess_uname2[NASI_USTRING_LEN];\
    uchar unused6;\
    uchar word_field_1[2];\
    uchar word_field_2[2];

struct nasi_init_port_struct_ {
    nasi_foo5
};

typedef struct nasi_init_port_struct_ nasi_preq;
#define sizeofnasi_preq_info 12  /* We will only overwrite this size */
#define sizeofnasi_preq_data 303

/*
 * This NASI packet is used for TTY data read/write
 */
#define nasi_foo3 \
    ushort code_subcode;\
    uchar size;\
    uchar zero1;\
    uchar field1;\
    uchar field2;\
    uchar field3;\
    uchar field4;\
    uchar zero2[2];\
    uchar field5;\
    uchar field6;\
    uchar zero3;\
    uchar data;

struct nasi_hdr3_ {
    nasi_foo3
};

typedef struct nasi_hdr3_ nasi_hdr3;
#define sizeofnasi_hdr3 13

/*
 * This NASI packet is used for exchanging server info
 */
#define nasi_foo1 \
    ushort code_subcode;\
    uchar err;\
    uchar zero1;\
    uchar field1;\
    uchar zero2;\
    uchar field2;\
    uchar field3;\
    uchar zero3;\
    uchar field4;\
    uchar field5;\
    uchar field6;\
    uchar zero4;\
    uchar in_srv_name[NASI_SRVSTRING_LEN + 1];\
    uchar out_srv_name[NASI_SRVSTRING_LEN + 1];\
    uchar snet[XNS_NETBYTES];\
    uchar shost[XNS_HOSTBYTES];\
    uchar s_sock[XNS_SOCKBYTES];

struct nasi_check_srv_struct_ {
    nasi_foo1
};

typedef struct nasi_check_srv_struct_ nasi_sinfo_hdr;
#define sizeofnasi_sinfo_hdr 43


/*
 * THis is how we maintain our local port structures.
 * these struct ptrs are passed b/w routines to
 * aggregate various fields in one.
 */
struct nasi_port_type {

    int num;
    uchar name[NASI_PORTSTRING_SIZE + 1];
    uchar group[NASI_GRPSTRING_SIZE + 1];
    uchar port_status;

    uchar nasi_rx_baud;
    uchar nasi_rx_wlen;
    uchar nasi_rx_stopbits;
    uchar nasi_rx_parity;

    uchar nasi_tx_baud;
    uchar nasi_tx_wlen;
    uchar nasi_tx_stopbits;
    uchar nasi_tx_parity;

    uchar nasi_dtr_state;
    uchar nasi_rts_state;
    uchar nasi_flow_type;

};

typedef struct nasi_port_type nasi_port;



/*
 * Some macros
 */

/*
 * External declarations
 */

/* nasi.c */
extern spxsocktype *nasi_my_spx_sock;
extern boolean nasi_configured;
extern void nasi_server_enable(void);
extern void nasi_server_disable(void);


/* nasi_chain.h */
extern void show_nasi(parseinfo *);
extern void nasi_line_subcommand(parseinfo *);
extern void nasi_setup_parser(void);

/*
 * that's all to it
 */

#endif __NASI_H__
