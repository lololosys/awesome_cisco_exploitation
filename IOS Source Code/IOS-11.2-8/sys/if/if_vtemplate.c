/* $Id: if_vtemplate.c,v 3.1.48.27 1996/09/13 23:15:24 perryl Exp $
 * $Source: /release/112/cvs/Xsys/if/if_vtemplate.c,v $
 *------------------------------------------------------------------
 * if_vtemplate.c - Virtual Template interface services
 *
 * January 1996, Perry Leung
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_vtemplate.c,v $
 * Revision 3.1.48.27  1996/09/13  23:15:24  perryl
 * CSCdi69162:  vtemplate cloning ignore the ppp_only flag after the 1st
 * clone
 * Branch: California_branch
 * Add a field to vaccess_info to keep track of the ppp_only filter on
 * a vaccess
 *
 * Revision 3.1.48.26  1996/08/28  12:48:08  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.48.25  1996/08/28  00:11:22  perryl
 * CSCdi64970:  vtemplate hide printf is too specific
 * Branch: California_branch
 * For IO redirection, Vtemplate uses its own tty socket rather
 * than using registry calls in printf().
 *
 * Revision 3.1.48.24  1996/08/22  07:19:52  perryl
 * CSCdi66647:  Can not tell what a virtual access interface is used for
 * Branch: California_branch
 * Add an addition parameter to reg_invoke_vtemplate_create_vaccess, to
 * describe the type of the new virtual access interface.
 *
 * Revision 3.1.48.23  1996/08/20  20:55:02  syiu
 * CSCdi66353:  Virtual template cloning arranges parser commands backwards
 * Branch: California_branch
 *
 * Revision 3.1.48.22  1996/08/17  00:38:24  sjackson
 * CSCdi66353:  Virtual template cloning arranges parser commands backwards
 * Filter the command buffer in ascending order as opposed to descending
 * Branch: California_branch
 *
 * Revision 3.1.48.21  1996/08/15  18:27:02  perryl
 * CSCdi64970:  vtemplate hide printf is too specific
 * Branch: California_branch
 * Back out the previous commit, because it didn't complete.
 *
 * Revision 3.1.48.20  1996/08/15  00:26:59  perryl
 * CSCdi64970:  vtemplate hide printf is too specific
 * Branch: California_branch
 * Change vtemplate hide printf to more general redirect printf
 * and add printc redirection
 *
 * Revision 3.1.48.19  1996/08/13  02:18:51  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.1.48.18  1996/08/01  18:55:11  perryl
 * CSCdi61809:  Reconfiguring virtual template causes configuration errors
 * Branch: California_branch
 * Buginf error messages from vtemplate (un)cloning, if vtemplate debug
 * is on. In any case, do not print any error message directly to stdio.
 *
 * Revision 3.1.48.17  1996/07/24  16:23:03  syiu
 * CSCdi63910:  Bridging not working with VPDN
 * Branch: California_branch
 *
 * Revision 3.1.48.16  1996/07/16  19:29:51  perryl
 * CSCdi61846:  Vtemplate free vaccess interface tracebacks
 * Branch: California_branch
 * The right fix for CSCdi61846 should be only do encap ppp
 * once when creating vaccess, and none during cleanup.
 * Remove the previous CSCdi61846 fix.
 *
 * Revision 3.1.48.15  1996/07/13  00:10:20  syiu
 * CSCdi62859:  Carrier timeout on Vaccess may be too long
 * Branch: California_branch
 *
 * Revision 3.1.48.14  1996/07/11  19:40:11  syiu
 * CSCdi62545:  Bridging disabled on vtemplate interface
 * Branch: California_branch
 *
 * Revision 3.1.48.13  1996/07/11  18:53:43  perryl
 * CSCdi61846:  Vtemplate free vaccess interface tracebacks
 * Branch: California_branch
 * Clear out the vaccess's lcpfsm field in vtemplate_cleanup_vaccess().
 *
 * Revision 3.1.48.12  1996/07/08  15:57:52  syiu
 * CSCdi62107:  Multichassis config conflict with Dialer
 * Branch: California_branch
 *
 * Revision 3.1.48.11  1996/07/04  02:23:34  perryl
 * CSCdi61809:  Reconfiguring virtual template causes configuration errors
 * Branch: California_branch
 * Add "clear vtemplate" command to reset the vtemplate command history
 * buffer.
 *
 * Revision 3.1.48.10  1996/06/27  16:35:55  hampton
 * Keep your mitts off the scheduler private data structures.  [CSCdi61518]
 * Branch: California_branch
 *
 * Revision 3.1.48.9  1996/06/24  22:54:20  perryl
 * CSCdi59423:  Reconfiguring virtual template causes configuration errors
 * Branch: California_branch
 * Use "default" instead of "no" to unconfigure commands in
 * add_unconfigured_command().
 *
 * Revision 3.1.48.8  1996/06/22  23:47:21  snyder
 * CSCdi60918:  empty seq: and req: fields should be NULL
 * Branch: California_branch
 *              and eliminate some unneccessary req: and seq:
 *              entries.  Hey 200 bytes is 200 bytes
 *
 * Revision 3.1.48.7  1996/06/22  07:39:01  syiu
 * CSCdi59165:  PPP Virtual-Access Int will not negotiate NCPs on
 * projected links
 * Branch: California_branch
 *
 * Revision 3.1.48.6  1996/05/21  09:49:52  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.48.5  1996/05/17  20:47:57  syiu
 * CSCdi57948:  Vaccess interfaces created while being freed
 * Branch: California_branch
 *
 * Revision 3.1.48.4  1996/05/13  21:05:42  perryl
 * CSCdi57039:  Traceback in Vtemplate
 * Branch: California_branch
 * In vtemplate_free_vaccess(), set VTEMPLATE_STATUS_FREED before
 * calling vtemplate_net_cstate(). Therefore if vtemplate_free_vaccess()
 * was called again while the 1st thread is still blocked on
 * VTEMPLATE_STATUS_CHANGE and the idb state is already down, the poor
 * idb won't get enqueue twice to dirtyQ.
 *
 * Revision 3.1.48.3  1996/05/06  18:52:11  perryl
 * CSCdi56515:  Virtual Access IP unnumbered is intrusive
 * Branch: California_branch
 * Virtual Template and Access interfaces are default to "no ip address"
 * when
 * created. Creating virtual access intf with "ip unnumbered" is now an
 * option.
 * For MLP, if there isn't a dialer interface associated with an incoming
 * call,
 * it will create a virtual access interface as the bundle interface with
 * the
 * "ip unnumbered" options. In most cases, the default "no ip address" is
 * used.
 * In selecting "ip unnumbered" source, it will first try to pick up
 * ethernet,
 * token ring, FDDI, ATM, serial and it will not pick dialer, virtual
 * template
 * and virtual access interfaces.
 *
 * Revision 3.1.48.2  1996/04/29  09:38:37  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Move Network Control Protocol (NCP) support from PPP subsystem to
 * individual subsystems.  Attach NCP subsystems to respective protocol
 * subsystems.  Add registry calls to network protocol init code to
 * register default NCP support for NCPs not requiring option negotiation.
 *
 * Revision 3.1.48.1  1996/04/27  05:49:42  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.1.2.15  1996/04/26  01:06:28  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.1.2.14  1996/04/17  01:59:30  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.1.2.13  1996/04/09  01:25:25  syiu
 * Branch: Dial1_branch
 * Get rid of oldstate check in Vaccess interface creation
 *
 * Revision 3.1.2.12  1996/04/08  22:51:19  syiu
 * Branch: Dial1_branch
 * Fix Vaccess clone check for oldstate before blocking
 *
 * Revision 3.1.2.11  1996/04/08  18:06:48  syiu
 * Branch: Dial1_branch
 * Synchronize Vaccess interface state changes. Otherwise route adjusts etc
 * won't work correctly.
 *
 * Revision 3.1.2.10  1996/04/05  23:28:27  perryl
 * CSCdi53772:  PPP virtual template/access int automaticaly assigns
 * unnumbered e0
 * Branch: Dial1_branch
 * Use hw_namestring field to build the "ip unnumbered" command.
 *
 * Revision 3.1.2.9  1996/03/23  00:19:34  perryl
 * Branch: Dial1_branch
 * Fix a looping problem in vtemplate_cleanup_vaccess, introduced by the
 * new one second process approach
 *
 * Revision 3.1.2.8  1996/03/19  23:40:15  syiu
 * Branch: Dial1_branch
 * Clear counters on a freed vaccess interface
 *
 * Revision 3.1.2.7  1996/03/19  22:39:13  perryl
 * Branch: Dial1_branch
 * Eliminate the vtemplate dirtyQ mgr process and the reset cmdlist
 * mgr. Replace them with vtemplate_onesec() which checks the dirtyQ
 * and the reset_cmdlist_flag every sec. If there are works to do
 * it forks off a process to do the works. The process will kill
 * itself when the works is done.
 *
 * Revision 3.1.2.6  1996/03/01  22:24:54  perryl
 * Branch: Dial1_branch
 * Enqueue dirtyQ managing, hide virtual access interface from UI.
 * Fixed cleanup virtual access interface race condition.
 * Fixed max virtual access interface boundary condition.
 * Cleanup debug messages
 *
 * Revision 3.1.2.5  1996/02/19  19:37:05  tkolar
 * Branch: Dial1_branch
 * Remove debugging code.
 *
 * Revision 3.1.2.4  1996/02/10  02:19:40  perryl
 * Branch: Dial1_branch
 * Add debug support for Virtual Template
 *
 * Revision 3.1.2.3  1996/02/06  20:20:58  perryl
 * Branch: Dial1_branch
 *
 * Revision 3.1.2.2  1996/01/18  04:58:11  perryl
 * Branch: Dial1_branch
 * Virtual Access interface services
 *
 * Revision 3.1.2.1  1996/01/18  01:17:38  perryl
 * Branch: Dial1_branch
 * Virtual Access interface services
 *
 * Revision 3.1  1996/01/18  01:02:46  perryl
 * Placeholder for Dial1_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "../wan/ppp_registry.h"
#include "interface_private.h"
#include "ctype.h"
#include "packet.h"
#include "subsys.h"
#include "sched.h"
#include "../h/config.h"
#include "../parser/parser_defs_config.h"
#include "../wan/serial_parser.h"
#include "../os/nv.h"
#include "ttysrv.h"
#include "../parser/parser_actions.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "../wan/serial.h"
#include "../parser/parser_defs_parser.h"
#include "../ts/if_async.h"
#include "network.h"
#include "ether.h"
#include "if_vtemplate.h"
#include "if_vtemplate_debug.h"
#include "../wan/serial_private.h"
#include "../h/stdarg.h"
#include <ciscolib.h>
#include "../h/cs_registry.regh"
#include "../h/auth_registry.regh"
#include "../iprouting/route.h"
#include "../ip/ip_registry.h"


/*
 * Function prototypes
 */
