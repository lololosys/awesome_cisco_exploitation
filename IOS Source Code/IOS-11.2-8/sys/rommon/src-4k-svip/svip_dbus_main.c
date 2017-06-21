/* $Id: svip_dbus_main.c,v 1.1.6.1 1996/07/08 22:43:04 mwu Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-4k-svip/svip_dbus_main.c,v $
 *------------------------------------------------------------------
 * svip_dbus_main.c 
 *
 * Mar, 96. Paul Greenfield
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: svip_dbus_main.c,v $
 * Revision 1.1.6.1  1996/07/08  22:43:04  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 1.1  1996/03/23  01:22:58  pgreenfi
 * Placeholder files for src-4k-svip.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "cpu_mips.h"
#include "dbus.h"
#include "dbstatus.h"
#include "queue.h"

typedef struct vector_table_ {
    uchar vect_utlb[0x80];
    uchar vect_xtlb[0x80];
    uchar vect_cerr[0x80];
    uchar vect_gexp[0x80];
}vector_table;

dbuscore_t dbuscore;
dbuscore_t *dbuscore_p;
vc_t       vc_intf;
unsigned long readint;

/* table of values to write to CY ASIC for initialization */
typedef struct {
    ulong address;
    ulong value;
} cy_init_s;

cy_init_s cy_init_table[] = {
#if 0
    0xb0044104, 1,		/* reset chip */
    0xB004422C, 0xFFFFFFFF,	/* error interrupt clear register */
#endif
    0xB0044254, 0xDFFFDDDD,	/* enable CYA interrupt */
    0xB004424C, 1,		/* other interrupt clear reg */
};


main() {
    int nn;
    unsigned long dummy;	
    unsigned short dummyshort;

#ifdef NO_POLL
    unsigned long sreg = getcp0_sreg();
#endif
    /*
     ** hookExceptions -- point to exception vector in KSEG0 memory
     */
    
    extern uchar vec_start[];
    
#if 0
    /* init CYA */
    for(nn = 0; nn < (sizeof(cy_init_table)/sizeof(cy_init_table[0])); nn++)
	{
	*((unsigned long *)cy_init_table[nn].address) = cy_init_table[nn].value;
	/* need a little delay for reset, so we just do a read  */
	dummy = *((volatile unsigned long *)0xB0044244);
	}
#endif

    dbuscore_p = &dbuscore;
    dbus_early_init();
    *((ushort *)&dbuscore_p->db.data_port) = *((ushort *)&db_hw->data_port);

	
	
#if 1
    *((volatile unsigned long *)0xB0044254) = 0xDFFFDDDD; /* enable CYA interrupt */
    *((volatile unsigned long *)0xB004424C) = 0x1;        
#endif

#ifdef NO_POLL
    sreg |= (SREG_IMASK6|SREG_IE);
    sreg &= ~SREG_BEV;
    setcp0_sreg(sreg);
#endif

    while (1) {                              /* wait for dbus download */
	if ((KSEG1_IO_ASIC->nev_isr & NEV_ISR_DBUS_INTR) != 0)
	    dbus_handler();
    }

#ifdef NO_POLL
    *(vector_table *)0x80000000 = *(vector_table *)vec_start;
    cache_flush(0, -1);
    setcp0_sreg( (getcp0_sreg() & ~SREG_BEV) );

    read_status();
    vc_intf.getc = console_getc;
    vc_intf.putc = console_putc;
    vc_intf.flush = dbus_init;
    exit(&vc_intf);
#endif /* SIM */

}

general_exception()
{
}

