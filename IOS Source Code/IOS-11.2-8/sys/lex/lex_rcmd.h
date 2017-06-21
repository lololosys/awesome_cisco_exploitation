/* $Id: lex_rcmd.h,v 3.2.62.3 1996/08/28 13:00:07 thille Exp $
 * $Source: /release/112/cvs/Xsys/lex/lex_rcmd.h,v $
 *------------------------------------------------------------------
 * lex_rcmd.h -- LEX Remote Command definitions
 *
 * Feb. 1994, David Coli
 *
 * Copyright (c) 1994-1997 by Cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: lex_rcmd.h,v $
 * Revision 3.2.62.3  1996/08/28  13:00:07  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.62.2  1996/08/07  09:01:34  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/04/19  15:12:47  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.2.74.1  1996/04/10  23:06:58  lol
 * Branch: IosSec_branch
 *         Incorporate code review comments.
 *
 * Revision 3.2  1995/11/17  17:43:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:35:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:41:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __LEX_RCMD_H__
#define __LEX_RCMD_H__

#include "../os/boot.h" /* needed for TFTP_MAXDATA definition */
#include "logger.h"     /* needed for errmsg call */

#define LEX_RCMD_REQ      0x40
#define LEX_RCMD_ACK      0x41
#define LEX_RCMD_NAK      0x42
#define LEX_RCMD_REJ      0x43
#define LEX_RCMD_UNSOLICITED 0x44

#define PPP_BRD_CNTL_SIZE  2 /* The size of broadcast and control field FF03*/
#define PPP_TYPE_FIELD_SIZE     2 /* The size of type field in ppp header */

/*
 * LEX Remote Command Codes
 *
 * IMPORTANT:
 *     (1) These codes must be disjoint from any NCP command codes!
 *         See lex_rcmd_enqueue().
 *     (2) When adding a command, also add it to lex_commands[]
 *         character array.
 */

#define LEX_RCMD_START     1
#define LEX_PROTO_FILTER   1
#define LEX_ADDR_FILTER    2
#define LEX_SET_PRIORITY   3
#define LEX_SHUTDOWN       4
#define LEX_NOSHUTDOWN     5
#define LEX_RELOAD         6
#define LEX_STATS          7
#define LEX_DLOAD_WRQ      8
#define LEX_DLOAD_DATA     9
#define LEX_DLOAD_STATUS   10
#define LEX_INVENTORY      11
#define LEX_UNSOLICITED    12
#define LEX_RCMD_END       LEX_UNSOLICITED

#define LEX_DEFAULT_CMD_RETRY   10
#define LEX_DEFAULT_TIMEOUT     (2 * ONESEC)
/*
 * It takes FLEX approximately 6 seconds to compute the file checksum
 * and write the file to flash.  Use below timeout instead of the default
 * for LEX_DLOAD_STATUS messages, so we don't get any re-transmissions.
 */
#define LEX_FLASH_WRITE_TIME    (7 * ONESEC)
                             

/*
 * LEX Remote Command Lengths
 *
 * Note: The following commands just consist of the Lex header,
 *       so they are all LEX_RCMD_HDR_LEN long: LEX_SHUTDOWN, LEX_NOSHUTDOWN, 
 *       LEX_RELOAD.
 */

#define LEX_RCMD_HDR_LEN     (sizeof(struct lex_hdr) + sizeof(struct lex_option))
#define LEX_RCMD_OPTHDR_LEN  sizeof(struct lex_option)
#define LEX_INVENTORY_LEN    sizeof(lex_inventory_msg) - sizeof(ushort)

#define LEX_PRI_ENTRY_LEN    sizeof(struct lex_prty_entry)

#define LEX_DLOAD_STATUS_LEN  sizeof(struct lex_download_status)
#define LEX_BLOCK_NUM_SIZE    sizeof(ushort) /* size of block number field of */
                                    	     /* LEX_DLOAD_DATA message */

#define LEX_PFILTER_ENTRY_LEN sizeof(struct lex_proto_filter_entry)
#define LEX_AFILTER_ENTRY_LEN sizeof(struct lex_addr_filter_entry)

#define LEX_MAX_PROTO_FILTERS     200
#define LEX_MAX_ADDR_FILTERS      100
#define LEX_MAX_PRIORITY_ENTRIES  254

