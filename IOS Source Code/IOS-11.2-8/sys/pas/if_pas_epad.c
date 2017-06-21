/* $Id: if_pas_epad.c,v 3.1.64.16 1996/09/11 20:17:38 ptran Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_pas_epad.c,v $
 *------------------------------------------------------------------
 * if_pas_epad.c - PAS specific Encryption PA driver routines.
 *
 * August 1995, Paulina D. Tran
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_pas_epad.c,v $
 * Revision 3.1.64.16  1996/09/11  20:17:38  ptran
 * Remove download messages from EPA.
 * CSCdi68840:  EPA should not display download done messages
 * Branch: California_branch
 *
 * Revision 3.1.64.15  1996/09/09  23:34:39  ptran
 * CSCdi68463:  ESA driver cleanup
 * Branch: California_branch
 *
 * Revision 3.1.64.14  1996/09/04  00:59:37  ptran
 * CSCdi67301:  Decrypted data is corrupted under low volt, high temp,
 * high freqq
 * Branch: California_branch
 *
 * Revision 3.1.64.13  1996/08/28  13:06:21  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.64.12  1996/08/19  19:55:48  ptran
 * Add ESA's idb to VIP and RSP hwidb/swidb chain.
 * CSCdi64021:  show diag doesnt display EPA info when the next PA is empty
 * Branch: California_branch
 *
 * Revision 3.1.64.11  1996/07/18  01:26:50  ptran
 * Added in check to avoid the diagnostic to re-enter the epad_interrupt().
 * CSCdi61737:  Encryption PA diagnostic
 * Branch: California_branch
 *
 * Revision 3.1.64.10  1996/07/03  20:43:29  ptran
 * CSCdi58378:  Encryption PA need performance improvement.
 * Branch: California_branch
 *
 * Revision 3.1.64.9  1996/06/29  01:10:49  ptran
 * CSCdi58378:  Encryption PA need performance improvement.
 * Branch: California_branch
 *
 * Revision 3.1.64.8  1996/06/27  09:38:56  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.1.64.7  1996/06/27  07:04:33  ptran
 * CSCdi58378:  Encryption PA need performance improvement.
 * Branch: California_branch
 *
 * Revision 3.1.64.6  1996/06/21  22:19:27  xliu
 * CSCdi61014:  Migrate to support vip2.
 * Branch: California_branch
 *
 * Revision 3.1.64.5  1996/06/07  20:43:10  fred
 * CSCdi58655:  turning RED off on an interface turns WFQ off on that
 * interface
 *         save default setting and restore it when folks remove RED,
 *         Custom, or Priority Queuing
 * Branch: California_branch
 *
 * Revision 3.1.64.4  1996/05/28  18:12:34  ptran
 * CSCdi58378:  Encryption PA need performance improvement.
 * Copy the encryption session control block to descriptor ring entry to
 * reduce number of DMA setup per packet.
 * Uses the PCI mailboxes to post the ring position intead of updating
 * ownership bit.
 * Branch: California_branch
 *
 * Revision 3.1.64.3  1996/05/21  10:00:00  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.64.2  1996/04/29  16:59:49  ptran
 * CSCdi56030:  Remove the support of bitflip mode from EPA driver.
 * Branch: California_branch
 *
 * Revision 3.1.64.1  1996/04/19  15:26:09  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.1.2.10  1996/04/16  21:54:54  ptran
 * EPA driver clean up. Removed statistic counters used for debugging
 * purposes.
 * Removed parser commands used for testing. Modified to compile in
 * California
 * branch.  Incorporate code review comments.
 *
 * Branch: IosSec_branch
 *
 * Revision 3.1.2.7  1996/02/14  23:45:04  ptran
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
 * Revision 3.1.2.6  1996/02/04  11:16:33  ptran
 * Branch: IosSec_branch
 *
 * Revision 3.1.2.5  1996/02/02  23:14:41  ptran
 * Commit first release of EPA firmware.
 * Branch: IosSec_branch
 *
 * $Log: if_pas_epad.c,v $
 * Revision 3.1.64.16  1996/09/11  20:17:38  ptran
 * Remove download messages from EPA.
 * CSCdi68840:  EPA should not display download done messages
 * Branch: California_branch
 *
 * Revision 3.1.64.15  1996/09/09  23:34:39  ptran
 * CSCdi68463:  ESA driver cleanup
 * Branch: California_branch
 *
 * Revision 3.1.64.14  1996/09/04  00:59:37  ptran
 * CSCdi67301:  Decrypted data is corrupted under low volt, high temp,
 * high freqq
 * Branch: California_branch
 *
 * Revision 3.1.64.13  1996/08/28  13:06:21  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.64.12  1996/08/19  19:55:48  ptran
 * Add ESA's idb to VIP and RSP hwidb/swidb chain.
 * CSCdi64021:  show diag doesnt display EPA info when the next PA is empty
 * Branch: California_branch
 *
 * Revision 3.1.64.11  1996/07/18  01:26:50  ptran
 * Added in check to avoid the diagnostic to re-enter the epad_interrupt().
 * CSCdi61737:  Encryption PA diagnostic
 * Branch: California_branch
 *
 * Revision 3.1.64.10  1996/07/03  20:43:29  ptran
 * CSCdi58378:  Encryption PA need performance improvement.
 * Branch: California_branch
 *
 * Revision 3.1.64.9  1996/06/29  01:10:49  ptran
 * CSCdi58378:  Encryption PA need performance improvement.
 * Branch: California_branch
 *
 * Revision 3.1.64.8  1996/06/27  09:38:56  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.1.64.7  1996/06/27  07:04:33  ptran
 * CSCdi58378:  Encryption PA need performance improvement.
 * Branch: California_branch
 *
 * Revision 3.1.64.6  1996/06/21  22:19:27  xliu
 * CSCdi61014:  Migrate to support vip2.
 * Branch: California_branch
 *
 * Revision 3.1.64.5  1996/06/07  20:43:10  fred
 * CSCdi58655:  turning RED off on an interface turns WFQ off on that
 * interface
 *         save default setting and restore it when folks remove RED,
 *         Custom, or Priority Queuing
 * Branch: California_branch
 *
 * Revision 3.1.64.4  1996/05/28  18:12:34  ptran
 * CSCdi58378:  Encryption PA need performance improvement.
 * Copy the encryption session control block to descriptor ring entry to
 * reduce number of DMA setup per packet.
 * Uses the PCI mailboxes to post the ring position intead of updating
 * ownership bit.
 * Branch: California_branch
 *
 * Revision 3.1.64.3  1996/05/21  10:00:00  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.64.2  1996/04/29  16:59:49  ptran
 * CSCdi56030:  Remove the support of bitflip mode from EPA driver.
 * Branch: California_branch
 *
 * Revision 3.1.64.1  1996/04/19  15:26:09  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.1.2.10  1996/04/16  21:54:54  ptran
 * EPA driver clean up. Removed statistic counters used for debugging
 * purposes.
 * Removed parser commands used for testing. Modified to compile in
 * California
 * branch.  Incorporate code review comments.
 *
 * Branch: IosSec_branch
 *
 * Revision 3.1.2.7  1996/02/14  23:45:04  ptran
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
 * Revision 3.1.2.4  1996/01/31  04:48:17  xliu
 * Branch: IosSec_branch
 * Updated epa driver.
 *
 * Revision 3.1.2.1  1995/12/06  21:48:59  rmontino
 * Branch: IosSec_branch
 *
 * Revision 3.1  1995/11/09  12:57:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/08/23  02:27:34  ptran
 * Placeholder file for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "logger.h"
#include "msg_pas.c"			/* Not a typo, see logger.h */
#include "msg_epad.c"			/* Not a typo, see logger.h */
#include "../ui/common_strings.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "ttysrv.h"
#include "subsys.h"
#include "sched.h"
#include "parser.h"
 
