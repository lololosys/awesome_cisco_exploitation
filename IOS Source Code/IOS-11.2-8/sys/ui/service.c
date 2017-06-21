/* $Id: service.c,v 3.4.18.6 1996/08/23 07:59:16 ahh Exp $
 * $Source: /release/112/cvs/Xsys/ui/service.c,v $
 *------------------------------------------------------------------
 * service.c -- turn on and off various services.
 *
 * March 1988, Bill Westfield
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: service.c,v $
 * Revision 3.4.18.6  1996/08/23  07:59:16  ahh
 * CSCdi66937:  IP: missed a few lines turning off small servers
 * CSCdi59121 didn't quite do the job...
 * Branch: California_branch
 *
 * Revision 3.4.18.5  1996/08/13  18:27:12  ahh
 * CSCdi65773:  slave commands show up in routers without VIPs
 * Fix the default pointer to match the default value.
 * Branch: California_branch
 *
 * Revision 3.4.18.4  1996/06/17  23:38:40  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.4.18.3  1996/06/10  04:53:18  dlobete
 * CSCdi45148:  Error messages on VIP not reported on RP/RSP
 * Branch: California_branch
 * Slave-log is now on by default.
 *
 * Revision 3.4.18.2  1996/05/30  22:19:34  ahh
 * CSCdi59121:  IP: disable small servers by default (security)
 * Branch: California_branch
 *
 * Revision 3.4.18.1  1996/03/18  22:29:20  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.22.3  1996/03/14  01:36:21  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.3.22.2  1996/03/09  05:14:13  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.3.22.1  1996/02/20  21:26:40  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/07  16:16:45  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.3  1995/11/22  05:43:57  eschaffe
 * CSCdi42241:  PROTOCONVERT is TRUE for images that don't include the PT
 * option.
 * Obsolete PROTOCONVERT flag in makefiles.  The control for defining
 * protocolconversion for an image is now platform_specific.
 *
 * Revision 3.2  1995/11/17  17:49:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:41:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/29  21:21:53  jenny
 * CSCdi38480:  UDP: should be able to disable toy servers
 * Added new knob "service udp-small-servers".
 *
 * Revision 2.3  1995/07/25  00:03:47  che
 * CSCdi28688:  Security tightening on product
 * Forces "service tcp-small-servers" to be written out during a NVGEN even
 * if it is the same as the default.
 *
 * Revision 2.2  1995/07/11  23:46:06  billw
 * CSCdi35800:  Need knob to supress user view of IP addresses
 *
 * Revision 2.1  1995/06/07  23:11:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../../boot/cpu.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_defs_service.h"
#include "service.h"
#include "../os/nv.h"
#include "logger.h"

/*
 * External Declarations
 */
extern int nttylines;

/*
 * Forward Declarations
 */
static boolean service_config(void);


/*
 * Storage
 */
boolean decimaltty_enable;	/* use decimal tty numbers */
boolean finger_enable;		/* enable finger server on port 79 */
boolean netconfig_enable;	/* use net config files */
boolean nagle_enable;		/* use nagle small packet avoidance */
boolean slavecore_enable;       /* enable coredump by VIPs */
boolean slavelog_enable;        /* enable coredump by VIPs */
boolean telnet_idle0window;	/* set tcp window 0 when connection is idle */
boolean tcp_keepalivesin;	/* Do keepalives on incoming TCP connections */
boolean tcp_keepalivesout;	/* Do keepalives on outgoing tcp connections */
boolean tcpservers_enable;	/* Allow connects to ECHO/CHARGEN/DISCARD/etc.*/
boolean udpservers_enable;	/* Allow connects to ECHO/CHARGEN/DISCARD/etc.*/
boolean execwait_enable;	/* Wait for noisy execs */
boolean linenumber_enable;	/* Give the linenumber message */
boolean internal_cmd_enable;	/* TRUE if internal commands are allowed */
boolean config_prompt;
boolean log_backtrace;          /* Print backtrace in buginf()  and errmsg() */
boolean compress_config_enable; /* enable nvram config compression */
boolean exec_callback_enable;   /* Exec callback flag */
boolean old_slip_prompts;	/* Use 9.1 style slip prompts */
boolean service_hideaddr;	/* don't type addresses in telnet command */
boolean vty_logging;            /* Log VTY-async events */

