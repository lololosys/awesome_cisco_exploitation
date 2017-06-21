/* $Id: dlsw_debug.h,v 3.2.60.4 1996/07/23 06:29:48 athippes Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/dlsw_debug.h,v $
 *------------------------------------------------------------------
 * Debugging declarations for DLSw
 *
 * July 1994, Frank Bordonaro 
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: dlsw_debug.h,v $
 * Revision 3.2.60.4  1996/07/23  06:29:48  athippes
 * CSCdi63699:  Adding a hidden knob - dlsw llc2 nornr
 * Branch: California_branch
 *
 * Revision 3.2.60.3  1996/07/19  03:55:24  athippes
 * CSCdi60758:  dlsw : duplicate circuit identifier
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/05/30  14:43:35  sbales
 * CSCdi59064:  DLSw+: show commands need to be more usable
 *              modify options for "sh dls reach", "sh dls cir", "sh dls
 *              loc", modify formatting for "show dls peer"
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/05/17  10:45:37  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.2  1996/04/20  02:08:05  fbordona
 * CSCdi55177:  DLSw phase I to convert peer to managed timers
 *              Required for scalability of large DLSw peer nets.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2.48.1  1996/04/03  14:01:28  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2  1995/11/17  09:03:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:23:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:55:28  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:26:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef _DLSW_DEBUG_H_
#define _DLSW_DEBUG_H_

/*
 * Show Function Prototypes
 */
extern void show_dlsw (parseinfo *);
extern void show_dlsw_debug (void);
extern void show_peer_debug (void);
extern void show_core_debug (void);
extern void show_csm_debug (void);
extern void show_local_debug (void);

extern void parse_debug_core (parseinfo *csb);
extern void parse_show_core (parseinfo *csb);
extern void parse_clear_circuits (parseinfo *csb);
extern void parse_clear_cache (parseinfo *csb);

extern void dlsw_core_disable(parseinfo *csb);

/*
 * Debug Function Prototypes
 */
extern void dlsw_debug_init(void);
extern void debug_dlsw (parseinfo *);
extern void dlsw_peer_disable (boolean);
extern void dlsw_local_disable (void);
extern void csm_debug (int, int, boolean, boolean);
extern void csm_debug_all (boolean, boolean);
extern void local_debug (boolean, boolean, int);

/* externs for DLSw debug flags */
extern boolean dlsw_csm_debug;
extern boolean dlsw_peer_debug;
extern boolean dlsw_local_debug;

/* dlsw disable flag to put dlsw to sleep */
extern boolean dlsw_disable;
extern boolean dlsw_nornr;

/*
 * DLSw show command constants
 */
#define DLSW_SHOW_CORE           1
#define DLSW_SHOW_CSM            2
#define DLSW_SHOW_PEER           3
#define DLSW_SHOW_CAP            4
#define DLSW_SHOW_FAST           5
#define DLSW_SHOW_LOC_CKT        6

#define DLSW_SHOW_BY_IP          1
#define DLSW_SHOW_BY_INT         2
#define DLSW_SHOW_LOCAL          3
#define DLSW_SHOW_ALL            4

#define DLSW_SHOW_CSM_ALL        0
#define DLSW_SHOW_CSM_MAC        1
#define DLSW_SHOW_CSM_NETB       2
#define DLSW_SHOW_CSM_LOCAL      1
#define DLSW_SHOW_CSM_REMOTE     2

#define DLSW_SHOW_CIRCUIT_ALL    0
#define DLSW_SHOW_CIRCUIT_KEY    1
#define DLSW_SHOW_CIRCUIT_MAC    2
#define DLSW_SHOW_CIRCUIT_SAP    3

/*
 * DLSw clear command constants
 */
#define CLEAR_DLSW_CIRCUITS     1
#define CLEAR_DLSW_CACHE	2
#define CLEAR_DLSW_FAST		3
#define CLEAR_DLSW_LOC_CKT      4

/*
 * DLSw debug command constants
 */
#define DEBUG_DLSW               0
#define DLSW_DEBUG_ALL           4
#define DLSW_DEBUG_IP_PEER       1
#define DLSW_DEBUG_INT_PEER      2
#define DLSW_DEBUG_ALL_PEERS     3

/*
 * DLSw CSM debug command constants
 */
#define DLSW_DEBUG_ALL_REACH     0
#define DLSW_DEBUG_SNA_REACH     1
#define DLSW_DEBUG_NETB_REACH    2

#define DLSW_DEBUG_CSM_VERBOSE   2
#define DLSW_DEBUG_CSM_ERRORS    1
#define DLSW_DEBUG_CSM_EVENTS    0

/*
 * The actual debugging flags are defined in dlsw_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../dlsw/dlsw_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../dlsw/dlsw_debug_flags.h"

#endif








