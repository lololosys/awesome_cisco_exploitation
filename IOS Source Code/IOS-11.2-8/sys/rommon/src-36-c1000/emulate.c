/* $Id: emulate.c,v 3.4.12.1 1996/03/21 23:28:55 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-36-c1000/emulate.c,v $
 *------------------------------------------------------------------
 * emulate.c
 *
 * October 1994, Rob Clevenger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * ROM Monitor System Calls for 68k boxes.
 *------------------------------------------------------------------
 * $Log: emulate.c,v $
 * Revision 3.4.12.1  1996/03/21  23:28:55  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.3  1996/03/19  01:01:11  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.2  1995/12/11  19:14:46  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.2.2.1  1995/12/01  04:21:51  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.4  1996/02/17  05:49:19  smaser
 * CSCdi46943:  break command in Rom Monitor causes image checksum
 * validation to fail
 *
 * Revision 3.3  1995/12/03  17:59:56  thille
 * CSCdi44905:  ciscopro targets not in 11.1
 * Put them in.
 *
 * Revision 3.2  1995/11/17  18:43:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:07:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/08  00:46:36  smaser
 * CSCdi38454:  boot system rom fails
 *
 * Revision 2.1  1995/06/07  22:39:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "mon_defs.h"
#include "signal_4k.h"
#include "confreg.h"
#include "cookie.h"
#include "mon_boot.h"
#include "extern.h"
#include "exception.h"
#include "context.h"

#include "../../../boot/cpu.h"
#include "../../../boot/emt.h"
#include "../../../boot/flh.h"

char vstring_sys[257];		/* remember previous image version */
char vstring_sys_rom[257];	/* ROM system version string */
uchar vsr_set;			/* has vstring_sys_rom[] been set? */
extern long evec;
struct bootinfo bootinfo;


