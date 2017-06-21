/* $Id: netbios.h,v 3.6 1996/03/07 06:34:06 kchiu Exp $
 * $Source: /release/111/cvs/Xsys/ibm/netbios.h,v $
 *------------------------------------------------------------------
 * netbios.h -- NETBIOS support definitions
 *
 * January 21, 1991 - Joel P. Bion
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: netbios.h,v $
 * Revision 3.6  1996/03/07  06:34:06  kchiu
 * CSCdi40165:  Add netbios session level byte filtering support
 *
 * Revision 3.5  1996/03/04  07:14:13  kchiu
 * CSCdi49101:  make netbios name recognized filtering optional
 *
 * Revision 3.4  1995/12/12  06:09:24  gschleic
 * CSCdi44259:  Netbios name-cache timeout parses incorrectly
 *
 * Revision 3.3  1995/12/11  23:18:02  jacobt
 * CSCdi44533:  Proxy explorer, parser bugs and more!
 *
 * Revision 3.2  1995/11/17  09:23:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:45:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:02:02  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/10/03  18:41:00  rbatz
 * CSCdi36649:  Need to filter on Netbios Name Recognized frames since
 *              they are All-Routes Broadcasts.
 *
 * Revision 2.3  1995/07/06  01:32:19  hampton
 * Eliminate magic timestamps from SDLC and STUN code by converting them
 * to use the passive timer macros.  [CSCdi36750]
 *
 * Revision 2.2  1995/06/28 09:25:35  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:47:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _NETBIOS_H_
#define _NETBIOS_H_

/* NETBIOS source/destination names are limited to 16 characters in length */
#define NETBIOS_MAX_NAME_LEN (16)
#define NETBIOS_NAME_LENGTH_DEFAULT (15)
#define NETBIOS_MAX_NAME_ULONG (NETBIOS_MAX_NAME_LEN / 4)

/*
 * NetBIOS access filter check enumerated types
 */
/* NetBIOS with a token ring/llc2 header */
#define NETBIOS_TOKEN_LLC2_CHECK (0)
/* NetBIOS with an IPX header only (no MAC header) */
#define NETBIOS_IPX_CHECK (1)

typedef struct netbios_acctype_ {
    struct netbios_acctype_ *next;
    boolean result;		/* Permit or deny */
    boolean prefix;
    uchar length;		/* 1-16 */
    ulong offset;		/* For "bytes" lists */
    ulong pattern[NETBIOS_MAX_NAME_ULONG];
    ulong mask[NETBIOS_MAX_NAME_ULONG];
} netbios_acctype;

struct netbios_acclisttype_ {
    struct netbios_acclisttype_ *next;
    NAMEDTHINGMACRO
    queuetype q;
};

/*
 * NETBIOS header
 */
#define NETBIOS_DELIMITER_0 ((uchar) 0xff)
#define NETBIOS_DELIMITER_1 ((uchar) 0xef)

/* netbios_header.command & ~A_MASK gives A_VALUE means check this one */

/* Allows 00 and 01 */
#define NETBIOS_SRC_VALUE (0x00)
#define NETBIOS_SRC_MASK  (0x01)

/* Allows 08 and 0A */
#define NETBIOS_DEST_VALUE (0x08)
#define NETBIOS_DEST_MASK_1 (0x02)
#define NETBIOS_DEST_MASK_2 (0x06)

#define NETBIOS_ADD_GROUP_QUERY (0x00)
#define NETBIOS_ADD_NAME_QUERY (0x01)
#define NETBIOS_NAME_IN_CONFLICT (0x02)
#define NETBIOS_STATUS_QUERY (0x03)
#define NETBIOS_TERMINATE_TRACE_REM  (0x07)
#define NETBIOS_DATAGRAM (0x08)
#define NETBIOS_DATAGRAM_BROADCAST (0x09)
#define NETBIOS_NAME_QUERY (0x0A)
#define NETBIOS_ADD_NAME_RESPONSE (0x0D)
#define NETBIOS_NAME_RECOGNIZED (0x0E)
#define NETBIOS_STATUS_RESPONSE (0x0F)
#define NETBIOS_TERMINATE_TRACE_REM_LOC  (0x13)
#define NETBIOS_DATA_ACK (0x14)
#define NETBIOS_DATA_FIRST_MIDDLE (0x15)
#define NETBIOS_ONLY_LAST (0x16)
#define NETBIOS_SESSION_CONFIRM (0x17)
#define NETBIOS_SESSION_END (0x18)
#define NETBIOS_SESSION_INITIALIZE (0x19)
#define NETBIOS_NO_RECEIVE (0x1A)
#define NETBIOS_RECEIVE_OUTSTANDING (0x1B)
#define NETBIOS_RECEIVE_CONTINUE (0x1C)
#define NETBIOS_SESSION_ALIVE (0x1F)

#define NETBIOS_CORRELATOR_SIZE (2)

/*
 * NETBIOS SESSION END termination codes
 */
#define NETBIOS_NORMAL 0
#define NETBIOS_HANGUP 1

/*
 * UI-Frame (non-session NetBIOS header) payload:
 * These are all uchars, as the NETBIOS header starts at an odd-byte
 * offset in the frame, and a bcopy shouldn't have to be done just
 * for word alignment before the frame goes through this access check.
 */
