/* $Id: dlsw_local_externs.h,v 3.3.20.2 1996/07/19 03:55:30 athippes Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/dlsw_local_externs.h,v $
 *------------------------------------------------------------------
 * DLSw Local Switching External Interface Header File
 *
 * August 1995, Frank Bordonaro
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dlsw_local_externs.h,v $
 * Revision 3.3.20.2  1996/07/19  03:55:30  athippes
 * CSCdi60758:  dlsw : duplicate circuit identifier
 * Branch: California_branch
 *
 * Revision 3.3.20.1  1996/05/30  14:43:38  sbales
 * CSCdi59064:  DLSw+: show commands need to be more usable
 *              modify options for "sh dls reach", "sh dls cir", "sh dls
 *              loc", modify formatting for "show dls peer"
 * Branch: California_branch
 *
 * Revision 3.3  1996/02/05  21:56:16  fbordona
 * CSCdi47942:  dlsw sdlc abm bit not turned off in 1st xid sent to
 *              sdlc station. Use abm default for media until xid3
 *              is received from a device.
 *
 * Revision 3.2  1995/11/17  09:03:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:24:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:55:36  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/24  18:49:12  fbordona
 * Placeholder for new file
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _DLSW_LOCAL_EXTERNS_H_
#define _DLSW_LOCAL_EXTERNS_H_

#include "../ibm/sna_util.h"

/*
 * Constants
 */
#define DL_HASH_SIZE       64
#define DL_HASH_MASK       (DL_HASH_SIZE - 1)

typedef enum {
    DLSW_ABM_UNKNOWN,
    DLSW_ABM_CAPABLE,
    DLSW_ABM_INCAPABLE
} dlsw_abm_type;

/*
 * Function Prototypes
 */
extern void dlsw_local_init (void);
extern void dlsw_show_local_ckt (parseinfo *);
extern void dlsw_clear_local_ckt (int);
extern void dlsw_local_background (void);
extern boolean dlsw_is_local_ckt (TCLSIMsg *);

/*
 * Inline Functions
 */

static inline void dlsw_learn_xid_parms (XID3type *xid,
                                         dlsw_abm_type *abm_state, 
                                         ushort *max_i_field_len, 
                                         uchar *window_size)
{
    if ((xid->xid3_fmt & XID_FMT_MASK) == XID_XID3) {
        if (dlsw_sna_spoof_and_seg())
            xid3_glean_maxbtu(xid, (uchar *) max_i_field_len);

        *window_size = xid->xid3_maxiframe;

        if (xid->xid3_lsflags & XID3_ABM)
            *abm_state = DLSW_ABM_CAPABLE;
        else
            *abm_state = DLSW_ABM_INCAPABLE;
    }
}

static inline void dlsw_spoof_xid_parms (XID3type *xid,
                                         dlsw_abm_type abm_state,
                                         dlsw_abm_type default_abm,
                                         ushort max_i_field_len,
                                         uchar window_size)
{ 
    if ((xid->xid3_fmt & XID_FMT_MASK) == XID_XID3) {
        /*
         * If configured for max i-field spoofing and have learned
         * the lan station's max receivable i-field, then echo it
         * back to them, otherwise, pass the value end to end.
         * Spoofing max i-field is optional, circuits will come up
         * and pass data without it. Spoofing requires biu
         * segmentation to all lan media.
         */
        if ((dlsw_sna_spoof_and_seg()) && (max_i_field_len)) {
            PUTSHORT(&xid->xid3_maxbtu[0], max_i_field_len);
        }

        /*
         * If we have learned the lan stations max receivable
         * i-frame window, then echo it back to them, otherwise,
         * pass the values end to end. Note that this is necessary
         * since xid3_maxiframe is NOT negotiated by some end stations
         * and will keep the session from coming up..
         */
        if (window_size)
            xid->xid3_maxiframe = window_size;

        /*
         * ABM bit must be spoofed on XIDs or the session will not
         * come up.
         *
         * If the target device's ABM capability is unknown, then
         * force the ABM to the normal setting for the media type.
         * Some devices complain when the abm bit doesn't match the 
         * default for the media when it's received in a 
         * pre-negotiation XID3.
         */
        if (abm_state == DLSW_ABM_CAPABLE)
            xid->xid3_lsflags |= XID3_ABM;
        else if (abm_state == DLSW_ABM_INCAPABLE)
            xid->xid3_lsflags &= ~XID3_ABM;
        else if (default_abm == DLSW_ABM_CAPABLE)
            xid->xid3_lsflags |= XID3_ABM;
        else
            xid->xid3_lsflags &= ~XID3_ABM;
    }
}


#endif 


