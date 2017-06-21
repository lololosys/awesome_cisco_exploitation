/* $Id: if_vip_4t.h,v 3.4 1995/12/26 23:08:25 dlobete Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/hes/if_vip_4t.h,v $
 *------------------------------------------------------------------
 * if_vip_4t.h -- header file for 4T Serial port adapter
 *
 * October 1995 - Donald E. Lobete
 * 
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_vip_4t.h,v $
 * Revision 3.4  1995/12/26  23:08:25  dlobete
 * CSCdi42282:  chassis mib incomplete for VIP
 * Add support for PA MIB information to be relayed from VIP to RP/RSP
 * via love-letter.
 * Rework some of the 4T PA's data structures as a result of above.
 *
 * Revision 3.3  1995/12/02  05:50:16  rharagan
 * CSCdi41966:  Messages between 4T PA & driver should be per interface
 * Move to single interface commands, incorporate code review comments.
 *
 * Revision 3.2  1995/11/17  09:19:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:41:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:01:08  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/17  07:17:09  dlobete
 * Placeholder for VIP_branch development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


typedef struct vip4t_data {
    ulong            signal_status;
    ulong	     applique_id;
} vip4t_data_t;

typedef struct vip4t_privdata {
    ulong            s4t_pa_sw_version;
    ulong            s4t_pa_hw_version;
    ulong            s4t_pa_is_dead_jim;
    ulong            s4t_pa_is_dead_jim_total;
} vip4t_privdata_t;

extern void cbus_vip_4t_show_controller(hwidbtype *hwidb, int ctrlr_type);
extern void parse_vip_4t_status_ll(hwidbtype *hwidb, uchar *love_ptr);


