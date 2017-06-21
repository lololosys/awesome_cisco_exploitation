/* $Id: atmtest_sig_cmds.h,v 3.2.60.1 1996/03/18 19:00:45 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/atm-test/atmtest_sig_cmds.h,v $
 *------------------------------------------------------------------
 * atmtest_atmsig_cmds.h -- Definitions to support ATM Signalling 
 * related TCL commands.
 * [Was ../tcl/tcl_atmsig_cmds.h]
 *
 * April 1995,  Chris Parrott
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmtest_sig_cmds.h,v $
 * Revision 3.2.60.1  1996/03/18  19:00:45  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/01/24  21:17:54  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  08:47:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:03:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _ATMTEST_ATMSIG_CMDS_H_
#define _ATMTEST_ATMSIG_CMDS_H_


/*
 * Externally declared functions
 */
extern void atmtest_sig_callback_func( void *ptr );
extern void atmSmap_sendToSigDriver(sig_api_msg *msg);
extern void *atmSig_api_allocate_call_id(void);

/*
 * Prototype declarations of the ATMSIG TCL command functions
 */
int 	Tcl_siCmd _ANSI_ARGS_((ClientData clientData, 
		    Tcl_Interp *interp, int argc, char **argv));
int 	Tcl_slnCmd _ANSI_ARGS_((ClientData clientData, 
		    Tcl_Interp *interp, int argc, char **argv));
int 	Tcl_scnCmd _ANSI_ARGS_((ClientData clientData, 
		    Tcl_Interp *interp, int argc, char **argv));
int 	Tcl_ssCmd _ANSI_ARGS_((ClientData clientData, 
		    Tcl_Interp *interp, int argc, char **argv));
int 	Tcl_scCmd _ANSI_ARGS_((ClientData clientData, 
		    Tcl_Interp *interp, int argc, char **argv));
int 	Tcl_srCmd _ANSI_ARGS_((ClientData clientData, 
		    Tcl_Interp *interp, int argc, char **argv));
int 	Tcl_srcCmd _ANSI_ARGS_((ClientData clientData, 
		    Tcl_Interp *interp, int argc, char **argv));
int 	Tcl_sapCmd _ANSI_ARGS_((ClientData clientData, 
		    Tcl_Interp *interp, int argc, char **argv));
int 	Tcl_sdpCmd _ANSI_ARGS_((ClientData clientData, 
		    Tcl_Interp *interp, int argc, char **argv));

/*
 * Other functions used to support the ATMSIG commands
 */
atmSvcType *atmtest_get_call_id( int vc_type );
void atmtest_set_call_id( int vc_type, atmSvcType *c_id );
void atmtest_dump_call_id_array( Tcl_Interp *interp );
int atmtest_stuff_blli_fields( blli_type *blli_ptr, Tcl_Interp *interp );
int atmtest_get_vc_type( char *vc_type );
int atmtest_store_call_id( atmSvcType *c_id, Tcl_Interp *interp );
atmSvcType *atmtest_retrieve_call_id( Tcl_Interp *interp );
void atmtest_process_sig_setup( sig_api_setup *setup, 
						Tcl_Interp *interp );
void atmtest_process_sig_connect( sig_api_connect *connect, 
						Tcl_Interp *interp );
void atmtest_process_sig_connect_ack( sig_api_connect_ack *connect_ack, 
						Tcl_Interp *interp );
void atmtest_process_sig_release( sig_api_msg *release, 
						Tcl_Interp *interp );
void atmtest_process_sig_release_comp( sig_api_release_comp *release_comp, 
						Tcl_Interp *interp );
void atmtest_process_sig_add_party( sig_api_add_party *add_party, 
						Tcl_Interp *interp );
void atmtest_process_sig_add_party_ack( sig_api_add_party_ack *add_party_ack, 
						Tcl_Interp *interp );
void atmtest_process_sig_add_party_rej( sig_api_add_party_rej *add_party_rej, 
						Tcl_Interp *interp );
void atmtest_process_sig_drop_party( sig_api_drop_party *drop_party, 
						Tcl_Interp *interp );
void atmtest_process_sig_drop_party_ack( sig_api_drop_party_ack *drop_party_ack, 
						Tcl_Interp *interp );
int atmtest_store_vcd( ushort vcd, Tcl_Interp *interp );
void atmtest_set_vcd( int vc_type, ushort vcd );
ushort atmtest_get_vcd( int vc_type );
void atmtest_dump_vcd_array( Tcl_Interp *interp );
void atmtest_set_g_interp( Tcl_Interp *interp );
Tcl_Interp *atmtest_get_g_interp( void );
void atmtest_link_lane_vcd_array( Tcl_Interp *interp );
int atmtest_convert_str( char *str, uchar *buf, int length, Tcl_Interp *interp );
void atmtest_clear_array_entries( atmSvcType *c_id, Tcl_Interp *interp );
atmSvcPartyType *atmtest_get_endpoint_id( int vc_type, int endpoint_id );
void atmtest_set_endpoint_id( int vc_type, int endpoint_id, atmSvcPartyType *c_id );
int atmtest_store_endpoint_id( atmSvcPartyType *c_id, Tcl_Interp *interp );
atmSvcPartyType *atmtest_retrieve_endpoint_id( Tcl_Interp *interp );
void atmtest_dump_endpoint_id_array( Tcl_Interp *interp );

/*
 * Maximum endpoint_id's we can store per call_id
 */
#define ATMTEST_MAX_MPT_ENDPTS	10


#endif _ATMTEST_ATMSIG_CMDS_H_
