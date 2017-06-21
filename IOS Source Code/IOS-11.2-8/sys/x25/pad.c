/* $Id: pad.c,v 3.1.2.2 1996/06/06 02:53:30 gglick Exp $
 * $Source: /release/112/cvs/Xsys/x25/pad.c,v $
 *------------------------------------------------------------------
 * pad.c -- X25 Protocol Engine PAD Client Interface
 *
 * April 1996, Eman Schaffer
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pad.c,v $
 * Revision 3.1.2.2  1996/06/06  02:53:30  gglick
 *         Sync to V112_0_1
 * Branch: X25_II_branch
 *
 * Revision 3.1.2.1  1996/05/24  16:02:06  eschaffe
 *         XPE/PAD Client Support
 *         P, D, and D1 states.
 * Branch: X25_II_branch
 *
 * Revision 3.1  1996/04/12  20:57:45  eschaffe
 * Add placeholder file
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "subsys.h"
#include "address.h"
#include "media_registry.h"
#include "config.h"
#include "parser.h"
#include "logger.h"
#include "connect.h"
#include "../ui/debug.h"
#include "../x25/parser_defs_x25.h"
#include "../snmp/sr_old_ltsmib.h"
#include "../tcp/tcp.h"
#include "../h/ttysrv.h"
#include "sys_registry.h"
#include "../pt/pt_registry.h"
#include "pad_debug.h"
#include "ll_ul.h"
#include "x25_library.h"
#include "x25_packet.h"
#include "x25_registry.h"
#include "x25_map.h"
#include "pad.h"
#include "../h/access.h"
#include "../pt/translate.h"
#include "../pt/pt_debug.h"
#include "../os/async.h"
#include "../h/stacks.h"
#include "../h/auth_registry.regh"


/*
 * local storage 
 */
vectortype PADvector;
connvector pad_connvector;              /* X.25 connection vector */
x25_packet *pad_x25_pak = NULL;
/* 
 * queue of Incoming Call packets waiting to be processed 
 */
static watched_queue *pad_callinQ;  
/*
 * PAD Callin process id
 */
static pid_t pad_pid;
/* To be deleted when x25lib_pid_form() is implemented */
uchar pad_cud_pid[PAD_MIN_PID_LEN] = {0x1, 0, 0, 0};        

/*
 * pad_map_delete
 * Invoked via x25map_addr_lookup()
 */
static boolean pad_map_delete (x25map_map *map, void *param)
{
    pad_map_ctxt *pad_map_entry = map->user_ctxt;
    /* leveltype level; */
    if (!pad_map_entry)
	return(FALSE);

    /*
     * Delete the PAD map entry if parameter matched
     */
    if (pad_map_entry->cfg_id.hwidb == param) {
	x25map_map_delete(pad_map_root, map);
    }
    return(TRUE);
}

/*
 * pad_map_validate
 *
 */
static boolean pad_map_validate (
    parseinfo *csb,
    x25_subconfig *subcfg,
    char **msg)
{
    x25_address x25_addr;
    x25map_map *map;
    pad_map_ctxt *pad_map_entry = NULL;
    x25map_addresses map_addrs;

    if (GETOBJ(paddr,1)->type == ADDR_X121) {
        memset(&x25_addr, 0, sizeof(x25_addr));
        x25_addr.type = X25_ADDR_X121;
        x25_addr.qualifier = X25_ADDRQ_NONE;
        x25_addr.digits = GETOBJ(paddr,1)->length;
        bcopy(GETOBJ(paddr,1)->addr.x121_address, x25_addr.addr.x121, 
	      x25_addr.digits);
	memset(&map_addrs, 0, sizeof(map_addrs));
        map_addrs.x25 = &x25_addr;
    }
    if (map_addrs.x25 == NULL) {
	*msg = "X.121 address required";
	return(FALSE);
    }
    /*
     *  look for a map that matches this datagram address
     */
    map = x25map_addr_lookup(pad_map_root, &map_addrs, X25MAP_X25, 0,
			     NULL, NULL);
    if (map) {
	pad_map_entry = map->user_ctxt;
	if (csb->sense && (bcmp(pad_map_entry->address,
				x25_addr.addr.x121, x25_addr.digits) == 0)) {
	    if (pad_map_entry->cfg_id.hwidb == csb->interface->hwptr) {
		*msg = "Specified address already mapped on this interface";
		return(FALSE);
	    }
	}
    }
    if (! x25lib_facil_validate(csb, subcfg->config, msg)) {
	return(FALSE);
    }
    return(TRUE);
}

/*
 * pad_map_parse
 * Process an x25 map command
 * Invoked to notify a client of a map configuration specification,
 * either to add or remove a map
 * For a map command:
 *  csb->uiarray[0] is set to the permissible map options
 *  csb->uiarray[1] is set to X25CLIENT_PAD
 *  OBJ(paddr,1) is set to the remote host's X.121 address
 */
