/* $Id: svip_moncmds.c,v 1.1.6.1 1996/07/08 22:43:10 mwu Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-4k-svip/svip_moncmds.c,v $
 *------------------------------------------------------------------
 * svip_moncmds.c 
 *
 * Mar, 96. Paul Greenfield
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: svip_moncmds.c,v $
 * Revision 1.1.6.1  1996/07/08  22:43:10  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 1.1  1996/03/23  01:23:03  pgreenfi
 * Placeholder files for src-4k-svip.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "setjmp.h"
#include "monitor.h"
#include "mon_boot.h"

extern int addrloop(), setalias(), alt_mem(), berrscan(), mon_boot(), break_point(), 
    call(), cat(), memory_checksum(), clrerrlog(), cmp_mem(), confreg(), sys_cont(), 
    show_context(), toss_cookie(), cpu_config(), hw_cycles(), devs(), directory(), 
    disasm(), dis_mem(), echo(), dumperrlog(), cat(), dump_ideeprom(),
    fil_mem(), show_frame(), help(), history(), fil_mem(), initfs(), 
    jump(), sys_launch(), testleds(), memdebug(), meminfo(), memloop(), memtest(), 
    mov_mem(), paritytest(), repeat(), reset(), setalias(), sleep_cmd(), 
    speed_lp(), show_stack(), sync(), sysretdump(), tcal(), tlbdump(), tlbflush(), 
    tlbmap(), tlbpid(), tlbphy(), tlbtest(), tlbscan(), tlbvir(), scopetimer(), 
    unset(), unset(), test_watchdog(), priv();

struct monitem moncmd[] = {  /* in alphabetical order */
  "addrloop",addrloop,"walk 1 thru range of addresses",MF_PRIVILEGED,
  "alias",setalias,"set and display aliases command",0,
  "alter",alt_mem,"alter locations in memory",MF_PRIVILEGED,
  "berrscan",berrscan,"scan range of addresses for bus errors",MF_PRIVILEGED,
  MON_BOOT_CMD,mon_boot,"boot up an external process",0,
  "break", break_point, "set/show/clear the breakpoint",0,
  "call",call,"call a subroutine at address with converted hex args",MF_PRIVILEGED,
  "cat",cat,"concatenate files",MF_PRIVILEGED,
  "checksum",memory_checksum,"checksum a block of memory",MF_PRIVILEGED,
  "clrerr",clrerrlog,"clear the error log",MF_PRIVILEGED,
  "compare",cmp_mem,"compare two blocks of memory",MF_PRIVILEGED,
  "confreg",confreg,"configuration register utility",0,
  "cont", sys_cont, "continue executing a downloaded image",0,
  "context", show_context, "display the context of a loaded image",0,
  "cpu", cpu_config, "cpu / system information and control",MF_PRIVILEGED,
  "dev",devs,"list the device table",0,
  "dir",directory,"list files in file system",0,
  "dis", disasm, "disassemble instruction stream",0,
  "dump",dis_mem,"display a block of memory",MF_PRIVILEGED,
  "echo",echo,"monitor echo command",MF_PRIVILEGED,
  "errlog",dumperrlog,"display the error log",MF_PRIVILEGED,
  "fdump",cat,"file dump utility",MF_PRIVILEGED,
  "fill",fil_mem,"fill a block of memory",MF_PRIVILEGED,
  "frame", show_frame, "print out a selected stack frame",0,
  "help",help,"monitor builtin command help",0,
  "history",history,"monitor command history",0,
  "ifill",fil_mem,"fill a block of memory w/incrementing pattern",MF_PRIVILEGED,
  "initfs",initfs,"re-initialize the file system access structures",MF_PRIVILEGED,
  "jump",jump,"call a subroutine at address with argc/argv",MF_PRIVILEGED,
  "launch", sys_launch, "launch a downloaded image",MF_PRIVILEGED,
  "leds",testleds,"check out the error LED",MF_PRIVILEGED,
  "memdebug",memdebug,"write/read/verify scope loop",MF_PRIVILEGED,
  "meminfo",meminfo,"main memory information",0, 
  "memloop",memloop,"write or read scope loop",MF_PRIVILEGED,
  "memtest",memtest,"simple memory test",MF_PRIVILEGED,
  "move",mov_mem,"move a block of memory",MF_PRIVILEGED,
  "repeat",repeat,"repeat a monitor command",0,
  "reset",reset,"system reset",0,
  "set",setalias,"set a monitor variable",0,
  "speed", speed_lp, "timed performance loop",MF_PRIVILEGED,
  "stack", show_stack, "produce a stack trace",0,
  "sync",sync,"write monitor environment to NVRAM",0,
  "sysret", sysretdump, "print out info from last system return",0,
  "tlbdump", tlbdump, "display the cpu TLB",MF_PRIVILEGED,
  "tlbflush", tlbflush, "flush the TLB",MF_PRIVILEGED,
  "tlbmap", tlbmap, "initialize a TLB mapping",MF_PRIVILEGED,
  "tlbpid", tlbpid, "set/display process ID number",MF_PRIVILEGED,
  "tlbphy", tlbphy, "search TLB for physical translation",MF_PRIVILEGED,
  "tlbtest", tlbtest, "test the TLB",MF_PRIVILEGED,
  "tlbscan", tlbscan, "scan for TLB exceptions",MF_PRIVILEGED,
  "tlbvir", tlbvir, "search TLB for a virtual translation",MF_PRIVILEGED,
  "unalias",unset,"unset an alias",0,
  "unset",unset,"unset a monitor variable",0,
  "priv",priv,"enter the privileged command state",MF_HIDDEN,
};
#define MONCMDSIZ (sizeof(moncmd)/sizeof(struct monitem))

int moncmdsiz = MONCMDSIZ;