#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"
 
#include "../if/pci.h"
#include "../hes/if_fci.h"
#include "../hes/dgbus.h"
#include "../if/network.h"
 
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_priority.h"

#include "../hes/ccb_cmd.h"
#include "../hes/if_fci.h"

#include "if_pas.h"
#include "pas_id.h"

#include "if_pas_epad.h"

#include "if_vip_epad.h"


#define ALTERNATE     NONE
#include "epad_show_chain.h"
LINK_POINT (show_epad_commands, ALTERNATE);
#undef ALTERNATE

static parser_extension_request epadshow_chain_init_table[] = {
    { PARSE_ADD_SHOW_PAS_CMD, &pname(show_epad_commands) },
    { PARSE_LIST_END, NULL }
};


hwidbtype *epad_gidb[2] = {NULL, NULL};
/*****************************************************************************
 * epad_clear_ds - clear the driver instance structure.
 *****************************************************************************/
void epad_clearcounters (uint ring)
{
    epad_instance_t *ds;
    hwidbtype *idb;

    /*** Get pointer to ds ***/
    if (!epad_gidb[ring]) {
	ttyprintf(CONTTY,"Error: IDB not found: Driver not initialized\n");
	return;
    }
    idb = epad_gidb[ring];
    ds = INSTANCE(epad_gidb[ring]);
    ds->num_snt = 0;
    ds->num_rcv = 0;
    ds->bytes_snt = 0;
    ds->bytes_rcv = 0;
    ds->num_too_big = 0;
    ds->num_nohdr = 0;
    ds->num_desc_err = 0;
    ds->num_status_err = 0;
    ds->num_drop = 0;
    ds->num_holdq_full = 0;
    ds->num_no_hdr_buffer = 0;
    ds->num_no_buffer = 0;
    ds->num_hq_enq = 0;
    ds->num_hq_deq = 0;
    ds->num_hq_req = 0;
    GET_TIMESTAMP(ds->last_clear);
}
void epad_clear_ds_cmd (parseinfo *csb)
{
    epad_clearcounters(GETOBJ(int,1));
}

/*****************************************************************************
 * epad_show_ds - display the driver instance structure.
 *****************************************************************************/
