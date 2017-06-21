/* $Id: hes_fip_inline.h,v 3.2 1995/11/17 09:16:37 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/hes_fip_inline.h,v $
 *------------------------------------------------------------------
 * hes/hes_fip_inline.h: FCI/FCIT specific inline CAM functions
 * [Was called fddi_ags_inline.h]
 *
 * April 1994, Tony Speakman
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: hes_fip_inline.h,v $
 * Revision 3.2  1995/11/17  09:16:37  hampton
 * Remove old entries from the RCS header logs.
 *
 *------------------------------------------------------------------
 * $Endlog$
 *------------------------------------------------------------------
 */

#include "../hes/ccb_cmd.h"


/*
 * FDDI CAM operation to create a CAM entry for a given MAC address.
 * This is usually done when the corresponding bridge table entry is
 * created.
 */

static inline
void hes_fddi_cam_populate_inline (hwidbtype *hwidb, uchar *mac_address)
{
    if (hwidb->status & IDB_VIP) {
        send_fddi_cam_cmd_3arg(hwidb, CCB_CMD_VIP_CAM_POPULATE,
            GETSHORT(mac_address),
            GETSHORT(mac_address+2),
            GETSHORT(mac_address+4));
        return;
    }

    if (hwidb->fddi_options & FDDI_BRIDGE_CAM_SUPPORTED) {
	select_if(hwidb);
	send_fddi_cam_wr_arg_cmd(hwidb, ((ushort *) mac_address)[0]);
	send_fddi_cam_wr_arg_cmd(hwidb, ((ushort *) mac_address)[1]);
	send_fddi_cam_wr_arg_cmd(hwidb, ((ushort *) mac_address)[2]);
	send_fddi_cam_cmd(hwidb, FDDI_CAM_INSERT);
    }
}

/*
 * FDDI CAM operation to remove any CAM entry that MAY exist for a
 * given MAC address.  This is usually done when the corresponding
 * bridge table entry is removed for any reason.
 * There are no side effects of performing this operation when
 * no CAM entry exists.
 */

static inline
void hes_fddi_cam_depopulate_inline (hwidbtype *hwidb, uchar *mac_address,
				     hwidbtype *input_hwidb)
{
    if (hwidb->status & IDB_VIP) {
        send_fddi_cam_cmd_3arg(hwidb, CCB_CMD_VIP_CAM_DEPOPULATE,
                GETSHORT(mac_address),
                GETSHORT(mac_address+2),
                GETSHORT(mac_address+4));
        if (input_hwidb != NULL)
            select_if(input_hwidb);
        return;
    }

    if (hwidb->fddi_options & FDDI_BRIDGE_CAM_SUPPORTED) {
	select_if(hwidb);
	send_fddi_cam_wr_arg_cmd(hwidb, ((ushort *) mac_address)[0]);
	send_fddi_cam_wr_arg_cmd(hwidb, ((ushort *) mac_address)[1]);
	send_fddi_cam_wr_arg_cmd(hwidb, ((ushort *) mac_address)[2]);
	send_fddi_cam_cmd(hwidb, FDDI_CAM_REMOVE);

	/*
	 * Since select_if selects the FDDI interface,
	 * we have to set it back to the input interface.
	 */
	if (input_hwidb != NULL)
	    select_if(input_hwidb);
    }
}
