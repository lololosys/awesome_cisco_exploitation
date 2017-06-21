/* $Id: appn_subsys.c,v 3.5.42.6 1996/06/24 09:17:32 fox Exp $
 * $Source: /release/112/cvs/Xsys/appn/appn_subsys.c,v $
 *------------------------------------------------------------------
 * One line description here.
 *
 * May 1995, Steve Berl
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: appn_subsys.c,v $
 * Revision 3.5.42.6  1996/06/24  09:17:32  fox
 * CSCdi59550:  PPP causes router to crash
 * Branch: California_branch
 * Slim down and speed up linktype to NCP index to PPP protocol lookups.
 * Extend NCP protocheck functions to pass original IDB as well as config
 * IDB.  Change NCP names to upper case.  Display NCP name in trace of
 * PPP negotiation.
 *
 * Revision 3.5.42.5  1996/05/21  09:41:33  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5.42.4  1996/05/10  01:35:32  wmay
 * CPP and ppp half bridge commit
 * Branch: California_branch
 *
 * Revision 3.5.42.3  1996/04/29  20:55:23  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Include interface_private.h to provide macros and structures for other
 * includes.
 *
 * Revision 3.5.42.2  1996/04/29  09:33:56  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Move Network Control Protocol (NCP) support from PPP subsystem to
 * individual subsystems.  Attach NCP subsystems to respective protocol
 * subsystems.  Add registry calls to network protocol init code to
 * register default NCP support for NCPs not requiring option negotiation.
 *
 * Revision 3.5.42.1  1996/03/18  18:49:38  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.12.2  1996/03/14  00:52:32  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.5.12.1  1996/03/07  08:27:22  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5  1996/01/05  20:00:37  mregan
 * CSCdi46501:  APPN images crash on boot trying to loab the APPN MIB (68K)
 * removed call to init_appnmib() since it is now (or actually has been)
 * loaded via the SUBSYS_HEADER() macro.
 *
 * Revision 3.4  1995/12/17  18:23:51  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/17  08:37:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:04:49  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  10:52:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/08/12  17:19:36  mregan
 * CSCdi29195:  APPN MIB values incorrect
 * update APPN MIB with latest SNMP changes.
 *
 * Revision 2.4  1995/07/17  21:30:34  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Another missed directory.  :-(
 *
 * Revision 2.3  1995/07/15  02:36:24  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Add a silent_mode parameter to some routines and registries and set it
 * to TRUE when making the calls from SNMP WriteNet, netConfigSet, or
 * hostConfigSet.  Also, add an errmsg for each so the event can still get
 * logged, and make it clear what caused the event.  Fix other callers to
 * pass FALSE so current behavior will be maintained.
 *
 * Revision 2.2  1995/06/22  02:31:53  pleung
 * CSCdi31977:  no debug all does not turn off debug appn
 *              show debug does not show appn debug inf
 *
 * Revision 2.1  1995/06/07  20:03:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "sys_registry.h"
#include "subsys.h"
#include "config.h"
#include "parser.h"
#include "logger.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "../parser/parser_defs_exec.h"
#include "file.h"       /* For tftp_appn_shared_mem() */
#include "ttysrv.h"     /* rdtty prototype */
#include "../ip/ip.h"   /* parse_hostname and martian prototypes */
#include "appn_function.h"
#include "appn_parser.h"
#include "appn_subsys.h"
#include "../ui/debug.h"
#include "interface_private.h"
#include "../wan/ppp.h"
#include "../wan/ppp_registry.h"

extern unsigned int appn_event_mask;
static void appn_debug_init (void);
static void show_appn_debug (void);
static void appn_debug_all (boolean sense);

int _tftp_appn_shared_mem (char *filename, ipaddrtype dumphost);

/*
 * APPN subsystem header
 */
#define APPN_MAJVERSION 1
#define APPN_MINVERSION 0
#define APPN_EDITVERSION  1

SUBSYS_HEADER(appn, APPN_MAJVERSION, APPN_MINVERSION, APPN_EDITVERSION,
              appn_init, SUBSYS_CLASS_PROTOCOL,
              "seq: ppp",
              NULL);


