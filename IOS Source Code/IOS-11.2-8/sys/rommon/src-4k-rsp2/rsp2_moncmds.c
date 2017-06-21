/* $Id: rsp2_moncmds.c,v 3.3.30.4 1996/04/19 18:01:27 mansonw Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-4k-rsp2/rsp2_moncmds.c,v $
 *------------------------------------------------------------------
 * rsp2_moncmds.c
 *
 * Oct, 95, Steve J. Zhang
 *
 * Copyright (c) 1995 - 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp2_moncmds.c,v $
 * Revision 3.3.30.4  1996/04/19  18:01:27  mansonw
 * CSCdi54854:  ROMMON set command does not set monitor variables
 * Branch: California_branch
 * Change meaning of set command
 *
 * Revision 3.3.30.3  1996/04/11  23:01:36  mansonw
 * CSCdi54327:  HSA ROMMON commands are not in alphabetical order
 * Branch: California_branch
 *
 * Reorder the ROMMON commands in order.
 *
 * Revision 3.3.30.2  1996/04/11  22:51:39  mansonw
 * CSCdi54329:  In privileged mode ROMMON accepts slave cmd, no slave cmd
 * supported
 * Branch: California_branch
 *
 * Remove slave command as the virtual console is not supported in RSP2
 *
 * Revision 3.3.30.1  1996/03/21  23:33:03  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.2  1996/02/08  08:59:45  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.1  1995/12/01  04:24:38  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.3  1996/01/17  23:32:24  mansonw
 * CSCdi47058:  Fix the broken ROMMON build
 *
 * Revision 3.2  1995/11/17  18:47:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:09:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:24:19  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/06  22:57:04  szhang
 * placeholder for rsp2_moncmds.c
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "mon_boot.h"

extern int addrloop(), setalias(), alt_mem(), berrscan(), mon_boot(), break_point(), 
    call(), cat(), memory_checksum(), clrerrlog(), cmp_mem(), confreg(), sys_cont(), 
    show_context(), toss_cookie(), cpu_config(), hw_cycles(), devs(), directory(), 
    disasm(), dnld(), dis_mem(), echo(), dumperrlog(), cat(), dump_ideeprom(),
    fil_mem(), flash(), show_frame(), help(), history(), fil_mem(), initfs(), 
    jump(), sys_launch(), testleds(), memdebug(), meminfo(), memloop(), memtest(), 
    mov_mem(), paritytest(), repeat(), reset(), setalias(), sleep_cmd(), 
    speed_lp(), show_stack(), sync(), sysretdump(), tcal(), tlbdump(), tlbflush(), 
    tlbmap(), tlbpid(), tlbphy(), tlbtest(), tlbscan(), tlbvir(), scopetimer(), 
    unset(), unset(), test_watchdog(), priv(), dump_ds1201(), setvars(), 
    setms(), showreg(), showvars(), show_env_mon(), resetboth(), dis_crash_info(), fusion();

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
  "dnld",dnld,"serial download a program module",0,
  "ds1201",dump_ds1201,"display dallas DS1201 info data",MF_PRIVILEGED,
  "dump",dis_mem,"display a block of memory",MF_PRIVILEGED,
  "echo",echo,"monitor echo command",MF_PRIVILEGED,
  "errlog",dumperrlog,"display the error log",MF_PRIVILEGED,
  "fdump",cat,"file dump utility",MF_PRIVILEGED,
  "fill",fil_mem,"fill a block of memory",MF_PRIVILEGED,
  "flash",flash,"flash services command",MF_PRIVILEGED,
  "frame", show_frame, "print out a selected stack frame",0,
  "help",help,"monitor builtin command help",0,
  "history",history,"monitor command history",0,
  "ideeprom",dump_ideeprom,"dump the ID PROM content",MF_PRIVILEGED,
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
  "resetboth", resetboth, "reset both RSP2", MF_PRIVILEGED,
  "set",setalias,"display the monitor variables",0,
  "setms",setms,"set master or slave",MF_PRIVILEGED,
  "setvars",setvars,"set the NVRAM variables for master/slave arbitration",MF_PRIVILEGED,
  "showenv", show_env_mon, "show the reason of environment monitor interrupt", MF_PRIVILEGED,
  "showerr", dis_crash_info, "show crash error message", MF_PRIVILEGED,
  "showreg",showreg,"show register content",MF_PRIVILEGED,
  "showvars",showvars,"show all the NVRAM variables for master/slave arbitration",MF_PRIVILEGED,
  "speed", speed_lp, "timed performance loop",MF_PRIVILEGED,
  "stack", show_stack, "produce a stack trace",0,
  "sync",sync,"write monitor environment to NVRAM",0,
  "sysret", sysretdump, "print out info from last system return",0,
  "tcal",tcal,"timer calibration test",MF_PRIVILEGED,
  "tlbdump", tlbdump, "display the cpu TLB",MF_PRIVILEGED,
  "tlbflush", tlbflush, "flush the TLB",MF_PRIVILEGED,
  "tlbmap", tlbmap, "initialize a TLB mapping",MF_PRIVILEGED,
  "tlbpid", tlbpid, "set/display process ID number",MF_PRIVILEGED,
  "tlbphy", tlbphy, "search TLB for physical translation",MF_PRIVILEGED,
  "tlbtest", tlbtest, "test the TLB",MF_PRIVILEGED,
  "tlbscan", tlbscan, "scan for TLB exceptions",MF_PRIVILEGED,
  "tlbvir", tlbvir, "search TLB for a virtual translation",MF_PRIVILEGED,
  "tscope",scopetimer,"timer scope loop",MF_PRIVILEGED,
  "unalias",unset,"unset an alias",0,
  "unset",unset,"unset a monitor variable",0,
  "watchdog", test_watchdog, "test watchdog rebooting of the box",MF_PRIVILEGED,
  "priv",priv,"enter the privileged command state",MF_HIDDEN,
  "fusion", fusion, "show the developer names", MF_HIDDEN};
#define MONCMDSIZ (sizeof(moncmd)/sizeof(struct monitem))

int moncmdsiz = MONCMDSIZ;






