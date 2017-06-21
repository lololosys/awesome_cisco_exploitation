/* $Id: cls_assert.h,v 3.2 1995/11/17 08:54:41 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/cls/cls_assert.h,v $
 *------------------------------------------------------------------
 * cls_assert.h -- Debugging declarations for CLS
 *
 * April 1993, Claude Alan Cartee
 *
 * Copyright (c) 1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cls_assert.h,v $
 * Revision 3.2  1995/11/17  08:54:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:10:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:19:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __CLS_ASSERT_H__
#define __CLS_ASSERT_H__


#define CLS_ALWAYS_CHECK_SANITY

/*
 * Check that the specified expression is true, if it isn't then
 * CLSASSERT() will print a message, and breakpoint(). A call to
 * CLSASSERT_FATAL() will crashdump().
 */
#ifdef CLS_ALWAYS_CHECK_SANITY

void cls_assert(char* file,int line, char *exp, boolean fatal);
#define CLSASSERT(exp) {if (!(exp)) {reg_invoke_cls_assert(__FILE__,__LINE__,#exp,FALSE);}}
#define CLSASSERT_FATAL(exp) {if (!(exp)) {reg_invoke_cls_assert(__FILE__,__LINE__,#exp,TRUE);}}
#else
#define CLSASSERT(exp)
#define CLSASSERT_FATAL(exp)
#endif

#endif