void epad_show_ds_cmd (parseinfo *csb)
{
    epad_instance_t *ds;
    uint ring = GETOBJ(int,1);
    hwidbtype *idb;

    /*** Get pointer to ds ***/
    if (!epad_gidb[ring]) {
        ttyprintf(CONTTY,"Error: IDB not found: Driver not initialized\n");
        return;
    }
    idb = epad_gidb[ring];
    ds = INSTANCE(epad_gidb[ring]);

    if (ring == EPAD_FRAME_DEVICE) 
        ttyprintf(CONTTY,"\nFRAME driver instance: 0x%x\n", ds);
    else
        ttyprintf(CONTTY,"\nKEY driver instance: 0x%x\n", ds);
    ttyprintf(CONTTY,"    pascb: \n");
    ttyprintf(CONTTY,"        idb:                0x%x\n", ds->pacb.idb);
    ttyprintf(CONTTY,"        pa_bay:             %d\n", ds->pacb.pa_bay);
    ttyprintf(CONTTY,"        pa_device:          %d\n", ds->pacb.pa_device);

    ttyprintf(CONTTY,"    beg:                    0x%x\n", ds->beg);
    ttyprintf(CONTTY,"    head:                   %d\n", ds->head);
    ttyprintf(CONTTY,"    tail:                   %d\n", ds->tail);
    ttyprintf(CONTTY,"    count:                  %d\n", ds->count);
    ttyprintf(CONTTY,"    shad_beg:               0x%x\n", ds->shadow);
    ttyprintf(CONTTY,"    static pak:             0x%08x\n", ds->pak);
    ttyprintf(CONTTY,"        size:                 %d\n", ds->pak->datagramsize);
    ttyprintf(CONTTY,"        num part:             %d\n", pak_particle_count(ds->pak));
    ttyprintf(CONTTY,"        first:                0x%08x\n", 
                                          pak_get_first_particle(ds->pak));
    ttyprintf(CONTTY,"        last:                 0x%08x\n",
                                          pak_get_last_particle(ds->pak));
    ttyprintf(CONTTY,"    callback:                 0x%08x\n", ds->callback);

    ttyprintf(CONTTY,"    num too big:            %d\n", ds->num_too_big);
    ttyprintf(CONTTY,"    num no header:          %d\n", ds->num_nohdr);
    ttyprintf(CONTTY,"    num ring error:         %d\n", ds->num_desc_err);
    ttyprintf(CONTTY,"    num status error:       %d\n", ds->num_status_err);
    ttyprintf(CONTTY,"    num holdq full:         %d\n", ds->num_holdq_full);
    ttyprintf(CONTTY,"    num no hdr buffer:      %d\n", ds->num_no_hdr_buffer);
    ttyprintf(CONTTY,"    num no buf:             %d\n", ds->num_no_buffer);

    ttyprintf(CONTTY,"    num_hq_enq:             %d\n", ds->num_hq_enq);
    ttyprintf(CONTTY,"    num_hq_deq:             %d\n", ds->num_hq_deq);
    ttyprintf(CONTTY,"    num_hq_req:             %d\n", ds->num_hq_req);
    ttyprintf(CONTTY,"    num_snt:                %d\n", ds->num_snt);
    ttyprintf(CONTTY,"    num_rcv:                %d\n", ds->num_rcv);
    ttyprintf(CONTTY,"    bytes_snt:              %d\n", ds->bytes_snt);
    ttyprintf(CONTTY,"    bytes_rcv:              %d\n", ds->bytes_rcv); 
    ttyprintf(CONTTY,"    num drop:               %d\n", ds->num_drop);

}

/*****************************************************************************
 * epad_show_idb_cmd - display driver idb structure
 *****************************************************************************/
void epad_show_idb_cmd (parseinfo *csb)
{
    int dev;
    idb_si_counters_t	*sic;

    /*** Get pointer to ds ***/
    if (!epad_gidb[0] && !epad_gidb[1]) {
        ttyprintf(CONTTY,"Error: IDB not found: Driver not initialized\n");
        return;
    }

    for (dev=0; dev<= 1; dev++) {
	hwidbtype *idb = epad_gidb[dev];
	sic = idb_get_counter_info(idb, IDB_COUNTER_SI_CNT_PTR);
	ttyprintf(CONTTY,"\n%s idb = 0x%x\n", dev ? "Key" : "Frame", 
		idb);
	ttyprintf(CONTTY,"    oqueue =           0x%x\n",idb->oqueue);
	ttyprintf(CONTTY,"    oqueue_dequeue =   0x%x\n",
		idb->oqueue_dequeue);
	ttyprintf(CONTTY,"    outputq[0] =       0x%x\n",
		idb->outputq[0]);
	ttyprintf(CONTTY,"    outputq[1] =       0x%x\n",
		idb->outputq[1]);
	ttyprintf(CONTTY,"    pool =             0x%x\n",
		idb->pool);
	ttyprintf(CONTTY,"    headerpool =       0x%x\n",
		idb->headerpool);
	ttyprintf(CONTTY,"    output_qcount =    %d\n", 
		idb->output_qcount);
	ttyprintf(CONTTY,"    output drop =     %d\n", 
	    idb->counters.output_total_drops);
	ttyprintf(CONTTY,"\n  Output queue %d/%d, %u drops; input queue %d/%d, %u drops",
               idb->outputq[DEFAULT_QUEUE(idb)].count,
               idb->outputq[DEFAULT_QUEUE(idb)].maximum,
               (idb->counters.output_total_drops - sic->output_total_drops),
               idb->input_qcount, idb->input_defqcount,
               (idb->counters.input_drops - sic->input_drops));
    }
}

