/* $Id: atmtest_sig_cmds.c,v 3.3.60.2 1996/04/25 23:03:14 ronnie Exp $
 * $Source: /release/112/cvs/Xsys/atm-test/atmtest_sig_cmds.c,v $
 *------------------------------------------------------------------
 * atmtest_sig_cmds.c -- Command functions for ATM Signalling related
 * TCL commands.
 * [Was ../tcl/tcl_atmsig_cmds.c]
 *
 * April, 1995  Chris Parrott
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmtest_sig_cmds.c,v $
 * Revision 3.3.60.2  1996/04/25  23:03:14  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.3.60.1.2.1  1996/03/24  03:46:18  rzagst
 * ATM modularity: Remove AIP from all files that are not referring to the
 * AIP i/f
 * Branch: ATM_Cal_branch
 *
 * Revision 3.3.60.1  1996/03/18  19:00:44  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/07  08:31:10  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  13:37:53  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  08:47:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:07:30  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:03:06  shaker
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

#include "../if/atm.h"
#include "../atm/atmsig_api.h"
#include "../atm/atmsig_private.h"
#include "../lane/lane.h"

#include "../atm-test/atmtest_lane_cmds.h"
#include "../atm-test/atmtest_lane_vars.h"

#include "../atm-test/atmtest_sig_cmds.h"



/*
 * Note on the implementation of the ATM Signalling related 
 * TCL commands: 
 * 
 * Source code to implement the commands si, sln, scn, ss, 
 * sc, sr, and src was copied to a large extent from the file
 * tcl_init.c that was modified originally by Sanjaya Kumar 
 * (in a private test environment) to support testing ATM Signalling
 * on the Cisco router.  The routines in this file were modified as
 * necessary to work with the newest version of the sigAPI, but 
 * where possible the code was reused to maintain consistency.
 */
/* 
 *
 * Tcl_siCmd()
 *
 * Syntax: si <CR>
 *
 * Command routine for the 'si' (Initialize Signalling) TCL command.
 * At this time it's only real purpose in life is to call a routine to 
 * set the global 'g_interp' used by the ATM signalling callback function.
 * NOTE:  The global interp variable implementation is a hack.
 * When we figure out how to bind an interp from the callback routine
 * with this one, the global should be nuked and the 'interp' variable
 * passed normally.
 * 
 */
int Tcl_siCmd (ClientData clientData, Tcl_Interp *interp, int
			argc, char **argv)
{
    int i = 0, vc_type; 

    /*
     * Set the global interp to this interp.
     */
    atmtest_set_g_interp( interp );

    /*
     * NULL all the elements of the call_id array.
     */
    vc_type = LANE_LEC_DIRECT;
    for ( i = LANE_LEC_DIRECT; i <= LANE_MAX_VC_TYPE; i++ ) {
	atmtest_set_call_id( vc_type++, (atmSvcType *) NULL );	
    }

    /*
     * Semi-hack...
     * Link the elements of the lane_vcd_array to the respective elements of
     * the internal vcd_array.  
     */
    atmtest_link_lane_vcd_array( interp );
	
    return( TCL_OK );

} /* Tcl_siCmd() */

/* 
 *
 * Tcl_slnCmd()
 *
 * Syntax: sln <CR>
 *
 * Command routine for the 'sln' (Send LISTEN_NSAP) TCL command.
 * This command is used to send a LISTEN_NSAP message to the local
 * sigAPI to register our (TCL) callback routine and NSAP. 
 * 
 * This command must be run to initialize the "Called" end of an 
 * SVC setup.
 *
 * Note that this code was stolen from atmsig_smap.c.
 */
