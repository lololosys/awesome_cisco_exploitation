/* $Id: monitor1.c,v 3.5.12.2 1996/03/21 22:29:31 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/dev/monitor1.c,v $
 *------------------------------------------------------------------
 * Monitor functions
 *
 * September 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: monitor1.c,v $
 * Revision 3.5.12.2  1996/03/21  22:29:31  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.5.12.1  1996/03/18  19:13:00  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.3  1996/03/18  22:46:14  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.3.2.2  1996/02/16  02:22:07  mbeesley
 * CSCdi48910:  Add storage of fault history for crashes
 * Branch: ELC_branch
 *
 * Revision 3.3.2.1  1995/12/11  18:32:19  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.5  1996/02/17  05:47:38  smaser
 * CSCdi46943:  break command in Rom Monitor causes image checsum
 * validation to fail
 *
 * Revision 3.4  1995/12/03  17:56:11  thille
 * CSCdi44905:  ciscopro targets not in 11.1
 * Put them in.
 *
 * Revision 3.3  1995/11/17  09:01:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:15:42  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:20:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:54:24  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/09/08  00:45:15  smaser
 * CSCdi38454:  boot system rom fails
 *
 * Revision 2.1  1995/06/07  20:25:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../dev/monitor1.h"
#include "../../boot/cpu.h"
#include "../../boot/emt.h"

void mon_reload (void)
{
    (void)emt_call(EMT_RELOAD);
}

uchar *mon_get_iobase (void)		/* LES */
{
    return((uchar *) emt_call(EMT_GET_IOBASE));
}

char *mon_vstring (void)
{
    return((char *)emt_call(EMT_VSTRING));
}

int mon_write_bad_main_memory_parity (ulong a, volatile uint *address)
{
    return((int)emt_call(EMT_WRITE_BAD_MAIN_MEMORY_PARITY, a, address));
}

uint mon_setcacr (uint val)
{
    return((uint)emt_call(EMT_SETCACR, val));
}

void *mon_get_powerfail_addr (void)		/* HES */
{
    return((void *)emt_call(EMT_GET_POWERFAIL_ADDR));
}

ulong mon_getmemsize (void)
{
    return((ulong)emt_call(EMT_GETMEMSIZE));
}

boolean mon_touchr (void *addr)
{
    return((boolean)emt_call(EMT_TOUCHR, addr));
}

void mon_disable_dcache (void)
{
    (void)emt_call(EMT_DISABLE_DCACHE);
}

void mon_enable_dcache (void)
{
    (void)emt_call(EMT_ENABLE_DCACHE);
}

uchar mon_get_rev (void)
{
    return((uchar)emt_call(EMT_GET_REV));
}

void *mon_get_cookie (void)
{
    return((void *)emt_call(EMT_GET_COOKIE));
}

void mon_get_cookie_copy (void *cookie)
{
    (void)emt_call(EMT_GET_COOKIE_COPY, cookie);
}

long mon_get_nvram_size (void)
{
    return((long)emt_call(EMT_GET_NVRAM_SIZE));
}

long mon_get_mon_nvram_bytes (void)
{
    return((long)emt_call(EMT_MON_NVRAM_BYTES));
}

int mon_get_iosize (void)
{
    return((int)emt_call(EMT_GET_IOSIZE));
}

ushort mon_version (void)
{
    return((ushort)emt_call(EMT_VERSION));
}

void *mon_get_bootinfo (void)
{
    return((void *)emt_call(EMT_GET_BOOTINFO));
}

ipaddrtype mon_bootipaddr (void)
{
    return((ipaddrtype)emt_call(EMT_BOOTIPADDR));
}

char *mon_procname (void)
{
    return((char *)emt_call(EMT_PROCNAME));
}

long *mon_lasterror (void)
{
    return((long *)emt_call(EMT_LASTERROR));
}
long mon_boot_sys_rom_flg_set(void)
{
  return (long)emt_call(EMT_CMD_BOOT_SYS_ROM);
}

