/* $Id: if_vip_epad.c,v 3.1.64.8 1996/09/09 23:34:47 ptran Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_vip_epad.c,v $
 *------------------------------------------------------------------
 * if_vip_epad.c - VIP Cylink Encryption Port Adaptor driver routines.
 *
 * August 1995, Paulina D. Tran
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_vip_epad.c,v $
 * Revision 3.1.64.8  1996/09/09  23:34:47  ptran
 * CSCdi68463:  ESA driver cleanup
 * Branch: California_branch
 *
 * Revision 3.1.64.7  1996/08/28  13:06:48  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.64.6  1996/07/18  01:26:52  ptran
 * Added in check to avoid the diagnostic to re-enter the epad_interrupt().
 * CSCdi61737:  Encryption PA diagnostic
 * Branch: California_branch
 *
 * Revision 3.1.64.5  1996/06/29  01:10:52  ptran
 * CSCdi58378:  Encryption PA need performance improvement.
 * Branch: California_branch
 *
 * Revision 3.1.64.4  1996/06/27  07:04:37  ptran
 * CSCdi58378:  Encryption PA need performance improvement.
 * Branch: California_branch
 *
 * Revision 3.1.64.3  1996/06/21  22:19:30  xliu
 * CSCdi61014:  Migrate to support vip2.
 * Branch: California_branch
 *
 * Revision 3.1.64.2  1996/05/21  10:00:06  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.64.1  1996/04/19  15:26:13  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.1.2.9  1996/04/16  21:54:57  ptran
 * EPA driver clean up. Removed statistic counters used for debugging
 * purposes.
 * Removed parser commands used for testing. Modified to compile in
 * California
 * branch.  Incorporate code review comments.
 *
 * Branch: IosSec_branch
 *
 * Revision 3.1.2.5  1996/02/14  23:45:09  ptran
 * Changed the default of 4E to use I/O mapped I/O.  This change only
 * effect
 * IosSec_branch.
 * Added parser commands to test the encryption key management.
 * Fixed lost of pak and particles when descriptor error occur.
 * Added statistic counters and parser command to display these
 * statistic.
 *
 * Branch: IosSec_branch
 *
 * Revision 3.1.2.4  1996/02/02  23:14:49  ptran
 * Commit first release of EPA firmware.
 * Branch: IosSec_branch
 *
 * Revision 3.1.2.3  1996/01/31  04:48:20  xliu
 * Branch: IosSec_branch
 * Updated epa driver.
 *
 * Revision 3.1.2.2  1996/01/08  19:23:58  ptran
 * Branch: IosSec_branch
 *
 * Revision 3.1.2.1  1995/12/06  21:49:02  rmontino
 * Branch: IosSec_branch
 *
 * Revision 3.1  1995/11/09  12:58:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/08/23  02:27:39  ptran
 * Placeholder file for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "../ui/common_strings.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "ttysrv.h"
#include "logger.h"
#include "subsys.h"
#include "sched.h"
#include "parser.h"
 
#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"
 
#include "../if/pci.h"
#include "../hes/if_fci.h"
#include "../if/network.h"
 
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
 
#include "../hes/ccb_cmd.h"

#include "if_pas.h"
#include "msg_pas.c"			/* Not a typo, see logger.h */
#include "pas_id.h"

#include "msg_epad.c"			/* Not a typo, see logger.h */
#include "if_pas_epad.h"
#include "if_vip_epad.h"

/*****************************************************************************
 * epad_idb_create - Create an idb for EPA. 
 * VIP platforms do not support hot swap, so just create it.
 *****************************************************************************/
hwidbtype *epad_idb_create (uint slot, uint port)
{
    hwidbtype *idb;
 
    idb = idb_create();
    return(idb);
}

/*****************************************************************************
 * epad_deallocate:
 * Clean up all resourcse for this interface. VIP does not support
 * hotswap, so there is no need to cleanup.
 *****************************************************************************/
