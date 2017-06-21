/* $Id: pred_moncmds.c,v 3.1.68.1 1996/03/21 23:31:40 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k-c7100/pred_moncmds.c,v $
 *------------------------------------------------------------------
 * pred_moncmds.c -- Predator ROM monitor commands
 *
 * November 1995, Steve Zhang
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pred_moncmds.c,v $
 * Revision 3.1.68.1  1996/03/21  23:31:40  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.4  1996/02/21  22:30:25  szhang
 * CSCdi49258:  Put back deleted rom monitor commands
 * Branch: ELC_branch
 *
 * Revision 3.1.2.3  1996/01/17  05:54:50  szhang
 * Branch: ELC_branch
 * Fixed the problem with chip select to mid-plane eeprom.
 * NVRAM size is also set in the FPGA I/O control register.
 *
 * Revision 3.1.2.2  1995/12/01  04:23:54  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.1.2.1  1995/11/23  19:14:45  szhang
 * Branch: ELC_branch
 * Initial source code commit to ELC branch for Predator (C7100) ROM
 * monitor.
 *
 * Revision 3.1  1995/11/23  00:55:11  mbeesley
 * Add placeholders for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "mon_boot.h"

extern int addrloop(), setalias(), alt_mem(), berrscan(), 
    mon_boot(), break_point(), 
    call(), cat(), memory_checksum(), clrerrlog(), cmp_mem(), confreg(), 
    sys_cont(), show_context(), toss_cookie(), cpu_config(), hw_cycles(), 
    devs(), directory(), disasm(), dis_mem(), echo(), dumperrlog(), cat(), 
    fil_mem(), show_frame(), help(), history(), initfs(), 
    jump(), sys_launch(), testleds(), memdebug(), meminfo(), memloop(), 
    memtest(), mov_mem(), paritytest(), repeat(), reset(), setalias(), 
    sleep_cmd(), speed_lp(), show_stack(), sync(), sysretdump(), 
    tlbdump(), tlbflush(), tlbmap(), tlbpid(), tlbphy(), tlbtest(), 
    tlbscan(), tlbvir(), scopetimer(), unset(), unset(), test_watchdog(), 
    priv(), rw_c7100_mpeeprom(), rw_c7100_cpueeprom();

struct monitem moncmd[] = {  /* in alphabetical order */

  { "addrloop", addrloop,
    "walk 1 thru range of addresses", MF_PRIVILEGED },

  { "alias", setalias,
    "set and display aliases command", 0 },

  { "alter", alt_mem,
    "alter locations in memory", MF_PRIVILEGED },

  { MON_BOOT_CMD, mon_boot,
    "boot up an external process", 0 },

  { "berrscan", berrscan,
    "scan range of addresses for bus errors", MF_PRIVILEGED },
 
  { "break", break_point, 
    "set/show/clear the breakpoint", 0 },
  
  { "call", call,
    "call a subroutine at address with converted hex args", MF_PRIVILEGED },

  { "cat", cat,
    "concatenate files", MF_PRIVILEGED },

  { "checksum", memory_checksum,
    "checksum a block of memory", MF_PRIVILEGED },

  { "compare", cmp_mem,
    "compare two blocks of memory", MF_PRIVILEGED },

  { "confreg", confreg,
    "configuration register utility", 0 },
  
  { "cont", sys_cont, 
    "continue executing a downloaded image", 0 },
  
  { "context", show_context, 
    "display the context of a loaded image", 0 },
  
  { "cpu", cpu_config, 
    "cpu / system information and control", MF_PRIVILEGED },

  { "cpueeprom", rw_c7100_cpueeprom,
    "read or write cpu assembly eeprom", MF_PRIVILEGED },
  
  { "dev", devs,
    "list the device table", 0 },
  
  { "dir", directory,
    "list files in file system", 0} ,
  
  { "dis", disasm, 
    "disassemble instruction stream", 0},
  
  { "dump", dis_mem,
    "display a block of memory", MF_PRIVILEGED },

  { "echo", echo,
    "monitor echo command", MF_PRIVILEGED },

  { "fdump", cat,
    "file dump utility", MF_PRIVILEGED },

  { "fill", fil_mem,
    "fill a block of memory", MF_PRIVILEGED },

  { "frame", show_frame, 
    "print out a selected stack frame", 0 },

  { "help", help,
    "monitor builtin command help", 0 },

  { "history", history,
    "monitor command history", 0 },

  { "ifill", fil_mem,
    "fill a block of memory w/incrementing pattern", MF_PRIVILEGED },

  { "initfs", initfs,
    "re-initialize the file system access structures", MF_PRIVILEGED },

  { "jump", jump,
    "call a subroutine at address with argc/argv", MF_PRIVILEGED },

  { "launch", sys_launch, 
    "launch a downloaded image", MF_PRIVILEGED },

  { "leds", testleds,
    "check out the error LED", MF_PRIVILEGED },

  { "memdebug", memdebug,
    "write/read/verify scope loop", MF_PRIVILEGED },

  { "meminfo", meminfo,
    "main memory information", 0 }, 

  { "memloop", memloop,
    "write or read scope loop", MF_PRIVILEGED },

  { "memtest", memtest,
    "simple memory test", MF_PRIVILEGED },

  { "move", mov_mem,
    "move a block of memory", MF_PRIVILEGED },

  { "mpeeprom", rw_c7100_mpeeprom, 
    "read or write midplane eeprom", MF_PRIVILEGED },

  { "repeat", repeat,
    "repeat a monitor command", 0 },

  { "reset", reset,
    "system reset", 0 },

  { "set", setalias,
    "show all monitor variables", 0 },

  { "stack", show_stack, 
    "produce a stack trace", 0 },

  { "sync", sync,
    "write monitor environment to NVRAM", 0 },

  { "sysret", sysretdump, 
    "print out info from last system return", 0 },

  { "tlbdump", tlbdump, 
    "display the cpu TLB", MF_PRIVILEGED },

  { "tlbflush", tlbflush, 
    "flush the TLB", MF_PRIVILEGED },

  { "tlbmap", tlbmap, 
    "initialize a TLB mapping", MF_PRIVILEGED },

  { "tlbpid", tlbpid, 
    "set/display process ID number", MF_PRIVILEGED },

  { "tlbphy", tlbphy, 
    "search TLB for physical translation", MF_PRIVILEGED },

  { "tlbtest", tlbtest, 
    "test the TLB", MF_PRIVILEGED },

  { "tlbscan", tlbscan, 
    "scan for TLB exceptions", MF_PRIVILEGED },

  { "tlbvir", tlbvir, 
    "search TLB for a virtual translation", MF_PRIVILEGED },

  { "tscope", scopetimer,
    "timer scope loop", MF_PRIVILEGED },
 
  { "unalias", unset,
    "unset an alias", 0 },

  { "unset", unset,
    "unset a monitor variable", 0 },
 
  { "watchdog", test_watchdog, 
    "test watchdog rebooting of the box", MF_PRIVILEGED },

  { "priv", priv,
    "enter the privileged command state", MF_HIDDEN }

};
#define MONCMDSIZ (sizeof(moncmd)/sizeof(struct monitem))

int moncmdsiz = MONCMDSIZ;




