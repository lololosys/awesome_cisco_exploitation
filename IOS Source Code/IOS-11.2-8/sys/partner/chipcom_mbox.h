/* $Id: chipcom_mbox.h,v 3.2.62.2 1996/08/28 13:05:19 thille Exp $
 * $Source: /release/112/cvs/Xsys/partner/chipcom_mbox.h,v $
 *------------------------------------------------------------------
 * chipcom_mbox.h -- Chipcom Mailbox Generic Header File
 *
 * July 1994, John G. Kuzma
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * The following file contains those common constructs used to implement the
 * Chipcom Mailbox Protocol, as documented in Chipcom P/N 97-00033.
 *------------------------------------------------------------------
 * $Log: chipcom_mbox.h,v $
 * Revision 3.2.62.2  1996/08/28  13:05:19  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.62.1  1996/05/23  07:38:07  myeung
 * CSCdi58029:  OSPF crash in ospf_interface_up() when unconfiguring OSPF
 * Branch: California_branch
 * - Add boolean parameter to ip_address_change registry to indicate if
 *   the change is for secondary or primary address.
 *
 * Revision 3.2  1995/11/17  18:53:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:57:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/12  22:27:24  gstovall
 * CSCdi36994:  Partner code needs to be pulled out of core subsystem
 * Move the parse chain init from the c3000 specific file to a partner
 * specific file.  Make "debug mailbox" work on the 4500 in the process.
 *
 * Revision 2.1  1995/06/07  22:32:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#define CHIPCOM_MAJVERSION      1
#define CHIPCOM_MINVERSION      0
#define CHIPCOM_EDITVERSION     1

#define MAX_MAILBOX_PDU_SIZE   256     /* Buffer size for outgoing messages */

#define MAILBOX_STATE__RESET    -1     /* Not yet initialized.              */
#define MAILBOX_STATE__OFFLINE   0     /* Something went wrong. Mbox off.   */
#define MAILBOX_STATE__ONLINE    1     /* Mbox has something in it.         */

#define MAILBOX_INIT_TIMEOUT        5  /* In seconds.                       */
#define MAILBOX_ECHO_TIMEOUT        5  /* In seconds.                       */
#define MAILBOX_INTEROCTET_TIMEOUT  1  /* In seconds.                       */
#define MAILBOX_INIT_DELAY        100  /* In milliseconds.                  */
#define MAILBOX_INTEROCTET_DELAY  100  /* In milliseconds.                  */

#define MAILBOX_QUEUE_SIZE            2048

#define MAILBOX_QUIESCE_FROM_ELSEWHERE   0
#define MAILBOX_QUIESCE_FROM_SEND_PROC   1
#define MAILBOX_QUIESCE_FROM_RECV_PROC   2

#define GET_MBOX_CHAR(word)         ((word & 0xff00) >> 8)
#define BUILD_MBOX_CHAR(word)       ((word & 0x00ff) << 8)

#define PRINTINT(v)                 printf("%-32s = %8d\n",#v,v)
#define PRINTLONG(v)                printf("%-32s = %8ld\n",#v,v)
#define PRINTCHAR(v)                printf("%-32s = %8c\n",#v,v)
#define PRINTBOOL(v)                printf("%-32s = %8s\n",#v,v?"TRUE":"FALSE")


/*
 * Chipcom specific constants:
 */
#define MAILBOX_MODULE_DESCR_SIZE     16
#define MAILBOX_MODULE_NUM_SIZE       16
#define MAILBOX_MODULE_NUM_EXT_SIZE    4
#define MAILBOX_SW_VERSION_SIZE        4
#define MAILBOX_PDU_HEADER_BYTES       3

#define MAILBOX_PDU__ECHO_RQST      1  /* ECHO_REQUEST                      */
#define MAILBOX_PDU__ECHO_RESP      2  /* ECHO_RESPONSE                     */
#define MAILBOX_PDU__PERSONALITY_N  3  /* PERSONALITY_NOTIFICATION          */
#define MAILBOX_PDU__OP_STATUS_N    4  /* OPERATIONAL_STATUS_NOTIFICATION   */
#define MAILBOX_PDU__ADMIN_STATE_N  5  /* ADMINISTRATIVE_STATE_NOTIFICATION */
#define MAILBOX_PDU__NET_CONFIG_N   6  /* NETWORK_CONFIG_NOTIFICATION       */
#define MAILBOX_PDU__SPEED_CONFIG_N 7  /* SPEED_CONFIG_NOTIFICATION         */
#define MAILBOX_PDU__IP_CONFIG_N    8  /* IP_CONFIG_NOTIFICATION            */
#define MAILBOX_PDU__MAC_CONFIG_N   9  /* MAC_CONFIG_NOTIFICATION           */
#define MAILBOX_PDU__RMT_CMD_RQST  10  /* REMOTE_COMMAND_REQUEST            */
#define MAILBOX_PDU__RMT_CMD_RESP  11  /* REMOTE_COMMAND_RESPONSE           */
#define MAILBOX_PDU__DATE_TIME_N   12  /* DATE_TIME_NOTIFICATION            */
#define MAILBOX_PDU__FAST_RESET_N  13  /* FAST_RESET_NOTIFICATION           */
#define MAILBOX_PDU__IP_GATEWAY_N  14  /* IP_GATEWAY_NOTIFICATION           */

