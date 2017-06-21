/* $Id: emulate.c,v 3.5.12.2 1996/05/10 23:16:55 gstovall Exp $
 * $Source: /release/112/cvs/Xboot/emulate.c,v $
 *------------------------------------------------------------------
 * emulate.c -- emulator trap handler
 *
 * August 1986, Greg Satz
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: emulate.c,v $
 * Revision 3.5.12.2  1996/05/10  23:16:55  gstovall
 * CSCdi56792:  Need to remove unused files
 * Branch: California_branch
 * The beast still exists, finish killing off the CS500, and while I am in
 * there, nuke the even crustier CSC1.
 *
 * Revision 3.5.12.1  1996/05/09  13:59:49  rbadri
 * Branch: California_branch
 * Synergy features
 *
 * Revision 3.5.38.1  1996/04/27  06:12:53  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.5.28.1  1996/04/08  01:34:13  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.5.18.1  1996/03/22  09:06:47  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.5  1996/02/19  17:28:16  abaindur
 * CSCdi30593:  SHOW VERSION underreports processor memory by 4 kilobytes
 *
 * Revision 3.4.8.1  1996/03/03  21:18:51  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.4.6.1  1996/03/02  01:06:30  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.4  1996/01/30  21:10:59  vandys
 * CSCdi46080:  Deep six CS500 in source base
 *
 * Revision 3.3.26.1  1996/02/29  07:41:49  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.3  1995/12/03  17:54:51  thille
 * CSCdi44905:  ciscopro targets not in 11.1
 * Put them in.
 *
 * Revision 3.2  1995/11/17  07:41:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.62.1  1996/02/03  06:11:31  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.44.1  1995/10/03  07:04:30  rlowe
 * Re-port IOS Boot for Synergy ATM line card from 10.3 to 11.0.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1  1995/06/07  19:11:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "defs.h"
#include "m68vectors.h"
#include "statreg.h"
#include "emt.h"
#include "cpu.h"
#include "ctype.h"
#include "reason.h"
#include "caps.h"
#include "extern.h"
#include "test.h"
#include "stdio.h"
#include "prom2.h"
#include "flh.h"
#include "flh_log.h"

extern char *oemversion, *ciscoversion;
extern int reset_io();
#if     defined (PAN) || defined (SYNALC)
extern cookie_t *cookie;
#endif /* PAN or SYNALC */

#define	ARG_TRAPTYPE	0	/* position of trap type on argument list */
#define	ARG_ARG1	1	/* position of argument 1 */
#define	ARG_ARG2	2	/* position of argument 2 */
#define ARG_ARG3        3       /* position of argument 3 */

