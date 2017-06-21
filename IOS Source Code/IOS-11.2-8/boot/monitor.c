/* $Id: monitor.c,v 3.2.58.2 1996/05/10 23:17:05 gstovall Exp $
 * $Source: /release/112/cvs/Xboot/monitor.c,v $
 *------------------------------------------------------------------
 * monitor.c -- command parser for monitor software
 *
 * August 1986, Greg Satz
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: monitor.c,v $
 * Revision 3.2.58.2  1996/05/10  23:17:05  gstovall
 * CSCdi56792:  Need to remove unused files
 * Branch: California_branch
 * The beast still exists, finish killing off the CS500, and while I am in
 * there, nuke the even crustier CSC1.
 *
 * Revision 3.2.58.1  1996/05/09  13:59:59  rbadri
 * Branch: California_branch
 * Synergy features
 *
 * Revision 3.2.88.1  1996/04/27  06:12:58  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.78.1  1996/04/08  01:34:19  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.64.1  1996/03/22  09:06:52  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.50.1  1996/03/03  21:18:57  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.48.1  1996/03/02  01:06:36  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.32.1  1996/02/29  07:41:55  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.2  1995/11/17  07:41:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.62.1  1996/02/03  06:11:37  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.44.1  1995/10/03  07:04:36  rlowe
 * Re-port IOS Boot for Synergy ATM line card from 10.3 to 11.0.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1  1995/06/07  19:12:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This is the main routine that determines why we are here
 * and parsing of input commands.
 *
 * This monitor will perform the following functions:
 *
 *	$		toggle cache state
 *	a [address]	load mac layer address into serial cookie (hidden)
 *	b [filename]	bootload [filename] and start it
 *	c [address]	continue [optional address]
 *	d / s l v	deposit value v of size s into location l
 *	e / s l		examine location l with size s
 *	f [filename] [address] load file into flash and boot it.
 *	g [address]	start up excution
 *	h		offer help with commands
 *	i		initialize
 *	k		stack trace
 *	l [filename]	bootload filename but do not start it
 *	o		show configure register options
 *	p [address]	set break point
 *	s		single step next instruction
 *	t function	test with option device function
 *
 * For the d and e commands:
 *	size s can be b for byte, w for word, and l for long, or
 *	s can represent type m for segment maps or p for page maps.
 *	location l can be a legal register name, hex location
 */

#include "defs.h"
#include "stdio.h"
#include "ctype.h"
#include "cpu.h"
#include "statreg.h"
#include "prom2.h"
#include "reason.h"
#include "extern.h"
#include "save_stack.h"
#include "flh.h"
#include "flh_log.h"

#if defined (PAN) /* defined (SYNALC) */

 extern cookie_t *cookie;
 extern uchar *syn_leds;
#endif

extern long bootload();
extern void dinstr(), options(), sum(), strtoupper();
#if defined (PAN) /* defined (SYNALC) */
extern void write_mac();
#endif

