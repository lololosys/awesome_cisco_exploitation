/* $Id: gdb_4k.h,v 3.1.6.4 1996/07/09 21:15:07 getchell Exp $
 * $Source: /release/112/cvs/Xsys/src-4k/gdb_4k.h,v $
 *------------------------------------------------------------------
 * gdb_4k.h -- R4k specific gdb header file
 *  
 * October 1993, Michael Beesley
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: gdb_4k.h,v $
 * Revision 3.1.6.4  1996/07/09  21:15:07  getchell
 * Modify read bus exception handler and DCL interrrupt handler to deal
 * with dbus accesses to non-existent card.
 * CSCdi58555:  bus exception caused by dbus access to non-existant card
 * Branch: California_branch
 *
 * Revision 3.1.6.3  1996/03/29  20:04:34  mbeesley
 * CSCdi52977: Some r4k implementations handle parity exceptions
 *             incorrectly. Fix them.
 * Branch: California_branch
 *
 * Revision 3.1.6.2  1996/03/21  23:46:32  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.6.1  1996/03/18  21:57:52  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.8.1  1996/03/18  23:32:54  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.1  1996/03/09  00:55:51  hampton
 * Migrate platform specific files to src-xxx directories.  [CSCdi51128]
 *   Step 1: Create copies of the files in their new location.
 *
 * Revision 3.4  1996/03/06  21:26:14  dlobete
 * CSCdi49840:  Support for 32 MB DRAM required for DFS on RVIP
 * Change name pf pick_dram_page() to pick_tlb_page() for easier
 * synching for ELC.
 * Declare pick_tlb_page() non-static.
 *
 * Revision 3.3  1996/02/18  19:42:11  kao
 * CSCdi45136:  EOIR Problems with VIP card
 *
 * Revision 3.2  1995/11/17  18:47:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:45:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:18:52  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/25  17:45:02  mleelani
 * CSCdi37536:  Fix the C4700 Millisecond Ticks.
 * Add support for the 133.33 mhz pipe.
 *
 * Revision 2.1  1995/06/07  21:55:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../src-4k/signal_4k.h"

/*
 * Number of registers/bytes in the context used internally
 * by our GDB client (bigmips, 32bit)
 */
#define CLIENT_REGBYTES 	320
#define CLIENT_REGS     	(CLIENT_REGBYTES/4)

/*
 * Number of registers/bytes used within the gdb server, 64bit
 */
#define SERVER_REGBYTES		312
#define SERVER_REGS		(SERVER_REGBYTES/4)

/*
 * The number of registers/bytes used for the communication
 * between the client and server
 */
#define COMM_REGBYTES		156
#define COMM_REGS		(COMM_REGBYTES/4)

/* 
 * Define the structure used to save R4k exception context
 */
typedef struct cpu_context_
{
    boolean   	      used;		/* frame used or not */
    r4k_context_t     sc;		/* context for R4k processor */
} cpu_context_t;

/*
 * Define a structure used for communication to a GDB client
 */
typedef struct r4k_comm_context_t_ {
    int regs[32];			/* CPU registers */
    int sr;				/* status register */
    int lo;				/* LO */
    int hi;				/* HI */
    int bad;				/* BadVaddr */
    int cause;				/* Cause */
    int pc;				/* EPC */
    int fp;				/* Psuedo frame pointer */
} r4k_comm_context_t;

/* 
 * Define a macro to cause the system to take a breakpoint (gdb k)
 */
#define BREAKPOINT() 		\
    asm volatile ("		\
	  .set noreorder;	\
	  ori $8,$0,0x100;	\
	  mtc0 $8,$13;		\
	  nop;			\
	  nop;			\
	  .set reorder"		\
	 :			\
	 :			\
	 : "$8");

/*
 * Define the opcode for a breakpoint used in single stepping 'break 0'
 */
#define OPCODE_BREAK		0x0000000D

/*
 * Define the opcode used for signalling to the debugger and core
 * writer that we took a software forced crash 'break 10'
 */
#define OPCODE_RELOAD		0x000A000D

/*
 * Prototypes and externals
 */
extern int 	tlb_entries_used;
extern char	*r4k_reg_name[];

extern void	r4k_fake_parity_error(void);
extern int 	r4k_get_cpu_reg(r4k_context_t *, int);
extern void 	r4k_set_cpu_reg(r4k_context_t *sc, int index, 
				int value, boolean ext);
extern void 	write_berr_interrupt(int, r4k_context_t *);
extern void 	handle_exception(int, int code);
extern void 	r4k_return_to_monitor(int, int, r4k_context_t *);
extern int 	disable_watchdog(void);
extern void 	restart_count_compare(int);
extern boolean 	level2_cache_detect(void);
extern void 	level2_cache_flush(void);
extern int	pick_tlb_page(long, long, long *, long *, long *);
extern void 	platform_r4k_set_clock_params(ulong config_reg);
extern void	fault_history_display(void);
extern void	fault_history_purge(void);
extern void	platform_fault_history(int, int, r4k_context_t *, char *, int);

extern int r4k_get_cpu_reg(r4k_context_t *, int);
extern void handle_exception(int, int code);
extern void r4k_return_to_monitor(int, int, r4k_context_t *);
extern int disable_watchdog(void);
extern void restart_count_compare(int);
extern boolean level2_cache_detect(void);
extern void level2_cache_flush(void);
extern void platform_r4k_set_clock_params(ulong config_reg);
extern int 	branch_emulate(r4k_context_t *sc);
extern union instr_r4k * check_load_inst (r4k_context_t *sc);
extern void*    r4k_get_cpu_addr(r4k_context_t *sc);
extern int      check_null_read(r4k_context_t *sc);
/* End of file */
