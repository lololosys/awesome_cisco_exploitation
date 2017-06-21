/* $Id: lex_public.h,v 3.2 1995/11/17 17:43:12 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lex/lex_public.h,v $
 *------------------------------------------------------------------
 * lex_public.h -- LEX public definitions
 *
 * May 1994, David Coli
 *
 * Copyright (c) 1994 by Cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: lex_public.h,v $
 * Revision 3.2  1995/11/17  17:43:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:35:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:40:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Media types supported by Lex.
 */
#define LEX_MEDIA_ETHER         0x01

#define PPP_LEX_ENCAPBYTES	6

#define REMOVE_PPP_LEX_HEADER(pak) \
                             (pak)->datagramstart += PPP_LEX_ENCAPBYTES;\
                             (pak)->datagramsize -= PPP_LEX_ENCAPBYTES;
#define ADD_PPP_LEX_HEADER(pak) \
	                 (pak)->datagramstart -= PPP_LEX_ENCAPBYTES;\
	                 (pak)->datagramsize += PPP_LEX_ENCAPBYTES;

/* External declarations */

extern boolean lex_rcmd_debug;