static void pad_map_parse (
    parseinfo *csb,
    x25_subconfig *subcfg,
    char **msg,
    boolean *error)
{

    pad_map_ctxt *pad_map_entry;
    x25map_addresses map_addrs;
    x25map_map *x25_public_map;
    x25_address address;
    leveltype level;

    switch (csb->which) {
      case X25SVC_MAP:
	if (!pad_map_validate(csb, subcfg, msg))
	    return;
	address.type = X25_ADDR_X121;
	address.digits = GETOBJ(paddr,1)->length;
	bcopy(GETOBJ(paddr,1)->addr.x121_address, address.addr.x121,
	      address.digits);
	memset(&map_addrs, 0, sizeof(map_addrs));
	map_addrs.x25 = &address;

	if (!csb->sense) {
	    /* Delete the entry */
	    x25map_addr_lookup(pad_map_root, &map_addrs, X25MAP_X25,
			       (X25MAP_ALL_ADDRESS_M & ~X25MAP_X25),
			       pad_map_delete, 
			       subcfg->config->id.hwidb);
	}else {
	    level = raise_interrupt_level(NETS_DISABLE);
	    /*
	     * Create a mapping for the datagram address(es) with 
	     * the X.121 address 
	     */
	    x25_public_map = x25map_map_create(pad_map_root, &map_addrs);
	    if (!x25_public_map) {
		reset_interrupt_level(level);    
		printf("\nPAD: failed to create a map entry");
		return;
	    }
	    pad_map_entry = x25_public_map->user_ctxt;
	    bcopy(address.addr.x121, pad_map_entry->address,
		  address.digits);
	    pad_map_entry->cfg_id.type = X25SERVICE_X25;
	    pad_map_entry->cfg_id.hwidb = csb->interface->hwptr; 
	    pad_map_entry->ctxt_id.type = X25SERVICE_X25;
	    pad_map_entry->ctxt_id.id_x25.hwidb = csb->interface->hwptr;
	    /* we need to pick up the options */
		memset(&pad_map_entry->map_facil, 0, sizeof(x25_client_facil));
	    x25lib_facil_parse(csb, &pad_map_entry->map_facil);
	    reset_interrupt_level(level);
	}
	break;
      case X25SVC_PVC:
	*msg = "x25 pvc pad subcommand is not implemented yet";
	*error = TRUE;
	break;
      default:
        break;
    }
}

/*
 * pad_map_nvgen
 * Invoked via x25map_addr_lookup()
 */
static boolean pad_map_nvgen (x25map_map *map, void *param)
{
    pad_map_ctxt *pad_map_entry = map->user_ctxt;
    if (!pad_map_entry)
	return(FALSE);
    /*
     * nvgen the PAD map entry if parameter matched
     */

    if (pad_map_entry->cfg_id.hwidb == param) {
	nv_write(TRUE, "x25 map pad");
	nv_add(TRUE, " %s", pad_map_entry->address);
	if (pad_map_entry->cud)
	    nv_add(TRUE, " %s", pad_map_entry->cud);
	x25lib_facil_map_nvgen(&pad_map_entry->map_facil);
    }
    return(FALSE);
}

/*
 * pad_client_map_nvgen
 * Invoked to notify the clients to generate any map configuration commands
 */
static void pad_client_map_nvgen (
    parseinfo *csb, 
    x25_subconfig *subcfg)
{
    if (csb->nvgen) {
	/*
	 * Write out approperiate "x25 map pad" command to nv ram
	 */
	x25map_addr_lookup(pad_map_root, NULL, X25MAP_X25, 0,
			   pad_map_nvgen, subcfg->config->id.hwidb);
    }
}

/* pad_map_show
 * Invoked via x25map_addr_lookup() to display the PAD map information
 */
static boolean pad_map_show (x25map_map *map, void *param)
{
    pad_map_ctxt *pad_map_entry = map->user_ctxt;
    x25_config *cfg = param;

    if (!pad_map_entry)
	return(FALSE);
    if (pad_map_entry->cfg_id.hwidb == cfg->id.hwidb) {
	printf("\n%s: X.121 %s%s <--> pad PERMANENT \n", 
	       cfg->first_subcfg.swidb->namestring,
	       pad_map_entry->address);
	x25lib_facil_show(&pad_map_entry->map_facil);
    }
    return(FALSE);
}

/* pad_show_map
 * Invoked to display PAD map info
 */
static void pad_show_map (x25_config *cfg)
{
    x25map_addr_lookup(pad_map_root, NULL, X25MAP_X25, 0, pad_map_show, cfg);
}

/*
 * pad_config_pvc
 * Invoked to notify a client of a pvc configuration specification,
 */
static void pad_config_pvc (
    parseinfo *csb, 
    x25_subconfig *subcfg, 
    char **msg, 
    boolean *error)
{
}

/*
 * pad_config_delete
 * Invokded when an X.25 configuration is deleted
 */
static void pad_config_delete (x25_config *cfg)
{

    /* 
     * For now, we encapsulate X.25 services only 
     */
    if (cfg->id.type != X25SERVICE_X25) {
	return;
    }

    /* If the x25 Config is deleted, we want to search all the PAD map
     * entries under the PAD map root and delete all entries associated
     * with that config
     */
    (void) x25map_addr_lookup(pad_map_root, NULL, X25MAP_X25,
			      (X25MAP_ALL_ADDRESS_M & ~X25MAP_X25), 
			      pad_map_delete, cfg->id.hwidb);
}

/*
 * pad_subconfig_delete
 * Invokded when an X.25 subconfiguration is deleted
 */
static void pad_subconfig_delete (x25_subconfig *subcfg)
{
    /* We don't do anything for now */
}

/*
 * pad_config_change_valid
 * Invoked so PAD can check if a certain configuration is valid
 */
