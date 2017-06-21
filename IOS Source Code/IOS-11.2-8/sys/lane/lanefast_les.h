/* $Id: lanefast_les.h,v 3.2.58.1 1996/05/09 14:32:44 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/lane/lanefast_les.h,v $
 *------------------------------------------------------------------
 * Definitions and inlines for low-end LANE fastswitching
 *
 * July, 1995     Steve Jay
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lanefast_les.h,v $
 * Revision 3.2.58.1  1996/05/09  14:32:44  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.86.1  1996/04/27  07:06:37  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.76.3  1996/04/12  07:45:40  kjadams
 * Branch: LE_Cal_V112_0_2_branch
 * For TR LANE, add switch to check lan type before calling lec_valid
 *
 * Revision 3.2.76.2  1996/04/08  14:51:55  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.2.76.1  1996/04/08  01:57:02  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.64.1  1996/03/22  22:54:47  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.48.1  1996/03/05  06:43:06  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.36.1  1996/02/27  21:10:26  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.14.2  1996/01/23  23:54:09  cakyol
 * HSRP over LANE Code review changes:
 * - "interface_up" now works properly with HSRP after
 *   Badri's network.c fix.
 * - additional HSRP mac addresses implemented much
 *   faster on the LANE client.
 * - starting an already running or stopping an already
 *   stopped LANE client now behaves properly.
 * - white space cleanup
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.14.1  1996/01/11  01:46:27  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.10.1  1995/12/29  01:06:46  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.2  1995/11/17  17:50:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:23:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:12:27  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/26  22:06:02  shj
 * Placeholders for LANE fastswitching on low-end.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../h/mgd_timers.h"
#include "../atm/atmsig_api.h"
#include "../atm/ilmi_public.h"
#include "../lane/lane_private.h"
#include "../lane/lec_private.h"


static inline boolean
lane_is_control_pak_inline (paktype *pak)
{
    lane_data_hdr_t     *hdr;

    hdr = (lane_data_hdr_t *)(pak->datagramstart + ATM_ENCAP_HDRBYTES);
    return(IS_LANE_CTL(GETSHORT(&hdr->lec_id)));
}

static inline boolean
lane_is_unicast_mac_inline (paktype *pak)
{
    lane_data_hdr_t     *hdr;

    hdr = (lane_data_hdr_t *)(pak->datagramstart + ATM_ENCAP_HDRBYTES);
    return(IS_ETHER_UNICAST(hdr->lan_type.ether.daddr));
}

static inline boolean
lane_from_my_lecid_inline (paktype *pak, vc_info_t *vc)
{
    lane_info_t         *lane_info;
    lec_info_t          *client;
    lane_data_hdr_t     *hdr;

    lane_info = vc->swidb->lane_info;
    if (lane_info) {
        client = lane_info->lec_info;
        if (client) {
            hdr = (lane_data_hdr_t *)(pak->datagramstart + ATM_ENCAP_HDRBYTES);
            if (GETSHORT(&hdr->lec_id) == client->lecid) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

static inline boolean
lane_is_my_mac_inline (paktype *pak, vc_info_t *vc)
{
    lane_info_t         *lane_info;
    lec_info_t          *client;
    lane_data_hdr_t     *hdr;

    lane_info = vc->swidb->lane_info;
    if (lane_info) {
        client = lane_info->lec_info;
        if (client) {
            hdr = (lane_data_hdr_t*)(pak->datagramstart+ATM_ENCAP_HDRBYTES);
            switch(lane_info->type) {
            case(LANE_TYPE_802_3):
              if (lec_mac_valid(client, hdr->lan_type.ether.daddr))
                return TRUE;
              break;
            case(LANE_TYPE_802_5):
              if (lec_mac_valid(client, hdr->lan_type.tring.daddr))
                return TRUE;
              break;
            }
        }
    }
    return FALSE;
}
                