int vtemplate_free_vaccess(hwidbtype *);
static char *vtemplate_filter_if_commands(hwidbtype *, char *, hwidbtype *, boolean);
static void vtemplate_generate_hwaddr(hwidbtype *);
static boolean vtemplate_filter_ppp(char *);
static char *vtemplate_print_type (hwidbtype *);

/*
 * Extern functions reference
 */
extern void  interface_command(parseinfo *csb);
extern void  shutdown_command(parseinfo *csb);

extern uchar *end_of_interface(uchar *config);
extern int generate_command_list(uchar *inbuf, uchar *outbuf, int inbuf_size);
extern uchar *get_command_entry(uchar *command_list, int n); 
extern void add_unconfigured_command(uchar *config_string, uchar *command);

extern vtemplate_parser_init(void);

#define NEWLINE '\n'
#define BLANK ' '
#define EOL '\0'

/* 
 * Global variables
 * vtemplate_vaccessQ - Global unused virtual access interface queue
 * vtemplate_dirtyQ - Global dirty virtual access interface queue
 * vtemplate_reset_cmdlist_flag - reset command list flag
 * vtemplate_num_access - virtual access idb unit# counter
 * vtemplate_bkg_pid - vtemplate background manager's pid
 * vtemplate_curproc - current (un)cloning process list
 * vtemplate_saved_stdio - saved stdio tty
 * vtemplate_io_tty - io redirect tty
 * vtemplate_io_vector - io redirect vector
 */

static queuetype vtemplate_vaccessQ;
static queuetype vtemplate_dirtyQ;
static boolean   vtemplate_reset_cmdlist_flag = FALSE;
static pid_t     vtemplate_bkg_pid = 0;
static uchar     vtemplate_num_vaccess = 1;
static struct    vtemplate_curproc_  vtemplate_curproc[VTEMPLATE_MAXPROC];
static tt_soc   *vtemplate_saved_stdio;
static tt_soc   *vtemplate_io_tty;	
static vectortype vtemplate_io_vector; 
	

/*static const char syshead[] = "    Line     User      Host(s)               Idle Location\n"; */

/*
 * Return the specific Vaccess type
 */
static char *vtemplate_print_type (hwidbtype* hwidb)
{
    switch(hwidb->firstsw->vaccess_info->type) {
      case VTEMPLATE_TYPE_MLP_BUNDLE:  
          return("Virtual PPP (Bundle)");
      case VTEMPLATE_TYPE_L2F:
          return("Virtual PPP (L2F)");
      case VTEMPLATE_TYPE_PPP_ATM_PVC:
          return("Virtual PPP (ATM)");
      case VTEMPLATE_TYPE_VP:
          return("Virtual PPP VDP)");
      case VTEMPLATE_TYPE_PT:
          return("Virtual PPP (PT)");
    }
    return("Unknown");
}

/*
 * Display the Virtual Access type
 */ 
static void vtemplate_systat (void)
{
    char *name;
    hwidbtype *hw;

    FOR_ALL_HWIDBS(hw) {
	if (hw->type == IDBTYPE_VACCESS) {

	    /*
             * Don't display Vaccess interfaces that are freed
             */
            if (hw->firstsw->vaccess_info->type == VTEMPLATE_TYPE_UNKNOWN)
               continue;
 
	    /*
	     * Dump out the display
	     */
	    name = "";
	    (void)reg_invoke_get_authname(hw, &name);
	    printf("  %10s %9s %20s ", hw->hw_short_namestring, name, 
		      vtemplate_print_type(hw));
	    print_dhms(hw->lastinput);
	    printf("\n");
	}
    }
}

static boolean vtemplate_curproc_init (int proc) {

    if ((proc < 0) || (proc >= VTEMPLATE_MAXPROC))
	return FALSE;
    vtemplate_curproc[proc].pid = 0;
    vtemplate_curproc[proc].unit = 0;
    vtemplate_curproc[proc].clonesrc = 0;
    vtemplate_curproc[proc].lindex = 0;
    vtemplate_curproc[proc].prev_caret = FALSE;
    memset(vtemplate_curproc[proc].linebuf,EOL,VTEMPLATE_LINEBUF_SIZE);
    vtemplate_curproc[proc].retcode = VTEMPLATE_SUCCESS;
    return TRUE;
}
/*
 * vtemplate_add_process
 * Add a process pid to the current running process list
 * Record the associated vaccess unit too
 */
static boolean vtemplate_add_process (pid_t pid, int unit, int clone_type) 
{
    int proc;

    for (proc = 0; proc < VTEMPLATE_MAXPROC; proc++) {
	if ((vtemplate_curproc[proc].pid) && 
	    (vtemplate_curproc[proc].pid == pid)) {
	    if (vtemplate_debug) 
		buginf("\nVTEMPLATE multiple passes in vtemplate_add_proc\n");
	    return TRUE;
	}
    }
    for (proc = 0; proc < VTEMPLATE_MAXPROC; proc++) {
	if (!vtemplate_curproc[proc].pid) {
	    vtemplate_curproc[proc].pid = pid;
	    vtemplate_curproc[proc].unit = unit;
	    vtemplate_curproc[proc].clonesrc = clone_type;
	    return TRUE;
	}
    }
    return FALSE;
}

/*
 * vtemplate_delete_process
 * Delete a process pid out of the current running process list
 */
static int vtemplate_delete_process (pid_t pid) 
{ 
    int proc, retcode;

    for (proc = 0; proc < VTEMPLATE_MAXPROC; proc++) {
	if (vtemplate_curproc[proc].pid == pid) {

	    /*
	     * Print out whatever in the line buffer, if there any
	     */
	    if (vtemplate_debug && vtemplate_curproc[proc].lindex) {
		buginf("\nVTEMPLATE Errors from (un)cloning vaccess%d ...\n",
		       vtemplate_curproc[proc].unit);
		buginf("%s",vtemplate_curproc[proc].linebuf);		
	    }
	    retcode = vtemplate_curproc[proc].retcode;
	    vtemplate_curproc_init(proc);
	    return retcode;
	}
    }

    return VTEMPLATE_BAD_PROC;
}


/*
 * vtemplate_putc
 * Buginf the error message from a (un)cloning process
 * if vtemplate debug is turned on.
 */
static void vtemplate_putc (tt_soc *tty, int ch) 
{
    int proc, i;
    int unit = 0;
    int caret;
    boolean found = FALSE;
    char * linebuf = NULL;
    int *lindex = NULL;

    /*
     * Multiple vtemplate process can execute this code
     * at the same time. Hence find out which process this
     * is and add the output to the right line buffer.
     * Also mark the flag for possible command error.
     */
    for (proc = 0; proc < VTEMPLATE_MAXPROC; proc++) {
	if (vtemplate_curproc[proc].pid == THIS_PROCESS) { 
	    unit = vtemplate_curproc[proc].unit;
	    /*
	     * Only consider it as a possible error, when we are
	     * cloning from a AAA command buffer.
	     */
	    if (vtemplate_curproc[proc].clonesrc & VTEMPLATE_CLONE_AAA)
		vtemplate_curproc[proc].retcode = VTEMPLATE_PARSE_ERR;
	    found = TRUE;
	    linebuf = vtemplate_curproc[proc].linebuf;
	    lindex = &(vtemplate_curproc[proc].lindex);
	    break;
	}
    }

    /*
     * if the process is not one of vtemplate (un)cloning process
     * stop here.
     */
    if (!found) {
	if (vtemplate_debug) 
	    buginf("\nVTEMPLATE No matching clone process in IO redirect\n");
	return;
    }
    /*
     * Debug is not turn on, don't need to print the line buffer
     * get out now
     */
    if (!vtemplate_debug) {
	return;
    }

    linebuf[*lindex] = ch;

     /*
     * End of the line buffer, have to terminate it
     * Always leave the last char as EOL
     */
    if (*lindex == VTEMPLATE_LINEBUF_SIZE - 2 ) {
	ch = NEWLINE;
    }
    else
	(*lindex)++;

    /*
     * Not end of a line yet, return now.
     */
    if ((ch != NEWLINE))
	return;

    /*
     * Check the content for a single caret
     */
    for (caret = 0, i = 0; i < *lindex; i++) {
	switch (linebuf[i]) {
	case '^':
	    caret++;
	    break;
	case BLANK:
	case NEWLINE:
	case EOL:
	case '\r':
	case '\t':
	    break;
	default:
	    caret += 2;
	}
    }

    /*
     * Print the error headline, if the content 
     * 1) is not a caret
     * 2) is not a blank line
     * 3) previous line doesn't contain a single caret
     * A single caret is typically used in printing out syntax error
     * on a command.
     */
    if ((caret > 1) && !vtemplate_curproc[proc].prev_caret)
	buginf("\nVTEMPLATE Error from (un)cloning vaccess%d ...\n",unit);
    buginf("%s",linebuf);

    if (caret == 1)
	vtemplate_curproc[proc].prev_caret = TRUE;
    else
	vtemplate_curproc[proc].prev_caret = FALSE;
    /*
     * Reset the line buffer
     */
    *lindex = 0;
    memset(linebuf,EOL,VTEMPLATE_LINEBUF_SIZE);
}