static void pad_config_change_valid (
    parseinfo *csb, 
    x25_config *cfg, 
    boolean *error, 
    char **msg)
{
    /* I am not sure what to do here */
}

/*
 * pad_config_changed
 * Invoked so PAD can be notified of a configuration change
 */
static void pad_config_changed (parseinfo *csb, x25_config *cfg)
{
    /* Not implemented */
}

/*
 * pad_client_config_change_valid
 * Invoked so PAD can validate a certain configuration
 */
static void pad_client_config_change_valid (
    parseinfo *csb, 
    x25_subconfig *subcfg, 
    boolean *error, 
    char **msg)
{
    /* char *padmsg;
    pad_parse_cmd(csb);
    * maybe pass padsmg too *
    *msg = padmsg;
    *error = TRUE;
    */
}

/*
 * pad_client_config_changed
 * Invoked so PAD can be notified of a configuration change
 */
static void pad_client_config_changed (
    parseinfo *csb, 
    x25_subconfig *subcfg)
{
    /* Why are we called with a subconfig??? */
}

/*
 * pad_callin_proc
 * Process X.25 PAD Incoming Call packets waiting on the Call input queue.
 */
static process pad_callin_proc (void)
{
    paktype *pak;
    x25_vc *vc;
    int count;
    int line;
    translatetype *trans;

    if (pad_callinQ == NULL)
	pad_callinQ = create_watched_queue("PAD input", 0, 0);
    process_watch_queue(pad_callinQ, ENABLE, RECURRING);
    while (TRUE) {
	process_wait_for_event();
	count = PAK_SWITCHCOUNT;
	/*
	 * Process only a limited number of packets per pass.
	 */
	while (count-- > 0) {
	    /*
	     * Dequeue an Incoming Call packet
	     */
	    pak = process_dequeue(pad_callinQ);
	    if (!pak)
		break;
	    vc = (x25_vc *)pak->x25_cb;
	    line = pak->unspecified[0];
	    trans = pak->tx_nominate_context;
	    if (!vc || !line) {
		buginf("\n check me");
		datagram_done(pak);
                continue;
	    }
	    datagram_done(pak);
	    if (pad_makecall(vc , line, trans)) {
		pad_setup_vc_vectors(vc->client.context, vc);
		/*
		 * Send a call confirmation back
		 */
		pad_send_x25pak(vc, CALL_CONF, 0, 0, NULL);
		break;
	    }
	}
    }
}

/*
 * pad_init
 * Set up global PAD stuff
 */
static void pad_init (subsystype *subsys)
{
    /*
     * Set up debugging flags
     */
    pad_debug_init();
    padname_init();
    padvty_init();
    pad_parser_init();
    pad_enable = TRUE;
    reg_add_x25_svc_announce(pad_incoming_call, "pad_incoming_call");
    reg_add_x25_config_delete_ind(pad_config_delete, "pad_config_delete");
    reg_add_x25_subconfig_delete_ind(pad_subconfig_delete,
				     "pad_subconfig_delete");
    reg_add_x25_config_changed(pad_config_changed,"pad_config_changed");
    reg_add_x25_config_change_valid(pad_config_change_valid,
				    "pad_config_change_valid");
    reg_add_x25_client_config_changed(pad_client_config_changed,
				      "pad_client_config_changed");
    reg_add_x25_client_config_change_valid(pad_client_config_change_valid,
					   "pad_client_config_change_valid");
    pad_pid = process_create(pad_callin_proc, "PAD InCall", NORMAL_STACK,
			     PRIO_HIGH);
    /* 
     * Allocate an X.25 pak structure to process x25 related packets
     */
    pad_x25_pak = malloc_named(sizeof(x25_packet), "X25PAD PACKET");
    /* 
     * Allocate a Global PAD Map Root for all the PAD related maps
     */
    pad_map_root = x25map_root_create(0, sizeof(pad_map_ctxt));
    reg_add_x25_client_config_map_ind(X25CLIENT_PAD, pad_map_parse, 
				      "pad_map_parse");
    reg_add_x25_client_map_nvgen(pad_client_map_nvgen, "pad_client_map_nvgen");
    reg_add_x25_client_config_pvc_ind(X25CLIENT_PAD, pad_config_pvc, 
				      "pad_config_pvc");
    reg_add_x25_client_show_map(pad_show_map, "pad_show_map");
    /*reg_add_x25_show_support(pcb_status, "pcb_status");*/
}

/*
 * PAD subsystem header
 */

#define PAD_MAJVERSION 1
#define PAD_MINVERSION 0
#define PAD_EDITVERSION  1

SUBSYS_HEADER(pad, PAD_MAJVERSION, PAD_MINVERSION, PAD_EDITVERSION,
              pad_init, SUBSYS_CLASS_PROTOCOL,
              "seq: x25",
              "req: x25, x25_library");

/*
 * padvty_init
 * Set up the vector table for PAD Virtual Terminals
 */
