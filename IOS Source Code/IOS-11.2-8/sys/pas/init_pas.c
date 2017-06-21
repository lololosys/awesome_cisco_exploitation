/* $Id: init_pas.c,v 3.4.18.10 1996/08/28 13:07:00 thille Exp $
 * $Source: /release/112/cvs/Xsys/pas/init_pas.c,v $
 *------------------------------------------------------------------
 * init_pas.c - Port Adaptor initialization code
 *
 * January 1995, David Getchell 
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: init_pas.c,v $
 * Revision 3.4.18.10  1996/08/28  13:07:00  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.18.9  1996/06/11  14:12:42  dcarroll
 * CSCdi59976:  Partially inserted PA will remain unusable
 * Branch: California_branch
 * Reset cached eeprom data when deactivating PA.
 *
 * Revision 3.4.18.8  1996/05/22  18:55:02  dcarroll
 * CSCdi57774:  Indefinite retries if init fails.
 * Branch: California_branch
 * Add support to limit init retries.
 *
 * Revision 3.4.18.7  1996/04/29  19:27:48  mbeesley
 * CSCdi55915:  One broken PA causes all interfaces to be ignored
 * Branch: California_branch
 *
 * Revision 3.4.18.6  1996/04/05  06:30:17  dcarroll
 * CSCdi53603:  avoid hogging cpu in analyze path
 * Branch: California_branch
 * Add platform dependant delay routines.
 *
 * Revision 3.4.18.5  1996/04/05  01:49:59  mbeesley
 * CSCdi52646:  Absent port adaptors cause incorrect error message
 * Branch: California_branch
 *
 * Revision 3.4.18.4  1996/04/04  20:04:30  getchell
 * CSCdi50943:  SVIP PA reset broken
 * Branch: California_branch
 *
 * Revision 3.4.18.3  1996/04/03  18:25:24  dcarroll
 * CSCdi53341:  clean up tables after oir removal
 * Branch: California_branch
 * Add routines to uninstall tables.
 *
 * Revision 3.4.18.2  1996/03/21  23:24:07  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.4.18.1  1996/03/18  21:34:56  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.9  1996/03/18  23:29:13  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.8  1996/02/17  05:54:36  fsunaval
 * CSCdi49008:  Fddi output counters are not reported and other fixes.
 * Branch: ELC_branch
 * 1.Output counts not increasing.
 * 2.Make FDDI on VIP memory-mapped.
 * 3.Distributed switching support.
 * 4.Reduce burst-size from 64 to 32.
 *
 * Revision 3.2.2.7  1996/02/14  01:58:19  gstovall
 * CSCdi48946:  Pull in 4T bugfixes from mainline
 * Branch: ELC_branch
 * Bring the ELC 4T driver up to date.
 *
 * Revision 3.2.2.6  1996/02/10  00:31:01  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.2.2.5  1996/02/08  08:48:21  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.4  1996/01/25  12:10:11  fsunaval
 * Branch: ELC_branch
 * 1.Predator FDDI support.
 * 2.Patch CSCdi46358 (PA management interrupt) changes into ELC_branch.
 *
 * Revision 3.2.2.3  1995/12/26  19:57:05  fsunaval
 * CSCdi46111:  Add parser support for FDDI PA and other changes
 * Branch: ELC_branch
 * 1.Add missing diffs from previous commit for init_pas.c
 *   (NMI interrupts).
 * 2.Add FDDI PA parser support (sh cont, test pas fddi).
 * 3.Do a little code cleanup of hes_fddi.c and rsp_fip.c specifically
 *   in regards to the way trt and tvx are passed down.
 *
 * Revision 3.2.2.2  1995/12/12  04:54:01  mbeesley
 * Branch: ELC_branch
 * Allow "show pci bridge" to display all bridges. Change the 1fe GPC
 * register settings for Predator IO card 1fe. Fix some amdp2 error
 * messages. And turn power off on unrecognized port adaptors.
 *
 * Revision 3.2.2.1  1995/11/30  02:06:43  dcarroll
 * Branch: ELC_branch
 *
 * Revision 3.4  1996/02/12  02:56:40  rharagan
 * CSCdi48311:  4T PA will hang rvip if download fails
 * 4T needs to see rising and faling edge after power is put on the PA.
 *
 * Revision 3.3  1996/01/12  21:52:47  jkuzma
 * CSCdi46358:  VIP1 PA mngmt and DMA interrupt stacks swapped
 *              General debug of PA management interrupts
 *
 * Revision 3.2  1995/11/17  18:55:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:58:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:43  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files
 */