void appn_init (subsystype *subsys)
{ 
    appn_status_byte = 0;
    appn_parser_init();
    clear_config_array();
    appn_debug_init();

    /*
     * Register APPNANRCP with PPP
     */
    reg_invoke_ncp_register(LINK_APPN_ANR, TYPE_PPP_APPNANRCP,
			    NULL, NULL, "APPN");
}


#define APPN_SHMEM_FILE		"appn_shmem"


/*
 * tftp_appn_shared_mem
 * Use TFTP to manually write the APPN Shared Memory partion(s).
 */

void tftp_appn_shared_mem (parseinfo *csb)
{
    ipaddrtype dumphost;
    int parsed;
    char *filename = APPN_SHMEM_FILE;

    /*
     * Get host IP address
     */
    if (GETOBJ(paddr,1)->type == ADDR_IP) {
	dumphost = GETOBJ(paddr,1)->ip_addr;
	parsed = TRUE;
    } else {
	printf("Remote host? ");
	parsed = rdtty(GETOBJ(string,2), PARSEBUF);
	if (parsed) {
	    parsed = parse_hostname(GETOBJ(string,2), &dumphost);
	}
    }
    if (!parsed || martian(dumphost, TRUE)) {
	printf("\n?Bad IP address or host name");
	return;
    }

    /*
     * Get file name
     */
    if (*GETOBJ(string,1) != '\0') {
	filename = GETOBJ(string,1);
	parsed++;
    } else {
	printf("\nName of file to write [%s]? ", filename);
	parsed = rdtty(GETOBJ(string,1), PARSEBUF);
	if (parsed) {
	    char* cp;

	    cp = deblank(GETOBJ(string,1));
	    if (cp && *cp)
		filename = cp;
	}
    }
    if (!parsed) {
	printf("%%Bad file name");
	return;
    }
    printf("\nWrite file %s on host %lu? ", filename, dumphost);
    if (!confirm(NULL))
	return;

    if (_tftp_appn_shared_mem(filename, dumphost))
	printf(" [OK]");
    else
	printf(" [Failed]");
}



/*
 * _tftp_appn_shared_mem
 *
 * Use TFTP to manually write the APPN Shared Memory segment(s).
 *
 * RETURNS:
 *	OK	 (1)
 *	ABORTED  (0)
 */

int _tftp_appn_shared_mem (char *filename, ipaddrtype dumphost)
{
    int      done, seg_nbr;
    char     xfilename[128];
    sys_timestamp timestamp;
    int      memlen = 0;
    char     *membase = NULL;
    filetype file;

    done = 0;
    seg_nbr = 0;
    GET_TIMESTAMP(timestamp);
    membase = NULL;

    while(!done)
    {
	/*  Set the base address and length to the (first/next)
	 *  Shared Memory segment.
	 */
	if (membase == NULL)
	{
	    membase = appn_get_membase();
	    memlen = appn_get_memlen();
	}
	else
	{
	    /* ONLY HANDLE 1 SEGMENT FOR NOW */
	    done = 1;
	    continue;
	}

	printf("\nBase Address = 0x%08lx, Length (in bytes) = 0x%08x\n",
	    (unsigned long)membase, memlen);

	/* Assign a uniqe name for each APPN Shared Memory partition
	 * dump file.
	 *  - A "timestamp" and "segment number" is appended to the
	 *    provided filename.
	 *  - All segments dumped together have the "same" timestamp
	 *    but a unique segment number.
	sprintf(xfilename, "%s.t%#08Ta.a%08lx", filename, timestamp, membase);
	 */
	sprintf(xfilename, "%s.t%#08Ta.%d", filename, timestamp, ++seg_nbr);

	/* Write the file via tftp
	*/
	file.proto = FILE_ACCESS_TFTP;

#ifdef CISCO_NOT
	file.filename = xfilename;
#else
        /* for now don't add the extension                     */
        /*   (compatibility with DOS and we don't support more */
        /*    dumping more than one partition anyways.         */
	file.filename = filename;
#endif

	file.ps.tuba.boothost.type = ADDR_IP;
	file.ps.tuba.boothost.ip_addr = dumphost;
	if (reg_invoke_file_write(file.proto, &file, membase, memlen))
	    continue;
	else
	    return(ABORTED);
    }

    return(OK);
}