void padvty_init (void)
{
    register vectortype *vector;

    vector = & PADvector;
    /* 
     * initialize vector structure 
     */
    vector_init(vector);
    /*
     * Set up the character input/output functions
     */
    vector->vgetc = pad_getc;
    vector->vclearinput = pad_clearinput;
    vector->vinputpending = pad_inputpending;
    vector->vputc = pad_putc;
    vector->vclearoutput = pad_clearoutput;
    vector->voutputpending = pad_outputpending;
    vector->vputpkt = generic_putpkt;
    vector->voutputblock = pad_outputblock;
    vector->vstartoutput = pad_startoutput;  
    vector->vsetsflow = pad_setsflow;
    vector->vservicehook = helper_servicehook;
    vector->vnoservicehook = helper_noservicehook;
    /* Set up the connection vector */
    conn_vector_init(&pad_connvector, "pad", "X.3 PAD");
    pad_connvector.snmp_sesstype = D_tslineSesType_pad;
    /* Set alive routine pointer */
    pad_connvector.valive = pad_alive;
    /* Set run routine pointer */
    pad_connvector.vrun = pad_telnet;
    /* Set close routine pointer */
    pad_connvector.vclose = pad_close;
    /* Set unread count pointer */
    pad_connvector.vunread = pad_unread;
    /* Parse connect command */
    pad_connvector.vopen = pad_multiproto_open;
    /* Parse resume command */
    pad_connvector.vresume = pad_parse_resume; 
    pad_connvector.vaccounting = pad_conn_acct;
}

/*
 * pad_check_subaddr
 * Givin a subaddress, return a line number on which to place the call.
 * If the line is invalid or if accesscheck on that line failed,
 * clear the call and return FALSE.
 */
static int pad_check_subaddr (
     x25_address *dest_addr,
     x25_vc *vc,
     x25_packet *x25_pak)
{
    uchar ifc_addr_len = vc->x25.config->first_subcfg.address.digits;
    int line = -1;
    boolean parsed;

    /*
     * If incoming address is larger than our input interface
     * address then we have a subaddress we should parse.
     * Make sure that address matches before trying to parse.
     */

    if ((dest_addr->digits > ifc_addr_len) &&
         ((dest_addr->digits - ifc_addr_len) <= 2)) {
        line = parse_unsigned(&dest_addr->addr.x121[ifc_addr_len], &parsed);
    }
    /*
     * If a sub-address was specified, use it as a rotary group
     * number.  Otherwise, get a VTY, if possible.
     */
    if ((line > 0) && (line < PORTINTERVAL))
        line = select_rotary(line, &pad_connvector, NULL, ROTARY_ASCENDING);
    else
        line = get_vtyline(TELNET_PORT, &pad_connvector);
    if (pad_debug)
        buginf("\nPAD: incoming call to %*s on line %d CUD length %d", 
	       dest_addr->digits, dest_addr->addr.x121,
               line == -1 ? 0 : line, x25_pak->user_data.data_len);
    if (line <= 0) {
        pad_send_x25pak(vc, CLEAR, X25_CLEAR_ACCESS_BARRED,
			X25_DIAG_INCOMING_CALL_BARRED, 
			"Can not allocate a line");
        return(FALSE);
    }
    if (!regexp_accesscheck(dest_addr->addr.x121, MODEMS[line]->accessin,
                            x29_accesslists)) {
        pad_send_x25pak(vc, CLEAR, X25_CLEAR_ACCESS_BARRED,
			X25_DIAG_INCOMING_CALL_BARRED, "Access denied");
        return(FALSE);
    }
    /* NB: For a PAD call to the PT prompt, we don't set reverse charging.
     * XPE will clear the call if reverse charging was set.
     */
    return(line);
}

/*
 * pad_q_call
 * Queue and X.25 packet to be processed later on at process level and
 * then claim the VC.
 */
static void pad_q_call (
    x25_vc *vc,
    paktype *pak,
    int line,
    translatetype *trans)
{
    /* 
     * Save the necessary information needed at task level 
     */
    pak->x25_cb = vc;
    pak->unspecified[0] = line;
    pak->tx_nominate_context = trans;
    /* queue the packet */
    process_enqueue_pak(pad_callinQ, pak);
    /* 
     * Clear the packet off of the input interface
     */
    clear_if_input(pak);
    /*
     * Claim the VC
     */
    reg_invoke_x25_vc_lock(&vc->x25.vc_id, FALSE, &vc);
}

/*
 * pad_last_resort
 */
boolean pad_last_resort (x25_vc *vc, x25_packet *x25_pak)
{
    int line;

    line = pad_check_subaddr(&x25_pak->a_block.dst, vc, x25_pak);
    if (line <= 0) {
	pad_send_x25pak(vc, CLEAR, X25_CLEAR_ACCESS_BARRED, 
			X25_DIAG_INCOMING_CALL_BARRED, 
			"Can not allocate a line");
	return(FALSE);
    }
    /* 
     * Queue the packet and claim VC
     */
    pad_q_call(vc, x25_pak->pak, line, NULL);
    return(TRUE);
}

/***********************************************************************
 ****  Process Incoming PAD Connections.
 ***********************************************************************/

/*
 * pad_incoming_call
 * Decide whether we want to accept an incoming call.  The call user data 
 * indicates that it is a pad call.  
 * When an X.25 service announces an incoming Call, The PAD client will 
 * determine first if it should claim the Call and if it does, it
 * it will queue it to a process level.
 */
