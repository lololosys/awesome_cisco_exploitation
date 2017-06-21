/* $Id: atmtest_lane_support.c,v 3.3.56.3 1996/05/09 14:11:52 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/atm-test/atmtest_lane_support.c,v $
 *------------------------------------------------------------------
 * atmtest_lane_support.c -- Support code for LAN Emulation related TCL 
 * commands. 
 * [Was ../tcl/tcl_lane_support.c]
 *
 * April, 1995  Chris Parrott
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmtest_lane_support.c,v $
 * Revision 3.3.56.3  1996/05/09  14:11:52  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.3.56.2.2.1  1996/04/27  06:34:14  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.3.56.2  1996/04/25  23:03:12  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.3.56.1.8.2  1996/04/08  14:49:46  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.3.56.1.8.1  1996/04/08  01:44:43  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.3.56.1  1996/03/18  19:00:42  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.60.1  1996/03/22  09:35:09  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.3.48.1  1996/03/05  05:49:01  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.3.36.1  1996/02/27  20:40:41  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.3.14.1  1996/01/11  01:44:42  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.10.1  1995/12/29  01:05:32  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.3  1995/11/17  08:47:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:07:29  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:03:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "ieee.h"
#include "ttysrv.h"
#include "config.h"
#include "file.h"
#include "parser.h"
#include "name.h"
#include "packet.h"
#include "../ip/ip.h"
#include "../tcl/tcl.h"
#include "../tcl/tcl_init.h"
#include "../h/address.h"
#include "../if/network.h"

#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/atm.h"
#include "../if/arp.h"
#include "../atm/atmsig_api.h"
#include "../lane/lane.h"
#include "../lane/lane_private.h"

#include "../atm-test/atmtest_lane_cmds.h"
#include "../atm-test/atmtest_lane_vars.h"
#include "../atm-test/atmtest_lane_arrays.h"
#include "../atm-test/atmtest_hlp_strings.h"


/* 
 *
 * atmtest_lane_debug_on()
 *
 * Returns TRUE if the variable defined by LANE_DEBUG_VAR is set true,
 * FALSE otherwise.  Used to switch on and off TCL LANE
 * debugging from the TCL command line.
 */
boolean atmtest_lane_debug_on( Tcl_Interp *interp )
{
    int bool_input;
    boolean result = FALSE;
    char *c_input;

    c_input = Tcl_GetVar( interp, LANE_DEBUG_VAR, 0 );

    if ( c_input == NULL ) { /* variable probably hasn't been set */
	return( FALSE );
    }

    if ( Tcl_GetBoolean( interp, c_input, &bool_input ) == TCL_OK ) {
	result = (boolean) bool_input;
	return( result );
    }

    else {/* LANE_DEBUG_VAR wasn't able to be evaluated as a boolean */

	buginf( "Must enter a boolean value for %s\n", LANE_DEBUG_VAR );
	return( FALSE );
    }

} /* atmtest_lane_debug_on() */

/* 
 *
 * atmtest_dump_buffer()
 *
 * Dump the contents of the buffer passed in if debugging is 
 * enabled.  The number of bytes printed in a row can be changed
 * via PKT_DUMP_WIDTH.
 */
void atmtest_dump_buffer( uchar *buf, int size, Tcl_Interp *interp )
{
    int i;

    if ( !atmtest_lane_debug_on( interp ) ) {
	return;
    }

    buginf( "\nContents of the message buffer:\n\n" );

    for ( i = 0; i < size; i++ ) {

	if ( !( i % PKT_DUMP_WIDTH ) )
	    buginf( "\n" );

        buginf( "%02x ", *buf++ );

    }

    buginf( "\n\n" );

} /* atmtest_dump_buffer() */


/* 
 *
 * debug_printf()
 *
 * Used to print debug info to the console so that it's easily
 * distiguishable from other output.  Will only print if debugging 
 * is enabled.
 */
void debug_printf( char *str, Tcl_Interp *interp )
{
   if ( atmtest_lane_debug_on( interp ) )
       printf( "%s%s\n", ATMTEST_LANE_DEBUG_LEADER, str ); 

} /* debug_printf() */

/* 
 * 
 * atmtest_init_lane_vars()
 *
 * This function initializes all the global variables that haven't
 * been set by the user usin the TCL 'set' command.  It works by
 * checking for the existence of the variable, and setting it to
 * a default value if it doesn't exist.
 *
 */
void atmtest_init_lane_vars( Tcl_Interp *interp )
{
    int i;


    debug_printf( "Initializing the following variables:", interp );

    /*
     * Initialize all of the variables contained in the LANE
     * variable array.
     */
    for (i = 0; i < LE_CP_VAR_COUNT; i++ ) {
        /*
         * Check to see if a variable exists and has a non-NULL 
         * value; if not, assign a default value.
         */
        if ( Tcl_GetVar( interp, lane_var_array[i], 0 ) == NULL ) {
	    if ( atmtest_lane_debug_on( interp ) ) 
	        printf( "%s   - %s = %s\n", ATMTEST_LANE_DEBUG_LEADER, 
				 lane_var_array[i], lane_var_def_array[i] );
	    Tcl_SetVar( interp, lane_var_array[i], lane_var_def_array[i], 0 );
        }
    }

    debug_printf( "Variable initialization complete.\n", interp );

} /* atmtest_init_lane_vars() */

