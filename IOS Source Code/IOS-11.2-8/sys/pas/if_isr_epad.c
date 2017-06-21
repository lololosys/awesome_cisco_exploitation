/* $Id: if_isr_epad.c,v 3.1.62.15 1996/09/11 07:31:55 ptran Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_isr_epad.c,v $
 *------------------------------------------------------------------
 * if_isr_epad.c - Cylink Encryption PA driver interrupt service 
 *                 routines
 *
 * August 1995, Paulina D. Tran
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_isr_epad.c,v $
 * Revision 3.1.62.15  1996/09/11  07:31:55  ptran
 * CSCdi68764:  access list on rsp doesnt get forward to vip
 * Branch: California_branch
 *
 * Revision 3.1.62.14  1996/09/10  11:16:37  xliu
 * CSCdi68600:  ESA should not be given the tralier bytes to encrypt
 * Branch: California_branch
 * Set the desc length as given by crypto engine.
 *
 * Revision 3.1.62.13  1996/09/10  01:15:57  ptran
 * Check crypto vector before using it.
 * CSCdi68571:  VIP crash when crypto map is removed
 * Branch: California_branch
 *
 * Revision 3.1.62.12  1996/09/04  00:59:35  ptran
 * CSCdi67301:  Decrypted data is corrupted under low volt, high temp,
 * high freqq
 * Branch: California_branch
 *
 * Revision 3.1.62.11  1996/08/28  13:05:59  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.62.10  1996/08/16  00:01:14  ptran
 * Increase the descriptor ring and decrease the holdq size.  Some clean
 * up.
 * CSCdi58378:  Encryption PA need performance improvement.
 * Branch: California_branch
 *
 * Revision 3.1.62.9  1996/08/09  21:47:14  ptran
 * Only send packet to media if the fastsend vector is not null, otherwise
 * drop
 * the packet
 * CSCdi64543:  Delete a crypto map with background traffic causes
 * alignment error
 * Branch: California_branch
 *
 * Revision 3.1.62.8  1996/07/18  01:26:48  ptran
 * Added in check to avoid the diagnostic to re-enter the epad_interrupt().
 * CSCdi61737:  Encryption PA diagnostic
 * Branch: California_branch
 *
 * Revision 3.1.62.7  1996/07/03  20:43:26  ptran
 * CSCdi58378:  Encryption PA need performance improvement.
 * Branch: California_branch
 *
 * Revision 3.1.62.6  1996/06/29  01:10:46  ptran
 * CSCdi58378:  Encryption PA need performance improvement.
 * Branch: California_branch
 *
 * Revision 3.1.62.5  1996/06/27  07:04:30  ptran
 * CSCdi58378:  Encryption PA need performance improvement.
 * Branch: California_branch
 *
 * Revision 3.1.62.4  1996/06/21  22:19:23  xliu
 * CSCdi61014:  Migrate to support vip2.
 * Branch: California_branch
 *
 * Revision 3.1.62.3  1996/05/28  18:12:29  ptran
 * CSCdi58378:  Encryption PA need performance improvement.
 * Copy the encryption session control block to descriptor ring entry to
 * reduce number of DMA setup per packet.
 * Uses the PCI mailboxes to post the ring position intead of updating
 * ownership bit.
 * Branch: California_branch
 *
 * Revision 3.1.62.2  1996/04/29  16:59:45  ptran
 * CSCdi56030:  Remove the support of bitflip mode from EPA driver.
 * Branch: California_branch
 *
 * Revision 3.1.62.1  1996/04/19  15:26:01  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.1.2.10  1996/04/16  21:54:52  ptran
 * EPA driver clean up. Removed statistic counters used for debugging
 * purposes.
 * Removed parser commands used for testing. Modified to compile in
 * California
 * branch.  Incorporate code review comments.
 *
 * Branch: IosSec_branch
 *
 * Revision 3.1.2.5  1996/02/14  23:45:03  ptran
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
 * Revision 3.1.2.4  1996/02/02  23:14:31  ptran
 * Commit first release of EPA firmware.
 * Branch: IosSec_branch
 *
 * Revision 3.1.2.3  1996/01/31  04:48:14  xliu
 * Branch: IosSec_branch
 * Updated epa driver.
 *
 * Revision 3.1.2.2  1996/01/08  19:23:53  ptran
 * Branch: IosSec_branch
 *
 * Revision 3.1.2.1  1995/12/06  21:48:55  rmontino
 * Branch: IosSec_branch
 *
 * Revision 3.1  1995/11/09  12:57:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/08/23  02:27:30  ptran
 * Placeholder file for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "sys_registry.h"
#include "media_registry.h"
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
 
#include "../if/ether.h"
#include "../if/pci.h"
#include "../if/network.h"
 
#include "if_pas.h"
#include "msg_pas.c"			/* Not a typo, see logger.h */
#include "msg_epad.c"			/* Not a typo, see logger.h */

