/* $Id: pt_autocommand.h,v 3.2 1995/11/17 18:59:28 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/pt/pt_autocommand.h,v $
 *------------------------------------------------------------------
 * AUTOCOMMAND specific definitions and prototypes
 *
 * November 1994, Eman Schaffer
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: pt_autocommand.h,v $
 * Revision 3.2  1995/11/17  18:59:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:01:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/15  18:54:42  eschaffe
 * CSCdi40294:  Increase the number of Protocol Translation sessions
 *
 * Revision 2.1  1995/06/07  22:34:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Prototypes
 */

/*
 * pt_autocomand.c
 */
void pt_autocommand_init(subsystype *subsys);
int pt_autocommand_command(parseinfo *csb, translatetype *trans, int dir);
void pt_show_autocommand_transinfo(translatetype *trans, int dir);
void pt_nv_autocommand_transinfo(translatetype *trans, int dir);
char *pt_auto_trans_string(int prot);
/*
 * pt_auto_chain.c
 */
void pt_autocommand_parser_init(void);
/*
 * transauto.c
 */
void pt_transauto_init(void);
void pt_setup_auto_session(translatetype *trans, tt_soc *vty);
void pt_terminate_helper(tt_soc *vty);
/*
 * tcpauto.c
 */
void pt_tcpauto_init(subsystype *subsys);
int tcp_transto_auto(translatetype *trans, tt_soc *vty);
/*
 * padauto.c
 */
void pt_padauto_init(subsystype *subsys);
int pad_transto_auto(translatetype *trans, tt_soc *vty);
/*
 * latauto.c
 */
void pt_latauto_init(subsystype *subsys);
int lat_transto_auto(translatetype *trans, tt_soc *vty);
/*
 * vtyarap.c
 */
void pt_vtyarap_init(void);
void pt_vtyarap_connection(tt_soc *vty);

extern translatetype **trans_map;