/*
 * vtemplate_safe_exec
 *
 * Temporily set blocking_disable to FALSE to allow the process to be suspended
 * ( blocked ). The parser every 250ms will suspend the process to let other
 * process runs, if there are ready processes.
 */
static int vtemplate_safe_exec (hwidbtype *hwidb, char *buf, int clone_type)
{
    int retcode;
    boolean save_blocking_disabled = FALSE;

    /*
     * Increment the prevent reset counter
     */
    set_vaccess_prevent_reset(hwidb->firstsw);
    /*
     * add this process to the current vtemplate process list
     * swap out the stdio and swap in the vtemplate redirect tty
     */
    vtemplate_add_process(THIS_PROCESS, hwidb->unit, clone_type);
    vtemplate_saved_stdio = stdio;
    process_set_ttysoc(THIS_PROCESS, vtemplate_io_tty); 

    /*
     * Allow the process to be suspended
     */
    save_blocking_disabled = blocking_disabled;
    blocking_disabled = FALSE;
    hwidb->firstsw->vaccess_status |= VTEMPLATE_STATUS_CONFIG;

    parse_configure_idb(buf, TRUE, RES_CONFIG, PRIV_ROOT, hwidb->firstsw);

    hwidb->firstsw->vaccess_status &= ~VTEMPLATE_STATUS_CONFIG;

    /*
     * Restore blocking_disabled
     */
    blocking_disabled = save_blocking_disabled;

    /*
     * Swap back the real stdio and delete this process out
     * of the current vtemplate process list
     */
    process_set_ttysoc(THIS_PROCESS, vtemplate_saved_stdio);
    retcode = vtemplate_delete_process(THIS_PROCESS);

    /*
     * Decrement the prevent reset counter
     */
    clear_vaccess_prevent_reset(hwidb->firstsw);

    return retcode;
}

/*
 * vtemplate_common_initidb
 *
 * Initialize a hwidb for vtemplate or vaccess
 */ 
static void vtemplate_common_initidb(hwidbtype *hwidb)
{
    /* 
     * Initialize encapsulation and virtual/template status
     */
    hwidb->status =  IDB_VTEMPLATE | IDB_SERIAL | IDB_VIRTUAL | IDB_BRIDGE;

    /*
     * Initialize idb vectors
     */
    hwidb->setencap = serial_encapsulation; /* Handle "encapsulation" command */
    hwidb->soutput = (soutput_t) return_nothing; /* Start pending output */
    hwidb->reset = (reset_t) return_nothing;
    hwidb->set_maxdgram = (set_maxdgram_t) return_false;
    hwidb->device_periodic = (device_periodic_t) return_nothing;
    hwidb->linestate = (linestate_t) return_true;
    hwidb->shutdown = (shutdown_t) return_nothing;
    hwidb->fastsend = (fastsend_t) return_nothing;
    hwidb->oqueue = holdq_enqueue;
    hwidb->oqueue_dequeue = holdq_dequeue;

    hwidb->firstsw->vpn_save_iqueue = NULL;
    hwidb->firstsw->vpn_save_oqueue = NULL;
    hwidb->firstsw->vpn_save_oqueue_dequeue = NULL;
    hwidb->firstsw->ip_passive_router = 0;

    /*
     * Reset the state to DOWN
     */
    hwidb->oldstate = hwidb->state = IDBS_DOWN;

    /*
     * Reset all counters 
     */
    clear_idb(hwidb);

    /*
     * Virtual Access interfaces always have 0 carrier delay
     */
    hwidb->carrierdelay = hwidb->carrierdelay_def = 0;

}


/*
 * vtemplate_clone_defaults
 *
 * Clone the default setting to a virtual access hwidb
 * The default is PPP encapsulation
 */
static void vtemplate_clone_defaults (hwidbtype *vaccessidb, 
			       boolean use_unnumbered,
			       boolean encap_ppp) 
{
    char *out_buf = NULL;
    hwidbtype *hwidb= NULL;
    idbtype *idb;
    boolean  found = FALSE;

    if((out_buf = malloc_named(VTEMPLATE_CONFBUF_SIZE,"VTEMPLATE clone default"))) {

	/*
	 * If use_unnumber is set, look for an unnumber source
	 * The source must be running IP and must not use unnumber itself
	 * It will first try to find a preferred interface likes
	 * an ethernet, token ring, serial, ATM or FDDDI first.
	 * if there are none, than use any idb except dialer, 
	 * virtual template and virtual access interfaces.
	 */
	if (use_unnumbered) {
	    FOR_ALL_HWIDBS(hwidb) {
		idb = hwidb->firstsw; 
		if (!(hwidb->status & (IDB_ETHER|IDB_TR|IDB_FDDI|
				       IDB_SERIAL|IDB_ATM)))
		    continue;
		if (hwidb->status & IDB_ASYNC)
		    continue;

		if (!idb->ip_enabled)	
		    continue;
		if (idb->ip_unnumbered)	
		    continue;
		if (vaccessidb == idb->hwptr) 
		    continue;
		found = TRUE;
		break;
	    }
	    if (!found) {
		FOR_ALL_HWIDBS(hwidb) {
		    idb = hwidb->firstsw;
		    if (hwidb->status & (IDB_DIALER|IDB_VTEMPLATE))
			continue;
		    if (!idb->ip_enabled)	
			continue;
		    if (idb->ip_unnumbered)	
			continue;
		    if (vaccessidb == idb->hwptr) 
			continue;
		    found = TRUE;
		    break;
		}
	    }
	}

	if (found) {
	    sprintf(out_buf, "interface %s%d\nip unnumbered %s\n",
		    vaccessidb->name, vaccessidb->unit, 
		    hwidb->hw_namestring); 
	}
	else {
	    sprintf(out_buf, "interface %s%d\nno ip address\n",
		    vaccessidb->name,vaccessidb->unit); 
	}

	if (encap_ppp)
	    sprintf(out_buf+strlen(out_buf), "encap ppp\nend\n");
	else
	    sprintf(out_buf+strlen(out_buf),"end\n");

	if (vtemplate_debug) {
	    buginf("\nVTEMPLATE set default vaccess%d with %s %s\n",
		   vaccessidb->unit, 
		   use_unnumbered ? "ip unnumbered" : "no ip address",
		   encap_ppp ? ", encap ppp" : " ");
	}

	vtemplate_safe_exec(vaccessidb, (char *) out_buf, 0);
	
	setstring(&out_buf,NULL);
    }
}


/*
 * vtemplate_unclone_defaults
 *
 * Unclone the default setting to a virtual access hwidb
 */
static void vtemplate_unclone_defaults (hwidbtype *hwidb) 
{    
    char defaults[] = "\nno ip address\n";
    char *out_buf = NULL;

    if ((out_buf = malloc_named(VTEMPLATE_CONFBUF_SIZE,"VTEMPLATE unclone default"))) { 
	sprintf(out_buf, "interface %s%d%send\n",hwidb->name, hwidb->unit,defaults);

	if (vtemplate_debug)
	    buginf("\nVTEMPLATE undo default settings vaccess%d\n", hwidb->unit);

	vtemplate_safe_exec(hwidb, (char *) out_buf, 0);
	setstring(&out_buf, NULL);
    }
}


/*
 * vtemplate_common_createidb
 *
 * Create an idb that can be used as virtual template or virtual access
 * It has the default setting of PPP, ip unnumbered.
 */
static hwidbtype *vtemplate_common_createidb (enum IDBTYPE type, uint unit,
				       boolean use_unnumbered)
{
    hwidbtype *hwidb;
    idbtype *swidb;
    char buffer[VTEMPLATE_NAME_SIZE];

    /*
     * Error checking 
     */
    if ((type != IDBTYPE_VTEMPLATE) && (type != IDBTYPE_VACCESS))
    {
	if (vtemplate_debug)
	    buginf("\nVTEMPLATE Bad IDB type %x in creating IDB\n",type);
	return NULL;
    }
    	
    /*
     * Set up storage, add IDB to interface queue
     */
    hwidb = idb_create();
    if (!hwidb)
	return(NULL);
    swidb = hwidb->firstsw;

    /*
     * Set up generic driver information
     */
    if (type == IDBTYPE_VTEMPLATE)
    {
	hwidb->type = IDBTYPE_VTEMPLATE;
	hwidb->typestring = "Virtual Template interface";
	hwidb->name = IFNAME_VTEMPLATE;	
    }
    else
    {
	hwidb->type = IDBTYPE_VACCESS;
	hwidb->typestring = "Virtual Access interface";
	hwidb->name = IFNAME_VACCESS;
    }
    
    hwidb->slotunit = hwidb->unit = unit;

    set_default_interface_bandwidth(swidb, BANDWIDTH_SCALE/ 100);
    set_default_interface_delay(swidb, 10000);


    /*
     * Our Async specific DTR pulse time
     */
    hwidb->serial_pulseinterval = VTEMPLATE_DEFAULT_PULSEINTERVAL;
    hwidb->serial_pulseinterval_def = VTEMPLATE_DEFAULT_PULSEINTERVAL;

    /************************************************************************
     * The follow code is a modification of init_async_idb_common()
     */

    /*
     * Call vtemplate_common_initidb to reset vectors, status, encapsulation
     */
    vtemplate_common_initidb(hwidb);

    /*
     * Set PPP as default encapsulation 
     * vtemplate_clone_default() will finish PPP encap setting.
     */
    hwidb->enctype = ET_NULL;
    hwidb->enctype_def = hwidb->enctype_cfg = ET_PPP;

    /*
     * Set up generic driver information
     */
    hwidb->state = IDBS_DOWN;
    GET_TIMESTAMP(hwidb->state_time);
    hwidb->vc = NO_VC;
    hwidb->reliability = 255;
    hwidb->load = 1;
    holdq_init(hwidb, VTEMPLATE_HOLDQ);

    sprintf(buffer, "%s%d", hwidb->name, hwidb->unit);
    setstring_named(&swidb->namestring, buffer, "VTEMPLATE namestr");
    hwidb->hw_namestring = swidb->namestring;

    sprintf(buffer, "%c%c%d", hwidb->name[0], hwidb->name[1], hwidb->unit);
    setstring_named(&swidb->short_namestring, buffer, "VTEMPLATE shortname");
    hwidb->hw_short_namestring = swidb->short_namestring;

    /*
     * Other stuff not yet cleaned up
     */
    hwidb->input_defqcount = INPUTQ_DEFAULT;
    hwidb->keep_period = KEEPALIVE_DEFAULT;
    hwidb->maxmtu = hwidb->maxmtu_def = MAXETHERSIZE; /* set the maximum MTU */
    hwidb->max_pak_size_def = MAXETHERSIZE;
    if_maxdgram_adjust(hwidb, MAXETHERSIZE);
    hwidb->buffersize = MAXETHERSIZE;

    /*
     * Initialize protocol state
     */
    create_serial_sb(hwidb);    
    reg_invoke_sw_idb_init(swidb, TRUE);
    media_idb_enqueue(hwidb);

    /*
     * Setup no ip address or ip unnumber
     * Setup ppp encapsulation, only need to do this once
     */
    vtemplate_clone_defaults(hwidb, use_unnumbered, TRUE);

    return(hwidb);
}


