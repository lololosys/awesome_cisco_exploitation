/* $Id: atmtest_sig_support.c,v 3.3.56.2 1996/05/09 14:11:54 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/atm-test/atmtest_sig_support.c,v $
 *------------------------------------------------------------------
 * atmtest_sig_support.c -- Support code for ATM Signalling related 
 * TCL commands.
 * [Was ../tcl/tcl_atmsig_support.c]
 *
 * April, 1995  Chris Parrott
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmtest_sig_support.c,v $
 * Revision 3.3.56.2  1996/05/09  14:11:54  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.3.56.1.18.1  1996/04/27  06:34:15  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.3.56.1.8.1  1996/04/08  01:44:45  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.3.56.1  1996/03/18  19:00:47  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.60.1  1996/03/22  09:35:10  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.3.48.1  1996/03/05  05:49:02  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.3.36.1  1996/02/27  20:40:43  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.3.14.1  1996/01/11  01:44:43  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.10.1  1995/12/29  01:05:33  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.3  1995/11/17  08:47:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:07:31  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:03:08  shaker
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
#include "../h/address.h"


#include "../if/atm.h"
#include "../atm/atmsig_api.h"
#include "../atm/atmsig_private.h"
#include "../lane/lane.h"

#include "../atm-test/atmtest_lane_cmds.h"
#include "../atm-test/atmtest_sig_cmds.h"
#include "../atm-test/atmtest_sig_vars.h"
#include "../atm-test/atmtest_lane_vars.h"

/* #include "../if/arp.h" */

/* #include "../atm-test/atmtest_hlp_strings.h" */

/*
 * The infamous, evil global interp variable...
 */
static Tcl_Interp *g_interp;

/*
 * Variables that are global to the ATMSIG functions
 */

/*
 * Internal (as opposed to being a TCL variable) static 
 * call_id and vcd arrays.
 */
static atmSvcType *call_id_array[LANE_MAX_VC_TYPE + 1];
static int vcd_array[LANE_MAX_VC_TYPE + 1];

/*
 * We need a two-dimensional array for the endpoint_id's since
 * there can be multiple endpoint_id's per call_id.
 */
static atmSvcPartyType 
		*endpoint_id_array[LANE_MAX_VC_TYPE + 1][ATMTEST_MAX_MPT_ENDPTS];

/*
 * Messages printed when atmsig messages are received.
 */
static char *rvc_setup_str          = "\n***Received SETUP message***\n";
static char *rvc_connect_str        = "\n***Received CONNECT message***\n";
static char *rvc_connect_ack_str    = "\n***Received CONNECT ACK message***\n";
static char *rvc_release_str        = "\n***Received RELEASE message***\n";
static char *rvc_release_comp_str   = "\n***Received RELEASE COMPLETE message***\n";
static char *rvc_add_party_str      = "\n***Received ADD PARTY message***\n";
static char *rvc_add_party_ack_str  = "\n***Received ADD PARTY ACK message***\n";
static char *rvc_add_party_rej_str  = "\n***Received ADD PARTY REJECT message***\n";
static char *rvc_drop_party_str     = "\n***Received DROP PARTY message***\n";
static char *rvc_drop_party_ack_str = "\n***Received DROP PARTY ACK message***\n";


/* 
 * 
 * atmtest_set_g_interp()
 *
 * This function sets the global g_interp to the value passed in.
 *
 */
void atmtest_set_g_interp( Tcl_Interp *interp )
{
    g_interp = interp;

    debug_printf( "Setting the global 'interp' variable\n", interp );

} /* atmtest_set_g_interp() */

/* 
 * 
 * atmtest_get_g_interp()
 *
 * This function returns the value of the global g_interp.
 *
 */
Tcl_Interp *atmtest_get_g_interp()
{
    return( g_interp );

} /* atmtest_get_g_interp() */

/* 
 * 
 * atmtest_get_call_id()
 *
 * This function returns a call_id associated with the particular
 * LANE VCC type passed in.
 *
 */
atmSvcType *atmtest_get_call_id( int vc_type )
{

    if ( vc_type > LANE_MAX_VC_TYPE )
	return( NULL );

    else
	return( call_id_array[vc_type] );

} /* atmtest_get_call_id() */

/* 
 * 
 * atmtest_set_call_id()
 *
 * This function sets the call_id associated with the particular
 * LANE VCC type passed in to the call_id value passed in.
 *
 */
void atmtest_set_call_id( int vc_type, atmSvcType *c_id )
{

    if ( vc_type > LANE_MAX_VC_TYPE )
	return;  /* It's bogus, but at least we won't crash... */

    else
	call_id_array[vc_type] = c_id;

} /* atmtest_set_call_id */

/* 
 * 
 * atmtest_get_vc_type()
 *
 * This function returns the integer constant defined for the 
 * PVC type string passed in.
 *
 */
