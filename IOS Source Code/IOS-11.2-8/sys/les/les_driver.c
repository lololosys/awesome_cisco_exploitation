/* $Id: les_driver.c,v 3.3.62.2 1996/08/13 02:20:48 gstovall Exp $
 * $Source: /release/112/cvs/Xsys/les/les_driver.c,v $
 *------------------------------------------------------------------
 * les_driver.c -- Common driver stuff for les drivers
 * 
 * November 1994, Ashwin Baindur
 *
 * Copyright (c) 1988-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: les_driver.c,v $
 * Revision 3.3.62.2  1996/08/13  02:20:48  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.3.62.1  1996/03/18  20:42:57  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/07  10:05:51  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  14:42:12  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  17:41:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:00:21  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:33:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:15:06  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/06/28  22:07:52  vnguyen
 * CSCdi36137:  Low-end hardware init returns wrong code on malloc failure
 *
 * Revision 2.1  1995/06/07  21:38:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "subsys.h"
#include "packet.h"
#include "config.h"

#include "../os/pool.h"
#include "../os/buffers.h"
#include "../les/if_les.h"
#include "../les/les_driver.h"
#include "../les/les_driver_inline.h"

paktype *les_driver_getbuf (hwidbtype *idb)
{
    return(les_driver_getbuf_inline(idb, FALSE));
}

/*
 * =================================================================
 * Name: les_driver_txqlength
 *
 * Description:
 *      Returns the number of packets in the transmit queue.
 *
 * Parameters:
 *      hwidbtype * - pointer to idb of this interface.
 *
 * Returns:
 *      int - length of transmit queue.
 * =================================================================
 */

int les_driver_txqlength (hwidbtype *idb)
{
    les_driver_instance_t *ds = (les_driver_instance_t *)INSTANCE;

    return (ds->tx_count);
}

/*
 * Return the index into the array that the pointer accesses:
 */
static int
get_bd_index (void *ptr, void *array, int element_size)
{
    if (ptr == array)
        return(0);
    return((((char *)ptr) - ((char *)array)) / element_size);
}

/*
 * =================================================================
 * Name: show_bdrings
 *
 * Description:
 *
 * Parameters:
 *      driver_instnce_t * - Common driver instance.
 *      ushort - buffer descriptor size.
 *
 * Returns:
 *      Boolean - TRUE if everything worked.
 *      Boolean - FLASE if malloc failed.
 * =================================================================
 */

boolean show_bdrings(hwidbtype *idb, ushort bd_size)
{

    les_driver_instance_t *ds = (les_driver_instance_t *)INSTANCE;

    if (ds != NULL) {
        leveltype SR;
        char *rx_head;
        paktype **rxp, **txp_head, **txp_tail;
        char *tx_head, *tx_tail;
        char *rx_ring;
        char *tx_ring;
        paktype *rx_p[MAX_RING];
        paktype *tx_p[MAX_RING];
        ushort tx_size, rx_size;
        ushort tx_count;
        int index, incr;


        /*
         * Allocate memory for the RX and TX rings.
         */
        rx_size = ds->rx_size * bd_size;
        rx_ring = malloc(rx_size);
        if (rx_ring == NULL)
            return(FALSE);

        tx_size = ds->tx_size * bd_size;
        tx_ring = malloc(tx_size);
        if (tx_ring == NULL) {
            free(rx_ring);
            return(FALSE);
        }

        /*
         * Disable network interrupts and take a snapshot of the
         * RX and TX ring states:
         */
        SR = raise_interrupt_level(ETHER_DISABLE);

        rx_head = ds->rx_head;
        rxp = (paktype **)ds->head_rxp;
        bcopy(ds->rx_ra, rx_ring, rx_size);
        for (index = 0; index < ds->rx_size; index++)
            rx_p[index] = ds->rx_p[index];

        tx_count = ds->tx_count;
        tx_head = ds->tx_head;
        tx_tail = ds->tx_tail;
        txp_head = ds->head_txp;
        txp_tail = ds->tail_txp;
        bcopy(ds->tx_ra, tx_ring, tx_size);
        for (index = 0; index < ds->tx_size; index++)
            tx_p[index] = ds->tx_p[index];

        reset_interrupt_level(SR);

        /*
         * Display the Rx ring
         */
        printf("RX ring with %d entries at 0x%x, Buffer size %d\n"
               "Rxhead = 0x%x (%d), Rxp = 0x%x (%d)\n",
               ds->rx_size, ds->rx_ra, idb->max_buffer_size,
	       rx_head, get_bd_index(rx_head, ds->rx_ra, bd_size),
               rxp, get_bd_index(rxp, &ds->rx_p[0], sizeof(paktype *)));

        for (incr = 0, index = 0; 
             index < ds->rx_size; 
             index++, incr += bd_size)
            (*ds->show_rxbd)((paktype *)rx_p[index], rx_ring + incr, index);
        free(rx_ring);

        /*
         * Display the Tx ring
         */
        printf("\nTX ring with %d entries at 0x%x, tx_count = %d\n",
               ds->tx_size, ds->tx_ra, tx_count);
        printf("tx_head = 0x%x (%d), head_txp = 0x%x (%d)\n",
               tx_head,
               get_bd_index(tx_head, ds->tx_ra, bd_size),
               txp_head,
               get_bd_index(txp_head, &ds->tx_p[0], sizeof(paktype *)));
        printf("tx_tail = 0x%x (%d), tail_txp = 0x%x (%d)\n",
               tx_tail,
               get_bd_index(tx_tail, ds->tx_ra, bd_size),
               txp_tail,
               get_bd_index(txp_tail, &ds->tx_p[0], sizeof(paktype *)));

        for (incr = 0, index = 0; 
             index < ds->tx_size; 
             index++, incr += bd_size)
            (*ds->show_txbd)((paktype *)tx_p[index], tx_ring + incr, index);
        free(tx_ring);

    }
    return(TRUE);
}

