/* $Id: atmtest_lane_cmds.c,v 3.2.56.2 1996/05/09 14:11:51 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/atm-test/atmtest_lane_cmds.c,v $
 *------------------------------------------------------------------
 * atmtest_lane_cmds.c -- Command functions for LAN Emulation related 
 * TCL commands. 
 * [Was ../tcl/tcl_lane_cmds.c]
 *
 * April, 1995  Chris Parrott
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmtest_lane_cmds.c,v $
 * Revision 3.2.56.2  1996/05/09  14:11:51  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.56.1.18.1  1996/04/27  06:34:13  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.56.1.8.2  1996/04/08  14:49:45  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.2.56.1.8.1  1996/04/08  01:44:41  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.56.1  1996/03/18  19:00:41  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.60.1  1996/03/22  09:35:08  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.48.1  1996/03/05  05:49:00  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.36.1  1996/02/27  20:40:40  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.14.1  1996/01/11  01:44:41  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.10.1  1995/12/29  01:05:31  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.2  1995/11/17  08:47:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:03:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "ttysrv.h"
#include "config.h"
#include "file.h"
#include "parser.h"
#include "name.h"
#include "packet.h"
#include "../ip/ip.h"
#include "../tcl/tcl.h"
#include "../tcl/tcl_init.h"

#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/atm.h"
#include "../atm/atmsig_api.h"
#include "../lane/lane.h"
#include "../lane/lane_private.h"

#include "../atm-test/atmtest_lane_cmds.h"
#include "../atm-test/atmtest_lane_vars.h"
#include "../atm-test/atmtest_sig_vars.h"
#include "../atm-test/atmtest_vcd_arrays.h"
#include "../atm-test/atmtest_lane_arrays.h"

/* 
 * 
 * General Notes: Running the LANE Send Control Packet Commands
 *
 * To enable TCL LANE debugging, set the TCL variable assigned to
 * LANE_DEBUG_VAR (see atmtest_lane_cmds.h) to a boolean TRUE value
 * (i.e. 1, "true", or "yes").  Setting the same variable to FALSE
 * turns debugging off.
 *
 */

/* 
 *
 * Tcl_HelpCmd()
 *
 * Syntax: tclhelp [item] <CR>
 *
 * Command routine for the 'tclhelp' TCL command.
 * This command is used to provide information to users of the 
 * TCL tools.
 *
 * This command will eventually be more generic - for now, it's
 * main purpose is to provide basic information to users of the 
 * 'lsend' command.
 *
 */
int Tcl_HelpCmd (ClientData clientData, Tcl_Interp *interp, int
			argc, char **argv)
{

    if ( argc > 1 ) {
	atmtest_print_help_string( argv[1] );
    }
    else {
	atmtest_print_gen_help();
    }

    return( TCL_OK );

} /* Tcl_HelpCmd() */

/* 
 *
 * Tcl_LaneSendCmd()
 *
 * Syntax: router(tcl)# lsend [msgtype] [vcd] <CR>
 *
 * Command routine for the 'lsend' TCL command.
 * This command is used to send LANE control packets of type
 * <msgtype>.  If the msgtype entered as an argument corresponds
 * to a known LANE ctl pkt opcode, the the global TCL variable
 * 'opcode' will be appropriately set, the message will be packed
 * with the LANE variables, then shipped out.
 *
 * Optionally the vcd to use may be specified.
 * 
 *
 */
int Tcl_LaneSendCmd (ClientData clientData, Tcl_Interp *interp, int
			argc, char **argv)
{
    boolean	match_found = FALSE;
    int		opcode = NULL;

    if ( argc > 1 ) {

	/*
	 * If a vcd number has been entered, set it; otherwise, the
	 * current value is used (or default if unset).
	 */
	if ( argc > 2 ) {
	    Tcl_SetVar( interp, LE_VAR_VCD, argv[2], 0 );
	}

	if ( Tcl_StringMatch( argv[1], "config_req" ) ) {
	    Tcl_SetVar( interp, LE_CP_VAR_OPCODE, LANE_CONFIG_REQ_STR, 0 );
	    match_found = TRUE;
	    opcode = LANE_CONFIG_REQ;
	}

	if ( Tcl_StringMatch( argv[1], "config_resp" ) ) {
	    Tcl_SetVar( interp, LE_CP_VAR_OPCODE, LANE_CONFIG_RESP_STR, 0 );
	    match_found = TRUE;
	    opcode = LANE_CONFIG_RSP;
	}

	if ( Tcl_StringMatch( argv[1], "join_req" ) ) {
	    Tcl_SetVar( interp, LE_CP_VAR_OPCODE, LANE_JOIN_REQ_STR, 0 );
	    match_found = TRUE;
	    opcode = LANE_JOIN_REQ;
	}

	if ( Tcl_StringMatch( argv[1], "join_resp" ) ) {
	    Tcl_SetVar( interp, LE_CP_VAR_OPCODE, LANE_JOIN_RESP_STR, 0 );
	    match_found = TRUE;
	    opcode = LANE_JOIN_RSP;
	}

	if ( Tcl_StringMatch( argv[1], "le_arp_req" ) ) {
	    Tcl_SetVar( interp, LE_CP_VAR_OPCODE, LANE_ARP_REQ_STR, 0 );
	    match_found = TRUE;
	    opcode = LANE_ARP_REQ;
	}

	if ( Tcl_StringMatch( argv[1], "le_arp_resp" ) ) {
	    Tcl_SetVar( interp, LE_CP_VAR_OPCODE, LANE_ARP_RESP_STR, 0 );
	    match_found = TRUE;
	    opcode = LANE_ARP_RSP;
	}

	if ( Tcl_StringMatch( argv[1], "ip_arp" ) ) {
	    match_found = TRUE;
	    opcode = LANE_IP_ARP;
	}

	if ( !match_found ) {
	    printf( "lsend: Unrecognized LANE Control Packet Type.\n");
	    atmtest_dump_lsend_arglist();
	    return( TCL_ERROR );
	}

    }
    else {
	/* No arguments; for now, just dump the argument list */
	atmtest_dump_lsend_arglist();
        return( TCL_OK );
    }

    switch ( opcode ) {
        
	case LANE_CONFIG_REQ:
	case LANE_CONFIG_RSP:
	case LANE_JOIN_REQ:
	case LANE_JOIN_RSP:
	    return( atmtest_send_cfg_pkt( interp ) );
	    break;

	case LANE_ARP_REQ:
	case LANE_ARP_RSP:
	    return( atmtest_send_arp_pkt( interp ) );
	    break;

	case LANE_IP_ARP:
	    return( atmtest_send_ip_arp_pkt( interp ) );
	    break;


        default:
	    printf( "lsend: Unknown LANE control packet type.\n" );
	    atmtest_dump_lsend_arglist();
	    return( TCL_ERROR );

    }

} /* Tcl_SetLaneDefCmd() */