/* 
 * 
 * atmtest_load_lane_hdr()
 *
 * This function is passed a pointer to the temporary buffer that 
 * it loads with the LANE variables representing the fields of
 * the header portion of a LANE control packet.  This routine is
 * used for all the different types of LANE control packets since the
 * header portion is the same (in terms of defined fields) for all types.
 *
 */
int atmtest_load_lane_hdr( uchar *bufptr, Tcl_Interp *interp )
{
    char 	     *value;
    int  	     i, int_value[LE_CP_HDR_COUNT];
    lane_ctl_hdr_t   *hdrptr;


    debug_printf( "Loading the LANE control packet header...", interp );
    
    hdrptr = ( lane_ctl_hdr_t  * ) bufptr;


    for ( i = 0; i < LE_CP_HDR_COUNT; i++ ) {

        value = Tcl_GetVar( interp, lane_var_array[i], 0 );

        if ( value == NULL )
	    return( TCL_ERROR );
        else {
	    /* Convert the fields to integers and stuff into the int array */
	    if ( Tcl_GetInt( interp, value, &int_value[i] ) == TCL_ERROR )
                return( TCL_ERROR );
	}
    }

    /* Now stuff the header fields of the datagram buffer */
    i = 0;
    hdrptr->marker           = (ushort) int_value[i++];
    hdrptr->protocol         = (uchar) int_value[i++];
    hdrptr->version          = (uchar) int_value[i++];
    hdrptr->opcode           = (ushort) int_value[i++];
    hdrptr->status           = (ushort) int_value[i++];
    hdrptr->transaction_id   = (ulong) int_value[i++];
    hdrptr->requestor_lecid  = (ushort) int_value[i++];
    hdrptr->flags            = (ushort) int_value[i];

    debug_printf( "LANE control packet header loaded.", interp );

    return( TCL_OK );

} /* atmtest_load_lane_hdr() */

/* 
 * 
 * atmtest_load_lane_cfg()
 *
 * This function is passed a pointer to the temporary buffer that 
 * it loads with the LANE variables representing the fields of
 * the body portion of a LANE config control packet.
 * This function may be used for packets other than the config
 * (e.g. the LANE 'join' control packets) since they differ only in the
 * contents of the fields, not the fields themselves.
 *
 */
