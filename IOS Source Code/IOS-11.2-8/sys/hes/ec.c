/* $Id: ec.c,v 3.4.52.5 1996/08/28 12:43:49 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/ec.c,v $
 *------------------------------------------------------------------
 * ec.c --  Environmental Monitor Controller driver
 *
 * 3-Oct-1991, Darren Kerr
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * - handle warning and shutdown interrupts from ENVM
 * - show current ENVM statistics
 * - read/write routines for Flash EPROM
 *------------------------------------------------------------------
 * $Log: ec.c,v $
 * Revision 3.4.52.5  1996/08/28  12:43:49  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.52.4  1996/08/02  22:53:26  smackie
 * Attempt to kill two types.h birds with one stone. (CSCdi64993)
 *
 *    o Add typedefs for size-oriented intX and uintX types
 *    o Rename dev_t to flash_dev_t to avoid near-future conflicts
 *
 * Branch: California_branch
 *
 * Revision 3.4.52.3  1996/06/28  23:06:07  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.4.52.2  1996/05/21  06:29:05  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.4.52.1  1996/03/18  19:40:09  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.20.2  1996/03/07  09:38:46  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.20.1  1996/01/24  21:45:12  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1995/12/01  19:03:35  sdurham
 * CSCdi44362:  envmon is non-modular w.r.t envmon mib.
 *   Remove direct calls from envmon code to mib code. Use service points.
 *
 * Revision 3.3  1995/11/17  09:15:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:26:04  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:38:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/08/21  07:39:53  hampton
 * Transition the hes directory code to use the passive timer macros for
 * all its timers.  It no longer references the system clock directly.
 * [CSCdi39033]
 *
 * Revision 2.5  1995/08/08 16:46:35  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.4  1995/06/28 21:16:45  sdurham
 * CSCdi36261:  CISCO-ENVMON-MIB and OLD-CISCO-ENV-MIB being loaded on ags
 *              improperly.
 *
 * Revision 2.3  1995/06/18  23:16:39  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  13:04:32  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:39:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include "logger.h"
#include "ec.h"
#include "../dev/dev.h"
#include "../dev/flash_private.h"
#include "config.h"
#include "../os/free.h"
#include "../snmp/chassismib_registry.h"
#include "envmib_registry.h"
#include "../hes/envmon_parser.h"
#include "msg_ec.c"			/* Not a typo, see logger.h */

/*
 * looking foward...
 */
static int env_init(void);
static void ec_intr(void);
static int ec_open(int);
static int ec_close(void);
static int ec_ioctl(ulong, int, void *);
static int ec_write(ulong, char *, int);
static int ec_read(ulong, char *, int);
static void env_show_init(ecshow_t *);
static int env_show_parse(ecshow_t *, char *);
static void clr_envintr(void);
static void ec_output(void);
static void ec_input(void);
static int ecioc_flash_cmd(uchar, uchar);
static int ecioc_flash_seek(ulong);
static int ecioc_mbaddr(void);
static void ec_shutdown(uchar);
static int ec_wait_for_nstate(int, ulong);
static boolean ec_nstateBLOCK(ulong);
static forktype env_update_stats(boolean forever);
#ifdef	TEST_FLASH
static void test_flash(parseinfo *csb);
#endif


/* device driver function table (see "dev.h", "dev.c")           */
const devfun_t env_flash_fun =
{
    env_init,       /* Initialize Environmental Controller       */
    ec_open,        /* To get file handle...                     */
    ec_close,       /* To close and decrement ref count          */
    ec_intr,        /* Handle EC exception                       */
    ec_ioctl,       /* Special commands to EC                    */
    ec_read,        /* read entrypoint                           */
    ec_write,       /* write entrypoint                          */
};

extern flash_t flash;
extern flash_dev_t devtable[];

ec_t    *ecp;                    /* Ptr to alloc'd Environmental Ctlr struct*/
boolean ec_present;              /* Used by PrintHardware()                 */
boolean ec_debug = FALSE;        /* for dprintf macro                       */
uint	ec_flash_cmdfails = 0;	 /* count of failed flash commands	    */
boolean envmib_load = FALSE;     /* global to tell mibcode ok to load */
extern void flash_init_ags(void);

#ifdef	TEST_FLASH
/*
 * Parse chain for TEST flash command
 */
#define	ALTERNATE	NONE
#include "../parser/exec_test_flash.h"
LINK_POINT(test_flash_commands, ALTERNATE);
#undef	ALTERNATE
#endif

/***************************************************************************
 *
 *     EC DRIVER's EXTERNAL DEVICE ENTRYPOINTS FROM SYSTEM CODE
 *
 ***************************************************************************/

/*
 * ENV init routine called from flash_init_ags()
 */
int env_init (void)
{
    int     i, level;
    echdr_t req;

    if (ecp)
	return DEV_OK;
    
    if (!touch_device((uchar *)EC_IO))
	return DEVERR_NODEV;
    
    if (!(ecp = malloc(sizeof(ec_t))))
	return DEVERR_ALLOC;
    
    ec_present = TRUE;
    ecp->ec_state = EC_STATE_GETREV;
    ecp->ec_iop = (echdr_t *) EC_IO;
    req.h_ctl = ECCTL_REV;
    req.h_mod =  0x69; /* something to look for on the logic analyzer */
    *ecp->ec_iop = req;

    envmon_parser_init();

    /*
     * Poll up to 2 seconds for revision acknowledge interrupt.
     */
    for (i=0; i<200; i++) {
	/*
	 * At this point the system has not yet enabled interrupts,
	 * but the envm board needs them.
	 */
	level = set_interrupt_level(ALL_ENABLE);
	DELAY(10);
	reset_interrupt_level(level);
	if (ecp->ec_state == EC_STATE_IDLE) {
	    /*
	     * Rev4 supports shared SRAM for faster data transfer.
	     * If the ENVM says its rev 4 and we haven't already
	     * alloc'ed a buffer and tried the assignment, do so now.
	     */
	    if (ecp->ec_hwrev == EC_HWREV_4)
		ecioc_mbaddr();

	    ecp->ec_inited = TRUE;

	    /*
	     * Make env_update_stats() its own process since it may be
	     * blocked for a while.
	     */
	    (void)cfork((forkproc *)env_update_stats, (boolean)TRUE, 0, 
			"ENVM Update", CONSOLE_LINENUM); 
	    envmib_load = TRUE;
	    return(DEV_OK);
	}
    }

    return(DEVERR_NODEV);
}

#define ENVMSLOT_AGSPLUS	1
static void
add_envm_card (void)
{
    if (ecp) {
	char* cp;
	char* hwver;
	char* swver;
	char buf[36];
	uint serial;
	
	cp = buf;
	cp += sprintf(hwver = cp, "%x", ecp->ec_hwrev);
	cp += sprintf(swver = ++cp, "%u.%u",
		      ecp->ec_swrev >> 4, ecp->ec_swrev & 0x0f);
	serial = NO_SERIALNUM;
	if (ecp->ec_show) {
	    uint u;
	    boolean ok;

	    u = parse_unsigned(ecp->ec_show->s_serial_num, &ok);
	    if (ok)
		serial = u;
	}
#ifdef	TEST_FLASH
	parser_add_commands(PARSE_ADD_TEST_CMD, &pname(test_flash_commands),
			    "ec");
#endif
    }
    return;
}

/*
 * Interrupt handler for Environmental Controller.
 *
 * We'll get an interrupt from the EC whenever it places
 * req/ack/data info in the I/O port or shared memory buffer.
 *
 */