/* 
 *
 * Tcl_SetLaneDefCmd()
 *
 * Syntax: router(tcl)# set_lane_def <CR>
 *
 * Command routine for the 'set_lane_def' TCL command.
 * When this command is typed at the TCL prompt, each uninitialized
 * lane variable will be set to it's corresponding default value.  The 
 * atmtest_init_lane_vars() routine is used to perform this, and is
 * automatically called when the send (e.g. send_cfg_pkt) commands 
 * are issued.
 * Note that this function will not overwrite the value of a LANE
 * variable that is already set.
 *
 */
int Tcl_SetLaneDefCmd (ClientData clientData, Tcl_Interp *interp, int
			argc, char **argv)
{
    (void) atmtest_init_lane_vars( interp );

    return( TCL_OK );

} /* Tcl_SetLaneDefCmd() */


/* 
 *
 * Tcl_ShowVarsCmd()
 *
 * Syntax: router(tcl)# sh_vars [default] <CR>
 *
 * Command routine for the 'sh_vars' TCL command.
 * When this command is typed at the TCL prompt the entire lane
 * variable array will be dumped, in addition to other variables
 * used to test LANE and ATM functionality on the router.  The optional 
 * 'default' argument causes the dafault values for each variable to be 
 *  printed instead of the variable's actual value.
 *
 */
int Tcl_ShowVarsCmd (ClientData clientData, Tcl_Interp *interp, int
			argc, char **argv)
{
    char    *value;
    int	    i;

    if ( ( argc > 1 ) && ( strcmp( argv[1], "default" ) == 0 ) ) {
        
	/*
	 * Optional 'default' argument provided; dump the lane variables
	 * and their default values.
	 */
	printf( "\n\nLANE Variables and their associated default values:\n\n" );
	printf( "%25s%s\n", "Variable Name", "Default Value" );
	printf( "%25s%s\n", "-------------", "-------------" );

	for (i = 0; i < LE_CP_VAR_COUNT; i++ ) {
	    printf( "%25s%s\n", lane_var_array[i], lane_var_def_array[i] );
	}

    }
    else {

	/*
	 * Dump all the variables and their values, if a value has been 
	 * assigned.  Note that default values are assigned automatically
	 * when the 'send' commands are issued.
	 */
	printf( "\n\nLANE Variables and their actual values:\n\n" );
	printf( "%25s%s\n", "Variable Name", "Value" );
	printf( "%25s%s\n", "-------------", "-----" );

	for (i = 0; i < LE_CP_VAR_COUNT; i++ ) {
	    printf( "%25s", lane_var_array[i] );

	    value = Tcl_GetVar( interp, lane_var_array[i], 0 );
	    if ( value == NULL ) {  /* var not defined */
		printf( "(Undefined)\n" );
	    }

	    else {
		printf( "%s\n", value );
	    }
	}

        /*
	 * Dump the VCD array
	 */
	printf( "\n\nContents of the LANE VCD array:\n\n" );
        
	for ( i = LANE_LEC_DIRECT; i <= LANE_MAX_VC_TYPE; i++ ) {
            printf( "%s(%s) = ", LANE_VCD_ARRAY_STR, lane_vcd_array[i] );
	    value = Tcl_GetVar2( interp, LANE_VCD_ARRAY_STR, lane_vcd_array[i], 0 );
	    if ( value == NULL ) /* var not defined */
	        printf( "(Undefined)\n" );
            else
	        printf( "%s\n", value );
        }
 
 
    }
    printf( "\n\n" );

    return( TCL_OK );

} /* Tcl_ShowVarsCmd() */