int atmtest_get_vc_type( char *vc_type )
{

    if ( strcmp( vc_type, LANE_LEC_DIRECT_STR ) == 0 )
	return( LANE_LEC_DIRECT );
    if ( strcmp( vc_type, LANE_LEC_DISTRIBUTE_STR ) == 0 )
	return( LANE_LEC_DISTRIBUTE );
    if ( strcmp( vc_type, LANE_LEC_CONFIGURE_STR ) == 0 )
	return( LANE_LEC_CONFIGURE );
    if ( strcmp( vc_type, LANE_LEC_SEND_STR ) == 0 )
	return( LANE_LEC_SEND );
    if ( strcmp( vc_type, LANE_LEC_FORWARD_STR ) == 0 )
	return( LANE_LEC_FORWARD );
    if ( strcmp( vc_type, LANE_LEC_DATA_STR ) == 0 )
	return( LANE_LEC_DATA );
    if ( strcmp( vc_type, LANE_LES_DIRECT_STR ) == 0 )
	return( LANE_LES_DIRECT );
    if ( strcmp( vc_type, LANE_LES_DISTRIBUTE_STR ) == 0 )
	return( LANE_LES_DISTRIBUTE );
    if ( strcmp( vc_type, LANE_BUS_SEND_STR ) == 0 )
	return( LANE_BUS_SEND );
    if ( strcmp( vc_type, LANE_BUS_FORWARD_STR ) == 0 )
	return( LANE_BUS_FORWARD );
    if ( strcmp( vc_type, LANE_CONFIGURE_STR ) == 0 )
	return( LANE_CONFIGURE );

    return( NULL );

} /* atmtest_get_vc_type() */

/* 
 * 
 * atmtest_get_vc_type_str()
 *
 * This function returns the LANE PVC type string defined for the 
 * user_vc_type_t constant vc type passed in.
 *
 */
char *atmtest_get_vc_type_str( user_vc_type_t vc_type )
{

    switch( vc_type ) {

        case LANE_LEC_DIRECT:
	    return( LANE_LEC_DIRECT_STR );
	    break;
        case LANE_LEC_DISTRIBUTE:
	    return( LANE_LEC_DISTRIBUTE_STR );
	    break;
        case LANE_LEC_CONFIGURE:
	    return( LANE_LEC_CONFIGURE_STR );
	    break;
        case LANE_LEC_SEND:
	    return( LANE_LEC_SEND_STR );
	    break;
        case LANE_LEC_FORWARD:
	    return( LANE_LEC_FORWARD_STR );
	    break;
        case LANE_LEC_DATA:
	    return( LANE_LEC_DATA_STR );
	    break;
        case LANE_LES_DIRECT:
	    return( LANE_LES_DIRECT_STR );
	    break;
        case LANE_LES_DISTRIBUTE:
	    return( LANE_LES_DISTRIBUTE_STR );
	    break;
        case LANE_BUS_SEND:
	    return( LANE_BUS_SEND_STR );
	    break;
        case LANE_BUS_FORWARD:
	    return( LANE_BUS_FORWARD_STR );
	    break;
        case LANE_CONFIGURE:
	    return( LANE_CONFIGURE_STR );
	    break;

        default:
	    buginf( "Error: cannot convert integer vc type.\n" );
	    return( NULL );
    }

} /* atmtest_get_vc_type_str() */

/* 
 * 
 * atmtest_store_call_id()
 *
 * This function stores the passed call_id into the call_id
 * array at the location specified by the current_vc global.
 *
 */
int atmtest_store_call_id( atmSvcType *c_id, Tcl_Interp *interp )
{
    char *cur_vc;


    /*
     * If the call_id stored in the call_id array matches 
     * the new one passed in, output a warning and return.
     */
    if ( atmtest_retrieve_call_id( interp ) == c_id ) {
	buginf( "Warning: call_id unchanged\n" );
	return( TCL_OK );
    }

    /*
     * Get the value of the current_vc global
     */
    cur_vc = Tcl_GetVar( interp, LANE_CURRENT_VC, 0 );
    if ( cur_vc == NULL )
	return( NULL );

    atmtest_set_call_id( atmtest_get_vc_type( cur_vc ), c_id ); 

    /*
     * Dump the updated call_id array if debugging is on.
     */
    atmtest_dump_call_id_array( interp );

    return( TCL_OK );

} /* atmtest_store_call_id() */

/* 
 * 
 * atmtest_retrieve_call_id()
 *
 * This function returns the call_id stored in the call_id array
 * at the location corresponding to the global current_vc.
 *
 */
atmSvcType *atmtest_retrieve_call_id( Tcl_Interp *interp )
{
    char *cur_vc;
    int  vc_type;

    if ( !interp ) {
        buginf( "Error: Cannot retrieve call_id: null interp\n" );
	return( NULL );
    }

    /*
     * Get the value of the current_vc global
     */
    cur_vc = Tcl_GetVar( interp, LANE_CURRENT_VC, 0 );
    if ( cur_vc == NULL )
	return( NULL );

    vc_type = atmtest_get_vc_type( cur_vc );
    if ( vc_type == NULL )
	return( NULL );

    return ( atmtest_get_call_id( vc_type ) );

} /* atmtest_retrieve_call_id() */

/* 
 * 
 * atmtest_dump_call_id_array()
 *
 * This function dumps the call_id array.
 *
 */