#include "if_pas_epad.h"
#if defined(RVIP) || defined(SVIP)
#include "if_vip_epad.h"
#endif
#if defined(PREDATOR)
#include "if_c7100_epad.h"
#endif

#include "../ip/ip.h"
#include "../crypto/crypto_public.h"


/*
 * Forward declarations
 */
static void epad_write_desc_ring (hwidbtype *idb, paktype *pak,
				  boolean continuous, 
				  int buffer_count);
static void epad_safe_start (hwidbtype *idb);
ulong epad_interrupt_handler (hwidbtype *idb);

/*****************************************************************************
 * epad_fastsend: Transmit this packet immediately (if possible)
 *****************************************************************************/
void epad_fastsend (hwidbtype *idb, paktype *pak)
{
    epad_instance_t *ds = INSTANCE(idb);
    boolean continuous;
    int buffer_count;
    char msg[100];

    /*
     * If there are packets on the holdq queue, 
     * queue the packet on the output hold queue.
     */
    if (idb->output_qcount) { 
        epad_holdq_enqueue(pak, idb);
        if (ds->count < ds->num_ring_entries - 1) {
            epad_safe_start(idb);
        } else {
            epad_interrupt_handler(epad_gidb[0]);
        }
        return;
    }

    continuous = !pak_has_particles(pak); 
    buffer_count = (continuous) ? 1 : pak_particle_count(pak); 

    /*
     * If our packet did not fit in the ring,
     * queue the packet on the output hold queue.
     */
    if (buffer_count  <= (ds->num_ring_entries - ds->count - 1)) {
	epad_write_desc_ring(idb, pak, continuous, buffer_count);
    } else if (buffer_count > (ds->num_ring_entries - 1)) {
	/*
	 * If our packet is larger than ring size then
	 * drop the packet
	 */
	sprintf(msg, "Packet too big, size %d, %d particles , %d ring entries.\n",
		pak->datagramsize, buffer_count, ds->num_ring_entries);
	errmsg(&msgsym(WARNING, EPAD), msg);
	ds->num_too_big++;
        epad_drop_packet(idb, pak);
    } else {
        epad_holdq_enqueue(pak, idb);
        if (ds->count < ds->num_ring_entries - 1) {
            epad_safe_start(idb);
        }
    } 
}

/*****************************************************************************
 *  epad_write_desc_ring:  Put in descriptor ring
 *  returns: 
 *      0 - success, should interrupt the EPA
 *      1 - failed, ring full, should enqueue in holdq
 *     -1 - fatal error, should drop packet
 *****************************************************************************/