/*****************************************************************************
 * epad_show_desc_rings_cmd - display the driver descriptor ring
 *****************************************************************************/
void epad_show_desc_rings_cmd (parseinfo *csb)
{
    uint ring = GETOBJ(int,1);
    epad_instance_t *ds;
    epad_desc_t *desc;
    epad_shadow_t *shdw;
    int i;
    int num;
    char headch,tailch;

    if (ring != EPAD_KEY_DEVICE)
        ring = EPAD_FRAME_DEVICE;

    /*** Get pointer to ds ***/
    if (!epad_gidb[ring]) {
        ttyprintf(CONTTY,"Error: IDB not found: Driver not initialized\n");
        return;
    }
 
    ds = INSTANCE(epad_gidb[ring]);
    
    desc=ds->beg;
    shdw=ds->shadow;
    num=ds->num_ring_entries;
        
    if (ring == EPAD_FRAME_DEVICE)
        ttyprintf(CONTTY,"Frame - desc:0x%08x shdw:0x%08x\n",desc,shdw);
    else
        ttyprintf(CONTTY,"Key - desc:0x%08x shdw:0x%08x\n",desc,shdw);
    ttyprintf(CONTTY,
"#    o/p/st  nb leng buffer     c pak        mp         outidb     flags\n");
    headch = tailch = ' ';
    for (i=0;i<num;i++) {
        if (i==ds->head)
            headch='<';
        else
            headch  = ' ';
        if (i==ds->tail)
            tailch='>';
        else
           tailch=' ';
        
     ttyprintf(CONTTY,
"#%2d %c%d/%d/%2d%c %02d %04d 0x%08x %d 0x%08x 0x%08x 0x%08x 0x%04x\n",
              i,tailch,
              (desc->control&EPAD_FW_OWN) >> 7,
              (desc->control&EPAD_ONLY) >> 5,
              desc->control&EPAD_ERROR,
              headch,
              desc->numbuf, desc->length,desc->buffer, 
              shdw->continuous,shdw->pak,shdw->mp,shdw->outidb,shdw->flags);    
        desc++;
        shdw++;
    }
}



/*****************************************************************************
 * epad_init_idb:  IDB initialization for a Cylink Encryption Port Adaptor 
 *****************************************************************************/
static hwidbtype *epad_init_idb (uint pa_bay, uint device_no)
{
    hwidbtype *idb;

    /*
     * Alloc memory for IDB
     */
    idb = epad_idb_create(pa_bay, device_no);
    if (!idb) {
        errmsg(&msgsym(INITFAIL, EPAD), "epad_idb_create");
	return(NULL);
    }

    /*
     * Save our unit number and PA slot number:
     */
    idb->slot = pa_bay;       /* Corresponds to IP */
    idb->unit = 1;            /* nth interface */
    idb->subunit = device_no; /* Corresponds to viper concept of interface # */
    idb->slotunit = device_no;/* For slotted interface names */

    /*
     * Crypto will call this vector                 
     */
    idb->fastsend = epad_fastsend;

    if (device_no == EPAD_FRAME_DEVICE) {
        idb->name = "ESA-Frame";
        idb->hw_namestring = "ESA-Frame";
    } else {
        idb->name = "ESA-Key";
        idb->hw_namestring = "ESA-Key";
    }
    idb->status |= IDB_DELETED;

    /*
     * Setup the idb so that the local VIP IOS does not generate
     * keepalives (the master IOS on the RP will do that if it is needed)
     */
    idb->nokeepalive = TRUE;

    /*
     * Needed in hold q processing
     */
    idb->old_queue_algorithm = idb->queue_algorithm = QUEUE_FCFS;
    if (device_no == EPAD_FRAME_DEVICE)
        holdq_init(idb,EPAD_FRAME_HOLDQ_SIZE);
    else
        holdq_init(idb,EPAD_KEY_HOLDQ_SIZE);

    /*
     * These are used in holdq_dequeue
     */
    idb->getlink = (linktype_t)return_zero;
    idb->enctype = ET_NULL;

    idb->pool = NULL;
    idb->pool_group = POOL_GROUP_INVALID;
    /*
     * Initialize buffer pools
     */
    epad_init_buffer_pools(idb);

    return(idb);
}

/*****************************************************************************
 * epad_init_ds:  Create and initialize the ds structure (device instance)
 *                    for a CYLINK EPA driver.
 *****************************************************************************/
