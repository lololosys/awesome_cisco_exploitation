/* $Id: emulate.c,v 3.5.4.1 1996/03/21 23:32:11 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k-rsp/emulate.c,v $
 *------------------------------------------------------------------
 * emulate_4k.c
 *
 * Mar, 1995, Steve J. Zhang
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * ROM Monitor System Calls for R4k boxes.
 *------------------------------------------------------------------
 * $Log: emulate.c,v $
 * Revision 3.5.4.1  1996/03/21  23:32:11  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.3  1996/03/19  01:01:34  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.2  1996/02/08  08:58:52  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.1  1995/12/01  04:24:04  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.5  1996/03/11  06:00:30  tkam
 * CSCdi51218:  Need to implement fault history mechanism
 * Added fault history support and misc. cleanup.
 *
 * Revision 3.4  1996/01/17  23:31:37  mansonw
 * CSCdi47058:  Fix the broken ROMMON build
 *
 * Revision 3.3  1996/01/10  03:30:39  tkam
 * CSCdi34476:  r4k ROM monitor doesnt save crashed system version
 * Bug fixes and adding RSP7000 support.
 *
 * Revision 3.2  1995/11/17  18:46:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:08:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:41:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "mon_defs.h"
#include "cpu_mips.h"
#include "signal_4k.h"
#include "confreg.h"
#include "dclasic.h"
#include "mon_boot.h"
#include "extern.h"
#include "proto.h"

#include "../../../boot/emt.h"
#include "../../../boot/cpu.h"

char vstring_sys[VSTRING_BUFF_SIZE];		/* remember previous image version */
char vstring_sys_rom[VSTRING_BUFF_SIZE];	/* ROM system version string */
uchar vsr_set;			/* has vstring_sys_rom[] been set? */

struct bootinfo bootinfo;

/*
 * emulate:
 * DiagMon EMT call support
 */
unsigned long
emulate(r4k_context_t *frame, unsigned long a0, unsigned long a1,
	unsigned long a2, unsigned long a3)
{
    char *emtmsg = (char *)0;

    enacon();  /* make sure console is enabled */

#ifdef DEBUG
printf("ROMMON: emt call %d\n", a0);
#endif

    switch(a0) {
    
    case EMT_PUTCHAR:
	putchar(a1);
	break;

/* FIXME
** kludge - return real version here
*/
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
	NV_WR_CONFREG(a1);
	break;

    case EMT_PROCTYPE:
	return(cpu_type);

    case EMT_RELOAD:
	setcp0_sreg(getcp0_sreg() | SREG_BEV);  /* use monitor vectors */
	disable_watchdog();
	log_sys_ret(EXC_RELOAD, frame);
#ifdef RSP2
        REG_IRSP2DBUS_FPGA->MasterSlaveCmd |= MS_CMD_IRSP2_NON_PART;
        if (REG_IRSP2DBUS_FPGA->MasterSlaveStatus & MS_STATUS_THIS_IRSP2_NON_PART == 0)
            printf("This RSP2 can not be set to non-participant during the system reload\n");
#endif
	reset(0, 0);
	break;

    case EMT_VSTRING:  /* return the monitor version string */
	return((unsigned long)banner_string);

    case EMT_LASTERROR:
	return((long)(NVRAM)->sys_ret_info);

    case EMT_GETCHAR_RAW:
	return(getchar());

    case EMT_RESET_IO:
	return(reset_io());

    case EMT_GET_REV:
	return((DCL_ASIC->asic_reg & ASIC_REV) >> ASIC_REV_BIT_OFS);

    case EMT_GET_NVRAM_SIZE:  /* the size of nvram */
	return(size_nvram());

	/*
	** DANGER, DANGER, if size of nvmonvars in nvram is greater
	** than ROMNVSIZE (4k) , customers may lose their system configs.
	*/
    case EMT_MON_NVRAM_BYTES:  /* offset from beginning for system config */
	if(sizeof(struct nvram) > ROMNVSIZE)
	    printf("ROM Monitor police: WARNING, user may loose system configuration\n");
	return(sizeof(struct nvram) < ROMNVSIZE ? ROMNVSIZE : sizeof(struct nvram));

    case EMT_PROCNAME:
	return((unsigned long)platform_string);

    case EMT_GET_BOOTINFO:
	return((int)&bootinfo);

    case EMT_BOOTIPADDR:
	return(bootipaddr);

    case EMT_GET_IOBASE:
	return(KSEG1_ADRSPC_MEMD);

    case EMT_GET_IOSIZE:
	return(sizeshmem());

    case EMT_GET_COOKIE:
	emtmsg = "EMT_GET_COOKIE";
	break;

    case EMT_VSTRING_SYS:
	return((long)NVRAM->vstring_sys);

    case EMT_VSTRING_SYS_SET:
#ifdef DEBUG
	printf("ROMMON EMT_VSTRING_SYS_SET called (boot_satisfied)\n");
#endif
	boot_satisfied = 1;
	clr_autoboot_state();
	strncpy(vstring_sys, (char*)a1, sizeof(vstring_sys));
	strncpy(NVRAM->vstring_sys, (char*)a1, sizeof(NVRAM->vstring_sys));
	return(1);

    case EMT_VSTRING_SYS_ROM:
	return((long)vstring_sys_rom);

    case EMT_VSTRING_SYS_ROM_SET:
	if (vsr_set == 0) {
	    vsr_set = 1;
	    strncpy(vstring_sys_rom, (char*) a1,
		    sizeof(vstring_sys_rom));
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
	return(getmonvar(a1,a2,a3));

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
    case EMT_SETMS:
        NVRAM_SAV->default_master = a1;
    case EMT_READMS:
        return(NVRAM_SAV->default_master);
    case EMT_FAULT_HISTORY_ADDR:
        return((long)(&(NVRAM)->fault_history));
    case EMT_FAULT_HISTORY_BYTES:
        return(FLTSIZ);
    case EMT_WRITE_FAULT_HISTORY:
        savenv();
        break;
    case EMT_ROMADDR:
    default:
	return(-1);
    }
    if(emtmsg) {
	enacon();
	printf("ROM Monitor EMT police: EMT call \"%s\" requested but not supported\n",
	       emtmsg);
	return(-1);
    }
    return(0);
}

/* End of file */