void atmtest_dump_call_id_array( Tcl_Interp *interp )
{

    if ( !atmtest_lane_debug_on( interp ) ) {
	return;
    }

    buginf( "\nContents of the call_id array:\n\n" );


    buginf( "call_id[lec_direct] = %08x\n", 
				atmtest_get_call_id( LANE_LEC_DIRECT ) );
    buginf( "call_id[lec_distribute] = %08x\n", 
				atmtest_get_call_id( LANE_LEC_DISTRIBUTE ) );
    buginf( "call_id[lec_configure] = %08x\n", 
				atmtest_get_call_id( LANE_LEC_CONFIGURE ) );
    buginf( "call_id[lec_send] = %08x\n", 
				atmtest_get_call_id( LANE_LEC_SEND ) );
    buginf( "call_id[lec_forward] = %08x\n", 
				atmtest_get_call_id( LANE_LEC_FORWARD ) );
    buginf( "call_id[lec_data] = %08x\n", 
				atmtest_get_call_id( LANE_LEC_DATA ) );
    buginf( "call_id[les_direct] = %08x\n", 
				atmtest_get_call_id( LANE_LES_DIRECT ) );
    buginf( "call_id[les_distribute] = %08x\n", 
				atmtest_get_call_id( LANE_LES_DISTRIBUTE ) );
    buginf( "call_id[bus_send] = %08x\n", 
				atmtest_get_call_id( LANE_BUS_SEND ) );
    buginf( "call_id[bus_forward] = %08x\n", 
				atmtest_get_call_id( LANE_BUS_FORWARD ) );
    buginf( "call_id[configure] = %08x\n", 
				atmtest_get_call_id( LANE_CONFIGURE ) );

    buginf( "\n\n" );

} /* atmtest_dump_call_id_array() */

/* 
 * 
 * atmtest_get_endpoint_id()
 *
 * This function returns a endpoint_id associated with the particular
 * LANE VCC type/endpoint_id index passed in.
 *
 */
atmSvcPartyType *atmtest_get_endpoint_id( int vc_type, int endpoint_id_idx )
{

    if ( ( vc_type > LANE_MAX_VC_TYPE ) || 
	 ( endpoint_id_idx > ATMTEST_MAX_MPT_ENDPTS ) )
	return( NULL );

    else
	return( endpoint_id_array[vc_type][endpoint_id_idx] );

} /* atmtest_get_endpoint_id() */

/* 
 * 
 * atmtest_set_endpoint_id()
 *
 * This function sets the endpoint_id associated with the particular
 * LANE VCC type/endpoint_id index passed in to the endpoint_id value 
 * passed in.
 *
 */
void atmtest_set_endpoint_id( int vc_type, int endpoint_id_idx, atmSvcPartyType *c_id )
{

    if ( ( vc_type > LANE_MAX_VC_TYPE ) ||
	 ( endpoint_id_idx > ATMTEST_MAX_MPT_ENDPTS ) )
	return;  /* It's bogus, but at least we won't crash... */

    else
	endpoint_id_array[vc_type][endpoint_id_idx] = c_id;

} /* atmtest_set_endpoint_id */

/* 
 * 
 * atmtest_store_endpoint_id()
 *
 * This function stores the passed endpoint_id into the endpoint_id
 * array at the location specified by the current_vc and 
 * current_endpoint_id globals.
 *
 */
int atmtest_store_endpoint_id( atmSvcPartyType *c_id, Tcl_Interp *interp )
{
    char *cur_vc, *value;
    int	 cur_endpoint_id;


    /*
     * If the endpoint_id stored in the endpoint_id array matches 
     * the new one passed in, output a warning and return.
     */
    if ( atmtest_retrieve_endpoint_id( interp ) == c_id ) {
	buginf( "Warning: endpoint_id unchanged\n" );
	return( TCL_OK );
    }

    /*
     * Get the value of the current_vc global
     */
    cur_vc = Tcl_GetVar( interp, LANE_CURRENT_VC, 0 );
    if ( cur_vc == NULL )
	return( NULL );

    /*
     * Get the value of the current_endpoint_id global.  Note that
     * it's an integer.
     */
    value = Tcl_GetVar( interp, CURRENT_ENDPOINT_ID, 0 );
    if ( value == NULL )
	return( NULL );
    if ( ( Tcl_GetInt( interp, value, &cur_endpoint_id ) ) == TCL_ERROR ) {
	buginf( "Error: Cannot convert %s to an integer.\n", CURRENT_ENDPOINT_ID );
        return( NULL );
    }

    atmtest_set_endpoint_id( atmtest_get_vc_type( cur_vc ), cur_endpoint_id, c_id ); 

    /*
     * Dump the updated endpoint_id array if debugging is on.
     */
    atmtest_dump_endpoint_id_array( interp );

    return( TCL_OK );

} /* atmtest_store_endpoint_id() */

/* 
 * 
 * atmtest_retrieve_endpoint_id()
 *
 * This function returns the endpoint_id stored in the endpoint_id array
 * at the location corresponding to the globals current_vc and 
 * current_endpoint_vc.
 *
 */