void ec_intr (void)
{
    ecp->ec_cnt.intr++;
    ecp->ec_ihdr = *ecp->ec_iop;
    clr_envintr();
    switch (ecp->ec_ihdr.h_ctl) {
    case ECCTL_SHUT:
	ec_shutdown(ecp->ec_ihdr.h_mod);
	break;
    case ECCTL_REV:
	ecp->ec_swrev = ecp->ec_ihdr.h_mod;
	/* Hard code for now. We don't really know... */
	ecp->ec_hwrev = EC_HWREV_4; 
	ecp->ec_state = EC_STATE_IDLE;
	return;
    case ECCTL_BCMD:
#if 0	
	buginf("\nENVM bad command interrupt (sts=%x)",
	       ecp->ec_ihdr.h_mod);
#endif	
	ecp->ec_cnt.bcmd_intr++;
	break;
    case ECCTL_FLASH_RESET:
    case ECCTL_FLASH_INFO:
    case ECCTL_FLASH_READ:
    case ECCTL_FLASH_ZERO:
    case ECCTL_FLASH_ERASE:
    case ECCTL_MBADDR:
    case ECCTL_FLASH_VPP:
	/* Wakeup sleeping ioctl command */
	ecp->ec_state = EC_STATE_IDLE;
	break;
    case ECCTL_CLRINT:
	break;
    default:
	if (ecp->ec_state == EC_STATE_OUTPUT) {
	    ec_output();
	    ecp->ec_cnt.output_intr++;
	} else if (ecp->ec_state == EC_STATE_INPUT) {
	    ec_input();
	    ecp->ec_cnt.input_intr++;
	} else {
	    ecp->ec_cnt.unknown_intr++;
	    ecp->ec_cnt.unknown_ctl = ecp->ec_ihdr.h_ctl;
	    ecp->ec_cnt.unknown_mod = ecp->ec_ihdr.h_mod;
	}
	return;
    }
#if 0    
    /* Re-transmit last CSC REQ/IND if it was lost due to ENV IND */
    if (ecp->ec_state == EC_STATE_OUTPUT) {
	*ecp->ec_iop = ecp->ec_indhdr;
    } else if (ecp->ec_state == EC_STATE_INPUT) {
	*ecp->ec_iop = ecp->ec_reqhdr;
    }
#endif    
}

/*
 * Just make sure that the EC is in a useable state:
 * This means that the Rev request OK (successfully fielded
 * an interrupt from the ENVM).
 */
int ec_open (int flags)
{
    ecp->ec_mbsram = FALSE;
    ecp->ec_indhdr.h_ctl = 0;
    ecp->ec_reqhdr.h_ctl = ECCTL_MON_REQ;
    return DEV_OK;
}

/* Nothing to do here */
int ec_close (void)
{
    return DEV_OK;
}

static void ec_not_responding (void)
{
#if 0    
    static int already = 0;

    if (!already++)
	buginf("\nENV controller not responding to commands.");
    flush();
#endif    
    ecp->ec_cnt.not_responding++;
    ecp->ec_state = EC_STATE_IDLE;
}
	

/*
 * Perform EC-specific action.
 * This is needed to setup for READ or WRITE.
 */
int ec_ioctl (ulong dev, int cmd, void *buf)
{
    int    sts, param;
    
    sts = DEV_OK;
    switch (cmd) {
    case IOC_FLASH_SEEK:
	sts = ecioc_flash_seek(*(ulong *) buf);
	break;
    case IOC_FLASH_VPP:
	/* Need seek to read/write, else VPP drops */
	param = *(int *) buf;
	if ((sts = ecioc_flash_cmd(ECCTL_FLASH_VPP, (uchar) param)) == 0)
	    flash.f_vpp_on = param;
	break;
    case IOC_FLASH_ERASE:
	sts = ecioc_flash_cmd(ECCTL_FLASH_ERASE, (flash.f_chipsize)>>16);
	break;
    case IOC_FLASH_ZERO:
	sts = ecioc_flash_cmd(ECCTL_FLASH_ZERO, (flash.f_chipsize)>>16);
	break;
    case IOC_FLASH_RESET:
	sts = ecioc_flash_cmd(ECCTL_FLASH_RESET, 0);
	break;
    case IOC_FLASH_INFO:
	sts = ecioc_flash_cmd(ECCTL_FLASH_INFO, 0);
	break;
    case IOC_FLASH_READ:
	sts = ecioc_flash_cmd(ECCTL_FLASH_READ, 0);
	break;
    case IOC_LOOPBACK:
	ecp->ec_mbsram = FALSE;
	ecp->ec_indhdr.h_ctl = ECCTL_LOOP_IND;
	ecp->ec_reqhdr.h_ctl = ECCTL_LOOP_REQ;
	break;
    case IOC_MBLOOPBACK:
	if (!ecp->ec_rev4) {
	    printf("Not rev4 hardware\n");
	    sts = -1;
	    break;
	}
	ecp->ec_mbsram = TRUE;
	ecp->ec_indhdr.h_ctl = ECCTL_MBLOOP_IND;
	ecp->ec_reqhdr.h_ctl = ECCTL_MBLOOP_REQ;
	break;
    default:
	sts =  DEVERR_BAD_IOCTL;
    }
    return sts;
}

/*
 * Set up ec_ioc fields needed during output processing, then
 * put in state OUTPUT.
 */
int ec_write (ulong dev, char *buf, int len)
{
    if (len < 1)
	return len;

    ecp->ec_buf = buf;
    ecp->ec_cur = 0;
    if (ecp->ec_indhdr.h_ctl == ECCTL_FLASH_IND)
	len = MIN(len, flash.f_maxseek - flash.f_cur);
    
    ecp->ec_len = len;
	
    /* Start output and sleep until complete, error, or timeout */
    ec_output();
    ec_wait_for_nstate(EC_STATE_OUTPUT, ONEMIN + len);
    
    if (ecp->ec_state != EC_STATE_IDLE) {
	ec_not_responding();
	return -1;
    }
    if (ecp->ec_indhdr.h_ctl == ECCTL_FLASH_IND)
	flash.f_cur += len;
    return ecp->ec_cur;
}

/*
 * Set up ec_ioc fields needed during input processing, then
 * put in state INPUT.
 */
int ec_read (ulong dev, char *buf, int len)
{
    sys_timestamp ms_start;

    if (len < 1)
	return len;

    ecp->ec_cnt.reads++;
    GET_TIMESTAMP(ms_start);
    ecp->ec_buf = buf;
    ecp->ec_cur = 0;
    if (ecp->ec_reqhdr.h_ctl == ECCTL_FLASH_REQ)
	len = MIN(len, flash.f_maxseek - flash.f_cur);
    
    ecp->ec_len = len;
	
    /* Start input and sleep until complete, error, or timeout */
    ec_input();
    ec_wait_for_nstate(EC_STATE_INPUT, 10 * ONEMIN + len);
	
    if (ecp->ec_state != EC_STATE_IDLE) {
	ec_not_responding();
	ecp->ec_cnt.read_errors++;
	return -1;
    }
    ecp->ec_cnt.read_ms += ELAPSED_TIME(ms_start);
    ecp->ec_cnt.read_bytes += ecp->ec_cur;
    return ecp->ec_cur;
}


/***************************************************************************
 *
 *     EXTERNAL NON-DEVICE ENTRYPOINTS INTO DRIVER
 *     (for "show" and "snmp")
 *
 ***************************************************************************/

/*
 * See if we need to print or send snmp warning on any environmental
 * measurement, but send at most one message per day so that we don't
 * irritate those sites that run their equipment at close to margin
 * condition.
 */
