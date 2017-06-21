/* $Id: c1000.c,v 3.4.52.3 1996/08/28 13:10:39 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-36-c1000/c1000.c,v $
 *------------------------------------------------------------------
 * c1000.c - General routines for C1000 based platforms
 *
 * November 1994, David Coli
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c1000.c,v $
 * Revision 3.4.52.3  1996/08/28  13:10:39  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.52.2  1996/07/19  01:19:01  snyder
 * CSCdi63448:  somethings can be said without stars
 *              140 bytes
 * Branch: California_branch
 *
 * Revision 3.4.52.1  1996/03/18  21:57:08  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.18.2  1996/03/09  05:11:22  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.4.18.1  1996/01/24  22:49:55  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1995/12/03  18:00:08  thille
 * CSCdi44905:  ciscopro targets not in 11.1
 * Put them in.
 *
 * Revision 3.3  1995/11/17  18:39:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:29:33  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:19:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/10/16  22:57:38  snyder
 * CSCdi42258:  spellink errors
 *              overwritting -> overwriting
 *
 * Revision 2.4  1995/09/23  05:58:08  enf
 * CSCdi39189:  100X need insurance against loading incorrect image
 *
 * Revision 2.3  1995/09/05  21:39:58  enf
 * CSCdi39019:  <SYSTEM OK> LED off after booting
 * move LED initialization to after Exception setup
 *
 * Revision 2.2  1995/06/16  04:29:36  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.1  1995/06/07  22:52:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../../boot/cpu.h"
#include "stacks.h"
#include "../os/platform_clock.h"
#include "../les/if_les.h"
#include "config_register.h"
#include "../os/hostname.h"	/* for prototype of set_hostname() */
#include "sys_registry.h"
#include "../dev/monitor1.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */

/*
 * Local Storage
 */

static boolean badimage = FALSE; /* bad image being run on box. */

/*
 * Externs
 */

extern void (*nmi_handler)(void);


/*
 * Determine if current image is valid for this platform.
 */
static boolean c1000_valid_image (void)
{
    /*
     * Don't make any assumptions about vendor image names.
     * Assume any image is valid.
     */
    if (cookie->vendor != VENDOR_CISCO)
        return(TRUE);

    switch (cpu_type) {
      case CPU_SAPPHIRE:
            switch (cookie->interfaces) {
              case INTERFACE_1E1B:      /* Sapphire */
              case INTERFACE_1E1N:      /* Frankenstorm */
                if (ciscopro_platform)
                    return(instring("1003", cw_table[CW_TABLE__CW_FAMILY].ptr));
                else {
                    if (instring("1000", cw_table[CW_TABLE__CW_FAMILY].ptr))
                        return(TRUE);
                    else
                        return(instring("1003", cw_table[CW_TABLE__CW_FAMILY].ptr));
                }
                break;
              case INTERFACE_1E1T:      /* Catwoman */
                return(instring("1005", cw_table[CW_TABLE__CW_FAMILY].ptr));
            }
      default:
        return(FALSE);
    }
}

/*
 * image_checker
 * if bad image is running 
 *  for the first 5 minutes errlog message (then once every 5 minutes)
 *  set prompt to 'Running_Invalid_Image'
 */
static void image_checker (void)
{
    static int min_count = 0;
	if (badimage) {
	    if (!bootstrap_enable) {	/* ignore validation for bootstrap */
		set_hostname("Running_Invalid_Image");
		reg_invoke_hostname_changed(FALSE);
		if (min_count++ < 5 || (min_count % 5 == 0)) {
		    errmsg(&msgsym(INVALID_IMAGE, SYS));
	    }
	}

    }
}

/*
 * c1000_ok_to_write_nv_check
 * determine if it is ok for us to write flash.  If the image we're 
 * running is invalid then print a message and ask for user confirmation.
 * Also suggest that the user save the contents on NVRAM on a tftp server
 * if possible.  This is the endpoint of a registry call from the 
 */