static void epad_write_desc_ring (hwidbtype *idb, paktype *pak,
			      boolean continuous, int buffer_count)
{
    epad_instance_t *ds = INSTANCE(idb);
    epad_desc_t *desc = NULL;
    epad_shadow_t *shdw = NULL;
    particletype *mp;
    uint i;
    uint tail;
    hwidbtype *outidb = NULL;
    ushort total_length;

    /*
     * Packet is guaranteed to fit
     * If frame ring, copy the encryption control block to 
     * the descriptor ring
     * Frames assume:
     *         pak->unspecified[0]      = encryption header
     *         pak->if_output->hwidb    = output hwidb if encrypting
     *         pak->cryptoflags         = cmd flags(encr/decr
     */
    if (idb->slotunit == EPAD_FRAME_DEVICE) {
	outidb = hwidb_or_null(pak->if_output);	
        desc = &ds->beg[ds->tail];
 
        mp = (particletype *)pak->unspecified[0];
        if (!mp) {
            errmsg(&msgsym(WARNING, EPAD), "missing encryption header"); 
	    ds->num_nohdr++;
            epad_drop_packet(idb, pak);
	    return;
        }

	bcopy(mp->data_start, (uchar *)desc->cntl_blk, EPAD_CNTL_BLK_SIZE);
        bcopy(&desc->cntl_blk[22], (uchar *)&total_length, 2);
	retparticle(mp);
	pak->unspecified[0] = (ulong)NULL;
    } else {
        total_length = pak->datagramsize;
    }

    /*
     * Write pak to ring            
     */
    if (continuous) {
	/*
	 * Continuous pak --- no particles
	 * Add buffer to ring
	 */
        desc = &ds->beg[ds->tail];
	desc->buffer = EPAD_BUF_ADDR(pak->datagramstart);
	desc->length = pak->datagramsize;
        desc->numbuf = buffer_count; 

        shdw = &ds->shadow[ds->tail];
        shdw->continuous = TRUE;
        shdw->pak = pak;         

	ds->bytes_snt += desc->length;
        ds->count++;    
	EPAD_ADVANCE_INDEX(ds->tail,ds->adv_idx_mask);
    } else {
	/*
	 * Packet has particles
	 * Dequeue and add particles to ring
	 */
        while (mp = particle_dequeue_inline(pak), mp) {
            desc = &ds->beg[ds->tail];
	    desc->buffer = EPAD_BUF_ADDR(mp->data_start);
	    desc->length = mp->data_bytes;
            desc->numbuf = buffer_count;

            shdw = &ds->shadow[ds->tail];
            shdw->continuous = FALSE;
            shdw->mp = mp;
            shdw->pak = ds->pak;

	    ds->bytes_snt += desc->length;
            ds->count++;
	    EPAD_ADVANCE_INDEX(ds->tail,ds->adv_idx_mask);
        }
    }
    /* sub tract the trailer bytes from the end of the packet */
    if (total_length < pak->datagramsize) {
        desc->length -= (pak->datagramsize - total_length);
    }
   
    tail = ds->tail;
    /* 
     * Save info in pak structure that needed by crypto engine
     * Only save these information to the last descriptor of a packet.
     */
    shdw->outidb = outidb;
    if (!continuous) {
        shdw->flags = pak->cryptoflags;
        shdw->conn_id = pak->crypto_conn_id;
        shdw->if_input = pak->if_input;
        shdw->network_start = pak->network_start;
    }

    /*
     * Fill in ownership from last to first
     */
    EPAD_RETRACT_INDEX(tail,ds->adv_idx_mask); 
    if (buffer_count == 1) {
        ds->beg[tail].control = EPAD_FIRST | EPAD_LAST | EPAD_FW_OWN;
    } else {
        ds->beg[tail].control = EPAD_LAST;
        EPAD_RETRACT_INDEX(tail,ds->adv_idx_mask); 
        for (i=1;i<buffer_count-1;i++) {
           ds->beg[tail].control = EPAD_MID;
           EPAD_RETRACT_INDEX(tail,ds->adv_idx_mask);
        }
        ds->beg[tail].control = EPAD_FIRST | EPAD_FW_OWN;
    }
    ds->num_snt++;
}

/*****************************************************************************
 * epad_safe_start: Take packets from the hold queue and puts them in the
 * descriptor ring
 *****************************************************************************/
static void epad_safe_start (hwidbtype *idb)
{
    epad_instance_t *ds = INSTANCE(idb);
    paktype *pak;
    boolean continuous = FALSE;
    int buffer_count;
    char msg[100];

    /*
     * Dequeue and attempt to write to descriptor ring
     */
    while ( (pak = (*idb->oqueue_dequeue)(idb)) ) {
        /*ds->num_hq_deq++;*/
	continuous = !pak_has_particles(pak);
	buffer_count = (continuous) ? 1 : pak_particle_count(pak);
	/*
	 * See if there's room in the descriptor ring
	 */
	if (buffer_count <= (ds->num_ring_entries - ds->count - 1)) {
	    epad_write_desc_ring(idb, pak, continuous, buffer_count);
	} else if (buffer_count > (ds->num_ring_entries - 1)) {
	    sprintf(msg, "Packet too big, size %d, %d particles , %d ring entries.\n",
		    pak->datagramsize, buffer_count, ds->num_ring_entries);
	    errmsg(&msgsym(WARNING, EPAD), msg);
	    ds->num_too_big++;
	    epad_drop_packet(idb,pak);
	} else {
	    pak_requeue_head(idb, pak);
            /*ds->num_hq_req++;*/
	    break;
	}
        if (!continuous) {
	    datagram_done(pak);
        }
    }
}
    
/*****************************************************************************
 * epad_interrupt: 
 * The EPA has interrupted to notify this driver that there's result or 
 * response packet on the descriptor ring
 * The idb determines the ring
 * On Frame ring: always call fastsend after encrypting
 *                always call callback after decrypting
 * On Key ring:   always call callback
 *****************************************************************************/