static void env_warning_check (void)
{
    int i;
    
    for (i=0; i<ECTESTPTS; i++) {
	if (ecp->ec_show->s_test[i].t_warning[0]) {
	    if (TIMER_RUNNING_AND_SLEEPING(ecp->ec_warndelay[i]))
		continue;
	    /*
	     * We don't want to be inundated with redundant ENVM warnings...
	     */
	    TIMER_START(ecp->ec_warndelay[i], 4*60*ONEMIN);
	    
	    if (i == ECTEST_T_AMB) {
		errmsg(&msgsym(TEMP, ENV),
		       ecp->ec_show->s_test[i].t_measured,
                       ecp->ec_show->s_test[i].t_shut_marg);
		reg_invoke_envmib_temperature_trap(i);
	    } else if (i == ECTEST_T_AIR) {
                errmsg(&msgsym(FLOW, ENV));
		reg_invoke_envmib_temperature_trap(i);
	    } else {
		if (i < ECTEST_V_P5) {   /* skip two reserved ids */
		    continue;
		}
                errmsg(&msgsym(VOLTAGE, ENV),
		       ecp->ec_show->s_test[i].t_description,
		       ecp->ec_show->s_test[i].t_measured,
		       ecp->ec_show->s_test[i].t_shut_marg);
		reg_invoke_envmib_voltage_trap(i);
	    }
	}
    }
}

#define STAT_FLAGS (DEV_FLAGS_NDLY | DEV_FLAGS_EXCL | DEV_FLAGS_QUIET)
/*
 * Queries ENVM for voltage/temperature information.
 *
 * This guy is now its own process.  When it was being called from
 * net_background() it was blocking other processes from being invoked
 * once a minute.
 *
 * The "forever" variable is used so that this function can be used
 * for both the regularly scheduled updates and every time "show env"
 * is typed. 
 */
forktype env_update_stats (boolean forever)
{
    char   *sbuf;
    int    len, id;
    ecshow_t *showp, *tmp;
    
    /*
     * Read measurements from ENVM and store in a buffer accessible
     * by "show env".
     * We select NDLY and EXCL flags on the open, so if we can't get
     * exclusive access to the ENV immediately, we return with failure
     * with no delay (doesnt' block).
     * (The flash routines call open withOUT NDLY, so they will block
     * until this routine is done).
     */
    
    if (ecp->ec_show == NULL)  /* first time */
	add_envm_card();
    
    while (TRUE) {
	ecp->ec_cnt.stat_calls++;
	if ((id = dev_open(DEV_ENV, STAT_FLAGS)) >= 0) {
	    if ((showp = malloc(sizeof(ecshow_t)))) {
		env_show_init(showp);
		if ((sbuf = malloc(ECSHOWBUFLEN))) {
		    if ((len = dev_read(id, sbuf, ECSHOWBUFLEN)) > 0) {
			sbuf[len] = 0;
			sstrncpy(ecp->ec_statbuf, sbuf, ECSTATBUFLEN);
			if (env_show_parse(showp, sbuf) == 0) {
			    tmp = ecp->ec_show;
			    ecp->ec_show = showp;
			    showp = tmp;
			    env_warning_check();
			    ecp->ec_cnt.stat_ok++;
			}
			ecp->ec_cnt.stat_bytes += len;
		    }
		    free(sbuf);
		}
		free(showp);
	    }
	    dev_close(id);
	}
	if (forever) 
	    process_sleep_for(5*ONEMIN);
	else
	    return;
    }
}


/*
 * Malloc's an ecshow_t buffer and copies in the latest and greatest
 * ENVM measurements.
 *
 *         REMEMBER TO FREE YOUR SHOW BUF WHEN DONE !!!
 *
 */
static ecshow_t *env_get_show (void)
{
    int level;
    ecshow_t *showp;
    
    if (!ecp || !ecp->ec_show)
	return NULL;
    
    if (!(showp = malloc(sizeof(ecshow_t)))) {
	return NULL;
    }
    
    /*
     * Mask out interrupts while we copy the struct into our local
     * buffer, so that we can't get stepped under.
     */
    level = raise_interrupt_level(ALL_DISABLE);
    bcopy(ecp->ec_show, showp, sizeof(ecshow_t));
    reset_interrupt_level(level);
    
    return showp;
}

/*
 * pretty picture of last environmental measurements
 */
void show_env (parseinfo *csb)
{
    int i;
    ecshow_t *showp;
    static char* const envm = "Environmental controller";
    static char* const warn = "WARNING! ";

    if (!ecp) {
	printf("%s not found", envm);
	return;
    }
    
    /*
     * If the ENVM is present and hasn't responded to revision
     * request at interrupt level, then it's *probably* old firmware.
     */
    if (ecp->ec_state < EC_STATE_IDLE) {
	printf("Requires %s firmware 2.0 or later\n", envm);
	return;
    }
    
    /*
     * Don't print the hardware version;  we don't REALLY know
     * that the hardware version is correct if the
     * firmware was installed by the customer, since it is hard-
     * coded into the firmware (not the hardware).
     */
    printf("%s firmware version %d.%d",
	   envm, ecp->ec_swrev>>4, ecp->ec_swrev&0xF);

    env_update_stats(FALSE);
    showp = env_get_show();
    if (showp) {
	printf("\n  Serial number is %s, calibrated on %s, by technician %s",
	       showp->s_serial_num,
	       showp->s_burn_date,
	       showp->s_tech_id);
	
	printf("\n%s  Internal temperature measured %s, shuts down at %s",
	       showp->s_test[ECTEST_T_AMB].t_warning[0] ? warn : "",
	       showp->s_test[ECTEST_T_AMB].t_measured,
	       showp->s_test[ECTEST_T_AMB].t_shut_marg);
	
	printf("\n%s  Air flow appears %s.",
	       showp->s_test[ECTEST_T_AIR].t_warning[0] ? warn : "",
	       showp->s_test[ECTEST_T_AIR].t_warning[0] ? "marginal" : "good");
	
	for (i=ECTEST_V_P5; i<=ECTEST_V_N5; i++) {
	    printf("\n%s  %s line measured at %s",
		   showp->s_test[i].t_warning[0] ? warn : "",
		   showp->s_test[i].t_description,
		   showp->s_test[i].t_measured);
	}
    } else {
	printf("\n  Measurements not collected");
    }
    switch (GETOBJ(int,1)) {

      case SHOW_ENV_ALL:
	printf("\nENV:    state=%x hwrev=%x swrev=%x",
	       ecp->ec_state, ecp->ec_hwrev, ecp->ec_swrev);
	printf("\nIOP:    addr=%x LAST: ictl=%x idat=%x CURR: ictl=%x idat=%x",
	       ecp->ec_iop, ecp->ec_ihdr.h_ctl, ecp->ec_ihdr.h_mod,
	       *(uchar *)&ecp->ec_iop->h_ctl, *(uchar *)&ecp->ec_iop->h_mod);
 	printf("\nSTAT:   calls=%d errs=%d perrs=%d bytes=%d",
 	       ecp->ec_cnt.stat_calls,
 	       ecp->ec_cnt.stat_calls - ecp->ec_cnt.stat_ok,
 	       ecp->ec_cnt.parse_errors,
 	       ecp->ec_cnt.stat_bytes);
 	printf("\nREAD:   calls=%d errs=%d bytes=%d millis=%d",
 	       ecp->ec_cnt.reads,
 	       ecp->ec_cnt.read_errors,
 	       ecp->ec_cnt.read_bytes,
 	       ecp->ec_cnt.read_ms);
 	printf("\nINTR:   total=%d write=%d read=%d noresponse=%d",
	       ecp->ec_cnt.intr,
	       ecp->ec_cnt.output_intr,
 	       ecp->ec_cnt.input_intr,
 	       ecp->ec_cnt.not_responding);
 	printf("\n        bcmd=%d unknown=%d (ctl=%x mod=%x)",
 	       ecp->ec_cnt.bcmd_intr,
 	       ecp->ec_cnt.unknown_intr,
 	       ecp->ec_cnt.unknown_ctl,
 	       ecp->ec_cnt.unknown_mod);
 	printf("\nSBUF:   %s", ecp->ec_statbuf);
	break;

      case SHOW_ENV_LAST:
	printf("\n\nLAST Environmental Shutdown Measurements:");
	if (showp->s_test[0].t_shut_last[0] == '\0') {
	    printf("\n  None logged");
	} else {
	    printf("\n  Internal temperature was %s",
		   showp->s_test[ECTEST_T_AMB].t_shut_last);
	    
	    i = strlen(showp->s_test[ECTEST_T_AIR].t_shut_last);
	    i = showp->s_test[ECTEST_T_AIR].t_shut_last[i-1] == '*' ? 1 : 0;
	    printf("\n  Air flow sensor was %s", i ? "FATAL*" : "good");
	    
	    for (i=ECTEST_V_P5; i<=ECTEST_V_N5; i++) {
		printf("\n  %s line was %s",
		       showp->s_test[i].t_description,
		       showp->s_test[i].t_shut_last);
	    }
	}
	break;

      case SHOW_ENV_DEFAULT:
      default:
	break;
    }
    free(showp);
}