int Tcl_slnCmd (ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{

    int 	     	*pi, int_val;
    sig_api_msg      	*sig_msg, s_msg;
    sig_api_listen  	*listen_msg;
    char		*value;
    hwaddrtype          tmp_h_addr, *h_ptr;


    /* Dummy variables so we can use the existing parse routines. */
    parseinfo	     	*c_ptr = NULL;  /* Dummy ptr */
    idbtype		*i_ptr = NULL;  /* Dummy ptr */
    int			hlp_dummy = 0;
    uint		flg_dummy = 0;



    sig_msg = ( sig_api_msg * ) &s_msg;
    
    /* 
     * fill in the sig_api_listen block 
     */
 
    sig_msg->opcode = ATM_LISTEN_NSAP;
    sig_msg->idb = atmtest_get_atm_idb( interp );
 
    listen_msg = ( sig_api_listen * )&sig_msg->u.listen;

    listen_msg->sig_user_callback = atmtest_sig_callback_func;
 
    /*
     * Determine the ATM Address (NSAP) from the "local NSAP" TCL variable.
     */
    value = Tcl_GetVar( interp, ATM_VAR_LOC_NSAP, 0 );

    if ( value == NULL ) {
	return( TCL_ERROR );
    }

    int_val = 0; 
    pi = &int_val;  /* A little paranoid, but who knows... */

    h_ptr = &tmp_h_addr;
    if ( match_atm_nsap( c_ptr, value, pi, h_ptr, hlp_dummy,
					flg_dummy, i_ptr ) ) {
	bcopy( h_ptr->addr, &listen_msg->nsap.addr, STATIONLEN_ATMNSAP );
	listen_msg->nsap.type = STATION_ATMNSAP;
	listen_msg->nsap.length = STATIONLEN_ATMNSAP;
	listen_msg->nsap.target_type = TARGET_UNICAST;
    }
    else {   /* Some problem parsing the NSAP address */
	return( TCL_ERROR );
    }
 
    /* send to Signalling driver */
    debug_printf( "Sending ATM_LISTEN_NSAP message to the sigAPI\n", interp );
    atmtest_dump_buffer( (uchar *)listen_msg, sizeof( sig_api_listen ), interp );

    atmSig_handleUserRequest(sig_msg); 
 
    return ( TCL_OK );

} /* Tcl_slnCmd() */

/* 
 *
 * Tcl_scnCmd()
 *
 * Syntax: scn <CR>
 *
 * Command routine for the 'scn' (Send ATM_CANCEL_NSAP) TCL command.
 * This command is used to send a ATM_CANCEL_NSAP message to the local
 * sigAPI to register our (TCL) callback routine and NSAP. 
 * 
 * This command is run to de-register a signalling user from listening
 * on a particular ATM Address (NSAP). 
 *
 * Note that this code was stolen from atmsig_smap.c.
 */
int Tcl_scnCmd (ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{

    int 	     	*pi, int_val;
    sig_api_msg      	*sig_msg, s_msg;
    sig_api_cancel  	*cancel_msg;
    char		*value;
    hwaddrtype          tmp_h_addr, *h_ptr;


    /* Dummy variables so we can use the existing parse routines. */
    parseinfo	     	*c_ptr = NULL;  /* Dummy ptr */
    idbtype		*i_ptr = NULL;  /* Dummy ptr */
    int			hlp_dummy = 0;
    uint		flg_dummy = 0;


    
    sig_msg = ( sig_api_msg * ) &s_msg;
 
    /* 
     * fill in the sig_api_cancel block 
     */
 
    sig_msg->opcode = ATM_CANCEL_NSAP;
    sig_msg->idb = atmtest_get_atm_idb( interp );
 
    cancel_msg = ( sig_api_cancel * )&sig_msg->u.cancel;
 
    /*
     * Determine the ATM Address (NSAP) from the "local NSAP" TCL variable.
     */
    value = Tcl_GetVar( interp, ATM_VAR_LOC_NSAP, 0 );

    if ( value == NULL )
	printf( "Error: Tcl_scnCmd: Null NSAP\n" );
	return( TCL_ERROR );

    int_val = 0; 
    pi = &int_val;  /* A little paranoid, but who knows... */

    h_ptr = &tmp_h_addr;
    if ( match_atm_nsap( c_ptr, value, pi, h_ptr, hlp_dummy,
					flg_dummy, i_ptr ) ) {
	bcopy( h_ptr->addr, &cancel_msg->nsap.addr, STATIONLEN_ATMNSAP );
    }
    else {   /* Some problem parsing the NSAP address */
	printf( "Error: Tcl_scnCmd: Unable to parse NSAP\n" );
	return( TCL_ERROR );
    }
 
    /* send to Signalling driver */
    debug_printf( "Sending ATM_CANCEL_NSAP message to the sigAPI\n", interp );
    atmtest_dump_buffer( (uchar *)cancel_msg, sizeof( sig_api_cancel ), interp );


    atmSig_handleUserRequest( sig_msg ); 
 
    return ( TCL_OK );

} /* Tcl_scnCmd() */

/* 
 *
 * Tcl_ssCmd()
 *
 * Syntax: ss <CR>
 *
 * Command routine for the 'ss' (Send ATM_SETUP) TCL command.
 * This command is used to send a ATM_SETUP message to the local
 * sigAPI to establish an SVC with a remote host.
 * 
 * Note that this code was stolen from atmsig_smap.c.
 */
int Tcl_ssCmd (ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{

    int 	     	*pi, int_val, nsap_len;
    sig_api_msg      	*sig_msg, s_msg;
    sig_api_setup  	*setup_msg;
    char		*value;
    hwaddrtype          tmp_h_addr, *h_ptr;
    atmSvcType 		*call_id;
    boolean		bool_input = FALSE;


    /* Dummy variables so we can use the existing parse routines. */
    parseinfo	     	*c_ptr = NULL;  /* Dummy ptr */
    idbtype		*i_ptr = NULL;  /* Dummy ptr */
    int			hlp_dummy = 0;
    uint		flg_dummy = 0;


    sig_msg = ( sig_api_msg * ) &s_msg;
 

    /*
     * Get a new call_id
     */
    call_id = atmSig_api_allocate_call_id();
    if ( !call_id ) {
        printf( "No memory to alloc call_id block, open svc req failed\n" );
        return ( TCL_ERROR );
    }

    /*
     * Store the call_id in the call_id array location
     * specified by the current_vc TCL global for future 
     * reference.
     */
    if ( atmtest_store_call_id( call_id, interp ) != TCL_OK ) {
	return( TCL_ERROR ); 
    }

    /* 
     * fill in the sig_api_setup block 
     */
 
    sig_msg->opcode = ATM_SETUP;
    sig_msg->idb = atmtest_get_atm_idb( interp );
 
    setup_msg = ( sig_api_setup * )&sig_msg->u.setup;

    setup_msg->call_id = call_id;
    setup_msg->sig_user_callback = atmtest_sig_callback_func;

    /*
     * Initialize the params field to defaults.
     */
    atm_params_init( &setup_msg->params );

    
    /*
     * Get the Calling Party ATM Address length from corresponding
     * TCL variable.
     */
    value = Tcl_GetVar( interp, ATM_VAR_LOC_NSAP_LEN, 0 );
    if ( value == NULL ) {
	return( TCL_ERROR );
    }
    if ( Tcl_GetInt( interp, value, &nsap_len ) == TCL_ERROR ) {
       buginf( "Error: Cannot convert %s to an integer.\n", ATM_VAR_LOC_NSAP_LEN ); 	
       return( TCL_ERROR );
    }

    /*
     * Determine the Calling Party ATM Address (NSAP) from the 
     * "local NSAP" TCL variable.
     */
    value = Tcl_GetVar( interp, ATM_VAR_LOC_NSAP, 0 );

    if ( value == NULL ) {
	return( TCL_ERROR );
    }


    int_val = 0; 
    pi = &int_val;  /* A little paranoid, but who knows... */

    h_ptr = &tmp_h_addr;
    if ( match_atm_nsap( c_ptr, value, pi, h_ptr, hlp_dummy,
					flg_dummy, i_ptr ) ) {
	bcopy( h_ptr->addr, &setup_msg->calling_party.addr, STATIONLEN_ATMNSAP );
        setup_msg->calling_party.type = STATION_ATMNSAP;
        setup_msg->calling_party.length = nsap_len;
        setup_msg->calling_party.target_type = TARGET_UNICAST;
    }
    else {   /* Some problem parsing the NSAP address */
	return( TCL_ERROR );
    }
 
    /*
     * Get the Called Party ATM Address length from corresponding
     * TCL variable.
     */
    value = Tcl_GetVar( interp, ATM_VAR_RMT_NSAP_LEN, 0 );
    
    if ( value == NULL ) {
	return( TCL_ERROR );
    }
    if ( Tcl_GetInt( interp, value, &nsap_len ) == TCL_ERROR ) {
       buginf( "Error: Cannot convert %s to an integer.\n", ATM_VAR_RMT_NSAP_LEN ); 	
       return( TCL_ERROR );
    }

    /*
     * Determine the Called Party ATM Address (NSAP) from the 
     * "remote NSAP" TCL variable.
     */
    value = Tcl_GetVar( interp, ATM_VAR_RMT_NSAP, 0 );

    if ( value == NULL ) {
	return( TCL_ERROR );
    }

    int_val = 0; 
    pi = &int_val;  /* A little paranoid, but who knows... */

    if ( match_atm_nsap( c_ptr, value, pi, h_ptr, hlp_dummy,
					flg_dummy, i_ptr ) ) {
	bcopy( h_ptr->addr, &setup_msg->called_party.addr, STATIONLEN_ATMNSAP );
        setup_msg->called_party.type = STATION_ATMNSAP;
        setup_msg->called_party.length = nsap_len;
        setup_msg->called_party.target_type = TARGET_UNICAST;
    }
    else {   /* Some problem parsing the NSAP address */
	return( TCL_ERROR );
    }

    /*
     * Should this be hard coded...?
     */
    setup_msg->flags = ATM_VC_AAL5LANE | VC_FLAG_NOIDLETIME;

    /*
     * Fill in Broadband Lower Layer info with their corresponding
     * TCL variable values.
     */
    if ( atmtest_stuff_blli_fields( &(setup_msg->blli[0]), interp ) != TCL_OK ) {
        printf( "Cannot stuff the blli fields of the ATM_SETUP msg.\n" );
	return( TCL_ERROR );
    }

#if 0
{
    int i, j;

    /*
     * I suspect a bug...dump the contents of the BLLI fields of the
     * message...
     */	
    for ( i = 0; i < ATM_MAX_BLLI_NUM; i++ ) {
	printf( "%ssetup_msg->blli[%d].length = %d\n", ATMTEST_LANE_DEBUG_LEADER,
						i, setup_msg->blli[i].length );
	printf( "%ssetup_msg->blli[%d] = ", ATMTEST_LANE_DEBUG_LEADER, i );
	for ( j = 0; j < ATM_BBAND_LOLAYER_MAXSIZE; j++ ) {
	    printf( "%02x", setup_msg->blli[i].string[j] );
        }
	printf( "\n" );
    }
}
#endif

    /*
     * Set the user_vc_type based on the current_vc TCL variable.
     */
    value = Tcl_GetVar( interp, LANE_CURRENT_VC, 0 );

    if ( value == NULL ) {
	printf( "Error retrieving the current_vc variable.\n" );
	return( TCL_ERROR );
    }
    setup_msg->user_vc_type = ( user_vc_type_t )atmtest_get_vc_type( value );

    /*
     * Stuff the multipoint field based on the associated TCL variable.
     */
    value = Tcl_GetVar( interp, ATM_SIG_MULTIPOINT, 0 );

    if ( value == NULL ) {
	buginf( "Error retrieving the multipoint variable.\n" );
	return( TCL_ERROR );
    }
    if ( Tcl_GetBoolean( interp, value, &bool_input ) == TCL_OK ) {
	setup_msg->multipoint = (boolean) bool_input;
    }
    else {
        buginf( "Error: multipoint variable must be a boolean.\n" );	
	return( TCL_ERROR );
    }
 
    /* send to Signalling driver */
    debug_printf( "Sending ATM_SETUP message to the sigAPI\n", interp );
    atmtest_dump_buffer( (uchar *)setup_msg, sizeof( sig_api_setup ), interp );

    atmSig_handleUserRequest( sig_msg ); 
 
    return ( TCL_OK );

} /* Tcl_ssCmd() */

/* 
 *
 * Tcl_scCmd()
 *
 * Syntax: sc <CR>
 *
 * Command routine for the 'sc' (Send ATM_CONNECT) TCL command.
 * This command is used to send a ATM_CONNECT message to the
 * sigAPI (in response to a received ATM_SETUP message) to establish 
 * an SVC with the remote host.  The ATM_CONNECT message is of course
 * forwarded to the remote host by the sigAPI.
 * 
 * This command is of course NOT run automatically by TCL; the 
 * sc command must either be explicitly issued by a user, or by
 * a scripting tool such as 'expect'.
 *
 * Note that this code was stolen from atmsig_smap.c.
 */
int Tcl_scCmd (ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{

    sig_api_msg      	*sig_msg, s_msg;
    sig_api_connect  	*connect_msg;
    atmSvcType		*call_id;
    char 		*value;



    sig_msg = ( sig_api_msg * ) &s_msg;
 
    /* 
     * fill in the sig_api_connect block 
     */
 
    sig_msg->opcode = ATM_CONNECT;
 
    connect_msg = ( sig_api_connect * )&sig_msg->u.connect;

    /*
     * Get the call_id stored in the call_id array for 
     * the global current_vc; it should have been set automatically
     * by the callback routine upon receipt of a ATM_SETUP message.  
     * If it's NULL, bail.
     */
    call_id = atmtest_retrieve_call_id( interp );
    if ( call_id == NULL ) {
	printf( "Cannot retrieve call_id.\n" );
	return( TCL_ERROR );
    }
    connect_msg->call_id = call_id;

    /*
     * Should this be hard coded...?
     */
    connect_msg->flags = ATM_VC_AAL5LANE;

    /*
     * Set the user_vc_type based on the current_vc TCL variable.
     */
    value = Tcl_GetVar( interp, LANE_CURRENT_VC, 0 );

    if ( value == NULL ) {
	printf( "Error retrieving the current_vc variable.\n" );
	return( TCL_ERROR );
    }
    connect_msg->user_vc_type = ( user_vc_type_t )atmtest_get_vc_type( value );
 
    /* send to Signalling driver */
    debug_printf( "Sending ATM_CONNECT message to the sigAPI\n", interp );
    atmtest_dump_buffer( (uchar *)connect_msg, sizeof( sig_api_connect ), interp );

    atmSig_handleUserRequest( sig_msg ); 
 
    return ( TCL_OK );

} /* Tcl_scCmd() */

/* 
 *
 * Tcl_srCmd()
 *
 * Syntax: sr <CR>
 *
 * Command routine for the 'sr' (Send ATM_RELEASE) TCL command.
 * This command is used to send a ATM_RELEASE message to the
 * sigAPI to tear-down an SVC.  
 * 
 * 
 */
int Tcl_srCmd (ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{

    sig_api_msg      	*sig_msg, s_msg;
    sig_api_release  	*release_msg;
    atmSvcType		*call_id;



    sig_msg = ( sig_api_msg * ) &s_msg;
 
    /* 
     * fill in the sig_api_release block 
     */
 
    sig_msg->opcode = ATM_RELEASE;
 
    release_msg = ( sig_api_release * )&sig_msg->u.release;

    /*
     * Get the call_id stored in the call_id array for 
     * the global current_vc; it should have been set automatically
     * by the callback routine upon initiating or receiving a ATM_SETUP message.  
     * If it's NULL, bail.
     */
    call_id = atmtest_retrieve_call_id( interp );
    if ( call_id == NULL ) {
	printf( "Cannot retrieve NULL call_id.\n" );
	return( TCL_ERROR );
    }
    release_msg->call_id = call_id;

    /*
     * Arbitrary for now; could be another TCL variable.
     */
    release_msg->cause = ATM_NORMAL_UNSPECIFIED;

 
    /* send to Signalling driver */
    debug_printf( "Sending ATM_RELEASE message to the sigAPI\n", interp );
    atmtest_dump_buffer( (uchar *)release_msg, sizeof( sig_api_release ), interp );

    atmSig_handleUserRequest( sig_msg ); 
 
    /*
     * Clear the call_id and vcd elements of their respective 
     * arrays based on the call_id.
     */
    atmtest_clear_array_entries( call_id, interp );

    return ( TCL_OK );

} /* Tcl_srCmd() */

/* 
 *
 * Tcl_srcCmd()
 *
 * Syntax: sr <CR>
 *
 * Command routine for the 'src' (Send ATM_RELEASE_COMPLETE) TCL command.
 * This command is used to send a ATM_RELEASE_COMPLETE message to the
 * sigAPI in response to an ATM_RELEASE message to confirm the tear-down
 * of an SVC.
 * 
 * 
 */
int Tcl_srcCmd (ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{

    sig_api_msg      		*sig_msg, s_msg;
    sig_api_release_comp  	*release_msg;
    atmSvcType			*call_id;



    sig_msg = ( sig_api_msg * ) &s_msg;
 
    /* 
     * fill in the sig_api_release_comp block 
     */
 
    sig_msg->opcode = ATM_RELEASE_COMPLETE;
 
    release_msg = ( sig_api_release_comp * )&sig_msg->u.release;

    /*
     * Get the call_id stored in the call_id array for 
     * the global current_vc; it should have been set automatically
     * by the callback routine upon initiating or receiving a ATM_SETUP message.  
     * If it's NULL, bail.
     */
    call_id = atmtest_retrieve_call_id( interp );
    if ( call_id == NULL ) {
	printf( "Cannot retrieve NULL call_id.\n" );
	return( TCL_ERROR );
    }
    release_msg->call_id = call_id;

    /*
     * Arbitrary for now; could be another TCL variable.
     */
    release_msg->cause = ATM_NORMAL_UNSPECIFIED;

 
    /* send to Signalling driver */
    debug_printf( "Sending ATM_RELEASE_COMPLETE message to the sigAPI\n", interp );
    atmtest_dump_buffer( (uchar *)release_msg, sizeof( sig_api_release_comp ), interp );

    atmSig_handleUserRequest( sig_msg ); 

    /*
     * Clear the call_id and vcd elements of their respective 
     * arrays based on the call_id.
     */
    atmtest_clear_array_entries( call_id, interp );
 
    return ( TCL_OK );

} /* Tcl_srcCmd() */

/* 
 *
 * Tcl_sapCmd()
 *
 * Syntax: sap <CR>
 *
 * Command routine for the 'sap' (Send ATM_ADD_PARTY) TCL command.
 * This command is used to send a ATM_ADD_PARTY message to the
 * sigAPI to initiate the addition of a client (as a leaf) of a 
 * point to multipoint SVC.
 * 
 * 
 */
int Tcl_sapCmd (ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{

    sig_api_msg      		*sig_msg, s_msg;
    sig_api_add_party  		*add_party_msg;
    atmSvcType			*call_id;
    atmSvcPartyType		*endpoint_id;
    char			*value;
    int				nsap_len, int_val, *pi;
    hwaddrtype          	tmp_h_addr, *h_ptr;

    /* Dummy variables so we can use the existing parse routines. */
    parseinfo	     		*c_ptr = NULL;  /* Dummy ptr */
    idbtype			*i_ptr = NULL;  /* Dummy ptr */
    int				hlp_dummy = 0;
    uint			flg_dummy = 0;



    sig_msg = ( sig_api_msg * ) &s_msg;
 
    /* 
     * fill in the sig_api_add_party block 
     */
 
    sig_msg->opcode = ATM_ADD_PARTY;
 
    add_party_msg = ( sig_api_add_party * )&sig_msg->u.add_party;

    /*
     * Get the call_id stored in the call_id array for 
     * the global current_vc; it should have been set automatically
     * by the callback routine upon initiating or receiving a ATM_SETUP message.  
     * If it's NULL, bail.
     */
    call_id = atmtest_retrieve_call_id( interp );
    if ( call_id == NULL ) {
	buginf( "Cannot retrieve NULL call_id.\n" );
	return( TCL_ERROR );
    }
    add_party_msg->call_id = call_id;

    /*
     * Get a new endpoint_id
     */
    endpoint_id = atmSig_api_allocate_endpoint_id( call_id );
    if ( !endpoint_id ) {
        buginf( "No memory to alloc endpoint_id block, open svc req failed\n" );
        return ( TCL_ERROR );
    }
    add_party_msg->endpoint_id = endpoint_id;
     
    /*
     * Store the endpoint_id in the endpoint_id array location
     * specified by the current_vc TCL global for future 
     * reference.
     * We need to define a different TCL global to reference added parties.
     */
    if ( atmtest_store_endpoint_id( endpoint_id, interp ) != TCL_OK ) {
	return( TCL_ERROR ); 
    }

    /*
     * Get the Called Party ATM Address length from corresponding
     * TCL variable.
     */
    value = Tcl_GetVar( interp, ATM_VAR_RMT_NSAP_LEN, 0 );
    
    if ( value == NULL ) {
	return( TCL_ERROR );
    }
    if ( Tcl_GetInt( interp, value, &nsap_len ) == TCL_ERROR ) {
       buginf( "Error: Cannot convert %s to an integer.\n", ATM_VAR_RMT_NSAP_LEN ); 	
       return( TCL_ERROR );
    }

    /*
     * Determine the Called Party ATM Address (NSAP) from the 
     * "remote NSAP" TCL variable.
     */
    value = Tcl_GetVar( interp, ATM_VAR_RMT_NSAP, 0 );

    if ( value == NULL ) {
	return( TCL_ERROR );
    }

    int_val = 0; 
    pi = &int_val;  /* A little paranoid, but who knows... */

    h_ptr = &tmp_h_addr;
    if ( match_atm_nsap( c_ptr, value, pi, h_ptr, hlp_dummy,
					flg_dummy, i_ptr ) ) {
	bcopy( h_ptr->addr, &add_party_msg->called_party.addr, STATIONLEN_ATMNSAP );
        add_party_msg->called_party.type = STATION_ATMNSAP;
        add_party_msg->called_party.length = nsap_len;
        add_party_msg->called_party.target_type = TARGET_UNICAST;
    }
    else {   /* Some problem parsing the NSAP address */
	return( TCL_ERROR );
    }

    /* send to Signalling driver */
    debug_printf( "Sending ATM_ADD_PARTY message to the sigAPI\n", interp );
    atmtest_dump_buffer( (uchar *)add_party_msg, sizeof( sig_api_add_party ), interp );

    atmSig_handleUserRequest( sig_msg ); 
 
    return ( TCL_OK );

} /* Tcl_sapCmd() */

/* 
 *
 * Tcl_sdpCmd()
 *
 * Syntax: sdp <CR>
 *
 * Command routine for the 'sdp' (Send ATM_DROP_PARTY) TCL command.
 * This command is used to send a ATM_DROP_PARTY message to the
 * sigAPI to initiate the dropping of a client (as a leaf) of a 
 * point to multipoint SVC.
 * 
 * 
 */
int Tcl_sdpCmd (ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{

    sig_api_msg      		*sig_msg, s_msg;
    sig_api_drop_party  	*drop_party_msg;
    atmSvcType			*call_id;
    atmSvcPartyType		*endpoint_id;



    sig_msg = ( sig_api_msg * ) &s_msg;
 
    /* 
     * fill in the sig_api_drop_party block 
     */
 
    sig_msg->opcode = ATM_DROP_PARTY;
 
    drop_party_msg = ( sig_api_drop_party * )&sig_msg->u.drop_party;

    /*
     * Get the call_id stored in the call_id array for 
     * the global current_vc; it should have been set automatically
     * by the callback routine upon initiating or receiving a ATM_SETUP message.  
     * If it's NULL, bail.
     */
    call_id = atmtest_retrieve_call_id( interp );
    if ( call_id == NULL ) {
	buginf( "Cannot retrieve NULL call_id.\n" );
	return( TCL_ERROR );
    }
    drop_party_msg->call_id = call_id;

    /*
     * Retrieve the endpoint_id from the endpoint_id array.
     * Note that we allow a NULL endpoint_id, since it corresponds
     * to the first leaf (established via the SETUP).
     */
    endpoint_id = atmtest_retrieve_endpoint_id( interp );;

    drop_party_msg->endpoint_id = endpoint_id;
     
    drop_party_msg->cause = ATM_NORMAL_UNSPECIFIED;

    /*
     * Still Need to add: 
     * drop_party_msg->party_id
     */
 
    /* send to Signalling driver */
    debug_printf( "Sending ATM_DROP_PARTY message to the sigAPI\n", interp );
    atmtest_dump_buffer( (uchar *)drop_party_msg, sizeof( sig_api_drop_party ), interp );

    atmSig_handleUserRequest( sig_msg ); 
 
    return ( TCL_OK );

} /* Tcl_sdpCmd() */