atmSvcPartyType *atmtest_retrieve_endpoint_id( Tcl_Interp *interp )
{
    char *cur_vc, *value;
    int  vc_type, endpoint_id_idx;

    if ( !interp ) {
        buginf( "Error: Cannot retrieve endpoint_id: null interp\n" );
	return( NULL );
    }

    /*
     * Get the value of the current_vc global
     */
    cur_vc = Tcl_GetVar( interp, LANE_CURRENT_VC, 0 );
    if ( cur_vc == NULL )
	return( NULL );

    /*
     * Get the value of the current_endpoint_id global.  Note that
     * it's an integer.
     */
    value = Tcl_GetVar( interp, CURRENT_ENDPOINT_ID, 0 );
    if ( value == NULL )
	return( NULL );
    if ( ( Tcl_GetInt( interp, value, &endpoint_id_idx ) ) == TCL_ERROR ) {
	buginf( "Error: Cannot convert %s to an integer.\n", CURRENT_ENDPOINT_ID );
        return( NULL );
    }

    vc_type = atmtest_get_vc_type( cur_vc );
    if ( vc_type == NULL )
	return( NULL );

    return ( atmtest_get_endpoint_id( vc_type, endpoint_id_idx ) );

} /* atmtest_retrieve_endpoint_id() */

/* 
 * 
 * atmtest_dump_endpoint_id_array()
 *
 * This function dumps the endpoint_id array.
 *
 */
void atmtest_dump_endpoint_id_array( Tcl_Interp *interp )
{
    int i, j;
    atmSvcPartyType *endpoint_id;


    if ( !atmtest_lane_debug_on( interp ) ) {
	return;
    }

    buginf( "\nContents of the endpoint_id array:\n\n" );

    for ( i = LANE_LEC_DIRECT; i <= LANE_MAX_VC_TYPE; i++ ) {
	for ( j = 0; j < ATMTEST_MAX_MPT_ENDPTS; j++ ) {
	    endpoint_id = atmtest_get_endpoint_id( i, j ); 
	    if ( endpoint_id )
		buginf( "endpoint_id[%s][%d] = %08x\n",
			atmtest_get_vc_type_str( (user_vc_type_t)i ), j, endpoint_id ); 
        }
    }

    buginf( "\n\n" );

} /* atmtest_dump_endpoint_id_array() */

/* 
 * 
 * atmtest_dump_vcd_array()
 *
 * This function dumps the vcd array.
 *
 */
void atmtest_dump_vcd_array( Tcl_Interp *interp )
{

    if ( !atmtest_lane_debug_on( interp ) ) {
	return;
    }

    buginf( "\nContents of the vcd array:\n\n" );


    buginf( "vcd_array[lec_direct] = %d\n", 
				atmtest_get_vcd( LANE_LEC_DIRECT ) );
    buginf( "vcd_array[lec_distribute] = %d\n", 
				atmtest_get_vcd( LANE_LEC_DISTRIBUTE ) );
    buginf( "vcd_array[lec_configure] = %d\n", 
				atmtest_get_vcd( LANE_LEC_CONFIGURE ) );
    buginf( "vcd_array[lec_send] = %d\n", 
				atmtest_get_vcd( LANE_LEC_SEND ) );
    buginf( "vcd_array[lec_forward] = %d\n", 
				atmtest_get_vcd( LANE_LEC_FORWARD ) );
    buginf( "vcd_array[lec_data] = %d\n", 
				atmtest_get_vcd( LANE_LEC_DATA ) );
    buginf( "vcd_array[les_direct] = %d\n", 
				atmtest_get_vcd( LANE_LES_DIRECT ) );
    buginf( "vcd_array[les_distribute] = %d\n", 
				atmtest_get_vcd( LANE_LES_DISTRIBUTE ) );
    buginf( "vcd_array[bus_send] = %d\n", 
				atmtest_get_vcd( LANE_BUS_SEND ) );
    buginf( "vcd_array[bus_forward] = %d\n", 
				atmtest_get_vcd( LANE_BUS_FORWARD ) );
    buginf( "vcd_array[configure] = %d\n", 
				atmtest_get_vcd( LANE_CONFIGURE ) );

    buginf( "\n\n" );

} /* atmtest_dump_vcd_array() */

/* 
 * 
 * atmtest_get_vcd()
 *
 * This function returns a vcd associated with the particular
 * LANE VCC type passed in. 
 *
 */
ushort atmtest_get_vcd( int vc_type )
{

    if ( vc_type > LANE_MAX_VC_TYPE )
	return( NULL );

    else
	return( ( ushort ) vcd_array[vc_type] );

} /* atmtest_get_vcd() */

/* 
 * 
 * atmtest_set_vcd()
 *
 * This function sets the vcd associated with the particular
 * LANE VCC type passed in to the vcd value passed in.
 *
 */
void atmtest_set_vcd( int vc_type, ushort vcd )
{

    if ( vc_type > LANE_MAX_VC_TYPE )
	return;  /* It's bogus, but at least we won't crash... */

    else
	vcd_array[vc_type] = (int) vcd;

} /* atmtest_set_vcd */

/* 
 * 
 * atmtest_store_vcd()
 *
 * This function stores the passed in vcd into the global vcd array
 * at the location specified by the global current_vc.
 */
