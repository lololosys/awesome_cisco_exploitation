/* $Id: rsp_cbus_monitor.c,v 3.5.10.1 1996/05/21 10:04:29 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_cbus_monitor.c,v $
 *------------------------------------------------------------------
 * CBus monitor module
 *
 * November 1995, Yan Ke 
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_cbus_monitor.c,v $
 * Revision 3.5.10.1  1996/05/21  10:04:29  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:38:27  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/01/29  07:30:06  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1996/01/25  11:22:12  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.2  1995/12/17  18:38:16  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.1  1995/11/27  21:18:27  yanke
 * Initial commit of rsp cbus protection mechanism
 *
 * CSCdi44478:  slave rsp crashes or resets during ips eoir events
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "stacks.h"
#include "logger.h"
#include "packet.h"
#include "interface.h"
#include "sys_registry.h"
#include "subsys.h"
#include "../if/network.h"
#include "exception.h"
#include "../os/gdb.h"

#include "../hes/if_fci.h"
#include "../hes/ccb_cmd.h"
#include "../if/if_controller.h"
#include "../hes/hes_controller.h"
#include "../hes/ucode.h"      
#include "../dev/flash_dvr_spec.h"
#include "../hes/slot.h"
#include "../hes/cbus_registry.regh"
#include "../hes/cbus_registry.regc"
#include "rsp_init.h"
#include "rsp_pcmap.h"
#include "rsp_if.h"

static int ss_err_flag = 0;
static int ss_err_cbus = 0;
static watched_boolean *ss_monitor_flag;
static uint *ss_memd_loc;
static pid_t cbus_monitor_pid;
static SIG_PF ss_sigbus_save;

extern boolean cbus_monitor_debug;

static void proc_bus_event (void)
{
    ss_err_flag++;
    if (ss_err_cbus == 0) {
	/*
	 * First time handler is invoked, we need
	 * to wake up the process to start banging
	 * on the cbus.
	 */
	process_set_boolean(ss_monitor_flag, TRUE);
    }
    /*
     * Make bus unavailable to applications.
     */
    ss_err_cbus = 1;
}

/*
 * We are invoked because of expected bus error/timeout most likely caused
 * by the CyBusses being stalled or having grants disabled.
 * All we need to do is to wake up our check process.
 */
#define CBUS_QA_TIMEOUT (0)   /* Value returned on a CBUS QA read error. */
#define CBUS_DFLT_TIMEOUT (0) /* Default value returned on a CBUS read
                                 error. */

static int srsp_handle_buserrors (int sig, int code, r4k_context_t *sc)
{
    int cause;
    ulong addr;
    int reg;

    cause = (sc->cause & CAUSE_MASK) >> CAUSE_SHIFT;
    addr = (ulong)r4k_get_cpu_addr(sc);
    if (cause == CAUSE_BUS_DATA) {
        /*
         * Check that the access was to the Master RSP's MEMD.
         *
         * XXXX-MKC: We may want to be more inteligent and actually use
         * the range registers to distinguish the portions of the regions
         * maintained on the  Master RSP versus those on the Slave RSP.
         * Also, we may want to return different values for these areas.
         */
        if (((addr >= ADRSPC_MEMD) &&
             (addr < (ADRSPC_MEMD+memd_size))) ||
            ((addr >= ADRSPC_MEMD_QAREGS) && 
             (addr < (ADRSPC_MEMD_QAREGS+memd_size))) ||
            ((addr >= ADRSPC_MEMD_WRITETHRU) && 
             (addr < (ADRSPC_MEMD_WRITETHRU+memd_size))) ||
            ((addr >= ADRSPC_MEMD_UNCACHED) && 
             (addr < (ADRSPC_MEMD_UNCACHED+memd_size)))) {
  
            /* Get register involved in read instruction. */
            reg = check_null_read(sc);
  
            if (cbus_monitor_debug)
                buginf("\nCBus monitor bus error handler, flag = %d,"
                       " addr = 0x%08X", ss_err_flag, addr);
            proc_bus_event();
            
            if (reg > 0)
                r4k_set_cpu_reg(sc, reg, CBUS_DFLT_TIMEOUT, FALSE);
  
            /* Move pc past instruction that caused exception. */
            if (!branch_emulate(sc)) {
                /* We've successfully incremented pc. */
                return(0);
            }
        }
    }
      
    return (*ss_sigbus_save)(sig, code, sc);
}

void cbus_timeout_handler (void)
{
    if (cbus_monitor_debug)
	buginf("\nCBus monitor bus timeout handler, flag = %d", ss_err_flag);
    proc_bus_event();
}