static epad_instance_t *epad_init_ds (hwidbtype *idb, amcc_op_regs_t *op_regs, 
                                                                uint device_no)
{
    epad_instance_t *ds;
    pas_cbtype *pascb; 
 
    /*
     * Alloc memory for ds
     */
    ds = malloc_fast(sizeof(epad_instance_t));
    if (!ds) {
        errmsg(&msgsym(INITFAIL, EPAD), "ds=malloc_fast");
	return(NULL);
    }

    /*
     * Set up pointer to control & status registers in ds
     */
    ds->op_regs = op_regs;

    pascb = &ds->pacb;
    pascb->idb = idb;
    pascb->pa_bay = idb->slot;
    pascb->pa_device = idb->subunit;
    pascb->pa_ifindex = (pascb->pa_bay * MAX_INTERFACES_PER_PA) +
        			pascb->pa_device;
    /*
     * Setup the removal vector here so that if the init process
     * fails from here on in, there will be a device specific vector
     * defined to clean up resources
     */
    pascb->pas_deallocate = epad_deallocate;

    /*
     * Init the static pak
     */
    ds->pak = pas_allocate_fspak();
    if (!ds->pak) {
         errmsg(&msgsym(INITFAIL, EPAD), "cannot init static pak");
         free(ds);
         return(NULL);
    }  


    /*
     * Set up some device dependent constants 
     */
    if (device_no == EPAD_FRAME_DEVICE) { 
        ds->callback = crypto_engine_callback;
        ds->num_ring_entries = EPAD_FRAME_RING_ENTRIES;
        ds->max_bufs_serv = EPAD_MAX_FRAME_BUFS_TO_SERVICE;
        ds->adv_idx_mask = EPAD_FRAME_RING_INDEX_MASK;
        ds->holdq_size = EPAD_FRAME_HOLDQ_SIZE;
    } else {
        ds->callback = crypto_engine_key_callback;
        ds->num_ring_entries = EPAD_KEY_RING_ENTRIES;
        ds->max_bufs_serv = EPAD_MAX_KEY_BUFS_TO_SERVICE;
        ds->adv_idx_mask = EPAD_KEY_RING_INDEX_MASK;
        ds->holdq_size = EPAD_KEY_HOLDQ_SIZE;
    }

    return(ds);
}
/*****************************************************************************
 * epad_init_desc_ring:  Create and initialize the descriptor rings
 * which is an array of epad buffer descriptors.  
 *****************************************************************************/
static boolean epad_init_desc_ring (hwidbtype *idb)
{
    epad_instance_t *ds = INSTANCE(idb);
    epad_desc_t *desc;
    epad_shadow_t *shdw;
    int i;
 
    /*
     * Allocate and initialize frame/key descriptor ring
     */
    ds->beg = malloc_named_pcimem((ds->num_ring_entries)*sizeof(epad_desc_t),
				  idb->hw_namestring);
    if (!ds->beg) {
        errmsg(&msgsym(INITFAIL, EPAD),"epad_init_desc_rings");
        return(FALSE);
    }
 
    for (i=0; i<ds->num_ring_entries; i++) {
        desc = &ds->beg[i];
        desc->control = EPAD_ONLY;
        desc->numbuf = 0;
        desc->length = 0;
        desc->buffer = NULL; 
	bzero(desc->cntl_blk, EPAD_CNTL_BLK_SIZE);
    }
 
    EPAD_RESET_INDEX(ds->head);
    EPAD_RESET_INDEX(ds->tail);
    ds->count = 0;
    ds->num_snt = 0;
    ds->num_rcv = 0;
    ds->num_too_big = 0;
    ds->num_nohdr = 0;
    ds->num_desc_err = 0;
    ds->num_status_err = 0;
    ds->num_holdq_full = 0;
    ds->num_no_hdr_buffer = 0;
    ds->num_no_buffer = 0;
    ds->num_drop = 0;
    ds->num_hq_enq = 0;
    ds->num_hq_deq = 0;
    ds->num_hq_req = 0;

    /*
     * Allocate and initialize shadow descriptor ring
     * no need for wrap descriptor here
     */
    ds->shadow = malloc_fast(ds->num_ring_entries*sizeof(epad_shadow_t));
    if (!ds->beg) {
        errmsg(&msgsym(INITFAIL, EPAD),"epad_init_desc_rings");
        return(FALSE);
    }

    for (i=0; i<ds->num_ring_entries; i++) {
        shdw = &ds->shadow[i];
        shdw->continuous = FALSE;
        shdw->pak = NULL;
        shdw->mp = NULL;
        shdw->outidb = NULL;
        shdw->flags = 0x0000;
	shdw->conn_id = 0;
	shdw->if_input = NULL;
	shdw->network_start = NULL;
    }
 
    return(TRUE);
}

/*****************************************************************************
 * epad_recv_mbx:  recv mailbox response from EPA
 * args:
 *     ver1, ver2, ver3  -  verify response in mbox 1,2,3
 *     res1, res2, res3  -  copy response from mbox 1,2,3
 *****************************************************************************/
