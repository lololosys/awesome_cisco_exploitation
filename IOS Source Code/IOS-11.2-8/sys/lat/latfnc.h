/* $Id: latfnc.h,v 3.2 1995/11/17 17:33:54 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/latfnc.h,v $
 *------------------------------------------------------------------
 * latfnc.h
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: latfnc.h,v $
 * Revision 3.2  1995/11/17  17:33:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:26:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:25:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    latfnc		lat function prototypes

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/
#ifndef LATFNC_H
#define LATFNC_H	0		/* Indicate file has been read	*/

/*----------------------------------------------------------------------
    Define dummy data types if no include file definition.
 -----------------------------------------------------------------------*/
#ifndef LATAS_H
    typedef struct as {int dummy;} AS_VIEW(as_ptr);
#endif

#ifndef LATCB_H
    typedef struct cb {int dummy;} CB_VIEW(cb_ptr);
#endif

#ifndef LATHI_H
    typedef struct hi {int dummy;} HI_VIEW(hi_ptr);
#endif

#ifndef LATLS_H
    typedef struct ls {int dummy;} LS_VIEW(ls_ptr);
#endif

#ifndef LATND_H
    typedef struct nd {int dummy;} ND_VIEW(nd_ptr);
#endif

#ifndef LATOB_H
    typedef struct ob {int dummy;} OB_VIEW(ob_ptr);
#endif

#ifndef LATQR_H
    typedef struct qr {int dummy;} QR_VIEW(qr_ptr);
#endif

#ifndef LATSB_H
    typedef struct sb {int dummy;} SB_VIEW(sb_ptr);
#endif

/*----------------------------------------------------------------------
    Define function argument types for configuration conditional
    compiles.
 ----------------------------------------------------------------------*/
#if CME_SLAT
#define lat_port  cid_type
#define ob_id	  cid_type
#else
#define lat_port  ob_ptr
#define ob_id	  oid_type
#endif

/*----------------------------------------------------------------------
    Define general LAT engine functions.
 -----------------------------------------------------------------------*/
fncdef(hs_zero            , hszero, void  ,(void))

fncdef(acl_clr_group      , aclclr, void  , (int))
fncdef(acl_cmp_group      , aclcmp, int   , (if_byte))
fncdef(acl_chk_group	  , ackchk, int   , (nd_ptr,if_byte))
fncdef(acl_set_group      , aclset, void  , (int))
fncdef(idl_chk_group      , idlchk, int   , (lat_port,nd_ptr))
fncdef(idl_clr_group      , idlclr, void  , (lat_port,int))
fncdef(idl_cmp_group      , idlcmp, int   , (lat_port,if_byte))
fncdef(idl_set_group      , idlset, void  , (lat_port,int))

/*----------------------------------------------------------------------
    Define timer based functions.
 -----------------------------------------------------------------------*/
fncdef(tic_elapse         , ticelp, void  , (void))
fncdef(toc_elapse         , tocelp, void  , (int))

/*----------------------------------------------------------------------
    Define ethernet message  functions.
 -----------------------------------------------------------------------*/
fncdef(if_check           , ifchk , int   , (if_byte,word[3],int,idbtype *))
fncdef(if_message	  , ifmsg , void  , (if_byte,word[3],word[3],idbtype *))
fncdef(of_message         , ofmsg , void  , (of_ptr))

fncdef(ix_error           , ixerr , void  , (if_byte,word[3],int,int,idbtype *))

/*----------------------------------------------------------------------
    Define circuit control functions.
 -----------------------------------------------------------------------*/
fncdef(cb_abort           , cbabo , void  , (cb_ptr))
fncdef(cb_create	  , cbcre , cb_ptr, (nd_ptr))
fncdef(cb_delete	  , cbdel , void  , (cb_ptr))
fncdef(cb_halt            , cbhalt, void  , (if_byte,word[3],int,idbtype *))

/*----------------------------------------------------------------------
    Define circuit input message functions.
 -----------------------------------------------------------------------*/
fncdef(cb_s_run_message   , cbsrun, void  , (if_byte,cb_ptr,word[3],idbtype *))
fncdef(cb_h_run_message   , cbhrun, void  , (if_byte,cb_ptr))
fncdef(cb_s_start_message , cbsstr, void  , (if_byte,cb_ptr))
fncdef(cb_h_start_message , cbhstr, void  , (if_byte,word[3],idbtype *))

/*----------------------------------------------------------------------
    Define circuit output message functions.
 -----------------------------------------------------------------------*/
fncdef(cb_run_output      , cbrout, void  , (cb_ptr))
fncdef(cb_o_run_message   , cborun, void  , (of_ptr,cb_ptr))
fncdef(cb_o_start_message , cbostr, void  , (of_ptr,cb_ptr))
fncdef(cb_retransmit      , cbrxmt, void  , (cb_ptr,int))

/*----------------------------------------------------------------------
    Define session functions.
 -----------------------------------------------------------------------*/