/*
 * vtemplate_alloc_vaccess
 *
 * Allocate virtual access idb. However, if there are unused idbs in
 * the vtemplate_vaccessQ, it will reuse them first.
 */
static hwidbtype* vtemplate_alloc_vaccess (uint unit, boolean use_unnumbered)
{
    hwidbtype      *hwidb = NULL;
    int             count, queue_size, unnumber;
    vaccess_info_block *member;


    if (!unit) {
	if (vtemplate_num_vaccess <= VTEMPLATE_MAXVACCESS) {
	    unnumber = TRUE;
	}
	else {
	    printf("\nMax # of virtual access interfaces %d are allocated\n",
		   VTEMPLATE_MAXVACCESS);
	    return (NULL);
	}
    }
    else
	unnumber = FALSE;

    if (unit <= VTEMPLATE_MAXVACCESS) {
	/*
	 * If there are unused virtual access interface, use it
	 */
	queue_size = QUEUESIZE(&vtemplate_vaccessQ);
	for (count = 0; count < queue_size; count++) {
	    member = dequeue(&vtemplate_vaccessQ);
	    if (unnumber || member->hwidb->unit == unit) {   
		if (vtemplate_debug)
		    buginf("\nVTEMPLATE Reuse vaccess%d, New Recycle queue size:%d\n",
			   member->hwidb->unit, QUEUESIZE(&vtemplate_vaccessQ));


		/* trun off the FREED flag */
		member->hwidb->firstsw->vaccess_status &=
		    ~VTEMPLATE_STATUS_FREED;
		/*
		 * Obtain the default setting, use_unnumbered may
		 * be TRUE or FALSE
		 * Do not need to setup ppp encap, it is already setup
		 */
		vtemplate_clone_defaults(member->hwidb, use_unnumbered, FALSE);

		return member->hwidb;
	    }
	    else
		enqueue(&vtemplate_vaccessQ, member);
	}
    } else {
	printf("\n%%Largest virtual access interface unit# allowed is %d", VTEMPLATE_MAXVACCESS);
	return (NULL);
    }

    if (vtemplate_debug)
	buginf("\nVTEMPLATE No unused vaccess, create new vaccess%d\n",unit);

    if (unnumber) {
	unit = vtemplate_num_vaccess;
	vtemplate_num_vaccess++;
    }

    if ((hwidb = vtemplate_common_createidb(IDBTYPE_VACCESS, unit, use_unnumbered))) {
	hwidb->firstsw->vaccess_info =
	    malloc_named(sizeof(vaccess_info_block), "VTEMPLATE vaccess info block");
	if (NULL == hwidb->firstsw->vaccess_info) {
	    if (vtemplate_debug)
		buginf(nomemory);
	    /* Well, can't free the idb, just leave it shut down as is */
	    return (NULL);
	}
	hwidb->firstsw->vaccess_info->hwidb = hwidb;

	hwidb->firstsw->vaccess_dirtyQ_info =
	    malloc_named(sizeof(vaccess_info_block), "VTEMPLATE vaccess dirtyQ info block");
	if (NULL == hwidb->firstsw->vaccess_dirtyQ_info) {
	    if (vtemplate_debug)
		buginf(nomemory);
	    /* Well, can't free the idb, just leave it shut down as is */
	    return (NULL);
	}
	hwidb->firstsw->vaccess_dirtyQ_info->hwidb = hwidb;

	hwidb->firstsw->vaccess_status = 0;

	return (hwidb);

    } else {
	if (vtemplate_debug)
	    buginf(nomemory);
	return (NULL);
    }
}

/*
 * Called when the interface state changes
 */
static void vtemplate_if_state_change (hwidbtype *hwidb)
{
  if (hwidb->type != IDBTYPE_VACCESS)
     return;

  /* 
   * Mark the vtemplate status flag
   */
  hwidb->firstsw->vaccess_status |= VTEMPLATE_STATUS_CHANGE;
  
}

/*
 * Block waiting for change in interface vaccess status
 */
static boolean vaccess_BLOCK (hwidbtype *hwidb)
{

  if (hwidb->firstsw->vaccess_status & VTEMPLATE_STATUS_CHANGE)
     return(FALSE);

  return(TRUE);
}

/*
 * Set the netstate to specified state. Block until state change 
 * is done by network background process
 */
static boolean vtemplate_net_cstate (hwidbtype *hwidb, int state)
{
   if (!hwidb)
      return(FALSE);

   hwidb->firstsw->vaccess_status &= ~VTEMPLATE_STATUS_CHANGE;
   net_cstate(hwidb, state);
   edisms((blockproc *)vaccess_BLOCK, (ulong)hwidb);
   hwidb->firstsw->vaccess_status &= ~VTEMPLATE_STATUS_CHANGE;
   return(TRUE);
} 

/*
 * vtemplate_create_vaccess
 *
 * Create virtual access interface, it first visits the vtemplate
 * recycle queue, if there is a unused vaccess, it will recycle it.
 * Otherwise a brand new vaccess is allocated.
 *
 * If block is specified, it will bring UP interface and block waiting
 * for state to change
 * If use_numbered is specified, it will create the vaccess with a
 * valid ip unnumbered XXX. Otherwise no ip address
 *
 * type should be one of the enum VACCESS_TYPE in if/if_vtemplate.h 
 * to record what is this vaccess is used for.
 */
hwidbtype *vtemplate_create_vaccess(boolean block, boolean use_unnumbered, 
				    int type)
{
    hwidbtype *hwidb;

    /*
     * Create a Virtual Access Interface and bring the interface
     * UP. Block waiting for it to be really up
     */
    hwidb = vtemplate_alloc_vaccess(0, use_unnumbered);

    if (!hwidb)
	return(NULL);

    /*
     * Fill in the type field and ppp_only field
     */
    if ((type > VTEMPLATE_TYPE_UNKNOWN) && (type < VTEMPLATE_TYPE_LASTDEFINE))
	hwidb->firstsw->vaccess_info->type = type;
    else
	hwidb->firstsw->vaccess_info->type = VTEMPLATE_TYPE_UNKNOWN;

    hwidb->firstsw->vaccess_info->ppp_only = FALSE;

    /*
     * set interface logging to default
     * Put here because we want the flags valid until the vaccess is used
     *    again.
     */
    hwidb->logging_events = 0;

    /*
     * Get a hardware address for this interface
     */
    vtemplate_generate_hwaddr(hwidb);
  
    if (!block)
	return(hwidb);
    
    if (vtemplate_net_cstate(hwidb, IDBS_UP) && 
        interface_up_simple(hwidb->firstsw)) {
        return(hwidb);
    } else {
        vtemplate_free_vaccess(hwidb);  
        return (NULL);
    }
}

/*
 * vtemplate_create_vtemplate
 *
 * Create virtual template interface
 */
static idbtype* vtemplate_create_vtemplate (uint unit)
{
    hwidbtype      *hwidb = NULL;

    if (unit < VTEMPLATE_MAXVTEMPLATE) {

	if (vtemplate_debug)
	    buginf("\nVTEMPLATE Create vtemplate%d\n", unit);
	/*
	 * If the idb already exists, return it;
	 */
	FOR_ALL_HWIDBS(hwidb) {
	    if ((hwidb->type == IDBTYPE_VTEMPLATE) && (hwidb->unit == unit))
		break;
	}
	if (NULL == hwidb) 
	  hwidb = vtemplate_common_createidb(IDBTYPE_VTEMPLATE, unit, FALSE);

	if (hwidb) {
	    return (hwidb->firstsw);
	} else {
	    if (vtemplate_debug)
		buginf(nomemory);
	    return (NULL);
	}
    }
    printf("\n%%Unit number must be less than %d", VTEMPLATE_MAXVTEMPLATE);

    return (NULL);
}

/*
 * Use the default mac address as the mac address, see
 * network.c for more info on default_mac_addr 
 */
static void vtemplate_generate_hwaddr (hwidbtype *hwidb)
{

    ieee_copy(default_mac_addr, hwidb->hardware);

    if (vtemplate_debug)
	buginf("\n%s VTEMPLATE hardware address %e", 
               hwidb->hw_namestring, hwidb->hardware);
}

/*
 * vtemplate_print_cmdbuf
 *
 * Printing a large command list buffer in one buginf may cause
 * log overrun problem. 
 * This routine is to break up the command list buffer
 * and print each command individually.
 */