static boolean epad_recv_mbx (amcc_op_regs_t *op_regs, 
                          ulong ver1, ulong ver2, ulong ver3, 
                          ulong *res1, ulong *res2, ulong *res3)
{
    ulong mbef;
    int t=0;
    char msg[100];

    while (t<EPAD_READ_RETRY) {
        mbef = op_regs->mbef;
	if ((!res1 || (mbef & IMB1_FULL)) &&
	    (!res2 || (mbef & IMB2_FULL)) &&
	    (!res3 || (mbef & IMB3_FULL))) {
            break;
        }
        t++; 
    }
    if (t == EPAD_READ_RETRY) {
	if (!(!res1 || (mbef & IMB1_FULL))) {
	    sprintf(msg, 
		    "mailbox 1 timeout: mbef=0x%08x, expecd mb1 = 0x%x\n", 
		    mbef, ver1);
	    errmsg(&msgsym(INITFAIL, EPAD), msg);
		   
	} 
	if (!(!res2 || (mbef & IMB2_FULL))) {
	    sprintf(msg, 
		    "mailbox 2 timeout: mbef=0x%08x, expect mb2 = 0x%x\n", 
		    mbef, ver2);
	    errmsg(&msgsym(INITFAIL, EPAD), msg);
	} 
        errmsg(&msgsym(INITFAIL, EPAD), "mailbox communication timeout");
        return(FALSE);
    } 

    if (res1)
        *res1 = op_regs->imb1;
    if (res2) 
        *res2 = op_regs->imb2;
    if (res3) 
        *res3 = op_regs->imb3;

    if (ver1 && res1 && (*res1 != ver1)) {
	sprintf(msg, "mbox1: expect %x, received %x\n", ver1, *res1);
	errmsg(&msgsym(INITFAIL, EPAD), msg);
        return(FALSE);
    }
    if (ver2 && res2 && (*res2 != ver2)) {
	sprintf(msg, "mbox2: cmd %x, expect %x, received %x\n", *res1, 
		ver2, *res2);
	errmsg(&msgsym(INITFAIL, EPAD), msg);
        return(FALSE);
    }

    if (ver3 && res3 && (*res3 != ver3)) {
	sprintf(msg, "mbox3: expect %x, received %x\n", ver3, *res3);
	errmsg(&msgsym(INITFAIL, EPAD), msg);
        return(FALSE);
    }
    
    return(TRUE);
}

/*****************************************************************************
 * epad_send_init_block - Send taskqueue begin and end address to 
 * the EPA card.
 *****************************************************************************/
boolean epad_send_init_block (hwidbtype *idb)
{
    epad_instance_t *ds;
    amcc_op_regs_t *op_regs;
    ulong mb1,mb2;
    ulong cmd;

    ds = INSTANCE(idb);

    if (idb->slotunit == EPAD_FRAME_DEVICE)
        cmd = EPAD_CMD_INIT_BLOCK_HIGH;
    else
        cmd = EPAD_CMD_INIT_BLOCK_LOW;
        
    op_regs = ds->op_regs;

    op_regs->omb3 = ds->num_ring_entries;
    op_regs->omb2 = (ulong)EPAD_CNTL_ADDR(ds->beg);
    op_regs->omb1 = cmd;

    mb2 = EPAD_CMD_RESP_OK;
    if (!epad_recv_mbx(op_regs, cmd, mb2, 0, &mb1, &mb2, NULL)) 
        return(FALSE);

    return(TRUE); 
}

/*****************************************************************************
 * epad_download -
 * Down load the executable code stored in epad_download_code[] array. 
 * This array is created using the srecParser utility which parsed the 
 * srecords and save the data/code portion into epad_download_code[] array.  
 * The start download address is saved in epad_download_addr global
 * variable.
 *****************************************************************************/