void epad_interrupt (hwidbtype *idb, ushort index)
{
    uint i;
    epad_instance_t *ds = INSTANCE(idb);
    epad_desc_t *desc;
    epad_shadow_t *shdw;
    fastsend_t media_fastsend = NULL;
    char msg[100];
    static boolean reentrance = FALSE;
    boolean error = FALSE;
    int buffer_count =0;

    if (reentrance == TRUE) {
        return;
    } else {
        reentrance = TRUE;
    }

    /*
     * Process ring
     */ 
    for (i=0; ds->count && i<ds->max_bufs_serv; i++) {	
        if (index == ds->head) {
            break;
        }
        desc = &ds->beg[ds->head];
	shdw = &ds->shadow[ds->head];

        desc->control = desc->control & (~EPAD_FW_OWN);
        /*
         * Check if firmware returned descriptor error
         * If so just ignore this packet for now
         */
        if (desc->control & EPAD_ERROR) {
	    sprintf(msg, "desc err 0x%08x: control = 0x%x, numbuf = %d\n", 
		    desc, desc->control, desc->numbuf);
	    errmsg(&msgsym(ERROR, EPAD), msg);
	    ds->num_desc_err++;
            error = TRUE;
        }

        switch (desc->control) {
	  case EPAD_ONLY:
	  case EPAD_FIRST:
            if (error) {
	        break;
            }
	    if (shdw->continuous == FALSE) {
		epad_cleanup_particles(ds->pak);
	    }
	    /* 
	     * On frame ring: check status field for error at the beginning
	     * of a packet. And drop the entire packet if there's an error.
	     */
	    if (idb->slotunit == EPAD_FRAME_DEVICE) {
		char status;
		status = ((epacmd_t *)desc->cntl_blk)->status;
		/* error detected, ignore the whole packet */
		if (status != 0) {
		    sprintf(msg, "encryption status = 0x%x\n", status);
		    errmsg(&msgsym(ERROR, EPAD), msg);
		    ds->num_status_err++;
                    error = TRUE;
                    break;
		} 
	    }
	  case EPAD_MID: 
	  case EPAD_LAST:
	    if (error) {
		break;
	    }
	    if (shdw->continuous == FALSE) {
		particle_enqueue_inline(ds->pak,shdw->mp);
	    }

	    if (desc->control == EPAD_ONLY || desc->control == EPAD_LAST) {
		if (shdw->continuous == FALSE) {
                    shdw->pak->cryptoflags = shdw->flags;
		    shdw->pak->crypto_conn_id = shdw->conn_id;
		    shdw->pak->if_input = shdw->if_input;
		    shdw->pak->network_start = shdw->network_start;
		}
		if ((idb->slotunit == EPAD_FRAME_DEVICE) && shdw->outidb) {
                    if (shdw->outidb->crypto_vectors) {
                        media_fastsend = ((crypto_vector_t *)
                              (shdw->outidb->crypto_vectors))->media_fastsend;
                        media_fastsend(shdw->outidb, shdw->pak);
                    } else {
                        epad_drop_packet(idb, shdw->pak);
                    } 
		} else {
		    ds->callback(shdw->flags, shdw->pak);
		}
		ds->num_rcv++;
	    }
	    break;
	  default: 
	    sprintf(msg,"FATAL: Invalid control 0x%02x\n",desc->control);
	    errmsg(&msgsym(ERROR, EPAD), msg);
	}

	ds->bytes_rcv += desc->length;
	ds->count--;
	buffer_count++;
        desc->control = EPAD_ONLY;
	EPAD_ADVANCE_INDEX(ds->head,ds->adv_idx_mask);
    }

    if (idb->output_qcount) {
        epad_safe_start(idb);
    }
    reentrance = FALSE;
}

/*****************************************************************************
 * epad_interrupt_handler
 *****************************************************************************/
ulong epad_interrupt_handler (hwidbtype *fidb)
{
    union {
	ulong w;
	ushort ind[2];
    } cause;
    epad_instance_t *fds;
    hwidbtype *kidb = epad_gidb[1]; 
    epad_instance_t *kds; 
    
    fds = INSTANCE(fidb);
    kds = INSTANCE(kidb);

    if (fds->op_regs->mbef & IMB3_FULL) {
	cause.w = fds->op_regs->imb3;
	
	if(fds->head != cause.ind[1]) {
	    epad_interrupt(fidb, cause.ind[1]);
	}
	if(kds->head != cause.ind[0]) {
	    epad_interrupt(kidb, cause.ind[0]);
	}
    }
    return (0); /*fds->op_regs->mbef & IMB3_FULL);*/
}

void pas_mngmt_interrupts (uint pa_bay, boolean onoff);
/*****************************************************************************
 * epa_nmi_handler
 *****************************************************************************/
void epa_nmi_handler (void *ptr)
{
    epad_instance_t *ds = ptr;
    amcc_op_regs_t *op_regs = ds->op_regs;
    ulong mbef;

    mbef = op_regs->mbef;
    if (mbef & IMB1_FULL) {
	errmsg(&msgsym(ERROR, EPAD), "0x%x\n", op_regs->imb1);
    } else {
	errmsg(&msgsym(ERROR, EPAD), "error number not posted\n");
    }
    pas_deactivate_bay(ds->pacb.pa_bay);
}