fncdef(sb_accept	  , sbacc , void  , (sb_ptr,sid_type,int))
fncdef(sb_create	  , sbcre , sb_ptr, (cb_ptr))
fncdef(sb_delete	  , sbdel , void  , (sb_ptr,int))
fncdef(sb_dups		  , sbdups, int   , (sb_ptr))
fncdef(sb_find		  , sbfind, sb_ptr, (sid_type))
fncdef(sb_open		  , sbopen, sb_ptr, (byte *,sid_type,ob_id,int,byte *,byte *,byte *,int))
fncdef(sb_reject          , sbrej , void  , (sb_ptr,int))
fncdef(sb_ri_action	  , sbria , void  , (hi_ptr))
fncdef(sb_signal	  , sbsig , void  , (sb_ptr,int,int))
fncdef(sb_start           , sbstr , void  , (sb_ptr,byte *,byte *))
fncdef(sb_stop            , sbstop, void  , (sb_ptr,int))

/*----------------------------------------------------------------------
    Define session slot functions.
 -----------------------------------------------------------------------*/
fncdef(sb_1_slot_inp      , sb1inp, void  , (if_byte,sb_ptr,cb_ptr))
fncdef(sb_1_slot_out      , sb1out, int   , (of_byte,sb_ptr,int))

/*----------------------------------------------------------------------
    Define session environment functions.
 -----------------------------------------------------------------------*/
fncdef(sx_get_char	  , sxchar, int   , (sb_ptr,of_byte,int))
fncdef(term_sx_get_char	  , sxchar, void  , (sb_ptr,of_byte,int,int *))
fncdef(sx_put_char	  , sxchar, void  , (sb_ptr,if_byte,int))
fncdef(term_sx_put_char	  , sxchar, void  , (sb_ptr,if_byte,int))
fncdef(sx_session	  , sxses , void  , (sb_ptr,int))
fncdef(term_sx_session	  , sxses , void  , (sb_ptr, int, boolean *))
fncdef(sx_signal	  , sxsig , void  , (sb_ptr,int))

/*----------------------------------------------------------------------
    Define advertised service functions.
 -----------------------------------------------------------------------*/
fncdef(as_clr_object	  , asclro, void  , (as_ptr,oid_type))
fncdef(as_create	  , ascre , as_ptr, (byte *,byte *,aid_type,int,int))
fncdef(as_delete	  , asdel , void  , (as_ptr))
fncdef(as_list		  , aslist, as_ptr, (byte *))
fncdef(as_reset		  , asrset, void  , (void))
fncdef(as_set_flags	  , assetf, int	  , (as_ptr,int))
fncdef(as_set_ident	  , asseti, int	  , (as_ptr,byte *))
fncdef(as_set_object	  , asseto, int	  , (as_ptr,oid_type))
fncdef(as_show		  , asshow, as_ptr, (byte *))

fncdef(as_message         , asmsg , int   , (of_ptr))
fncdef(ax_set_rate        , axrate, byte  , (as_ptr))

/*----------------------------------------------------------------------
    Define host-initiated control functions.
 -----------------------------------------------------------------------*/
fncdef(hi_create	  , hicre , hi_ptr, (int))
fncdef(hi_delete	  , hidel , void  , (hi_ptr,int))
fncdef(hi_entry_cancel	  , hiecan, void  , (hi_ptr))
fncdef(hi_entry_report	  , hierpt, void  , (hi_ptr))
fncdef(hi_find		  , hifind, hi_ptr, (word,word,int,byte *))
fncdef(hi_initiate	  , hiinit, hi_ptr, (byte *, byte *,byte *,byte *,sid_type,ob_id,int,int,idbtype *,int))
fncdef(hi_open		  , hiopen, sb_ptr, (hi_ptr,sid_type,int))
fncdef(hi_queue_report	  , hiqrpt, hi_ptr, (byte *, idbtype *))
fncdef(hi_queue_status	  , hiqsts, hi_ptr, (byte *, idbtype *))
fncdef(hi_reject	  , hirej , void  , (hi_ptr,int))
fncdef(hi_session	  , hises , hi_ptr, (sid_type))
fncdef(hi_start           , histr , void  , (sb_ptr,word))

/*----------------------------------------------------------------------
    Define host-initiated environment functions.
 -----------------------------------------------------------------------*/
fncdef(hx_session	  , hxses , void  , (hi_ptr,int))
fncdef(hx_status	  , hxsts , void  , (hi_ptr))

/*----------------------------------------------------------------------
    Define host-initiated input functions.
 -----------------------------------------------------------------------*/
fncdef(hi_h_ri_message      , hihri , void  , (if_byte,word[3],idbtype *))
fncdef(hi_h_ri_action	    , hihra , void  , (hi_ptr))
fncdef(hi_h_sts_message     , hihsts, void  , (if_byte))
fncdef(hi_s_si_message      , hissi , void  , (if_byte,word[3],word[3],idbtype *))
fncdef(hi_s_cmd_message     , hiscmd, void  , (if_byte,word[3],idbtype *))

