/* $Id: parser_defs_sse.h,v 3.2 1995/11/17 18:52:45 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/parser/parser_defs_sse.h,v $
 *------------------------------------------------------------------
 * parser_defs_sse.h - Definitions for driving the parser SSE commands 
 *
 * Sept., 1992 Tony Li
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_defs_sse.h,v $
 * Revision 3.2  1995/11/17  18:52:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:56:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/25  08:36:27  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.1  1995/06/07  22:31:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define SSE_CMD_ADDRESS		0
#define SSE_CMD_INTERFACE	1
#define SSE_CMD_INTERNALS	2
#define SSE_CMD_MEMORY		3
#define SSE_CMD_REWRITE		4
#define SSE_CMD_SUMMARY		6
#define SSE_CMD_CONSISTENCY	7
#define SSE_CMD_MEMFREE		8
#define SSE_CMD_MATCH		9
#define SSE_CMD_ACCESS_INPUT   10
#define SSE_CMD_ACCESS_LIST    11

/*
 * NB: these get used to form a bitmask elsewhere.
 */
#define SSE_PROTOCOL_ANY	0
#define SSE_PROTOCOL_IP		1
#define SSE_PROTOCOL_IPX	2
#define SSE_PROTOCOL_SRB	3
#define SSE_PROTOCOL_CLNS	4
#define MIN_SSE_REAL_PROTOCOL	SSE_PROTOCOL_IP
#define MAX_SSE_PROTOCOL	SSE_PROTOCOL_CLNS

#define SSE_TEST_MANAGER	0
#define SSE_TEST_MEMORY		1
#define SSE_TEST_REWRITE	2
#define SSE_TEST_MICROCODE	3
#define SSE_TEST_SANITY		4

#define SSE_MANAGER_QUIET	0
#define SSE_MANAGER_PERIOD	1