emulate(pcaddr, usrsr, usrsp)
    long *pcaddr;		/* address of saved pc */
    long usrsr;			/* user's status register */
    register long *usrsp;	/* user's stack pointer */
{
    register long traptype;
    register long traparg1;
    register long traparg2;
#ifdef RP1
    register long traparg3;
#endif
    register u_short conf;
    register char *cp;
    static long bootloop_counter;

#ifdef BUSERROR
    char oldstate;
#endif

    if (usrsr & SR_SUPERVISOR) { /* supervisor mode trap */
	usrsp = &pcaddr[1];	/* arguments follow saved pc */
#ifndef MC68000
	++((u_short *)usrsp);	/* and frame type */
#endif
    }
    
    traptype = usrsp[ARG_TRAPTYPE];
    traparg1 = usrsp[ARG_ARG1];	/* this could be a bug */
    traparg2 = usrsp[ARG_ARG2];	/* if traptype is bottom of stack */
#ifdef RP1
    traparg3 = usrsp[ARG_ARG3];
#endif    

    switch (traptype) {
	
      case EMT_TICKS:
	return(clock);
	
      case EMT_PUTCHAR:
	putchar(traparg1);
	break;
	
      case EMT_VERSION:
	return(MAJORVERSION*256+MINORVERSION);
	
      case EMT_GETCHAR:
	return(getchar());
	
      case EMT_GETMEMSIZE:
#if defined(PAN) && !defined(XX)
 	return(memsize - flh_log_size());
#else defined(PAN) && !defined(XX)
 	return(memsize);
#endif defined(PAN) && !defined(XX)
      case EMT_SETECHO:
	echoing = traparg1;	/* set true or false */
	break;
	
      case EMT_GETCONFIG:
	/*
	 * Return the current configuration register:
	 */
#if defined(PAN) || defined(XX) || defined(SYNALC)
	/* Handle Pancake configuration register in NVRAM */
	conf = *(u_short *)config_magic_ptr;
	/* Is magic value in NVRAM ok? */
	if (conf == CONFREG_MAGIC)
	    /* Yes, use NVRAM config value */
	    conf = *(u_short *)config_ptr;
	else
	    /* No, set configuration to default value */
	    conf = DEFAULT_CONFIG;
	conf = ~conf;
#endif
#if defined(CSC2) 
	/* Use CSC/[23] config register */
	conf = ~*(u_short *)&sys_status_reg->config;
#endif
#if defined(RP1)
	conf = read_nv_short(config_magic_ptr);
	/* Is magic value in NVRAM ok? */
	if (conf == CONFREG_MAGIC) {
	    /* Yes, use NVRAM config value */
	    conf = read_nv_short((ushort *)config_ptr);
	}
	else {
	    /* No, set configuration to default value */
	    conf = DEFAULT_CONFIG;
        }
	conf = ~conf;
#endif
	return(conf);
	
      case EMT_PROCTYPE:
	if (ciscopro_flag != 0) {
	    return((long)(cpu_type | CPU_CISCOPRO_MASK));
	} else {
	    return((long)cpu_type);
	}
	
      case EMT_RELOAD:
#ifdef RP1
	reset_autoboot();	
#endif	
	restart(EXC_RELOAD, 0, 0, 0);
	break;
	
      case EMT_BOOTIPADDR:
	return (bootaddr);
	
      case EMT_VSTRING:
	return ((long) ((!config.oem) ? oemversion : ciscoversion));
	
      case EMT_VSTRING_SYS:
	return (long) vstring_sys;

    case EMT_VSTRING_SYS_SET:
#ifdef RP1
	boot_satisfied = TRUE;
	reset_autoboot();
#endif
	strncpy(vstring_sys, (char*) traparg1, sizeof(vstring_sys));
	return(TRUE);

      case EMT_VSTRING_SYS_ROM:
	return (long) vstring_sys_rom;

      case EMT_VSTRING_SYS_ROM_SET:
	if (vsr_set == 0) {
	    vsr_set = 1;
	    strncpy(vstring_sys_rom, (char*) traparg1,
		    sizeof(vstring_sys_rom));
	} 
	return(TRUE);

      case EMT_CLOCKADDR:
	return ((long)&clock);
	
      case EMT_TOUCHR:
#ifdef BUSERROR
	buserrcnt = 0;
	buserrhandler = buserrnote;
	oldstate = abortstate;
	abortstate = ABORT_INTERNAL;
	/*
	 * Try byte reference first.  If that fails, try short
	 * reference if address is on even boundary.
	 * We must reference the possibly non-existing locations
	 * with an assembly language routine so that our bus
	 * error handler routine can be simple.
	 */
	/*		traparg2 = *(u_char *)traparg1; */
	traparg2 = btouchr(traparg1);
	if (buserrcnt != 0) {
	    if ((traparg1 & 1) == 0) {
		buserrcnt = 0;
		/*			traparg2 = *(u_short *)traparg1; */
		/* call a routine to do this */
		traparg2 = stouchr(traparg1);
	    }
	}
	abortstate = oldstate;
	buserrhandler = 0;
#if defined(MC68020) || defined(MC68040)
	if (cpu_type != CPU_CANCUN) { /* avoid ASIC Bug on cancun */
				      /* where reading ADRSPC_CONTROL can */
				      /* put us into boot mode. */
	    conf = *((volatile u_short *)ADRSPC_CONTROL); /* clear any errors */
	}
#endif
	return (buserrcnt == 0);
#else
	return(FALSE);
#endif
	
      case EMT_LASTERROR:
	return ((long)bootstuff);
	
      case EMT_ROMADDR:
	return ((long)prom2);
	
      case EMT_HCLOCKADDR:
	return ((long)&hclock);
	
      case EMT_TOUCHW:
#ifdef BUSERROR
	buserrcnt = 0;
	buserrhandler = buserrnote;
	oldstate = abortstate;
	abortstate = ABORT_INTERNAL;
	/*
	 * Try byte reference first.  If that fails, try short
	 * reference if address is on even boundary.
	 * We must reference the possibly non-existing locations
	 * with an assembly language routine so that our bus
	 * error handler routine can be simple.
	 */
	/*		*(u_char *)traparg1 = traparg2; */
	btouchw(traparg1, traparg2);
	if (buserrcnt != 0) {
	    if ((traparg1 & 1) == 0) {
		buserrcnt = 0; 
		/*			*(u_short *)traparg1 = traparg2; */
		stouchw(traparg1, traparg2);
	    }
	}
	abortstate = oldstate;
	buserrhandler = 0;
#if defined(MC68020) || defined(MC68040)
	if (cpu_type != CPU_CANCUN) { /* avoid ASIC Bug on cancun */
				      /* where reading ADRSPC_CONTROL can */
				      /* put us into boot mode. */
	    conf = *((volatile u_short *)ADRSPC_CONTROL); /* clear any errors */
	}
#endif
	return (buserrcnt == 0);
#else
	return(FALSE);
#endif
	
      case EMT_GETCAP:
	/*
	 * as of 9.1 we don't do this anymore
	 */
	return(-1);
	
      case EMT_SETCAP: {
	  struct capabilities* ncp;

	  ncp = (struct capabilities*) traparg1;
	  if (ncp->length == SYSCAP_LENGTH) {
	      /*
	       * Set capability list from pointer:
	       */
	      switch (ncp->version) {
		case SYSCAP_VERSION:
		  caps = *ncp;
		  return(TRUE);
	      }
	  }
	  return(-1);
      }
	
      case EMT_GETCHAR_RAW:
	/*
	 * Return an unedited, unmodified character from the console
	 */
	return(getch());

      case EMT_RESET_IO:
	return(reset_io());

#if defined(PAN) || defined(XX) || defined(SYNALC)
      case EMT_GET_IOBASE:
	return(iomem_base);

      case EMT_GET_REV:
	return(revision);

      case EMT_GET_NVRAM_SIZE:
	return(nvram_size);
#endif

#ifdef XX
      case EMT_GET_IOSIZE:
        return(shmemsize);
#endif

#if defined (PAN) || defined (SYNALC)
      case EMT_GET_IOSIZE:
        return(iomem_size);
#endif

      case EMT_PROCNAME:
	return((int)cputypes[cpu_type]);
      case EMT_GET_BOOTINFO:
	return((int)boot_information);
      case EMT_BOOT_CONFREG:
	return((int)((~(*(u_short *)&config)) & 0xffff));

#if defined(PAN) || defined(XX) || defined (SYNALC)
      case EMT_DISABLE_DCACHE:
	dcacheoff();
        return(0);   
      case EMT_ENABLE_DCACHE:
	dcacheon();
        return(0);   
#endif
#ifdef RP1
    case EMT_GET_POWERFAIL_ADDR:
	return((int)&pf_info);
#endif
#ifdef NOTYET
    case EMT_SIMBRK:
	return(simbrk());
#endif
#if defined (PAN) || defined (SYNALC)
      case EMT_GET_COOKIE:
	return((ulong)cookie);
#endif

      case EMT_SETCACR:
	return(setcacr(traparg1));

      case EMT_WRITE_BAD_MAIN_MEMORY_PARITY:
	return(write_bad_main_memory_parity(traparg1, traparg2));

#if defined(PAN) && !defined(XX)
      case EMT_FLASH_LOAD_HELPER:
        {
            /*
	     * traparg1 = INDEX of type of FLH command
             * traparg2 = pointer to flash_load_helper struct
             */
            /* these become static since we futz with the stack below */
            static struct flash_load_helper *helper;
            static char *p, *c;
            static int flags;

	    switch (traparg1) {
		struct _flh_log_routines *flh_routine_ptr;

	      case FLH_GET_VERSION:
		/* return the version of this FLH emt call */
                return(FLH_VERSION_WITH_RCP);
		
	      case FLH_GET_PARMBUFF_LEN:
		return(FNAME_LEN);

	      case FLH_GET_LOGBUFF_LEN:
		return(flh_log_size());

	      case FLH_DO_IT:
		flags = BOOTRUNIMAGE;
		helper = (struct flash_load_helper *) traparg2;

		c = helper->src_filename;
		for (p = bootfilename ; *c; c++, p++){
		    *p = *c;
		}
		*p++ = ' ';
		c = helper->host;
		for (; *c; c++, p++){
		    *p = *c;
		}
		*p++ = ' ';
		c = helper->dst_filename;
		for (; *c; c++, p++){
		    *p = *c;
		}
		*p = '\0';
		if (helper->flags & HELPER_TFTP_COPY_FLASH) {
		    flags |= BOOTCOPYTFTPFLASH;
		}
		else if (helper->flags & HELPER_RCP_COPY_FLASH) {
		    flags |= BOOTCOPYRCPFLASH;
		}
		else if (helper->flags & HELPER_MOP_COPY_FLASH) {
		    flags |= BOOTCOPYMOPFLASH;
		}
		if (helper->flags & HELPER_FLASH_ERASE) {
		    flags |= BOOTERASEFLASH;
		}
		if (helper->flags & HELPER_IGNORE_CONFIG_BITS) {
		    flags |= BOOTIGNORECONFIGBITS;
		}
		reset_io(); /* disable all i/o devices */
		/* disable all interrupts (except nmi -- watchdog) */
		sil(7);
		/* restore to original system vectors */
		restore_vectors();
		reset_stack(); /* need to clean up stack */
		bootstuff[BOOT_ADDRESS] = 0;
		bootloop_counter = 0;
		while(bootloop_counter++ < 2 ) {
		    bootload(bootfilename, flags);
		    reset_io(); /* disable all i/o devices */
		    /* disable all interrupts (except nmi -- watchdog) */
		    sil(7);
		    /* restore to original system vectors */
		    restore_vectors();
		    reset_stack(); /* need to clean up stack */
		    if (bootstuff[BOOT_ADDRESS] == (ROMBOOT_REQUEST - 1)) {
			printf("\n...*** BOOTING ROM IMAGE ***\n");
			restart(EXC_RESET, 0, ROMBOOT_REQUEST,0);
		    }
		}
		/*
		 * ok inspite of our best attempts we just failed to
		 * load the new image.  so we're just going to do
		 * a reload and try to normally reboot the system.
		 */
		restart(EXC_RELOAD, 0, 0, 0);
		break;

	  case FLH_GET_ROUTINES:
		/* traparg2 is pointer to struct _flh_log_routines */

		flh_routine_ptr = (struct _flh_log_routines *)traparg2;

		/* now return pointers to our logging functions. */
		flh_routine_ptr->flh_write_char = (void (*)(char)) flh_putc;
		flh_routine_ptr->flh_print_output = (void (*) ()) flhbuf_print;
		flh_routine_ptr->flh_clear_buff = 
		    (void (*)()) clear_flh_buffer;
		return((int)flh_routine_ptr);

	      default:
		return(-1);
	    }
	}
	break;
#endif /* defined(PAN) && !defined(XX) */

#ifdef RP1
      case EMT_SWCONFREG_SUPPORT:
	return(TRUE);

      case EMT_SETVAR:
        /* 
         * set an nvram environment variable
         */	
	return(setmonvar(traparg1));
	
      case EMT_GETVAR:
        /* 
         * retrieve an nvram environment variable
         */	
	return(getmonvar(traparg1, traparg2, traparg3));
#endif

      default:
	return(-1);
    }
    return(0);
}