int atmtest_load_lane_cfg( uchar *bufptr, Tcl_Interp *interp )
{
    char 		*value;
    int  	     	i, *pi, int_val, int_value[LE_CP_VAR_COUNT];
    lane_cfg_pdu_t      *cfgptr;
    hwaddrtype      	tmp_h_addr, *h_ptr;

    /* Dummy variables so we can use the existing parse routines. */
    parseinfo	     	*c_ptr = NULL;  /* Dummy ptr */
    idbtype		*i_ptr = NULL;  /* Dummy ptr */
    int			hlp_dummy = 0;
    uint		flg_dummy = 0;


    h_ptr = &tmp_h_addr;

    debug_printf( "Loading the LANE config packet body...", interp );

    cfgptr = ( lane_cfg_pdu_t * const ) bufptr;

    /* 
     * Stuff the src_lan_dest.tag field 
     */
    value = Tcl_GetVar( interp, LE_CP_VAR_SRC_DST_TAG, 0 );

    if ( value == NULL )
        return( TCL_ERROR );
   
    if ( Tcl_GetInt( interp, value, &int_val ) == TCL_ERROR )
        return( TCL_ERROR );

    cfgptr->src_lan_dest.tag = (ushort)int_val;

    
    /* 
     * Stuff the src_lan_dest.mac field MAC address 
     */ 
    value = Tcl_GetVar( interp, LE_CP_VAR_SRC_DST_MAC, 0 );

    if ( value == NULL )
	return( TCL_ERROR );

    /* 
     * match_en_addr expects pi to point to an integer = 0.
     * Note that intval will be changed by the called procedure.
     */
    int_val = 0; 
    pi = &int_val;

    if ( match_en_addr( c_ptr, value, pi, h_ptr, hlp_dummy,
					flg_dummy, i_ptr ) ) {
	bcopy( h_ptr->addr, cfgptr->src_lan_dest.addr.mac,
						 IEEEBYTES );
    }
    else {   /* Some problem parsing the MAC address */
	return( TCL_ERROR );
    }

    /* 
     * Stuff the target_dest.tag field 
     */
    value = Tcl_GetVar( interp, LE_CP_VAR_TGT_DST_TAG, 0 );

    if ( value == NULL )
        return( TCL_ERROR );
   
    if ( Tcl_GetInt( interp, value, &int_val ) == TCL_ERROR )
        return( TCL_ERROR );

    cfgptr->target_dest.tag = (ushort)int_val;


    /* 
     * Stuff the target_dest.mac field MAC address 
     */ 
    value = Tcl_GetVar( interp, LE_CP_VAR_TGT_DST_MAC, 0 );

    if ( value == NULL )
	return( TCL_ERROR );

    int_val = 0; 
    pi = &int_val;  /* A little paranoid, but who knows... */

    if ( match_en_addr( c_ptr, value, pi, h_ptr, hlp_dummy,
					flg_dummy, i_ptr ) ) {
	bcopy( h_ptr->addr, cfgptr->target_dest.addr.mac,
						 IEEEBYTES );
    }
    else {   /* Some problem parsing the MAC address */
	return( TCL_ERROR );
    }


    /* 
     * Stuff the src_atm_addr field NSAP address 
     */ 
    value = Tcl_GetVar( interp, LE_CP_VAR_SRC_ATM_ADDR, 0 );

    if ( value == NULL )
	return( TCL_ERROR );

    int_val = 0; 
    pi = &int_val;  /* A little paranoid, but who knows... */

    if ( match_atm_nsap( c_ptr, value, pi, h_ptr, hlp_dummy,
					flg_dummy, i_ptr ) ) {
	bcopy( h_ptr->addr, cfgptr->src_atm_addr, 
					STATIONLEN_ATMNSAP );
    }
    else {   /* Some problem parsing the NSAP address */
	return( TCL_ERROR );
    }


    /* 
     * Read a bunch of vars into an integer array to 
     * save some coding!
     */
    for ( i = LE_CP_LAN_TYPE_VAR_POS; 
	      i < (LE_CP_LAN_TYPE_VAR_POS + LE_CP_CFG_STUFF_GRP_SZ); i++ ) {

        value = Tcl_GetVar( interp, lane_var_array[i], 0 );

        if ( value == NULL )
	    return( TCL_ERROR );
        else {
	    /* Convert the fields to integers and stuff into the int array */
	    if ( Tcl_GetInt( interp, value, &int_value[i] ) == TCL_ERROR )
                return( TCL_ERROR );
	}
    }

    /* Now stuff the cfg fields of the datagram buffer */
    i = LE_CP_LAN_TYPE_VAR_POS;
    cfgptr->lan_type         = (uchar) int_value[i++];
    cfgptr->max_frame_size   = (uchar) int_value[i++];
    cfgptr->num_tlvs   	     = (uchar) int_value[i++];

    /*
     * TLVS isn't currently supported
     */
    if ( cfgptr->num_tlvs > 0 ) {
        debug_printf("Error: Num TLVS must currently be zero.", interp );
	return( TCL_ERROR );
    }

    cfgptr->name_size        = (uchar) int_value[i++];

    if ( cfgptr->name_size > MAX_ELAN_NAME_LENGTH ) {
	debug_printf("Error: ELAN name length is too long.", interp );
	return( TCL_ERROR );
    }

    /* 
     * Stuff the target_atm_addr field NSAP address 
     */ 
    value = Tcl_GetVar( interp, LE_CP_VAR_TGT_ATM_ADDR, 0 );

    if ( value == NULL )
	return( TCL_ERROR );

    int_val = 0; 
    pi = &int_val;  /* A little paranoid, but who knows... */

    if ( match_atm_nsap( c_ptr, value, pi, h_ptr, hlp_dummy,
					flg_dummy, i_ptr ) ) {
	bcopy( h_ptr->addr, cfgptr->target_atm_addr, 
					STATIONLEN_ATMNSAP );
    }
    else {   /* Some problem parsing the NSAP address */
	return( TCL_ERROR );
    }

    /*
     * Stuff the ELAN name string into the buffer if 
     * the length is greater than 0.
     */
    if ( cfgptr->name_size > 0 ) {
        value = Tcl_GetVar( interp, LE_CP_VAR_ELAN_NAME, 0 );

        if ( value == NULL )
	    return( TCL_ERROR );

	bcopy( value, cfgptr->elan_name, cfgptr->name_size );
    }

    /*
     * Zero-out the remainder of the elan name buffer.
     */
    for ( i = cfgptr->name_size; i < MAX_ELAN_NAME_LENGTH; i++ ) {
	cfgptr->elan_name[i] = 0;
    }

    /*
     * When it's supported, the TLVS stuffing will go here.
     */

    debug_printf( "LANE config packet body loaded.", interp );

    return( TCL_OK );

} /* atmtest_load_lane_cfg() */

/* 
 * 
 * atmtest_get_atm_interface()
 *
 * This function is passed the slot #, port #, and subinterface on
 * this router that has been specified via TCL variables to be the 
 * ATM subinterface which we send the packet out.  A pointer to the 
 * corresponding IDB (if it exists) is returned.  If the IDB isn't 
 * found, NULL is returned.
 *
 * This routine is essentially the same routine used in the parser
 * (actions.c: find_interface()) stolen for our purposes (we don't
 * create subinterfaces...).
 */