boolean pad_incoming_call (
    x25_vc *vc,               /* pointer to VC strutctur */
    x25_packet *x25_pak,      /* pointer to x25 packet info */
    service_call_accept_t *last_resort) /* Client vectore of last resort */
{
    x25_address *dest_addr = &x25_pak->a_block.dst;
    char *cud = x25_pak->user_data.data;
    ushort cud_len = x25_pak->user_data.data_len;
    translatetype *trans = NULL;
    enum X25_ADDR_MATCH address_match;
    int line = -1;

    /* 
     * Is it a PAD incoming call?
     */
    if (cud[PAD_PID_OFFSET] != PAD_PROTOCOL_ID) {
	if (pad_debug)
	    buginf("\nincoming call - invalid PID");
	return(FALSE);
    }
    /* 
     * Get a matching address
     */
    address_match = x25lib_address_match(dest_addr, cud, cud_len, 
					 vc->x25.config);
    /*
     * If the dest address == interface address or destination = an alias
     * of an interface
     */
    if ((address_match == X25ADRM_IFC) || 
	(address_match == X25ADRM_IFC_ALIAS)) {
	line = pad_check_subaddr(&x25_pak->a_block.dst, vc, x25_pak);
	if (line <= 0) {
	    if (pad_debug)
		buginf("\nincoming call - invalid subaddress");
	    return(FALSE);
	}
    }else if (vc->x25.config->client.default_protocol == LINK_PAD) {
	/* 
	 * Don't claim vc now, but set PAD as the client of last resort
	 */
	*last_resort = &pad_last_resort;
	if (pad_debug)
	    buginf("\nincoming call - PAD as client of last resort");
	return(FALSE);
    } else { 
	/* 
	 * Does it match a translation entry? 
	 */
	trans = pt_trans_lookup(dest_addr, cud);
	if (!trans) {
	    if (pad_debug)
		buginf("\nincoming call - no pad or trans match");
	    return(FALSE);
	}
	/* Check on the access first */
	if (!pad_accesscheck(trans, x25_pak->a_block.src.addr.x121,
			     x25_pak->a_block.src.digits, x29_accesslists)) {
	    pad_send_x25pak(vc, CLEAR, X25_CLEAR_ACCESS_BARRED,
			    X25_DIAG_INCOMING_CALL_BARRED, "Access denied");
	    if (pad_debug)
		buginf("\nincoming trans - failed access check");
	    return(FALSE);
	}
	/* get a vty */
	line = get_vtyline(TELNET_PORT, &pad_connvector);
	if (line <= 0) {
	    pad_send_x25pak(vc, CLEAR, X25_CLEAR_ACCESS_BARRED,
			    X25_DIAG_INCOMING_CALL_BARRED, 
			    "Can not allocate a line");
	    if (pad_debug || translate_debug)
		buginf("\nincoming trans - could not get a line");
	    return(FALSE);
	}
	/*
	 * Set a flag to indicates if reverse charging is acceptable.
	 * Later on when the PAD claims the VC, if the Call indicated 
	 * reverse charging and the client hasn't set the reverse
	 * charges are acceptable flag, X.25 will clear the Call.
	 */
	if (trans->in.x25.facil->reverse)
	    vc->x25.config->client.facil.reverse = TRUE;
	if (pad_debug || translate_debug)
	    buginf("\nPAD: translate call to %*s CUD %x %x %x %x \"%*s\"",
		   dest_addr->digits, dest_addr, cud[0], cud[1], cud[2], 
		   cud[3], cud_len - 4, &cud[4]);
    }
    /* 
     * queue the packet to be processed at a task level 
     */
    pad_q_call(vc, x25_pak->pak, line, trans);
    /* 
     * tell XPE NOT to free the packet 
     */
    x25_pak->pak = NULL;
    return(TRUE);
}

/*
 * pad_completecall
 * called when we get back a call accepted from someone we have sent a call
 * to.  Update the pcb, so that openblock will return.
 */
static void pad_completecall (
    void *vc_context,
    enum X25_P_STATE p_was,
    x25_reason *reason,
    x25_packet *pak)
{

    pcbtype *pcb = vc_context;
    x25_vc *vc = pcb->pcb_vc;
    
    if (pcb) {
	pcb->state = PAD_ESTABLISHED;
	if (pad_debug) {
	    buginf("\nPAD%t: Call completed", pcb->ttynum);
	}
    } else {
	if (pad_debug) {
	    buginf("\nPAD: Unable to complete call.  Reason: &d",
		   reason);
	}
    }
    pcb->pad_txQ_id = DEFAULT_QUEUE(vc->x25.config->id.hwidb);
}

/*
 * pad_enqueue
 * P4 state (Data Transfer)
 * Enqueue an arriving X25 packet for the appropriate PAD control block.
 */