/*
 * Definitions for the "flags" field of the LEX_STATS message
 * option header.
 */
#define LEX_RESET_SERIAL_STATS 0x01
#define LEX_SERIAL_STATS       0x02
#define LEX_ETHER_STATS        0x04
#define LEX_RESET_ETHER_STATS  0x08
#define LEX_STATS_BOTH_INTS    (LEX_SERIAL_STATS | LEX_ETHER_STATS)
#define LEX_RESET_STATS        (LEX_RESET_SERIAL_STATS | LEX_RESET_ETHER_STATS)

/* 
 * this define is necessary because some real string space 
 * must be allocated in order to generate the remote interface name
 * for snmp and for the remote ethernet line
 * protocol messatges.
 */
#define MAX_LEX_IFDESCR_LENGTH 80

/*
 * Definitions for the "flags" field of the LEX_RELOAD message
 * option header.
 */
#define LEX_RELOAD_DEFAULT_IMAGE   0x00
#define LEX_RELOAD_PROM_IMAGE      0x01

typedef enum {
    LEX_ETHER_INTF,
    LEX_SERIAL_INTF
} lex_interface_type;


/*
 * lex_rcmdst_type
 * Remote command state structure that describes the state of a
 * remote command.  Lives on LEXrcmdQ.
 */
typedef struct lex_rcmdst_type_ {
    struct   lex_rcmdst_type_ *next;
    idbtype  *swidb;       /* sw IDB of Lex interface sending the rcmd */
    paktype  *pak;         /* Pointer to packet for re-transmission */
    timer_t  *retrans_tmr; /* Re-transmit pak if this timer expires */
    uchar    cmd_code;     /* Remote command function code */
    uchar    cmd_id;       /* Identifies response to original command */
    short    retry_count;  /* Number of times we have re-sent the rcmd */
    ushort   status;       /* Final status of the rcmd */
    boolean  status_posted;/* Indicates the status field has been set */
    boolean  delete_rcmd;  /* Should be deleted, not re-transmitted */
    boolean  process_waiting; /* Process is waiting on this rcmd */
    tt_soc   *tty;         /* On which tty the command is issued*/
} lex_rcmdst_type;

typedef struct lex_rcmd_stub_type_ {
    struct lex_rcmd_stub_t_ *next;
    idbtype *swidb;
    uchar   cmd_code;
} lex_rcmd_stub_type;

/* definitions for unsolicited status msg */
#define LEXSTATUS_ETHER_LINEPROTOCOL  0x1   /* set if remote ether is up */
#define LEXSTATUS_RESERVED_FOR_CISCO  0xfffe
#define LEXSTATUS_RESERVED_FOR_OTHERS 0xffff0000


/*
 *  LEX download definitions
 */
#define LEX_MAX_FILENM       64
#define LEX_MAX_HOSTNM       64
#define LEX_CONFIRM_BUF_SIZE 80
#define LEX_SEQ_ERR_RETRIES  10    /* Number of LEX_SEQ_ERROR until quiting */

struct lex_dload_args {
    idbtype *idb;
    uchar   *dload_buf;
    long    block_number;
};

/*
 * Definitions for lex_dload_version_args.status field
 */
#define LEX_IMAGE_OK           1
#define LEX_IMAGE_BAD          2
#define LEX_IMAGE_NOT_CHECKED  3
#define LEX_IMAGE_NO_STATUS    4

struct lex_dload_version_args {
    idbtype *idb;
    uchar   *dload_buf;
    ulong   length;
    uchar   status;
};

#define LEX_IMAGE_CSUM_SIZE 16

typedef struct lex_image_header {
    uchar  checksum[LEX_IMAGE_CSUM_SIZE]; /* MD5 checksum */
    ulong  file_length;
    ulong  *start_addr;
    uchar  sw_major_ver;
    uchar  sw_minor_ver;
} lex_image_header;

/* 
 * LEX rcmd status byte
 */
