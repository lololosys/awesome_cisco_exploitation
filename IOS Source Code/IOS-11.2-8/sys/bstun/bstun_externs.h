/* $Id: bstun_externs.h,v 3.3.18.1 1996/09/10 08:48:41 jbalestr Exp $
 * $Source: /release/112/cvs/Xsys/bstun/bstun_externs.h,v $
 *------------------------------------------------------------------
 * bstun_externs.h - BSTUN external declaration/prototypes
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bstun_externs.h,v $
 * Revision 3.3.18.1  1996/09/10  08:48:41  jbalestr
 * CSCdi62395:  bsc/bstun debug for specific groups and addresses
 * Branch: California_branch
 *
 * Revision 3.3  1996/02/08  18:01:42  ahh
 * CSCdi48380:  TCP: TCP Driver close callback needs more context
 * Have some t with your cookie.
 *
 * Revision 3.2  1995/11/17  08:49:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:04:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/25  11:41:05  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.3  1995/08/11 08:21:59  arothwel
 * CSCdi38625:  Fix:
 *              1. Broken timer s/ware stopping Tx/Rx.
 *              2. Incorrect bsc-mib sub-system dependencies.
 *              3. Premature starting of bstun-event-process.
 *              4. Removal of timestamp info from debugging messages.
 *              5. Replace deprecated process mngmt s/ware w/ new s/ware.
 *
 * Revision 2.2  1995/06/28  09:22:46  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:14:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _BSTUN_EXTERNS_H_
#define _BSTUN_EXTERNS_H_

#include "bstun.h"

extern hwidbtype *bstun_first_idb;

/* from sna_la */
extern int bstun_addr_pri_tcp(hwidbtype *, paktype *, int *);
extern boolean prilist_uses_bstun_group(uchar);
/* efrom sna_la*/

#ifndef lint
#include "../tcp/tcp.h"
extern void     bstun_init(subsystype *);
extern char     *bstpeer_id(bstunpeertype *stp, char *buff);
extern char     *bstpeer_state(bstunpeertype *stp);
extern bstunpeertype *bstif_findpeer(uchar group, uchar *addrp, hwidbtype *in_idb);
extern bstunpeertype *bsttcp_findpeer(uchar group, uchar *addrp, ipaddrtype ipaddr,
				    int, int);
extern paktype  *bstpak(ushort op , uchar group, uchar *addrp, ushort len);
extern char     *bst_op2str(ushort op);
extern boolean  bstcheck_conn_state(bstunpeertype *stp);
extern void     bstun_background(void);
extern void     bstif_nukepak(paktype *pak);
extern boolean  bstif_direct(bstunpeertype *peer, paktype *pak, hwidbtype *outidb,
                             boolean force, bstunpeertype **outpeer,
                             boolean usethispeer, boolean toallpp);
extern void     bstif_abort(bstunpeertype *peer);
extern void     bstif_close(bstunpeertype *peer, ulong param);
extern bstunconnrtn bstif_connect(bstunpeertype *stp);
extern boolean  bsttcpd_sender(bstunpeertype *inpeer, paktype *pak,
                               hwidbtype *outidb, boolean force,
                               bstunpeertype **outpeer, boolean usethispeer,
                               boolean toallpp);
extern void     bsttcpd_abort(bstunpeertype *stp);
extern void     bsttcpd_close(bstunpeertype *stp, ulong param);
extern bstunconnrtn bsttcpd_connect(bstunpeertype *stp);
extern void     bsttcpd_send_close(tcp_encaps *t, ushort op,
    bstunhdrtype *sth, char *label, void *abort_cookie, boolean abort);
extern void     bsttcpd_err(bstunpeertype *stp, tcp_encaps *t, bstunhdrtype *sth,
		       char *label);
extern boolean  bstget_hdr(tcp_encaps *t, tcbtype *tcb, int thresh,
    bstunpeertype *stp, bstunhdrtype *sth, char *label, boolean *ignore);
extern void     bsttcpd_drain(tcp_encaps *t, tcbtype *tcb, int thresh, void *cookie);
extern void     bsttcpd_phdr(tcp_encaps *t, tcbtype *tcb, int thresh, void *cookie);
extern void     bsttcpd_listen(tcbtype *tcb);
extern void     bsttcpd_rdhdr(tcp_encaps *t, tcbtype *tcb, int thresh, void *cookie);
extern void     bsttcpd_rddata(tcp_encaps *t, tcbtype *tcb, int thresh, void *cookie);
extern void     bsttcpd_fini(tcp_encaps *t, void *cookie);
extern void     bstun_newstate(bstunpeertype *stp, bstunstatetype);
extern boolean  bstun_active(ulong sport, ulong dport,ipaddrtype fhost);
extern bstun_protocol_info *bstun_new_generic(char *name, int addrlen,
					    int addroff,
					    char addrfmt, boolean onlyall,
					    boolean neverall  );
extern bstun_protocol_info *bstun_noaddress_predefined_init(char **name);
extern void     bstun_to_network(hwidbtype *idb, paktype *pak, int pak_opcode);
extern boolean  bstun_backBLOCK(void);
extern mgd_timer* bstun_get_bevent_timer(void);

extern boolean bevent_backgroundBLOCK(void);
extern void    bevent_background(void);
extern boolean bevent_network(hwidbtype *, paktype *);
extern ushort  bevent_hardware(hwidbtype *, paktype *);

extern paktype *get_flush_pak(void);
#else
extern void     bstun_init();
extern char     *bstpeer_id();
extern char     *bstpeer_state();
extern bstunpeertype *bstif_findpeer();
extern bstunpeertype *bsttcp_findpeer();
extern paktype  *bstpak();
extern char     *bst_op2str();
extern boolean  bstcheck_conn_state();
extern forktype bstun_background();
extern void     bstif_nukepak();
extern boolean  bstif_direct();
extern void     bstif_abort();
extern void     bstif_close();
extern bstunconnrtn bstif_connect();
extern boolean  bsttcpd_sender();
extern void     bsttcpd_abort();
extern void     bsttcpd_close();
extern bstunconnrtn bsttcpd_connect();
extern void     bsttcpd_send_close();
extern void     bsttcpd_err();
extern boolean  bstget_hdr();
extern void     bsttcpd_drain();
extern void     bsttcpd_phdr();
extern void     bsttcpd_listen();
extern void     bsttcpd_rdhdr();
extern void     bsttcpd_rddata();
extern void     bsttcpd_fini();
extern void     bstun_newstate();
extern boolean  bstun_active();
extern bstun_protocol_info *bstun_new_generic();
extern bstun_protocol_info *bstun_noaddress_predefined_init();
extern void     bstun_to_network();
extern boolean  bstun_backBLOCK();

extern boolean bevent_backgroundBLOCK ();
extern forktype bevent_background ();
extern boolean bevent_hardware ();

extern paktype *get_flush_pak();
#endif
#endif