int atmtest_store_vcd( ushort vcd, Tcl_Interp *interp )
{
    char *cur_vc;

    /*
     * Get the value of the current_vc global
     */
    cur_vc = Tcl_GetVar( interp, LANE_CURRENT_VC, 0 );
    if ( cur_vc == NULL ) {
	buginf( "\nError: atmtest_store_vcd: Cannot get current_vc.\n" );
	return( TCL_ERROR );
    }

    atmtest_set_vcd( atmtest_get_vc_type( cur_vc ), vcd );

    /*
     * Dump the updated vcd array if debugging is on.
     */
    /* atmtest_dump_vcd_array( interp ); */

    return( TCL_OK );

} /* atmtest_store_vcd() */

/* 
 * 
 * atmtest_stuff_blli_fields()
 *
 * This function stuffs the BLLI fields pointed to by the blli_ptr
 * passed in with the appropriate TCL variables defined for BLLI.
 *
 */
int atmtest_stuff_blli_fields( blli_type *blli_ptr, Tcl_Interp *interp )
{
    char *value;
    int  i, j;
    blli_type *ptr;

    /*
     * First clear all the BLLI fields in the message
     */
    for ( i = 0; i < ATM_MAX_BLLI_NUM; i++ ) {
	ptr = blli_ptr + i;
	ptr->length = 0;
     
        for ( j = 0; j < ATM_BBAND_LOLAYER_MAXSIZE; j++ ) {
            ptr->string[j] = 0;	    
        }
    }

    /* Stuff the blli[0] length and string fields. */
    value = Tcl_GetVar( interp, ATM_SIG_BLLI_LENGTH, 0 );
    if ( value == NULL ) {
	buginf( "Error: Cannot get value of %s.\n", ATM_SIG_BLLI_LENGTH );
        return( TCL_ERROR );
    }
    if ( Tcl_GetInt( interp, value, &(blli_ptr->length) ) == TCL_ERROR ) {
	buginf( "Error: Cannot convert %s to an integer.\n", ATM_SIG_BLLI_LENGTH );
        return( TCL_ERROR );
    }

    value = Tcl_GetVar( interp, ATM_SIG_BLLI_STRING, 0 );
    if ( value == NULL ) {
	buginf( "Error: Cannot get value of %s.\n", ATM_SIG_BLLI_STRING );
        return( TCL_ERROR );
    }
    if ( atmtest_convert_str( value, &(blli_ptr->string[0]), 
					blli_ptr->length, interp ) != TCL_OK )
        buginf( "Error: Invalid BLLI string %s\n", ATM_SIG_BLLI_STRING );
    

    /* Stuff the blli[1] length and string fields. */
    blli_ptr++;
    value = Tcl_GetVar( interp, ATM_SIG_BLLI_LENGTH_2, 0 );
    if ( value == NULL ) {
	buginf( "Error: Cannot get value of %s.\n", ATM_SIG_BLLI_LENGTH_2 );
        return( TCL_ERROR );
    }
    if ( Tcl_GetInt( interp, value, &(blli_ptr->length) ) == TCL_ERROR ) {
	buginf( "Error: Cannot convert %s to an integer.\n", ATM_SIG_BLLI_LENGTH_2 );
        return( TCL_ERROR );
    }

    value = Tcl_GetVar( interp, ATM_SIG_BLLI_STRING_2, 0 );
    if ( value == NULL ) {
	buginf( "Error: Cannot get value of %s.\n", ATM_SIG_BLLI_STRING_2 );
        return( TCL_ERROR );
    }
    if ( atmtest_convert_str( value, &(blli_ptr->string[0]), 
					blli_ptr->length, interp ) != TCL_OK )
        buginf( "Error: Invalid BLLI string %s\n", ATM_SIG_BLLI_STRING );
    


    /* Stuff the blli[2] length and string fields. */
    blli_ptr++;
    value = Tcl_GetVar( interp, ATM_SIG_BLLI_LENGTH_3, 0 );
    if ( value == NULL ) {
	buginf( "Error: Cannot get value of %s.\n", ATM_SIG_BLLI_LENGTH_3 );
        return( TCL_ERROR );
    }
    if ( Tcl_GetInt( interp, value, &(blli_ptr->length) ) == TCL_ERROR ) {
	buginf( "Error: Cannot convert %s to an integer.\n", ATM_SIG_BLLI_LENGTH_3 );
        return( TCL_ERROR );
    }

    value = Tcl_GetVar( interp, ATM_SIG_BLLI_STRING_3, 0 );
    if ( value == NULL ) {
	buginf( "Error: Cannot get value of %s.\n", ATM_SIG_BLLI_STRING_3 );
        return( TCL_ERROR );
    }
    if ( atmtest_convert_str( value, &(blli_ptr->string[0]), 
					blli_ptr->length, interp ) != TCL_OK )
        buginf( "Error: Invalid BLLI string %s\n", ATM_SIG_BLLI_STRING );

    return( TCL_OK );

} /* atmtest_stuff_blli_fields() */

/* 
 * 
 * atmtest_convert_str()
 *
 * This function takes the ascii string passed in and converts it char
 * by char to an array of uchars, passed back in buf.  
 *
 */