typedef struct netbios_header_ {
    uchar header_length[2];	/* 0-1 Header Length */
    uchar delimiter[2];		/* 2-3 delimiter (0xefff) */
    uchar command;		/* 4 command 0-0x13 = UI, 0x14-0x1f = I */
    uchar data1;		/* 5 optional data 1 */
    uchar data2[2];		/* 6-7 optional data 2 */
    uchar xmit_correlator[NETBIOS_CORRELATOR_SIZE]; /* 8-9 first correlator */
    uchar rsp_correlator[NETBIOS_CORRELATOR_SIZE];/* 10-11 second correlator */
    uchar destname[NETBIOS_MAX_NAME_LEN]; /* destination name */
    uchar srcname[NETBIOS_MAX_NAME_LEN]; /* source name */
} netbios_header;

/*
 * I-Frame (session NetBIOS header) payload:
 * These are all uchars, as the NETBIOS header starts at an odd-byte
 * offset in the frame, and a bcopy shouldn't have to be done just
 * for word alignment before the frame goes through this access check.
 */
typedef struct netbios_header_sess_ {
    uchar header_length[2];	/* 0-1 Header Length */
    uchar delimiter[2];		/* 2-3 delimiter (0xefff) */
    uchar command;		/* 4 command 0-0x13 = UI, 0x14-0x1f = I */
    uchar data1;		/* 5 optional data 1 */
    uchar data2[2];		/* 6-7 optional data 2 */
    uchar xmit_correlator[NETBIOS_CORRELATOR_SIZE]; /* 8-9 first correlator */
    uchar rsp_correlator[NETBIOS_CORRELATOR_SIZE];/* 10-11 second correlator */
    uchar destnum; /* destination session number */
    uchar srcnum;  /* source session number */
} netbios_header_sess;


#define NETBIOS_HEADER_SIZE	44	/* size of netbios_header */
#define NETBIOS_HEADER_SESS_SIZE  14    /* size of netbios_header_sess */ 

/*
 * NetBIOS name cache data structures
 */
#define NETBIOS_HASH_LEN (0x100)
#define NETBIOS_HOLD_DOWN_TIME 60*ONESEC

typedef struct netbios_name_cache_ {
    struct  netbios_name_cache_ *next;	/* pointer to next record */
    int     magic;			/* magic no for paranoia code */
    int     flags;
    uchar   addr[IEEEBYTES];	/* hardware address of device */
    uchar   name[NETBIOS_MAX_NAME_LEN + 1]; /* actual name */
    uchar   nr_name[NETBIOS_MAX_NAME_LEN + 1];	/* NAME_RECOGNIZED name */
    hwidbtype *idb;			/* interface if local... */
    ushort  vrn;			/* ring-group if remote */
    sys_timestamp time;			/* time of last update */
    sys_timestamp query_tick;           /* time of last proxy NAME_QUERY */
    sys_timestamp datagram_tick;
    sys_timestamp name_recog_time;	/* When next NAME_RECOGIZED will go */
    ulong   netb_saved;			/* Packets saved from transmission */
    sys_timestamp hold_down_time;
} netbios_name_cache;

#define NETBIOS_DEFAULT_CACHE_TIMEOUT (15*ONEMIN) /* default time-out */
#define NETBIOS_DEFAULT_QUERY_TIMEOUT (6) 	/* In seconds, default */
#define NETBIOS_DEFAULT_RECOGNIZED_TIMEOUT (1) 	/* In seconds, default */
#define NETBIOS_NAME_MAGIC  0xA15AA156
#define NETBIOS_NAME_QUEUE_MAX 200		/* max queue length */
#define NETBIOS_NAME_QUEUE_MIN 180		/* max queue length */

#define NETBIOS_NAME_NULL 0		/* none */
#define NETBIOS_NAME_AGED 1		/* learned from traffic */
#define NETBIOS_NAME_AGED_REM 2		/* learned from a remote peer */
#define NETBIOS_NAME_HANDCRAFT 4	/* entered from config mode */
#define NETBIOS_NAME_HANDCRAFT_REM 8	/* entered from config mode */


/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

/*
 * netbios.c
 */
extern boolean netbios_expr_host_check(uchar *, ulong, netbios_acclisttype *);
extern boolean netbios_expr_bytes_check(uchar *, ulong, netbios_acclisttype *);
extern boolean netbios_acfcheck(uchar *, ulong, idbtype *, boolean,
				netbios_acclisttype *, boolean, int);
extern void netbios_name_cache_update_pak(uchar *, int, hwidbtype *, 
					  ushort);
extern void netbios_age_cache(void);
extern void netbios_name_cache_ager(boolean, boolean);
extern boolean netbios_name_cache_proxy(paktype *);
extern void netbios_init_idb(hwidbtype*);
extern void netbios_name_if_disable(hwidbtype*);
extern boolean netbios_session_acfcheck(uchar *, ulong, netbios_acclisttype *);

/*
 * netbios_acl.c
 */
extern queuetype netbios_bytes_accQ;
extern queuetype netbios_host_accQ;
extern void netbios_acl_parser_init(void);

/*
 * netbios_name.c
 */
extern queuetype free_netbios_name_cacheQ;
extern netbios_name_cache *netbios_name_cache_table[];
extern boolean netbios_cache_add_name;
extern int netbios_name_cache_timeout;
extern int netbios_query_timeout;
extern int netbios_recognized_timeout;
extern int netbios_name_length;
extern boolean global_netbios_name_cache_enable;
extern void netbios_name_init(subsystype *);
extern void netbios_name_parser_init(void);
extern boolean netb_acc_filter_name_recognized;

#endif _NETBIOS_H_
