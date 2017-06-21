/* $Id: sm_exec.h,v 3.2 1995/11/17 08:49:25 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/bstun/sm_exec.h,v $
 *------------------------------------------------------------------
 * Prototype definition for general purpose FSM caller.
 *
 * February 1995, Andrew R. Rothwell.
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sm_exec.h,v $
 * Revision 3.2  1995/11/17  08:49:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:04:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:14:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _SM_EXEC_H
#define _SM_EXEC_H

#include "cape.h"

int
sm_exec (SM* psm,
         int* curstate,
         int event,
         void* pdata);

#endif  /* _SM_EXEC_H. */
