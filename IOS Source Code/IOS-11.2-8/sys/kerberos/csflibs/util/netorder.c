/* $Id: netorder.c,v 3.2 1995/11/17 17:46:57 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/csflibs/util/netorder.c,v $
 *------------------------------------------------------------------
 * netorder.c - Misc functions to set net byte order
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: netorder.c,v $
 * Revision 3.2  1995/11/17  17:46:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:20:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * All Rights Reserved.
 */

#include <csf/header.h>

#ifndef csfNEThton32
csf_ui32 csfNEThton32 (
	csf_ui32 src
){
	union {
		csf_ui8 b[4];
		csf_ui32 l;
	} src1, dst;

	src1.l = src;
	dst.b[3] = src1.b[0];
	dst.b[2] = src1.b[1];
	dst.b[1] = src1.b[2];
	dst.b[0] = src1.b[3];

	return dst.l;
}
#endif

#ifndef csfNETntoh32
csf_ui32 csfNETntoh32 (
	csf_ui32 src
){
   return csfNEThton32( src );
}
#endif

#ifndef csfNEThton16
csf_ui16 csfNEThton16 (
	csf_ui16 src
){
	union {
		csf_ui8 b[2];
		csf_ui16 s;
	} src1, dst;

	src1.s = src;
	dst.b[1] = src1.b[0];
	dst.b[0] = src1.b[1];

	return dst.s;
}
#endif

#ifndef csfNETntoh16
csf_ui16 csfNETntoh16 (
	csf_ui16 src
){
   return csfNEThton16( src );
}
#endif
