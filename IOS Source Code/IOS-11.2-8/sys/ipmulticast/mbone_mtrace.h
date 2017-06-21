/* $Id: mbone_mtrace.h,v 3.3 1995/11/21 23:12:05 dino Exp $
 * $Source: /swtal/cherf.111/ios/sys/ipmulticast/mbone_mtrace.h,v $
 *------------------------------------------------------------------
 * mbone_mtrace.h - mbone multicast trace definitions
 * 
 * August, 1995 chwhite
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mbone_mtrace.h,v $
 * Revision 3.3  1995/11/21  23:12:05  dino
 * CSCdi43427:  cisco does not process DVMRP prunes.
 *
 * Revision 3.2  1995/11/16  23:42:10  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:01:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/22  21:54:27  dino
 * CSCdi39110:  DVMRP version number should be added to all packets, fix
 * style
 *
 * Revision 2.3  1995/08/17  22:44:08  chwhite
 * CSCdi38823:  Code review comments and minor bug fixes
 *
 * Revision 2.2  1995/08/15  21:22:14  chwhite
 * CSCdi38823:  Code review comments and minor bug fixes
 *
 * Revision 2.1  1995/08/03  01:13:00  chwhite
 * CSCdi38104:  It would be nice if the router supported mtrace/mrinfo.
 *              Added support for mbone traceroute and mrinfo
 *
 * 
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __MBONE_TRACE_H__
#define __MBONE_TRACE_H__

/*
 * Constant definitions.
 */
#define MBONE_MTRACE_DEFAULT_TTL 32
#define MBONE_MTRACE_MAX_TTL     192
#define MBONE_MTRACE_TIMEOUT     (3 * ONESEC)
#define MRINFO_TIMEOUT           (10 * ONESEC)
#define MAXHOPS                  32

#define WAITTIME   10   /* Default time to wait before second stat query */

#define TR_NO_ERR      0
#define TR_WRONG_IF    1
#define TR_PRUNED      2
#define TR_OPRUNED     3
#define TR_SCOPED      4
#define TR_NO_RTE      5
#define TR_NO_FWD      7
#define TR_HIT_RP      8
#define TR_RPF_IF      9
#define TR_NO_MULTI    10

#define TR_NO_SPACE    0x81
#define TR_OLD_ROUTER  0x82
#define TR_ADMIN_PROHIB 0x83

#define PROTO_DVMRP 1
#define PROTO_MOSPF 2
#define PROTO_PIM   3
#define PROTO_CBT   4
#define PROTO_PIM_SPECIAL 5
#define PROTO_PIM_STATIC 6
#define PROTO_DVMRP_STATIC 7

#define NEITHER 0
#define INS     1
#define OUTS    2
#define INOUT    3

typedef struct mbone_mtraceQtype_ {
    struct mbone_mtraceQtype_ *next;
    ulong         id;	
    tt_soc        *tty;
    ulong         time_sent;	/* 16bits seconds, 16bits fractional */
    sys_timestamp timeout;
    boolean       timeout_flag;
    boolean       interrupted;
    queuetype     data;
} mbone_mtraceQtype;

typedef struct mrinfoQtype_ {
  struct mrinfoQtype_ *next;
  ulong         id;
  tt_soc        *tty;
  sys_timestamp timeout;
  boolean       timeout_flag;
  boolean       interrupted;
  queuetype     data;
} mrinfoQtype;

typedef struct mbone_mtrace_query_ {
  ipaddrtype tr_src;              /* Traceroute source */
  ipaddrtype tr_dst;              /* Traceroute destination */
  ipaddrtype tr_raddr;            /* Traceroute response address */
  ulong      ttl_qid;             /* Query ID and ttl word */
} mbone_mtrace_query;

typedef struct mbone_mtrace_resp_ {
  ulong      tr_qarr;
  ipaddrtype tr_inaddr;
  ipaddrtype tr_outaddr;
  ipaddrtype tr_rmtaddr;
  ulong      tr_ifin;
  ulong      tr_ifout;
  ulong      tr_pktcnt;
  uchar      tr_rproto;
  uchar      tr_fttl;
  uchar      tr_smask;
  uchar      tr_rflags;
} mbone_mtrace_resp;

#define  QLEN sizeof(struct mbone_mtrace_query_)
#define  RLEN sizeof(struct mbone_mtrace_resp_)

/*
 * External Function Declarations.
 */
extern void mbone_mtrace_command(parseinfo *);
extern void mrinfo_command(parseinfo *);
extern void mbone_mstat_command(parseinfo *);
extern mbone_mtraceQtype *ip_mbone_mtrace_parse_csb(parseinfo *, ipaddrtype *,
				    ipaddrtype *, ipaddrtype *, uchar *, 
						    ipaddrtype *);
extern void ip_mbone_mtrace_receive_response(igmptype *, paktype *, int);
extern void ip_mbone_mtrace_send_request(ipaddrtype, ipaddrtype,
					 ipaddrtype, ipaddrtype, uchar, int,
					 int, mbone_mtraceQtype *);
extern boolean ip_mbone_mtrace_process_request (igmptype *, paktype *, int);
extern ulong what_kind(ipaddrtype, boolean);
extern boolean ip_mrinfoBLOCK(mrinfoQtype *);

#endif __MBONE_TRACEROUTE_H__