/* routines to convert ascii text to numbers for SNMP */
int getenvnumber (char *s, int i)
{
    int retvalue = 0;

    while ((s[i] >= '0') && (s[i] <= '9') && (i < TESTPOINTLEN)) {
        retvalue = (retvalue * 10) + (s[i] - '0');
        i++;
    }
    return(retvalue);
}

int findenvdot(char *s, int i)
{

    while ((s[i] != '.') && (i < TESTPOINTLEN))
        i++;
    return(i);
}

int findenvnumber(char *s, int i)
{

    while ((s[i] != '-') && ((s[i] < '0') || (s[i] > '9'))
           && (i < TESTPOINTLEN))
        i++;
    return(i);
}

int getenvtemp(char *s)
{
    int i;
    int retvalue = 0;

    i = findenvnumber(s, 0);
    retvalue = getenvnumber(s, i);
    return(retvalue);
}

int getvoltfrac(char *s, int i)
{
    int j;
    int retvalue;

    retvalue = 0;
    for (j=1; j<= 3; j++) {
        if ((s[i] >= '0') && (s[i] <= '9') && (i < TESTPOINTLEN))
            retvalue = (retvalue * 10) + (s[i] - '0');
        else
            retvalue *= 10;
        i++;
    }
    return(retvalue);
}

int getenvvoltage(char *s)
{
    int i;
    int retvalue = 0;
    int sign = 1;
    int frac;

    i = findenvnumber(s, 0);
    if (s[i] == '-') {
        sign = -1;
        i++;
    }
    retvalue = getenvnumber(s, i);
    retvalue *= 1000;
    i = findenvdot(s,i);
    i = findenvnumber(s,i);
    frac = getvoltfrac(s,i);

    retvalue += frac;
    return(retvalue * sign);
}


/*
 * 
 */
void env_show_init (ecshow_t *showp)
{
    /* ECMON specific. LATER: update for clean machine */
    sstrncpy(showp->s_devnam, devtable[DEV_ENV].dev_name, 12);
    sstrncpy(showp->s_test[ECTEST_T_AMB].t_description, "ambient", 12);
    sstrncpy(showp->s_test[ECTEST_T_AIR].t_description, "airflow", 12);
    sstrncpy(showp->s_test[ECTEST_V_P5].t_description , "+5 volt", 12);
    sstrncpy(showp->s_test[ECTEST_V_P12].t_description, "+12 volt", 12);
    sstrncpy(showp->s_test[ECTEST_V_N12].t_description, "-12 volt", 12);
    sstrncpy(showp->s_test[ECTEST_V_N5].t_description , "-5 volt", 12);
}


/*
 * Parse through show buffer read from ENV (*s), and
 * assign show pointers to info strings in the show
 * buffer.
 *
 * I know the hard coded constant string lengths are ugly, but I like the
 * extra safety, and it's too tedious to define macros for all the lengths.
 */
int env_show_parse (ecshow_t *showp, char *s)
{
    char   *next, *vp;
    int    index;
    char   *testpt_next(char *);
    int    testpts = 0;
    
    for ( ; *s; s = next) {
	if (!(next = testpt_next(s)))
	    break;
	vp = &s[3];
	if (*s == 'm' || *s == 'S' || *s == 's' || *s == 'W') {
	    /* Up to 36 test points... */
	    if (s[1] >= '0' && s[1] <= '9')
		index = s[1] - '0';
	    else if (s[1] >= 'a' && s[1] <= 'z')
		index = s[1] - 'a' + 10;
	    else
		break;
	    if (index >= ECTESTPTS)
		break;
	    if (*s == 'S')
		sstrncpy(showp->s_test[index].t_shut_last, vp, 12);
	    else if (*s == 's')
		sstrncpy(showp->s_test[index].t_shut_marg, vp, 12);
	    else if (*s == 'm') {
		sstrncpy(showp->s_test[index].t_measured, vp, 12);
		testpts++;
	    } else if (*s == 'W')
		sstrncpy(showp->s_test[index].t_warning, vp, 2);
	} else if (s[0] == 'r') {
	    /* Revision Information */
	    if (s[1] == 'f')
		sstrncpy(showp->s_revfw, vp, 4);
	    else if (s[1] == 'h')
		sstrncpy(showp->s_revhw, vp, 4);
	} else if (s[0] == 'b') {
	    /* Burn-in Information */
	    if (s[1] == 'd')
		sstrncpy(showp->s_burn_date, vp, 12);
	    else if (s[1] == 'n')
		sstrncpy(showp->s_serial_num, vp, 12);
	    else if (s[1] == 'i')
		sstrncpy(showp->s_tech_id, vp, 4);
	}
    }
    if (testpts < ECVALIDTESTS) {
	ecp->ec_cnt.parse_errors++;
	return -1;
    }
    return 0;
}

/***************************************************************************
 *
 *     Output, Input, Request handling functions
 *
 ***************************************************************************/


void clr_envintr (void)
{
    int i, j;
    
    
    for (i=0; i<10; i++) {
	*(uchar *)&ecp->ec_iop->h_ctl = ECCTL_CLRINT;
	for (j=0; j<10000; j++) {
	    if (*(uchar *)&ecp->ec_iop->h_ctl == ECCTL_CLRINT)
		return;
	}
    }
    buginf("\nENVM interrupt will not clear (ctl=%x)", ecp->ec_ihdr.h_ctl);
}

/*
 * Output routine
 */
void ec_output (void)
{
    int i, tlen;
    
    if (ecp->ec_state == EC_STATE_OUTPUT) {
	if (ecp->ec_ihdr.h_ctl == ECCTL_FINISH) {
	    ecp->ec_state = EC_STATE_IDLE; /* wakeup write */
	    return;
	}
	if (ecp->ec_ihdr.h_ctl != ecp->ec_indhdr.h_ctl) {
	    ecp->ec_state = EC_STATE_ERROR; /* wakeup write routine */
	    return;
	}
	if (ecp->ec_ihdr.h_mod != ecp->ec_indhdr.h_mod) {
	    ecp->ec_state = EC_STATE_ERROR; /* wakeup write routine */
	    return;
	}
    } else
	ecp->ec_state = EC_STATE_OUTPUT;
    
    if (ecp->ec_cur < ecp->ec_len) {
	if (ecp->ec_mbsram) {
	    tlen = MIN(EC_MBSIZE, ecp->ec_len - ecp->ec_cur);
	    for (i=0; i<tlen; i++)
		ecp->ec_mbp[i] = ecp->ec_buf[ecp->ec_cur++];
	    ecp->ec_indhdr.h_mod = tlen - 1;
	} else
	  ecp->ec_indhdr.h_mod = ecp->ec_buf[ecp->ec_cur++];
	*ecp->ec_iop = ecp->ec_indhdr;
    } else {
	ecp->ec_state = EC_STATE_IDLE; /* wakeup write */
    }
    return;
}

