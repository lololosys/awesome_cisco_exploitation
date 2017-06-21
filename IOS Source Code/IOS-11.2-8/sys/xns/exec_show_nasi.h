/* $Id: exec_show_nasi.h,v 3.2 1995/12/14 23:05:34 irfan Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/xns/exec_show_nasi.h,v $
 *------------------------------------------------------------------
 * exec_show_nasi.h   NASI  parser chain for "show nasi" command
 *
 * Sept 1995, Syed Mohammad Irfan Ashraf
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_nasi.h,v $
 * Revision 3.2  1995/12/14  23:05:34  irfan
 * CSCdi43135:  show ipx nasi connections doesnt exist.
 * File doesn't contain the expected parse chain.
 *
 * Revision 3.1  1995/11/09  13:58:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/26  12:13:08  irfan
 * Netware Asynchronous Services Interface: holding area
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*************************
 * show ipx nasi con
 *
 *************************
 */

EOLS    (show_nasi_con_eol, show_nasi, SHOW_NASI_CON);

KEYWORD (show_nasi_con, show_nasi_con_eol, no_alt,
         "connections", "Show NASI connections",
         PRIV_ROOT);

KEYWORD (show_nasi, show_nasi_con, ALTERNATE,
         "nasi", "Netware Asynchronous Services Interface status",
         PRIV_ROOT);

#undef  ALTERNATE
#define ALTERNATE       show_nasi