static void pad_enqueue (
    void *vc_context,
    x25_packet *x25_pak)
{

    pcbtype *pcb = vc_context;
    x25_vc *vc = pcb->pcb_vc;
    paktype *pak = x25_pak->pak;

    /* 
     * Keep XPE from freeing the packet 
     */
    x25_pak->pak = NULL;
    if (!pcb || !vc) {
	datagram_done(pak);
	return;
    }
    /*
     *** Do we still need to do this?
     * remember time of last input
     */
    GET_TIMESTAMP(vc->x25.data_rx_time);
    /*** save it in packet too */
    pak->inputtime = vc->x25.data_rx_time;
    
    pak->length = pak->datagramsize;
    pak->dataptr = pak->datagramstart;

    pcb->qinput++;
    pcb->pakincount++;
    pcb->unreadbytes += pak->length;
    if (x25_pak->user_data.q_bit) {
	pcb->pakincontrol++;
	pak->flags |= PAK_CONTROL;	/* control packet */
	if (pad_debug)
	    buginf("\nPAD%t: Control packet received.",pcb->ttynum);
    }

    if (QUEUEFULL(&pcb->inqueue)) {
	if (pad_debug)
	    buginf("\nPAD%t: inputQ is full, discard packet", pcb->ttynum);
	datagram_done(pak);
	return;
    }
#ifdef DEBUG1
    if (pad_debug)
	buginf("\nPAD%t: packet %lx received, length %d, datastart %lx",
	       pcb->ttynum, pak, pak->length, pak->dataptr);
#endif
    p_enqueue(&pcb->inqueue, pak);
    clear_if_input(pak);
}

/*
 * pad_clearcall
 * when XPE receives a clear request on a PAD VC, call this routine to
 * pass the failure data back to the user.
 */
static void pad_clearcall (
    void *vc_context, 
    enum X25_P_STATE p_was,
    x25_reason *reason, 
    x25_packet *x25_pak)
{
    pcbtype *pcb = vc_context;
    if (pcb) {
	pcb->state = PAD_CLEARED;
/*	pcb->pcb_vc = NULL; */
    }
    if (pad_debug)
	buginf("\nPAD%t: call cleared. cause: %d diag: %d", 
	       pcb ? pcb->ttynum : 0, x25_pak->cause_diag.cause, 
	       x25_pak->cause_diag.diag);
    
}

/*
 * pad_restart
 * This vector will be invoked when the X.25 service removes
 * the VC from service; this will happen for SVCs when a Clear Confirm
 * is sent or received, a Clear collision occurs, a DCE has a Call
 * collision or the X.25 service exits state R1, and for a PVC when it
 * is deleted by management action or new administrative VC ranges
 * take effect rendering the PVC's channel number invalid
 */
static void pad_restart (
    void *vc_context, 
    enum X25_P_STATE p_was,
    x25_reason *reason, 
    x25_packet *x25_pak)
{
    pcbtype *pcb = vc_context;
    if (pcb) {
	pcb->state = PAD_RESTART;
	pcb->pcb_vc = NULL;
    }
	
    if (pad_debug)
	buginf("\nPAD%t: X25 restarted. Reason: %d", pcb ? pcb->ttynum : 0,
	       x25_pak->cause_diag.cause, x25_pak->cause_diag.diag);
}

/* 
 * pad_pvc_inactive
 * Called when XPE issues a reset request.
 * Resume PAD connection after the reset confirmation.
 */
static void pad_pvc_inactive (
    void *vc_context, 
    enum X25_P_STATE p_was,
    x25_reason *reason, 
    x25_packet *x25_pak)
{
    pcbtype *pcb = vc_context;

    if (pcb) {
	pcb->state = PAD_RESTART;
	pcb->pcb_vc = NULL;
    }
	
    if (pad_debug)
	buginf("\nPAD%t: PVC restarted. Reason: %d", pcb ? pcb->ttynum : 0,
	       x25_pak->cause_diag.cause, x25_pak->cause_diag.diag);
}

/* 
 * pad_reset
 * Called when XPE issues a reset request.
 * Resume PAD connection after the reset confirmation.
 */
static void pad_reset (
    void *vc_context, 
    enum X25_P_STATE p_was,
    x25_reason *reason, 
    x25_packet *x25_pak)
{
    pcbtype *pcb = vc_context;

    if (pcb) {
	if (pcb->state != PAD_RESTART) {
	    pcb->flags &= ~PCB_DISCARD;
	}
    }
    if (pad_debug)
	buginf("\nPAD%t: X25 VC reset. Reason: %d", pcb ? pcb->ttynum : 0,
	       x25_pak->cause_diag.cause, x25_pak->cause_diag.diag);
}

static void pad_idle (void *client_context, x25_packet *packet)
{
    if (pad_debug) {
        buginf("\nxpad_idle: context %x, packet %x", client_context, packet);
    }
}

static void pad_reset_tx (
    void *client_context, 
    enum X25_D_STATE state,
    x25_reason *reason, 
    x25_packet *packet)
{
    if (pad_debug) {
        buginf("\nxpad_reset_tx: context %x, packet %x", 
	       client_context, packet);
    }    
}

static void pad_clear_tx (
    void *client_context, 
    enum X25_P_STATE state,
    x25_reason *reason, 
    x25_packet *packet)
{
    if (pad_debug) {
        buginf("\nxpad_clear_tx: context %x, packet %x", client_context, 
	       packet);
    }    
}

/* 
 * pad_send_x25pak
 */
