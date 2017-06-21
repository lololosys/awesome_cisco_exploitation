/* $Id: ibm_externs.h,v 3.3 1996/02/08 18:01:56 ahh Exp $
 * $Source: /release/111/cvs/Xsys/ibm/ibm_externs.h,v $
 *------------------------------------------------------------------
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ibm_externs.h,v $
 * Revision 3.3  1996/02/08  18:01:56  ahh
 * CSCdi48380:  TCP: TCP Driver close callback needs more context
 * Have some t with your cookie.
 *
 * Revision 3.2  1995/11/17  09:22:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:44:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/06  01:32:16  hampton
 * Eliminate magic timestamps from SDLC and STUN code by converting them
 * to use the passive timer macros.  [CSCdi36750]
 *
 * Revision 2.2  1995/06/28 09:25:23  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:46:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _IBM_EXTERNS_H_
#define _IBM_EXTERNS_H_
#include "../ibm/ibm.h"
extern sys_timestamp stun_lastframe;
extern hwidbtype *stun_first_idb;
extern queuetype stconn_pakQ;
#ifdef NOBODY_CALLS_ME
extern void stun_sdlc_tg_reroute(paktype *pak, stunpeertype *stp);
#endif /* NOBODY_CALLS_ME */
extern void stun_cos_dest_reseq (stunpeertype *stp, paktype *pak);
extern void stun_cos_flush_reseq (queuetype *queue);
extern void stun_fromnet_forward ( paktype *pak, stunpeertype *stp, ushort opcode);
#ifndef lint
#include "../tcp/tcp.h"
extern void stun_init(subsystype *);
extern char *stpeer_id(stunpeertype *stp, char *buff);
extern char *stpeer_state(stunpeertype *stp);
extern stunpeertype *stif_findpeer(uchar group, uchar *addrp, hwidbtype *in_idb);
extern stunpeertype *sttcp_findpeer(uchar group, uchar *addrp, ipaddrtype ipaddr,
				    int, int);
extern paktype *stpak(ushort op , uchar group, uchar *addrp, ushort len);
extern char *st_op2str(ushort op);
extern boolean stcheck_conn_state(stunpeertype *stp);
extern forktype stun_background(void);
extern void stif_nukepak(paktype *pak);
extern boolean stif_direct(stunpeertype *peer, paktype *pak, hwidbtype *outidb, boolean force, stunpeertype **outpeer, boolean usethispeer, boolean toallpp);
extern void stif_abort(stunpeertype *peer);
extern void stif_close(stunpeertype *peer, ulong param);
extern stunconnrtn stif_connect(stunpeertype *stp);
extern boolean sttcpd_sender(stunpeertype *inpeer, paktype *pak,
			     hwidbtype *outidb, boolean force,
			     stunpeertype **outpeer, boolean usethispeer, boolean toallpp);
#ifdef NOBODY_CALLS_ME
extern boolean sttcpd_direct(stunpeertype *stp, paktype *pak, hwidbtype *outidb, boolean force);
#endif /* NOBODY_CALLS_ME */
extern void sttcpd_abort(stunpeertype *stp);
extern void sttcpd_close(stunpeertype *stp, ulong param);
extern stunconnrtn sttcpd_connect(stunpeertype *stp);
extern void sttcpd_send_close(tcp_encaps *t, ushort op,
    stunhdrtype *sth, char *label, void *abort_cookie, boolean abort);
extern void sttcpd_err(stunpeertype *stp, tcp_encaps *t, stunhdrtype *sth,
		       char *label);
extern boolean stget_hdr(tcp_encaps *t, tcbtype *tcb, int thresh,
    stunpeertype *stp, stunhdrtype *sth, char *label, boolean *ignore);
extern void sttcpd_drain(tcp_encaps *t, tcbtype *tcb, int thresh, void *cookie);
extern void sttcpd_phdr(tcp_encaps *t, tcbtype *tcb, int thresh, void *cookie);
extern void sttcpd_listen(tcbtype *tcb);
extern void sttcpd_rdhdr(tcp_encaps *t, tcbtype *tcb, int thresh, void *cookie);
extern void sttcpd_rddata(tcp_encaps *t, tcbtype *tcb, int thresh, void *cookie);
extern void sttcpd_fini(tcp_encaps *t, void *cookie);
extern void stun_newstate(stunpeertype *stp, stunstatetype);
extern boolean stun_active(ulong sport, ulong dport, ipaddrtype fhost);
extern void show_stun_local_ack(void);
extern boolean stun_print_check(uchar group, uchar *address);
extern stun_protocol_info *stun_new_generic(char *name, int addrlen,
					    int addroff,
					    char addrfmt, boolean onlyall,
					    boolean neverall,
					    boolean local_ack_capable,
					    boolean local_ack_required);
extern void stun_print_frame(paktype *pak, int group, char *prefix,
			     hwidbtype *idb, void (*routine)(paktype *, int, 
							     char*, 
							     hwidbtype *));
extern boolean stun_parse_addr_core(uchar group, char *buff, uchar *addr);
extern stun_protocol_info *stun_sdlc_predefined_init(char **name);
extern stun_protocol_info *stun_sdlc_tg_predefined_init(char **name);
extern stun_protocol_info *stun_noaddress_predefined_init(char **name);
extern stunpeertype *stun_addr_to_peer(hwidbtype *idb, char *addr);
extern void stun_to_network(hwidbtype *idb, paktype *pak, stunpeertype *stp,
			    char *addrstart, int pak_opcode);
extern void stun_send_keepalive (stunpeertype *stp);
extern boolean stun_backBLOCK(void);
extern paktype *get_flush_pak(void);
#else
extern char *stpeer_id();
extern char *stpeer_state();
extern stunpeertype *stif_findpeer();
extern stunpeertype *sttcp_findpeer();
extern paktype *stpak();
extern char *st_op2str();
extern boolean stcheck_conn_state();
extern forktype stun_background();
extern void stif_nukepak();
extern boolean stif_direct();
extern void stif_abort();
extern void stif_close();
extern stunconnrtn stif_connect();
extern boolean sttcpd_sender();
#ifdef NOBODY_CALLS_ME
extern boolean sttcpd_direct();
#endif /* NOBODY_CALLS_ME */
extern void sttcpd_abort();
extern void sttcpd_close();
extern stunconnrtn sttcpd_connect();
extern void sttcpd_send_close();
extern void sttcpd_err();
extern boolean stget_hdr();
extern void sttcpd_drain();
extern void sttcpd_phdr();
extern void sttcpd_listen();
extern void sttcpd_rdhdr();
extern void sttcpd_rddata();
extern void sttcpd_fini();
extern void stun_newstate();
extern boolean stun_active();
extern void show_stun_local_ack();
extern boolean stun_print_check();
extern stun_protocol_info *stun_new_generic();
extern void stun_print_frame();
extern boolean stun_parse_addr_core();
extern stun_protocol_info *stun_sdlc_predefined_init();
extern stun_protocol_info *stun_sdlc_tg_predefined_init();
extern stun_protocol_info *stun_noaddress_predefined_init();
extern stunpeertype *stun_addr_to_peer();
extern void stun_to_network();
extern void stun_send_keepalive();
extern boolean stun_backBLOCK();
extern paktype *get_flush_pak();
#endif
#endif





