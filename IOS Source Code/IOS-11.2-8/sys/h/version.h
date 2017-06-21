/* $Id: version.h,v 3.2 1995/11/17 09:13:54 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/h/version.h,v $
 *------------------------------------------------------------------
 * Declarations used in the cisco*.c files for version information
 *
 * November 1994, Mark D. Baushke
 *
 * Copyright (c) 1994-95 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: version.h,v $
 * Revision 3.2  1995/11/17  09:13:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:36:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:37:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __VERSION_H__
#define __VERSION_H__

/*
 * The maximum size of the description strings used for ciscoversion,
 * oemversion and cw_sysdescr
 */
#ifndef CW_MAX_DESCR_STRLEN
#define CW_MAX_DESCR_STRLEN 400
#endif

/*
 * The maximum size of the CiscoWorks special strings that may get
 * patched during the release process.
 */
#ifndef CW_MAX_STRLEN
#define CW_MAX_STRLEN 64
#endif

/*
 * The external version strings for cisco products. The definition for
 * these strings is found in the appropriate cisco*.c files.
 */
extern char *ciscoversion;	/* version strings */
extern char *oemversion;	/* version strings */

/*
 * A NULL terminated list of CiscoWorks special strings. 
 */
#ifndef cw_struct_
#define cw_struct_
struct cw_struct
{
    char *ptr;
    unsigned short size; 
};
#endif
extern struct cw_struct cw_table[];
extern int cw_table_cnt;

/* 
 * The following constants should be used to index into the 'cw_table' struct
 * defined above. __CW_BEGIN and __CW_END are intentionally missing to allow for
 * structure growth.
 */
#define CW_TABLE__CW_IMAGE      1
#define CW_TABLE__CW_FAMILY     2
#define CW_TABLE__CW_FEATURE    3
#define CW_TABLE__CW_VERSION    4
#define CW_TABLE__CW_MEDIA      5
#define CW_TABLE__CW_SYSDESCR   6


#endif /* __VERSION_H__ */