static void vtemplate_print_cmdbuf(char *cmd_buf)
{
    char *out_buf, *head, *tail;
    int count;

    /*
     * Duplicate the command list buffer to a temp buffer for printing
     */
    out_buf = malloc_named(VTEMPLATE_CONFBUF_SIZE,"VTEMPLATE print buffer");
    if (out_buf == NULL)
	return;
    bcopy(cmd_buf, out_buf, VTEMPLATE_CONFBUF_SIZE);

    /*
     * Loop thru the command list buffer, look for NEWLINE.
     * If a NEWLINE is found, it indicates we found the end of
     * a command. If so, print out the command and advance the
     * pointers to the next command.
     * The loop is terminate when we reach the end of the buffer
     * or we find a NULL, which indicate the end of the command
     * buffer list.
     */

    count = 0;
    head = out_buf;
    tail = out_buf;

    while ((*tail != 0) && (count < VTEMPLATE_CONFBUF_SIZE)) {
	/*
	 * If it is a NEWLINE, it is the end of a command line
	 * print it out and advance the head pointer to the next
	 * command.
	 */
	if (*tail == NEWLINE) {
	    *tail = 0;
	    buginf("%s\n",head);
	    head = tail + 1;
	}
	count++;
	tail++;
    }

    free(out_buf);
}

/*
 * vtemplate_passive_interface
 * This routine clone the vtemplate's passive router mask to its
 * active virtual access interfaces and adjust their routes. 
 */  
void vtemplate_passive_interface (hwidbtype *hwidb, pdbtype *pdb, 
				  boolean sense)
{
    int i;
    vaccess_info_block *member = NULL;

    if (!hwidb || !hwidb->firstsw || !pdb)
	return;

    for(i=0; i<QUEUESIZE(&hwidb->firstsw->vtemplate_memberQ); i++) {
	member = dequeue(&hwidb->firstsw->vtemplate_memberQ);
	    reg_invoke_ip_passive_cmd_route_adjust(pdb->proctype, pdb, 
						   member->hwidb->firstsw,
						   sense);
	enqueue(&hwidb->firstsw->vtemplate_memberQ,member);
    }
}

    

/*
 * vtemplate_clone_vaccess
 * Clone from various sources of interface configuration to the vaccess
 * interface.
 * Args:
 * clone_type = 
 *        VTEMPLATE_CLONE_VTEMPLATE Clone from Vtemplate 
 *        VTEMPLATE_CLONE_DIALER Clone from Dialer Interface
 *        VTEMPLATE_CLONE_AAA Clone from buffer (from AAA)
 * template_num = Vtemplate Number (if applicable)
 * *dialer_hwidb = ptr to Dialer hardware idb (if applicable)
 * *buffer = ptr to buffer of interface commands from AAA (if applicable)
 * *vaccess_idb = ptr to Virtual Access interface where commands are to be 
 *                copied to
 * block = TRUE -  bring interface UP and block waiting for it to come up 
 *                before returning control
 *         FALSE - no blocking & interface UP required
 * ppp_only = TRUE - filter so that only PPP commands from source config 
 *                   is cloned to vaccess_idb
 *            FALSE - all commands from source config cloned to vaccess_idb
 *
 * Note:
 * If a virtual access idb still cloned to a virtual template idb, it first 
 * must be freed. In addition it takes the virtual access out of the 
 * vtemplate_vaccessQ and puts it in the virtual template's memberQ, so 
 * all the changes to virtual template, will also be updated to the 
 * virtual access idb.
 */
int vtemplate_clone_vaccess (int clone_type,
			     uchar template_num, 
			     hwidbtype *dialer_hwidb,
			     uchar *buffer,
			     hwidbtype *vaccess_idb,
			     boolean block,
			     boolean ppp_only)
{
    hwidbtype      *hwidb = NULL;
    char           *out_buf = NULL;

    /*
     * Check for arguments that must be there
     */
    if (!vaccess_idb)
       return (VTEMPLATE_BAD_VACCESS);

    switch(clone_type) {
       case VTEMPLATE_CLONE_DIALER:
         if (!dialer_hwidb)
            return (VTEMPLATE_BAD_VACCESS);
     
         /*
          * Clone by copying PPP fields from the Dialer interface
          */
         reg_invoke_media_copy_encap_fields(ET_PPP, dialer_hwidb, vaccess_idb);
         break;

       case VTEMPLATE_CLONE_AAA:
         return (VTEMPLATE_BAD_VACCESS);
       
       case VTEMPLATE_CLONE_VTEMPLATE:

         /*
          * find the virtual template hwidb
          */
         FOR_ALL_HWIDBS(hwidb) {
	    if ((hwidb->type == IDBTYPE_VTEMPLATE) && 
                (hwidb->unit == template_num))
	        break;
         }
   
         if (NULL == hwidb) 
	    return (VTEMPLATE_NO_VTEMPLATE);

         if (NULL == vaccess_idb)
	    return (VTEMPLATE_BAD_VACCESS);

         /*
          * The virtual access interface still clone from a virtual template
          */
         if (vaccess_idb->firstsw->vaccess_info->vtemplate) {
	    if (vtemplate_debug)
	        buginf("\nVTEMPLATE vaccess%d still cloned from vtemplate%d\n",
		   vaccess_idb->unit,
		   vaccess_idb->firstsw->vaccess_info->vtemplate->unit); 
	    return (VTEMPLATE_BAD_VACCESS);
         }

	 if (vaccess_idb->firstsw->vaccess_status & VTEMPLATE_STATUS_FREED) {
	     buginf("\nVTEMPLATE try to clone a FREED vaccess%d\n",
		    vaccess_idb->unit);
	     return(VTEMPLATE_BAD_VACCESS);
	 }

         /*
          * all uncloned vaccess has default conf, so 1st is to undo 
          * the defaults. Then clone the virtual template hwidb to the 
          * virtual access hwidb
          */
         vtemplate_unclone_defaults(vaccess_idb);

	 /*
	  * keep track of the ppp_only status
	  *
	  * Copy over the passive interface mask, the individual
	  * routing protocol will reference this mask when creating
	  * its data structure for this vaccess. For example,
	  * for eigrp, when we clone the ip address to the vaccess
	  * the iidb is created and is enqueuee to the regular or passive
	  * queue based on the mask.
	  */
	 vaccess_idb->firstsw->vaccess_info->ppp_only = ppp_only;
	 vaccess_idb->firstsw->ip_passive_router = 
	     hwidb->firstsw->ip_passive_router;
	 /*
	  * Filter out non ppp commands, if ppp_only is TRUE
	  */
         out_buf = vtemplate_filter_if_commands(hwidb, NULL, vaccess_idb, 
                                                ppp_only);
         if (!out_buf)
            return (VTEMPLATE_NOMEMORY);

         vtemplate_safe_exec(vaccess_idb, (char *) out_buf, clone_type);

         setstring(&out_buf,NULL);
    
         /*
          * put it into the virtual template's queue 
          * assign the vaccess's vtemplate pointer to the virtual template 
          */
         enqueue(&hwidb->firstsw->vtemplate_memberQ,
	        vaccess_idb->firstsw->vaccess_info);
         vaccess_idb->firstsw->vaccess_info->vtemplate = hwidb;
         break;

       default:
         return (VTEMPLATE_BAD_VACCESS);  
    }

    /*
     * Virtual Access interfaces always have 0 carrier delay
     * Override any interface configs
     */
    vaccess_idb->carrierdelay = vaccess_idb->carrierdelay_def = 0;

    /*
     * If Block requested, bring the Vaccess interface UP.
     * This interface should not already be in the UP state
     */
    if (block) {
       if (vaccess_idb->state == IDBS_UP) {
           if (vtemplate_debug)
	      buginf("VTEMPLATE Warning: %s already UP", 
                     vaccess_idb->hw_namestring);
           return (VTEMPLATE_SUCCESS);
       }
       vtemplate_net_cstate(vaccess_idb, IDBS_UP);
    }
    return (VTEMPLATE_SUCCESS);
}

/*
 * vtemplate_filter_if_commands
 * If ppp_only is FALSE, copy the command list from the Vtemplate's 
 * hwidb OR a given command list TO a newly allocated buffer and return it. 
 * Otherwise, do pretty much the same thing but allow only PPP commands
 * in the new buffer.
 */
static char *vtemplate_filter_if_commands (hwidbtype *vtemplate_hwidb,
char *command_list, hwidbtype *vaccess_hwidb, boolean ppp_only)
{
    int num_commands = -1;
    int count;
    char *listptr, *out_buf, *save_buf, *ptr;
    char *temp_command_list = NULL;

    /*
     * Set up a command list from the two possible sources.
     * The two sources should be exclusive of each other but
     * we don't check for it. The vtemplate command list simply 
     * supercedes the given command_list. 
     */
    if (vtemplate_hwidb)
       temp_command_list = vtemplate_hwidb->firstsw->command_list;
    else if (command_list)
       temp_command_list = command_list;

    if (!temp_command_list)
       return(NULL);

    /*
     * Allocate a buffer 
     */
    save_buf = out_buf = malloc_named(VTEMPLATE_CONFBUF_SIZE,"VTEMPLATE clone buffer");
    if (!out_buf)
       return(NULL);

    bzero(out_buf, VTEMPLATE_CONFBUF_SIZE);
    
    if (!ppp_only) {
       sprintf(out_buf, "interface %s%d%send\n",
	    vaccess_hwidb->name, vaccess_hwidb->unit,
	    temp_command_list);
    } else {

        sprintf(out_buf, "interface %s%d\n",
	    vaccess_hwidb->name, vaccess_hwidb->unit);

        while (*out_buf != 0)
             out_buf++;

        /*
         * Count the total number of commands in the template interface
         */
	for (num_commands = -1, listptr = temp_command_list; 
             *listptr; listptr++) {
	    if (NEWLINE == *listptr)
		num_commands++;
        }

        /*
         * Go thru each command. If it is a PPP command, copy it to
         * newly allocated buffer. 
         */
       	for (count = 0; count <= num_commands; count++) {
	    ptr = get_command_entry(temp_command_list, count);
            if (ptr && vtemplate_filter_ppp(ptr)) {
               while ((*ptr != NEWLINE) && (*ptr != 0)) {
                 *out_buf++ = *ptr++;
               } 
               *out_buf++ = NEWLINE;   
	    }
	}
        strcat(out_buf, "end\n");
    }
    if (vtemplate_debug) {
	buginf("\nVTEMPLATE ************* CLONE VACCESS%d *****************\n",
	       vaccess_hwidb->unit);
       if (vtemplate_hwidb)
          buginf("VTEMPLATE Clone from vtemplate%d to vaccess%d\n",
                  vtemplate_hwidb->unit,vaccess_hwidb->unit);
       else 
          buginf("VTEMPLATE Clone from AAA to vaccess%d\n",
                  vaccess_hwidb->unit);

	vtemplate_print_cmdbuf(save_buf);
    }
    return(save_buf);
}