/*
 * This routine install our own handler to catch
 * possible cbus error/timeout for our own processing.
 */
static void install_bus_handler (void)
{
    leveltype level;

    level = raise_interrupt_level(ALL_DISABLE);
    /*
     * Install our handler and keep the original handler for use in
     * handling the errors we are not interested in.
     */
    ss_sigbus_save = signal(SIGBUS, (SIG_PF)srsp_handle_buserrors);

    reset_interrupt_level(level);
}

/*
 * Keep bang on the cbus periodically until it's 
 * becomes available.
 */
static void cbus_sniff (void)
{
    if (cbus_monitor_debug)
	buginf("\nMonitoring CBus availability ...");
    process_suspend();
    for (;;) {
	if (touch_device((void *)(ss_memd_loc + 1)) == TRUE) {
	    if (cbus_monitor_debug)
		buginf("\nCBus is now available!");
	    return;
	}
	if (cbus_monitor_debug)
	    buginf("\nCBus is NOT available!");
	process_sleep_for(ONESEC);
    }
}

/*
 * This process is activated by our bus error handler.
 * Once activated, it will sniff the cbus using 'touch device'
 * from time to time to see if the bus is available for access.
 * Once that happens, it will notify all registered modules
 * by a list registry function.
 */
static forktype cbus_monitor (void)
{
    ulong major, minor;

    install_bus_handler();

    process_watch_boolean(ss_monitor_flag, ENABLE, RECURRING);
    for (;;) {
	process_wait_for_event();
	process_set_boolean(ss_monitor_flag, FALSE);
	if (cbus_monitor_debug)
	    buginf("\nMonitor process wake up");

	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
		case BOOLEAN_EVENT:
		    reg_invoke_cbus_unavailable_notify();
		    cbus_sniff();
		    ss_err_flag = 0;
		    reg_invoke_cbus_available_notify();
		    ss_err_cbus = 0;
		    break;

		default:
		    break;
	    } /* switch */
	} /* while */
    } /* for */
}

/*
 * Basic light-weight check for use after a read.
 */
static inline boolean rsp_cbus_check_read_inline (void)
{
    if (ss_err_flag)
	return FALSE;
    else
	return TRUE;
}

/*
 * Invoked from application to check current cbus
 * status of the last read performed on the cbus.
 * This is a light-weight check to be used after a read.
 */
boolean rsp_cbus_check_read (void)
{
    return(rsp_cbus_check_read_inline());
}

/*
 * Invoked from application to check current cbus status by
 * performing a write and read to the cbus to ensure that any
 * writes in the pipeline have completed.
 */
boolean rsp_cbus_check (void)
{
    volatile uint value;

    *(uint *)ss_memd_loc = 0;
    value = *(uint *)ss_memd_loc;
    return rsp_cbus_check_read();
}

/*
 * Invoked from application to copy data from MEMD and periodically
 * check the current cbus status. If an error is detected, the
 * copy terminates early and returns the appropriate result.
 *
 * The intent of this routine is to optimize the reads from the MEMD
 * since they probably go across the CyBus and take a minimum of ~480ns
 * each on an RSP2.
 *
 * We only allow 4 access between checks because if they were to all
 * timeout, this would mean a freeze of over 1ms. (250us TO each access)
 */