/*
 * Interrupt driven input routine.
 */
void ec_input (void)
{
    int i, tlen;
    uchar cks;
    
    if (ecp->ec_state == EC_STATE_INPUT) {
	if (ecp->ec_ihdr.h_ctl == ECCTL_FINISH) {
	    ecp->ec_state = EC_STATE_IDLE; /* wakeup read */
	    return;
	}
	if (ecp->ec_ihdr.h_ctl != ecp->ec_reqhdr.h_ctl) {
	    ecp->ec_state = EC_STATE_ERROR; /* wakeup read routine */
	    return;
	}
	if (ecp->ec_mbsram) {
	    for (i=0, cks=0; i<=ecp->ec_reqhdr.h_mod; i++)
	      cks += ecp->ec_buf[ecp->ec_cur++] = ecp->ec_mbp[i];
	    if (ecp->ec_ihdr.h_ctl == ECCTL_FLASH_REQ) {
		if (cks != ecp->ec_ihdr.h_mod) {
		    buginf("\nENVM checksum (%x != %x) at offset 0x%x\n",
			   cks,
			   ecp->ec_ihdr.h_mod,
			   flash.f_cur + ecp->ec_cur - ecp->ec_reqhdr.h_mod);
		    ecp->ec_state = EC_STATE_ERROR; /* wakeup read routine */
		    return;
		} else
		    flash.f_cur += ecp->ec_reqhdr.h_mod + 1;
	    }
	} else
	  ecp->ec_buf[ecp->ec_cur++] = ecp->ec_ihdr.h_mod;
    } else
	ecp->ec_state = EC_STATE_INPUT;
    
    if (ecp->ec_cur < ecp->ec_len) {
	if (ecp->ec_mbsram) {
	    tlen = MIN(EC_MBSIZE, ecp->ec_len - ecp->ec_cur);
	    ecp->ec_reqhdr.h_mod = tlen - 1;
	}
	*ecp->ec_iop = ecp->ec_reqhdr;
    } else {
	ecp->ec_state = EC_STATE_IDLE;
    }
    return;
}

/*
 * Send simple flash command to ENVM.
 */
int ecioc_flash_cmd (uchar cmd, uchar param)
{
    ulong delay;

    ecp->ec_indhdr.h_ctl = cmd;
    ecp->ec_indhdr.h_mod = param;
    if (cmd == ECCTL_FLASH_ERASE || cmd == ECCTL_FLASH_ZERO)
	delay = (1000 * 80);
    else
	delay = 1000;
    ecp->ec_state = EC_STATE_FLASH;

    /*
     * Issue command.
     */
    *ecp->ec_iop = ecp->ec_indhdr;
    
    /*
     * Now subsequent reads/writes are from/to flash device.
     */
    ecp->ec_indhdr.h_ctl = ECCTL_FLASH_IND;
    ecp->ec_reqhdr.h_ctl = ECCTL_FLASH_REQ;

    /*
     * Block for command complete interrupt.
     */
    ec_wait_for_nstate(EC_STATE_FLASH, delay);

    if (cmd == ECCTL_FLASH_VPP)
	ecp->ec_ihdr.h_mod &= VPP_MASK;
    
    if (ecp->ec_state != EC_STATE_IDLE || ecp->ec_ihdr.h_mod != param) {
	if (cmd != ECCTL_FLASH_VPP) /* OK to fail during flash probe */
	    ec_not_responding();
	++ec_flash_cmdfails;
	return -1;
    }
    
    return 0;
}

/*
 * Position PBUS address pointer to specified offset from the
 * start of Flash ROM.
 */
int ecioc_flash_seek (ulong off)
{
    uchar  tbuf[4];
    
    ecp->ec_mbsram = FALSE;
    if (off > flash.f_maxseek)
	return DEVERR_SEEK_PAST_EOM;

    /* Divide offset by 2, since there's two byte-wide banks */
    off >>= 1;
    tbuf[0] = off & 0xff;
    tbuf[1] = (off>>8) & 0xff;
    tbuf[2] = (off>>16) & 0xff;
    
    ecp->ec_indhdr.h_ctl = ECCTL_FLASH_SEEK;
    if (ec_write(0, (char *)tbuf, 3) < 3) {
	ecp->ec_indhdr.h_ctl = ecp->ec_reqhdr.h_ctl = 0;
	ec_not_responding();
	return -1;
    }

    /*
     * Now subsequent reads/writes are from/to flash device.
     */
    ecp->ec_indhdr.h_ctl = ECCTL_FLASH_IND;
    ecp->ec_reqhdr.h_ctl = ECCTL_FLASH_REQ;
    
    flash.f_cur = off << 1;
    ecp->ec_mbsram = TRUE;
    return 0;
}

/*
 * ASSUMES REV-4 ENVM HARDWARE...
 * Attempt to alloc a 256-byte aligned block of Multibus
 * shared SRAM, which the ENVM and CSC may then use for
 * more efficient data transfer.
 */
int ecioc_mbaddr (void)
{
    uchar offbyte;
    
    if (ecp->ec_mbp)
	return 0;
    
    /*
     * We waste 256 bytes, in order that we may guarantee we are
     * aligned on a 256 byte boundary.
     */
    if (!(ecp->ec_mbp = mempool_malloc(MEMPOOL_CLASS_MULTIBUS,
				       EC_MBSIZE+256))) {
      return -1;
    }
    ecp->ec_mbp = (uchar *)((ulong)(ecp->ec_mbp + 255) & ~255);
    offbyte = ((ulong) ecp->ec_mbp >> 8) & 0xff;
    if (ecioc_flash_cmd(ECCTL_MBADDR, offbyte) < 0)
	return -1;

    ecp->ec_rev4 = TRUE;
    return 0;
}


/***************************************************************************
 *
 *     EC SUPPORT FUNCTIONS
 *
 ***************************************************************************/

void ec_shutdown (uchar error)
{
    static int count = 0;
#ifdef GIVE_IT_ALL_WHILE_ALIVE
    int i;
    ecshow_t *showp;
#endif    
    
    if (count++ > 3)
	return;
    
    if (error < 1 || error > ENVMERR_UNKNOWN)
	error =  ENVMERR_UNKNOWN;
    errmsg(&msgsym(SHUTDOWN, ENV));

    reg_invoke_envmib_shutdown_trap();
    process_suspend();

#ifdef GIVE_IT_ALL_WHILE_ALIVE
    /*
     * Print environment statistics last collected,
     * though the power supply will probably be shut
     * off by now (or shortly thereafter).
     */
    if (showp = ecp->ec_show) {
	buginf("\n  Ambient temperature measured %s",
	       showp->s_test[ECTEST_T_AMB].t_measured);
	
	buginf("\n  Air flow appears %s.",
	       showp->s_test[ECTEST_T_AIR].t_warning[0] ? "marginal" : "good");
	
	for (i=ECTEST_V_P5; i<=ECTEST_V_N5; i++) {
	    buginf("\n  %s line measured at %s",
		   showp->s_test[i].t_description,
		   showp->s_test[i].t_measured);
	}
	flush();
    }
#endif    
}

/*
 * Wait for ec_state to change from state, or timeout.
 */
int ec_wait_for_nstate (int state, ulong maxmilliseconds)
{
    int i, level;
    
    if (!systeminit_flag) {
	/* 
	 * Interrupts haven't been enabled yet.
	 */ 
	for (i=0; i<maxmilliseconds; i++) {
	    level = set_interrupt_level(ALL_ENABLE);
	    DELAY(1);
	    reset_interrupt_level(level);
	    if (ecp->ec_state == EC_STATE_IDLE)
		return 0;
	}
	return -1;
    }
    TIMER_START(ecp->ec_delay, maxmilliseconds);
    edisms((blockproc *)ec_nstateBLOCK, state);
    return (ecp->ec_state == state) ? 0 : -1;
}

