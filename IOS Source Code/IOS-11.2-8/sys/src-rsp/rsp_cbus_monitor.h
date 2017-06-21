/* $Id: rsp_cbus_monitor.h,v 3.1 1995/11/27 21:19:44 yanke Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-rsp/rsp_cbus_monitor.h,v $
 *------------------------------------------------------------------
 * CBus monitor header file.
 *
 * November 1995, Yan Ke
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_cbus_monitor.h,v $
 * Revision 3.1  1995/11/27  21:19:44  yanke
 * Initial commit of rsp cbus protection mechanism
 *
 * CSCdi44478:  slave rsp crashes or resets during ips eoir events
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern boolean rsp_cbus_check(void);
extern boolean rsp_cbus_check_read(void);
extern boolean rsp_cbus_monitored_copy_from_memd(void *srcptr, void *dstptr,
                                               int bytes);
extern boolean rsp_cbus_monitored_copy_to_memd(void *srcptr, void *dstptr,
                                               int bytes);
extern void cbus_timeout_handler(void);
extern void rsp_init_cbus_monitor(void);
