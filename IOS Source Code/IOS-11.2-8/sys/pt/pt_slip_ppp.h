/* $Id: pt_slip_ppp.h,v 3.2 1995/11/17 19:00:06 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/pt/pt_slip_ppp.h,v $
 *------------------------------------------------------------------
 * SLIP/PPP specific definitions and prototypes
 *
 * May 1994, Eman Schaffer
 *
 * Copyright (c) 1994-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: pt_slip_ppp.h,v $
 * Revision 3.2  1995/11/17  19:00:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:01:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:35:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * pt_slip_ppp.c
 */
void pt_slip_ppp_init(subsystype *subsys);
int pt_slip_ppp_command(parseinfo *csb, translatetype *trans, int dir);
boolean pt_slip_ppp_add_alias(translatetype *trans);
void pt_show_slip_ppp_transinfo(translatetype *trans, int dir);
void pt_nv_slip_ppp_transinfo(translatetype *trans, int dir);
char *pt_async_trans_string(int prot);
/*
 * pt_slip_ppp_chain.c
 */
void pt_slip_ppp_parser_init(void);
void pt_vty_async_init(void);

#define SLIP_PROTO 1
#define PPP_PROTO 2

