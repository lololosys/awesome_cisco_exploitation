/* $Id: rif_util.h,v 3.1.62.1 1996/03/18 20:14:42 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/if/rif_util.h,v $
 *------------------------------------------------------------------
 * rif_util.h - defines for rif utility routines in libutil.
 *
 * May 1995, Richard Livingston
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rif_util.h,v $
 * Revision 3.1.62.1  1996/03/18  20:14:42  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.28.1  1996/01/24  22:19:06  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.1  1995/11/09  11:52:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/08/07  05:11:06  richl
 * CSCdi38315:  makefile rework
 *              add new files for libraries
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * if/rif.c - rif utility routines
 * Note: this is NOT the place to store functions which
 * manipulate the RIF table for multiring.  We're trying
 * to achieve a clear distinction in use of the rif table for
 * proxy and multiring functions, and other code which must 
 * be rif-cognizant, but doesn't use the rif table.
 */
extern int bytes_to_rc_code(int bytes);
extern int rc_code_to_bytes(int code);
extern char *get_rif_str(uchar *ptr, char *buff);
extern void flip_rif(srbroute_t *source);
extern void make_forward_rif(srbroute_t *source, srbroute_t *dest);
extern boolean rif_contains_rif(srbroute_t *long_route,
				srbroute_t *short_route); 

#define RC_LFMASK	0x70
#define RC_LF516	0x00
#define RC_LF1500	0x10
#define RC_LF1470	0x10
#define RC_LF2052	0x20
#define RC_LF4472	0x30
#define RC_LF8144	0x40
#define RC_LF11407	0x50
#define RC_LF11454	0x50
#define RC_LF17800	0x60
#define RC_LFINITIAL	0x70

#define RC_LF_MIN	516
#define RC_LF516_VAL	516
#define RC_LF1470_VAL	1470
#define RC_LF1500_VAL	1500
#define RC_LF2052_VAL	2052
#define RC_LF4472_VAL	4472
#define RC_LF8144_VAL	8144
#define RC_LF11407_VAL	11407
#define RC_LF11454_VAL	11454
#define RC_LF17800_VAL	17800
#define RC_LF_MAX	17800
#define RC_LF_INFINITY	65535