idbtype *atmtest_get_atm_interface( uint slot_n, uint port_n, 
					 uint subif, Tcl_Interp *interp )
{
    hwidbtype *ifp = NULL;
    idbtype *swifp = NULL;


    debug_printf( "Attempting to get matching ATM IDB", interp );

    /*
     * Search though all the hardware IDBs until we find the ATM interface
     * that matches.
     */
    FOR_ALL_HWIDBS( ifp ) {
	if ( ( strcmp( ifp->name, ATMTEST_ATM_INTERFACE_NAME ) == 0 ) && 
		( ifp->slot == slot_n ) && ( ifp->slotunit == port_n ) ) {
            /* Valid Interface */
	    debug_printf( "Found an ATM IDB that matches variables", interp );
	    break;
        }
    }

    swifp = firstsw_or_null( ifp );

    if ( swifp && swifp->hwptr && ( swifp->hwptr->status & IDB_DELETED ) ) {
	/* Found a match, but it's deleted. */
	debug_printf( "Found a deleted IDB...no dice.", interp );
	return( NULL );
    }

    if ( ifp == NULL ) {
        /* Couldn't find an interface that matched. */
	debug_printf( "Couldn't find a matching IDB...no dice.", interp );
	return( NULL );
    }

    else {
	/*
	 * Found the matching hardware interface.  Now look for the
	 * appropriate subinterface, if any.  
	 */
	if ( subif ) {
	    if ( !subinterfaces_allowed_here( ifp ) ) {
	        debug_printf( "Subinterface is not allowed on this interface.", 
									interp );
		return( NULL );
            }

	    else {
		FOR_ALL_SWIDBS_ON_HW( ifp, swifp ) {
		    if ( swifp->sub_number == subif ) 
			return( swifp );
                }
		/*
		 * Non-zero subinterface was specified, but we couldn't
		 * find it, so return a null IDB.
		 */
		debug_printf( "Couldn't find a matching subinterface...no dice.", 
									  interp );	  
		return( NULL );
            }
	}
    }

    return(swifp);
    
} /* atmtest_get_atm_interface() */


/* 
 * 
 * atmtest_send_ctl_pkt()
 *
 * This function is passed a pointer to the temporary buffer that 
 * contains the datagram (LANE control packet) that we've packed
 * and are ready to send, along with the size of the datagram.  
 * When invoked this function allocates a pak, loads the datagram, 
 * sets required variables, and sends the packet out the specified 
 * interface.
 *
 */
int atmtest_send_ctl_pkt( uchar *bufptr, int dgram_size, Tcl_Interp *interp )
{
    uchar	*p_ptr;
    paktype	*pak;
    char	*value;
    int		i_value;
    ushort	vc;
    


    /*
     * Grab a pak buffer.
     */
    pak = getbuffer( ATMTEST_LANE_PKT_ALLOC_SZ );

    if ( !pak ) {
	Tcl_AppendResult(interp, "\nError: Could not get a pak buffer.\n", NULL);
	return( TCL_ERROR );
    }

    pak->datagramstart = pak->network_start;
    p_ptr = ( uchar * )pak->datagramstart;

    /*
     * for now, we'll make the datagram size the same size as our 
     * buffer, but we could eventually make the length adjustable
     * via another TCL variable.
     */
    pak->datagramsize = dgram_size; 

    /*
     * Load the datagram portion of the packet with the contents 
     * of our temporary datagram buffer.
     */
    bcopy( bufptr, p_ptr, dgram_size ); 

    /*
     * Load the if_output portion of the packet so that it goes
     * out the proper interface based on the VCD, slot, port, and
     * subinterface global variables.
     */

    /* Get the VCD */
    value = Tcl_GetVar( interp, LE_VAR_VCD, 0 ); 
    if ( value == NULL ) {
        datagram_done(pak);	
	return( TCL_ERROR );
    }

    if ( Tcl_GetInt( interp, value, &i_value ) == TCL_ERROR ) {
        datagram_done(pak);
	return( TCL_ERROR );
    }
    vc = (ushort) i_value;


    pak->if_output = atmtest_get_atm_idb( interp ); 

    if ( pak->if_output == NULL ) {
        Tcl_AppendResult(interp, 
		   "\nError: Invalid interface specified.\n", NULL);	

        datagram_done(pak);		    
	return( TCL_ERROR );
    }

    /*
     * Encapsulate control packets with vcd/enctype before the packet
     * is sent.
     */
    if ( !lane_control_encap( pak, vc ) ) {
	Tcl_AppendResult(interp, 
	      "\nError: ATM Control Encapsulation failed.\n", NULL);

	debug_printf( "Error: ATM Control Encapsulation failed.", interp );
        datagram_done(pak);
	return( TCL_ERROR );
    }

    /* We successfully stuffed the packet, so send it out! */
    debug_printf( "Calling datagram_out to send packet...", interp );
    datagram_out(pak);

    debug_printf( "Packet sent...returning to command routine.", interp );
    return( TCL_OK );

} /* atmtest_send_ctl_pkt() */

/* 
 *
 * atmtest_send_cfg_pkt()
 *
 * This function causes a LANE Configuration
 * Control Frame to be sent over an ATM interface based on the
 * values supplied by the user for the fields of the packet.
 * 
 * Note that it is expected that the proper global variables for
 * the fields of the packet (e.g. slot#, VCD, etc.) have been 
 * set by the user prior to running this command.
 */