/*
 * Disallow PAD connections and X.29 commands
 * It would have been much cleaner if the definition of pad_enable 
 * was in x29.c file where it really belongs.  But "service pad" command
 * needs to be processed and executed even if the pad subsystem is not
 * included in some images.
 */
boolean pad_enable = FALSE;

/*
 * Command tables
 */


/*
 * service_on
 * service_off
 * Routines that always return TRUE or FALSE.
 */

static boolean service_on (void)
{
    return(TRUE);
}

static boolean service_off (void)
{
    return(FALSE);
}


/*
 * service_command
 * Set one of the service booleans, which provides administrative control
 * over various features.
 */

void service_command (parseinfo *csb)
{
    boolean	*var = NULL;
    boolean	(*func)(void) = NULL;

    switch (csb->which) {
      case SERVICE_CONFIG:
	var = &netconfig_enable;
	func = service_config;
	break;
      case SERVICE_COMPRESS_CFG:
	var = &compress_config_enable;
	func = service_off;
	if (!csb->nvgen && csb->sense) {
	    if (!nv_rom_supports_nvcompress()) {
		printf("Boot ROMs do not support NVRAM compression.\n");
		printf("Disabling service compress-config.\n");
		csb->sense = FALSE;
	    }
	}
	break;
      case SERVICE_DECIMAL:
	var = &decimaltty_enable;
	if (cpu_family != FAMILY_CSC2) {
	    func = service_on;
	    break;
	}
	/* CSC2 platforms only */
	if (nttylines == 0)
	    func = service_on;
	else
	    func = service_off;
	break;
      case SERVICE_FINGER:
	var = &finger_enable;
	func = service_on;
	break;
      case SERVICE_LINE_NUMBER:
	var = &linenumber_enable;
	func = service_off;
	break;
      case SERVICE_NAGLE:
	var = &nagle_enable;
	func = service_off;
	break;
      case SERVICE_SLAVECORE:
        var = &slavecore_enable;
        func = service_off;
        break;
      case SERVICE_SLAVELOG:
        var = &slavelog_enable;
        func = service_on;
        break;
      case SERVICE_PAD:
	var = &pad_enable;
	func = service_on;
	break;
      case SERVICE_TELNETZERO:
	var = &telnet_idle0window;
	func = service_off;
	break;
      case SERVICE_TCPKIN:
	var = &tcp_keepalivesin;
	func = service_off;
	break;
      case SERVICE_TCPKOUT:
	var = &tcp_keepalivesout;
	func = service_off;
	break;
      case SERVICE_PWD_ENCRYPT:
	var = &password_encryption_enable;
	func = service_off;
	break;
      case SERVICE_EXECWAIT:
	var = &execwait_enable;
	func = service_off;
	break;
      case SERVICE_INTERNAL:
	var = &internal_cmd_enable;
	func = service_off;
	break;
      case SERVICE_PROMPT:
	var = &config_prompt;
	func = service_on;
	break;
      case SERVICE_LOG_BACKTRACE:
	var = &log_backtrace;
	func = service_off;
	break;
      case SERVICE_TCPSERVERS:
	var = &tcpservers_enable;
	func = service_off;
	break;
      case SERVICE_UDPSERVERS:
	var = &udpservers_enable;
	func = service_off;
	break;
      case SERVICE_CALLBACK:
 	var = &exec_callback_enable;
 	func = service_off;
 	break; 
      case SERVICE_OLDSLIP:
	var = &old_slip_prompts;
	func = service_off;
	break;
      case SERVICE_HIDEADDR:
	var = &service_hideaddr;
	func = service_off;
	break;
      case SERVICE_VTYLOG:
	var = &vty_logging;
	func = service_off;
	break;
      default:
	bad_parser_subcommand(csb, csb->which);
	return;
    }
    if (csb->nvgen) {
	if ((csb->which == SERVICE_TCPSERVERS) ||
	    (csb->which == SERVICE_UDPSERVERS)) {
	    /* Remove this in release after 11.2 */
	    nv_write(TRUE, "%s%s", (*var ? "" : "no "),
		     csb->nv_command);
	} else {
	    nv_write((*var != (*func)()), "%s%s", (*var ? "" : "no "),
		     csb->nv_command);
	}
	return;
    }
    if (csb->set_to_default)
	*var = (*func)();
    else
	*var = csb->sense;
}

