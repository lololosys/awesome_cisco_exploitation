/* $Id: if_vip_epad.h,v 3.1.64.6 1996/08/28 13:06:50 thille Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_vip_epad.h,v $
 *------------------------------------------------------------------
 * if_vip_epad.h - VIP Encryption Port Adaptor driver specific 
 *		   header file.
 *
 * August 1995, Paulina D. Tran
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_vip_epad.h,v $
 * Revision 3.1.64.6  1996/08/28  13:06:50  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.64.5  1996/07/18  01:26:54  ptran
 * Added in check to avoid the diagnostic to re-enter the epad_interrupt().
 * CSCdi61737:  Encryption PA diagnostic
 * Branch: California_branch
 *
 * Revision 3.1.64.4  1996/07/03  20:43:32  ptran
 * CSCdi58378:  Encryption PA need performance improvement.
 * Branch: California_branch
 *
 * Revision 3.1.64.3  1996/06/29  01:10:53  ptran
 * CSCdi58378:  Encryption PA need performance improvement.
 * Branch: California_branch
 *
 * Revision 3.1.64.2  1996/06/27  07:04:38  ptran
 * CSCdi58378:  Encryption PA need performance improvement.
 * Branch: California_branch
 *
 * Revision 3.1.64.1  1996/04/19  15:26:15  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.1.2.9  1996/04/16  21:54:58  ptran
 * EPA driver clean up. Removed statistic counters used for debugging
 * purposes.
 * Removed parser commands used for testing. Modified to compile in
 * California
 * branch.  Incorporate code review comments.
 *
 * Branch: IosSec_branch
 *
 * Revision 3.1.2.5  1996/02/14  23:45:10  ptran
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
 * Revision 3.1.2.4  1996/02/02  23:14:52  ptran
 * Commit first release of EPA firmware.
 * Branch: IosSec_branch
 *
 * Revision 3.1.2.3  1996/01/31  04:48:21  xliu
 * Branch: IosSec_branch
 * Updated epa driver.
 *
 * Revision 3.1.2.2  1996/01/08  19:23:59  ptran
 * Branch: IosSec_branch
 *
 * Revision 3.1.2.1  1995/12/06  21:49:04  rmontino
 * Branch: IosSec_branch
 *
 * Revision 3.1  1995/11/09  12:58:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/08/23  02:27:40  ptran
 * Placeholder file for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __IF_VIP_EPAD_H__
#define __IF_VIP_EPAD_H__

/*
 * Include VIP header files
 */
#include "../src-vip/vip_memd.h"
#include "../src-vip/vip_af.h"

#define EPAD_BUF_ADDR(x)  (x)
#define EPAD_CNTL_ADDR(x)  (x)
#define CHECK_BYTE  (uchar)0xAA

/*
 * Externs
 */
extern hwidbtype *epad_idb_create (uint slot, uint port);
extern void epad_deallocate (hwidbtype *hwidb);
extern boolean epad_init_buffer_pools (hwidbtype *idb);
extern void epad_drop_packet (hwidbtype *idb, paktype *pak);

/*****************************************************************************
 * epad_flush_pak:
 * NULL out the relevant entries in the paktype
 *****************************************************************************/
static inline void epad_flush_pak (paktype *pak)
{
    pak->datagramsize = 0;
    pak->datagramstart = NULL;
}

/*****************************************************************************
 * epad_cleanup_particle:
 * Return any particles and then NULL out relevant entries in the paktype
 *****************************************************************************/
static inline void epad_cleanup_particles (paktype *pak)
{
    particletype *mp;
 
    while (mp = particle_dequeue_inline(pak), mp) {
	retparticle(mp);
    }
   
    epad_flush_pak(pak);
}

/****************************************************************************
  epad_holdq_enqueue -
  ****************************************************************************/
static inline void epad_holdq_enqueue (paktype *pak, hwidbtype *idb)
{
    epad_instance_t *ds = INSTANCE(idb);
    paktype *newpak;
    
    if (idb->output_qcount >= ds->holdq_size) {
        ds->num_holdq_full++;
        /*errmsg(&msgsym(ERROR, EPAD) ,"HoldQ is full\n");*/
	epad_drop_packet(idb, pak);
	return;
    }

    if (pak_has_particles(pak)) {
        /*
         * Reparent onto a new paktype header as we can not hold onto
         * the one passed into us.
         */
        newpak = particle_reparent(pak, idb->headerpool);
        if (!newpak) {
            errmsg(&msgsym(ERROR, EPAD), 
		   "Out of pak, cannot enqueue holdqueue\n");
            epad_cleanup_particles(pak);
            ds->num_no_hdr_buffer++;
            return;
        }
        newpak->unspecified[0] = pak->unspecified[0];
        newpak->if_output = pak->if_output;
        newpak->cryptoflags = pak->cryptoflags;
	newpak->crypto_conn_id = pak->crypto_conn_id;
	newpak->if_input = pak->if_input;
	newpak->network_start = pak->network_start;
    } else {
        newpak = pak;
    }
    holdq_enqueue_fast(idb, newpak);
    /*ds->num_hq_enq++; */
}

extern inline particletype *epad_get_particle_inline (hwidbtype *, boolean);
#endif __IF_VIP_EPAD_H__



