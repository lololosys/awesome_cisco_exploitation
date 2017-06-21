/* $Id: configx.h,v 3.2 1995/11/17 17:47:35 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csf/config/configx.h,v $
 *------------------------------------------------------------------
 * configx.h - CSF meta-config file
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: configx.h,v $
 * Revision 3.2  1995/11/17  17:47:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSF_CONFIGX_H_
#define _CSF_CONFIGX_H_ "%W% %G%"
#endif

/*---------------------------------------------*/
#ifdef CSF_BUILD_BEGIN
#endif /* CSF_BUILD_BEGIN */
/*---------------------------------------------*/

/*---------------------------------------------*/
#ifdef CSF_BUILD_HDRI

#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#ifdef CSF_HDRI_STDIO
#include <stdio.h>
#endif /* CSF_HDRI_STDIO */

#ifdef CSF_HDRI_ERRNO
#include <errno.h>
#endif /* CSF_HDRI_ERRNO */

#ifdef CSF_HDRI_FILEIO
#include <stdio.h>
/*** As required ****/
#endif /* CSF_HDRI_FILEIO */

#ifdef CSF_HDRI_NET
/*** As required ***/
#endif /* CSF_HDRI_NET */

#endif /* CSF_BUILD_HDRI */
/*---------------------------------------------*/

/*---------------------------------------------*/
#ifdef CSF_BUILD_CFG

#define CSF_CFG0_M_WORDSIZE		XXX
#define CSF_CFG0_M_LSBF			XXX
#define CSF_CFG0_M_MSBF			XXX
#define CSF_CFG0_M_RALIGN		XXX

/* Base value for status codes--ALL csf status codes. */
#ifndef CSF_CFG_STSBASE
#define CSF_CFG_STSBASE			4096
#endif

/* Turn off module source identification by default. */
#ifndef csfSRCIDGEN_
#define csfSRCIDGEN_ 0
#endif

#endif /* CSF_BUILD_CFG */
/*---------------------------------------------*/

/*---------------------------------------------*/
#ifdef CSF_BUILD_END
#endif /* CSF_BUILD_END */
/*---------------------------------------------*/