/*
 * vtemplate_filter_ppp
 *
 * Return TRUE for all PPP commands - includes all commands starting 
 * with "pp". Also "ke" for keepalive, "mt" for mtu and all commands starting 
 * with "def" for default. We include all default commands in the PPP  
 * filter simply because it's too cumbersome to check for all PPP commands 
 * (mentioned above) that start with def, defa, defau etc. 
 * Since default commands generally do not put a protocol 
 * config in the member Vaccess interfaces, this should be OK.
 * Return FALSE if PPP command is not found.
 *
 * Note: Yes, yes, this requires maintenance to make sure all PPP
 *       commands are included.
 */
static boolean vtemplate_filter_ppp (char *ptr)
{
    if ((strncmp(ptr,"pp",2) == 0) ||
        (strncmp(ptr,"no pp",5) == 0) ||
        (strncmp(ptr,"ke",2) == 0) ||
        (strncmp(ptr,"no ke",5) == 0) ||
	(strncmp(ptr,"mt",2) == 0) ||
        (strncmp(ptr,"no mt",5) == 0) ||
        (strncmp(ptr,"def",3) == 0)) {
        return(TRUE);
    }
    return(FALSE);
}

/*
 * vtemplate_free_vaccess
 *
 * This routine enqueue the virtual access idb to the  unused dirty idb 
 * queue,  vtemplate_dirtyQ. 
 * Every second, vtemplate_onesec() looks at vtemplate_dirtyQ, if
 * there is dirty idb in there it will fork off a process to
 * run vtemplate_background_mgr()to cleanup the dirty idb
 * Before vtemplate_background_mgr() cleanup the dirty idb, the idb
 * remains in its virtual template queue, so it remains in sync
 * with the virtual template's command list.
 */

int vtemplate_free_vaccess (hwidbtype * vaccessidb)
{
    idbtype        *vaccess_sw = NULL;


    if (NULL == vaccessidb)
	return (VTEMPLATE_BAD_VACCESS);

    if (!(vaccess_sw = vaccessidb->firstsw))
	return (VTEMPLATE_BAD_VACCESS);

    if (VTEMPLATE_STATUS_FREED & vaccessidb->firstsw->vaccess_status) {
	if (vtemplate_debug)
	    buginf ("\nVTEMPLATE try to free a freed vaccess%d\n",
		    vaccessidb->unit);
	return (VTEMPLATE_ALREADY_FREED);
    }
    /*
     * Turn on the FREED status 
     * Reset ppp_only field
     */
    vaccessidb->firstsw->vaccess_status |= VTEMPLATE_STATUS_FREED;
    vaccessidb->firstsw->vaccess_info->ppp_only = FALSE;

    if (vtemplate_debug)
	buginf("\nVTEMPLATE Free vaccess%d\n", vaccessidb->unit);

    /*
     * change the idb state to down.
     * vtemplate_net_cstate will block until the idb state
     * goes to down
     * reset type field, the type field is used, during
     * vtemplate_net_cstate()
     */
    if (vaccessidb->state != IDBS_DOWN)
	vtemplate_net_cstate(vaccessidb, IDBS_DOWN);
    vaccessidb->firstsw->vaccess_info->type = VTEMPLATE_TYPE_UNKNOWN;

    /*
     * Copy the vtemplate pointer to the dirtyQ_info  
     * enqueue the virtual access hwidb to the vtemplate_dirtyQ, 
     * the dirty virtual access interface queue.
     */
    vaccess_sw->vaccess_dirtyQ_info->vtemplate  =
	vaccess_sw->vaccess_info->vtemplate;
    enqueue(&vtemplate_dirtyQ, vaccess_sw->vaccess_dirtyQ_info);

    return VTEMPLATE_SUCCESS;

}


/*
 * vtemplate_cleanup_vaccess
 *
 * This routine cleanup any cloned "dirty" virtual access idbs in the 
 * vtemplate_dirtyQ. It takes the dirty idb out of its
 * virtual template queue and put it into the vtemplate_vaccessQ.
 * It unclones (cleanup) the dirty idb, by running a negated 
 * reserved ordered command list to the idb.
 * Then it resets the idb back to its default setting.
 */
static int  vtemplate_cleanup_vaccess(void)
{
    idbtype        *vtemplate_sw = NULL;
    hwidbtype      *vtemplate = NULL;
    hwidbtype      *vaccessidb = NULL;
    int             num_commands = -1;
    int             count;
    int             qsize, cnt;
    char          *listptr;
    char          *out_buf = NULL;
    vaccess_info_block *member = NULL;
    vaccess_info_block *dirty = NULL;

    if (vtemplate_debug)
	buginf("\nVTEMPLATE  clean up dirty vaccess\n");

    while (TRUE) {	
	/*
	 * exist if there is no more dirty interface
	 */
	dirty = dequeue(&vtemplate_dirtyQ);
	if (NULL == dirty)
	    return (VTEMPLATE_SUCCESS);

	vtemplate = dirty->vtemplate;
	vaccessidb = dirty->hwidb;
       
	/*
	 * if the virtual access interface is clean,
	 * put it back to the vtemplate_vaccessQ
	 */
	if (NULL == vtemplate) {
	    vtemplate_clone_defaults(vaccessidb, FALSE, FALSE);
	    enqueue(&vtemplate_vaccessQ, vaccessidb->firstsw->vaccess_info);
	    continue;
	}
   
	/*
	 * The vaccess was cloned from a vtemplate
	 * First dequeue the vaccess from the vtemplate queue
	 */
	vtemplate_sw = vtemplate->firstsw;
	qsize = QUEUESIZE(&vtemplate_sw->vtemplate_memberQ);
	for (cnt = 0; cnt < qsize; cnt++) {
	    member = dequeue(&vtemplate_sw->vtemplate_memberQ);
	    if (member->hwidb == vaccessidb)
		break;
	    else
		enqueue(&vtemplate_sw->vtemplate_memberQ, member);
	}
    
	/*
	 * Erase the virtual access interface configuration by using the virtual
	 * template command list. Negate the command list and run it thru the
	 * parser
	 */	

	if(vtemplate_debug)
	    buginf("\nVTEMPLATE Found a dirty vaccess%d\n",vaccessidb->unit);

	/*
	 * Count the number of commands in the command list. The command list
	 * always begin with a NEW LINE and each command is terminated with a
	 * NEW LINE
	 */
	for (num_commands = -1, listptr = vtemplate_sw->command_list; *listptr; listptr++)
	    if (NEWLINE == *listptr)
		num_commands++;

	/*
	 * Allocate double the size of a command list for the unclone
	 * buffer to ensure we have enough space
	 */
	if (NULL == (out_buf =
		     malloc_named(VTEMPLATE_MAXBUF_SIZE,"VTEMPLATE unclone buffer"))) {
	    if (vtemplate_debug)
	    buginf("\nVTEMPLATE can not alloc memory for the unclone buffer.\n");
	    return (VTEMPLATE_NOMEMORY);
	}
	bzero(out_buf, VTEMPLATE_MAXBUF_SIZE);
	sprintf(out_buf, "interface %s%d\n", vaccessidb->name, vaccessidb->unit);

	/* 
	 * build the negated command list in the reserve order 
	 * run the negated command list thru the parser
	 */
	for (count = num_commands; count > 0; count--) {
	    add_unconfigured_command(out_buf, get_command_entry(vtemplate_sw->command_list, count));
	}

	strcat(out_buf, "end\n");

	if (vtemplate_debug) {
	    buginf("\nVTEMPLATE ************ UNCLONE VACCESS%d **************",
		   vaccessidb->unit);
	    buginf("\nVTEMPLATE Unclone to-be-freed vaccess%d command#%d\n",
		   vaccessidb->unit,num_commands);
	    vtemplate_print_cmdbuf(out_buf);
	}

	vtemplate_safe_exec(vaccessidb, (char *)out_buf, VTEMPLATE_CLONE_VTEMPLATE);
    
	setstring(&out_buf,NULL);

	/*
	 * Null the virtual access interface 's vtemplate pointer
	 * this indicate the vaccess is clean.
	 * Put the default setting back in
	 */
	member->vtemplate = NULL;

	/*
	 * Setup default setting; 
	 * no ip address
	 * ppp encapsulation is setup, do not need to redo it
	 */
	vtemplate_clone_defaults(vaccessidb, FALSE, FALSE);
	vtemplate_common_initidb(vaccessidb);

       /*
        * Put the virtual access interface back into the queue
        */
	enqueue(&vtemplate_vaccessQ, member);
	if(vtemplate_debug)
	    buginf("\nVTEMPLATE Add vaccess%d to recycle queue, size=%d",
		   vaccessidb->unit,QUEUESIZE(&vtemplate_vaccessQ));
    }

}


/*
 * vtemplate_reset_cmdlist
 *
 * vtemplate_reset_cmdlist() obtains the current configuration from
 * nv_current_to_buffer(). Then it parse thru the whole configuration to
 * build command lists for the virtual template interfaces.
 * This routine is called when a virtual template interface's command
 * list has reached its max. Since user entered command list may 
 * contain duplicated commands, so the actual number of non-duplicated 
 * commands for a virtual template interface could be less. 
 * Since this routine generates the command list from the current 
 * configuration and by default the current configuration is a 
 * non-duplicated set of commands, the resulting command list could 
 * be smaller.
 */  
