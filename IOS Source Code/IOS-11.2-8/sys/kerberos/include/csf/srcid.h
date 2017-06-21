/* $Id: srcid.h,v 3.2 1995/11/17 17:47:28 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csf/srcid.h,v $
 *------------------------------------------------------------------
 * srcid.h - SRCIDGEN definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: srcid.h,v $
 * Revision 3.2  1995/11/17  17:47:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSF_SRCID_H_
#define _CSF_SRCID_H_ "%W% %G%"

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * All rights reserved.
 */

/*.Revision
6/15/94		Joe Kovara
	Original.
*/

/* csfSRCIDGEN_ - Do/don't generate ident string.
 * Default is to generate source module ident string.
 */
#ifndef csfSRCIDGEN_
#define csfSRCIDGEN_ 1
#endif

/* csfSRCIDSTR - Default name of string variable to hold module ident.
 */
#ifndef csfSRCIDSTR
#define csfSRCIDSTR csfSRCIDstr0
#endif

/* _csfSRCID( idstr ) -- Generate source module identifier string.
 * This is for use by source modules for embedding it's identifier
 * string in the binary (if desired).
 */
#ifndef _csfSRCID
#define _csfSRCID( idstr ) \
	static const char csfSRCIDSTR [] = idstr " (CyberSAFE/OCSG)";
#endif

/* If a source identifier is defined and ident generation is enabled,
 * generate the string and set "csfSRCIDENT" to the name of the string.
 * Else just set "csfSRCIDENT" to "__FILE__".
 *
 * We do this because other's may want to know what to use for the module
 * ident string (e.g., for debugging).  It is generally preferable to use
 * the source ident string, as this will consume less space when using
 * compilers that don't collapse duplicate strings.
 */
#if defined( csfSRCID ) && csfSRCIDGEN_
_csfSRCID( csfSRCID );
#define csfSRCIDENT csfSRCIDSTR
#else
#define csfSRCIDENT __FILE__
#endif

#endif /*_CSF_SRCID_H_*/
