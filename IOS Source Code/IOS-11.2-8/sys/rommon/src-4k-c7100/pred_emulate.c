/* $Id: pred_emulate.c,v 3.1.68.2 1996/04/04 17:05:55 szhang Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-4k-c7100/pred_emulate.c,v $
 *------------------------------------------------------------------
 * pred_emulate.c - Predator EMT call support
 *
 * November 1995, Michael Beesley
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pred_emulate.c,v $
 * Revision 3.1.68.2  1996/04/04  17:05:55  szhang
 * CSCdi52979:  Bootloader hang after OIR HW change
 * Branch: California_branch
 *
 * Revision 3.1.68.1  1996/03/21  23:31:36  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.3  1996/02/13  00:43:07  mbeesley
 * CSCdi48770:  Make reset_io() work, add support for fault history
 * Branch: ELC_branch
 *
 * Revision 3.1.2.2  1995/12/11  19:14:57  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.1.2.1  1995/12/01  21:31:32  mbeesley
 * Branch: ELC_branch
 * Fix LED register handling, add EMT call support and memory sizing.
 *
 * Revision 3.1  1995/12/01  18:27:41  mbeesley
 * Add placeholder files for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "mon_defs.h"
#include "cpu_mips.h"
#include "signal_4k.h"
#include "confreg.h"
#include "mon_boot.h"
#include "extern.h"
#include "proto.h"

#include "../../../boot/emt.h"
#include "../../../boot/cpu.h"
#include "../../../boot/buff.h"

char vstring_sys[VSTRING_BUFF_SIZE];		/* remember previous image 
						   version */
char vstring_sys_rom[VSTRING_BUFF_SIZE];	/* ROM system version string */
uchar vsr_set;					/* has vstring_sys_rom[] 
						   been set? */
struct bootinfo bootinfo;

/*
 * emulate:
 * DiagMon EMT call support
 */
unsigned long emulate (r4k_context_t *frame, ulong a0, 
		       ulong a1, ulong a2, ulong a3)
{
    char *emtmsg = (char *)0;

    enacon();  		/* make sure console is enabled */

    switch(a0) {
    
    case EMT_PUTCHAR:
	putchar(a1);
	break;

    case EMT_VERSION:
	return((ROM_MAJORVERSION*256)+ROM_MINORVERSION);

    case EMT_GETCHAR:
	return(getchar());

    case EMT_GETMEMSIZE:
	return(sizemainmem());
	
    case EMT_BOOT_CONFREG:  		/* return the current copy */
	return((NVRAM)->confreg);

    case EMT_GETCONFIG:  		/* return the ones compliment 
					   copy from NVRAM */
	if (NV_VALID_CONFREG) {
	    return(nv_read_ushort(&NVRAM_SAV->rconfreg));
	} else {
	    return(-1);
	}

    case EMT_SETCONFIG:
	/*
	 * Change confreg in NVRAM
	 * Do not change the in-core copy
	 */
	NV_WR_CONFREG(a1);
	break;

    case EMT_PROCTYPE:
	return(cpu_type);

    case EMT_RELOAD:
	setcp0_sreg(getcp0_sreg() | SREG_BEV);  /* use monitor vectors */
	disable_watchdog();
	log_sys_ret(EXC_RELOAD, frame);
	reset(0, 0);
	break;

    case EMT_VSTRING:  			/* return the monitor version string */
	return((unsigned long)banner_string);

    case EMT_LASTERROR:
	return((long)(NVRAM)->sys_ret_info);

    case EMT_GETCHAR_RAW:
	return(getchar());

    case EMT_RESET_IO:
	return(reset_io());

    case EMT_GET_REV:
	return(-1);			/* is this right ? */

    case EMT_GET_NVRAM_SIZE:  		/* the size of nvram */
	return(size_nvram());

    /*
     * DANGER, DANGER, if size of nvmonvars in nvram is greater
     * than 2k, customers may lose their system configs.
     */
    case EMT_MON_NVRAM_BYTES:  		/* offset from beginning for 
					   system config */
	if (sizeof(struct nvram) > 2048) {
	    printf("ROM: WARNING, user may loose system configuration\n");
	}
	return(sizeof(struct nvram) < 2048 ? 2048 : sizeof(struct nvram));

    case EMT_PROCNAME:
	return((unsigned long)platform_string);

    case EMT_GET_BOOTINFO:
	return((int)&bootinfo);

    case EMT_BOOTIPADDR:
	return(bootipaddr);

    case EMT_GET_IOBASE:
	return(ADRSPC_PCISRAM);

    case EMT_GET_IOSIZE:
	return(0x100000);

    case EMT_GET_COOKIE:
	emtmsg = "EMT_GET_COOKIE";
	break;

    case EMT_VSTRING_SYS:
	return((long)vstring_sys);

    case EMT_VSTRING_SYS_SET:
	boot_satisfied = 1;
	clr_autoboot_state();
	strncpy(vstring_sys, (char *)a1, sizeof(vstring_sys));
	return(1);

    case EMT_VSTRING_SYS_ROM:
	return((long)vstring_sys_rom);

    case EMT_VSTRING_SYS_ROM_SET:
	if (vsr_set == 0) {
	    vsr_set = 1;
	    strncpy(vstring_sys_rom, (char *) a1, sizeof(vstring_sys_rom));
	}
	return(1);

    case EMT_NVCOMPRESS_SUPPORT:
	return(0);

    case EMT_SWCONFREG_SUPPORT:
	return(1);

    case EMT_EXCEPTION_TABLE_ADDR:
	return((unsigned long)r4k_exception_table);

    case EMT_CPU_EXCEPTION:
	setcp0_sreg(getcp0_sreg() | SREG_BEV);  /* use monitor vectors */
	system_returned(frame, a1, a2, a3);	/* will not return */
	break;

    case EMT_GET_COOKIE_COPY:
	emtmsg = "EMT_GET_COOKIE_COPY";
	break;

    case EMT_SETVAR:
	return(setmonvar(a1));

    case EMT_GETVAR:
	return(getmonvar(a1, a2, a3));

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

    case EMT_RESET_REASON:
	return(get_reset_reason());

    case EMT_FAULT_HISTORY_ADDR:
	return((long)(&(NVRAM)->fault_history));

    case EMT_FAULT_HISTORY_BYTES:
	return(FLTSIZ);

    case EMT_WRITE_FAULT_HISTORY:
	savenv();
	break;

    case EMT_DRAM_PARITY:
 	return(bank_parity);

    default:
	return(-1);
    }
    if (emtmsg) {
	enacon();
	printf("ROM: EMT call \"%s\" requested but not supported\n", emtmsg);
	return(-1);
    }
    return(0);
}

/* End of file */