boolean rsp_cbus_monitored_copy_from_memd (void *srcptr, void *dstptr,
                                           int bytes)
{
    uchar *src = (uchar *)srcptr;
    uchar *dst = (uchar *)dstptr;

    /* Handle the high runner case. */
    if (bytes >= 8) {
      int check_count;
      uchar *end;
      ulong data;

      /* Check for unaligned src. */
      if ((uint)src & (8-1)) {
          int adjust;
          
          /*
           * Read 8 bytes of data from unaligned source address
           * into a 64-bit register, and write out 8 bytes of data
           * to a potentially unaligned destination. Some data will
           * be copied twice in overlap.
           */
          asm volatile (".set mips3");
          asm volatile (".set noreorder");
          asm volatile ("ldl %0, 0(%1); ldr %0, 7(%1);"  
                        : "=&r" (data) : "r" (src) );
          asm volatile ("sdl %0, 0(%1); sdr %0, 7(%1);" 
                        : "=&r" (data) : "r" (dst) );
          asm volatile (".set reorder");
          asm volatile (".set mips2");

          if (rsp_cbus_check_read_inline() == FALSE)
              return(FALSE);

          /* Align up to the 8-byte boundary in MEMD. */
          adjust = 8 - ((uint)src & 0x7);
          dst += adjust;
          src += adjust;
      }

      /* Move 8 byte chunks. */
      end = (uchar *)(((uint)srcptr + bytes) & ~(8-1));
      check_count = 4;
      while (src < end) {
          asm volatile (".set mips3");
          asm volatile (".set noreorder");
          asm volatile ("ld %0, 0(%1);"
                        : "=&r" (data) : "r" (src) );
          src += 8; /* Use delay slot. */
          asm volatile ("sdl %0, 0(%1); sdr %0, 7(%1);" 
                        : "=&r" (data) : "r" (dst) );
          asm volatile (".set reorder");
          asm volatile (".set mips2");
          dst += 8;
          if (check_count-- == 0) {
              if (rsp_cbus_check_read_inline() == FALSE)
                  return(FALSE);
              check_count = 4;
          }
      }
      /* Accesses not checked due to exiting the loop are checked below. */

      /* Move any residue. */
      end = (uchar *)srcptr + bytes;
      if (((uint)end & (8-1)) != 0) {
          src = end - 8;
          dst = (uchar *)dstptr + bytes - 8;

          /*
           * Read 8 bytes of data from the unaligned source
           * address into a 64-bit register, and write out 8 bytes of
           * data to a potentially unaligned destination. The residue
           * will actually recopy some data in the overlap.
           */
          asm volatile (".set mips3");
          asm volatile (".set noreorder");
          asm volatile ("ldl %0, 0(%1); ldr %0, 7(%1);"  
                        : "=&r" (data) : "r" (src) );
          asm volatile ("sdl %0, 0(%1); sdr %0, 7(%1);" 
                        : "=&r" (data) : "r" (dst) );
          asm volatile (".set reorder");
          asm volatile (".set mips2");
      }

      /* Ensure that all accesses have been checked. */
      return(rsp_cbus_check_read_inline());
    }

    /* Handle small byte counts, (bytes < 8) */
    for (; bytes >= 4; bytes -= 4) {
      *dst++ = *src++;
      *dst++ = *src++;
      *dst++ = *src++;
      *dst++ = *src++;
      if (rsp_cbus_check_read_inline() == FALSE)
          return(FALSE);
    }
    
    while (bytes--) {
      *dst++ = *src++;
    }
    return(rsp_cbus_check_read_inline());
}

/*
 * Invoked from application to copy data to MEMD and periodically
 * check the current cbus status. If an error is detected, the
 * copy terminates early and returns the appropriate result.
 *
 * The intent of this routine is to optimize the writes to the MEMD
 * since they probably go across the CyBus and take a minimum of ~?ns
 * each on an RSP2. NOTE also that 7-byte, 6-byte, 5-byte, and 3-byte
 * writes cannot be performed on the CyBus.
 *
 * We only allow 4 access between checks because if they were to all
 * timeout, this would mean a freeze of over 1ms. (250us TO each access)
 */
boolean rsp_cbus_monitored_copy_to_memd (void *srcptr, void *dstptr,
                                         int bytes)
{
    uchar *src = (uchar *)srcptr;
    uchar *dst = (uchar *)dstptr;
    volatile uchar value;
          
    /* Handle the high runner case. */
    if (bytes >= 8) {
      int check_count;
      int adjust;
      uchar *end;
      ulong data;

      /* Check for unaligned dst. */
      if ((uint)dst & (8-1)) {
          /* Align up to the 8-byte boundary in MEMD. */
          adjust = 8 - ((uint)dst & 0x7);

          /* Move appropriate sized chunks until we are aligned. */
          if (adjust & 0x1) {
              *dst++ = *src++;
          }
          if (adjust & 0x2) {
              /* src may be unaligned. */
              *(ushort *)dst = GETSHORT(src);
              dst += 2;
              src += 2;
          }
          if (adjust & 0x4) {
              *(ulong *)dst = GETLONG(src);
              dst += 4;
              src += 4;
          }

          /* Force the writes to clear the pipe and use a read as a check. */
          value = *(dst - 1);
          if (rsp_cbus_check_read_inline() == FALSE)
              return(FALSE);
      }

      /* Move 8 byte chunks. */
      end = (uchar *)(((uint)dstptr + bytes) & ~(8-1));
      check_count = 4;
      while (dst < end) {
          asm volatile (".set mips3");
          asm volatile (".set noreorder");
          asm volatile ("ldl %0, 0(%1); ldr %0, 7(%1);"  
                        : "=&r" (data) : "r" (src) );
          src += 8; /* Use delay slot. */
          asm volatile ("sd %0, 0(%1);" 
                        : "=&r" (data) : "r" (dst) );
          asm volatile (".set reorder");
          asm volatile (".set mips2");
          dst += 8;
          if (check_count-- == 0) {
              value = *(dst - 1);
              if (rsp_cbus_check_read_inline() == FALSE)
                  return(FALSE);
              check_count = 4;
          }
      }
      /* Accesses not checked due to exiting the loop are checked below. */

      /* Move any residue. */
      adjust = ((int)dstptr + bytes) & (8-1);
      if (adjust != 0) {
          /* Move appropriate sized chunks to cover residual. */
          if (adjust & 0x4) {
              *(ulong *)dst = GETLONG(src);
              dst += 4;
              src += 4;
          }
          if (adjust & 0x2) {
              /* src may be unaligned. */
              *(ushort *)dst = GETSHORT(src);
              dst += 2;
              src += 2;
          }
          if (adjust & 0x1) {
              *dst++ = *src;
          }
      }

      /*
       * Ensure that all the accesses have been checked.
       * Force the writes to clear the pipe and use a read as a check.
       */
      value = *(dst - 1);
      return(rsp_cbus_check_read_inline());
    }

    /* Handle small byte counts, (bytes < 8) */
    for (; bytes >= 4; bytes -= 4) {
      *dst++ = *src++;
      *dst++ = *src++;
      *dst++ = *src++;
      *dst = *src++;

      /* Force the writes to clear the pipe and use a read as a check. */
      value = *dst;
      if (rsp_cbus_check_read_inline() == FALSE)
          return(FALSE);
      dst++;
    }

    while (bytes--) {
      *dst++ = *src++;
    }

    /* Force the writes to clear the pipe and use a read as a check. */
    value = *(dst - 1);
    return(rsp_cbus_check_read_inline());
}

