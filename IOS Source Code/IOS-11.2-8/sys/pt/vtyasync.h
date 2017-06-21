/* $Id: vtyasync.h,v 3.3 1996/02/22 05:20:38 fox Exp $
 * $Source: /release/111/cvs/Xsys/pt/vtyasync.h,v $ 
 *------------------------------------------------------------------
 * LAT/TCP/PAD to SLIP-PPP conversion specific definitions and prototypes
 *
 * April 1994, Eman Schaffer
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: vtyasync.h,v $
 * Revision 3.3  1996/02/22  05:20:38  fox
 * CSCdi35908:  Should allow CHAP and PAP clients to same dialin pool
 * Add support for multiple acceptable inbound PPP authentication methods
 *
 * Revision 3.2  1995/11/17  19:00:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:02:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:35:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Prototypes
 */

/*
 * vtyasync.c
 */
void pt_padslip_ppp_init(subsystype *subsys);
extern void pt_run_vtyasync(translatetype *trans, tt_soc *tty);
extern boolean pt_virtual_async_mode(tt_soc *tty, idbtype **idb, 
				  hwidbtype **hwidb);
extern void pt_manage_vtyasync(tt_soc *tty);
extern void pt_check_ppp_options(tt_soc *tty);
extern void pt_vtyasync_exec_connection(tt_soc *tty);
extern void pt_close_vtyasync(translatetype *trans, tt_soc *tty);
extern boolean pt_vtyasync_compression(tt_soc *vty);
extern void pt_enable_vtyasync(int maxlines);
extern boolean pt_wakeup_vtyasync(tt_soc *vty);

/*
 * tcpslip.c
 */
int tcp_transto_slip(translatetype *trans, tt_soc *tty);
void pt_tcpslip_ppp_init(subsystype *subsys);

/*
 * latslip.c
 */
int lat_transto_slip(translatetype *trans, tt_soc *tty);
void pt_latslip_ppp_init(subsystype *subsys);

/*
 * padslip.c
 */
int x25_transto_slip(translatetype *trans, tt_soc *tty);
extern vectortype PADvector;

/*
 * pt_slip_ppp_parse.c
 */
extern boolean pt_vty_async_enabled;
extern boolean pt_compress_passive;
extern boolean pt_compress_on;
extern boolean pt_dynamic_routing;
extern boolean pt_ppp_authentication;
extern boolean pt_ppp_use_tacacs;
extern uchar pt_auth_types[];
extern char pt_ppp_aaa_method[];
extern short pt_keepalive_period;
extern boolean pt_ipx_pppclient;
extern idbtype *pt_ipx_pppclient_owner;
extern int pt_vtyasync_mtu;