int atmtest_convert_str( char *str, uchar *buf, int length, Tcl_Interp *interp )
{
    int 	int_value, i;
    char 	str_buf[5], *str_buf_ptr;


   
    /*
     * If the first two bytes of the string are '0x', ignore them.
     * If not, assume a string of hex bytes.
     */
    if ( ( *str == '0' ) && ( *( str + 1 ) == 'x' ) ) {
	    str += 2;
    }

    for ( i = 0; i < length; i++ ) {

        /* 
	 * Copy two bytes at a time into str_buf.  
	 */
        str_buf_ptr = str_buf;
        *str_buf_ptr++ = '0';
        *str_buf_ptr++ = 'x';

	/*
	 * If we encounter a NULL character at this point, bail with 
	 * an error message.
	 */
        if ( *str == '\0' ) {
	    buginf( "Error: BLLI string shorter than specified length\n" );
	    return( TCL_ERROR );
        }

	/*
	 * Copy two characters from str into str_buf.
	 */
	*str_buf_ptr++ = *str++;
	*str_buf_ptr = *str++;

	/*
	 * If the last character copied in wasn't NULL, we
	 * still need to append NULL to str_buf.
	 */
        if ( *str_buf_ptr != '\0' ) {
	    *(++str_buf_ptr) = '\0';
        }

	/*
	 * Convert the str_buf hex character to an integer.
	 */
	if ( Tcl_GetInt( interp, str_buf, &int_value ) != TCL_OK ) {
	    return( TCL_ERROR );
        }

        else 
	    buf[i] = ( uchar )int_value;
    }

    return( TCL_OK );

} /* atmtest_convert_str() */

/* 
 * 
 * atmtest_link_lane_vcd_array()
 *
 * This function is called used to link all the elements of the 
 * internal vcd array to the elements of the global TCL array 
 * lane_vcd_array.  Why two arrays?  This is an interim solution...we 
 * should be able to have a single TCL array, but sprintf() didn't work
 * in converting the VCD to a string, so this will have to do for now.
 *
 * We need to be able to retrieve the VCD for particular types of
 * VCs in order to pass VCDs as arguments to the 'lsend' commands.
 * With the VCDs stored in a TCL array, we can do stuff like
 *     lsend $lane_vcd_array(lec_configure)
 */
void atmtest_link_lane_vcd_array( Tcl_Interp *interp )
{

    Tcl_LinkVar( interp, VCD_ARRAY_LEC_DIRECT_STR, 
		( char * ) &vcd_array[LANE_LEC_DIRECT], TCL_LINK_INT );    
    Tcl_LinkVar( interp, VCD_ARRAY_LEC_DISTRIBUTE_STR, 
		( char * ) &vcd_array[LANE_LEC_DISTRIBUTE], TCL_LINK_INT );    
    Tcl_LinkVar( interp, VCD_ARRAY_LEC_CONFIGURE_STR, 
		( char * ) &vcd_array[LANE_LEC_CONFIGURE], TCL_LINK_INT );    
    Tcl_LinkVar( interp, VCD_ARRAY_LEC_SEND_STR, 
		( char * ) &vcd_array[LANE_LEC_SEND], TCL_LINK_INT );    
    Tcl_LinkVar( interp, VCD_ARRAY_LEC_FORWARD_STR, 
		( char * ) &vcd_array[LANE_LEC_FORWARD], TCL_LINK_INT );    
    Tcl_LinkVar( interp, VCD_ARRAY_LEC_DATA_STR, 
		( char * ) &vcd_array[LANE_LEC_DATA], TCL_LINK_INT );    
    Tcl_LinkVar( interp, VCD_ARRAY_LES_DIRECT_STR, 
		( char * ) &vcd_array[LANE_LES_DIRECT], TCL_LINK_INT );    
    Tcl_LinkVar( interp, VCD_ARRAY_LES_DISTRIBUTE_STR, 
		( char * ) &vcd_array[LANE_LES_DISTRIBUTE], TCL_LINK_INT );    
    Tcl_LinkVar( interp, VCD_ARRAY_BUS_SEND_STR, 
		( char * ) &vcd_array[LANE_BUS_SEND], TCL_LINK_INT );    
    Tcl_LinkVar( interp, VCD_ARRAY_BUS_FORWARD_STR, 
		( char * ) &vcd_array[LANE_BUS_FORWARD], TCL_LINK_INT );    
    Tcl_LinkVar( interp, VCD_ARRAY_CONFIGURE_STR, 
		( char * ) &vcd_array[LANE_CONFIGURE], TCL_LINK_INT );    

} /* atmtest_link_lane_vcd_array() */

/* 
 * 
 * atmtest_clear_array_entries()
 *
 * This function first searches through the call_id array looking for
 * a match to the call_id passed in; if it finds a match, it clears the 
 * call_id entry, and the corresponding vcd entry in the vcd array.
 */