/* Sleep until ecp->ec_state != state, or until timeout */
boolean ec_nstateBLOCK (ulong state)
{
    if (ecp->ec_state != state)
	return FALSE; /* wakeup */
    if (AWAKE(ecp->ec_delay))
	return FALSE; /* wakeup */
    return TRUE; /* keep sleeping */
}


#ifdef TEST_FLASH
static int ecm_close(char *);
static int ecm_dump(char *);
static int ecm_erase(char *);
static int ecm_fcopy(char *);
static int ecm_fverify(char *);
static int ecm_help(char *);
static int ecm_loop(char *);
static int ecm_mbloop(char *);
static int ecm_open(char *);
static int ecm_quit(char *);
static int ecm_read(char *);
static int ecm_rep(char *);
static int ecm_seek(char *);
static int ecm_set(char *);
static int ecm_verify(char *);
static int ecm_vpp(char *);
static int ecm_write(char *);
static int ecm_cmd_index(char *);
static char *next_arg(char *);
static long get_hex(char *);
static void hexdump(uchar *, int, int);
static char *testpt_next(char *);

/***************************************************************************
 *
 *     ENVM DEBUG SHELL ROUTINES
 *
 ***************************************************************************/

ulong   ecmRepDelay = 0;
boolean ecmRepBail = TRUE;
boolean ecmRepSilent = FALSE;
int     ecid = -1;
char    ecmBuf[256];

struct ecm_cmd_type {
    char *c_str;
    int  (*c_fun)();
    char *c_help;
} ecm_cmd[] = {
    "close", ecm_close, "- de-select device",
    "dump",  ecm_dump,  "[offset] [length] - display memory",
    "erase", ecm_erase, "- electrically erase flash ROMs",
/*  "ftest", ecm_ftest, "[vercnt] - write and verify flash",*/
    "fcopy", ecm_fcopy, "[file_name] [host_ip] - copy tftp flash",
    "fver",  ecm_fverify,"- checksum verification of flash",
    "help",  ecm_help,  "- flash help commands",
    "init",  ecm_init,  "[nvram|flash] - initialize like bootup",
    "loop",  ecm_loop,  "- Loopback test thru ENV I/O port",
    "mbloop",ecm_mbloop,"- Loopback test thru ENV Multibus SRAM",
    "open",  ecm_open,  "[mci|env] - select (non-default) flash device",
    "quit",  ecm_quit,  "- exit",
    "read",  ecm_read,  "<len> - display <len> bytes from device",
    "rep",   ecm_rep,   "<cnt> <command> - Repeat <cmd> <cnt> times",
    "seek",  ecm_seek,  "[off] - set flash seek offset to <off>",
    "set",   ecm_set,   "<var> - set/reset a variable",
    "verify",ecm_verify,"<pat> <len> - verify that next <len> bytes == <pat>",
    "vpp",   ecm_vpp,   "on|off|toggle - turn on/off flash VPP",
    "write", ecm_write, "<pat> [len] - write <pat> to device",
    "?",     ecm_help,  "- Same as 'help'",
};
#define ECM_MAX (sizeof(ecm_cmd)/sizeof(struct ecm_cmd_type))

/*
 * Entrypoint into FLASH/ENVM diagnostic shell
 */
void test_flash (parseinfo *csb)
{
    const int size = 80;
    int  i, sts;
    char  *ecmbuf;
    void ecm_open_default();
    
    if (!(ecmbuf = malloc(size))) {
	printf(nomemory);
	return;
    }
    ecm_open_default();
    while (TRUE) {
	printf("flash> ");
	if (!rdtty(ecmbuf, size) || strlen(ecmbuf)<1)
	    continue;
	if ((i = ecm_cmd_index(ecmbuf)) < 0) {
	    printf("Bad command: (%s), type '?' for help\n", ecmbuf);
	    continue;
	}
	if ((sts = (*ecm_cmd[i].c_fun)(next_arg(ecmbuf))) == ECM_QUIT)
	    break;
    }
    if (ecid >= 0)
	dev_close(ecid);
    (void) free(ecmbuf);
}

void ecm_open_default (void)
{
    if (!flash.f_dev) {
	printf("\nFlash memory not detected");
	ecid = -1;
    } else if ((ecid = dev_open(flash.f_dev, FLASH_OPEN)) < 0) {
	printf("\nCouldn't open default flash device thru %s", flash.f_ctlr);
    } else {
	printf("Accessing %s thru interface %s\n", flash.f_card, flash.f_ctlr);
    }
}

/*
 * 
 */
int ecm_close (char *s)
{
    if (ecid >= 0)
	if (dev_close(ecid) >= 0)
	    ecid = -1;
    return ECM_OK;
}

#if 1
/*
 */
int ecm_dump (char *s)
{
    static long off = 0;
    static long len = 0x100;
    long        o, l;
    
    if ((o = get_hex(s)) >= 0) {
	off = o;
	if ((l = get_hex(s = next_arg(s))) >= 0)
	    len = l;
    }
    hexdump((uchar *) off, off, len);
    return ECM_OK;
}
#endif

/*
 */
int ecm_erase (char *s)
{
    return flash_erase_all(ecid);
}

/*
 * fcopy <filename> <hostname|host_ip_addr>
 *
 */
int ecm_fcopy (char *s)
{
    flash_fhdr *fh;
    char *bp, *hp, name[64];
    int i;
    int erase = 0;
    addrtype host;
    nametype *ptr;
    
    if (ecid < 0)
	return ECM_ERROR;
    
    hp = next_arg(s);
    
    for (i=0; i<64; i++) {
	if ((name[i] = *s) == ' ' || !*s++) {
	    break;
	}
    }
    name[i] = 0;

    if (!numeric_lookup(ADDR_IP, hp, &host)) {
	if (ptr = (nametype *) name_lookup(hp, &i, NAM_IP|NAM_CLNS)) {
	    address_copy(&host, &ptr->v.ip_adr[0].addr);
	} else {
	    printf("Bad address or host name\n");
	    return ECM_ERROR;
	}
    }
    
    printf("Copying file '%s' from %s to flash\n", name, 
	   name_addr2string(&host));

    if (!(fh = flash_get_fhdr(ecid, name))) {
	if ((flash.f_size - flash.f_free_offset) < 2*1024*1024) {
	    printf(" - less than 2MB flash free, erase first\n");
	    erase++;
	}
    } else if (fh->ffh_len >
	(flash.f_size - flash.f_free_offset - sizeof(flash_fhdr))) {
	printf(" - not enough space to append file, erase first\n");
	erase++;
    } else {
	printf(" - file already present in flash, invalidating\n");
	flash_delete_file(ecid, name);
    }
    


    if (!(bp = malloc(1024))) {
	printf(nomemory);
	return ECM_ERROR;
    }
    
    i = tftp2flash(ecid, &host, name, bp, FALSE, TRUE, erase);
    free(bp);
    return i;
}

/*
 *
 */
int ecm_fverify (char *s)
{
    flash_fhdr *fh;
    char *bp;
    int sts;
    
    if (ecid < 0)
	return ECM_ERROR;

    if (flash.f_filecnt < 1) {
	printf("Nothing in flash to verify\n");
	return ECM_ERROR;
    }

    if (!(fh = flash_get_fhdr(ecid, s)))
	return ECM_ERROR;
    
    if (!(bp = malloc(1024))) {
	printf(nomemory);
	return ECM_ERROR;
    }
    
    sts =  flash_verify(ecid, fh, bp);
    free(bp);
    return sts;
}