#define LEX_BLOCK_ACK     1 /* Valid for DLOAD_WRQ, DLOAD_DATA, STATS_REQUEST */
#define LEX_TIMEOUT_ERROR 2 /* Valid for all commands */
#define LEX_SEQ_ERROR     3 /* Valid for LEX_DLOAD_WRQ and LEX_DLOAD_DATA */
#define LEX_RCMD_ABORTED  4 /* Valid for DLOAD_WRQ, DLOAD_DATA, STATS_REQUEST */
#define LEX_LINK_DOWN     5 /* Valid for DLOAD_WRQ, DLOAD_DATA, STATS_REQUEST */
#define LEX_FLASH_WRERR   6 /* Valid for DLOAD_STATUS */
#define LEX_CHECKSUM_ERR  7 /* Valid for DLOAD_STATUS */
#define LEX_NAK_ERROR     8 /* Valid for all commands */
#define LEX_REJ_ERROR     9 /* Valid for all commands */

#define LEX_MAX_OUTSTANDING_RCMDS 5

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#define lex_rcmd_headstart(ptr)  ((ptr)->network_start)

struct lex_hdr {
    uchar code;
    uchar id;
    ushort plen;
};

struct lex_option {
    uchar type;
    uchar flags;
    ushort length;
};

struct lex_proto_filter_entry {
    ushort value;
    ushort mask;
    ushort grant;
};

struct lex_proto_filter {
    struct lex_proto_filter_entry filter[LEX_MAX_PROTO_FILTERS];
};

struct lex_addr_filter_entry {
    uchar   macaddr[IEEEBYTES];
    uchar   mask[IEEEBYTES];
    ushort  grant;
};

struct lex_addr_filter {
    struct  lex_addr_filter_entry flt[LEX_MAX_ADDR_FILTERS];
};

struct lex_prty_entry {
    ushort proto;
    ushort priority;
};

struct lex_proto_prty {
    struct lex_prty_entry pentry[LEX_MAX_PRIORITY_ENTRIES];
};

struct lex_download {
    ushort block_number;
    uchar data[TFTP_MAXDATA];
};

struct lex_download_wrq {
    ushort block_number;
    ulong  PACKED(filesize);
    char filename[LEX_MAX_FILENM];
};

struct lex_download_status {
    ushort status_code;
};

typedef struct lex_unsolicited_t {
    ulong status;
} lex_unsolicited_t;

typedef struct lex_serial_counters_t {
    ulong inputs;               /* no. packets received */
    ulong input_crc;            /* no. received with CRC errors */
    ulong input_frame;          /* no. rcvd with framing/alignment error */
    ulong input_overrun;        /* no. overruns and resource errors */
    ulong input_drops;          /* no. dropped for input congestion */
    ulong input_abort;          /* no. aborts received */
    ulong input_error;          /* no. damaged packets received */
    ulong input_resource;       /* no buffer descriptors */
    ulong lastinput_delta;      /* time since last input */
    ulong rx_datarate;          /* average data rate */
    ulong rx_packetrate;        /* average input packet rate */
    ulong rx_cumbytes;          /* total pkt bytes received */
    ulong nobuffers;            /* no. dropped because no buffers */

    ulong outputs;              /* no. packets transmitted */
    ulong output_error;         /* no. packets erred on output */
    ulong output_underrun;      /* no of underruns on output */
    int   output_qcount_low;
    int   output_qmaximum_low;
    int   output_qdrops_low;
    int   output_qcount_normal;
    int   output_qmaximum_normal;
    int   output_qdrops_normal;
    int   output_qcount_medium;
    int   output_qmaximum_medium;
    int   output_qdrops_medium;
    int   output_qcount_high;
    int   output_qmaximum_high;
    int   output_qdrops_high;
    ulong lastoutput_delta;     /* time since last output */
    ulong transitions;          /* no. of carrier transitions */
    ulong tx_datarate;          /* average data rate */
    ulong tx_packetrate;        /* average output packet rate */
    ulong tx_cumbytes;          /* total bytes sent */
} lex_serial_counters_t;

