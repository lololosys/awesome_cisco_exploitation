/* $Id: stun.h,v 3.2 1995/11/17 09:24:31 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ibm/stun.h,v $
 *------------------------------------------------------------------
 * stun.h -- Stun SPECIFIC definitions
 *
 * March 1991, Joel P. Bion
 *
 * Copyright (c) 1991-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: stun.h,v $
 * Revision 3.2  1995/11/17  09:24:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:46:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:49:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _STUN_H_
#define _STUN_H_

#define MAXDGRAM_STUN    2104	/* bytes, for buffer size checking */
#define DLU_TYPE_STUN  1      /* type for STUN for cls driver */ 
typedef struct _stun_protocol_list {
    struct _stun_protocol_list *next;
    NAMEDTHINGMACRO
    stun_protocol_info *q;
} stun_protocol_list;

/*
 * stun_chain.c
 */
extern void stun_parser_init(subsystype *subsys);
#endif