/*
 * ---------------------------------------------------------------
 * Debug Functions
 * ---------------------------------------------------------------
 */

static void appn_debug_init ()
{
    static boolean appn_debug_inited = FALSE;

    /* Repeated calls to this routine do nothing */
    if (appn_debug_inited)
        return;

    appn_debug_inited = TRUE;

    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    reg_add_debug_all(appn_debug_all, "appn_debug_all");

    reg_add_debug_show(show_appn_debug, "show_appn_debug"); 

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when APPN is initialized, we want to turn on
     * all the APPN debugging right now.
     */
    appn_debug_all(debug_all_p());

}
/*
 * appn_debug_all()
 */

static void appn_debug_all (boolean sense)
{
     /* if the 'no' prefix is present... */
    if (!sense) { /* Turn off the bits */

       appn_event_mask = 0x000000FF;

    } else { /* Turn on the bits */

       appn_event_mask = 0xFFFFFFFF;

    } /* end if else */

}

/*
 * show_appn_debug()
 */

static void show_appn_debug()
{
    if ( !appn_status ) return; /* appn not started */

    if (appn_event_mask == 0xff) return; /* no debug appn all */

    printf("APPN:\n");
    if ((appn_event_mask & DEBUG_DLUR) == DEBUG_DLUR)
	printf("  dlur debugging is on\n");
    if ((appn_event_mask & DEBUG_PSNAV) == DEBUG_PSNAV)
	printf("  psnav debugging is on\n");
    if ((appn_event_mask & DEBUG_ACS) == DEBUG_ACS) 
	printf("  acs debugging is on\n");
    if ((appn_event_mask & DEBUG_MODULE_TRACE) == DEBUG_MODULE_TRACE) 
	printf("  trace debugging is on\n");
    if ((appn_event_mask & DEBUG_LU) == DEBUG_LU) 
	printf("  lu debugging is on\n");
    if ((appn_event_mask & DEBUG_API) == DEBUG_API) 
	printf("  api debugging is on\n");
    if ((appn_event_mask & DEBUG_LSR) == DEBUG_LSR) 
	printf("  lsr debugging is on\n");
    if ((appn_event_mask & DEBUG_SSA) == DEBUG_SSA) 
	printf("  ssa debugging is on\n");
    if ((appn_event_mask & DEBUG_EGPE_NONTIMER) == DEBUG_EGPE_NONTIMER) 
	printf("  egpe_nontimer debugging is on\n");
    if ((appn_event_mask & DEBUG_EGPE_TIMER) == DEBUG_EGPE_TIMER) 
	printf("  egpe_timer debugging is on\n");
    if ((appn_event_mask & DEBUG_MS) == DEBUG_MS) 
	printf("  ms debugging is on\n");
    if ((appn_event_mask & DEBUG_DLC) == DEBUG_DLC) 
	printf("  dlc debugging is on\n");
    if ((appn_event_mask & DEBUG_HS) == DEBUG_HS) 
	printf("  hs debugging is on\n");
    if ((appn_event_mask & DEBUG_SM) == DEBUG_SM) 
	printf("  sm debugging is on\n");
    if ((appn_event_mask & DEBUG_CS) == DEBUG_CS) 
	printf("  cs debugging is on\n");
    if ((appn_event_mask & DEBUG_NOF) == DEBUG_NOF) 
	printf("  nof debugging is on\n");
    if ((appn_event_mask & DEBUG_SCM) == DEBUG_SCM) 
	printf("  scm debugging is on\n");
    if ((appn_event_mask & DEBUG_PC) == DEBUG_PC) 
	printf("  pc debugging is on\n");
    if ((appn_event_mask & DEBUG_DS) == DEBUG_DS) 
	printf("  ds debugging is on\n");
    if ((appn_event_mask & DEBUG_SS) == DEBUG_SS) 
	printf("  ss debugging is on\n");
    if ((appn_event_mask & DEBUG_PS) == DEBUG_PS) 
	printf("  ps debugging is on\n");
    if ((appn_event_mask & DEBUG_TRS) == DEBUG_TRS) 
	printf("  trs debugging is on\n");
    if ((appn_event_mask & DEBUG_RM) == DEBUG_RM) 
	printf("  rm debugging is on\n");

}