long mon_sys_chksum_disable(void)
{
  /*
   * If this emt_call is not defined for a particular
   * platform, a non-zero value is returned, allowing
   * the image checksum validation code to run.
   */
  return ((long)emt_call(EMT_SYS_CHKSUM_DISABLE));
}

int mon_getchar_raw (void)
{
    return((int)emt_call(EMT_GETCHAR_RAW));
}

int mon_boot_confreg (void)
{
    return((int)emt_call(EMT_BOOT_CONFREG));
}

ushort mon_getconfig (void)
{
    return((ushort)emt_call(EMT_GETCONFIG));
}

int mon_set_confreg (ushort number)
{
    return((int)emt_call(EMT_SETCONFIG, number));
}

void mon_set_ms (uint val)
{
    (void)emt_call(EMT_SETMS, val);
}

uint mon_get_ms (void)
{
    return((uint)emt_call(EMT_READMS));
}

void mon_vstring_sys_rom_set (char *version)
{
    (void)emt_call(EMT_VSTRING_SYS_ROM_SET, version);
}

char *mon_vstring_sys (void)
{
    return((char *)emt_call(EMT_VSTRING_SYS));
}

void mon_vstring_sys_set (char *version)
{
    (void)emt_call(EMT_VSTRING_SYS_SET, version);
}

ulong mon_proctype (void)
{
    return((ulong)emt_call(EMT_PROCTYPE));
}

ulong *mon_clockaddr (void)
{
    /*
     * On 68k based boxes, the ROM Monitor is the keeper of
     * the millisecond timer variables.
     */
    return((ulong *)emt_call(EMT_CLOCKADDR));
}

ulong *mon_hclockaddr (void)
{
    /*
     * On 68k based boxes, the ROM Monitor is the keeper of
     * the millisecond timer variables
     */
    return((ulong *)emt_call(EMT_HCLOCKADDR));
}

void *mon_romaddr (void)
{
    return((void *)emt_call(EMT_ROMADDR));
}

char *mon_vstring_sys_rom (void)
{
    return((char *)emt_call(EMT_VSTRING_SYS_ROM));
}

int mon_simbrk (void)
{
    return((int)emt_call(EMT_SIMBRK));
}

ulong *mon_exception_addr (void)
{
    return((ulong *)emt_call(EMT_EXCEPTION_TABLE_ADDR));
}

int mon_cpu_exception (int sig, int code, void *ptr)
{
    return(emt_call(EMT_CPU_EXCEPTION, sig, code, ptr));
}

boolean mon_nvcompress_support (void)
{
    if (emt_call(EMT_NVCOMPRESS_SUPPORT) == TRUE)
	return (TRUE);
    else
	return (FALSE);
}

boolean mon_swconfreg_support (void)
{
    if (emt_call(EMT_SWCONFREG_SUPPORT) == TRUE)
	return (TRUE);
    else
	return (FALSE);
}

int mon_set_var (char *varstr)
{
    return((int) emt_call(EMT_SETVAR, varstr));
}
 
/*
 * returns -1 if var does not exist
 *	-2 if buffer too small
 *	copies & NUL-terminates variable value into destbuf
 *	returns number of characters in variable value
 */
int mon_get_var (char *varname, char *destbuf, int destbufsize)
{
    return((int) emt_call(EMT_GETVAR, varname, destbuf, destbufsize));
}

/*
 * return "reset reason" register
 */
uchar mon_reset_reason (void)
{
    return emt_call(EMT_RESET_REASON);
}

/*
 * mon_fault_history_addr:
 * Return a pointer to the fault history buffer
 */
char *mon_fault_history_addr (void)
{
    return((char *)emt_call(EMT_FAULT_HISTORY_ADDR));
}

/*
 * mon_fault_history_bytes:
 * Return size of fault history buffer
 */
int mon_fault_history_bytes (void)
{
    return((int)emt_call(EMT_FAULT_HISTORY_BYTES));
}

/*
 * mon_write_fault_history:
 * Commit the fault history to NVRAM
 */
void mon_write_fault_history (void)
{
    emt_call(EMT_WRITE_FAULT_HISTORY);
}

/* end of file */
