/* $Id: ipaccess2.h,v 3.1.2.3 1996/07/29 22:45:46 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipaccess2.h,v $
 *------------------------------------------------------------------
 * ipaccess2.h -- Extended Access Lists Definitions
 *
 * March 1996, Greg Christy
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipaccess2.h,v $
 * Revision 3.1.2.3  1996/07/29  22:45:46  snyder
 * CSCdi64484:  declare arrays const in sub_ipcore
 *              moves 1344 out of data, 20 out of image
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/05/08  01:09:46  gchristy
 * CSCdi56944:  Compilation errors from IP named access list commit
 * Branch: California_branch
 * - Modularize new ACL code for inclusion in VIP images.
 *
 * Revision 3.1.2.1  1996/05/04  08:00:30  wilber
 * Branch: California_branch
 *
 * Name Access List
 *
 * Revision 3.1  1996/05/04  07:48:42  speakman
 * Two more (the last two?) missing placeholders for
 * IP Named Access Lists.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern const id_string proto_items[];
extern const id_string ip2access_cmds[];
extern const id_string icmptypes[];
extern const id_subid_string icmptypecodes[];
extern const id_string igmptypes[];
extern const id_string old_proto_items[];

extern dynlisttype *ipaccess_get_listbyname(char *);
extern void ipaccess_remove_dynacl (dynlisttype *);