boolean epad_download (amcc_op_regs_t *op_regs)
{
    ulong mb1,mb2,mb3;
    int i,j;
    ulong *lp;
    char msg[100];

    /* 
     * Clear incoming mailboxes
     */
    mb1 = op_regs->imb1;
    mb2 = op_regs->imb2;
    mb3 = op_regs->imb3;

    /* 
     * The driver asks the EPA if it is ready for download.
     * EPA replies with crc of already loaded code.
     */

    op_regs->omb1 = EPAD_CMD_DOWNLOAD_READY;
    mb1 = EPAD_CMD_RESP_OK;
    if (!epad_recv_mbx(op_regs, mb1, 0, 0, &mb1, &mb2, &mb3)) {
	errmsg(&msgsym(INITFAIL, EPAD), "Download ready failed\n");
	return(FALSE);    
    }

    /* 
     * Check the CRC
     */
    if (mb3 == EPAD_FW_CRC) {
	goto download_exec;
    }

    /*
     * For each contiguous region
     */
    for (i=0;i<EPAD_FW_NUM_REGIONS;i++) {
        /*
         * Send the load address of this region with first 4 bytes of data
         */
        lp = (ulong *)epad_fw_code[i];
        op_regs->omb3 = *lp;
        op_regs->omb2 = epad_fw_load_addr[i];
        op_regs->omb1 = EPAD_CMD_DOWNLOAD;

        /* Read response in mailbox 1 and 2 */
        mb1 = EPAD_CMD_DOWNLOAD;
        mb2 = EPAD_CMD_RESP_OK;

        if (!epad_recv_mbx(op_regs, mb1, mb2, 0, &mb1, &mb2, NULL)) {
	    errmsg(&msgsym(INITFAIL, EPAD), "download cmd failed\n");
            return(FALSE);
	}

        /*
         * Send rest of data for this region
         */
        for (j=4;j<epad_fw_code_size[i];j+=4) {
            /* Read response in mailbox 1 and 2 */
            mb1 = EPAD_CMD_DOWNLOAD;
            mb2 = EPAD_CMD_RESP_OK;
	
            lp = (ulong *)&epad_fw_code[i][j];
            op_regs->omb2 = *lp;
            op_regs->omb1 = EPAD_CMD_DOWNLOAD;
            if (!epad_recv_mbx(op_regs, mb1, mb2, 0, &mb1, &mb2, NULL)) {
		sprintf(msg, "download cmd2 failed reg = %d pos=%d\n", i, j);
		errmsg(&msgsym(INITFAIL, EPAD), msg);
                return(FALSE);
	    }
        }
    }

    /*
     * The EPA driver sends command DOWNLOAD_DONE to notify the EPA card
     * that it has downloaded all the code.
     */
    op_regs->omb2 = EPAD_FW_CRC;
    op_regs->omb1 = EPAD_CMD_DOWNLOAD_DONE;

    mb1 = EPAD_CMD_DOWNLOAD_REPORT;
    mb2 = EPAD_CMD_RESP_OK;
    if (!epad_recv_mbx(op_regs, mb1, mb2, 0, &mb1, &mb2, NULL)) {
	errmsg(&msgsym(INITFAIL, EPAD), "download done failed\n");
	return(FALSE);
    }

  download_exec:
    /*
     * The EPA driver sends to the EPA card the address for 68360 execution.
     */
    op_regs->omb1 = EPAD_CMD_DOWNLOAD_EXEC;

    mb1 = EPAD_CMD_DOWNLOAD_EXEC;
    mb2 = EPAD_CMD_RESP_OK;
    if (!epad_recv_mbx(op_regs, mb1, mb2, 0, &mb1, &mb2, NULL)) {
	errmsg(&msgsym(INITFAIL, EPAD), "download exec failed\n");
        return(FALSE);
    }
    return(TRUE);
}

/*****************************************************************************
 * epad_pci_init: Preform PCI configuration specific to the Cylink
 * Crypto's PCI controller (AMCC).
 * Configuration include setting command, latency timer,
 * and base address registers.
 *****************************************************************************/
amcc_op_regs_t *epad_pci_init (uint pa_bay, uint device_no)
{
    ulong vendor_device_id;
    uint value;
    amcc_op_regs_t *device_base;
    uchar bus_no;
    char msg[100];
 
    /* Fetch PCI device configuration information */
    bus_no = get_pci_bus_no(pa_bay);

    /*
     * Read the device/vendor registers to verify that this
     * is an AMCC PCI controller.
     */
    vendor_device_id = pci_config_read(bus_no, device_no, 0,
				       PCI_VENDOR_ID_OFFSET);
 
    if (vendor_device_id != AMCC_VENDOR_DEVICE_ID) {
	sprintf(msg, "invalid vendor id =%x\n", vendor_device_id);
        errmsg(&msgsym(INITFAIL, EPAD), msg);
        return(NULL);
    }

    /*
     * Write to the PCI Base address register #0
     * Writing 'value' there lets us use 'device_base' later
     */
    value = get_pci_device_base_offset(pa_bay, device_no);
    pci_config_write(bus_no, device_no, 0, PCI_IO_BASE_OFFSET, value);

    /*
     * Get device base address. Where we find the 
     * operational registers
     */
    device_base = get_pci_device_base(pa_bay, device_no);
 
    /*
     * Configure PCICMD, the command configuration register.
     * The write clears PCISTS, the status register.
     * 
     * Set bus master enable and I/O access enable.
     */
    value = PCI_COMMAND_IOEN  | PCI_COMMAND_MEMEN | PCI_COMMAND_BMEN;
    pci_config_write(bus_no, device_no, 0, PCI_COMMAND_REG_OFFSET,
		     value);
    
    /*
     * Configure LAT, the configuration latency timer register.
     * Note this actual offset for LAT is 0x0D; PCI_LATENCY_OFFFSET
     * is 0x0C.
     */
    value = EPAD_LATENCY_TIMER_VALUE;
    pci_config_write(bus_no, device_no, 0, PCI_LATENCY_OFFSET, 
		     value);

    /* 
     * Set read & write enable in bus master operation register 
     */
    value = EPAD_BUS_MSTR_RD_XFER_ENABLE | EPAD_BUS_MSTR_WR_XFER_ENABLE;
    device_base->mcsr = value;

    /*
     * Set interrupt control to interrupt us on imcoming mailbox 3
     */
    value = device_base->intcsr;
    value &= EPAD_INTCSR_INBOX3_MASK;
    value |= EPAD_INTCSR_INBOX3_ENABLE;
    device_base->intcsr = value;
 

    return(device_base);
}

