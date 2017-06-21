/* $Id: buffers_inline.h,v 3.3.20.4 1996/08/28 13:02:55 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/buffers_inline.h,v $
 *------------------------------------------------------------------
 * Inline function for buffer pool support
 *
 * Aug 94, Scott Mackie
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: buffers_inline.h,v $
 * Revision 3.3.20.4  1996/08/28  13:02:55  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.20.3  1996/06/04  22:54:58  mbeesley
 * CSCdi59380:  Optimize buffer code for traditional platforms
 * Branch: California_branch
 *
 * Revision 3.3.20.2  1996/06/04  05:15:56  mshannon
 * CSCdi45507:  sanity checking code required for memory corruptions
 * Branch: California_branch
 * Added initial debug sanity code for memory corruptions
 *
 * Revision 3.3.20.1  1996/04/04  09:37:38  smackie
 * Un-obfuscate buffer copy code for prolonged simplicity. Remove
 * misguided internal particle structure from paktype. Add a discrete
 * pak_coalesce() call for particle re-assembly. (CSCdi51223)
 * Branch: California_branch
 *
 * Revision 3.3  1996/02/02  04:29:39  mbeesley
 * CSCdi48003:  Buffer manager not optimal on all boxes
 * Make pak_set_dirty() and pak_ignore_inputq() platform specific.
 *
 * Revision 3.2  1995/11/17  18:44:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:43:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:18:25  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  21:53:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../os/buffers_debug.h"    /* Required */
#include "logger.h"			/* Required for next include. */
#include "../os/msg_system.c"		/* Not a typo, see logger.h */


/*
 * Externs:
 */

extern boolean sanity_debug;

#ifdef BUFRETDEBUG
#define BUFRETCMD(x) x
#else
#define BUFRETCMD(x)
#endif


/*
 * Inlines:
 */

/*
 * buffer_sanity_savecaller_inline
 *  Set the address of the calling routine.
 */

static inline void buffer_sanity_savecaller_inline(caller_t *caller_pc) 
{
    if (sanity_debug)
   	*caller_pc = cpu_caller();
    return;
}

/*
 * buffer_sanity_setcaller_inline
 *  Set the address of the calling routine in the paktype caller_pc
 * field.
 */

static inline void buffer_sanity_setcaller_inline(paktype *pak, 
   						  caller_t caller_pc)
{
    if (sanity_debug) {
   	pak->caller_pc = (ulong)caller_pc;
   	GET_TIMESTAMP(pak->inputtime);
    }
    return;
}

/*
 * buffer_sanity_check_inline
 * Returns TRUE if the buffer is complete.
 */

static inline boolean buffer_sanity_check_inline(paktype *pak) 
{
    pooltype *pool = pak->pool;
    
    if (!sanity_debug)
   	return(TRUE);
 
    if (!pak_pool_item_validate(pool, pak))
   	return(FALSE);
    
    return(TRUE);
}

/*
 * platform_pak_ignore_inputq_inline:
 * Return TRUE if the input queue count should be ignored.
 *
 * Note : Each platform defines pak_ignore_inputq() to either 
 * FALSE or to platform_pak_ignore_inputq_inline(). No direct calls
 * should be made to platform_ignore_inputq_inline(). The platform 
 * specified pak_ignore_inputq() should always be used.
 */
static inline boolean platform_pak_ignore_inputq_inline (paktype *pak)
{
    return(pak->buffer_flags & PAK_BUF_IGNOREQ);
}

/*
 * set_if_input_inline
 *
 * This inline must be called whenever pak->if_input is being set.
 * It takes care of input queueing accounting.
 */

static inline void set_if_input_inline (paktype *pak, idbtype *idb)
{
    if (pak->flags & PAK_INPUTQ) {
        errmsg(&msgsym(INPUTQ, SYS), pak);
    } else {
	pak->if_input = idb;
	if (!pak_ignore_inputq(pak)) {
	    pak->flags |= PAK_INPUTQ;
	    atomic_increment(&idb->hwptr->input_qcount);
	}
    }
}

/*
 * clear_if_input_inline
 *
 * This inline must be called whenever pak->if_input is being cleared.
 * It takes care of input queueing accounting.
 */

static inline void clear_if_input_inline (paktype *pak)
{
    if (pak->flags & PAK_INPUTQ) {
	pak->flags &= ~PAK_INPUTQ;
	if (pak->if_input) {
	    atomic_decrement(&pak->if_input->hwptr->input_qcount);
	} else {
            errmsg(&msgsym(INPUTQ, SYS), pak);
	}
    }
    pak->if_input = NULL;
}