int atmtest_send_cfg_pkt ( Tcl_Interp *interp )
{
    uchar	dgram_buf[CFG_CTL_PKT_SZ];

    /*
     * If any TCL variables haven't been set at the TCL prompt
     * prior to running this command, then set them to default
     * values.
     * Note that these TCL variables are global.
     */
    (void) atmtest_init_lane_vars( interp );

    /*
     * Load the temporary buffer with the header info, which should
     * be common to all lane control packets.
     */
    if ( atmtest_load_lane_hdr( dgram_buf, interp ) != TCL_OK ) {
	Tcl_AppendResult(interp, "\nError: Cannot load LANE hdr variables.\n", NULL);
	return( TCL_ERROR );
    }

    /*
     * Load the 'Configuration Control Packet' specific fields.
     * These fields are common to some of the control packets, so we
     * do it one place.
     */
    if ( atmtest_load_lane_cfg( dgram_buf, interp ) != TCL_OK ) {
	Tcl_AppendResult(interp, "\nError: Cannot load LANE cfg variables.\n", NULL);
	return( TCL_ERROR );
    }

    (void) atmtest_dump_buffer( dgram_buf, CFG_CTL_PKT_SZ, interp ); 

    /*
     * Now send the packet out the specified interface.  Return an error
     * if we can't send the packet for some reason.
     */

    if ( atmtest_send_ctl_pkt( dgram_buf, CFG_CTL_PKT_SZ, interp ) != TCL_OK ) {
	Tcl_AppendResult(interp, "\nError: LANE Cfg pkt not sent\n", NULL);	
	return( TCL_ERROR );
    }

    return( TCL_OK );


} /* atmtest_send_cfg_pkt() */

/* 
 * 
 * atmtest_load_lane_arp()
 *
 * This function is passed a pointer to the temporary buffer that 
 * it loads with the LANE variables representing the fields of
 * the body portion of a LANE LE-ARP control packet.
 *
 */
int atmtest_load_lane_arp( uchar *bufptr, Tcl_Interp *interp )
{
    char 		*value;
    int  	     	i, *pi, int_val;
    lane_arp_pdu_t      *cfgptr;
    hwaddrtype      	tmp_h_addr, *h_ptr;

    /* Dummy variables so we can use the existing parse routines. */
    parseinfo	     	*c_ptr = NULL;  /* Dummy ptr */
    idbtype		*i_ptr = NULL;  /* Dummy ptr */
    int			hlp_dummy = 0;
    uint		flg_dummy = 0;


    h_ptr = &tmp_h_addr;

    debug_printf( "Loading the LANE LE-ARP packet body...", interp );

    cfgptr = ( lane_arp_pdu_t * const ) bufptr;

    /* 
     * Stuff the src_lan_dest.tag field 
     */
    value = Tcl_GetVar( interp, LE_CP_VAR_SRC_DST_TAG, 0 );

    if ( value == NULL )
        return( TCL_ERROR );
   
    if ( Tcl_GetInt( interp, value, &int_val ) == TCL_ERROR )
        return( TCL_ERROR );

    cfgptr->src_lan_dest.tag = (ushort)int_val;

    
    /* 
     * Stuff the src_lan_dest.mac field MAC address 
     */ 
    value = Tcl_GetVar( interp, LE_CP_VAR_SRC_DST_MAC, 0 );

    if ( value == NULL )
	return( TCL_ERROR );

    /* 
     * match_en_addr expects pi to point to an integer = 0.
     * Note that intval will be changed by the called procedure.
     */
    int_val = 0; 
    pi = &int_val;

    if ( match_en_addr( c_ptr, value, pi, h_ptr, hlp_dummy,
					flg_dummy, i_ptr ) ) {
	bcopy( h_ptr->addr, cfgptr->src_lan_dest.addr.mac,
						 IEEEBYTES );
    }
    else {   /* Some problem parsing the MAC address */
	return( TCL_ERROR );
    }

    /* 
     * Stuff the target_dest.tag field 
     */
    value = Tcl_GetVar( interp, LE_CP_VAR_TGT_DST_TAG, 0 );

    if ( value == NULL )
        return( TCL_ERROR );
   
    if ( Tcl_GetInt( interp, value, &int_val ) == TCL_ERROR )
        return( TCL_ERROR );

    cfgptr->target_dest.tag = (ushort)int_val;


    /* 
     * Stuff the target_dest.mac field MAC address 
     */ 
    value = Tcl_GetVar( interp, LE_CP_VAR_TGT_DST_MAC, 0 );

    if ( value == NULL )
	return( TCL_ERROR );

    int_val = 0; 
    pi = &int_val;  /* A little paranoid, but who knows... */

    if ( match_en_addr( c_ptr, value, pi, h_ptr, hlp_dummy,
					flg_dummy, i_ptr ) ) {
	bcopy( h_ptr->addr, cfgptr->target_dest.addr.mac,
						 IEEEBYTES );
    }
    else {   /* Some problem parsing the MAC address */
	return( TCL_ERROR );
    }


    /* 
     * Stuff the src_atm_addr field NSAP address 
     */ 
    value = Tcl_GetVar( interp, LE_CP_VAR_SRC_ATM_ADDR, 0 );

    if ( value == NULL )
	return( TCL_ERROR );

    int_val = 0; 
    pi = &int_val;  /* A little paranoid, but who knows... */

    if ( match_atm_nsap( c_ptr, value, pi, h_ptr, hlp_dummy,
					flg_dummy, i_ptr ) ) {
	bcopy( h_ptr->addr, cfgptr->src_atm_addr, 
					STATIONLEN_ATMNSAP );
    }
    else {   /* Some problem parsing the NSAP address */
	return( TCL_ERROR );
    }

    /*
     * For now, zero-out the first reserved field (4-bytes).
     * (It's hard-coded in the lane_arp_pdu_t structure, so it's
     * hard-coded here.
     */
    for ( i = 0; i < 4; i++ ) {
	cfgptr->reserved1[i] = NULL;
    }

    /* 
     * Stuff the target_atm_addr field NSAP address 
     */ 
    value = Tcl_GetVar( interp, LE_CP_VAR_TGT_ATM_ADDR, 0 );

    if ( value == NULL )
	return( TCL_ERROR );

    int_val = 0; 
    pi = &int_val;  /* A little paranoid, but who knows... */

    if ( match_atm_nsap( c_ptr, value, pi, h_ptr, hlp_dummy,
					flg_dummy, i_ptr ) ) {
	bcopy( h_ptr->addr, cfgptr->target_atm_addr, 
					STATIONLEN_ATMNSAP );
    }
    else {   /* Some problem parsing the NSAP address */
	return( TCL_ERROR );
    }

    /*
     * For now, zero-out the second reserved field (32-bytes).
     * (As above, it's hard-coded in the lane_arp_pdu_t structure, 
     * so it's hard-coded here.
     */
    for ( i = 0; i < 32; i++ ) {
	cfgptr->reserved2[i] = NULL;
    }

    debug_printf( "LANE LE-ARP packet body loaded.", interp );

    return( TCL_OK );

} /* atmtest_load_lane_arp() */

