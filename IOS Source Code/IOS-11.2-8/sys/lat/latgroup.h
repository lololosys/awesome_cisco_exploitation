/* $Id: latgroup.h,v 3.3 1995/12/17 18:32:04 thille Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/lat/latgroup.h,v $
 *------------------------------------------------------------------
 * latgroup.h - LAT group code definitions
 *
 * July 4, 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: latgroup.h,v $
 * Revision 3.3  1995/12/17  18:32:04  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  17:33:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:26:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:25:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define LATGROUPNAMELEN 20

typedef struct latgroup_ {
    struct latgroup_ *next;
    char name[LATGROUPNAMELEN+1];
    byte groups[GROUPS];
} latgroup;

extern latgroup *lat_find_group(char *);
extern boolean lat_parse_grouplist(parseinfo *, byte *, byte *);
extern void lat_apply_groupmask(byte *, byte *, byte *);
extern latgroup *lat_mask_group(uchar *);
extern void nv_lat_groupmask(char *, byte *, boolean, boolean);
extern boolean lat_compare_groups(byte *, byte *);
extern int print_lat_groups(uchar *, int, int, boolean);
extern boolean match_lat_group_name(char *buf, int *pi, char *num, char *name);

extern queuetype latgrpQ;
