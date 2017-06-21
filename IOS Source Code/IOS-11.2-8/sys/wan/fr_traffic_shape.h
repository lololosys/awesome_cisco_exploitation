/* $Id: fr_traffic_shape.h,v 3.1.2.2 1996/08/15 19:32:51 fowler Exp $
 * $Source: /release/112/cvs/Xsys/wan/fr_traffic_shape.h,v $
 *------------------------------------------------------------------
 * fr_traffic_shape.h
 *
 * April 1996, Greg Fowler
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fr_traffic_shape.h,v $
 * Revision 3.1.2.2  1996/08/15  19:32:51  fowler
 * CSCdi60278:  Removing frame-relay traffic shaping from an int causes
 *              traceback
 *              nvgen fixes and timer fix for de-encapsulation
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/05/15  05:54:46  fowler
 * Frame relay VC queuing and rate enforcement
 * Branch: California_branch
 *
 * Revision 3.1  1996/05/02  20:55:44  fowler
 * Placeholders.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern void fr_traffic_setup(hwidbtype *, parseinfo *);
extern void fr_idb_config_shape(hwidbtype *, enum FR_SHAPE_CONF);

#define FR_DEFAULT_CIR 56000
#define FR_DEFAULT_BC  7000
enum FR_PARAMS_TYPE {
    VC_CIR_OUT,
    VC_BC_OUT,
    VC_BECN,
    VC_BE_OUT,
    VC_PEAK,
    VC_AVE,
    VC_CIR_LOWER};


extern void fr_shape_setup(idbtype *, fr_pvc_info_type *, enum FR_SHAPE_CONF);
static inline boolean fr_tr_on_idb (hwidbtype *hwidb)
{
    fr_idb_struct_type *fr_idb;

    fr_idb = hwidb->frame_relay_stuff;
    if (fr_idb &&
	fr_idb->fr_traffic_shape)
	return(TRUE);
    else
	return(FALSE);
}