/* 
 *
 * atmtest_send_arp_pkt()
 *
 * This function causes a LANE Address Resolution Protocol (LE-ARP)
 * Control Frame to be sent over an ATM interface based on the
 * values supplied by the user for the fields of the packet.
 * 
 * Note that it is expected that the proper global variables for
 * the fields of the packet (e.g. slot#, VCD, etc.) have been 
 * set by the user prior to running this command.
 */
int atmtest_send_arp_pkt ( Tcl_Interp *interp )
{
    uchar	dgram_buf[ARP_CTL_PKT_SZ];

    /*
     * If any TCL variables haven't been set at the TCL prompt
     * prior to running this command, then set them to default
     * values.
     * Note that these TCL variables are global.
     */
    (void) atmtest_init_lane_vars( interp );

    /*
     * Load the temporary buffer with the header info, which should
     * be common to all lane control packets.
     */
    if ( atmtest_load_lane_hdr( dgram_buf, interp ) != TCL_OK ) {
	Tcl_AppendResult(interp, "\nError: Cannot load LANE hdr variables.\n", NULL);
	return( TCL_ERROR );
    }

    /*
     * Load the 'LE-ARP Control Packet' specific fields.
     */
    if ( atmtest_load_lane_arp( dgram_buf, interp ) != TCL_OK ) {
	Tcl_AppendResult(interp, "\nError: Cannot load LANE cfg variables.\n", NULL);
	return( TCL_ERROR );
    }

    (void) atmtest_dump_buffer( dgram_buf, CFG_CTL_PKT_SZ, interp ); 

    /*
     * Now send the packet out the specified interface.  Return an error
     * if we can't send the packet for some reason.
     */

    if ( atmtest_send_ctl_pkt( dgram_buf, ARP_CTL_PKT_SZ, interp ) != TCL_OK ) {
	Tcl_AppendResult(interp, "\nError: LANE LE-ARP pkt not sent\n", NULL);	
	return( TCL_ERROR );
    }

    return( TCL_OK );


} /* atmtest_send_arp_pkt() */

/* 
 * 
 * atmtest_load_ip_arp()
 *
 * This function is passed a pointer to the temporary buffer that 
 * it loads with the TCL variables representing the fields of
 * the body portion of an IP ARP packet.
 *
 */