#define STALL_DEBOUNCE	50		/* milliseconds */

/*
 * Slave stall interrupt handler.
 */
static void srsp_oir_interrupt (void)
{
    volatile uchar *cbus0reg, *cbus1reg;
    sys_timestamp stall_debounce;
    uchar cbus0val, cbus1val;

    cbus0reg = (uchar*) ADRSPC_CBUS0REG;
    cbus1reg = (uchar*) ADRSPC_CBUS1REG;

    TIMER_START(stall_debounce, STALL_DEBOUNCE);

    if (cbus_monitor_debug)
	buginf("\nCBus stall detected");
    for (;;) {
	/* 
	 * Check both Latch and Unlatch bits to determine if stall
	 * is debounced. Checking both bits is for robustness.
	 */
        cbus0val = *cbus0reg;
        cbus1val = *cbus1reg;
        if ((cbus0val & (CBUSX_UNLATCHED_STALL | CBUSX_LATCHED_STALL)) ||
            (cbus1val & (CBUSX_UNLATCHED_STALL | CBUSX_LATCHED_STALL))) {
            *cbus0reg = (cbus0val & ~CBUSX_LATCHED_STALL) | CBUSX_STALL_UPDATE;
            *cbus1reg = (cbus1val & ~CBUSX_LATCHED_STALL) | CBUSX_STALL_UPDATE;
	    TIMER_START(stall_debounce, STALL_DEBOUNCE);
	    continue;
	}
	if (AWAKE(stall_debounce)) {
	    break;
	}
    }
    *cbus0reg = (*cbus0reg & ~CBUSX_LATCHED_STALL) | CBUSX_STALL_UPDATE;
    *cbus1reg = (*cbus1reg & ~CBUSX_LATCHED_STALL) | CBUSX_STALL_UPDATE;
}

#include "config.h"
#include "parser.h"
#include "../ui/debug.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../hes/cbus_debug.h"

#define ALTERNATE	NONE
#include "../hes/exec_debug_cbus.h"
LINK_POINT(cbus_debug_commands, ALTERNATE);
#undef  ALTERNATE

static parser_extension_request cbus_debug_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(cbus_debug_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * Initialization routine to setup our
 * cbus protection.
 */
void rsp_init_cbus_monitor (void)
{
    int slotnum;

    slotnum = get_rsp_self_slot();
    ss_memd_loc = (uint *)&ccb_base[slotnum];

    createlevel(LEVEL_STALL, srsp_oir_interrupt, "Slave OIR interrupt");

    ss_monitor_flag = create_watched_boolean("Slave Cbus monitor", 0);
    cbus_monitor_pid = process_create(cbus_monitor,
	"cbus monitor", LARGE_STACK, PRIO_LOW);

    if (rsp_is_master() == FALSE)
	parser_add_command_list(cbus_debug_chain_init_table, "cbus");
}
