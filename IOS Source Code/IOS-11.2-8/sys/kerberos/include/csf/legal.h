/* $Id: legal.h,v 3.2 1995/11/17 17:47:24 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csf/legal.h,v $
 *------------------------------------------------------------------
 * legal.h - Lots of legal garbage
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: legal.h,v $
 * Revision 3.2  1995/11/17  17:47:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSF_LEGAL_H_
#define _CSF_LEGAL_H_ "%W% %G%"

/*.Notice
 *
 * Copyright (C) 1994 CyberSAFE Corporation.
 * All rights reserved.
 *
 */

/*.Revision
6/15/94		Joe Kovara
	Original.
*/

/*.Preface LEGAL.H

This module contains macros for generating appropriate copyright
and notice strings for embedding in binaries, and the definition
of those copyrights and notices for CyberSAFE, OCSG, MIT and UCB.

The copyright macros take a name and a year; the notice macros
take a name.  The name is used as a suffix to define or refernce
a "char []" with the generated string.  All generated strings
have a "@(#)" in front of them so sccs/what can find them.  This
means they are not suitable for printing directly.  Multi-line
notices have new-lines embedded in them.

The reason for embedding this in macros is to allow us to easily
change the form and content of this stuff, and to control what
and how much gets embedded in binaries.  E.g., Programs may need
only one copyright notice which covers all modules; libraries
shipped with individually linkable modules may want copyrights
embedded in each module, with each module referencing a library
module which contains the notices.

Typical uses for these would be:

	1.	You want to embed copyrights or notices in the binaries:
			_csfCOPYRIGHTTXT( OCSG, 1991-1993 )
		generates:
			static char csf_copyright_OCSG [] =
				"@(#) Copyright (C) 1991-1993 ..."

	2.	You want to reference a copyright notice elsewhere:
			_csfCOPYRIGHTREF( OCSG )
		generates:
			extern char * csf_copyrights_OCSSG [];
			static char ** csf_copyrights_ref_OCSG =
				csf_copyrights_OCSG;

	The use of references is of questionable value (but they're
	there if needed).  The analogous macros for notices are
	probably more likely to be used.  E.g., a module in the
	library which defines each notice, with a reference in main
	to force them to be loaded.

	Note that a reference does not specify a year.  It is
	assumed that some other module contains an array of
	pointers to copyright strings for all of the appropriate
	years.  E.g.,
	- In main.c:
		char * csf_copyrights_OCSG [] = {
			_csfCOPYRIGHTTXT( OCSG, 1991 )
			_csfCOPYRIGHTTXT( OCSG, 1993 )
		}
	- In foo.c
		_csfCOPYRIGHTREF( OCSG )
	Thus, "foo.c" would contain a pointer to "csf_copyrights_OCSG",
	which would point to the approrpiate copyright strings.
*/

/* csfCOPYRIGHT__PREFIX -- Whatever the copyright form dujour is.
 * Used by the macros below.
 */
#ifndef csfCOPYRIGHT__PREFIX
#define csfCOPYRIGHT__PREFIX \
	"Copyright (c)"
#endif

/* _csfCOPYRIGHTTXT -- Generate copyright text for a given name & year.
 * Note that we define the string so that sccs/what can find it.
 */
#ifndef _csfCOPYRIGHTTXT
#define _csfCOPYRIGHTTXT( name, year ) \
	static const char csf_copyright_##name [] = \
		"@(#)" csfCOPYRIGHT__PREFIX #year csfCOPYRIGHT_##name ".";
#endif

/* _csfCOPYRIGHTREF -- Generate a reference to copyright text.  E.g., use
 * this in places where you don't want to replicate the string cuz it's
 * defined somewhere else (such as in the main module).  Note that this
 * does NOT use a year.
 */
#ifndef _csfCOPYRIGHTREF
#define _csfCOPYRIGHTREF( name ) \
	extern const char [] csf_copyright_##name; \
	static const char ** const csf_copyright_ref_##name= csf_copyright_##name;
#endif

/* _csfNOTICETXT -- Analagous to _csfCOPYRIGHTTXT, only for a notice.
 * As with copyright text, we define the string so that sccs/what
 * can find it.
 */
#ifndef _csfNOTICETXT
#define _csfNOTICETXT( name ) \
	static const char csf_notice_##name [] = \
		"@(#)" csfNOTICE_##name;
#endif

/* _csfNOTICEREF -- Analogouse to _csfCOPYRIGHTREF, only for a notice.
 */
#ifndef _csfNOTICEREF
#define _csfNOTICEREF( name ) \
	extern const char [] csf_notice_##name; \
	static const char * const csf_notice_ref_##name = csf_notice_##name;
#endif

/*
 * CyberSAFE.
 */

#ifndef csfCOPYRIGHT_CSF
#define csfCOPYRIGHT_CSF \
	"CyberSAFE Corporation"
#endif

#ifndef csfCOPYRIGHT_OCSG
#define csfCOPYRIGHT_OCSG \
	"Open Computing Security Group"
#endif

#ifndef csfNOTICE_CSF
#define csfNOTICE_CSF \
	"Licensed material property of CyberSAFE Corpration.\n" \
	"Use of this software or documentation is subject to a\n" \
	"license agreement, and may be used only in accordance\n" \
	"with the terms and conditions of that license agreement.\n"
#endif

/*
 * MIT.
 */

#ifndef csfCOPYRIGHT_MIT
#define csfCOPYRIGHT_MIT \
	"Massachusets Institute of Technology"
#endif

#ifndef csfNOTICE_MIT
#define csfNOTICE_MIT \
	"WITHIN THAT CONSTRAINT, permission to use, copy, modify, and\n" \
	"distribute this software and its documentation for any purpose and\n" \
	"without fee is hereby granted, provided that the above copyright\n" \
	"notice appear in all copies and that both that copyright notice and\n" \
	"this permission notice appear in supporting documentation, and that\n" \
	"the name of M.I.T. not be used in advertising or publicity pertaining\n" \
	"to distribution of the software without specific, written prior\n" \
	"permission.  M.I.T. makes no representations about the suitability of\n" \
	"this software for any purpose.  It is provided \"as is\" without express\n" \
	"or implied warranty.\n"
#endif

#ifndef csfNOTICE_MITEXPORT
#define csfNOTICE_MITEXPORT \
	"Export of this software from the United States of America may\n" \
	"require a specific license from the United States Government.\n" \
	"It is the responsibility of any person or organization contemplating\n" \
	"export to obtain such a license before exporting.\n"
#endif

/*
 * UCB.
 */

#ifndef csfCOPYRIGHT_UCB
#define csfCOPYRIGHT_UCB \
	"The Regents of the University of California"
#endif

#ifndef csfNOTICE_UCB
#define csfNOTICE_UCB \
	"Redistribution and use in source and binary forms are permitted\n" \
	"provided that the above copyright notice and this paragraph are\n" \
	"duplicated in all such forms and that any documentation,\n" \
	"advertising materials, and other materials related to such\n" \
	"distribution and use acknowledge that the software was developed\n" \
	"by the University of California, Berkeley.  The name of the\n" \
	"University may not be used to endorse or promote products derived\n" \
	"from this software without specific prior written permission.\n" \
	"THIS SOFTWARE IS PROVIDED \"AS IS\" AND WITHOUT ANY EXPRESS OR\n" \
	"IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED\n" \
	"WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.\n"
#endif

#endif /*_CSF_LEGAL_H_*/