void pad_send_x25pak (
    x25_vc *vc,        
    uchar type,
    uchar cause,
    uchar diag,
    uchar *clear_reason)
{
    paktype *pak;
    leveltype level;
    enum X25LIB_REQ_RESULT result;
    pcbtype *pcb = vc->client.context;
    if (!pcb) {
	return;
    }
    /*
     * We need to build a Data packet for sending--
     * Lock out interrupts while we use the global pad_tx_packet
     */
    level = raise_interrupt_level(NETS_DISABLE);
    /*
     * X25PAK_SMALL_MAX includes x25_pak overhead and max cause and diag 
     * if necessary.
     */
    pak = getbuffer(sizeof(X25PAK_SMALL_MAX));   
    if (!pak) {
	(*vc->x25.p_vect->client_detach)(pcb, NULL, &result);
	if (pad_debug && result != X25LIB_REQ_OK) {
	    buginf("\nPAD: detach request failed (%d)", result);
	}
	pcb->state = PAD_CLEARED;
	return;
    }
    pad_x25_pak->pak = pak;
    pad_x25_pak->user_data.data = NULL;
    pad_x25_pak->user_data.data_len = 0;

    pad_x25_pak->user_data.q_bit = FALSE;
    pad_x25_pak->user_data.m_bit = FALSE;   /* assumes no fragmentation */
    pad_x25_pak->a_block.specified = FALSE;
    pad_x25_pak->f_block.specified = FALSE;
    pak = NULL;
    if (cause)
	pad_x25_pak->cause_diag.cause = cause;
    if (diag)
	pad_x25_pak->cause_diag.diag = diag;
    pad_x25_pak->d_bit = FALSE;
    pad_x25_pak->a_block.a_bit = FALSE;
    pad_x25_pak->flow.pr = x25lib_vc_tx_ack_everything_inline(vc);
    pad_x25_pak->d_bit = FALSE;

    switch (type) {
      case CALL_CONF:
	pad_x25_pak->a_block.specified = TRUE;
	pad_x25_pak->a_block.src = vc->x25.p.source;
	pad_x25_pak->a_block.dst = vc->x25.p.dest;
	pad_x25_pak->f_block.specified = TRUE;
/*** figure out the facilities stuff. 
     x25lib_facility_fillin(pad_x25_pak, &vc->x25->config.client.facil,??)     */
	pad_x25_pak->type = X25PID_CALL_CONF;
	/* XPE will encode x25_pak and free pak for us */
	(*vc->x25.p_vect->call_conf)(vc->x25.context, pad_x25_pak, &result);
	break;
      case CLEAR:
	pad_x25_pak->type = X25PID_CLEAR;
	(*vc->x25.p_vect->clear)(vc->x25.context, pad_x25_pak, &result);
	if (pad_debug || translate_debug) {
	    buginf("\nPAD: %s", clear_reason);
	}
	break;
      case RESET:
	pad_x25_pak->type = X25PID_RESET;
	(*vc->x25.d_vect->reset)(vc->x25.context, pad_x25_pak, &result);
	break;
      case RR:
	pad_x25_pak->type = X25PID_RR;
	(*vc->x25.d1_vect->rr_tx)(vc->x25.context, pad_x25_pak, &result);
	break;
      case RNR:
	pad_x25_pak->type = X25PID_RNR;
	pad_x25_pak->flow.pr = x25lib_vc_tx_ack_nothing_inline(vc);
	(*vc->x25.d1_vect->rnr_tx)(vc->x25.context, pad_x25_pak, &result);
	break;
      case INTERRUPT:
	pad_x25_pak->type = X25PID_INTERRUPT;
	(*vc->x25.d1_vect->interrupt_tx)(pcb, pad_x25_pak, &result);
      case INTERRUPT_CONF:
	pad_x25_pak->type = X25PID_INTERRUPT_CONF;
	(*vc->x25.d1_vect->interrupt_conf_tx)(vc->x25.context, pad_x25_pak, 
					      &result);
      default:
	buginf("\nPAD: Packet type identifier is not valid");
	break;
    }
    reset_interrupt_level(level);
    if (result != X25LIB_REQ_OK) {
	if (pad_debug || translate_debug)
	    buginf("\nPAD: library request failed (%d) packet type %d", 
		   result, type);
    }
}

/*
 * pad_rx_rr
 */
static void pad_rx_rr (
   void *vc_context,
   x25_packet *pak)
{
    /*buginf("\nPAD: rx_rr");*/
}

/*
 * pad_rx_rnr
 */
static void pad_rx_rnr (
    void *vc_context,
    x25_packet *x25_pak)
{
    buginf("\nPAD: rx_rnr");
}

static void pad_interrupt_rx (
    void *vc_context,
    x25_packet *x25_pak)
{
    pcbtype *pcb = vc_context;
    x25_vc *vc = pcb->pcb_vc;

    if (!pcb || !vc) {
	buginf("\nPAD: pad_interrupt_rx no vc or pcb");
	return;
    }
    /* Send Interrupt_confirm back */
    pad_send_x25pak(vc, INTERRUPT_CONF, 0, 0, NULL);
}
/*
 * vectors for handling events on established virtual circuits
 */
client_p_vectors pad_p_vectors = {
    NULL,               /* Call Collision */
    NULL,               /* VC has entered P4 state (call completed) */
    pad_clearcall,      /* Clear received */
    pad_clear_tx,       /* Clear sent */
    pad_pvc_inactive,   /* PCV inactivate */
    pad_restart,        /* VC out of service */
    pcb_status          /* "show x25 vc" hook */
};
client_d_vectors pad_d_vectors = {
    pad_completecall,   /* VC ready to exchange data */
    pad_reset,          /* Reset received */
    pad_reset_tx,       /* Reset sent */
    NULL                /* D state undefined */
};
client_d1_vectors pad_d1_vectors = {
    pad_enqueue,        /* Incoming data */
    pad_interrupt_rx,   /* Interrupt received */
    NULL,               /* Interrupt confirmation received */
    pad_rx_rr,          /* RR received */
    pad_rx_rnr,         /* RNR received */
    pad_idle            /* Idle timer expired */
};

