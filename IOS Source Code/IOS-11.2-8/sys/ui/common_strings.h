/* $Id: common_strings.h,v 3.2.32.2 1996/07/06 05:57:38 thille Exp $
 * $Source: /release/112/cvs/Xsys/ui/common_strings.h,v $
 *------------------------------------------------------------------
 * C O M M O N _ S T R I N G S . H
 *
 * February, 1996, Nick Thille
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: common_strings.h,v $
 * Revision 3.2.32.2  1996/07/06  05:57:38  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.2.32.1  1996/05/11  16:52:02  thille
 * CSCdi51398:  Duplicate strings in ping / trace
 * Branch: California_branch
 * Every 116 bytes counts.
 *
 * Revision 3.2  1996/02/08  20:13:37  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.1  1996/02/08  00:12:08  thille
 * CVS:
 * ----------------------------------------------------------------------
 *
 * Add placeholder for the shared strings work.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(__COMMON_STRING_H__)
#define __COMMON_STRING_H__

/*
 * common words that are complete strings
 */
extern const char common_str_empty_str[];
extern const char common_str_interface[];
extern const char common_str_unknown[];
extern const char common_str_protocol[];
extern const char common_str_invalid[];
extern const char common_str_unreachable[];

/*
 * common strings
 */
extern const char ping_sending_msg[];
extern const char ping_timeout_msg[];
extern const char type_escape_seq_abort_msg[];
extern const char tracing_the_route_to_msg[];
extern const char if_is_line_proto_is_msg[];
extern const char subsys_req_iphost[];
extern const char subsys_seq_iphost[];

#endif /* !defined(__COMMON_STRING_H__) */
