/* $Id: if_async.h,v 3.4.20.4 1996/06/18 01:49:40 hampton Exp $
 * $Source: /release/112/cvs/Xsys/ts/if_async.h,v $
 *------------------------------------------------------------------
 * Prototypes for if_async.c
 *
 * Dave Katz, January 1993
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: if_async.h,v $
 * Revision 3.4.20.4  1996/06/18  01:49:40  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.4.20.3  1996/06/17  08:35:58  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.4.20.2  1996/06/16  21:20:11  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.4.20.1  1996/04/27  06:28:21  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.3.20.2  1996/04/16  23:46:13  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.4  1996/02/07  00:47:33  irfan
 * CSCdi48054:  async interfaces should default to no keepalive
 * Allow hwidb's to have default other than KEEPALIVE_DEFAULT.
 *
 * Revision 3.3.20.1  1996/04/06  21:25:34  dblair
 * Branch: Dial1_branch
 * Fix CSCdi53768 and added restart lcp when ppp multilink command
 * processed, and fix multilink not negotiated at reload problem.
 *
 * Revision 3.3  1995/11/17  18:54:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:45:09  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:39:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/23  07:17:50  tkolar
 * CSCdi40082:  Dialer requires PPP subsystem hence also require CHAT
 * subsystem
 * Create registry for returning first member of an interface group
 *
 * Revision 2.2  1995/09/07  20:32:40  tkolar
 * CSCdi39804:  group async is incompatible with dialer
 * Undo some collateral damage from CSCdi36407.
 *
 * Revision 2.1  1995/06/07  23:09:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#define GROUPMASK_DEFAULT_IP_ADDRESS 	0x1
#define GROUPMASK_DESCRIPTION 		0x2

struct async_member_info_ {
    struct async_member_info_ *next;
    hwidbtype *hwidb;
    int difference_mask;	/* list of differences from group leader */
};

/*
 * Some Async specific Constants
 */

#define ASYNC_DEFAULT_PULSEINTERVAL (5 * ONESEC)
#define ASYNC_MIN_PULSEINTERVAL (2 * ONESEC)
#define ASYNC_IGRP_DELAY 10000		         /* 100 msec */
#define ASYNC_DEF_KEEPPERIOD (0)                 /* No keepalives */


/*
 * SLIP traffic statistics
 */
typedef struct slip_traffic_t_ {
    ulong inpacks;
    ulong inbytes;
    ulong fmterrors;
    ulong inescapes;
    ulong crcerr;
    ulong badlen;
    ulong nobuf;
    ulong outpacks;
    ulong outbytes;
    ulong outescapes;
    ulong dropped;
} slip_traffic_t;

/*
 * Externs
 */
extern slip_traffic_t slip_traffic;

/*
 * Function prototypes
 */

extern void async_reset_tty(tt_soc *);
extern boolean async_framing_debug;
extern boolean async_packet_debug;
extern boolean async_state_debug;
extern void async_init(void);
extern boolean async_framing_setup(tt_soc *);
extern void async_setmap(hwidbtype *, ulong);
extern void async_new_encap(hwidbtype *, encapstype);
extern hwidbtype *init_async_idb(tt_soc *, boolean);
extern void init_async_fs(void);
extern void async_group_config_command(parseinfo *csb);
extern hwidbtype *find_async_group_leader(hwidbtype *);

