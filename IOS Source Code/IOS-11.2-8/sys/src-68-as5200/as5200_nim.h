/* $Id: as5200_nim.h,v 1.1.62.1 1996/06/16 21:19:33 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/src-68-as5200/as5200_nim.h,v $
 *------------------------------------------------------------------
 * as5200_nim.h - Include for for NIM handler routines
 *
 * December, 1995 - Jonathan Chiung-Hsi Lin
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: as5200_nim.h,v $
 * Revision 1.1.62.1  1996/06/16  21:19:33  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 1.1  1995/12/20  00:08:10  jchlin
 * Placeholders for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

enum { CSM_INIT_INTERFACE, CSM_ADD_DIGIT_COLLECTOR };

#define HIRQ_INTLEVEL 4
#define MAX_LEVEL4_ISR_PER_SLOT 3

#define AS5200_NIM_HI_IRQ_PRIOR_M32      3 /* M32 has highest IRQ4 priority */
#define AS5200_NIM_HI_IRQ_PRIOR_FRAMER   2 /* Framer has next highest IRQ4 priority */
#define AS5200_NIM_HI_IRQ_PRIOR_LOW      1 /* All other NIMs have low IRQ4 priority */
#define AS5200_NIM_HI_IRQ_PRIOR_ZERO     0 /* IRQ4 priority of unused slots */
 
typedef enum nim_hi_irq_type_t_ {
    NIM_HI_IRQ_T1,
    NIM_HI_IRQ_E1,
    NIM_HI_IRQ_M32,
    NIM_HI_IRQ_CSM_MODEM,
    NIM_HI_IRQ_COMPRESSOR
} nim_hi_irq_type_t;

typedef struct {
    void (* func)(ushort);
    ushort arg;
    ushort prior;
} isr_handler_t;

typedef struct hi_irq_handler_struc {
    ushort mask;
    isr_handler_t handler_list[MAX_LEVEL4_ISR_PER_SLOT+1];
} hi_irq_handler_t;
 
/*
 * Externs
 */
 
extern hi_irq_handler_t hi_irq_handler_table[BRASIL_MAX_SLOTS+1];
extern hi_irq_handler_t *hi_irq_dispatch_table[(1<<BRASIL_MAX_SLOTS)];
 
/*
 * Prototypes
 */

extern void nim_subsys_init_class(ushort class);
extern ushort nim_get_type(int slot);
extern ushort nim_get_version(int slot);
extern boolean nim_exists(int slot);
extern void nim_init_hi_irq_handler(void);
extern void nim_register_hi_irq_handler(ushort slot, void *func,
                                        nim_hi_irq_type_t type, ushort arg); 
extern void hi_irq_dispatcher (register hwidbtype *idb);
extern uint get_slot_number(void);
