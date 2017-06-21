/* $Id: moncmds.c,v 1.1.68.1 1996/06/16 21:18:55 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-68-as5200/moncmds.c,v $
 *------------------------------------------------------------------
 * moncmds.c The monitor command table for Brasil.
 *
 * October 1995, Joe Turner
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: moncmds.c,v $
 * Revision 1.1.68.1  1996/06/16  21:18:55  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 1.1  1995/12/03  21:16:32  jturner
 * Placeholder for Brasil.
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
    disasm(), dnld(), dram_verify(), dis_mem(), echo(), dumperrlog(), cat(), 
    fil_mem(), flash(), help(), history(), fil_mem(), initfs(), 
    jump(), sys_launch(), testleds(), memdebug(), meminfo(), memloop(), memtest(), 
    diag_menu(), mov_mem(), parityset(), repeat(), reset(), setalias(), sleep_cmd(), 
    show_stack(), sync(), sysretdump(), tcal(), scopetimer(), sys_step(),
    unset(), unset(), test_watchdog(), priv(),
    wr_ena_nvram(), wr_prot_nvram(), xmodem();

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
  "cookie",toss_cookie,"display contents of cookie PROM in hex",0,
  "dev",devs,"list the device table",0,
  "dir",directory,"list files in file system",0,
  "dis", disasm, "disassemble instruction stream",0,
  "dnld",dnld,"serial download a program module",0,
  "dump",dis_mem,"display a block of memory",MF_PRIVILEGED,
  "echo",echo,"monitor echo command",MF_PRIVILEGED,
  "errlog",dumperrlog,"display the error log",MF_PRIVILEGED,
  "fdump",cat,"file dump utility",MF_PRIVILEGED,
  "fill",fil_mem,"fill a block of memory",MF_PRIVILEGED,
  "flash",flash,"flash services command",MF_PRIVILEGED,
  "frame", show_stack, "print out a selected stack frame",0,
  "help",help,"monitor builtin command help",0,
  "history",history,"monitor command history",0,
  "ifill",fil_mem,"fill a block of memory w/incrementing pattern",MF_PRIVILEGED,
  "initfs",initfs,"re-initialize the file system access structures",MF_PRIVILEGED,
  "jump",jump,"call a subroutine at address with argc/argv",MF_PRIVILEGED,
  "launch", sys_launch, "launch a downloaded image",MF_PRIVILEGED,
  "memdebug",memdebug,"write/read/verify scope loop",MF_PRIVILEGED,
  "meminfo",meminfo,"main memory information",0, 
  "memloop",memloop,"write or read scope loop",MF_PRIVILEGED,
  "memtest",memtest,"simple memory test",MF_PRIVILEGED,
  "menu",diag_menu,"main diagnostic menu",MF_PRIVILEGED,
  "move",mov_mem,"move a block of memory",MF_PRIVILEGED,
  "nvwr",wr_ena_nvram,"write enable the NVRAM",MF_PRIVILEGED,
  "nvprot",wr_prot_nvram,"write protect the NVRAM",MF_PRIVILEGED,
  "parityset",parityset,"memory parity utility",MF_PRIVILEGED,
  "repeat",repeat,"repeat a monitor command",0,
  "reset",reset,"system reset",0,
  "set",setalias,"display the monitor variables",0,
  "stack", show_stack, "produce a stack trace",0,
  "step", sys_step, "step from a breakpoint",MF_PRIVILEGED,
  "sync",sync,"write monitor environment to NVRAM",0,
  "sysret", sysretdump, "print out info from last system return",0,
  "unalias",unset,"unset an alias",0,
  "unset",unset,"unset a monitor variable",0,
  "watchdog", test_watchdog, "test watchdog rebooting of the box",MF_PRIVILEGED,
  "xmodem",xmodem,"x/y modem download",0,
  "priv",priv,"enter the privileged command state",MF_HIDDEN,
};
#define MONCMDSIZ (sizeof(moncmd)/sizeof(struct monitem))

int moncmdsiz = MONCMDSIZ;

/* end of module */
