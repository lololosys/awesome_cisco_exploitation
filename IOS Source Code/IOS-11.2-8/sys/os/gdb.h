/* $Id: gdb.h,v 3.5.4.3 1996/05/10 23:53:06 gstovall Exp $
 * $Source: /release/112/cvs/Xsys/os/gdb.h,v $
 *------------------------------------------------------------------
 * gdb.h -- Definitions used in GDB
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: gdb.h,v $
 * Revision 3.5.4.3  1996/05/10  23:53:06  gstovall
 * CSCdi56792:  Need to remove unused files
 * Branch: California_branch
 * Nuke support for the Irix image, also stamp out the last of the CS500.
 *
 * Revision 3.5.4.2  1996/03/28  02:18:39  hampton
 * Remove the unnecessary use of a poisoned symbols.  [CSCdi52864]
 * Branch: California_branch
 *
 * Revision 3.5.4.1  1996/03/18  21:29:48  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.20.2  1996/03/09  05:09:30  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.3.20.1  1996/02/20  16:39:18  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/13  20:49:42  hampton
 * Clean up includes of gdb.h and references to breakpoint.  [CSCdi48844]
 *
 * Revision 3.3  1995/11/30  00:02:38  hampton
 * Print a stack traceback before attempting to dump core.  [CSCdi44670]
 *
 * Revision 3.2  1995/11/17  18:47:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:45:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:55:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __GDB_H__
#define __GDB_H__

#if defined(__mips)
#include "../src-4k/gdb_4k.h"	/* R4k */
#endif

#if defined(UNIX)
#include "../unix/gdb_sp.h"	/* SPARC */
#endif

#if defined(mc68000)
#include "../src-68/gdb_68.h"	/* M68K */
#endif

#include "../os/signal.h"

/* Define the size of the buffers used for communications with the remote
 * GDB. This value must match the value used by GDB, or the protocol will
 * break.
 */

#define BUFMAX 400

struct debug_context_ {
    int (*getchar)(void);		/* Get character routine */
    void (*putchar)(uchar);		/* Put character routine */
    void (*flush)(void);		/* Flush output */
    tt_soc *tty;			/* TTY pointer of debugging process */
    char inbuf[BUFMAX];			/* Current input command */
    char outbuf[BUFMAX];		/* Current output command */
    char scratchbuf[BUFMAX];		/* Scratch buffer for compression */
    cpu_context_t cpu_ctx;		/* Saved cpu context */
    int savelevel;			/* Saved interrupt level of GDB */
    SIG_PF handler;			/* Saved bus error handler */
    sprocess *fork;			/* Pointer to process, if any */
    int signal;				/* Signal number of exception */
};

/*
 * Data structures
 */
extern debug_context gdb_context;
extern boolean gdb_debug;
extern boolean gdb_reqreload;
extern jmp_buf gdb_env;

/*
 * Define the crash structures used in core files processed by gdb
 */
#define CRASH_MAGIC 0xdead1234
#define CRASH_VERSION 5

typedef enum {
    CRASH_REASON_NOTCRASHED,
    CRASH_REASON_EXCEPTION,
    CRASH_REASON_CORRUPT
} crashreason;

typedef struct {
    int magic;			/* Magic number */
    int version;		/* Version number */
    crashreason reason;		/* Crash reason */
    int cpu_vector;		/* CPU vector for exceptions */
    void *old_context;		/* Old context - for backwards compatibility */
    void *rambase;		/* Start of RAM on this platform */
    void *textbase;             /* Base address of .text section */
    void *database;             /* Base address of .data section */
    void *bssbase;              /* Base address of .bss section */
    cpu_context_t *context;	/* CPU context at crash time */
} crashinfo;


/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

/*
 * gdbcore.c
 */

extern boolean crashBLOCK(debug_context *);
extern void debug_putchar(uchar);
extern boolean gdb_kill_debug(sprocess *);
extern boolean gdbioBLOCK(tt_soc *);
extern int debug_getchar(void);
extern int gdb_printf(const char *, ...);
extern void debug_flush(void);
extern void debug_process(int);
extern void debug_suspend(void);
extern void examine_process(int);
extern void gdb_exception(int);
extern void gdb_flush(void);
extern void gdb_init_core(void);
extern void gdb_kernel_enable(void);
extern void gdb_shutdown(tt_soc *);

/*
 * gdbproto.c
 */

extern boolean gdb_interface(debug_context *, boolean);
extern boolean gethexnum(char *, char **, int *);
extern boolean getpacket(debug_context *);
extern boolean parse2hexnum(char *, int *, int *);
extern boolean parsehexnum(char *, int *);
extern char *hex2mem(char *, char *, int);
extern char *mem2hex(char *, char *, int);
extern void putpacket(debug_context *);
extern void gdb_set_reloc_sem(void);
extern boolean gdb_test_reloc_sem(void);
extern void gdb_clear_reloc_sem(void);

/*
 * remcompress.c
 */
extern void gdb_compress(char *, char *);
extern void gdb_expand(char *, char *);

/*
 * processor dependant routines
 */
extern void cache_flush(void);
extern void exception_save_context(cpu_context_t *);
extern void exception_restore_context(cpu_context_t *);
extern void gdb_mark_frame_unused(debug_context *);
extern boolean gdb_frame_unused(debug_context *);
extern void copy_fork_debug(debug_context *, sprocess *);
extern void gdb_proc_cause_exception(sprocess *);
extern int computeSignal(int);
extern void gdb_show_exception_info(debug_context *);
extern void gdb_get_cpuregs(debug_context *);
extern void gdb_set_cpuregs(debug_context *);
extern void gdb_read_membytes(debug_context *);
extern void gdb_write_membytes(debug_context *);
extern void gdb_proc_continue(debug_context *, int);
extern void restore_vectors(void);
extern void trap_bus_errors(void);
extern boolean exception_interrupt(void);
extern void *exception_fault_addr(void);
extern char *exception_save_traceback(void);

#endif /* __GDB_H___ */
