/* $Id: pt_lat.h,v 3.2 1995/11/17 18:59:39 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/pt/pt_lat.h,v $
 *------------------------------------------------------------------
 * LAT specific definitions and prototypes
 *
 * November 1993, Eman Schaffer, Scott Mackie
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: pt_lat.h,v $
 * Revision 3.2  1995/11/17  18:59:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:01:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:35:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Macros
 */

#define	STRING_CHECK(src, dst, len, str)\
    if ((dst[0] = strlen(src)) > (len-1)) {\
	printf("\nTranslate: LAT %s name too long", str);\
        return(TRANS_NONE);\
    } else { \
	uppercase(src); \
	bcopy(src, &dst[1], dst[0]); \
    }

/*
 * Prototypes
 */

/*
 * pt_lat.c
 */
void pt_lat_init(subsystype *subsys);
int pt_lat_command(parseinfo *csb, translatetype *trans, int dir);
boolean pt_lat_add_alias(translatetype *trans);
boolean pt_lat_compare_entry(translatetype *trans1, 
				     translatetype *trans2);
boolean pt_lat_delete_alias(translatetype *trans);
void pt_show_lat_transinfo(translatetype *trans, int dir);
void pt_nv_lat_transinfo(translatetype *trans, int dir);
char *pt_lat_trans_string(int prot);

/*
 * pt_lat_chain.c
 */
void pt_lat_parser_init(void);