/*
** emulate:
** Monitor EMT call support
**
** We got here thru a trap #15, thus the stack has a trap stack
** frame (4 word stack) which we don't use, but must account for.
*/
unsigned long
emulate(ulong trapframe1, ulong trapframe2,
	int emt_code, ulong arg1, ulong arg2, ulong arg3)
{
    char *emtmsg = (char *)0;

#ifdef DEBUG
save_sys_console_state();
printf("ROMMON: emt call %d\n", emt_code);
restore_sys_console_state();
#endif

    switch(emt_code) {
    
    case EMT_PUTCHAR:  /* FIXME! */
	putchar(arg1);
	break;

    case EMT_VERSION:
	return((ROM_MAJORVERSION*256)+ROM_MINORVERSION);

    case EMT_GETCHAR:  /* FIXME! */
	return(getchar());

    case EMT_GETMEMSIZE:
	return(sizemainmem());
	
    case EMT_BOOT_CONFREG:  /* return the current copy */
	return((NVRAM)->confreg);

    case EMT_GETCONFIG:  /* return the ones compliment copy from NVRAM */
	if(NV_VALID_CONFREG) return((NVRAM_SAV)->rconfreg);
	return(-1);

    case EMT_SETCONFIG:
	/*
	** Change confreg in NVRAM
	** Do not change the in-core copy
	*/
	NV_WR_CONFREG(arg1);
	break;

    case EMT_PROCTYPE:
	if (ciscopro_flag != 0)
	    return(cpu_type | CPU_CISCOPRO_MASK);
	else
	    return(cpu_type);

    case EMT_RELOAD:
	/* timer_disable() - turn off PIT was moved to do_reset() */
	chgint(7);        /* turn off interrupts */
	setvbr(&evec);    /* use monitor vectors */
	log_sys_ret(EXC_RELOAD, (cpu_context_t *)0);
	do_reset();
	break;

    case EMT_VSTRING:  /* return the monitor version string */
	return((unsigned long)banner_string);

    case EMT_LASTERROR:
	return((long)(NVRAM)->sys_ret_info);
    /*
     * This call has been added to provide a mechanism for bootload() to inform
     * the C1000 rom monitor that it has detected the "boot system rom" request
     * in the configuration and that the rom monitor should go ahead and boot up the
     * rboot image as the running system image.
     */
    case EMT_CMD_BOOT_SYS_ROM:
	boot_sys_rom_flag = 1;
	return (0);

    case EMT_GETCHAR_RAW:  /* FIXME! */
	return(getchar());

    case EMT_RESET_IO:
	return(reset_io());

    case EMT_GET_REV:         /* the hardware rev */
	return(hw_rev);

    case EMT_GET_NVRAM_SIZE:  /* the size of nvram */
	return(size_nvram());

    case EMT_MON_NVRAM_BYTES:  /* offset from beginning for system config */
	return(sizeof(struct nvram));

    case EMT_PROCNAME:
	return((unsigned long)platform_string);

    case EMT_GET_BOOTINFO:
	return((int)&bootinfo);

    case EMT_BOOTIPADDR:
	return(bootipaddr);

    case EMT_GET_IOBASE:
	return(-1);  /* sapphire has no I/O memory */

    case EMT_GET_IOSIZE:
	return(0);   /* sapphire has no I/O memory */

    case EMT_GET_COOKIE:
	return((unsigned long)get_cookie());

    case EMT_VSTRING_SYS:
	return((long)vstring_sys);

    case EMT_VSTRING_SYS_SET:
	boot_satisfied = 1;
	clr_autoboot_state();
	strncpy(vstring_sys, (char*)arg1, sizeof(vstring_sys));
	return(1);

    case EMT_VSTRING_SYS_ROM:
	return((long)vstring_sys_rom);

    case EMT_VSTRING_SYS_ROM_SET:
	if (vsr_set == 0) {
	    vsr_set = 1;
	    strncpy(vstring_sys_rom, (char*) arg1,
		    sizeof(vstring_sys_rom));
	}
	return(1);

    case EMT_NVCOMPRESS_SUPPORT:
	return(0);

    case EMT_SWCONFREG_SUPPORT:
	return(1);

    case EMT_EXCEPTION_TABLE_ADDR:
	return(ADRSPC_PROM);

    case EMT_GET_COOKIE_COPY:
	bcopy((char *)get_cookie(), (char *)arg1, sizeof(struct cookie)); 
	return(0);

    case EMT_SETVAR:
	return(setmonvar(arg1));

    case EMT_GETVAR:
	return(getmonvar(arg1,arg2,arg3));

    case EMT_FLASH_LOAD_HELPER:
	switch(arg1) {
	case FLH_GET_VERSION:
	    /* return the version of this FLH emt call */
	    return(FLH_VERSION_WITH_RCP);
	default:
	    return(-1);
	}

    case EMT_TOUCHR:
	emtmsg = "EMT_TOUCHR";
	break;
    case EMT_TOUCHW:
	emtmsg = "EMT_TOUCHW";
	break;
    case EMT_HCLOCKADDR:
	emtmsg = "EMT_HCLOCKADDR";
	break;
    case EMT_CLOCKADDR:
	emtmsg = "EMT_CLOCKADDR";
	break;
    case EMT_WRITE_BAD_MAIN_MEMORY_PARITY:
	emtmsg = "EMT_WRITE_BAD_MAIN_MEMORY_PARITY";
	break;
    case EMT_SETCAP:
	emtmsg = "EMT_SETCAP";
	break;
    case EMT_SETECHO:
	emtmsg = "EMT_SETECHO";
	break;
    case EMT_TICKS:
	emtmsg = "EMT_TICKS";
	break;
    case EMT_SETCACR:
	emtmsg = "EMT_SETCACR";
	break;
    case EMT_DISABLE_DCACHE:
	emtmsg = "EMT_DISABLE_DCACHE";
	break;
    case EMT_ENABLE_DCACHE:
	emtmsg = "EMT_ENABLE_DCACHE";
	break;
    case EMT_SYS_CHKSUM_DISABLE:
	return (sys_chksum_disable);
	break;

    case EMT_CPU_EXCEPTION: /* this is not supported on 68k based platforms */
    case EMT_ROMADDR:
    default:
	return(-1);
    }
    if(emtmsg) {
	save_sys_console_state();
	printf("ROM Monitor EMT police: EMT call \"%s\" requested but not supported\n",
	       emtmsg);
	restore_sys_console_state();
	return(-1);
    }
    return(0);
}

/* End of file */