/*----------------------------------------------------------------------
    Define host-initiated output function.
 -----------------------------------------------------------------------*/
fncdef(hi_o_cmd_message     , hiocmd, void   , (of_ptr,hi_ptr))
fncdef(hi_o_ra_message      , hiora , void   , (of_ptr,hi_ptr))
fncdef(hi_o_ri_message      , hiori , void   , (of_ptr,hi_ptr))
fncdef(hi_o_si_message      , hiosi , void   , (of_ptr,hi_ptr))
fncdef(hi_o_sts_message     , hiosts, void   , (of_ptr,hi_ptr))
fncdef(hi_o_request         , hioreq, of_byte, (of_byte,hi_ptr))

/*----------------------------------------------------------------------
    Define learned service functions.
 -----------------------------------------------------------------------*/
fncdef(ls_create	  , lscre , ls_ptr, (byte *,nd_ptr))
fncdef(ls_delete	  , lsdel , void  , (ls_ptr))
fncdef(ls_list		  , lslist, ls_ptr, (byte *,byte *,lat_port,int))
fncdef(ls_permanent	  , lsperm, ls_ptr, (byte *,byte *,word[3], idbtype *))
fncdef(ls_remove	  , lslrem, void  , (byte *))
fncdef(ls_reset		  , lsrset, void  , (void))
fncdef(ls_scan		  , lsscan, ls_ptr, (byte *,lat_port,int))
fncdef(ls_show		  , lsshow, ls_ptr, (byte *,byte *,lat_port,int))

fncdef(ls_dups            , lsdups, int   , (byte *,byte))
fncdef(ls_look            , lslook, ls_ptr, (byte *))
fncdef(ls_message         , lsmsg , void  , (if_byte,word[3], idbtype *))
fncdef(ls_timeout         , lstmo , void  , (void))

/*----------------------------------------------------------------------
    Define node functions.
 -----------------------------------------------------------------------*/
fncdef(nd_create          , ndcre , nd_ptr, (byte *,word [3], idbtype *))
fncdef(nd_delete          , nddel , void  , (nd_ptr))
fncdef(nd_fail		  , ndfail, void  , (nd_ptr,int))
fncdef(nd_list		  , ndlist, nd_ptr, (byte *))
fncdef(nd_purge		  , ndpur , int   , (int))
fncdef(nd_show		  , ndshow, nd_ptr, (byte *, idbtype *))
fncdef(nd_zero            , ndzero, void  , (nd_ptr))

/*----------------------------------------------------------------------
    Define object functions.
 -----------------------------------------------------------------------*/
fncdef(ob_clone		  , obclon, ob_ptr, (ob_ptr))
fncdef(ob_create	  , obcdr , ob_ptr, (byte *,oid_type,int,int,int))
fncdef(ob_delete	  , obdel , void  , (ob_ptr))
fncdef(ob_find		  , obfind, ob_ptr, (oid_type))
fncdef(ob_list		  , oblist, ob_ptr, (byte *))
fncdef(ob_rename  	  , obren , int   , (ob_ptr,byte *,oid_type))
fncdef(ob_reset		  , obrset, void  , (void))
fncdef(ob_release	  , obrel , void  , (ob_ptr))
fncdef(ob_set_flags	  , obsetf, void  , (ob_ptr,int))
fncdef(ob_show		  , obshow, ob_ptr, (byte *))

fncdef(ox_session	  , oxses , void  , (sb_ptr,ob_ptr,int))
fncdef(term_ox_session	  , oxses , void  , (sb_ptr, ob_ptr, int, boolean *))

/*----------------------------------------------------------------------
    Define DATA-B functions.
 -----------------------------------------------------------------------*/
fncdef(db_change_config	  , dbchgc, void  , (lat_port,word,word,byte,byte,int,int))
fncdef(db_report_config	  , dbrepc, void  , (sb_ptr))
fncdef(db_report_status	  , dbreps, void  , (sb_ptr,byte,byte))
fncdef(db_report_break	  , dbrepb, void  , (sb_ptr,byte))
fncdef(db_set_config	  , dbsetc, void  , (sb_ptr,word,word,word,byte,byte,int,int))
fncdef(db_set_mode	  , dbsetm, void  , (sb_ptr,byte))

/*----------------------------------------------------------------------
    Define queued request functions.
 -----------------------------------------------------------------------*/
fncdef(qr_create	  , qrcre , qr_ptr, (hi_ptr))
fncdef(qr_delete	  , qrdel , hi_ptr, (qr_ptr))
fncdef(qr_find		  , qrfind, qr_ptr, (word))
fncdef(qr_list		  , qrlist, qr_ptr, (int,int,byte *,byte *, byte *))
fncdef(qr_scan		  , qrscan, void  , (void))
#endif
