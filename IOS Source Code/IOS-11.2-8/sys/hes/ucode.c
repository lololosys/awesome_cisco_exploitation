/* $Id: ucode.c,v 3.14.6.6 1996/08/28 12:46:15 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/ucode.c,v $
 *------------------------------------------------------------------
 * ucode.c - downloadable microcode support
 * 
 *  William H. Palmer, Tue Apr 14 17:34:34 1992  
 *
 *  Copyright (c) 1992-1997 by cisco Systems, Inc.  
 *  All rights reserved. 
 *------------------------------------------------------------------
 * $Log: ucode.c,v $
 * Revision 3.14.6.6  1996/08/28  12:46:15  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.14.6.5  1996/07/08  22:25:47  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.14.6.4  1996/05/21  09:48:14  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.14.6.3  1996/05/02  17:14:02  rbabu
 * CSCdi44138:  rsp becomes unresponsive when microcode is reloaded from
 * rom
 * Branch: California_branch
 *              Don't allow IPs in RSP to load ucode from ROM.
 *
 * Revision 3.14.6.2  1996/04/03  22:49:08  jwelder
 * CSCdi47348:  lingering references to rvip
 * Branch: California_branch
 *
 * Revision 3.14.6.1  1996/03/18  19:44:10  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.9.2.4  1996/03/16  06:52:07  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.9.2.3  1996/03/07  09:41:03  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.9.2.2  1996/02/20  00:53:34  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.14  1996/03/02  23:00:20  dbath
 * CSCdi50584:  mis-specified micro file results in disabled board
 * Fix broken logic whereby even after we'd successfully loaded
 * the system bundle ucode as a backup, we'd fail to realise we'd
 * done it, and so wouldn't start the board.  While the bonnet is
 * up, tidy up some enums, and shuffle some code to enable other
 * modules to manipulate the ucode database in the future.
 *
 * Revision 3.13  1996/02/22  14:33:29  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.12  1996/02/16  15:26:29  banderse
 * CSCdi47098:  CIP ucode sneaks into slot0:   NOT !!
 * Change wording on default flash images; add indication of file presence.
 *
 * Revision 3.11  1996/02/01  06:02:53  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.10  1996/01/29  07:27:52  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.9.2.1  1996/01/24  21:48:36  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.9  1996/01/18  22:47:45  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.8  1996/01/07  13:51:24  banderse
 * CSCdi45351:  show microcode should display CIP flash default
 * if_cip.c - cleanup up printout.
 * ucode.c,
 * ucode.h  - handle IP flash default from mkbndl FLASH keyword.
 *
 * Revision 3.7  1995/12/28  08:18:54  foster
 * CSCdi46157:  Dbus down-load needs to work on rsp2 slave down-load
 *
 * Revision 3.6  1995/12/17  18:27:39  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.5  1995/12/07  22:28:46  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.4  1995/11/30  16:24:48  banderse
 * CSCdi44666:  cip reloader breaks micro <IP> flash on older images
 * Return to embedded flash configuration statements without device prefix.
 *
 * Revision 3.3  1995/11/29  18:45:33  banderse
 * CSCdi43438:  CIP needs default microcode flash image
 * Added release specific default flash image; changes to ensure flash file
 * system is initialized completely before IP download.
 *
 * Revision 3.2  1995/11/17  09:21:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:43:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:01:21  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/09/12  18:22:14  kramling
 * CSCdi37548:  FEIP Parity error requires OIR or Power cycle to clear it
 *
 * Revision 2.4  1995/07/31  21:28:48  schaefer
 * CSCdi37941: OIR processes should block
 * remove edisms
 *
 * Revision 2.3  1995/07/17  08:21:25  schaefer
 * CSCdi36181: more robust microcode management (on RSP)
 *
 * Revision 2.2  1995/06/18  23:17:56  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:45:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include <ciscolib.h>
#include "subsys.h"
#include "mgd_timers.h"
#include "config.h"
#include "parser.h"
#include "sys_registry.h"
#include "../dev/flash_registry.h"
#include "../filesys/filesys.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_exec.h"
#include "../hes/parser_defs_ucode.h" 
#include "interface_private.h"
#include "if_mci.h"
#include "if_fci.h"
#include "dbus.h"
#include "ucode.h"
#include "parser_defs_ucode.h" 
#include "../dev/flash_spec.h"	
#include "../dev/flash_dvr_spec.h"
#include "../filesys/flash_command.h"
#include "../hes/slot.h" 
#include "ucode_hdr.h"
#include "logger.h"
#include "file.h"
#include "ucode_compress.h"
#include "dgbus.h"
#include "msg_ucode.c"			/* Not a typo, see logger.h */
#include "msg_dbus.c"
 
#include "../ipc/ipc_util.h"
#include "../ipc/ipc.h"

extern void hotswap_preallocate_memory(boolean);
extern void hotstall_interrupt(void);

static struct ucodetype_ ucode_prefs[FCI_NUM_CONTROLLERS];

static uchar* ucode_buf;  /* large buffer used for both uncompressing bundled
			     ucode and downloading from flash */

static mgd_timer ucode_loader_timer;


/*
 * The array below must be in the same order as 
 * enum UCODE_CTRLR_TYPE, since it is used as
 * an index into this array.
 */
static const char* const ctrlr_names[] = {
    "EIP",			/* 0 - FCI_ETHER_CONTROLLER */
    "FIP",			/* 6 - FCI_FDDIT_CONTROLLER */
    "FSIP",			/* 7 - FCI_SIP_CONTROLLER */
    "HIP",			/* 3 - FCI_LINK_CONTROLLER */
    "SIP",			/* 2 - FCI_SERIAL_CONTROLLER */
    "SP",			/* 1 - Switch Processor */
    "SSP", 			/* 10 - Silicon Switch Processor */
    "TRIP",			/* 5 - FCI_CTR_CONTROLLER */
    "AIP",			/* 8 - FCI_AIP_CONTROLLER */
    "MIP",			/* 7 - FCI_MIP_CONTROLLER */
    "CIP",                      /* 13 - FCI_CIP_CONTROLLER */
    "FEIP",                     /* 19 - FCI_FEIP_CONTROLLER */
    "VIP",                      /* 20 - FCI_RVIP_CONTROLLER */  
    "VIP2",                     /* 21 - FCI_SVIP_CONTROLLER */
    "RSP2",                     /* 15 - FCI_RSP2_CONTROLLER */
    "POSIP",
};	