void atmtest_clear_array_entries( atmSvcType *c_id, Tcl_Interp *interp )
{
    int i;

    /*
     * Determine the index corresponding to the call_id passed in.
     */
    for ( i = LANE_LEC_DIRECT; i <= LANE_CONFIGURE; i++ ) {
	
        if ( atmtest_get_call_id( i ) == c_id ) {
            atmtest_set_call_id( i, NULL );
	    atmtest_set_vcd( i, NULL );
	    if ( atmtest_lane_debug_on( interp ) )
	        buginf( "%sCleared %s entries in the call_id and vcd arrays.\n",
		        ATMTEST_LANE_DEBUG_LEADER, 
			atmtest_get_vc_type_str( ( user_vc_type_t ) i ) );
        }
    }

} /* atmtest_clear_array_entries() */

/*================================================================*/
/* 
 * 
 * atmtest_sig_callback_func()
 *
 * This is the TCL callback function used to handle events
 * reported to TCL by the signalling driver on the router.  A 
 * pointer to this function is passed to the sigAPI as part of the
 * registration of the TCL 'called' and/or 'calling' parties.
 *
 */
void atmtest_sig_callback_func( void *ptr )
{

    sig_api_msg *sig_msg;
    sig_api_connect *connect;
    sig_api_connect_ack *connect_ack;
    sig_api_setup *setup;
    sig_api_release_comp *release_complete;
    sig_api_add_party *add_party;
    sig_api_add_party_ack *add_party_ack;
    sig_api_add_party_rej *add_party_rej;
    sig_api_drop_party *drop_party;
    sig_api_drop_party_ack *drop_party_ack;
    Tcl_Interp *c_interp;


    /*
     * KLUDGE ALERT!!
     * We shouldn't be using the a global interp here...there should
     * be a way to get the interp using TCL, but alas, this callback
     * routine has created great headaches.  If someone figures out a
     * way to properly link the interp into this callback routine, be
     * my guest - until then...
     */
    c_interp = atmtest_get_g_interp();
    if ( !c_interp ) {
	buginf( "\ncallback: error: cannot service message - null interp\n" );
	return;
    }

    sig_msg = (sig_api_msg *)ptr;

    switch (sig_msg->opcode) {

       case ATM_SETUP:
          setup = &sig_msg->u.setup;

	  /* Dump the buffer to debug */
	  if ( c_interp )
	      atmtest_dump_buffer( (uchar *)setup, sizeof( sig_api_setup ), c_interp );
          else
	      buginf( "\nATM_SETUP: error: Cannot call atmtest_dump_buffer; null interp\n" );

          atmtest_process_sig_setup( setup, c_interp );
          break;

       case ATM_CONNECT:
          connect = &sig_msg->u.connect;
          atmtest_process_sig_connect( connect, c_interp );
          break;

       case ATM_CONNECT_ACK:
          connect_ack = &sig_msg->u.connect_ack; 
          atmtest_process_sig_connect_ack( connect_ack, c_interp ); 
          break;

       case ATM_RELEASE:
          atmtest_process_sig_release( sig_msg, c_interp ); 
          break;

       case ATM_RELEASE_COMPLETE:
          release_complete = &sig_msg->u.release_comp;
          atmtest_process_sig_release_comp( release_complete, c_interp );
          break;

       case ATM_ADD_PARTY:
          add_party = &sig_msg->u.add_party;
          atmtest_process_sig_add_party( add_party, c_interp );
          break;

       case ATM_ADD_PARTY_ACK:
          add_party_ack = &sig_msg->u.add_party_ack;
          atmtest_process_sig_add_party_ack( add_party_ack, c_interp );
          break;

       case ATM_ADD_PARTY_REJ:
          add_party_rej = &sig_msg->u.add_party_rej;
          atmtest_process_sig_add_party_rej( add_party_rej, c_interp );
          break;

       case ATM_DROP_PARTY:
          drop_party = &sig_msg->u.drop_party;
          atmtest_process_sig_drop_party( drop_party, c_interp );
          break;

       case ATM_DROP_PARTY_ACK:
          drop_party_ack = &sig_msg->u.drop_party_ack;
          atmtest_process_sig_drop_party_ack( drop_party_ack, c_interp );
          break;

       default:
          break;

    }

} /* atmtest_sig_callback_func() */

/* 
 * 
 * atmtest_process_sig_setup()
 *
 * This function is called by the atmtest_sig_callback_func() when
 * a SETUP message is received for the local NSAP.
 *
 */
void atmtest_process_sig_setup( sig_api_setup *setup, Tcl_Interp *interp )
{

   buginf( rvc_setup_str );

   if ( atmtest_lane_debug_on( interp ) )
       buginf( "\nVCD %d received in the SETUP message\n", setup->vcd );

   /*
    * Store the call_id and the vcd in the call_id and vcd arrays in the 
    * location specified by the global current_vc if the interp 
    * is non-NULL.  
    */
    if ( interp ) {
        
        atmtest_store_call_id( setup->call_id, interp );
  	if ( atmtest_store_vcd( setup->vcd, interp ) != TCL_OK )
	   buginf( "process_sig_setup: error trying to store VCD.\n" );
    } 

    else
        buginf( "\nprocess_sig_setup: Error: NULL interp.\n" );


} /* atmtest_process_sig_setup() */

/* 
 * 
 * atmtest_process_sig_connect()
 *
 * This function is called by the atmtest_sig_callback_func() when
 * a CONNECT message is received for the local NSAP.
 *
 */
