/* $Id: pt_tcp.h,v 3.2 1995/11/17 19:00:14 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/pt/pt_tcp.h,v $
 *------------------------------------------------------------------
 * TCP specific definitions and prototypes
 *
 * November 1993, Eman Schaffer, Scott Mackie
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: pt_tcp.h,v $
 * Revision 3.2  1995/11/17  19:00:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:01:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:35:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Prototypes
 */

/*
 * pt_tcp.c
 */
void pt_tcp_init(subsystype *subsys);
int pt_tcp_command(parseinfo *csb, translatetype *trans, int dir);
boolean pt_tcp_add_alias(translatetype *trans);
boolean pt_tcp_compare_entry(translatetype *trans1, 
				     translatetype *trans2);
boolean pt_tcp_delete_alias(translatetype *trans);
void pt_show_tcp_transinfo(translatetype *trans, int dir);
void pt_nv_tcp_transinfo(translatetype *trans, int dir);
char *pt_tcp_trans_string(int prot);
/*
 * pt_tcp_chain.c
 */
void pt_tcp_parser_init(void);