/*****************************************************************************
 * epad_analyze_device:  Entry point into the device level driver for 
 *                       Cylink Encryption port adapter.
 *****************************************************************************/
static boolean epad_analyze_device (uint pa_bay, uint device_no, 
                                                  amcc_op_regs_t *op_regs)
{
    hwidbtype *idb = NULL;
    epad_instance_t *ds;

    /*
     * Initialize the idb
     */
    idb = epad_init_idb(pa_bay, device_no);
    if (!idb) {
        errmsg(&msgsym(INITFAIL, EPAD), "epad_init_idb");
	return(FALSE);
    }
   
 
    /*
     * Initialize the ds
     */
    ds = epad_init_ds(idb, op_regs, device_no);
    if (!ds) {
        errmsg(&msgsym(INITFAIL, EPAD),"epad_init_ds");
	return(FALSE);
    }
    
    INSTANCE(idb) = ds;

    /*
     * Create and initialize the descriptor ring structures
     */
    if (!epad_init_desc_ring(idb)) {
        errmsg(&msgsym(INITFAIL, EPAD),"epad_init_desc_ring");
	return(FALSE);
    }

    /*
     * Send the EPA the init block.
     */
    if (!epad_send_init_block(idb)) {
	errmsg(&msgsym(INITFAIL, EPAD),"epad_send_init_block");
	return(FALSE);
    } 

    epad_platform_init(idb);
    /*
     * Allow epad_wrapper to get this idb          
     * Must install frame idb first
     */
    pas_install_port_instance(idb, TRUE);

    crypto_register_hwidb(idb->slotunit, idb);
    epad_gidb[device_no] = idb; /* For testing purposes onnly */

    return(TRUE);
}

/*****************************************************************************
 * pci_epad_pa_analyze_interface -
 * Init the pci to pci bridge above the port adaptor, then discover
 * the port adaptor.
 *****************************************************************************/
static void epad_analyze_pa (int pa_bay)
{
    amcc_op_regs_t *op_regs;
    epad_instance_t *ds;

    /*
     * Perform all PCI configuration necessary to access the PCI
     * bus. This is specific to this device and is done only once.
     */
    op_regs = epad_pci_init(pa_bay, 0); /* device_no = 0 */
    if (!op_regs) {                     /* there is only one amcc */
	pas_deactivate_bay(pa_bay);
        return;
    }

    /*
     * Call analyze device directly. But let us treat this like
     * 2 devices. The frame encryption device and the key mgmt device.
     * epad_wrapper requires frame idb be init'd first
     */
    if (!epad_analyze_device(pa_bay, EPAD_FRAME_DEVICE, op_regs))  {
        pas_deactivate_bay(pa_bay);
        return;
    }
    if (!epad_analyze_device(pa_bay, EPAD_KEY_DEVICE, op_regs))  {
        pas_deactivate_bay(pa_bay);
        return;
    }

    /*
     * download code to EPA
     */
    if (!epad_download(op_regs)) {
	errmsg(&msgsym(INITFAIL, EPAD),"download failed");
	pas_deactivate_bay(pa_bay);
        return;
    }
    DELAY(60); /* permit the '360 to load it's Xlinix */

    /*
     * Install PA wrapper
     */
    pas_install_wrapper(pa_bay, epad_wrapper);
    pas_install_pa_nmi_handler(pa_bay, epa_nmi_handler, 
			       epad_gidb[EPAD_FRAME_DEVICE], PA_NMI_PRIOR_LOW);
    ds = INSTANCE(epad_gidb[EPAD_KEY_DEVICE]);
    op_regs->omb2 = (ulong)(&ds->beg[ds->head]);
    /* 
     * Add in the epa parser chain 
     */
    parser_add_command_list(epadshow_chain_init_table, "epadshow");
}
 
/*****************************************************************************
 * epad_subsys_init - Encryption Port Adaptor Driver subsystem initialization
 * entry point
 *****************************************************************************/
static void epad_subsys_init (subsystype *subsys)
{
    /*
     * Register the PCI ENCRYPTION PA.
     */
    reg_add_analyze_interface(PA_HARDWARE_EEC,
                              epad_analyze_pa,
                              "epad_analyze_pa");
}

/*
 * EPA subsystem header
 */

#define EPAD_MAJVERSION   1
#define EPAD_MINVERSION   0
#define EPAD_EDITVERSION  1

SUBSYS_HEADER(epad, EPAD_MAJVERSION, EPAD_MINVERSION,
	      EPAD_EDITVERSION,
	      epad_subsys_init, SUBSYS_CLASS_DRIVER,
	      NULL,
	      NULL);
