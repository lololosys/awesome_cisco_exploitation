/* $Id: sum.h,v 3.2 1995/11/17 18:52:09 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/os/sum.h,v $
 *------------------------------------------------------------------
 * sum.h -- perform checksum over given range of addresses
 *
 * Copyright (c) 1989-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sum.h,v $
 * Revision 3.2  1995/11/17  18:52:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:48:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:59:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#if !defined(__SUM_H__)
#define __SUM_H__

extern uint checksum_text_segment(void);
extern void validate_sum(void);
extern void defeat_sum(void);
extern void check_compile_time_sum(void);
extern void sum_init(void);

extern uint textsum;			/* Text segment checksum */

#define SIMPLE_CHECKSUM_ALGORITHM       1

#endif /* !defined(__SUM_H__) */