#define MAILBOX_INIT_ECHO_RQST     "\125\252"    /* 0x55aa */
#define MAILBOX_INIT_ECHO_RESP     "\252\125"    /* 0xaa55 */

/* These are all bitmasks. */
#define MAILBOX_CAP__OP_STATUS_N    0x0001
#define MAILBOX_CAP__ADMIN_STATE_N  0x0002
#define MAILBOX_CAP__NET_CONFIG_N   0x0004
#define MAILBOX_CAP__SPEED_CONFIG_N 0x0008
#define MAILBOX_CAP__RMT_CMD_EXEC   0x0010
#define MAILBOX_CAP__MPU_HRTBT_CHK  0x0020
#define MAILBOX_CAP__MAC_CONFIG_N   0x0040
#define MAILBOX_CAP__IP_CONFIG_N    0x0080
#define MAILBOX_CAP__IP_GATEWAY_N   0x0100
#define MAILBOX_CAP__DATE_TIME_N    0x0200
#define MAILBOX_CAP__FAST_RESET_N   0x0400

#define OP_STATUS__NORMAL             0x00
#define OP_STATUS__TRANSIENT_ERR      0x01
#define OP_STATUS__FATAL_ERR          0x02

#define ADMIN_STATE__NORMAL           0x00

#define PORT_CAP__FB_SWITCH           0x01  /* This is a bitmask! */

#define PORT_OP_STATUS__NORMAL        0x00
#define PORT_OP_STATUS__TRANSIENT_ERR 0x01
#define PORT_OP_STATUS__FATAL_ERR     0x02
#define PORT_OP_STATUS__MAC_OPEN      0x10
#define PORT_OP_STATUS__WIRE_FAULT    0x11
#define PORT_OP_STATUS__BEACON        0x12
#define PORT_OP_STATUS__PORT_UNAVAIL  0x13
#define PORT_OP_STATUS__FATAL         0x14
#define PORT_OP_STATUS__INVALID_CONF  0x15
#define PORT_OP_STATUS__MAC_REMOVED   0x16

#define PORT_ADMIN_STATE__DISABLE     0x00
#define PORT_ADMIN_STATE__ENABLE      0x01

#define PORT_NETWORK_TYPE__UNSPEC     0x00
#define PORT_NETWORK_TYPE__ETHERNET   0x10
#define PORT_NETWORK_TYPE__TOKEN_RING 0x20
#define PORT_NETWORK_TYPE__FDDI       0x30
#define PORT_NETWORK_TYPE__ATM        0x40
#define PORT_NETWORK_TYPE__WAN        0x50

#define PORT_CONN_TYPE__UNSPEC        0x00
#define PORT_CONN_TYPE__BACKPLANE     0x01
#define PORT_CONN_TYPE__RJ_UTP        0x02
#define PORT_CONN_TYPE__RJ_STP        0x03
#define PORT_CONN_TYPE__FEMALE_AUI    0x04
#define PORT_CONN_TYPE__BNC           0x05
#define PORT_CONN_TYPE__FIBER_ST      0x06
#define PORT_CONN_TYPE__FIBER_SMA     0x07
#define PORT_CONN_TYPE__FIBER_SC_MIC  0x08
#define PORT_CONN_TYPE__FIBER_ST_MIC  0x09
#define PORT_CONN_TYPE__V35           0x0A
#define PORT_CONN_TYPE__X21           0x0B
#define PORT_CONN_TYPE__DB9           0x0C
#define PORT_CONN_TYPE__DB25          0x0D
#define PORT_CONN_TYPE__GEN_SERIAL    0x0E
#define PORT_CONN_TYPE__MALE_AUI      0x0F
#define PORT_CONN_TYPE__DB50          0x10
#define PORT_CONN_TYPE__TELCO         0x11
#define PORT_CONN_TYPE__VIRTUAL       0x12
#define PORT_CONN_TYPE__FIBER_SC      0x13
#define PORT_CONN_TYPE__DB37          0x14
#define PORT_CONN_TYPE__SNGL_MD_FDDI  0x15
#define PORT_CONN_TYPE__MULT_MD_FDDI  0x16
#define PORT_CONN_TYPE__DIN8          0x17
#define PORT_CONN_TYPE__DIN9          0x18