int ecm_help (char *s)
{
    int i;
    
    for (i=0; i<ECM_MAX; i++)
	printf("%s %s\n", ecm_cmd[i].c_str, ecm_cmd[i].c_help);
    return ECM_OK;
}

int ecm_init (char *s)
{
    if (partialmatch("nvram", s)) {
	flash_nv_idbp = NULL;
	nv_init();
    } else if (partialmatch("flash", s)) {
	if (ecid >= 0)
	    dev_close(ecid);
	memset(&flash, 0, sizeof(flash));
	flash_init_ags();
	ecm_open_default();
    } else if (!*s) {
	if (ecid >= 0)
	    dev_close(ecid);
	flash_nv_idbp = NULL;
	nv_init();
	memset(&flash, 0, sizeof(flash));
	flash_init_ags();
	ecm_open_default();
    } else {
	printf("Parameters are 'nvram' or 'flash', defaults to both\n");
	return ECM_ERROR;
    }
    return ECM_OK;
}

/*
 * Loopback test through ENV I/O Port
 * The ENV knows to just use the I/O Port (not MB SRAM), regardless
 * of the current revision level, so we need to do the same.
 */
int ecm_loop (char *s)
{
    static char *id = "ecm_loop";
    int     len, i, j, sts, max;
    uchar   *buf;
    
    if (ecid < 0) {
	printf("%s ec not open\n", id);
	return ECM_ERROR;
    }
    if (dev_ioctl(ecid, IOC_LOOPBACK, (void *)0) < 0) {
	printf("%s ioctl failed\n", id);
	return ECM_ERROR;
    }
    if (!(buf = malloc(256))) {
	printf(nomemory);
	return ECM_ERROR;
    }
    if ((len = get_hex(s)) < 0)
	len = 100000000;
    max = 255;
    for (i=0; i<len; i++) {
	for (j=0; j<max; j++)
	    buf[j] = (uchar) ((i+j) % 256);
	if ((sts = dev_write(ecid, (char *)buf, max)) < max) {
	    printf("%s write %d != %d iter=%d\n", id, sts, max, i);
	    free(buf);
	    return ECM_ERROR;
	}
	printf("r");
	flush();
	for (j=0; j<max; j++)
	    buf[j] = 0;
	if ((sts = dev_read(ecid, (char *)buf, max)) != max) {
	    printf("%s read %d != %d iter=%d\n", id, sts, max, i);
	    free(buf);
	    return ECM_ERROR;
	}
	for (j=0; j<max; j++)
	    if (buf[j] != (uchar) ((i+j) % 256)) {
		printf("%s cmp err iter=%d byte=%d (%x != %x)\n",
		       id, i, j, buf[j], ((i+j) % 256));
		free(buf);
		return ECM_ERROR;
	    }
	printf("w");
	flush();
	if (serial_getc(stdio) >= 0) {
	    printf("%s Abort after %d %d-byte packets\n", id, i, max);
	    free(buf);
	    return ECM_ERROR;
	}
    }
    printf("Successfully looped %d %d-byte packets\n", i, max);
    free(buf);
    return ECM_OK;
}


/*
 * Multibus shared memory loopback test.
 */
int ecm_mbloop (char *s)
{
    static char *id = "ecm_mbloop:";
    int    i, j, len, sts, max;
    
    if (ecid != DEV_ENV) {
	printf("ENVM not open\n");
	return ECM_ERROR;
    }
    if (dev_ioctl(ecid, IOC_MBLOOPBACK, (void *)0) < 0) {
	printf("%s ioctl failed\n", id);
	return ECM_ERROR;
    }
    if ((len = get_hex(s)) < 0) {
	len = 100000000;
	max = 256;
    } else {
	if ((max = get_hex(next_arg(s))) < 1 || max > 256)
	    max = 256;
    }
    
    for (i=0; i<len; i++) {
	for (j=0; j<max; j++)
	    ecmBuf[j] = (char) ((i+j) & 0xFF);
	if ((sts = dev_write(ecid, (char *)ecmBuf, max)) != max) {
	    printf("\nwrite %d != %d iter=%d\n", id, sts, max, i);
	    return ECM_ERROR;
	}
	printc('W');
	flush();
	for (j=0; j<max; j++) {
	    ecp->ec_mbp[j] = 0;
	    ecmBuf[j] = 0;
	}
	if ((sts = dev_read(ecid, (char *)ecmBuf, max)) != max) {
	    printf("%s read %d != %d iter=%d\n", id, sts, max, i);
	    return ECM_ERROR;
	}
	printc('R');
	flush();
	for (j=0; j<max; j++)
	    if (((uchar)ecmBuf[j]) != (uchar) ((i+j) & 0xff)) {
		hexdump((uchar *)ecmBuf, 0, max);
		printf("\nCompare error!  iter=%d offset=0x%x (%x != %x)\n",
		       i, j, ecmBuf[j], ((i+j) & 0xff));
		return ECM_ERROR;
	    }
	if (serial_getc(stdio) >= 0) {
	    printf("%s Abort after %d %d-byte packets\n", id, i, max);
	    return ECM_ERROR;
	}
    }
    printf("Successfully looped %d %d-byte packets thru MB\n", i, max);
    return ECM_OK;
}

/*
 * 
 */
int ecm_open (char *s)
{
    
    int dev;

    if (strcmp(s, "mci") == 0)
	dev = DEV_MCI;
    else if (strcmp(s, "env") == 0)
	dev = DEV_MCI;
    else {
	printf("Bad argurment (%s): try 'mci' or 'env'\n", s);
	return ECM_ERROR;
    }
	
    if (ecid >= 0)
	dev_close(ecid);

    if ((ecid = dev_open(dev, FLASH_OPEN)) < 0) {
	printf("No nvram found through '%s' driver\n", s);
	return ECM_ERROR;
    }
    printf("OK\n");
    return ECM_OK;
}

int ecm_quit (char *s)
{
    return ECM_QUIT;
}

int ecm_read (char *s)
{
    long   len, i, rlen, toff = 0;
    const int blen = 1024;
    uchar *buf;
    
    if ((len = get_hex(s)) < 0) {
	printf("length needed\n");
	return ECM_ERROR;
    }
    if (!(buf = malloc(blen))) {
	printf(nomemory);
	return ECM_ERROR;
    }
    while (len) {
	for (i=0; i<blen; i++)
	    buf[i] = 0;
	rlen = MIN(blen, len);
	if ((i = dev_read(ecid, (char *)buf, rlen)) < 1) {
	    printf("read fail %x\n", i);
	    free(buf);
	    return ECM_ERROR;
	}
	hexdump(buf, flash.f_cur - rlen, i);
	if (i < rlen) {
	    printf("read returned %d (expect %d) toff=%x off=%x\n",
		   i, rlen, toff, flash.f_cur - rlen);
	    break;
	}
	len -= rlen;
	toff += rlen;
    }
    free(buf);
    return ECM_OK;
}

/*
 * Repeat an ECMON command.
 */
int ecm_rep (char *s)
{
    int  i, n, cmd, sts, success;
    
    if (*s && (n=get_hex(s)) > 0) {
	s = next_arg(s);
	if (*s && (cmd = ecm_cmd_index(s)) >= 0) {
	    s = next_arg(s);
	    for (i=0, success=0; i<n; i++) {
		if ((sts = (*ecm_cmd[cmd].c_fun)(s)) != ECM_OK) {
		    if (ecmRepBail) {
			printf("Failed after %x iterations\n", i+1);
			return sts;
		    }
		} else
		    success++;
		if (!ecmRepSilent) {
		    printf("Rep 0x%x of 0x%x\n", i+1, n);
		    flush();
		}
		if (serial_getc(stdio) >= 0) {
		    printf("BREAK\n");
		    break;
		}
		if (ecmRepDelay)
		    process_sleep_for(ecmRepDelay);
	    }
	    printf("Successful 0x%x out of 0x%x iterations\n", success, i+1);
	    return ECM_OK;
	}
    }
    printf("Need COUNT and COMMAND parameters\n");
    return ECM_ERROR;
}
/*
 */