static boolean c1000_ok_to_write_nv_check (void)
{
    if(badimage) {
	printf("Warning: **Running an invalid image for this platform**\n");
	printf("Writing a configuration to memory will *lose* some"
	       " configuration\ncommands.\n\n");
	printf("If possible, you might wish to write the present config "
	       "to a local\ntftp server via the "
	       "\"copy startup-config tftp\" command before\n"
	       "overwriting NVRAM.\n\n");
	if (confirm("OK to overwrite config "
		    "(losing WAN config, etc.)?")) {
	    return(0);
	} else {
	    return(1);
	}
    } else {
	return(0);
    }
}
/*
 * c1000_post_exception_init();
 * 
 * Some initialization needs to be done after the exception handling
 * has been set up.  This should be done in c1000_post_exception_init()
 */
void c1000_post_exception_init (void)
{
    quicc_dpr_t *dpr;

    dpr = (quicc_dpr_t *) getmbar();

    /* 
     * Now safe to turn on OK led.  The system image has
     * now establised it's own NMI handler (which is used by
     * the ROMMON to flash the LED) and thus writing the led 
     * NOW will NOT be overwritten by the ROMMON LED flashing code.
     */
    C1000_RUN_LED_ON(dpr);
    if (!bootstrap_enable) {	/* ignore image validatio test for bootstrap */
	if (!c1000_valid_image()) {
	    console_message("\n\nINVALID IMAGE FOR THIS PLATFORM");
	    badimage = TRUE;
	}
    }
}



/*
 * c1000_init_pit_timer
 */

void c1000_init_pit_timer (void)
{
    quicc_dpr_t *dpr;

    createlevel(QUICC_IPL_LEVEL7, NULL, "Programmable Interval Timer");
    nmi_handler = c1000_pit_isr; /* Set pointer to our NMI handler */
    dpr = (quicc_dpr_t *) getmbar();
    quicc_pit_init(dpr, REFRESHTIME, NULL, PIT_VECTOR_NUM);
}


/*
 * c1000_pit_interrupt - Perform C1000-specific clock interrupt handling
 *
 * This routine is called at level 7 by the clock interrupt handler which
 * goes off every 4 Ms.  While booting we will flash the OK led via
 * this routine and we perform break checking here.
 */
 
 
#define LED_OFF_TIME    50      /* 200 Ms */
#define LED_ON_TIME     25      /* 100 Ms */
#define LED_ON          1
#define LED_OFF         0
 
boolean c1000_pit_interrupt (void)
{
    quicc_dpr_t *dpr;
    static int led_status = LED_OFF;
    static int led_count = LED_OFF_TIME;
 
    dpr = getmbar();            /* Get the DPR pointer */
 
    QASSERT(dpr != NULL);
    /*
     * Keep the watchdog timer happy.
     */
    dpr->regs.sim_swsr = QUICC_SWSR_VAL1;
    dpr->regs.sim_swsr = QUICC_SWSR_VAL2;
 
 
    if (bootstrap_enable || badimage) {
        if (--led_count < 0) {
            switch (led_status) {
              case LED_OFF:             /* led was off */
                led_count = LED_ON_TIME;
                C1000_RUN_LED_ON(dpr);
                led_status = LED_ON;
                break;
              case TRUE:                /* led was on */
              default:          /* unknown state assume on */
                led_count = LED_OFF_TIME;
                C1000_RUN_LED_OFF(dpr);
                led_status = LED_OFF;
                break;
            }
        }
    }
 
    /*
     * Check for break key
     */
 
    if (dpr->regs.smc_regs[CONSOLE_SMC_IDX].smc_smce & QUICC_SMCE_BRK) {
 
        /*
         * Acknowledge the break
         */
 
        dpr->regs.smc_regs[CONSOLE_SMC_IDX].smc_smce = QUICC_SMCE_BRK;
 
        if ((!(configregister & CFG_BREAK)) ||
            CLOCK_IN_STARTUP_INTERVAL(BREAK_BOOT_WINDOW))
          return(TRUE);         /* Do break handling now */
    }
    return(FALSE);              /* No break handling right now */
}

/*
 * platform_print_memory
 *
 */

static void platform_print_memory (void)
{
    if (nvsize)
	printf("%dK bytes of non-volatile configuration memory.\n",
               (mon_get_nvram_size()/1024));
}

void c1000_add_registries(void)
{
    reg_add_print_memory(platform_print_memory, "platform_print_memory");
    reg_add_onemin(image_checker,"image_checker");
    reg_add_ok_to_write_nv_check(c1000_ok_to_write_nv_check,
				 "c1000_ok_to_write_nv_check");
}
