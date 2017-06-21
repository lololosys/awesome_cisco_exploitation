/* $Id: rvip_rsp.c,v 3.2.62.1 1996/03/18 22:03:21 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/src-rvip/rvip_rsp.c,v $
 *------------------------------------------------------------------
 * rvip_rsp.c - RSP MEMD support for Checkers
 *
 * July 1995, David Getchell
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rvip_rsp.c,v $
 * Revision 3.2.62.1  1996/03/18  22:03:21  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  10:45:39  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  18:02:58  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:50:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:29:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files
 */
#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "../pas/if_pas.h"
#include "../hes/ccb_cmd.h"
#include "rvip_dma.h"
#include "../src-vip/vip_memd.h"
#include "../src-vip/vip_dma.h"
#include "rvip_memd.h"


/*
 * rsp_network_chores
 * platform and rsp dependent network periodic tasks
 */
boolean rsp_network_chores (void)
{
    boolean busy;

    busy = dmaq_consumer();
    busy |= rvip_tx_poll();

    return busy;
}

/* end of file */

