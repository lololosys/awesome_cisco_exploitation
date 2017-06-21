/* $Id: emulate.c,v 1.1.68.1 1996/06/16 21:18:47 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-68-as5200/emulate.c,v $
 *------------------------------------------------------------------
 * emulate.c -- ROM Monitor System Calls for 68k Brasil boxes.
 *
 * October 1995, Joe Turner
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: emulate.c,v $
 * Revision 1.1.68.1  1996/06/16  21:18:47  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 1.1  1995/12/03  21:16:25  jturner
 * Placeholder for Brasil.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "types.h"
#include "emt.h"
#include "mon_defs.h"
#include "mon_plat_defs.h"
#include "cpu.h"
#include "signal_4k.h"
#include "confreg.h"
#include "nvmonvars.h"
#include "pcmap.h"
#include "as5200_pcmap.h"
#include "cookie.h"
#include "mon_boot.h"
#include "extern.h"
#include "exception.h"
#include "context.h"
#include "flh.h"
#include "console.h"

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
printf("ROMMON: emt call %d\n", emt_code);
printf("ROMMON: emt arg1 %c\n", arg1);
printf("ROMMON: emt arg2 %c\n", arg2);
printf("ROMMON: emt arg3 %c\n", arg3);
#endif

    switch(emt_code) {
    
    case EMT_PUTCHAR:
	putchar(arg1);
	break;

    case EMT_VERSION:
	return((ROM_MAJORVERSION*256)+ROM_MINORVERSION);

    case EMT_GETCHAR:
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
     * the rom monitor that it has detected the "boot system rom" request in 
     * the configuration and that the rom monitor should go ahead and boot up
     * the rboot image as the running system image.
     */
    case EMT_CMD_BOOT_SYS_ROM:
        boot_sys_rom_flag = 1;
        return (0);

    case EMT_GETCHAR_RAW:
	return(getchar());

    case EMT_RESET_IO:
	return(reset_io());

    case EMT_GET_REV:         /* the hardware rev */
	return(hw_rev);

    case EMT_GET_NVRAM_SIZE:  /* the size of nvram */
	return(size_nvram());

        /*
        ** DANGER, DANGER, if size of nvmonvars in nvram is greater
        ** than 2k, customers may lose their system configs.
        */
    case EMT_MON_NVRAM_BYTES:  /* offset from beginning for system config */
        if(sizeof(struct nvram) > 2048)
            printf("ROM Monitor police: WARNING, user may loose system configuration\n");
        return(sizeof(struct nvram) < 2048 ? 2048 : sizeof(struct nvram));

    case EMT_PROCNAME:
	return((unsigned long)platform_string);

    case EMT_GET_BOOTINFO:
	return((int)&bootinfo);

    case EMT_BOOTIPADDR:
	return(bootipaddr);

    case EMT_GET_IOBASE:
	return(shmemstart());

    case EMT_GET_IOSIZE:
	return(sizeshmem());

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
	 return(-1);

    case EMT_TOUCHR:
	emtmsg = "EMT_TOUCHR";
	break;
    case EMT_TOUCHW:
	emtmsg = "EMT_TOUCHW";
	break;
    case EMT_HCLOCKADDR:
	return((long)&hclock);
    case EMT_CLOCKADDR:
	return((long)&clock);
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
	return(setcacr(arg1));
	break;
    case EMT_DISABLE_DCACHE:
	setcacr(CLR_DCACHE); /* Clear dcache */
	setcacr(~CACHE_D); /* Disable dcache */
	return(0);
	break;
    case EMT_ENABLE_DCACHE:
	setcacr(CLR_DCACHE); /* Clear dcache */
	setcacr(ENABLE_CACHES); /* Enable */
	return(0);
	break;

    case EMT_CPU_EXCEPTION: /* this is not supported on 68k based platforms */
    case EMT_ROMADDR:
    default:
	return(-1);
    }
    if(emtmsg) {
	printf("ROM Monitor EMT police: EMT call \"%s\" requested but not supported\n",
	       emtmsg);
	return(-1);
    }
    return(0);
}

/* End of file */
