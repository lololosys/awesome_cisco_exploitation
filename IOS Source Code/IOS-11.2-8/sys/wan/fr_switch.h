/* $Id: fr_switch.h,v 3.2.62.4 1996/07/29 22:11:41 snyder Exp $
 * $Source: /release/112/cvs/Xsys/wan/fr_switch.h,v $
 *------------------------------------------------------------------
 * fr_switch.h : include file for fr switching.
 *
 * Summer 92, Sanjeev Newarikar
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: fr_switch.h,v $
 * Revision 3.2.62.4  1996/07/29  22:11:41  snyder
 * CSCdi64472:  declare some arrays const in shr_frame, shr_frame_svc
 *              2380 out of data, 40 image
 * Branch: California_branch
 *
 * Revision 3.2.62.3  1996/07/10  07:37:43  chifei
 * CSCdi60658:  Clear counters clears frame relay MIB objects
 * Branch: California_branch
 *
 * Revision 3.2.62.2  1996/05/31  18:19:33  vrawat
 * CSCdi58846:  frame-relay debugging enhancements
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/04/25  23:23:58  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.2.66.1  1996/04/24  04:11:38  shankar
 * Branch: ATM_Cal_branch
 * Patch FR_SVC_branch changes
 *
 * Revision 3.2  1995/11/17  18:02:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:49:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/02  21:26:31  ccpoe
 * CSCdi37746:  FR PVCs may become out of sync with subif state.
 * Subinterface comes back after it is deleted. fr_if_statechange_complete
 * is implemented to make subif state in sync with PVC state.
 *
 * Revision 2.1  1995/06/07  23:17:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

# define PVC_ACTIVE_BIT    0x02
# define PVC_NEW_BIT       0x08
# define PVC_STATUS_MASK   0x1f
# define SET_PVC_ACTIVE(x) (x = ((x) & ~PVC_STATUS_MASK) | PVC_ACTIVE_BIT)
# define RESET_PVC_ACTIVE(x) (x = (x & ~PVC_ACTIVE_BIT))
# define IS_PVC_NEW_BIT_SET(x) (x & PVC_NEW_BIT)
# define IS_PVC_ACTIVE_BIT_SET(x) (x & PVC_ACTIVE_BIT)
# define FR_NON_DLCI_BITS 0x030f
# define FR_SWITCH_PVC_LEN 5

/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/
extern fr_pvc_info_type *create_fr_pvc (void);
extern void parse_fr_route (parseinfo *, idbtype *);
extern fr_route_type *dsearch_fr_route (ushort, hwidbtype *);
extern void delete_fr_route (fr_route_type *);
extern void add_fr_route (ushort, hwidbtype *, ushort, hwidbtype *);
extern void show_fr_route (void);
extern void nv_fr_route (idbtype *, parseinfo *);
extern void show_pvc_stats (parseinfo *);
extern void show_pvc_idb_stats (idbtype *, ushort);
extern void display_pvc_stats (fr_pvc_info_type *, ushort, boolean);
extern boolean fr_switch_process (hwidbtype *, paktype *);
extern char *fr_pvc_usage_string (enum PVC_USAGE_TYPE);
extern  char *const disp_fr_intf[];
