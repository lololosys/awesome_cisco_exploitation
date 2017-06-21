/* $Id: if_les.c,v 3.2.62.1 1996/09/05 01:16:40 snyder Exp $
 * $Source: /release/112/cvs/Xsys/les/if_les.c,v $
 *------------------------------------------------------------------
 * if_les.c - Interface support for low-end platforms
 *
 * March 1995, Scott Mackie (moved from les/init_les.c)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_les.c,v $
 * Revision 3.2.62.1  1996/09/05  01:16:40  snyder
 * CSCdi68132:  declare consts in lapb x25 atm_arp tcl
 *              save 248 out of data, 12 from image
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:39:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:32:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:35:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "../dev/flash_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "exception.h"
#include "ttysrv.h"
#include "config.h"
#include "logger.h"
#include "../parser/parser_defs_exec.h"
#include "../dev/monitor1.h"
#include "../ui/exec.h"
#include "../os/free.h"
#include "../os/region.h"
#include "../os/buffers.h"
#include "../les/if_les.h"


/*
 * Local storage
 */

const char not_initialized[] = "Not yet initialized\n";


/*
 * make_ring_size
 * Return a legal ring size given a requested ring size
 */

static const int ring_sizes[8] = {1, 2, 4, 8, 16, 32, 64, 128};

int make_ring_size (int n)
{
    int i, size;

    if (n <= 0)
	return(0);
    if (n >= 128)
	return(128);

    size = 2;
    for (i = 0; i < 7; i++)
	if ((n >= ring_sizes[i]) && (n < ring_sizes[i+1])) {
	    size = ring_sizes[i];
	    break;
	}
    return(size);
}

/*
 * log2n - Return the base 2 log of ring size n
 */

int log2n (int n)
{
    int i, log;

    if ((n < 0) || (n > 128))
	return(0);

    log = 0;
    for (i = 0; i < 7; i++)
	if (n == ring_sizes[i]) {
	    log = i;
	    break;
	}
    return(log);
}

/*
 * Select proper RX and TX ring sizes for DMA devices
 * Note that the ring sizes must be powers of two between 1 and 128.
 * Also, the minimum RX ring size for the MK5025 is 2.
 */

void select_ring_size (int *rx_size, int *tx_size, int maxdgram)
{
    /*
     * Select size of RX and TX rings:
     */
    if (system_loading || bootstrap_enable) {
	*rx_size = RXBOOT_MAX_RXRING;
	*tx_size = RXBOOT_MAX_TXRING;
    } else {
	if (maxdgram <= BIGDATA) {
	    *rx_size = MAX_RXRING_BIG;		/* maxdgram is BIGDATA */
	    *tx_size = MAX_TXRING_BIG;
  	} else if (maxdgram <= LARGEDATA) {
 	    *rx_size = MAX_RXRING_LARGE;	/* maxdgram is LARGEDATA */
 	    *tx_size = MAX_TXRING_LARGE;
  	} else {
 	    *rx_size = MAX_RXRING_HUGE;		/* maxdgram is HUGEDATA */
	    *tx_size = MAX_TXRING_HUGE;
  	}
    }
}

/*
 * Adjust RX ring size to MAX_RXRING_BIG:
 * This function is for the serial interfaces with SMDS encapsulation only.
 *     maxdgram of SMDS is 1582, which is larger than BIGDATA(1524),
 *     this is why select_ring_size() assigns less than enough ring size,
 *     and then the pool size is specified not enough later.
 * This function is to be invoked after select_ring_size() is invoked.
 * This function is to be invoked from serial drivers such as hd64570 and
 * mk5025; but not from cd2430, as cd2430 pool size is a constant and there
 * is no ring involved.
 */
void adj_smds_ring_size(int *rx_size)
{
    *rx_size = MAX_RXRING_BIG;
}


void show_les_controllers (parseinfo *csb)
{
    automore_enable(NULL);	/* Enable more processing */

    switch (GETOBJ(int,1)) {
      case SHOW_CONTROLLERS_ALL:
	SETOBJ(int,1) = 0;
	show_controllers(csb);
	break;
    }
    automore_disable();
}

/*
 * Return true if bridging is enabled on interface
 */

boolean bridging_on_this_interface (idbtype *idb)
{
    if (RUNNING_IF(LINK_BRIDGE,idb) && bridge_enable &&
        (!(system_loading | bootstrap_enable)))
        return(TRUE);

    return(FALSE);
}