int atmtest_load_ip_arp( uchar *bufptr, Tcl_Interp *interp )
{
    char 		*value;
    int  	     	i, *pi, int_value[LE_CP_VAR_COUNT];
    rfc_arp      	*rfcptr;
    lane_data_hdr_t     *dptr;
    hwaddrtype      	tmp_h_addr, *h_ptr;

    /* Dummy variables so we can use the existing parse routines. */
    parseinfo	     	*c_ptr = NULL;  /* Dummy ptr */
    idbtype		*i_ptr = NULL;  /* Dummy ptr */
    int			hlp_dummy = 0;
    uint		flg_dummy = 0;


    h_ptr = &tmp_h_addr;

    debug_printf( "Loading the IP ARP packet body...", interp );

    /*
     * rfcptr points to the IP ARP packet datagram; 
     * dhptr points to the LANE data header portion of the IP ARP
     * packet.
     */
    dptr = ( lane_data_hdr_t * const ) bufptr;
    rfcptr = ( rfc_arp * const ) &( dptr->ether.data[0] );


    /*
     * Load a chunk of the packet the same way we did some of the
     * fields of the LANE control packets; create an integer array
     * and copy the variables from there.  It's fewer lines of code...
     */
    for ( i = LE_IP_ARP_ARHRD_POS; 
	      i < (LE_IP_ARP_ARHRD_POS + LE_IP_ARP_STUFF_GRP_SZ); i++ ) {

        value = Tcl_GetVar( interp, lane_var_array[i], 0 );

        if ( value == NULL )
	    return( TCL_ERROR );
        else {
	    /* Convert the fields to integers and stuff into the int array */
	    if ( Tcl_GetInt( interp, value, &int_value[i] ) == TCL_ERROR )
                return( TCL_ERROR );
	}
    }

    /* Now stuff the cfg fields of the datagram buffer */
    i = LE_IP_ARP_ARHRD_POS;
    rfcptr->arhrd            = (ushort) int_value[i++];
    rfcptr->arpro            = (ushort) int_value[i++];
    rfcptr->arhln            = (uchar) int_value[i++];
    rfcptr->arpln            = (uchar) int_value[i++];
    rfcptr->opcode           = (ushort) int_value[i++];

    /* 
     * Stuff the ipshdw field source MAC address 
     */ 
    value = Tcl_GetVar( interp, LE_IP_ARP_IPSHDW, 0 );

    if ( value == NULL )
	return( TCL_ERROR );

    /* 
     * match_en_addr expects pi to point to an integer = 0.
     * Note that int_value[0] will be changed by the called procedure.
     */
    int_value[0] = 0; 
    pi = &int_value[0];

    if ( match_en_addr( c_ptr, value, pi, h_ptr, hlp_dummy,
					flg_dummy, i_ptr ) ) {
	bcopy( h_ptr->addr, rfcptr->ipshdw, IEEEBYTES );
    }
    else {   /* Some problem parsing the MAC address */
	return( TCL_ERROR );
    }

    /* 
     * Stuff the ipspro field IP address 
     */ 
    value = Tcl_GetVar( interp, LE_IP_ARP_IPSPRO, 0 );

    if ( value == NULL )
	return( TCL_ERROR );

    if ( !parse_ip_address ( value, &rfcptr->ipspro ) )
	/* Some problem parsing the IP address */
	return( TCL_ERROR );

    /* 
     * Stuff the ipthdw field target MAC address 
     */ 
    value = Tcl_GetVar( interp, LE_IP_ARP_IPTHDW, 0 );

    if ( value == NULL )
	return( TCL_ERROR );

    /* 
     * match_en_addr expects pi to point to an integer = 0.
     * Note that int_value[0] will be changed by the called procedure.
     */
    int_value[0] = 0; 
    pi = &int_value[0];

    if ( match_en_addr( c_ptr, value, pi, h_ptr, hlp_dummy,
					flg_dummy, i_ptr ) ) {
	bcopy( h_ptr->addr, rfcptr->ipthdw, IEEEBYTES );
    }
    else {   /* Some problem parsing the MAC address */
	return( TCL_ERROR );
    }

    /* 
     * Stuff the iptpro field IP address 
     */ 
    value = Tcl_GetVar( interp, LE_IP_ARP_IPTPRO, 0 );

    if ( value == NULL )
	return( TCL_ERROR );

    if ( !parse_ip_address ( value, &rfcptr->iptpro ) )
	/* Some problem parsing the IP address */
	return( TCL_ERROR );


    /*
     * dhptr points to the LANE data header portion of the IP ARP
     * packet;  since it gets stuffed with values that exist as
     * TCL variables (and are contained within the datagram portion
     * of the ARP packet), stuff them here.
     * HACK...
     * What the hell...who needs layering anyway? This is TCL.
     */

    /* Load the LECID portion of the header based on the TCL variable. */
    value = Tcl_GetVar( interp, LE_CP_VAR_REQ_LECID, 0 );
    if ( value == NULL )
	return( TCL_ERROR );
    else {
	if ( Tcl_GetInt( interp, value, &int_value[0] ) == TCL_ERROR )
            return( TCL_ERROR );
    }
    dptr->lec_id = (ushort) int_value[0];

    /*
     * Stuff the type/len field with TYPE_RFC826_ARP (IP ARP).
     */
    dptr->ether.type_or_len = TYPE_RFC826_ARP;

    /*
     * The remaining fields of the ARP packet header are in the body...
     */
    bcopy( rfcptr->ipthdw, dptr->ether.daddr, IEEEBYTES );
    bcopy( rfcptr->ipshdw, dptr->ether.saddr, IEEEBYTES );


    debug_printf( "IP ARP packet body loaded.", interp );

    return( TCL_OK );

} /* atmtest_load_ip_arp() */

/* 
 *
 * atmtest_send_ip_arp_pkt()
 *
 * This function causes an IP Address Resolution Protocol (IP ARP)
 * packet to be sent over an ATM interface based on the
 * values supplied by the user for the fields of the packet.
 * 
 * Note that it is expected that the proper global variables for
 * the fields of the packet (e.g. slot#, VCD, etc.) have been 
 * set by the user prior to running this command.
 */