void epad_deallocate (hwidbtype *hwidb)
{
}
/*****************************************************************************
 * epad_init_buffer_pools - Initialize buffer pools for this interface
 *****************************************************************************/
boolean epad_init_buffer_pools (hwidbtype *idb)
{
    pooltype *pool;
    int particles_in_pool;
    mempool *buffer_mempool;
    ushort size;
 
    if (!idb->pool) {
	buffer_mempool = pas_buffer_mempool(idb);
	if (idb->pool_group == POOL_GROUP_INVALID) {
	    idb->pool_group = pool_create_group();
	}
            
	if (idb->slotunit == EPAD_FRAME_DEVICE) {
	    size = EPAD_FRAME_PARTICLE_SIZE;
	    particles_in_pool = EPAD_FRAME_RING_ENTRIES + EPAD_FRAME_HOLDQ_SIZE + 1;
	} else {
	    size = EPAD_KEY_PARTICLE_SIZE;
	    particles_in_pool = EPAD_KEY_RING_ENTRIES + EPAD_KEY_HOLDQ_SIZE + 1;
	}
	pool = particle_pool_create(idb->hw_namestring,
				    idb->pool_group,
				    size,
				    POOL_SANITY,
				    32,
				    buffer_mempool);
	if (!pool) {
	    errmsg(&msgsym(ERROR, EPAD), "Can not create pool\n");
	    return(FALSE);
	}
 
	/*
	 * Add a fallback particle pool
	 */
	pool->fallback = pas_interface_fallback_pool(idb);
 
	/*
	 * Populate the pool and add a cache
	 */
	pool_adjust(pool, 0, particles_in_pool, particles_in_pool, TRUE);
	particle_pool_create_cache(pool, particles_in_pool);
	pool_adjust_cache(pool, particles_in_pool);
	idb->pool = pool;
    }
    
    /*
     * Init the header pool
     * Needed for initializing static paks 
     * and possibly by memd poller 
     */
    if (!idb->headerpool) {
	idb->headerpool = pas_interface_header_pool(idb);
    }
    
    return(TRUE);
}
/*****************************************************************************
 * epad_drop_packet - just free pak for now
 *****************************************************************************/
void epad_drop_packet (hwidbtype *idb, paktype *pak)
{
    epad_instance_t *ds = INSTANCE(idb);

    /*
     * We're going to drop the packet. Return the encryption particle
     */
    if (idb->slotunit == EPAD_FRAME_DEVICE && pak->unspecified[0]) {
	retparticle((particletype *)pak->unspecified[0]);    
    }
    ds->num_drop++;
    if (pak_has_particles(pak)) {
        epad_cleanup_particles(pak);
    } else {
        datagram_done(pak);
    }
}

/*****************************************************************************
 * epad_get_particle_inline:
 * Get an rx particle.
 *****************************************************************************/
inline particletype *epad_get_particle_inline (hwidbtype *idb,
                                                          boolean cache_ok)
{
    particletype *mp = NULL;
    epad_instance_t *ds = INSTANCE(idb);

    /*
     * If cache_ok, try the buffer cache. This check will compile out.
     */
    if (cache_ok) {
        mp = pool_dequeue_cache(idb->pool);
    }

    /*
     * If we did not get one from the cache, try a global pool
     */
    if (!mp) {
        mp = pool_getparticle(idb->pool->fallback);
    }

    /*
     * If we still do not have one, mark it as a nobuffer.
     */
    if (mp) {
        /*
         * Initialize start
         */
        mp->data_start = particle_center(mp);
        return(mp);
    } else {
        idb->counters.output_nobuffers++;
        ds->num_no_buffer++;
        return(NULL);
    }
}

void epad_platform_init (hwidbtype *idb)
{
    /*
     * Advertise idb
     */
    idb_enqueue(idb);
    idb->fci_type = FCI_TYPE_VIP_ESA;
    vip_interface_init(idb);
}

