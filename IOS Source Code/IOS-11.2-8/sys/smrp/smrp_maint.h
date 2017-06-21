/* $Id: smrp_maint.h,v 3.2 1995/11/17 18:53:47 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/smrp/smrp_maint.h,v $
 *------------------------------------------------------------------
 * Maintenance routines for SMRP.
 *
 * April 1995, Steven Lin
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smrp_maint.h,v $
 * Revision 3.2  1995/11/17  18:53:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:13:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:45:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __SMRP_MAINT_H__
#define __SMRP_MAINT_H__
/*
 *------------------------------------------------------------------
 * SMRP Network Layer Entry Points defined by smrp_maint.
 *------------------------------------------------------------------
 */


/*
 *------------------------------------------------------------------
 * SMRP Internal Entry Points defined by smrp_maint.
 *------------------------------------------------------------------
 */
extern void smrp_maint_Initialize(void);

extern void smrp_maint(void);

#endif __SMRP_MAINT_H__