static void vtemplate_reset_cmdlist(void)
{
    char          *cfgptr = NULL;
    char          *unitptr;
    char          *my_buf = NULL;
    char          *cmdlist = NULL;
    char          *config_buf = NULL;
    char           interface_name[VTEMPLATE_NAME_SIZE];
    uint           num_commands;
    int            unit = 0;
    uint           my_bufsize;
    uint           config_bufsize, nv_size;
    hwidbtype     *hwidb;
    boolean        save_blocking_disabled = FALSE;

    if (vtemplate_debug) 
	buginf("\nVTEMPLATE start to reset command list\n");

    config_bufsize = nv_get_config_size();
    nv_size = config_bufsize;
    while (((config_buf = malloc_named(nv_size,"VTEMPLATE NV buffer")) == NULL) && 
	   (nv_size > 0x3fff)) {
	nv_size = nv_size >> 1;
    }

    if (config_buf == NULL) {
	if (vtemplate_debug) 
	    buginf("\nVTEMPLATE No memory for reset cmdlist buffer\n");
	goto vtemplate_reset_cmdlist_exit;
    }

    /*
     * get a set of buffers to save interface configurations and command
     * lists in.
     */
    if (NULL == (my_buf = malloc_named(VTEMPLATE_MAXBUF_SIZE,
				       "Reset cmdlist buffer"))) {
	if (vtemplate_debug) 
	    buginf("\nVTEMPLATE No memory for reset cmdlist buffer\n");
	goto vtemplate_reset_cmdlist_exit;
    }

    if (NULL == (cmdlist = malloc_named(VTEMPLATE_MAXBUF_SIZE,
				       "Reset cmdlist buffer"))) {
	if (vtemplate_debug) 
	    buginf("\nVTEMPLATE No memory for reset cmdlist buffer\n");
	goto vtemplate_reset_cmdlist_exit;
    }

    /*
     * Allow the process to be suspended
     */
    save_blocking_disabled = blocking_disabled;
    blocking_disabled = FALSE;

    if (!(nv_current_to_buffer(config_buf, nv_size, TRUE, PRIV_ROOT, FALSE))) {
	if (vtemplate_debug) 
	    buginf("\nVTEMPLATE Reset Cmd list Did not read any configuration\n");
	blocking_disabled = save_blocking_disabled;
	goto vtemplate_reset_cmdlist_exit;
    }    
    blocking_disabled = save_blocking_disabled;

    cfgptr = config_buf;

    while (cfgptr) {
	/*
	 * Scan thru the configuration file and look for virtual template
	 * interface
	 */

	sprintf(interface_name, "interface %s", IFNAME_VTEMPLATE);
	if (NULL == (cfgptr = strstr(cfgptr, interface_name))) {
	    break;			/* no more virtual template */
	}
	/*
	 * find  the virtual template interface idb
	 * skip "interface Virtual-Template", advance to the unit number
	 */
	for (unitptr = cfgptr; !(('9' >= *unitptr) && ('0' <= *unitptr)); unitptr++) {
	};

	/* find the unit number */
	for (unit = 0; (('9' >= *unitptr) && ('0' <= *unitptr)); unitptr++) {
	    unit *= 10;
	    unit += *unitptr - '0';
	}

	if (0 == unit) {
	    if (vtemplate_debug)
		buginf("\nVTEMPLATE Reset Cmdlist missing unit#\n");
	    goto vtemplate_reset_cmdlist_exit;	    
	}
	FOR_ALL_HWIDBS(hwidb) {
	    if ((IDBTYPE_VTEMPLATE == hwidb->type) && (unit == hwidb->unit))
		break;
	}
	if (NULL == hwidb) {
	    if (vtemplate_debug)
		buginf("\nVTEMPLATE Reset Cmdlist Missing vtemplate%d\n", unit);
	    goto vtemplate_reset_cmdlist_exit;
	}
	
	bzero(cmdlist, VTEMPLATE_MAXBUF_SIZE);
	bzero(my_buf, VTEMPLATE_MAXBUF_SIZE); 
	my_bufsize = (int) end_of_interface(cfgptr) - (int) cfgptr;

	/*
	 * generate the commund list from the configuration buffer
	 */
	if (my_bufsize > VTEMPLATE_MAXBUF_SIZE) {
	    if (vtemplate_debug)
		buginf("\nVTEMPLATE Config file size is %d, buffer size is %d",
		       my_bufsize, VTEMPLATE_MAXBUF_SIZE);
	    bcopy(cfgptr, my_buf, VTEMPLATE_MAXBUF_SIZE);
	    my_buf[VTEMPLATE_MAXBUF_SIZE - 1] = 0;
	    my_buf[VTEMPLATE_MAXBUF_SIZE - 2] = NEWLINE;
	}
	else
	    bcopy(cfgptr, my_buf, my_bufsize);

	num_commands = generate_command_list(my_buf, cmdlist,
					     my_bufsize);

	if (0 == num_commands) {
	    if (hwidb->firstsw->command_list)
		setstring(&(hwidb->firstsw->command_list),NULL);
	    hwidb->firstsw->command_list = NULL;
	} else {

	    if (NULL == hwidb->firstsw->command_list) {
		hwidb->firstsw->command_list =
		    malloc_named(VTEMPLATE_CMDLIST_SIZE,"VTEMPLATE command list");
		if (NULL == hwidb->firstsw->command_list) {
		    if (vtemplate_debug) 
			buginf("\nVTEMPLATE No Memory when creating command list\n");
		    goto vtemplate_reset_cmdlist_exit;
		}
	    }
	    bzero(hwidb->firstsw->command_list, VTEMPLATE_CMDLIST_SIZE);
	    if (strlen(cmdlist) >= VTEMPLATE_CMDLIST_SIZE) {
		printf("%% virtual template%d configuration is too big, pls reduce # of commands",hwidb->unit);
		if (vtemplate_debug)
		    buginf("\nVTEMPLATE Cmd list is too big %d",strlen(cmdlist));
		/* 
		 * The command list is larger than the max command
		 * list size, force to the the max command list size
		 */
		bcopy(cmdlist, hwidb->firstsw->command_list, VTEMPLATE_CMDLIST_SIZE);
		hwidb->firstsw->command_list[VTEMPLATE_CMDLIST_SIZE -  2] = NEWLINE;
		hwidb->firstsw->command_list[VTEMPLATE_CMDLIST_SIZE - 1] = 0;
	    } 
	    else
		bcopy(cmdlist, hwidb->firstsw->command_list, strlen(cmdlist));		
	    if (vtemplate_debug)
		buginf("\nVTEMPLATE Unit: %d, # of commands: %d%s\n",
		       unit, num_commands, hwidb->firstsw->command_list);
	}

	/* advance cfgptr */
	cfgptr += my_bufsize;

    } /* end of configuration buffer */

    if (vtemplate_debug) 
	buginf("\nVTEMPLATE reset command list successful\n");

 vtemplate_reset_cmdlist_exit:
    setstring(&config_buf, NULL);
    setstring(&my_buf, NULL);
    setstring(&cmdlist, NULL);
    return;


}


/*
 * vtemplate_background_mgr
 *
 * vtemplate_background_mgr() manages the vtemplate_dirtyQ.
 * It is called by vtemplate_onesec when there is a dirty idb in 
 * the "dirty" virtual access idb.
 *
 * vtemplate_background_mgr() also manages the vtemplate_reset_cmdlist_flag
 * When the flag is set by vtemplate_config_changed(), vtemplate_onesec()
 * will call vtemplate_background _mgr. In turn vtemplate_background_mgr()
 * will call vtemplate_reset_cmdlist() to reset the command list.
 * Since vtemplate_reset_cmdlist() obtains the current configuration
 * like in write term, which takes a long time (6 sec), so it is
 * better to do in background process rather than on the same UI process
 * that set the flag. 
 */
static process vtemplate_background_mgr (void)
{
    if (QUEUESIZE(&vtemplate_dirtyQ))
	vtemplate_cleanup_vaccess();

    if (vtemplate_reset_cmdlist_flag) {
	vtemplate_reset_cmdlist();
	vtemplate_reset_cmdlist_flag = FALSE;
    }

    vtemplate_bkg_pid = 0;
    process_kill(THIS_PROCESS);
}
	
/* 
 * vtemplate_onesec()
 *
 * vtemplate_onesec() manages the vtemplate_dirtyQ and the
 * vtemplate_cmdlist_reset_flag.
 * If there is new dirty idb in vtemplate_dirtyQ or
 * the vtemplate_cmdlist_reset_flag is TRUE, 
 * it will fork off vtemplate_background_mgr() to 
 * cleanup the dirty idbs or to reset the command list
 * If an instant of vtemplate_background_mgr() is active,
 * it will not fork off a new instant.
 */
static void vtemplate_onesec (void)
{

    if (QUEUESIZE(&vtemplate_dirtyQ) ||
	vtemplate_reset_cmdlist_flag) {
	if (!vtemplate_bkg_pid)
	vtemplate_bkg_pid = process_create(vtemplate_background_mgr,
		       "VTEMPLATE Background Mgr", NORMAL_STACK, PRIO_LOW);
    }
}

#ifdef TESTING
static int      test_create = 1;
static int      test_free = 1;
static int      test_double_clone = 0;
static int      test_double_free = 1;
static uchar    aaabuf[]="username ff password ff\ne \nkeepalive 20\n ppp multilink\n ppp fdfd\nipx delay 10\n     end\nappletalk\nppp auth chap\nasync mode\n";


static void test_stub (void)
{
    hwidbtype *vaccessidb = NULL;
    hwidbtype *vaccessidb2 = NULL;
    int retcode;

    if (test_create) {
	if ((vaccessidb = vtemplate_create_vaccess(FALSE, FALSE, 
						   VTEMPLATE_TYPE_VP))) {
	    vaccessidb2 = vtemplate_create_vaccess(FALSE, FALSE, 
						   VTEMPLATE_TYPE_VP);
	    retcode = vtemplate_clone_vaccess(VTEMPLATE_CLONE_VTEMPLATE,
					      2, NULL, NULL,
 					      vaccessidb2, FALSE, FALSE);    
	    retcode = vtemplate_clone_vaccess(VTEMPLATE_CLONE_VTEMPLATE,
					      1, NULL, NULL,
 					      vaccessidb, FALSE, FALSE);
	    vtemplate_clone_vaccess( VTEMPLATE_CLONE_AAA, 
				    1, NULL, aaabuf, 
				    vaccessidb, FALSE, FALSE);
	    if (test_double_clone) {
		vtemplate_clone_vaccess( VTEMPLATE_CLONE_VTEMPLATE, 
					1, NULL, NULL, 
					vaccessidb, FALSE, FALSE);
		vtemplate_clone_vaccess( VTEMPLATE_CLONE_AAA, 
					1, NULL, aaabuf, 
					vaccessidb, FALSE, FALSE);
		vtemplate_clone_vaccess( VTEMPLATE_CLONE_DIALER, 
					0, NULL, NULL, 
					vaccessidb, FALSE, FALSE);
	    }
	}
    }

    if (test_free) {
	vtemplate_free_vaccess(vaccessidb);
	if (test_double_free) {
	    vtemplate_free_vaccess(vaccessidb);
	}
    }
}
#endif


