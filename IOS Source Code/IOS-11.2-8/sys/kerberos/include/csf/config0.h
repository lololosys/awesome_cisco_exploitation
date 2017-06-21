/* $Id: config0.h,v 3.2 1995/11/17 17:47:12 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csf/config0.h,v $
 *------------------------------------------------------------------
 * config0.h - CSF meta-config file
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: config0.h,v $
 * Revision 3.2  1995/11/17  17:47:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:20:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSF_CONFIG0_H_
#define _CSF_CONFIG0_H_ "%W% %G%"
#endif

#ifdef CSF_CFG_BUILD_FILE
#include CSF_CFG_BUILD_FILE
#else
#include <csf/configx.h>
#endif