static const char* const microcode_srcs[] = {
    "flash",
    "rom",
    "system",
    "<none>",
};

/*
 * microcode config commands
 */
#define ALTERNATE NONE
#include        "cfg_micro.h"
LINK_POINT      (ucode_config_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * microcode show commands
 */
#define ALTERNATE	NONE
#include "exec_show_microcode.h"
LINK_POINT(microcode_show_commands, ALTERNATE);
#undef  ALTERNATE

static parser_extension_request ucode_parse_init_table[] = {
    { PARSE_ADD_CFG_TOP_CMD, &pname(ucode_config_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(microcode_show_commands) },
    { PARSE_LIST_END, NULL},
};

static void ucode_parser_init (void)
{
    parser_add_command_list(ucode_parse_init_table, "downloadable ucode");
}

/*
 * ctrlr_users_foreign_parse_chain()
 * 
 * Returns TRUE if the ctrlr_type uses a foreign parse chain (i.e. a parse
 * chain different from the "micro" parse chain) for controlling boot images.
 * XXX RSP4 alert,  either add it to the list below, or use a "family"
 */
static boolean ctrlr_uses_foreign_parse_chain (ctrlr_type_t ctrlr_type)
{
    switch (ctrlr_type) {
    case FCI_RSP2_CONTROLLER:
	return TRUE;
    default:
	return FALSE;
    }
}

/*
 * ip_uses_directory
 * 
 * Returns TRUE if the ctrlr_type utilizes a directory structure for
 * boot images.
 */
static inline boolean ip_uses_directory (uint ctrlr_type) {
    
    switch (ctrlr_type) {
    case FCI_CIP_CONTROLLER:
	return TRUE;
    default:
	return FALSE;
    }
}

/*  
 * dbus_no_system_ucode
 *
 * Return TRUE if the ctrlr_type doesn't have operational code in system.
 * The CIP does not have operational code in the system.  This may 
 * become more common as size of ucode increases.  
 */
static inline boolean dbus_no_system_ucode (uint ctrlr_type)
{
    switch (ctrlr_type) {
    case FCI_CIP_CONTROLLER:
	return TRUE;
    default:
	return FALSE;
    }
}

static const char *get_flash_default (const char *string, boolean name)
{
    static char full_flashname[MAX_LEN_FLASH_FILENAME + 
			       sizeof(UCODE_FLASH_DEFAULT_DEVICE) + 1];
    int i;

    if (name) {           /* get full filename from cardtype */
	for (i=0; i<flash_default_files; i++) {
	    if (strstr(flash_toc[i].filename,string)) {
		build_whole_file_id(full_flashname, UCODE_FLASH_DEFAULT_DEVICE,
			    (char *)flash_toc[i].filename);
		return(full_flashname);
	    }
	}
	return(NULL);
    } else {                /* get cardtype from filename      */
	if (strstr(string,"cip"))
	    return("CIP ");
	return("    ");
    }
}

static inline void set_default_image (uint ctrlr_type)
{
    switch (ctrlr_type) {
      case FCI_CIP_CONTROLLER:
	/* default = flash */
	ucode_prefs[ctrlr_type].src = UCODE_FLASH;
	setstring(&ucode_prefs[ctrlr_type].filename, 
		  get_flash_default("cip",TRUE));
	break;

      default:
	/* default is from system */
	setstring(&ucode_prefs[ctrlr_type].filename,"system image");
	ucode_prefs[ctrlr_type].src = UCODE_SYSTEM;	
	break;
    }
}

static inline boolean is_default_image (uint ctrlr_type)
{
    switch (ctrlr_type) {
      case FCI_CIP_CONTROLLER:
	if ( (strcmp(ucode_prefs[ctrlr_type].filename, 
		     get_flash_default("cip",TRUE)) 
	      == 0) && 
	    (ucode_prefs[ctrlr_type].src == UCODE_FLASH)) 
	    return TRUE;
	break;
      default:
	if (ucode_prefs[ctrlr_type].src == UCODE_SYSTEM)
	    return TRUE;
	break;
    }
    return FALSE;
}

/*
 * set_ucode_source()
 * 
 * Called from within the context of the parser.
 * Given a source (flash, system, rom etc), a controller type, a
 * "sense" from the parser, and a filename, validate and update the
 * local databases of where a given controller should get it's ucode
 * from.
 * 
 */
void set_ucode_source (ucode_src_t src, ucode_ctrlr_type_t ucode_ctrlr_type,
		       boolean sense, char* filename) 
{
    int i;
    int   retcode;
    uint  dev_type;

    if (src == UCODE_FLASH) {
	if (!reg_invoke_file_exists(FILE_ACCESS_FLASH, filename) &&
	    !system_loading && sense) {
	    printf("Warning - flash file %s does not "
		   "currently exist!\n", filename);
	}
	if (sense){
            retcode = FS_ioctl(filename, IOCTL_GET_DEV_TYPE, &dev_type);
            if ((!(dev_type & IOCTL_DEV_TYPE_LOCAL)) && (retcode >= 0)){
                printf("%% Error - specified device must be local (not a slave)\n");
                return;
            }
	}
    }
    for (i = 0; i < FCI_NUM_CONTROLLERS; i++) {
	if (ucode_prefs[i].cardtype == ucode_ctrlr_type) {
	    if (ucode_prefs[i].src != src) {
		ucode_prefs[i].src = src;
	    }
	    if (sense) {
		switch (src) {
		case UCODE_FLASH:
		    if (strcmp(ucode_prefs[i].filename, filename)) {
			setstring(&ucode_prefs[i].filename, filename);
		    }
		    break;
		case UCODE_ROM:
		    if (dbus_no_rom_ucode(i)) {
			if (dbus_no_system_ucode(i)) {
			    printf("\nROM microcode not supported by %s.  "
				   "Use flash microcode",
				   ctrlr_names[ucode_ctrlr_type]);
			    ucode_prefs[i].src = UCODE_FLASH;
			} else {
			    printf("\nROM microcode not supported by %s.  "
				   "Use either flash or system microcode.",
				   ctrlr_names[ucode_ctrlr_type]);
			    ucode_prefs[i].src = UCODE_SYSTEM;
			}
		    } else {
			ucode_prefs[i].src = UCODE_ROM;
		    }
		    break;
		default:
		    ucode_prefs[i].src = UCODE_SYSTEM;
		case UCODE_SYSTEM:
		    if (dbus_no_system_ucode(i)) {
			printf("\nSystem microcode not supported by %s.  "
			       "Must use flash microcode.",
			       ctrlr_names[ucode_ctrlr_type]);
			ucode_prefs[i].src = UCODE_FLASH;
		    } else {
			ucode_prefs[i].src = UCODE_SYSTEM;
			setstring(&ucode_prefs[i].filename,
				  "system image");
		    }
		    break;
		}
	    } else {
		set_default_image(i);
	    }
	}
    }
}

/*
 * microcode_command - handle microcode interface command
 */

/*
 * microcode <interfacetype> {ROM|SYSTEM|<flashfilename>} [slot]
 *
 * microcode eip rom loads all EIP controllers with their onboard rom ucode
 *
 * microcode eip eip.ucode 1 loads EIP in slot 1 with contents
 *    of flash file eip.ucode
 *
 * microcode reload forces actual reloading of controllers
 * 
 * Ability to specify/load only certain slots not yet implemented
 */

void microcode_command (parseinfo *csb)
{
    leveltype level;
#ifdef DEBUG_TIMING
    sys_timestamp reload_time;
#endif
    sys_timestamp etime;
    ucode_src_t src = GETOBJ(int,2);
    ucode_ctrlr_type_t ctrlr_type = GETOBJ(int,1);
    char *filename = GETOBJ(string,1);

    if (csb->nvgen) {
	if (csb->which == UCODE_SET_PREFS) {
	    if (src == UCODE_FLASH || src == UCODE_ROM) {
		nv_write(ucode_prefs[ctrlr_type].src == src,
			 "%s %s", csb->nv_command,
			 ucode_prefs[ctrlr_type].filename);
	    }
	}
	return;
    }

    switch (csb->which) {
      case UCODE_SET_PREFS:
	set_ucode_source(src, ctrlr_type, csb->sense, filename);
	break;
    case UCODE_RELOAD:
#ifdef DEBUG_TIMING
	GET_TIMESTAMP(reload_time);
#endif
	hotswap_preallocate_memory(TRUE); /* make sure we have memory */
	level = raise_interrupt_level(LEVEL_CONSOLE);
	GET_TIMESTAMP(etime);
	hotstall_interrupt();	/* call worker routine */
#ifdef DEBUG_PARALLEL
	ttyprintf(CONTTY,"\nhotstall_interrupt took %d\n",ELAPSED_TIME(etime));
#endif
	reset_interrupt_level(level);
#ifdef DEBUG_TIMING
	buginf("\nmicrocode reload took %d ms", ELAPSED_TIME(reload_time));
#endif
	break;
    }
}



/*
 * microcode_ipc_service
 *
 * handle IPC requests involving microcode
 */
static void microcode_ipc_service (ipc_message *req_msg, void *context, 
				   ipc_error_code error)
{
    struct microcode_msg *m_msg;
    ipc_message *rsp_msg;
    uint ctrlr_type;
    uint *data_p;
    char full_filename[FLASH_NAME_LEN];
    char *p;

    m_msg  = req_msg->data;
    if (m_msg->msg_version != MICROCODE_MSG_VERSION) {
	errmsg(&msgsym(IPCINVALID, UCODE), m_msg->request_code, 
	       req_msg->header->source_port);
	ipc_return_message(req_msg);
	return;
    }

    rsp_msg = ipc_get_message(MICROCODE_BUFSIZE, 0, 0);
    if (rsp_msg == NULL) {
	errmsg(&msgsym(IPCBUFFAIL, UCODE));
	ipc_return_message(req_msg);
	return;
    }

    /* 
     * check received message for request type
     */
    switch (m_msg->request_code) {

      case MICROCODE_GET_PREFIX:
	/*
	 * IP requests filename prefix from "micro ip flash" command
	 */
	ctrlr_type = m_msg->data[0];      /* request data is ctrlr_type for */
	                                  /* this request code              */
	p = ucode_prefs[ctrlr_type].filename;
	/*
	 * Prefix filename with device: if necessary
	 */
	reg_invoke_add_flash_prefix(&p,full_filename);
	strncpy(rsp_msg->data, p, FLASH_NAME_LEN);
	ipc_send_rpc_reply(req_msg, rsp_msg);
	break;

      case MICROCODE_GET_IOS_VERSION:
	/*
	 * IP requests IOS version info
	 */

	/* No request argument */
	data_p = rsp_msg->data;
	*data_p = SYS_MAJORVERSION*256 + SYS_MINORVERSION;
	ipc_send_rpc_reply(req_msg, rsp_msg);
	break;

      default:
	errmsg(&msgsym(IPCINVALID, UCODE), m_msg->request_code, 
	       req_msg->header->source_port);
	ipc_return_message(rsp_msg);
	ipc_return_message(req_msg);
	break;
    }
}


/*
 * microcode_ipc_init
 *
 * Prepare for incoming IPC requests involving microcode
 */

static boolean microcode_ipc_init (void) {
    int error;
    ipc_port_id microcode_port;
    
    error = ipc_create_named_port(MICROCODE_PORT_NAME, &microcode_port, 0);
    if (error != IPC_OK){
	errmsg(&msgsym(IPCINITFAIL, UCODE),"port");
	return(FALSE);
    }
    /*
     * setup IPC hook to service microcode requests
     */
    error = ipc_set_receive_callback(microcode_port, NULL, 
				     microcode_ipc_service);
    if (error != IPC_OK) {
	errmsg(&msgsym(IPCINITFAIL, UCODE),"callback");
	return(FALSE);
    }
    return(TRUE);
}


/*
 * cause the microcode loader to run in 'msec' milliseconds
 */
void ucode_loader_wakeup (uint msec)
{
    mgd_timer_start(&ucode_loader_timer, msec);
    return;
}

/*
 * ucode_loader
 * scan system for newly inserted cards and load their microcode
 */

static void ucode_loader (void)
{
    int indx;
    dgtype *dgbus;
    leveltype level;
    int card_type;
    boolean ipc_initialized_ok = FALSE;
    sys_timestamp ttime;

    process_watch_mgd_timer(&ucode_loader_timer, ENABLE);
    while (TRUE) {

	if (!ipc_initialized_ok)
	    ipc_initialized_ok = microcode_ipc_init();

	process_wait_for_event();
#ifdef DEBUG_PARALLEL
	ttyprintf (CONTTY, "\nUcoder loader wakeup\n");
#endif
	if (oir_debug)
	    buginf("\nucode_loader starting");
	level = raise_interrupt_level(STALL_DISABLE);
	if (!mgd_timer_expired(&ucode_loader_timer)) {
	    reset_interrupt_level(level);
	    continue;
	}
	mgd_timer_stop(&ucode_loader_timer);
	reset_interrupt_level(level);

	for (indx = 0; indx < MAX_SLOTS; indx++) {
 	    dgbus = pslot_to_dbus(indx);
	    card_type = slots[indx].ctrlr_type;
 	    if (!slots[indx].ucode_loaded && dbus_present(dgbus)) {
		GET_TIMESTAMP(ttime);
 		dbus_hard_ctrlr_reset(dgbus); /* reset controller */
#ifdef DEBUG_PARALLEL
		ttyprintf(CONTTY,"Ucode loader hard reset slot %d took %d\n",indx,ELAPSED_TIME(ttime));
#endif
 	    }

	    if (!dbus_validate(indx))
		continue;

	    /* if this card has not been loaded, do so */
	    if (!slots[indx].ucode_loaded) {
		if (oir_debug) 
		    buginf("\nslot %d about to load, reset, ucode", indx);
		dgbus = pslot_to_dbus(indx);
		/*
		 * dbus_hard_ctrlr_reset() not needed here because
		 * presumably this card was just inserted.
		 */
		GET_TIMESTAMP(ttime);
		dbus_ctrlr_reset(dgbus); /* reset controller */
#ifdef DEBUG_PARALLEL
		ttyprintf(CONTTY,"Ucode loader soft reset slot %d took %d\n",indx,ELAPSED_TIME(ttime));
		ttyprintf (CONTTY, "Ucode loader loading slot %d\n",indx);
#endif
		dbus_load_ucode(indx);
		if (oir_debug)
		    buginf("\nslot %d did ucode load", indx);
	    }
	}

	/* is there another request pending? */
	if (mgd_timer_running(&ucode_loader_timer)) {
	    if (oir_debug)
		buginf("\nucode_loader restarting");
	    continue;
	}

	hotstall_flag = TRUE;
	if (oir_debug)
	    buginf("\nucode_loader finished");
	process_set_boolean(oir_process_wakeup, TRUE);
    }
}

/*
 * ucode_init
 * initialize microcode configuration data structures
 */
static void ucode_init (subsystype* subsys) 
{
    uint i;

    ucode_parser_init();
    mgd_timer_init_leaf(&ucode_loader_timer,
			NULL,		/* no parent */
			0,		/* no type */
			NULL,		/* no context */
			TRUE);		/* will be tweaked by interrupt */
    for (i = 0; i < FCI_NUM_CONTROLLERS; i++) {
	ucode_prefs[i].cardtype = NUCODE_CTRLR_TYPES;
	set_default_image(i);
    }

    /*
     * only elements with cardtype != NUCODE_CTRLR_TYPES are valid
     */
    ucode_prefs[FCI_ETHER_CONTROLLER].cardtype = UCODE_EIP;
    ucode_prefs[FCI_SERIAL_CONTROLLER].cardtype = UCODE_SIP;
    ucode_prefs[FCI_LINK_CONTROLLER].cardtype = UCODE_HIP;
    ucode_prefs[FCI_CTR_CONTROLLER].cardtype = UCODE_TRIP;
    ucode_prefs[FCI_FDDIT_CONTROLLER].cardtype = UCODE_FIP;
    ucode_prefs[FCI_FSIP_CONTROLLER].cardtype = UCODE_FSIP;
    ucode_prefs[FCI_MIP_CONTROLLER].cardtype = UCODE_MIP;
    ucode_prefs[FCI_SP_CONTROLLER].cardtype = UCODE_SP;
    ucode_prefs[FCI_SSP_CONTROLLER].cardtype = UCODE_SSP;
    ucode_prefs[FCI_AIP_CONTROLLER].cardtype = UCODE_AIP;
    ucode_prefs[FCI_CIP_CONTROLLER].cardtype = UCODE_CIP;
    ucode_prefs[FCI_FEIP_CONTROLLER].cardtype = UCODE_FEIP;
    ucode_prefs[FCI_RVIP_CONTROLLER].cardtype = UCODE_RVIP;
    ucode_prefs[FCI_SVIP_CONTROLLER].cardtype = UCODE_SVIP;
    ucode_prefs[FCI_RSP2_CONTROLLER].cardtype = UCODE_RSP2;
    ucode_prefs[FCI_POSIP_CONTROLLER].cardtype = UCODE_POSIP;

    ucode_buf = malloc_named(max(UCODE_BUFFER_SIZE, max_ucode_uncompressed+1),
			     "Ucode Buffer");
    process_create(ucode_loader, "Microcode Loader",
		   NORMAL_STACK, PRIO_NORMAL);
}


/*
 * nv_ucode
 * write out microcode configuration commands to NVRAM
 */ 

void nv_ucode (parseinfo *csb)
{
    int i;
    boolean reload_needed = FALSE;

    for (i = 0; i < FCI_NUM_CONTROLLERS; i++) {

	/*
	 * Some controllers have a "foreign" parse chain for controlling
	 * images.  For example, the slave RSP uses the "slave image/rel"
	 * parse chain, not the "micro" parse chain.  If this controller type
	 * is one that uses a foreign parse chain, disregard it while it
	 * nvgenning our "micro" commands.
	 */
	if (ctrlr_uses_foreign_parse_chain(i))
	    continue;

	if (ucode_prefs[i].cardtype != NUCODE_CTRLR_TYPES) {
	    if (!is_default_image(i)){
		reload_needed = TRUE;
		nv_write(TRUE, "%s %s %s %s",
			 csb->nv_command, ctrlr_names[ucode_prefs[i].cardtype],
			 microcode_srcs[ucode_prefs[i].src],
			 ucode_prefs[i].src == UCODE_FLASH ?
			 ucode_prefs[i].filename : "");
	    }
	}
    }
    nv_write(reload_needed, "%s reload", csb->nv_command);
}


/*
 * sanity check microcode header
 */
boolean check_microcode_header (uchar** src, ulong* size, char* name,
				int slot, boolean checksum) 
{
    dlu_hdr_t *hdr;
    dlu_trlr_t *trlr = NULL;
    ushort crc, fw_rev, req_hw_ver;

    /*
     * warning - src is arbitrarily aligned, so extract & save anything
     * larger than a byte
     */
    hdr = (dlu_hdr_t*)*src;
    fw_rev = GETSHORT(&hdr->fw_rev);
    req_hw_ver = GETSHORT(&hdr->req_hw_ver);

    /*
     * If we were called from download_microcode_chunk_from_ram()
     * then 'src' is only pointing to the 1st chunk of microcode and
     * the checksum flag is set FALSE.  The checksum is tacked on to
     * the end of the microcode, so we can not verify the checksum in
     * this routine without the entire image.
     */
    if (checksum) 
	trlr = (dlu_trlr_t*)(*src+*size-sizeof(dlu_trlr_t));
    if (hdr->skip == DLU_NO_HDR)
	return TRUE;		/* no header, assume it is okay */
    if (hdr->skip != DLU_HDR_SKIP) {
	errmsg(&msgsym(HDRCORRUPT, UCODE), name, hdr->skip, DLU_HDR_SKIP);
	return FALSE;
    }
    if (hdr->hdr_ver != DLU_HDR_VER) {
	errmsg(&msgsym(HDRCORRUPT, UCODE), name, hdr->hdr_ver, DLU_HDR_VER);
	return FALSE;
    }
    if (hdr->hw_type != slots[slot].ctrlr_type) {
	errmsg(&msgsym(WRONGHARD, UCODE), name,
	       card_type_to_str(hdr->hw_type, req_hw_ver),
	       card_type_to_str(slots[slot].ctrlr_type,
				slots[slot].hw_version));
	return FALSE;
    }

    if (!ucode_version_ck(hdr->hw_type, fw_rev >> 8, fw_rev & 0xff)) {
	errmsg(&msgsym(VERSIONCK, UCODE),
	       fw_rev >> 8, fw_rev & 0xff,
	       card_type_to_str(hdr->hw_type, req_hw_ver));
	return FALSE;
    }

    /*
     * Verify that the hardware version is OK.
     * This is currently only a requirement for loading the
     * appropriate ucode to SP/SPP, but we will require it 
     * for all boards.
     */
    if ((req_hw_ver >> 8) != (slots[slot].hw_version >> 8)) {
	errmsg(&msgsym(BADHWVER, UCODE), name,
	       card_type_to_str(hdr->hw_type, req_hw_ver), req_hw_ver >> 8);
	return FALSE;
    }
    if (checksum) {
	crc = ipcrc((ushort*)*src, *size);
	if (crc) {
	    errmsg(&msgsym(BADCHKSUM, UCODE), name, (ulong)crc,
		   GETSHORT(&trlr->crc));
	    return FALSE;
	}
	*size = *size - sizeof(dlu_hdr_t) - sizeof(dlu_trlr_t);
    } else {
	/*
	 * Don't subtract the trailer since this chuck of microcode 
	 * may not contain the trailer.  Deal with the trailer at a
	 * higher level.
	 */
	*size = *size - sizeof(dlu_hdr_t);
    }
    /*
     * Passed all tests.  Peel off header and return success.
     */
    *src = *src + sizeof(dlu_hdr_t);
    return TRUE;
}

/*
 * get_ucode_src
 */
const char *get_ucode_src (int slot)
{
    if (slot < MAX_SLOTS)
	return microcode_srcs[slots[slot].ucode_source];
    else
	return "<bad slot>";
}

/*
 * get_ucode_src_file
 */
char *get_ucode_src_file (int slot)
{
    if (slot >= MAX_SLOTS)
	return NULL;

    if (slots[slot].ucode_source == UCODE_FLASH)
	return slots[slot].flash_file;
    else
	return NULL;
}

/*
 * find_ucode_in_bundle
 * note - returns arbitrarily aligned byte ptr
 */
static uchar* find_ucode_in_bundle (int cardtype, int hw_ver, ulong* size)
{
    register int i;
    long new_size;
     
    for (i=0; i<bundled_files; i++) {
	if (bundle_toc[i].hw_type != cardtype)
	    continue;
	if (bundle_toc[i].req_hw_ver >> 8 != hw_ver >> 8)
	    continue;
	if (bundle_toc[i].compressed) {
	    if (!ucode_buf) {
		errmsg(&msgsym(NOBUF, UCODE));
		return(NULL);
	    }
	    switch (bundle_toc[i].compress_type) {
	    case UNIX_COMPRESS:
		new_size = ucode_decompress(&bundle[bundle_toc[i].offset],
					    bundle_toc[i].size,
					    ucode_buf,
 					    max_ucode_uncompressed);
 		if (new_size != -1) {
 		    *size = (ulong)new_size;
 		    return ucode_buf;
 		}
 		else
 		    return NULL;
	    default:
 		break;
 	    }
 	}
 	*size = bundle_toc[i].size;
 	return (uchar *)&bundle[bundle_toc[i].offset];
     }
     return NULL;
}

/* 
 * show_ucode_in_bundle
 */ 
void show_ucode_in_bundle (parseinfo *csb)
{
    int i;
    char full_flashname[FLASH_NAME_LEN + 
			    sizeof(UCODE_FLASH_DEFAULT_DEVICE) + 1];

    printf("\nMicrocode bundled in system\n\n");
    printf("Card    Microcode    Target Hardware    Description\n"
	   "Type    Version      Version\n"
	   "----    ---------    ---------------    -----------\n");
 
    for (i=0; i<bundled_files; i++) {
	printf("%4s     %-3d.%3d         %-3d.x          %25s\n",
	       card_type_to_str(bundle_toc[i].hw_type, bundle_toc[i].req_hw_ver),
	       bundle_toc[i].fw_rev >> 8, 
	       bundle_toc[i].fw_rev & 0xff,
	       bundle_toc[i].req_hw_ver >> 8,
	       bundle_toc[i].desc);
    }

    printf("\nMicrocode flash default images\n\n");
    printf("Card    Microcode\n"
	   "Type    Version      device:filename\n"
	   "----    ---------    -------------------\n");

    for (i=0; i<flash_default_files; i++) {
	build_whole_file_id(full_flashname, UCODE_FLASH_DEFAULT_DEVICE, 
			    (char *)flash_toc[i].filename);
	printf("%4s    %9s    %22s",	
	       get_flash_default(flash_toc[i].filename,FALSE),
	       flash_toc[i].version, full_flashname);
	if (!reg_invoke_file_exists(FILE_ACCESS_FLASH, full_flashname))
	    printf(" - Not present\n");
	else
	    printf("\n");
    }
    if (!flash_default_files)
	printf("none\n");
}

/*
 * load_from_system_image
 */
static boolean load_from_system_image (dgtype* dbus, int pslot, boolean wait)
{
    uchar* p;
    ulong size;
     
    p = find_ucode_in_bundle(slots[pslot].ctrlr_type,
			     slots[pslot].hw_version, &size);
    if (!p) 
	return(FALSE);
     
    if (!check_microcode_header(&p, &size, "system image", pslot, TRUE)) 
	return(FALSE);
    if (!download_microcode_from_ram(pslot, p, size, wait)) {
	errmsg(&msgsym(LDFAIL, UCODE), "system image", pslot, "rom");
	return(FALSE);
    }
    return(TRUE);
}

/*
 * supports_chunk_download
 */
static inline boolean supports_chunk_download (int card_type)
{
    switch (card_type) {
    case FCI_CIP_CONTROLLER:
    case FCI_FEIP_CONTROLLER:
    case FCI_RVIP_CONTROLLER:
    case FCI_SVIP_CONTROLLER:
    case FCI_RSP2_CONTROLLER:
    case FCI_POSIP_CONTROLLER:
	return TRUE;
    case FCI_AIP_CONTROLLER:
    case FCI_FSIP_CONTROLLER:
    default:
	return FALSE;
    }
}


/*
 * load all controllers of specific type. This routine insures
 * hard reset, soft reset, download code decompression are done
 * in parallel. It works for all SWDBUS controllers, however, 
 * some mysteries remain for RSP2 eventhough it seems to work.
 * Start by supporting VIP2 controllers
 */
void dbus_load_ucode_all_specific (boolean validated[])
{
    int indx;
    dgtype *dgbus;
    sys_timestamp etime; /* elapsed */
    sys_timestamp stime; /* subtime */
    uchar old_control[MAX_SLOTS]; /* for restoring controller state */
    uchar old_status, status, control;
    boolean recovered[MAX_SLOTS]; /* no_wait loads converted to waits */

    /* 
     * for specific devices perform parallelized version of preamble code
     * in download_microcode_from_ram. This code must always be in sync
     */
    for (indx = 0; indx < MAX_SLOTS; indx++) {
	if (!dbus_parallel_inline(slots[indx].ctrlr_type)) continue;
	if (!validated[indx]) continue;
	dgbus = pslot_to_dbus(indx);
	
	/*
	 * save original state of board (enabled, LED, etc.)
	 */
	old_status = dbus_prot_rd(&dgbus->diag_status);
	old_control[indx] = dbus_prot_rd(&dgbus->diag_control);
#ifdef DEBUG_PARALLEL
	ttyprintf (CONTTY,"slot %d Real Old Control Reg 0x%x\n",indx,old_control[indx]);
#endif
	if (old_status & WCS_ERROR_MASK) {
	    errmsg(&msgsym(WCSERR, DBUS), indx, old_status);
	    dbus_slot_reset(indx);
	    old_status = dbus_prot_rd(&dgbus->diag_status);
	    old_control[indx] = dbus_prot_rd(&dgbus->diag_control);
	}

	/*
	 * halt microcode and instruct WCS controller to accept commands
	 * from DBUS
	 */
	dbus_prot_wr(indx, &dgbus->diag_control, WCS_DBUS_CMD_ENABLE);
    }
    
    /*
     * reset all specific slots and perform Xilinx workaround for each
     */
    dbus_soft_reset_all_specific_controllers(validated);
    
    /* 
     * load microcode to all controllers, perform no waited ops for specific
     * controllers
     */
    for (indx = 0; indx < MAX_SLOTS; indx++) {
	if (!validated[indx]) continue;	
	dgbus = pslot_to_dbus(indx);
	if (dbus_parallel_inline(slots[indx].ctrlr_type)) {
	    GET_TIMESTAMP(etime);
	    recovered[indx] = dbus_load_ucode_nowaited(indx); /* load with no pauses */
#ifdef DEBUG_PARALLEL
	    if (recovered[indx]) ttyprintf(CONTTY,"nowaited load slot %d RECOVERED!\n",indx);
	    ttyprintf (CONTTY, "nowaited download slot %d, took %d ms\n",
		     indx, ELAPSED_TIME(etime));
#endif
	    dbus_reset_recovery_counters(indx);
	}
    }

    /*
     * for specific devices perform parallelized postamble code in
     * download_microcode_from_ram. NB: This code must always be in sync
     */
    GET_TIMESTAMP (etime);
    for (indx = 0; indx < MAX_SLOTS; indx++) {
	if (!dbus_parallel_inline(slots[indx].ctrlr_type)) continue;
	if (!validated[indx]) continue;	
	if (recovered[indx]) continue;
	dgbus = pslot_to_dbus(indx);
	GET_TIMESTAMP(stime);
	dbus_wait_for_sw_ready (dgbus,"DOWNLOAD"); /* wait for decompress */
#ifdef DEBUG_PARALLEL
	ttyprintf (CONTTY, "SWDBUS ready for slot %d took %d\n",indx,ELAPSED_TIME(stime));
#endif
	status = dbus_prot_rd(&dgbus->diag_status);
	if (status & (WCS_ERROR_MASK | DBUS_INTERNAL_ERROR)) {
	    errmsg(&msgsym(WCSLDERR, DBUS), indx, status, 0xdead, 96);
#ifdef DEBUG_PARALLEL
	    ttyprintf(CONTTY, "Reloading non parallel\n");
#endif
	    dbus_load_ucode (indx); /* load regularly */
	    continue;
	}
	control = dbus_prot_rd(&dgbus->diag_control);
#ifdef DEBUG_PARALLEL
	ttyprintf (CONTTY, "Control reg 0x%x\n", control);
	ttyprintf(CONTTY,"Wrote control to slot %d control %x\n",indx,old_control[indx] | WCS_LOADED);
#endif
	dbus_prot_wr(indx, &dgbus->diag_control, old_control[indx] | WCS_LOADED);

	DELAY(1); /* XXX temp, needed for the rsp2 slave prom until the */
	          /* timing is sorted out */
		
	control = dbus_prot_rd(&dgbus->diag_control);
	status = dbus_prot_rd(&dgbus->diag_status);

	if ((control & WCS_LOADING) ||
	    !(control & WCS_LOADED) ||
	    (status & WCS_ERROR_MASK)) {
	    errmsg(&msgsym(WCSLDERR, DBUS), indx, status, 0xdead, 97);	
#ifdef DEBUG_PARALLEL
	    ttyprintf (CONTTY, "Reloading non parallel\n");
#endif
	    dbus_load_ucode (indx); /* load regularly */
	    continue;
	}
    }
#ifdef DEBUG_PARALLEL
    ttyprintf (CONTTY, "decompress completes took %d\n",
	     ELAPSED_TIME(etime));
#endif
}

/*
 * worker routine for dbus_load_ucode or nowaited version.
 *
 * parameter: wait indicates whether waited SWDBUS functions should
 * be performed. When FALSE, indicates that the caller must perform
 * all waited functions.
 *
 * Returns TRUE if recovery performed. This indicates to a caller
 * requesting no waits that parallel operation are not required
 */
static boolean dbus_load_ucode_common (int pslot, boolean wait)
{
    ucode_src_t source;
    char *filename;
    dgtype *dbus = pslot_to_dbus(pslot);
    ulong size;
    sys_timestamp flash_read_time;
    sys_timestamp download_time;
    boolean success;
    uchar* ucp;
    int card_type;
    ucode_downloadtype ucode_args;
    filetype confg;
    char full_filename[FLASH_NAME_LEN];
    boolean recovery_performed = FALSE;

    GET_TIMESTAMP(download_time);
    card_type = slots[pslot].ctrlr_type;
    if (card_type == FCI_SP_CONTROLLER)
	if (slots[pslot].hw_version >> 8 == SSP_HARDWARE)
	    card_type = FCI_SSP_CONTROLLER;
    source = ucode_prefs[card_type].src;
    filename = ucode_prefs[card_type].filename;

    slots[pslot].flags |= BOARD_RELOADED;

#ifdef DEBUG_TIMING
buginf("\ndbus_load_ucode(slot %d, source %s%s)",
	pslot, (source == UCODE_ROM) ? "rom" : ((source == UCODE_FLASH) ?
						"flash " : "system"),
       (source == UCODE_FLASH) ? filename : "");
    GET_TIMESTAMP(download_time);
#endif
    success = FALSE;
    switch (source) {
    case UCODE_ROM:
	success = download_installed_microcode(dbus, TRUE /* always wait for now */);
	break;
    case UCODE_SYSTEM:
	filename = "system image";
	success = load_from_system_image(dbus, pslot, wait);
	break;
    case UCODE_FLASH:
    /*
     * Certain IP's have a collection of "kernel" images.
     * In this case, the prefs.filename is the device/prefix.
     * full_filename consists of filename + separator + kernel_name + hwrev
     */
	if (ip_uses_directory(card_type) && 
	    (strlen(filename) < 
	     FLASH_NAME_LEN - (IP_KERNEL_NAME_SIZE + 1 + 1))){

	sprintf(full_filename,"%s_%s%1d",
		filename, IP_KERNEL_NAME, (slots[pslot].hw_version >> 8));
	filename = full_filename;
    }

/* CLEANTODO - need to allow for someone else using flash */

	size = flash_file_length(filename);
	if (size == -1) {
	    errmsg(&msgsym(NOFILE, UCODE), filename, pslot);
	    break;
	}
	if (supports_chunk_download(card_type)) {
	    memset(&ucode_args, 0, sizeof(ucode_args));
	    ucode_args.slot = pslot;
	    ucode_args.ucode_size = size;
	    ucode_args.card_type = card_type;
	    ucode_args.checksum = 0;
	    ucode_args.filename = filename;
	    ucode_args.wait = wait;
	    size = UCODE_BUFFER_SIZE;
	    GET_TIMESTAMP(flash_read_time);
	    GET_TIMESTAMP(download_time);
	    success = reg_invoke_flash_block_server(filename,
						    ucode_buf, 
						    &size,
						    TRUE,
						    download_microcode_chunk_from_ram, 
						    &ucode_args); 
	    if (!success) {
		errmsg(&msgsym(RDFAIL, UCODE), filename);
		break;
	    }
#ifdef DEBUG_TIMING
	    buginf("\ntime to read flash: %d ms", ELAPSED_TIME(flash_read_time));
#endif
	} else { /* Does NOT support chunk download */
	    if (size > UCODE_BUFFER_SIZE) {
		errmsg(&msgsym(TOOBIG, UCODE), filename, size, UCODE_BUFFER_SIZE);
		break;
	    }
	    GET_TIMESTAMP(flash_read_time);
            confg.filename = filename;
	    if (reg_invoke_file_read(FILE_ACCESS_FLASH, &confg, ucode_buf, 
				     &size, FALSE, NULL, NULL) != ERROR_OK) {
		errmsg(&msgsym(RDFAIL, UCODE), filename);
		break;
	    }
#ifdef DEBUG_TIMING
	buginf("\ntime to read flash: %d ms", ELAPSED_TIME(flash_read_time));
#endif
	    ucp = ucode_buf;
	    if (!check_microcode_header(&ucp, &size, filename, pslot, TRUE)) {
		break;
	    }
	    
	    GET_TIMESTAMP(download_time);
	    success = download_microcode_from_ram(pslot, ucp, size, wait); 
	}
	break;
    default:
	buginf("\ndbus_load_ucode: illegal case value %d", source);
	break;
    }
    if (!success) {

	/*
	 * recovery_performed means all no waited specific functions 
	 * are changed to waited functions on recovery so that there
	 * is nothing left for a waited caller to perform.
	 */
	recovery_performed = TRUE;
	if ( (source != UCODE_SYSTEM) && 
	     (!dbus_no_system_ucode(slots[pslot].ctrlr_type)) ) {

	    errmsg(&msgsym(LDFAIL, UCODE), filename, pslot, "system");
 	    source = UCODE_SYSTEM;

	    /*
	     * parallel load algorithm changes control register state
	     * prior to some traditional checks that may have failed
	     * This reset of the registers allows us to recover.
	     */
	    if (dbus_parallel_inline(slots[pslot].ctrlr_type)) {
		dbus_slot_reset (pslot); /* recover the controller status */
	    }
 	    success = load_from_system_image(dbus, pslot, TRUE /* perform all waited functions to completion */);
#ifdef DEBUG_PARALLEL
	    if (success) ttyprintf(CONTTY,"Retry from system slot %d SUCCEEDED!\n",pslot);
	    if (!success) ttyprintf(CONTTY,"Retry from system FAILED!\n");
#endif
 	}

 	if (!success && !dbus_no_rom_ucode(slots[pslot].ctrlr_type)) {
	    source = UCODE_ROM;
 	    success = rom_ucode_fallback(pslot);
 	}

	if (!success) {
	    source = UCODE_NONE;
	    slots[pslot].flags |= BOARD_DISABLED;
	}
    }
    slots[pslot].ucode_source = source;
    slots[pslot].ucode_loaded = (source != UCODE_NONE);
    if (source == UCODE_FLASH)
	sstrncpy(slots[pslot].flash_file, filename, FLASH_NAME_LEN);
#ifdef DEBUG_TIMING
    buginf("\ndbus_load_ucode: time to download: %d ms from %s to slot %d\n",
	   ELAPSED_TIME(download_time),microcode_srcs[source],pslot);
#endif
    /* tell caller if no_waited functions converted to waited on recovery */
    return recovery_performed;
}

/*
 * dbus_load_ucode
 * load board in physical slot with proper code
 */
void dbus_load_ucode (int pslot)
{
    dbus_load_ucode_common (pslot, TRUE /* perform all waited operations to completion */);
}

/*
 * perform dbus_load_ucode however do not include events that must
 * wait for SWDBUS. Do not perform soft reset on controller and
 * do not wait for software ready after last byte of microcode loaded
 * on SWDBUS controller. Perform loads the same on non SWDBUS, except
 * do not perform initial soft reset.
 *
 * Returns TRUE if recovery performed. This indicates that the caller
 * should not perform parallel operations because the recovery performed
 * a full load.
 */
boolean dbus_load_ucode_nowaited (int pslot)
{
    return dbus_load_ucode_common (pslot, FALSE);
}

/*
 * Downloadable microcode subsystem header
 */

#define UCODE_MAJVERSION 1
#define UCODE_MINVERSION 0
#define UCODE_EDITVERSION 1

SUBSYS_HEADER(ucode, UCODE_MAJVERSION, UCODE_MINVERSION, UCODE_EDITVERSION,
	      ucode_init, SUBSYS_CLASS_KERNEL,
	      "seq: slot",
	      NULL);

