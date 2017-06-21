/* $Id: rsp_commands.h,v 3.2 1995/11/17 18:47:54 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-rsp/rsp_commands.h,v $
 *------------------------------------------------------------------
 * Include file that provides prototypes for rsp_command.c
 *
 * April 1995, David Getchell
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_commands.h,v $
 * Revision 3.2  1995/11/17  18:47:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:27:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:28:14  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/09/17  20:55:24  motto
 * CSCdi40168:  IPC does not work with a CIP in a RSP (c75xx) router
 *
 * Revision 2.2  1995/07/16  22:30:56  getchell
 * CSCdi35501:  src-rsp/ccb_cmd.h should be moved to hes
 * src-rsp/[ccb.h,ccb_cmd.h] combined and moved to hes/ccb_cmd.h.
 * rsp_command prototypes removed from ccb_cmd.h and moved to
 * src-rsp/rsp_commands.h.  Files including altered files modifed to
 * reflect new file structure.
 *
 * Revision 2.1  1995/06/07  22:57:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __RSP_COMMAND_H__
#define __RSP_COMMAND_H__

/*
 * External referencces to functions for issuing commands and getting
 * responses 
 */
extern void send_if_cmd (hwidbtype* idb, ushort cmnd);
extern void send_if_cmd_1arg (hwidbtype* idb, ushort cmnd, ushort arg0);
extern void send_if_cmd_2arg (hwidbtype* idb, ushort cmnd, 
			      ushort arg0, ushort arg1);
extern void send_if_cmd_3arg (hwidbtype* idb, ushort cmnd, 
			      ushort arg0, ushort arg1, ushort arg2);
extern void send_if_cmd_larg (hwidbtype* idb, ushort cmnd, ulong larg);

extern ushort get_if_status (hwidbtype* idb);

extern ushort get_if_val (hwidbtype* idb);
extern ulong get_if_lval (hwidbtype* idb);

extern void send_ip_cmd (slottype *slotp, ushort cmnd);
extern void send_ip_cmd_1arg (slottype *slotp, ushort cmnd, ushort arg0);
extern void send_ip_cmd_2arg (slottype *slotp, ushort cmnd, 
			      ushort arg0, ushort arg1);
extern void send_ip_cmd_3arg (slottype *slotp, ushort cmnd, 
			      ushort arg0, ushort arg1, ushort arg2);
extern void send_ip_cmd_4arg (slottype *slotp, ushort cmnd, 
			      ushort arg0, ushort arg1, ushort arg2,
			      ushort arg3);
extern void send_ip_cmd_larg (slottype *slotp, ushort cmnd, ulong larg);
extern void send_ip_cmd_2larg (slottype *slotp, ushort cmnd,
			       ulong larg0, ulong larg1);

extern ushort get_ip_status (slottype *slotp);

extern ushort get_ip_val (slottype *slotp);
extern ulong get_ip_lval (slottype *slotp);

extern ushort get_ip_diag0 (slottype* slotp);
extern ushort get_ip_diag1 (slottype* slotp);

extern void send_ip_ccb_ptr (slottype* slotp);
extern ushort get_ip_ccb_status (slottype* slotp);

extern ushort read_ccb_done(struct ccbtype_ *ccb);

#endif  /* __RSP_COMMAND_H__ */
