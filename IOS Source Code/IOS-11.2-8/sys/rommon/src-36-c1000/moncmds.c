/* $Id: moncmds.c,v 3.3.10.1 1996/03/21 23:29:08 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-36-c1000/moncmds.c,v $
 *------------------------------------------------------------------
 * mon_cmds.c
 *
 * November 1994, Rob Clevenger
 *
 * Copyright (c) 1995 - 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * The monitor command table for Sapphire
 *------------------------------------------------------------------
 * $Log: moncmds.c,v $
 * Revision 3.3.10.1  1996/03/21  23:29:08  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.2  1996/03/19  01:01:17  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.1  1995/12/01  04:22:04  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.3  1996/02/23  17:15:54  cshen
 * CSCdi48454:  Need to support on-board flash on c1005 derivative
 *
 * Revision 3.2  1995/11/17  18:44:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:07:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/22  04:36:21  enf
 * CSCdi40869:  Add frankenstorm to 11.0
 *
 * Revision 2.2  1995/07/21  22:17:31  clev
 * CSCdi37337:  Set help is vague
 * Correct the help message
 *
 * Revision 2.1  1995/06/07  22:39:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "monitor.h"
#include "mon_boot.h"

extern int addrloop(), setalias(), alt_mem(), berrscan(), mon_boot(), break_point(), 
    call(), cat(), memory_checksum(), clrerrlog(), cmp_mem(), confreg(), sys_cont(), 
    show_context(), toss_cookie(), cpu_config(), hw_cycles(), devs(), directory(), 
    disasm(), dnld(), dram_verify(), dis_mem(), echo(), dumperrlog(), cat(), 
    fil_mem(), flash(), help(), history(), fil_mem(), initfs(), 
    jump(), sys_launch(), testleds(), memdebug(), meminfo(), memloop(), memtest(), 
    diag_menu(), mov_mem(), paritytest(), repeat(), reset(), setalias(), sleep_cmd(), 
    show_stack(), sync(), sysretdump(), tcal(), scopetimer(), sys_step(),
    watch_point(), unset(), unset(), test_watchdog(), priv(), pcmcia_cmds();

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
#ifdef ONBOARD_FLASH  
  "flash",flash,"flash services command",MF_PRIVILEGED,
#endif
  "frame", show_stack, "print out a selected stack frame",0,
  "help",help,"monitor builtin command help",0,
  "history",history,"monitor command history",0,
  "ifill",fil_mem,"fill a block of memory w/incrementing pattern",MF_PRIVILEGED,
  "initfs",initfs,"re-initialize the file system access structures",MF_PRIVILEGED,
  "jump",jump,"call a subroutine at address with argc/argv",MF_PRIVILEGED,
  "launch", sys_launch, "launch a downloaded image",MF_PRIVILEGED,
#ifdef FIXME_LATER
  "leds",testleds,"check out the error LED",MF_PRIVILEGED,
#endif /* FIXME_LATER */
  "memdebug",memdebug,"write/read/verify scope loop",MF_PRIVILEGED,
  "meminfo",meminfo,"main memory information",0, 
  "memloop",memloop,"write or read scope loop",MF_PRIVILEGED,
  "memtest",memtest,"simple memory test",MF_PRIVILEGED,
  "menu",diag_menu,"main diagnostic menu",MF_PRIVILEGED,
  "move",mov_mem,"move a block of memory",MF_PRIVILEGED,
  "nvwr",(int (*)())wr_ena_nvram,"write enable the NVRAM",MF_PRIVILEGED,
  "nvprot",(int (*)())wr_prot_nvram,"write protect the NVRAM",MF_PRIVILEGED,
  "partest",paritytest,"memory parity test",MF_PRIVILEGED,
#ifndef ONBOARD_FLASH
  "pcmcia",pcmcia_cmds,"pcmcia commands submenu",MF_PRIVILEGED,
#endif
  "repeat",repeat,"repeat a monitor command",0,
  "reset",reset,"system reset",0,
  "set",setalias,"display the monitor variables",0,
#ifdef FIXME_LATER
  "sleep",sleep_cmd,"millisecond sleep command",MF_PRIVILEGED,
#endif /* FIXME_LATER */
  "stack", show_stack, "produce a stack trace",0,
  "step", sys_step, "step from a breakpoint",MF_PRIVILEGED,
  "sync",sync,"write monitor environment to NVRAM",0,
  "sysret", sysretdump, "print out info from last system return",0,
#ifdef FIXME_LATER
  "tcal",tcal,"timer calibration test",MF_PRIVILEGED,
  "tscope",scopetimer,"timer scope loop",MF_PRIVILEGED,
#endif /* FIXME_LATER */
  "unalias",unset,"unset an alias",0,
  "unset",unset,"unset a monitor variable",0,
  "watch",watch_point,"set/clear a watchpoint",0,
  "watchdog", test_watchdog, "test watchdog rebooting of the box",MF_PRIVILEGED,
  "priv",priv,"enter the privileged command state",MF_HIDDEN,
};
#define MONCMDSIZ (sizeof(moncmd)/sizeof(struct monitem))

int moncmdsiz = MONCMDSIZ;

/* end of module */
