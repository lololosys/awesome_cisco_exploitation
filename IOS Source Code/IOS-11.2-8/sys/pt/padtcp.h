/* $Id: padtcp.h,v 3.2.62.1 1996/07/12 20:09:25 widmer Exp $
 * $Source: /release/112/cvs/Xsys/pt/padtcp.h,v $
 *------------------------------------------------------------------
 * PAD/TCP conversion specific definitions and prototypes
 *
 * December 1993, Eman Schaffer Scott Mackie
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: padtcp.h,v $
 * Revision 3.2.62.1  1996/07/12  20:09:25  widmer
 * CSCdi62775:  Bogus externs in telnet.c cause 4k builds to fail
 * Branch: California_branch
 * Remove externs and move externs to header files
 *
 * Revision 3.2  1995/11/17  18:59:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:01:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:34:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Prototypes
 */

/*
 * padtcp.c
 */
void run_padtcp(translatetype *trans, tt_soc *tty);
boolean padtcp_tnoptioncheck(conntype *conn, uchar code);
boolean padtcp_padoptioncheck(conntype *conn, int param, int value);
void padtcp_tnconfirm(conntype *conn, ulong bit, ulong state, ulong code);
int x25_transto_tcp(translatetype *trans, tt_soc *tty);

/*
 * tcppad.c
 */
boolean tcppadBLOCK(register tt_soc *tty);
void run_tcppad(translatetype *trans, tt_soc *tty);
int tcppad_getc(conntype *conn);
boolean tcppad_tnoptioncheck(conntype *conn, uchar code);
boolean tcppad_padoptioncheck(conntype *conn, int param, int value);
void tcppad_tnconfirm(conntype *conn, ulong bit, ulong state, ulong code);
int tcp_transto_x25(translatetype *trans, tt_soc *tty);




