/* $Id: c4000_nim.h,v 3.4 1996/02/18 23:00:47 vdukki Exp $
 * $Source: /release/111/cvs/Xsys/src-c4000/c4000_nim.h,v $
 *------------------------------------------------------------------
 * c4000_nim.h - Include for for NIM handler routines
 *
 * February 1995, Scott Mackie (moved from init_c4000.h)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c4000_nim.h,v $
 * Revision 3.4  1996/02/18  23:00:47  vdukki
 * CSCdi47769:  need to distinguish 4700m model using snmp
 * Modified the code to recognize the 4700m platform using snmp.
 *
 * Revision 3.3  1996/02/18  22:03:42  vdukki
 * CSCdi41637:  need to distinguish between 4700 models via snmp
 * Modified code to return new values for cardType and chassisType mib
 * attributes for 4700s platform.
 *
 * Revision 3.2  1995/11/17  18:45:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:26:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/25  17:45:14  mleelani
 * CSCdi37536:  Fix the C4700 Millisecond Ticks.
 * Add support for the 133.33 mhz pipe.
 *
 * Revision 2.2  1995/06/28  09:31:03  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  22:56:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define C4000_NUM_SLOTS		3       /* # of slots on C4000 Core Board */
#define C4000_EVEN_UNIT		(!(idb->subunit & 1))
#define C4000_MAX_SUBUNITS      8      /* max number of NIM subunits */
#define REVNUM_4000             0xA0   /* To differentiate between c4000 */
                                       /* and c4000m models              */
#define REVNUM_4500             0x01   /* To differentiate between c4500 */
                                       /* and c4500m models              */
#define REVNUM_4700             0x02   /* To differentiate between c4700 */
                                       /* and c4700m models              */


/*
 * Cookie defines
 *
 * The previous 4000 cookie only had space for 6 addresses. 
 * The current 4000 has space for 24 addresses, ie 24/3 subunits are possible.
 */
#define C4000_MAX_COOKIE_SUBUNITS  8
#define C4000_MAX_COOKIE_ADDRESS   (C4000_MAX_COOKIE_SUBUNITS * C4000_NUM_SLOTS)

#define C4000_NIM_HI_IRQ_PRIOR_BSI   3 /* FDDI has highest IRQ4 priority */
#define C4000_NIM_HI_IRQ_PRIOR_BRI   2 /* BRI has next highest IRQ4 priority */
#define C4000_NIM_HI_IRQ_PRIOR_LOW   1 /* All other NIMs have low IRQ4 priority */
#define C4000_NIM_HI_IRQ_PRIOR_ZERO  0 /* IRQ4 priority of unused slots */

typedef struct hi_irq_handler_struc {
    void (* func)(ushort);
    ushort arg;
    ushort mask;
    ushort prior;
} hi_irq_handler_t;

/*
 * Externs
 */

extern hi_irq_handler_t hi_irq_handler_table[C4000_NUM_SLOTS + 1];
extern hi_irq_handler_t *hi_irq_dispatch_table[(1<<C4000_NUM_SLOTS)];

/*
 * Prototypes
 */

extern void nim_add_chassis_mib_cards(void);
extern void nim_subsys_init_class(ushort class);
extern ushort nim_get_type(int slot);
extern ushort nim_get_version(int slot);
extern boolean nim_exists(int slot);
extern void nim_get_slot_hardware_address(hwidbtype *idb, uchar *addr);
extern void nim_init_hi_irq_handler(void);
extern void nim_register_hi_irq_handler(ushort slot, void *func,
					int type, ushort arg);
