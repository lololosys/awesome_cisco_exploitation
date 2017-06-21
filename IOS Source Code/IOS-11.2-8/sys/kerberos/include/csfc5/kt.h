/* $Id: kt.h,v 3.2 1995/11/17 17:48:35 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csfc5/kt.h,v $
 *------------------------------------------------------------------
 * kt.h - Keytab public definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: kt.h,v $
 * Revision 3.2  1995/11/17  17:48:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_KT_H_
#define _CSFC5_KT_H_ "%W% %G%"
/*.original keytab.h 3.1 12/29/93*/

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * Copyright 1990 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

/*
 * Key table (KT) public definitions.
 */

#ifndef _CSFC5_KT_STS_HS_
#define _CSFC5_KT_STS_HS_
#include <csfc5/sts/kt_sts.hs>
#endif

#ifndef CSFC5_KT_TYPE_NULL
#define CSFC5_KT_TYPE_NULL		1
#endif

#ifndef CSFC5_KT_TYPE_MEM
#define CSFC5_KT_TYPE_MEM		2
#endif

#ifndef CSFC5_KT_TYPE_FILE
#define CSFC5_KT_TYPE_FILE		3
#endif

/*TBD/JNK--remove WFILE*/
#ifndef CSFC5_KT_TYPE_WFILE
#define CSFC5_KT_TYPE_WFILE		4
#endif


#ifndef csfC5_ktType
typedef csf_i16 csfC5_ktType;
#endif

typedef struct _csfC5_ktEntry {
	csfC5_prnId prnid;			/* Principal of key. */
	csfC5_tStamp tstamp;		/* Time written to keytable. */
	csfC5_crsKVNo kvno;		/* Key version number. */
	csfC5_crsKBlock key;		/* The key. */
} csfC5_ktEntry;

void csfC5free_ktEntry ( csfC5_ktEntry * );
void csfC5clear_ktEntry ( csfC5_ktEntry * );
csf_sts csfC5copy_ktEntry ( const csfC5_ktEntry *, csfC5_ktEntry * );


/* kt -- Visible portion of keytab. */
typedef struct _csfC5_kt {
	const char *name;			/* Name */
	const char *tname;			/* Type name */
	csfC5_ktType type;			/* Type. */
	csf_flags oflags;			/* Open flags. */
	csf_flags iflags;			/* Information flags. */
} csfC5_kt;

typedef void * csfC5_ktCursor;

/*TBD/JNK--open_kt -- replace resolve, initialize*/
#define CSFC5_KT_OPTS_CREATE		0x0001
#define CSFC5_KT_OPTS_CREATEIF		0x0002
#define CSFC5_KT_OPTS_WRITE			0x0004
#define CSFC5_KT_OPTS_TEMP			0x0008
#define CSFC5_KT_OPTS_LOCK			0x0010

typedef struct _csfC5_ktOpts {
	csfC5_prnId *prnid;
} csfC5_ktOpts;


csf_sts csfC5open_kt (
	csfC5_kt **kt,
	const char *name,
	csf_flags opts,
	const csfC5_ktOpts *ktopts
);

csf_sts csfC5resolve_kt (
	csfC5_kt **kt,
	const char *name
);

csf_sts csfC5close_kt (
	csfC5_kt **kt
);

csf_sts csfC5create_kt (
	csfC5_kt *kt
);

csf_sts csfC5getEntry_kt (
	csfC5_kt *kt,
	const csfC5_prnId *prn,
	csfC5_crsKVNo kvno,
	csfC5_ktEntry *ktentry
);

csf_sts csfC5startGetEntry_kt (
	csfC5_kt *kt,
	csfC5_ktCursor *curs
);

csf_sts csfC5nextGetEntry_kt (
	csfC5_kt *kt,
	csfC5_ktCursor *curs,
	csfC5_ktEntry *kte
);

csf_sts csfC5endGetEntry_kt (
	csfC5_kt *kt,
	csfC5_ktCursor *curs
);

csf_sts csfC5addEntry_kt (
	csfC5_kt *kt,
	const csfC5_ktEntry *kte
);

csf_sts csfC5removeEntry_kt (
	csfC5_kt *kt,
	const csfC5_ktEntry *kte
);

#endif /* _CSFC5_KT_H_ */