typedef struct lex_ether_counters_t {
    ulong inputs;               /* no. packets received */
    ulong input_crc;            /* no. received with CRC errors */
    ulong input_frame;          /* no. rcvd with framing/alignment error */
    ulong input_overrun;        /* no. overruns and resource errors */
    ulong input_drops;          /* no. dropped for input congestion */
    ulong input_abort;          /* no. aborts received */
    ulong input_broad;          /* no. broadcasts received */
    ulong input_error;          /* no. damaged packets received */
    ulong input_resource;       /* no buffer descriptors */
    ulong runts;                /* no. too small packets received */
    ulong giants;               /* no. too large packets received */
    ulong lastinput_delta;      /* time since last input */
    ulong rx_datarate;          /* average data rate */
    ulong rx_packetrate;        /* average input packet rate */
    ulong rx_cumbytes;          /* total pkt bytes received */
    ulong nobuffers;            /* no. dropped because no buffers */

    ulong outputs;              /* no. packets transmitted */
    ulong output_error;         /* no. packets erred on output */
    ulong output_underrun;      /* no of underruns on output */
    ulong output_collision;     /* no. of packets collided on output */
    int   output_qcount;
    int   output_qmaximum;
    int   output_qdrops;
    ulong lastoutput_delta;     /* time since last output */
    ulong tx_datarate;          /* average data rate */
    ulong tx_packetrate;        /* average output packet rate */
    ulong tx_cumbytes;          /* total bytes sent */
    ulong resets;               /* incremented for every shut/no-shut */
} lex_ether_counters_t;

typedef struct lex_stats_area_t {
    lex_serial_counters_t serial;
    lex_serial_counters_t si_serial;   /* shadow serial counters */
    lex_ether_counters_t ether;
    lex_ether_counters_t si_ether;     /* shadow ether counters */
} lex_stats_area_t;

/*
 * Definitions for lex_inventory_msg->flags field.
 */
#define  LEX_RUNNING_PROM       0x01
#define  LEX_RUNNING_FLASH      0x02
#define  PROM_IMAGE_CKSUM_OK    0x04
#define  FLASH_IMAGE_CKSUM_OK   0x08

#define LEX_FLASH_HAS_FILE(idb) \
    (idb->lex_info->inventory->flags & FLASH_IMAGE_CKSUM_OK)

typedef struct lex_inventory_msg {
    uint   flash_size;
    uint   serial_number;
    uchar  hw_rev;
    uchar  prom_sw_ver_maj;
    uchar  prom_sw_ver_min;
    uchar  flash_sw_ver_maj;
    uchar  flash_sw_ver_min;
    uchar  flags;  /* 1 ==> running PROM code; 2 ==> running flash code */
    uchar  running_sw_ver_maj;
    uchar  running_sw_ver_min;
    uchar  mac_addr[IEEEBYTES];
    ushort padding; /* unused, padding to a word boundary in case of mips.*/
} lex_inventory_msg;


/*
 * lex_command
 * union of all the defined lex remote command message structures.
 */
typedef struct lex_command {
    struct lex_hdr  hdr;
    struct lex_option opt;
    union {
	uchar  data[4];
               lex_inventory_msg    inventory;
        struct lex_addr_filter      addr_fltr;
        struct lex_proto_filter     prot_fltr;
        struct lex_proto_prty       prot_prty;
        struct lex_download         dload_data;
        struct lex_download_wrq     dload_wrq;
        struct lex_download_status  dload_status;
	lex_unsolicited_t      unsolicited;
    } cmd;
} lex_command;

/*
 * Externs
 */
extern queuetype LEXinputQ;
extern queuetype LEXrcmdQ;
extern const char lex_not_bound[];


/*
 * Prototypes
 */
lex_rcmdst_type *lex_setup_rcmd(uchar, idbtype *, boolean);
boolean lex_rcmdBLOCK (lex_rcmdst_type *);
void lex_send_rcmd (lex_rcmdst_type *);
void lex_shutdown(hwidbtype *, int);
void lex_noshutdown(hwidbtype *);
void lex_rcmd_init(void);
void lex_rcmd_input(void);
char *lex_print_rcmd(uchar);
void lex_rcmd_stub_request_send(uchar, idbtype *);
void lex_rcmd_cleanup(lex_rcmdst_type *);
boolean lex_sendblock(uchar *, int, long, idbtype *);
boolean lex_show_interface_common(idbtype *, lex_interface_type);
void lex_send_clear_counters_rcmd(idbtype *, lex_interface_type);
void lex_clear_ether_counters(parseinfo *);
void lex_clear_serial_counters(parseinfo *);

#endif