#include "master.h"
#include "logger.h"
#include "interface_private.h"
#include "interface.h"
#include "sys_registry.h"
#include "stacks.h"
#include "packet.h"
#include "subsys.h"

#include "if_pas.h"
#include "msg_pas.c"			/* Not a typo, see logger.h */

/*
 * Globals
 */
pa_jumptable_t pa_jumptable[MAX_PA_BAYS];
pa_nmi_handler_t pa_nmi_handler_table[MAX_PA_BAYS + 1];
pa_nmi_handler_t *pa_nmi_dispatch_table[(1<<MAX_PA_BAYS)];

/*
 * pas_init_jumptable :
 * Initialize the pointers used in the pa jumptable
 */
void pas_init_jumptable (void)
{
    int i, j;
    pa_jumptable_t *ptr;

    for (i=0; i<MAX_PA_BAYS; i++) {
	ptr = &pa_jumptable[i];
	ptr->wrapper = pa_default_wrapper;
	for(j=0; j<MAX_INTERFACES_PER_PA; j++) {
	    ptr->idb[j] = (hwidbtype *)NULL;
	}
    }
}

/*
 * pas_init_network_level :
 * Setup any wrapper initialization and create a Net IO stack
 */
void pas_init_network_level (void)
{
    /*
     * Create the network interface interrupt stack:
     */
    createlevel(LEVEL_PA_NETWORK, NULL, "Network interfaces");
}

/*
 * pas_init_pa_nmi_handler :
 * Initialize the PA NMI interrupt tables and register the interrupt
 * dispatcher.
 */
void pas_init_pa_nmi_handler (void)
{
    ushort i;

    for (i=0; i<=MAX_PA_BAYS; i++) {
        pa_nmi_handler_table[i].func = NULL;
        pa_nmi_handler_table[i].arg = NULL;
        pa_nmi_handler_table[i].mask = ~(1 << i);
        pa_nmi_handler_table[i].prior = PA_NMI_PRIOR_ZERO;
    }

    for (i=0; i<(1<<MAX_PA_BAYS); i++)
        pa_nmi_dispatch_table[i] = &pa_nmi_handler_table[MAX_PA_BAYS];

    createlevel (LEVEL_PA_MANAGEMENT, pas_nmi_dispatcher, 
		 "PA Management Int Handler");
}

/*
 * pas_install_port_instance :
 * Install the ds pointer into the appropriate element in
 * the ds array of the pa_jumptable
 */
void pas_install_port_instance (hwidbtype *idb, boolean enable_interrupt)
{
    leveltype level;
    pas_cbtype *pascb;
    pa_jumptable_t *ptr;
    int intfc;

    pascb = INSTANCE(idb);
    if (pascb->pa_bay >= MAX_PA_BAYS) {
	errmsg(&msgsym(BADPABAY, PA), pascb->pa_bay);
	crashdump(0);
    }
    ptr = &pa_jumptable[pascb->pa_bay];
    intfc = pascb->pa_device;
    if (intfc > MAX_INTERFACES_PER_PA) {
	errmsg(&msgsym(BADINTERFACE, PA), intfc, pascb->pa_bay);
	crashdump(0);
    } else {
	level = raise_interrupt_level(ALL_DISABLE);
	ptr->idb[intfc] = idb;
	pas_netio_interrupts(pascb->pa_bay, enable_interrupt);
	reset_interrupt_level(level);
    }
}

/*
 * pas_install_wrapper:
 * Install the wrapper pointer, and the port base address into the 
 * port adapter jumptable  
 */
void pas_install_wrapper (uint pa_bay, pa_wrapper_t handler)
{
    pa_jumptable_t *ptr;

    if (pa_bay >= MAX_PA_BAYS) {
	errmsg(&msgsym(BADPABAY, PA), pa_bay);
	crashdump(0);
    }
    ptr = &pa_jumptable[pa_bay];
    ptr->wrapper = handler;
    ptr->pa_base_address = get_io_base_address(pa_bay);
}

/*
 * Clear the jumptable entry for a PA that was removed.
 */
