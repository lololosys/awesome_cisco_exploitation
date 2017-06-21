/* $Id: header.h,v 3.2 1995/11/17 17:47:57 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csfc5/header.h,v $
 *------------------------------------------------------------------
 * header.h - Meta-header file
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: header.h,v $
 * Revision 3.2  1995/11/17  17:47:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_HEADER_H_
#define _CSFC5_HEADER_H_  "%W% %G%"

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * All Rights Reserved.
 */

#ifndef CSFC5_CFG_BUILD_FILE
#define CSFC5_CFG_BUILD_FILE "../kerberos/include/csfc5/config/es.h"
#endif

#define CSFC5_BUILD_BEGIN
#include <csfc5/config0.h>
#undef CSFC5_BUILD_BEGIN

#ifdef CSFC5_HDRI_NET
#define CSF_HDRI_NET 1
#endif

#include <csf/header.h>

#include <csfc5/rfc1510.h>
#if 0
#include <csfc5/rfc1510e.h>		/*Unused. See c5c (vs. c5) status. */
#endif

#define CSFC5_BUILD_CFG 1
#include <csfc5/config0.h>
#undef CSFC5_BUILD_CFG

#include <csfc5/type0.h>

#include <csfc5/sts.h>
#include <csfc5/cks.h>
#include <csfc5/crs.h>
#include <csfc5/net.h>
#include <csfc5/prn.h>
#include <csfc5/pauth.h>
#include <csfc5/authz.h>
#include <csfc5/trans.h>
#include <csfc5/autht.h>
#include <csfc5/tkt.h>
#include <csfc5/lreq.h>
#include <csfc5/msgkdc.h>
#include <csfc5/msgap.h>
#include <csfc5/msgerr.h>
#include <csfc5/msgsp.h>
#include <csfc5/msgcred.h>

#include <csfc5/asn.h>

#include <csfc5/creds.h>
#include <csfc5/rlmtre.h>
#include <csfc5/cc.h>
#include <csfc5/rc.h>
#include <csfc5/kt.h>

#include <csfc5/libz.h>
#include <csfc5/libx.h>
#include <csfc5/appx.h>

#include <csfc5/host.h>

#define CSFC5_BUILD_END 1
#include <csfc5/config0.h>
#undef CSFC5_BUILD_END

#endif
