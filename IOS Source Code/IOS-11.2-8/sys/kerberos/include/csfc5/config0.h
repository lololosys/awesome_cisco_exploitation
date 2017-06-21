/* $Id: config0.h,v 3.2 1995/11/17 17:47:51 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csfc5/config0.h,v $
 *------------------------------------------------------------------
 * config0.h - Meta-configuration file
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: config0.h,v $
 * Revision 3.2  1995/11/17  17:47:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_CONFIG0_H_
#define _CSFC5_CONFIG0_H_ "%W% %G%"
#endif

#ifdef CSFC5_CFG_BUILD_FILE
#include CSFC5_CFG_BUILD_FILE
#else
#include <csfc5/configx.h>
#endif
