/* $Id: nasi_debug.h,v 3.2.62.1 1996/07/25 09:58:46 snyder Exp $
 * $Source: /release/112/cvs/Xsys/xns/nasi_debug.h,v $
 *------------------------------------------------------------------
 * NASI_debug.h Netware Asynchronous Services Interface
 *              (Debug flag support header include file)
 *
 * Sept 1995, Syed Mohammad Irfan Ashraf
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nasi_debug.h,v $
 * Revision 3.2.62.1  1996/07/25  09:58:46  snyder
 * CSCdi63981:  eliminate unused code, fix bugs, make a common routine
 *              1. create a routine call generic_debug_all and
 *                 make most debugs use it
 *              2. eliminate if (*_debug_inited) return, most never set
 *              the var
 *                 to TRUE making the code useless.
 *              3. declare some constant arrays const
 *              4. fix bugs found along the way
 *              5. save 2768 bytes from image, 660 out of data section
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  19:22:24  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:58:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:38:49  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/26  12:13:05  irfan
 * Netware Asynchronous Services Interface: holding area
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern void nasi_debug_all(boolean);
extern void nasi_debug_show(void);
extern void nasi_debug_init(void);


extern boolean nasi_err_debug;
extern boolean nasi_pkt_debug;
extern boolean nasi_activity_debug;

/*
 * The actual debugging flags are defined in nasi_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../xns/nasi_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../xns/nasi_debug_flags.h"
