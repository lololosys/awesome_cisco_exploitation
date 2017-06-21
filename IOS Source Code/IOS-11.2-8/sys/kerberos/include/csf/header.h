/* $Id: header.h,v 3.2 1995/11/17 17:47:23 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csf/header.h,v $
 *------------------------------------------------------------------
 * header.h - Meta-header file
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: header.h,v $
 * Revision 3.2  1995/11/17  17:47:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSF_HEADER_H_
#define _CSF_HEADER_H_ "%W% %G%"

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * All rights reserved.
 */

#ifndef CSF_CFG_BUILD_FILE
#define CSF_CFG_BUILD_FILE "../kerberos/include/csf/config/cisco.h"
#endif

#define CSF_BUILD_BEGIN
#include <csf/config0.h>
#undef CSF_BUILD_BEGIN

#define CSF_BUILD_HDRI
#include <csf/config0.h>
#undef CSF_BUILD_HDRI

#define CSF_BUILD_CFG
#include <csf/config0.h>
#undef CSF_BUILD_CFG

#include <csf/srcid.h>
#include <csf/legal.h>
#include <csf/type0.h>
#include <csf/crt0.h>
#include <csf/appss.h>
#include <csf/sts.h>
#include <csf/dbg.h>
#include <csf/crt1.h>
#include <csf/crtx.h>
#include <csf/crc.h>
#include <csf/md.h>
#include <csf/des.h>
#include <csf/desran.h>
#include <csf/net.h>
#ifdef CSF_HDRI_STDIO
#include <csf/file.h>
#endif

#define CSF_BUILD_END
#include <csf/config0.h>
#undef CSF_BUILD_END

#endif /*_CSF_HEADER_H_*/