void pas_uninstall_jumptable_entry (uint pa_bay)
{
    int i;
    pa_jumptable_t *jump_ptr;
    leveltype level;

    if (pa_bay >= MAX_PA_BAYS) {
	errmsg(&msgsym(BADPABAY, PA), pa_bay);
	crashdump(0);
    }

    level = raise_interrupt_level(ALL_DISABLE);
    /*
     * Reset jump table entry.
     */
    jump_ptr = &pa_jumptable[pa_bay];
    jump_ptr->wrapper = pa_default_wrapper;
    for (i = 0; i < MAX_INTERFACES_PER_PA; i++) {
	jump_ptr->idb[i] = NULL;
    }

    /*
     * Reset the netio interrupts and restore
     * interrupt level.
     */
    pas_netio_interrupts(pa_bay, FALSE);
    reset_interrupt_level(level);
}

static void pas_dispatch_table_update (void)
{
    ushort index, index_mask;
    int i;
    pa_nmi_handler_t *cur_ptr, *hi_prior_ptr;

    /* Update dispatching table according to new priorities */
    for (index=1; index<(1<<MAX_PA_BAYS); index++) {
        hi_prior_ptr = pa_nmi_dispatch_table[index]; /* current setting */
        for (i=0, index_mask=1; i<MAX_PA_BAYS; i++, index_mask<<=1) {
            if (index & index_mask) {
                cur_ptr = &pa_nmi_handler_table[i];
                if (cur_ptr->prior > hi_prior_ptr->prior)
                    hi_prior_ptr = cur_ptr;
            }
        }
        pa_nmi_dispatch_table[index] = hi_prior_ptr;
    }
}

/*
 * pas_install_pa_nmi_handler :
 *
 * Interrupt Handlers are assigned priorities based on the PA
 * type. Each dispatch table entry contains a pointer to the
 * interrupt handler table entry which describes the function
 * (associated to the highest priority PA) to be called when
 * the interrupt mask bits assume the combination which indexes
 * the dispatch table.
 */
void pas_install_pa_nmi_handler (uint pa_bay, pa_nmi_wrapper_t func, 
                                 void *arg, int priority)
{
    leveltype level;

    /* Check arguments */
    if (pa_bay >= MAX_PA_BAYS || 		/* past the last bay ? 	    */
	!func || 				/* Null handler ?	    */
	priority > PA_NMI_PRIOR_HI ||		/* Above highest priority ? */
	priority < PA_NMI_PRIOR_LOW) { 		/* Below lowest priority ?  */

        /* This should really be an assertion... */
        errmsg(&msgsym(RUPTCONFLICT, PA),"management",pa_bay);
        return;
    }

    /* 
     * Lock out interrupts to stop getting a PA NMI interrupt while 
     * the PA NMI interrupt table is changing
     */
    level = raise_interrupt_level(LEVEL_PA_MANAGEMENT);
       
    /* Fill pa_bay table entry */
    pa_nmi_handler_table[pa_bay].func = func;
    pa_nmi_handler_table[pa_bay].arg = arg;
    pa_nmi_handler_table[pa_bay].prior = priority;
    pa_nmi_handler_table[pa_bay].mask = ~(1 << pa_bay);

    /*
     * Update the dispatch table according to the
     * priorities of the remaining PAs.
     */
    pas_dispatch_table_update();

    /*
     * Enable the management interrups for this bay
     */
    pas_mngmt_interrupts(pa_bay, TRUE);
    reset_interrupt_level(level);
}

/*
 * When a PA is removed, clean up the handler table
 * entry for the slot. Then update the dispatch table
 * entries based on the remaining PAs.
 */
void pas_uninstall_pa_nmi_handler (uint pa_bay)
{
    ushort i;
    pa_nmi_handler_t *pa_nmi_ptr;
    leveltype level;

    if (pa_bay >= MAX_PA_BAYS) {
	errmsg(&msgsym(BADPABAY, PA), pa_bay);
	crashdump(0);
    }

    /* 
     * Lock out interrupts to stop getting a PA NMI interrupt while 
     * the PA NMI interrupt table is changing
     */
    level = raise_interrupt_level(LEVEL_PA_MANAGEMENT);

    /*
     * Get pointer to handler table entry for the slot that has been 
     * emptied. Find all entries in the dispatch table that 
     * point to this entry and point them at the dummy entry.
     */
    pa_nmi_ptr = &pa_nmi_handler_table[pa_bay];
    for (i=1; i<(1<<MAX_PA_BAYS); i++) {
	if (pa_nmi_dispatch_table[i] == pa_nmi_ptr) {
	    pa_nmi_dispatch_table[i] = &pa_nmi_handler_table[MAX_PA_BAYS];
	}
    }

    /*
     * Clear the handler table entry.
     */
    pa_nmi_ptr->func = NULL;
    pa_nmi_ptr->arg = NULL;
    pa_nmi_ptr->mask = ~(1 << pa_bay);
    pa_nmi_ptr->prior = PA_NMI_PRIOR_ZERO;

    /*
     * Update the dispatch table according to the
     * priorities of the remaining PAs.
     */
    pas_dispatch_table_update();

    /*
     * Disable the management interrups for this bay
     */
    pas_mngmt_interrupts(pa_bay, FALSE);
    reset_interrupt_level(level);
}