#define PORT_SPEED__UNSPEC            0x00
#define PORT_SPEED__T1_E1             0x01
#define PORT_SPEED__56Kbps            0x02
#define PORT_SPEED__19_2Kbps          0x03
#define PORT_SPEED__4Mbps             0x04
#define PORT_SPEED__T2_E2             0x05
#define PORT_SPEED__10Mbps            0x06
#define PORT_SPEED__16Mbps            0x07
#define PORT_SPEED__25Mbps            0x08
#define PORT_SPEED__45Mbps            0x09
#define PORT_SPEED__51Mbps            0x0A
#define PORT_SPEED__100Mbps           0x0B
#define PORT_SPEED__155Mbps           0x0C
#define PORT_SPEED__255Mbps           0x0D
#define PORT_SPEED__622Mbps           0x0E
#define PORT_SPEED__1Gbps             0x0F
#define PORT_SPEED__2Gbps             0x10
#define PORT_SPEED__16Mbps_4Mbps      0x11


/* 
 * The following structure is used to make building the PERSONALITY PDU easier.
 */
struct mbox_personality_PDU {
    uchar li;                                          /* 0x0000 */
    uchar mc;                                          /* 0x0001 */
    char module_descr[MAILBOX_MODULE_DESCR_SIZE];      /* 0x0002 */
    char module_num[MAILBOX_MODULE_NUM_SIZE];          /* 0x0012 */
    char module_num_ext[MAILBOX_MODULE_NUM_EXT_SIZE];  /* 0x0022 */
    ushort module_identifier;                          /* 0x0026 */
    uchar module_vendor;                               /* 0x0028 */
    uchar module_config;                               /* 0x0029 */
    char sw_version[MAILBOX_SW_VERSION_SIZE];          /* 0x002a */
    ushort module_capabilities;                        /* 0x002e */
    uchar op_status;                                   /* 0x0030 */
    uchar admin_state;                                 /* 0x0031 */
    uchar num_complex_ports;                           /* 0x0032 */
    uchar num_simple_ports;                            /* 0x0033 */
    long ip_dflt_gateway;                              /* 0x0034 */
    uchar port_info;                                   /* 0x0035 */
};

/* 
 * The following structure is used to make building the PERSONALITY PDU easier.
 */
struct mbox_complex_port_info {
    uchar port_capabilities;
    uchar op_status;
    uchar admin_state;
    uchar network_type;
    uchar conn_type;
    uchar speed;
    uchar mac_addr[IEEEBYTES];
    long ip_address;
    long ip_subnet_mask;
};


/*
 * Simple character queue "header". 
 */
typedef struct {
    uint  size;
    char  full;
    uchar *toq;
    uchar *boq;
    uchar *data;
} char_queue_t;


/*
 * Global variables.
 */
extern int outgoing_mbox_count;
extern int incoming_mbox_state;
extern int outgoing_mbox_state;
extern char_queue_t *incoming_mbox_queue;
extern char_queue_t *outgoing_mbox_queue;

/*
 * Function prototypes.
 */
#define memcpy(dest,src,n)      bcopy(src,dest,n)

extern void mailbox_v2_early_init_common(void);
extern void mailbox_v2_init_common(ushort mbox_capabilities);
extern void mailbox_v2_outbox_hndlr(void);
extern void mailbox_v2_inbox_hndlr(void);
extern void mailbox_v2_quiesce(int);

extern char_queue_t  *cqcreate(uint size);
extern inline uchar   cdequeue(char_queue_t *q);
extern inline void    cenqueue(char_queue_t *q,uchar c);
extern inline boolean cqempty(char_queue_t *q);
extern void           cenqueue_PDU(char_queue_t *q,uchar *s);

extern int   mailbox_v2_validate_echo_resp(uchar *p, int len);

extern void  mailbox_v2_update_ip(idbtype *idb,ipaddrtype addr,ipaddrtype mask,
                                  boolean secondary, boolean adding);
extern void  mailbox_v2_update_mac(hwidbtype *idb);
extern void  mailbox_v2_update_ip_defaultgw (ipaddrtype gw);
extern void  mailbox_v2_process_PDU(uchar *buffer);
extern void  mailbox_v2_build_PDU(int pdu_type, uchar *data, int len,
                                  ushort flags);
extern void  show_mailbox_state(void);
extern uchar mailbox_v2_compute_checksum(uchar *msg, int len);

/*
 * external calls
 */
extern void chipcom_subsys_init(subsystype *subsys);
extern void mailbox_v2_init(void);
extern void mailbox_v2_start_output(ushort c);
extern void mailbox_v2_fence_interface(void);
extern uchar *mailbox_v2_fill_personality_PDU (struct mbox_personality_PDU *p,
                                               ushort flags);
extern inline boolean mailbox_v2_is_complex_port (hwidbtype *idb);

extern void chipcom_mbox_parser_init(void);