int atmtest_send_ip_arp_pkt ( Tcl_Interp *interp )
{
    uchar	dgram_buf[IP_LANE_DGRAM_SZ];

    /*
     * If any TCL variables haven't been set at the TCL prompt
     * prior to running this command, then set them to default
     * values.
     * Note that these TCL variables are global.
     */
    (void) atmtest_init_lane_vars( interp );

    /*
     * Load the 'IP ARP' specific fields.
     */
    if ( atmtest_load_ip_arp( dgram_buf, interp ) != TCL_OK ) {
	Tcl_AppendResult(interp, "\nError: Cannot load IP ARP variables.\n", NULL);
	return( TCL_ERROR );
    }

    (void) atmtest_dump_buffer( dgram_buf, CFG_CTL_PKT_SZ, interp ); 

    /*
     * Now send the packet out the specified interface.  Return an error
     * if we can't send the packet for some reason.
     */

    if ( atmtest_send_ctl_pkt( dgram_buf, IP_LANE_DGRAM_SZ, interp ) != TCL_OK ) {
	Tcl_AppendResult(interp, "\nError: LANE IP ARP pkt not sent\n", NULL);	
	return( TCL_ERROR );
    }

    return( TCL_OK );


} /* atmtest_send_ip_arp_pkt() */

/* 
 * 
 * atmtest_dump_lsend_arglist()
 *
 * Dump out a list of the valid control packet types that can be
 * entered as arguments to the TCL lsend command.
 */
void atmtest_dump_lsend_arglist()
{
    printf( "\n" );

    printf( "  Syntax: lsend [msgtype] [vcd] <CR>\n\n" );

    printf( "  Valid 'msgtype' arguments:\n" );
    printf( "    config_req       Configuration Request  (opcode = 0x0001)\n" );
    printf( "    config_resp      Configuration Response (opcode = 0x0101)\n" );
    printf( "    join_req         Join Request           (opcode = 0x0002)\n" );
    printf( "    join_resp        Join Response          (opcode = 0x0102)\n" );
    printf( "    le_arp_req       LE-ARP Request         (opcode = 0x0006)\n" );
    printf( "    le_arp_resp      LE-ARP Response        (opcode = 0x0106)\n" );
    printf( "    ip_arp           IP ARP                 (opcode = N/A)\n" );
    printf( "    <CR>             Help\n" );
    printf( "\n" );

    printf( "  Use 'sh_vars' to examine the fields of the control packets.\n\n" );

} /* atmtest_dump_lsend_arglist() */


/* 
 * 
 * atmtest_print_help_string()
 *
 * Print out a help string for the string passed in, which was supplied
 * by the user via the TCL 'help' command.  If no help is available,
 * tell the user.
 *
 * Note that this function doesn't really belong in this file, but 
 * since the only help available currently is related to the LANE 
 * testing software, it's here for now.
 */
void atmtest_print_help_string( char *str )
{
    boolean help_available = FALSE;
   
    if ( Tcl_StringMatch( str, "lane" ) || 
	 Tcl_StringMatch( str, "lsend" ) ) {
	printf( lane_help_str );
	help_available = TRUE;
    }
    
    if ( Tcl_StringMatch( str, "atmsig" ) ) {
	printf( atmsig_help_str );
	help_available = TRUE;
    }
    
    if ( !help_available )
	printf( "No help available for %s.\n", str );


} /* atmtest_print_help_string() */

/* 
 * 
 * atmtest_print_gen_help()
 *
 * Print out a top-level help string.
 *
 */
void atmtest_print_gen_help()
{
    printf( general_atmtest_help_msg );

} /* atmtest_print_gen_help() */

/* 
 * 
 * atmtest_get_atm_idb()
 *
 * This function is simply a front-end to the atmtest_get_atm_interface()
 * function that uses the TCL slot, port, and subinterface variables
 * to find the idb so that they don't need to be recomputed each time
 * we need to get the idb.
 *
 */
idbtype *atmtest_get_atm_idb( Tcl_Interp *interp )
{
    char        *value;
    uint        slot_num, port_num, subif;
    int         i_value;


    /* Get the slot # */
    value = Tcl_GetVar( interp, LE_VAR_SLOT, 0 );

    if ( value == NULL ) {
	return( NULL );
    }

    if ( Tcl_GetInt( interp, value, &i_value ) == TCL_ERROR ) {
	return( NULL );
    }
    slot_num = (uint) i_value;   


    /* Get the port # */
    value = Tcl_GetVar( interp, LE_VAR_PORT, 0 );

    if ( value == NULL ) {
	return( NULL );
    }

    if ( Tcl_GetInt( interp, value, &i_value ) == TCL_ERROR ) {
	return( NULL );
    }
    port_num = (uint) i_value;   


    /* Get the subinterface # */
    value = Tcl_GetVar( interp, LE_VAR_SUBINTF, 0 );

    if ( value == NULL ) {
	return( NULL );
    }

    if ( Tcl_GetInt( interp, value, &i_value ) == TCL_ERROR ) {
	return( NULL );
    }
    subif = (uint) i_value;   


    return( atmtest_get_atm_interface( slot_num, port_num, subif, interp ) ); 

} /* atmtest_get_atm_idb() */
