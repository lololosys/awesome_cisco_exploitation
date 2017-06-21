/* $Id: rvip_rpsp.c,v 3.2.62.1 1996/03/18 22:03:20 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/src-rvip/rvip_rpsp.c,v $
 *------------------------------------------------------------------
 * rvip_rpsp.c - RPSP MEMD support for Checkers
 *
 * February 1995, David Getchell
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rvip_rpsp.c,v $
 * Revision 3.2.62.1  1996/03/18  22:03:20  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  10:45:38  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  18:02:56  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:50:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:29:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:29:17  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:59:16  hampton
 * Bump version numbers from 1.x to 2.x.
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
 * rpsp_network_chores
 * platform and rpsp dependent network periodic tasks
 */
boolean rpsp_network_chores (void)
{
    boolean busy;

    busy = dmaq_consumer();
    busy |= rvip_tx_poll();
    busy |= !local_txq_sync();
    busy |= !local_rxq_sync();

    return busy;
}

/* end of file */