/*
 * service_init
 * Initialize the service variables
 */

void service_init (void)
{
    /*
     * Figure out what the defaults should be for this particular system
     * The only thing we want to number in octal is physical terminal lines,
     * since we want to match the rear panel lables.  On pure routers, we
     * don't care (less than 8 lines exist anyway!), on devices with protocol
     * translation, use octal if there are also STP lines (PT option in TS),
     * but not on IGS with PT option...
     */
    netconfig_enable = service_config();
    if (cpu_family != FAMILY_CSC2) {
	decimaltty_enable = service_on();
    } else {
	/* CSC2 platforms only */
	if (nttylines == 0)
	    decimaltty_enable = service_on();
	else
	    decimaltty_enable = service_off();
    }
    finger_enable = service_on();
    tcpservers_enable = service_off();
    udpservers_enable = service_off();
    linenumber_enable = service_off();
    nagle_enable = service_off();
    slavecore_enable = service_off();
    slavelog_enable = service_on();
    telnet_idle0window = service_off();
    tcp_keepalivesin = service_off();
    tcp_keepalivesout = service_off();
    password_encryption_enable = service_off();
    execwait_enable = service_off();
    compress_config_enable = service_off();
    config_prompt = service_on();
    old_slip_prompts = service_off();
}

/*
 * service_timestamps
 *
 * service timestamps [{debug | log}] [{uptime | datetime [msec] [localtime]
 *                                       [show-timezone]}]
 * no service timestamps [{debug | log}]
 *  OBJ(int,1) = service (debug/log)
 *  OBJ(int,2) = type (uptime/datetime)
 *  OBJ(int,3) = msec flag
 *  OBJ(int,4) = localtime flag
 *  OBJ(int,5) = show-timezone flag
 */
void service_timestamps_cmd (parseinfo *csb)
{

    ulong *svc_ptr;
    int i;

    /* First NVGENs. */

    if (csb->nvgen) {
        if (GETOBJ(int,1) >= NUM_SERVICE_TIMESTAMPS) /* Parser cruft */
            return;
        svc_ptr = &service_timestamps[GETOBJ(int,1)];
        if (*svc_ptr) {
            nv_write(TRUE, "%s", csb->nv_command);
            if (*svc_ptr & SERVICE_TIMESTAMP_DATETIME) {
                nv_add(TRUE, " datetime");
                nv_add(*svc_ptr & SERVICE_TIMESTAMP_MSEC, " msec");
                nv_add(*svc_ptr & SERVICE_TIMESTAMP_LOCAL, " localtime");
                nv_add(*svc_ptr & SERVICE_TIMESTAMP_SHOWTZ, " show-timezone");
            } else {
                nv_add(TRUE, " uptime");
            }
        }
        return;
    }

    /* Now the "no" form. */

    if (!csb->sense) {

        /*
         * Special kludge.  If the type is NUM_SERVICE_TIMESTAMPS, this
         * says that they did a "no service timestamps", meaning that we
         * should turn off all timestamp services.
         */

        if (GETOBJ(int,1) == NUM_SERVICE_TIMESTAMPS) {
            for (i=0; i<NUM_SERVICE_TIMESTAMPS; i++)
                service_timestamps[i] = 0;
        } else {

            /* Otherwise, zero the appropriate field. */

            service_timestamps[GETOBJ(int,1)] = 0;
        }
    } else {

        /* Positive.  OR together the option bits and put 'em away. */

        service_timestamps[GETOBJ(int,1)] = GETOBJ(int,2) | GETOBJ(int,3) |
            GETOBJ(int,4) | GETOBJ(int,5) | SERVICE_TIMESTAMP_ENAB;
    }
}

/*
 * service_config
 *
 * [no] service config
 */

static boolean service_config (void)
{
    return(platform_get_value(PLATFORM_VALUE_SERVICE_CONFIG) != 0);
}