/* pad_setup_vc_vectors
 * Setup XPE/PAD client VC vectors
 */
void pad_setup_vc_vectors (
    void *client_context,
    x25_vc *vc)               /* pointer to VC strutctur */
{

    pcbtype *pcb = client_context;

    if (!pcb) {
	buginf ("\nPAD - No client context");
	return;
    }
    /* 
     * Set the VC's client info 
     */
    vc->client.p_vect = &pad_p_vectors;
    vc->client.d_vect = &pad_d_vectors;
    vc->client.d1_vect = &pad_d1_vectors;
}

/*
 * padtranslate_cfork 
 * Create an approriate type of process for performing protocol translation
 * on an incoming X.25 connection.  Create special processes if they exist,
 * otherwise create a normal exec with the appropriate "autocommand".
 */

static int padtranslate_cfork (tt_soc *tty, translatetype *trans)
{
    trans->totaluses++;
    if (trans->maxallowed && (trans->active >= trans->maxallowed)) {
	trans->failures++;
	return(-1);
    }
    return(reg_invoke_pt_x25input(OUTPROT(trans->type), trans, tty));
}

/*
 * pad_makecall
 * Worker routine that makes call
 * Handles PVCs and SVCs
 */
int pad_makecall (
    x25_vc *vc,                /* pointer to VC strutctur */
    int line,
    translatetype *trans)
{
    tt_soc *tty;
    conntype *conn;
    pcbtype *pcb;
    ulong stacksize = 0;

    tty = (tt_soc *) tty_allocate(line,TRUE);
    if (tty == NULL) {
	pad_send_x25pak(vc, CLEAR, X25_CLEAR_ACCESS_BARRED, 
			X25_DIAG_INCOMING_CALL_BARRED, "No free VTYs");
	return(PAD_FAILED);
    }
    conn = getconnection(tty, &pad_connvector);
    if (conn == NULL) {
	pad_send_x25pak(vc, CLEAR, X25_CLEAR_ACCESS_BARRED, 
			X25_DIAG_INCOMING_CALL_BARRED, 
			"Can not get Connection");
	return(PAD_FAILED);
    }
    pcb = getpcb(tty);
    if (pcb == NULL) {
	tty_deallocate(tty);
	freeconnection(conn);
	pad_send_x25pak(vc, CLEAR, X25_CLEAR_ACCESS_BARRED, 
			X25_DIAG_INCOMING_CALL_BARRED, "Can not get PCB");
	return(PAD_FAILED);
    }

    conn->tcb = pcb;			/* point to PCB */
    /*
     * copy source address to connection
     */
    bcopy(vc->x25.p.source.addr.x121, conn->hostname, vc->x25.p.source.digits);
    pcb->state = PAD_ESTABLISHED;
    pcb->pcb_vc = vc;
    pcb->pcb_conn = conn;
    pcb->last_acked = 0;
    /* 
     * Assoicate the vc with a pcb 
     */
    vc->client.context = pcb;
    /* 
     * remove from general q 
     */
    unqueue(&tty->connhead, conn);
    /* 
     * remember the driver conn 
     */
    tty->conntop = conn;
    tty->statbits &= ~(CARDROP+HANGUP+IDLE);
    tty->statbits |= READY;
    tty->conncount++;				/* another use */
    tty->ivector = & PADvector;			/* this is a PAD vty */
    switch (tty->type) {
      case AUX_LINE:
      case TTY_LINE:
	if (pad_debug)
	    buginf("\nPAD: Creating TTY daemon on tty%t for vc %d",
		   tty->ttynum, vc->x25.p.vc_number);
	if (reg_invoke_kerberos_is_running())
	    stacksize = (ulong) EXEC_STACK_SIZE;
	tty->tiptop = cfork((forkproc *)tty_daemon, (long)tty,
			    stacksize, "TTY Daemon", tty->ttynum);
	break;
      case VTY_LINE:
	tty->ovector = & PADvector;
	if (trans) {
	    if (pad_debug)
		buginf("\nPAD: Creating proto translation on tty%t for vc %d",
		       tty->ttynum, vc->x25.p.vc_number);
	    tty->tiptop = padtranslate_cfork(tty, trans);
	} else {
	    if (pad_debug)
		buginf("\nPAD: Creating exec on tty%t for vc %d",
		       tty->ttynum, vc->x25.p.vc_number);
	    tty->tiptop = cfork((forkproc *) exec, (long)tty, EXEC_STACK_SIZE, 
				"Virtual Exec", tty->ttynum);
	}
	break;
      default:
	tty->tiptop = -1;
	break;
    }
    if (tty->tiptop < 0) {
	freeconnection(conn);
	death(tty);
	pad_send_x25pak(vc, CLEAR, X25_CLEAR_ACCESS_BARRED, 
			X25_DIAG_INCOMING_CALL_BARRED, 
			"Can not fork a process");
	return(PAD_FAILED);
    }
    /* 
     * If translation and the "printer" option is set, let the PT 
     * confirm the call.
     */
    return((trans && (trans->in.x25.x25flags & TRANSF_X25_PRINTER)) ?
           PAD_IN_PROGRESS : PAD_OK);
}