int ecm_seek (char *s)
{
    static char *msg = "ecm_seek: %sseek ioctl failed\n";
    long off;
    
    if ((off = get_hex(s)) >= 0) {
	if (dev_ioctl(ecid, IOC_FLASH_SEEK, &off) < 0) {
	    printf(msg, NULL);
	    return ECM_ERROR;
	}
    } else
	printf("Enter seek offset\n");
    return ECM_OK;
}

/*
 * Set an internal variable to non-default value.
 */
int ecm_set (char *s)
{
    int n;
    
    if (ExactToSPsmatch(s, "repdelay")) {
	if ((n = get_hex(next_arg(s))) >= 0) {
	    if (n > 100)
		printf("Rep delay of %x seconds is too long\n", n);
	    else
		ecmRepDelay = n * 1000;
	}
    } else if (ExactToSPsmatch(s, "repmsdelay")) {
	if ((n = get_hex(next_arg(s))) >= 0) {
	    if (n > (100 * 1000))
		printf("Rep delay of 0x%x (%d) milliseconds is too long\n",
		       n, n, n);
	    else
		ecmRepDelay = n;
	}
    } else if (ExactToSPsmatch(s, "repbail")) {
	ecmRepBail = !ecmRepBail;
    } else if (ExactToSPsmatch(s, "repsilent")) {
	ecmRepSilent = !ecmRepSilent;
    } else if (ExactToSPsmatch(s, "debug")) {
	ec_debug = !ec_debug;
    }
    printf("repbail is %s\n", ecmRepBail ? "on" : "off");
    printf("repsilent is %s\n", ecmRepSilent ? "true" : "false");
    printf("repdelay value is 0x%x (%d) seconds\n",
	   ecmRepDelay/1000, ecmRepDelay/1000);
    printf("repmsdelay value is 0x%x (%d) milliseconds\n",
	   ecmRepDelay, ecmRepDelay);
    printf("debug (envm) is %s\n", ec_debug ? "on" : "off");
    return ECM_OK;
}

/*
 */
int ecm_verify (char *s)
{
    long   len, i, j, bytes;
    ulong  pat, *lp;

    if (ecid < 0) {
	printf("Device not accessible\n");
	return ECM_ERROR;
    }
    pat = (ulong) get_hex(s);
    if ((len = get_hex(s = next_arg(s))) < 0)
	len = 1;
    for (i=0; i<256; i++)
	ecmBuf[i] = (char) ~pat;
    printf("Verifying 0x%x bytes of long pattern %04x\n", len, pat);
    for (bytes=0; bytes<len; bytes+=i) {
	i = MIN((len-bytes), 256);
	if (dev_read(ecid, ecmBuf, i) < i) {
	    printf("Read failed at byte offset %x\n", bytes);
	    return ECM_ERROR;
	}
	lp = (ulong *)ecmBuf;
	for (j=0; j<(i>>2); j++) {
	    if (pat != lp[j]) {
		printf("Verify error at flash offset %x (read %04x)\n",
		   flash.f_cur, lp[j]);
		return ECM_ERROR;
	    }
	}
    }
    return ECM_OK;
}

/*
 */
int ecm_vpp (char *s)
{
    static int onoff = VPP_OFF;
    
    if (ExactToSPsmatch(s, "on"))
	onoff = VPP_ON;
    else if (ExactToSPsmatch(s, "off"))
	onoff = VPP_OFF;
    else if (ExactToSPsmatch(s, "toggle"))
	onoff = (onoff == VPP_OFF) ? VPP_ON : VPP_OFF;
    else {
	printf("bad param\n");
	return ECM_ERROR;
    }
    if (dev_ioctl(ecid, IOC_FLASH_VPP, &onoff) == 0)
	return ECM_OK;
    if (!ecmRepSilent)
	printf("VPP '%s' failed\n", s);
    return ECM_ERROR;
}

/*
 */
int ecm_write (char *s)
{
    long   len, i, pat, byte;
    ulong  *bp;

    if (ecid < 0) {
	printf("Device not accessible\n");
	return ECM_ERROR;
    }
    pat = (ulong) get_hex(s);
    if ((len = get_hex(s = next_arg(s))) < 0)
	len = 1;
    bp = (ulong *)ecmBuf;
    for (i=0; i<64; i++)
	*bp++ = pat;
    printf("Writing %x bytes with long pattern %04x\n", len, pat);
    for (byte=0; byte<len; byte+=i) {
	i = MIN((len - byte), 256);
	if (dev_write(ecid, ecmBuf, i) < i) {
	    printf("Write failed at byte offset %x\n", byte);
	    return ECM_ERROR;
	}
    }
    return ECM_OK;
}

/***************************************************************************
 *
 *                        ECM Support Routines
 *
 ***************************************************************************/

int ecm_cmd_index (char *s)
{
    int i;
    for (i=0; i<ECM_MAX; i++)
	if (ExactToSPsmatch(s, ecm_cmd[i].c_str))
	    return i;
    return -1;
}

char *next_arg (char *s)
{
    while (*s && *s != ' ')
	s++;
    while (*s && *s == ' ')
	s++;
    return s;
}

long get_hex (char *s)
{
    long n;
    
    if (!*s)
	return -1;
    n = 0;
    while (TRUE) {
	if (!*s || *s == ' ' || *s == '\t' || *s == '=')
	    return n;
	n *= 16;
	if (*s >= '0' && *s <= '9')
	    n += (*s - '0');
	else if (*s >= 'A' && *s <= 'F')
	    n += 10 + (*s - 'A');
	else if (*s >= 'a' && *s <= 'f')
	    n += 10 + (*s - 'a');
	else
	    return -1;
	s++;
    }
}

void hexdump (uchar *p, int off, int len)
{
    int i;
    char buf[20];
    
    buf[16] = '\0';
    for (i=0; i<len; i++) {
	buf[i % 16] = (p[i]<' '||p[i]>126) ? '.' : p[i];
	if ((i % 16) == 0)
	    printf("%08x: %02x", off+i, p[i]);
	else if ((i % 8) == 0)
	    printf("  %02x", p[i]);
	else if ((i % 16) == 15)
	    printf(" %02x   %s\n", p[i], buf);
	else
	    printf(" %02x", p[i]);
    }
    if ((i = (i % 16)) != 0) {
	buf[i] = 0;
	if (i<9)
	    printf(" ");
	while (i++ < 16)
	    printf("   ");
	printf("   %s\n", buf);
    }
}
#endif /* TEST_FLASH */

/*
 * This will lex the show string: "o2=5.000 w2=5% s2=10% c2=5.012 o3=12.00 ..."
 * Puts null in place of '=' and following white space.
 * If found valid-looking field, returns pointer to next field
 * in the string.
 * If ERROR, returns NULL.
 */
char *testpt_next (char *s)
{
    /* Eat white space */
    while (*s == ' ' || *s == '\t' || *s == '\n')
	s++;
    if (*s) {                   /* id[0] */
	s++;
	if (*s) {                 /* id[1] */
	    s++;
	    if (*s == '=') {      /* "="   */
		*s++ = 0;         /* null terminate (not needed)*/
		while (*s == ' ') /* scan past string arg       */
		    s++;
		while (*s && *s != ' ' && *s != '\t' && *s != '\n')
		    s++;
		*s++ = 0;
		while (*s == ' ' || *s == '\t' || *s == '\n')
		    s++;
		return s;
	    }
	}
    }
    return NULL;
}