/*
 * vtemplate_config_changed
 *
 * This function is called from the parser every time a command is
 * executed.  If the command refers to a virtual template idb,
 * the vtemplate_memberQ is walked and the command is executed against
 * each member idb.
 * In addition the command is copy into the command_list.
 */
static void vtemplate_config_changed (parseinfo * csb)
{
    hwidbtype      *hwidb = hwidb_or_null(csb->interface);
    idbtype        *original_idb;
    idbtype        *member_idb;
    int             queue_size, count, cmdlen;
    queuetype      *memberQ = NULL;
    uchar          *cmdlist, *listptr, *cmdptr;
    vaccess_info_block *member;
    boolean        is_ppp = FALSE;

    if (hwidb && (IDBTYPE_VTEMPLATE == hwidb->type)) {

	/*
	 * Do validation checks
	 * - do not record interface command 
	 * - do not record shutdown command
	 */
	if ((interface_command == csb->action_func) ||
	    (shutdown_command == csb->action_func))
	    return;

	/*
	 * Add the command to the command list
	 */
	if (NULL == (cmdlist = hwidb->firstsw->command_list)) {
	    if (NULL == (cmdlist =
			 malloc_named(VTEMPLATE_CMDLIST_SIZE,"VTEMPLATE command list"))) {
		if (vtemplate_debug) 
		    buginf("\nVTEMPLATE No Memory when creating command list\n");
		return;

	    }
	    bzero(cmdlist, VTEMPLATE_CMDLIST_SIZE);
	    hwidb->firstsw->command_list = cmdlist;
	    *cmdlist = NEWLINE;
	    listptr = cmdlist + 1;
	} else {
	    /* advance to end of command list */
	    for (listptr = cmdlist; *listptr; listptr++) {
	    };
	}

	/*
	 * remove all leading blank space on the command
	 */
	for (cmdptr = csb->line, cmdlen = strlen(cmdptr); cmdlen; cmdptr++, cmdlen--)
	    if (BLANK != *cmdptr)
		break;


	/*
	 * check do we have any space for this command 
	 * (listptr - cmdlist) : used space 
	 * csb->line_index : size of the current command 
	 * 2 : 1 for new line char and 1 for buffer termination NULL
	 *
	 * if command list is full, signal to reset the command list
	 * else record the command.
	 */
	if (VTEMPLATE_CMDLIST_SIZE < ((listptr - cmdlist) +
				      csb->line_index + 2)) {
	    vtemplate_reset_cmdlist_flag = TRUE;
	} 
	else {
	    /*
	     * copy the command line to the command list 
	     */
	    bcopy(cmdptr, listptr, cmdlen);
	    listptr[cmdlen] = NEWLINE;
	}

	if (vtemplate_filter_ppp(cmdptr)) 
	    is_ppp = TRUE;

	/* populate the command to its virtual interfaces */
	original_idb = csb->interface;
	memberQ = &hwidb->firstsw->vtemplate_memberQ;
	queue_size = QUEUESIZE(memberQ);

	if (vtemplate_debug)
	    buginf("\nVTEMPLATE Template %d Member:%d Command:%s\n", hwidb->unit, queue_size, csb->line);

	/*
	 * Run the command to all the member virtual access interfaces
	 * If the command is a PPP command, do not apply it to the 
	 * vaccess clone with ppp_only on.
	 * turn on the prevent reset counter.
	 */
	for (count = 0; count < queue_size; count++) {
	    member = dequeue(memberQ);
	    enqueue(memberQ, member);
	    member_idb = firstsw_or_null(member->hwidb);
	    if (member_idb) {
		if (!member_idb->vaccess_info->ppp_only || is_ppp) {
		    csb->interface = member_idb;
		    set_vaccess_prevent_reset(member_idb);
		    csb->action_func(csb);
		    clear_vaccess_prevent_reset(member_idb);
		}
	    }
	}
	csb->interface = original_idb;

    }
}

void vtemplate_clear_cmdbuf(parseinfo *csb) 
{
#ifdef TESTING	
    test_stub();
#endif

    printf("\nReset virtual templates' command history buffers\n");
    vtemplate_reset_cmdlist_flag = TRUE;
}

static boolean vtemplate_parser_createidb (uint64 *ifcode, parseinfo *csb)
{
    if (*ifcode == IFTYPE_VTEMPLATE) {
	csb->createidb = vtemplate_create_vtemplate;
	return(TRUE);
    }
    return(FALSE);
}


/*********************************************************************
 *
 *	       DISABLE & RE-ENABLE OUTPUT FOR THE CALLING PROCESS
 *			(redirect to /dev/null)
 *
 *********************************************************************/

/*
 * Various null IO vectors.  All just return as if everything has been
 * sent or received as the case may be.
 */
static int vtemplate_io_return_minus_one (tt_soc *tty)
{
    return(-1);
}

static int vtemplate_io_return_zero (tt_soc *tty)
{
    return(0);
}

static void vtemplate_io_just_return (tt_soc *tty)
{
    return;
}

static boolean vtemplate_io_return_false (tt_soc *tty) 
{
    return(FALSE);
}

static int vtemplate_io_putpkt (tt_soc *tty, paktype *pak,
			   uchar *datastart, int len)
{
    return(len);
}

static boolean vtemplate_io_servicehook (tt_soc *tty, txserv_t txhook,
				    rxserv_t rxhook)
{
    return(FALSE);
}

/*
 * vtemplate_io_init
 * Initializes the vtemplate_io_tty for io redirection.
 * The vtemplate_io_tty replaces stdio, so vtemplate can trap all
 * the error messages, during (un)cloning virtual access interfaces.
 */
static void vtemplate_io_init (void)
{
    /*
     * Set all vectors to a 'safe' function address before filling
     * in the real vectors.
     */
    vector_init(&vtemplate_io_vector);

    vtemplate_io_vector.vgetc = vtemplate_io_return_minus_one;
    vtemplate_io_vector.vclearinput = vtemplate_io_just_return;
    vtemplate_io_vector.vinputpending = vtemplate_io_return_zero;
    vtemplate_io_vector.vputc = vtemplate_putc;
    vtemplate_io_vector.vputpkt = vtemplate_io_putpkt;
    vtemplate_io_vector.voutputblock = vtemplate_io_return_false;
    vtemplate_io_vector.vstartoutput = vtemplate_io_return_false;
    vtemplate_io_vector.voutputpending = vtemplate_io_return_zero;
    vtemplate_io_vector.vclearoutput = vtemplate_io_just_return;
    vtemplate_io_vector.vsetsflow = vtemplate_io_just_return;
    vtemplate_io_vector.vservicehook = vtemplate_io_servicehook;
    vtemplate_io_vector.vnoservicehook = vtemplate_io_just_return;

    vtemplate_io_tty = malloc_named(sizeof(tt_soc), "VTEMPLATE TTY");
    if (vtemplate_io_tty != NULL) {
	tty_init_data(vtemplate_io_tty, 0, READY, BAD_LINE, &vtemplate_io_vector);
	vtemplate_io_tty->tty_length = 0;
    }
}

/*
 * vtemplate_init
 */
static void vtemplate_init (subsystype *subsys)
{
    int i;

    /*
     * Initialize the vtemplate_dirtyQ
     * Initialize the vtemplate_vaccessQ
     */
    queue_init(&vtemplate_dirtyQ, 0);
    queue_init(&vtemplate_vaccessQ, 0);

    reg_add_onesec(vtemplate_onesec, "vtemplate_onesec");

    reg_add_create_idb(vtemplate_parser_createidb,
		       "vtemplate_parser_createidb");
    reg_add_config_changed(vtemplate_config_changed,
			   "vtemplate_config_changed");
    reg_add_vtemplate_create_vaccess(vtemplate_create_vaccess,
				     "vtemplate_create_vaccess");
    reg_add_vtemplate_clone_vaccess(vtemplate_clone_vaccess,
				    "vtemplate_clone_vaccess");
    reg_add_vtemplate_free_vaccess(vtemplate_free_vaccess,
				   "vtemplate_free_vaccess");  
    reg_add_vtemplate_passive_interface(vtemplate_passive_interface,
					"vtemplate_passive_interface");
    
    reg_add_hwif_state_change(vtemplate_if_state_change, "vtemplate_if_state_change");

    reg_add_systat(vtemplate_systat, "vtemplate_systat");

    /*
     * initialize the vtemplate io redirection tty
     * and the current process array
     */
    vtemplate_io_init();
    for (i = 0; i<VTEMPLATE_MAXPROC; i++) 
	vtemplate_curproc_init(i);

   /*
    * Debug initialization chores
    */
   vtemplate_debug_init();
    /*
     * Parser initialization
     */
    vtemplate_parser_init();
} 

/*
 * Virtual Template  subsystem header
 */
#define VTEMPLATE_MAJVERSION 1
#define VTEMPLATE_MINVERSION 0
#define VTEMPLATE_EDITVERSION 1

SUBSYS_HEADER(vtemplate_sys,
              VTEMPLATE_MAJVERSION, VTEMPLATE_MINVERSION,
              VTEMPLATE_EDITVERSION,
              vtemplate_init, SUBSYS_CLASS_LIBRARY,
              NULL,
              NULL);
