/* $Id: sysfnc.h,v 3.2.62.1 1996/06/28 23:20:39 hampton Exp $
 * $Source: /release/112/cvs/Xsys/lat/sysfnc.h,v $
 *
 * Copyright (c) 1993-1994,1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sysfnc.h,v $
 * Revision 3.2.62.1  1996/06/28  23:20:39  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:35:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:28:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:29:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    sysfnc		lat system function prototypes

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
    Define advertised service functions.
 -----------------------------------------------------------------------*/
#define as_alloc()	(as_ptr)malloc(sizeof(as_node))
#define as_free(as)	free(as)

#define as_str_diff_sl(dst_s,src_l)	str_diff(dst_s,src_l)
#define as_str_move_os(dst_o,src_s)	str_move(dst_o,src_s)
#define as_str_move_sl(dst_s,src_l)	str_move(dst_s,src_l)

/*----------------------------------------------------------------------
    Define circuit block functions.
 -----------------------------------------------------------------------*/
#define cb_alloc()	(cb_ptr)malloc(sizeof(cb_node))
#define cb_free(cb)	free(cb)

/*----------------------------------------------------------------------
    Define host-initiated functions.
 -----------------------------------------------------------------------*/
#define hi_alloc()	(hi_ptr)malloc(sizeof(hi_node))
#define hi_free(hi)	free(hi)

#define hi_str_move_ls(dst_l,src_s)	str_move(dst_l,src_s)
#define hi_str_move_os(dst_o,src_s)	str_move(dst_o,src_s)
#define hi_str_move_sl(dst_s,src_l)	str_move(dst_s,src_l)
#define hi_str_equal_sl(dst_s,src_l)    str_equal(dst_s,src_l)
#define hi_str_equal_ss(dst_s,src_s)    str_equal(dst_s,src_s)

/*----------------------------------------------------------------------
    Define host structure functions.
 -----------------------------------------------------------------------*/
#define hs_str_move_os(dst_o,src_l)	str_move(dst_o,src_l)
#define hs_str_equal_sl(dst_s,src_l)	str_equal(dst_s,src_l)

/*----------------------------------------------------------------------
    Define learned service functions.
 -----------------------------------------------------------------------*/
#define ls_alloc()	(ls_ptr)malloc(sizeof(ls_node))
#define ls_free(ls)	free(ls)

#define ls_str_diff_sl(dst_s,src_l)	str_diff(dst_s,src_l)
#define ls_str_move_os(dst_o,src_s)	str_move(dst_o,src_s)
#define ls_str_move_sl(dst_s,src_l)	str_move(dst_s,src_l)
#define ls_str_equal_sl(dst_s,src_l)    str_equal(dst_s,src_l)
#define ls_str_equal_ss(dst_s,src_s)    str_equal(dst_s,src_s)

/*----------------------------------------------------------------------
    Define remote node functions.
 -----------------------------------------------------------------------*/
#define nd_alloc()	(nd_ptr)malloc(sizeof(nd_node))
#define nd_free(nd)	free(nd)

#define nd_str_diff_sl(dst_s,src_l)	str_diff(dst_s,src_l)
#define nd_str_diff_ss(dst_s,src_s)	str_diff(dst_s,src_s)
#define nd_str_move_os(dst_o,src_s)	str_move(dst_o,src_s)
#define nd_str_move_sl(dst_s,src_l)	str_move(dst_s,src_l)
#define nd_str_equal_sl(dst_s,src_l)    str_equal(dst_s,src_l)
#define nd_str_move_ls(dst_l,src_s)	str_move(dst_l,src_s)
/*----------------------------------------------------------------------
    Define object functions.
 -----------------------------------------------------------------------*/
#define ob_alloc()	(ob_ptr)malloc(sizeof(ob_node))
#define ob_free(ob)	free(ob)

#define ob_str_diff_sl(dst_s,src_l)	str_diff(dst_s,src_l)
#define ob_str_move_os(dst_o,src_s)	str_move(dst_o,src_s)
#define ob_str_move_sl(dst_s,src_l)	str_move(dst_s,src_l)
#define ob_str_equal_sl(dst_s,src_l)    str_equal(dst_s,src_l)

extern byte ob_temp[];

/*----------------------------------------------------------------------
    Define queued requests functions.
 -----------------------------------------------------------------------*/
#define qr_alloc()	(qr_ptr)malloc(sizeof(qr_node))
#define qr_free(qr)	free(qr)

/*----------------------------------------------------------------------
    Define session block functions.
 -----------------------------------------------------------------------*/
#define sb_alloc()	(sb_ptr)malloc(sizeof(sb_node))
#define sb_free(sb)	free(sb)

#define sb_str_move_os(dst_o,src_s)	str_move(dst_o,src_s)
#define sb_str_move_ls(dst_l,src_s)	str_move(dst_l,src_s)
#define sb_str_move_sl(dst_s,src_l)	str_move(dst_s,src_l)
#define sb_str_equal_sl(dst_s,src_l)    str_equal(dst_s,src_l)

/*----------------------------------------------------------------------
    Define output functions.
 -----------------------------------------------------------------------*/
extern of_ptr lat_getbuffer(void);
extern void lat_retbuffer(of_ptr);

#define of_alloc()	lat_getbuffer()
#define of_free(of)	lat_retbuffer(of)

/*----------------------------------------------------------------------
    Define string handling functions.
 -----------------------------------------------------------------------*/
fncdef(str_convert	, strcvt, int   , (byte *,byte *,int,int))
fncdef(str_return 	, strret, int   , (byte *,byte *,int))
fncdef(str_input  	, strinp, int   , (byte *,if_byte,int,int))

fncdef(str_move		, strmov, byte *, (byte *,byte *))
fncdef(str_diff		, strdif, int	, (byte *,byte *))
fncdef(str_equal	, strequ, int   , (byte *,byte *))
fncdef(str_check        , strchk, int   , (if_byte, int))