void atmtest_process_sig_connect( sig_api_connect *connect, Tcl_Interp *interp )
{
    buginf( rvc_connect_str );

    /*
     * Just output the received VCD to the console for now.
     */
    if ( atmtest_lane_debug_on( interp ) )
        buginf( "\nVCD %d received in the CONNECT message\n", connect->vcd );

    /*
     * Store the vcd in the LANE vcd array in the location specified
     * by the global current_vc.
     */

    if ( interp ) {
        if ( atmtest_store_vcd( connect->vcd, interp ) != TCL_OK )
	    buginf( "process_sig_connect: error trying to store VCD.\n" );
    }
    else
	buginf( "\nprocess_sig_connect: cannot store VCD - NULL interp.\n" );

} /* atmtest_process_sig_connect() */

/* 
 * 
 * atmtest_process_sig_connect_ack()
 *
 * This function is called by the atmtest_sig_callback_func() when
 * a ATM_CONNECT_ACK message is received for the local NSAP.
 *
 */
void atmtest_process_sig_connect_ack( sig_api_connect_ack *connect_ack, 
							Tcl_Interp *interp )
{
    buginf( rvc_connect_ack_str );

} /* atmtest_process_sig_connect_ack() */

/* 
 * 
 * atmtest_process_sig_release()
 *
 * This function is called by the atmtest_sig_callback_func() when
 * a ATM_RELEASE message is received for the local NSAP.
 *
 */
void atmtest_process_sig_release( sig_api_msg *release, Tcl_Interp *interp )
{
    sig_api_msg relComplete;

    buginf( rvc_release_str );
    /* respond immediately with a release complete */
    relComplete.opcode = ATM_RELEASE_COMPLETE;
    relComplete.u.release_comp.call_id = release->u.release.call_id;
    relComplete.u.release_comp.cause = release->u.release.cause;
    atmSig_handleUserRequest(&relComplete);

} /* atmtest_process_sig_connect_ack() */

/* 
 * 
 * atmtest_process_sig_release_comp()
 *
 * This function is called by the atmtest_sig_callback_func() when
 * a ATM_RELEASE_COMPLETE message is received for the local NSAP.
 *
 */
void atmtest_process_sig_release_comp( sig_api_release_comp *release_comp, 
						Tcl_Interp *interp )
{
    buginf( rvc_release_comp_str );

    /*
     * Clear the call_id and vcd elements of their respective 
     * arrays based on the call_id in the release message.
     */
    atmtest_clear_array_entries( release_comp->call_id, interp );

} /* atmtest_process_sig_release_comp() */

/* 
 * 
 * atmtest_process_sig_add_party()
 *
 * This function is called by the atmtest_sig_callback_func() when
 * a ATM_ADD_PARTY message is received for the local NSAP.
 *
 */
void atmtest_process_sig_add_party( sig_api_add_party *add_party, Tcl_Interp *interp )
{
    buginf( rvc_add_party_str );

} /* atmtest_process_sig_add_party() */

/* 
 * 
 * atmtest_process_sig_add_party_ack()
 *
 * This function is called by the atmtest_sig_callback_func() when
 * a ATM_ADD_PARTY_ACK message is received for the local NSAP.
 *
 */
void atmtest_process_sig_add_party_ack( sig_api_add_party_ack *add_party_ack, Tcl_Interp *interp )
{
    buginf( rvc_add_party_ack_str );

} /* atmtest_process_sig_add_party_ack() */

/* 
 * 
 * atmtest_process_sig_add_party_rej()
 *
 * This function is called by the atmtest_sig_callback_func() when
 * a ATM_ADD_PARTY_REJ message is received for the local NSAP.
 *
 */
void atmtest_process_sig_add_party_rej( sig_api_add_party_rej *add_party_rej, Tcl_Interp *interp )
{
    buginf( rvc_add_party_rej_str );


} /* atmtest_process_sig_add_party_rej() */

/* 
 * 
 * atmtest_process_sig_drop_party()
 *
 * This function is called by the atmtest_sig_callback_func() when
 * a ATM_DROP_PARTY message is received for the local NSAP.
 *
 */
void atmtest_process_sig_drop_party( sig_api_drop_party *drop_party, Tcl_Interp *interp )
{
    buginf( rvc_drop_party_str );

    if ( atmtest_lane_debug_on( interp ) ) {
	buginf( "%s call_id received in DROP_PARTY = 0x%x\n", ATMTEST_LANE_DEBUG_LEADER,
						drop_party->call_id );
	buginf( "%s endpoint_id received in DROP_PARTY = 0x%x\n", ATMTEST_LANE_DEBUG_LEADER,
						drop_party->endpoint_id );
        buginf( "\n" );
    }

} /* atmtest_process_sig_drop_party() */

/* 
 * 
 * atmtest_process_sig_drop_party_ack()
 *
 * This function is called by the atmtest_sig_callback_func() when
 * a ATM_DROP_PARTY_ACK message is received for the local NSAP.
 *
 */
void atmtest_process_sig_drop_party_ack( sig_api_drop_party_ack *drop_party_ack, Tcl_Interp *interp )
{
    buginf( rvc_drop_party_ack_str );

} /* atmtest_process_sig_drop_party_ack() */