monitor(r_d0, r_d1, r_d2, r_d3, r_d4, r_d5, r_d6, r_d7,
	r_a0, r_a1, r_a2, r_a3, r_a4, r_a5, r_a6, r_ss,
	r_us, cause, r_sr, r_pc)
    long r_d0, r_d1, r_d2, r_d3, r_d4, r_d5, r_d6, r_d7,
	r_a0, r_a1, r_a2, r_a3, r_a4, r_a5, r_a6, r_ss,
	r_us, cause, r_sr, r_pc;
{
    register char *c, *p;
    register long goadx;
    char *comm;
    short confbootfile;
    long (*calladx)() = (long (*)())0;
    char old_abortstate;
    register char processor_version;
    long error_address;
#ifdef PAN
    uchar syn_state = SYNOPTICS_LED_ALL;
#endif
    
#ifdef	lint
    r_d1 = r_d1;	r_d2 = r_d2;	r_d3 = r_d3;	r_d4 = r_d4;
    r_d5 = r_d5;	r_d6 = r_d6;	r_d7 = r_d7;
    r_a0 = r_a0;	r_a1 = r_a1;	r_a2 = r_a2;	r_a3 = r_a3;
    r_a4 = r_a4;	r_a5 = r_a5;	r_a6 = r_a6;
    r_ss = r_ss;
#endif
    
#ifdef  SYNALC
    PUNCH_WD ;          /* SYNALC watchdog is always enabled. */
#endif

    /*
     * Make sure we echo characters here
     */
    if (cause != EXC_EMT) {
	echoing = TRUE;
	init_console();	/* Make sure the UART is set up OK */
#if defined(PAN) && !defined(XX)
	init_flh_buffer((uchar *)memsize);
#endif /* defined(PAN) && !defined(XX) */
    }

    /*
     * Set up the boot file number
     */
    confbootfile = ((~config.bootfile) & 0xF);

    /*
     * Get the correct error address
     */
#ifdef CSC2
    processor_version = sys_status_reg->version;
    if (processor_version == VER_CSC4) 
       error_address = ERROR_ADDRESS_040;   /*Specific to csc/4 */
    else
       error_address = ERROR_ADDRESS;       /*Global to everyone else */
#else
       error_address = ERROR_ADDRESS;       /*Global to everyone else */
#endif

#ifdef RP1
	if (cause != EXC_EMT)
	    if (check_stack_corruption(FALSE))
		printf("\nrom monitor stack may have been corrupted %d\n", 
		       cause);
#endif
    /*
     * Determine why we got here.  On a reset, the version string
     * has already been printed out.  If we get here, much of our
     * memory works; indicate this by printing out the memory size.
     */
    switch (cause) {

      case EXC_RESET:
#ifdef SYNALC
	printf("%s processor with %d (%u) Kbytes of main memory\n",
		cputypes[cpu_type], (memsize/1024), (true_memsize/1024));
#else
	printf("%s processor with %d Kbytes of main memory\n",
	        cputypes[cpu_type], (memsize/1024));
#endif
	if (!prom2)
	    puts(noprom2);
	else if (*prom2 == PROM2_MAGIC2 || *prom2 == PROM2_MAGIC3) 
	    sum();
	r_us = ADRSPC_RAM + memsize;	/* reset user stack pointer */
	
	/*
	 * Check to see what action we need to perform on powerup.
	 * If jumpers are set to boot the default ROM system image
	 * or are set to netboot a "cisco<NN>-csc<[12]>" system image,
	 * attempt to boot the requested system image:
	 */
	if (confbootfile) {
	    abortstate = ABORT_EXTERNAL;
	    /*
	     * If config.romboot (bit 13) is strapped, attempt to
	     * netboot up to THRESHOLD times and then boot the default
	     * ROM software. Otherwise, keep netbooting forever.
	     * Note that we use the Buserror Address argument to
	     * restart() to keep our retry count:
	     */
	    /* 
	     * Alternatively, regardless of the jumper setting, if
	     * bootstuff[BOOT_ADDRESS] contains ROMBOOT_REQUEST,
	     * boot the default ROM software.  This is currently used
	     * to allow user control of the action after a failed
	     * boot attempt on a box with Flash EPROM without
	     * necessitating jumper changes.  This code should be
	     * revisited for a more elegant manner of doing this,
	     * when time permits.
	     */
	    if ((config.romboot == 0 &&
		 bootstuff[BOOT_ADDRESS] == THRESHOLD) ||
		(bootstuff[BOOT_ADDRESS] == ROMBOOT_REQUEST)) {
		
		/*
		 * If we have attempted to boot and failed THRESHOLD
		 * times, attempt to boot the default ROM software:
		 */
		/*
		 * Or, we really want to boot the roms, no matter the
		 * jumper setting.  See above.
		 */
		confbootfile = 1;  /* boot rom software */
		bootstuff[BOOT_ADDRESS] = 0;  /* restart */
	    }
            if (bootload(makebfile(confbootfile), BOOTRUNIMAGE) == -1) {
                /* if we should just give up and go to the ROM MON... */
                bootstuff[BOOT_ADDRESS] = ROMBOOT_REQUEST - 1;
            }
	    restart(EXC_RESET, 0, bootstuff[BOOT_ADDRESS]+1,0);
	}
	/* FALL THROUGH */
	
      case EXC_EXIT:
	break;
	
      case EXC_ABORT:
	puts("\nAbort");
	goto printpc;
	
      case EXC_BREAK:
	takebreak(&r_pc);
	break;
	
      case EXC_TRACE:
	r_sr &= ~SR_TRACE;
	if (breakf) {		/* break in progress */
	    /* this is single step past broken instruction */
	    breakf = FALSE;
	    if (*breakaddr == breakvalue)
		/* instruction still good since we got it */
		*breakaddr = 0x4E41;  /* Trap 1 */
	    return;		/* to user code */
	}
	puts("\nTrace trap");
	goto printpc;
	
      case EXC_EMT:
	r_d0 = emulate(&r_pc, r_sr, (long *)r_us);
	return;			/* to user code */
	
      case EXC_BUSERR:
	decodebe(r_sr);
	goto printadd;
	
      case EXC_ADRERR:
	printf("Address");
      printadd:
	   printf(" Error, address: %#x", error_address);
      printpc:
	printf(" at %#x (PC)\n", r_pc);
	break;
	
      case EXC_PARITY:
	printf("Parity");
	goto printadd;
	
#ifdef XX
      case EXC_SHMEM_PE:  /* shared memory parity error on XX */
	decodeshmempe();    /* this resides in decodebe.c */
	break;

      case EXC_UNEXP_INT:
	printf("\nunexpected vectored interrupt (vector offset %#x)",error->offset);
	break;
#endif

      default:
	printf("\nException: %s", cause);
	goto printpc;
    }

#ifdef  SYNALC
    PUNCH_WD ;          /* SYNALC watchdog is always enabled. */
#endif
    /*
     * If we have a bootfile and we got here by an error, reboot.
     * This depends on EXC_RESET being not equal to any
     * exception message.
     */
    flush();		/* Make sure that the UART is drained */

#ifdef	SYNALC
        /* Restart if Debug jumper not strapped (ie. no Debug TTY available).*/
        /* If there is a problem with IOS kernel image, may also need to */
        /* scratch it so that Primary Boot will re-download image.       */
    if (* (volatile u_char *) (ACP_HW_IOREG_BASE + ACP_RDRG_DEBUG)
                                                & (1 << ACP_RRB_JMP_DEBUG_))
#endif

    if (confbootfile && (cause != EXC_RESET) && (cause != EXC_ABORT)) {
#ifdef RP1
	/*
	 * Image has crashed.  If "boot_satisfied" has not been set,
	 * then it is probably a corrupt image and we don't want to
	 * boot it again and get stuck in a boot loop.
	 */
	if (!boot_satisfied) {
	    /*
	     * If it was an rxboot image that crashed, then try using
	     * the default ROM image instead.  Otherwise, restart()
	     * and hopefully continue the autoboot() sequence.
	     */
	    if (system_state == SYSTATE_BOOTING) {
		printf("Error in rxboot, trying ROM\n");
		default_rxboot();
	    } else {
		printf("Error during boot, resetting...\n");
	    }
	    restart(EXC_RESET, 0, bootstuff[BOOT_ADDRESS]+1,0);
	} else {
	    reset_autoboot();
	}
#endif    
	restart(cause, r_pc, 
		((cause == EXC_ADRERR) || (cause == EXC_BUSERR)) ?
		error_address : 0, save_stack(r_a6));
    }   
    
#ifdef PAN
	if (cookie->vendor == VENDOR_SYNOPTICS) {
	    CLR_SYNOPTICS_LEDS(syn_leds, SYNOPTICS_LED_ALL);
#ifndef XX
	    SET_SYNOPTICS_LEDS(syn_leds, SYNOPTICS_LED_STEP_23 | 
			                 SYNOPTICS_LED_BOOT);
#endif
	}
#endif 

    for (;;) {

#ifdef  SYNALC
        PUNCH_WD ;              /* SYNALC watchdog is always enabled. */
#endif

	old_abortstate = abortstate;
	abortstate = ABORT_IGNORE;
	puts(">");
	preserve_case=TRUE;     /* leave the input stream the way it is */
	gets(bufu.buf);
	preserve_case=FALSE;
	abortstate = old_abortstate;
	
	c = bufu.buf;
	while (*c && isspace(*c))
	    c++;		/* skip leading spaces */
	if (*c == '\0')
	    continue;
#ifdef RP1
	/*
	 * Check if 'c' is a RP1 specific command. 
	 */
	if (consume_rp1_command(c))
	    continue;
#endif
	switch(*c) {
	case 'B': case 'b':
	case 'L': case 'l':
	    break;
	default:
	    strtoupper(c);
	    break;
	}
	comm = c;
	/* find command argument if present */
	while (*c && !isspace(*c) && *c != '/')
	    c++;		/* skip rest of command */
	while (*c && isspace(*c))
	    c++;		/* skip leading spaces */
	
	switch (*comm) {

#ifdef MC68040
	  case '$':		/* cache toggle */
            setcache(c);
	    break;
#endif
#ifdef MC68020
	  case '$':		/* cache toggle */
#if defined(XX) || defined(PAN) || defined (SYNALC)
	    setcache(c);
	    break;
#else
# ifdef CSC2
            processor_version = sys_status_reg->version;
            if (processor_version == VER_CSC4) 
                setcache(c);
            else {
                 if (cache)          /* Instruction cache in 68020 */
	             cacheoff();
	         else
		     cacheon();
	         cache = !cache;
	         printf("Cache is %s\n", cache ? "enabled" :
		        "disabled");
                 }
	    break;
# else
                 if (cache)          /* Instruction cache in 68020 */
	             cacheoff();
	         else
		     cacheon();
	         cache = !cache;
	         printf("Cache is %s\n", cache ? "enabled" :
		        "disabled");
	    break;
# endif CSC2
#endif XX
#endif MC68020

#ifdef PAN
	  case 'A':		/* load mac address into serial nvram */
	    /*
	     * This command only works on cristal/cancun platforms. Check 
	     * to see if we're on a cristal and do the command, otherwise, 
	     * treat this as an unknown command
	     */
	    if (cpu_type == CPU_CRISTAL || cpu_type == CPU_CANCUN)
		write_mac(c);
	    else
		puts("What?\n");
	    break;
#endif	    
	  case 'B': case 'b':		/* bootload */
	    abortstate = ABORT_EXTERNAL;
	    if (confbootfile && !*c)
		c = makebfile(confbootfile);
	    else {
		for (p = bootfilename ; *c; c++, p++)
		    *p = *c;
		*p = '\0';
	    }
	    bootload(bootfilename, BOOTRUNIMAGE | BOOTLOADNETBOOTIMAGE);
	    abortstate = ABORT_INTERNAL;
	    break;
	    
	  case 'C':     	/* continue */
	    if (abortstate == ABORT_EXTERNAL) {
		if ((goadx = gethexnum(c)) >= 0)
		    r_pc = goadx;
		if (breakf)  /* break in progess */
		    r_sr |= SR_TRACE; /* set trace trap */
#ifdef RP1
                /* re-enable powerfail handler */
                if (pf_info.pf_addr_save) {
                    pf_info.pf_addr = pf_info.pf_addr_save;
                    pf_info.pf_addr_save = 0;
                }
#endif RP1
		return;
	    }
	    break;
	    
	  case 'D':	        /* deposit */
	    deposit(c, &r_d0);
	    break;
	    
	  case 'E':		/* examine */
	    examine(c, &r_d0);
	    break;

          case 'F':
	    /* test for flh */
	    {
		/* For SYNALC just try to load the flash image */
		/* without being too fancy.                    */
#ifndef	SYNALC
		char *cptr = comm;;
		cptr++;
		/* we've seen 'f' now look for 'l' and 'h' */
		if ((*cptr == 'L') && (*(cptr+1) == 'H')) {
#endif
		    abortstate = ABORT_EXTERNAL;
		    for (p = bootfilename ; *c; c++, p++){
			*p = isupper(*c) ? tolower(*c) : *c ;
		    }
		    *p = '\0';
		    bootload(bootfilename, (BOOTRUNIMAGE | BOOTCOPYTFTPFLASH | 
					    BOOTERASEFLASH));
		    abortstate = ABORT_INTERNAL;
#ifndef	SYNALC
		} else {
		    puts("What?\n");
		}
#endif
		break;
	    }
	    
	  case 'G':     	/* go */
	    if ((goadx = gethexnum(c)) >= 0) {
		if (abortstate == ABORT_EXTERNAL) {
		    r_pc = goadx;
		    return;
#ifdef RP1
                    /* re-enable powerfail handler */
                    if (pf_info.pf_addr_save) {
                        pf_info.pf_addr = pf_info.pf_addr_save;
                        pf_info.pf_addr_save = 0;
                    }
#endif RP1
		}
		calladx = (long (*)())goadx;
	    }
	    if (calladx) {
		abortstate = ABORT_EXTERNAL;
		system_state = SYSTATE_RUNNING;
		(*calladx)(PROM2_BOOTLOAD, NULL);
		system_state = SYSTATE_MONITOR;
		abortstate = ABORT_INTERNAL;
	    }
	    break;
	    

	  case '?':
	  case 'H':		/* help */
	    help();
	    break;
	    
	  case 'I':		/* initialize */
	    restart(EXC_RESET, 0, 0, 0);
	    break;
	    
	  case 'K':		/* stack trace */
	    stackdump(&r_d0);
	    break;
	    
	  case 'L': case 'l':		/* load */
	    if (*c || *prom2 == PROM2_MAGIC2 || *prom2 == PROM2_MAGIC3) {
		for (p = bootfilename ; *c; c++, p++)
		    *p = *c;
		*p = '\0';
		abortstate = ABORT_EXTERNAL;
		calladx = (long (*)())bootload(bootfilename, 0);
		abortstate = ABORT_INTERNAL;
	    } else
		puts(illarg);
	    break;
	    
	  case 'O':		/* conf. reg. options */
	    options(c);
	    break;
	  case 'P':		/* breakpoint */
	    dobreak(c);
	    break;
	  case 'S':		/* single step */
	    dinstr(r_pc);
	    breakf = FALSE;
	    r_sr |= SR_TRACE; /* set trace trap */
	    return;
	  case 'T':		/* test */
	    test(c);
	    break;

#ifdef XX	    
	  case 'M':		/* special memory operations */
	    memops(c);
	    break;
#endif

#if defined(XX) || defined(PAN)
          case 'U':             /* UART binary download */
	    dnld(c);
	    break;
#endif /* PAN */

	  default:
	    puts("What?\n");
	    break;
	}
	
	flush();	/* Make sure that the UART is drained */
    }
}
