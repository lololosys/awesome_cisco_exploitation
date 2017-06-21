/* $Id: asm_4k_c7100.h,v 3.1.66.1 1996/03/21 22:27:09 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/asm-mips/asm_4k_c7100.h,v $
 *------------------------------------------------------------------
 * asm_4k_c7100.h : Assembly definitions for Predator (c7100)
 *
 * Novemeber 1995, Michael Beesley 
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asm_4k_c7100.h,v $
 * Revision 3.1.66.1  1996/03/21  22:27:09  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.6  1996/02/20  00:32:49  mbeesley
 * CSCdi49338:  Predator external intr. controller is static - dont check
 *         o Add support for a static external interrupt controller
 *         o Remove some old dead debug code
 * Branch: ELC_branch
 *
 * Revision 3.1.2.5  1996/02/10  00:25:21  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.1.2.4  1995/12/13  02:16:37  mbeesley
 * Branch: ELC_branch
 * Reenable GT64010 and Packet SRAM error interrupts. Leave OIR
 * interrupts as defaulted off (C7100_OIR_INTERRUPT).
 *
 * Revision 3.1.2.3  1995/12/12  04:32:52  mbeesley
 * Branch: ELC_branch
 * Add C7100_EXTERNAL_INTERRUPT to control usage of the external
 * error interrupt control. Defautl is off for now (waiting hw fix).
 *
 * Revision 3.1.2.2  1995/12/09  20:46:54  mbeesley
 * Branch: ELC_branch
 * Fix interrupt enable register address.
 *
 * Revision 3.1.2.1  1995/11/22  20:25:00  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/11/20  21:36:23  mbeesley
 * Add placeholder files for future development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "target_cisco.h"
#include "../src-4k/signal_4k.h"
#include "../src-4k-c7100/c7100.h"
#include "../src-4k-c7100/c7100_pcmap.h"

/*
 * On Predator, the external interrupt controller is static IE its setting
 * does not change as the interrupt level is raised and lowered. The
 * interrupt level primitives (raise_interrupt_level() and 
 * reset_interrupt_level()) can consequently be more optimal. Tell
 * them to be compiled so.
 */
#define R4K_STATIC_EXTERNAL_INTR_CONTROLLER

/*
 * Define NMI stack, counter and subroutine
 */
#define NMI_STACK	l7stk
#define NMI_COUNTER	l7cnt
#define NMI_SUBR	lev7subr
#define NMI_SREG_MASK	0xffff80fd

/*
 * Define level for handling signals, exceptions etc
 */
#define SIG_LEVEL	LEVEL7

/*
 * Define external interrupt controller cause bit
 */
#define CAUSE_EXT_BIT	CAUSE_IBIT6

/*
 * Some overall system defines
 */
#define MSEC_SETTING	1000			   /* timer setting for 1ms */
#define	REFRFREQUENCY	(REFRESHTIME*MSEC_SETTING) /* Refresh freq. */

/*
 * Define the interrupt control, watchdog register addresses
 */
#define IO_IMASK_REG	(ADRSPC_MP_FPGA+0x7B)	   /* err_int_enbl_lsw */
#define IO_WATCHDOG_REG	(ADRSPC_IO_FPGA+0x34)	   /* watchdog_reset */

/*
 * Level status register masks
 */
#define LEVEL1_SREG_MASK    	0xfffff9fd
#define LEVEL2_SREG_MASK    	0xffffe9fd
#define LEVEL3_SREG_MASK        0xffffe1fd
#define LEVEL4_SREG_MASK    	0xffffc1fd
#define LEVEL5_SREG_MASK    	0xffff80fd

/*
 * Port Adaptor Network IO jumptable defintions
 */
#define PA_WRAPPER_OFS		0
#define PA_BASE_OFS		4
#define PA_SCRATCH0_OFS		8
#define PA_SCRATCH1_OFS		12
#define PA_IDB_OFS	        16
#define PA_SREG_OFS		(PA_IDB_OFS + (4 * MAX_INTERFACES_PER_PA))
#define PA_TABLE_SIZE		(PA_SREG_OFS + (4 * MAX_INTERFACES_PER_PA))

/* End of file */

