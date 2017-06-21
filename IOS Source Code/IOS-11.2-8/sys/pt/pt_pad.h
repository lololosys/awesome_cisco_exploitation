/* $Id: pt_pad.h,v 3.2 1995/11/17 18:59:49 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/pt/pt_pad.h,v $
 *------------------------------------------------------------------
 * X.25 PAD specific definitions and prototypes
 *
 * November 1993, Eman Schaffer, Scott Mackie
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: pt_pad.h,v $
 * Revision 3.2  1995/11/17  18:59:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:01:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:35:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Prototypes
 */

/*
 * pt_pad.c
 */
void pt_pad_init(subsystype *subsys);
int pt_pad_command(parseinfo *csb, translatetype *trans,
			    int dir);
boolean pt_pad_add_alias(translatetype *trans);
boolean pt_pad_compare_entry(translatetype *trans1, 
				     translatetype *trans2);
boolean pt_pad_delete_alias(translatetype *trans);
boolean x25pvc_cfork(translatetype *trans);
void pt_show_pad_transinfo(translatetype *trans, int dir);
void pt_nv_pad_transinfo(translatetype *trans, int dir);
char *pt_pad_trans_string (int prot);

/*
 * pt_pad_chain.c
 */
void pt_pad_parser_init(void);