/*
 * pas_analyze_interfaces : 
 * Analyze all the PA bays in the system, starting up drivers
 */
void pas_analyze_interfaces (void)
{
    tinybool bridge_ok[MAX_PA_BAYS];
    uint pa_bay;
    int pa_type; 

    /*
     * Loop thru all present PA's, put them in reset
     * and power them on. Also mark bridge_ok.
     */
    for (pa_bay=0; pa_bay<MAX_PA_BAYS; pa_bay++) { 
	bridge_ok[pa_bay] = TRUE;
	if (pas_pa_present(pa_bay) && !pas_pa_analyzed(pa_bay)) {
	    /*
	     * Mark this slot to have an OIRable PA and 
	     * enable OIR interrupts if applicable.
	     */
	    pas_pa_exists(pa_bay);
	    pas_control_reset(pa_bay, FALSE);
	    pas_control_power(pa_bay, TRUE);
	}
    }

    /*
     * Delay to allow all the PA hardware to stabilize after power on
     */
    pas_delay(300);

    for (pa_bay=0; pa_bay<MAX_PA_BAYS; pa_bay++) { 
        if (pas_pa_present(pa_bay) && !pas_pa_analyzed(pa_bay)) {
	    pas_control_reset(pa_bay, TRUE);
	}
    }
    pas_delay(20);

    for (pa_bay=0; pa_bay<MAX_PA_BAYS; pa_bay++) { 
	if (pas_pa_present(pa_bay) && !pas_pa_analyzed(pa_bay)) {
	    pas_control_reset(pa_bay, FALSE);

	    /* 
	     * Initialize the system side bridge for this PA
	     */
	    if (!pas_bridge_config(pa_bay)) {
		pas_deactivate_bay(pa_bay);
		bridge_ok[pa_bay] = FALSE;
		errmsg(&msgsym(PABRIDGE, PA), pa_bay);
	    }
	}
    }
    
    /*
     * Delay to allow all the PA hardware to stabilize after reset
     */
    pas_delay(400);

    /*
     * Loop thru them all again, analyzing them
     */
    for (pa_bay=0; pa_bay<MAX_PA_BAYS; pa_bay++) { 
	/*
	 * See if the port adaptor is present and defined. If so call the
	 * driver's analyze routine if the bridge init succeeded.
	 */
        if (pas_pa_present(pa_bay) && !pas_pa_analyzed(pa_bay) && 
	    bridge_ok[pa_bay]) {

	    /*
	     * get pa controller type and hw version
	     */
	    pa_type = pas_get_pa_type(pa_bay);

	    if (reg_used_analyze_interface(pa_type)) {
		reg_invoke_analyze_interface(pa_type, pa_bay);
	    } else {
		errmsg(&msgsym(UNDEFPA, PA), pa_type, pa_bay);
		pas_deactivate_bay(pa_bay);
	    }
	}
    }
}

/*
 * pas_subsys_init_class :
 * Callback routine from the subsys_class_init() call
 */
void pas_subsys_init_class (ushort class)
{
   /*
    * If we've just started the driver subsystems, start the port
    * adaptor interfaces for real.
    */
    if (class == SUBSYS_CLASS_DRIVER) {
	pas_slots_init();
	pas_analyze_interfaces();
    }
}

/*
 * pas_if_final_init :
 * Called after all interfaces have been discovered and initialized
 */
void pas_if_final_init (void)
{
    /*
     * Initialize parser chains before the subsystems try to link
     * into the pas or pci chain
     */
    pas_parser_init();

    /*
     * Call the platform specific routine to do any post
     * analyze work. On VIP, this puts the interfaces admin down
     */
    pas_platform_if_final_init();
}

/* end of file */
