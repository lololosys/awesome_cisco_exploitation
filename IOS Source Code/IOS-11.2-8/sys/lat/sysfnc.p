/*-----------------------------------------------------------------------
    sysfnc		lat system function prototypes

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/
#ifndef SYSFNC_H
#define SYSFNC_H	0		/* Indicate file has been read	*/

/*----------------------------------------------------------------------
    Define advertised service functions.
 -----------------------------------------------------------------------*/
#define as_alloc()	(as_ptr)alloc(sizeof(as_node))
#define as_free(as)	free((char *)as)

#define as_str_diff_sl(dst_s,src_l)	str_diff(dst_s,src_l)
#define as_str_move_os(dst_o,src_s)	str_move(dst_o,src_s)
#define as_str_move_sl(dst_s,src_l)	str_move(dst_s,src_l)

/*----------------------------------------------------------------------
    Define circuit block functions.
 -----------------------------------------------------------------------*/
#define cb_alloc()	(cb_ptr)alloc(sizeof(cb_node))
#define cb_free(cb)	free((char *)cb)

/*----------------------------------------------------------------------
    Define host-initiated functions.
 -----------------------------------------------------------------------*/
#define hi_alloc()	(hi_ptr)alloc(sizeof(hi_node))
#define hi_free(hi)	free((char *)hi)

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
#define ls_alloc()	(ls_ptr)alloc(sizeof(ls_node))
#define ls_free(ls)	free((char *)ls)

#define ls_str_diff_sl(dst_s,src_l)	str_diff(dst_s,src_l)
#define ls_str_move_os(dst_o,src_s)	str_move(dst_o,src_s)
#define ls_str_move_sl(dst_s,src_l)	str_move(dst_s,src_l)
#define ls_str_equal_sl(dst_s,src_l)    str_equal(dst_s,src_l)
#define ls_str_equal_ss(dst_s,src_s)    str_equal(dst_s,src_s)

/*----------------------------------------------------------------------
    Define remote node functions.
 -----------------------------------------------------------------------*/
#define nd_alloc()	(nd_ptr)alloc(sizeof(nd_node))
#define nd_free(nd)	free((char *)nd)

#define nd_str_diff_sl(dst_s,src_l)	str_diff(dst_s,src_l)
#define nd_str_diff_ss(dst_s,src_s)	str_diff(dst_s,src_s)
#define nd_str_move_os(dst_o,src_s)	str_move(dst_o,src_s)
#define nd_str_move_sl(dst_s,src_l)	str_move(dst_s,src_l)
#define nd_str_move_ls(dst_l,src_s)	str_move(dst_l,src_s)
#define nd_str_equal_sl(dst_s,src_l)    str_equal(dst_s,src_l)

/*----------------------------------------------------------------------
    Define object functions.
 -----------------------------------------------------------------------*/
#define ob_alloc()	(ob_ptr)alloc(sizeof(ob_node))
#define ob_free(ob)	free((char *)ob)

#define ob_str_diff_sl(dst_s,src_l)	str_diff(dst_s,src_l)
#define ob_str_move_os(dst_o,src_s)	str_move(dst_o,src_s)
#define ob_str_move_sl(dst_s,src_l)	str_move(dst_s,src_l)
#define ob_str_equal_sl(dst_s,src_l)    str_equal(dst_s,src_l)

/*----------------------------------------------------------------------
    Define queued requests functions.
 -----------------------------------------------------------------------*/
#define qr_alloc()	(qr_ptr)alloc(sizeof(qr_node))
#define qr_free(qr)	free((char *)qr)

/*----------------------------------------------------------------------
    Define session block functions.
 -----------------------------------------------------------------------*/
#define sb_alloc()	(sb_ptr)alloc(sizeof(sb_node))
#define sb_free(sb)	free((char *)sb)

#define sb_str_move_os(dst_o,src_s)	str_move(dst_o,src_s)
#define sb_str_move_ls(dst_l,src_s)	str_move(dst_l,src_s)
#define sb_str_move_sl(dst_s,src_l)	str_move(dst_s,src_l)
#define sb_str_equal_sl(dst_s,src_l)    str_equal(dst_s,src_l)

/*----------------------------------------------------------------------
    Define output functions.
 -----------------------------------------------------------------------*/
#define of_alloc()	(of_ptr)alloc(sizeof(of_node))
#define of_free(of)	free((char *)of)

/*----------------------------------------------------------------------
    Define string handling functions.
 -----------------------------------------------------------------------*/
fncdef(str_check	, strchk, int   , (if_byte,int))
fncdef(str_convert	, strcvt, int   , (byte *,byte *,int,int))
fncdef(str_return 	, strret, int   , (byte *,byte *,int))
fncdef(str_input  	, strinp, int   , (byte *,if_byte,int,int))

fncdef(str_move		, strmov, byte *, (byte *,byte *))
fncdef(str_diff		, strdif, int	, (byte *,byte *))
fncdef(str_equal	, strequ, int   , (byte *,byte *))

/*----------------------------------------------------------------------
    Define memory allocation functions.
 -----------------------------------------------------------------------*/
fncdef(alloc	        , alloc , char *, (int))
fncdef(free		, free  , void 	, (char *))
#endif