#define PAKDATAMAGIC 0xAFACEFAD
#define PAKDATAVALID(x) ((x)->magic == PAKDATAMAGIC)


/*
 * data_area_to_pakdata
 *
 * Given a pointer to data_area, return a pointer to the pakdata
 * backpointer
 */

static inline pakdata *data_area_to_pakdata (char *data_area)
{
    pakdata *pd;

    pd = (pakdata *)(data_area - sizeof(pakdata));
    return(pd);
}


/*
 * particle_attach_pakdata
 *
 * Attaches a fresh particle to a pakdata block
 */

/*
 * Magic defines for the pakdata structure
 */

#define PAKDATAMAGIC 0xAFACEFAD
#define PAKDATAVALID(x) ((x)->magic == PAKDATAMAGIC)

static inline void particle_attach_pakdata (pooltype     *pool,
					    particletype *particle,
					    pakdata      *data)
{
    /*
     * Fill in our particle header
     */
    particle->pool       = pool;
    particle->data_block = data;
    
    /*
     * Fill in the pakdata header for the data
     */
    data->magic = PAKDATAMAGIC;
}	    


/*
 * platform_pak_has_particles_inline:
 *
 * Returns TRUE if the paktype is built with particles.
 *
 * Note : Each platforms defines pak_has_particles() to either FALSE
 * or to platform_pak_has_particles_inline(). No direct calls should ever
 * be made to the private inline. The platform specific pak_has_particles()
 * should always be used.
 */

static inline boolean platform_pak_has_particles_inline (paktype *pak)
{
    return(pak->particlequeue.count != 0);
}


/*
 * pak_particle_count
 *
 * Returns the number of particles in a packet
 */

static inline int pak_particle_count (paktype *pak)
{
    return(pak->particlequeue.count);
}


/*
 * pak_get_first_particle
 *
 * Returns either the first particle in the particle queue or the
 * internal particle for paktypes without a real particle queue.
 */

static inline particletype *pak_get_first_particle (paktype *pak)
{
    return(pak->particlequeue.qhead);
}


/*
 * pak_get_last_particle
 *
 * Returns either the last particle in the particle queue or the
 * internal particle for paktypes without a real particle queue.
 */

static inline particletype *pak_get_last_particle (paktype *pak)
{
    return(pak->particlequeue.qtail);
}


/*
 * pak_get_leading_size
 *
 * Returns the number of bytes between the start of the valid particle
 * data area and the supplied pointer
 */

static inline uint pak_get_leading_size (paktype *pak, void *ptr)
{
    particletype *particle;
    uint          size, leading = 0;

    if (!ptr)
	return(0);

    particle = pak_get_first_particle(pak);

    while (particle) {
	size = particle->data_bytes;
	if (((uchar *)ptr >= particle->data_start) &&
	    ((uchar *)ptr < (particle->data_start + size))) {
	    leading += (uint)((uchar *)ptr - particle->data_start);
	    break;
	}
	leading  += size;
	particle  = particle->next;
    }
    return(leading);
}


/*
 * pak_get_trailing_size
 *
 * Returns the number of bytes between the supplied pointer and the
 * end of the valid data in the particle data
 */

static inline uint pak_get_trailing_size (paktype *pak, void *ptr)
{
    particletype *particle;
    uint          size, trailing = 0;

    if (!ptr)
	return(0);

    particle = pak_get_first_particle(pak);

    while (particle) {
	size = particle->data_bytes;
	if (((uchar *)ptr >= particle->data_start) &&
	    ((uchar *)ptr < (particle->data_start + size))) {
	    trailing += (size - (uint)((uchar *)ptr - particle->data_start));
	} else {
	    if (trailing)
		trailing += size;
	}
	particle = particle->next;
    }
    return(trailing);
}


/*
 * pak_coalesce_inline
 *
 * Takes a packet comprised of particles and returns a contiguous one
 */

static inline paktype *pak_coalesce_inline (paktype *pak)
{
    paktype *newpak;

    if (!pak_has_particles(pak))
	return(pak);

    newpak = getbuffer(pak->datagramsize);
    if (newpak)
	pak_copy_and_recenter(pak, newpak, pak->datagramsize);
    datagram_done(pak);
    return(newpak);
}



