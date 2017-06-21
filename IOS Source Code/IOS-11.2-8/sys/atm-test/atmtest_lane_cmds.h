/* $Id: atmtest_lane_cmds.h,v 3.3 1995/11/17 08:47:46 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/atm-test/atmtest_lane_cmds.h,v $
 *------------------------------------------------------------------
 * atmtest_lane_cmds.h -- Definitions to support LAN Emulation related
 * TCL commands.
 * [Was ../tcl/tcl_lane_cmds.h]
 *
 * April 1995,  Chris Parrott
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmtest_lane_cmds.h,v $
 * Revision 3.3  1995/11/17  08:47:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:07:27  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:03:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _ATMTEST_LANE_CMDS_H_
#define _ATMTEST_LANE_CMDS_H_


/*
 * Externally declared functions
 */
extern boolean match_en_addr(parseinfo *, char *, int *, 
				void *, int, uint, idbtype *);

extern boolean match_atm_nsap(parseinfo *, char *, int *, 
				void *, int, uint,idbtype *);

extern boolean lane_control_encap(paktype *pak, ushort vcd);
extern boolean parse_ip_address (char *ptr, ipaddrtype *result);

/*
 * Prototype declarations of the LANE TCL command functions
 */
int 	Tcl_HelpCmd _ANSI_ARGS_((ClientData clientData, 
		    Tcl_Interp *interp, int argc, char **argv));
int 	Tcl_LaneSendCmd _ANSI_ARGS_((ClientData clientData, 
		    Tcl_Interp *interp, int argc, char **argv));
int 	Tcl_SetLaneDefCmd _ANSI_ARGS_((ClientData clientData, 
		    Tcl_Interp *interp, int argc, char **argv));
int     Tcl_ShowVarsCmd _ANSI_ARGS_((ClientData clientData, 
		    Tcl_Interp *interp, int argc, char **argv));

/*
 * Other functions used to support the LANE commands
 */
boolean atmtest_lane_debug_on( Tcl_Interp *interp );
void debug_printf( char *str, Tcl_Interp *interp );
void atmtest_init_lane_vars( Tcl_Interp *interp );
int atmtest_load_lane_hdr( uchar *bufptr, Tcl_Interp *interp );
int atmtest_load_lane_cfg( uchar *bufptr, Tcl_Interp *interp );
int atmtest_load_lane_arp( uchar *bufptr, Tcl_Interp *interp );
void atmtest_dump_buffer( uchar *buf, int size, Tcl_Interp *interp );
int atmtest_send_ctl_pkt( uchar *bufptr, int dgram_size, Tcl_Interp *interp );
idbtype *atmtest_get_atm_interface( uint slot_n, uint port_n, 
					uint subif, Tcl_Interp *interp );
int atmtest_send_cfg_pkt ( Tcl_Interp *interp );
int atmtest_send_arp_pkt ( Tcl_Interp *interp );
void atmtest_dump_lsend_arglist( void );
void atmtest_print_help_string( char *str );
void atmtest_print_gen_help( void );
int atmtest_send_ip_arp_pkt ( Tcl_Interp *interp );
int atmtest_load_ip_arp( uchar *bufptr, Tcl_Interp *interp );
idbtype *atmtest_get_atm_idb( Tcl_Interp *interp );

/*
 * Literals used by the LANE command and support functions.
 */
#define CFG_CTL_PKT_SZ		sizeof( lane_cfg_pdu_t )
#define ARP_CTL_PKT_SZ		sizeof( lane_arp_pdu_t )
#define IP_LANE_DGRAM_SZ	( sizeof( lane_data_hdr_t ) + RBYTES )

/* This needs to change if we decide to send bigger packets */
#define ATMTEST_LANE_PKT_ALLOC_SZ	MEDDATA

#define ATMTEST_ATM_INTERFACE_NAME	"ATM"

/* 
 * Additional types of messages supported by "lsend". 
 * Note that these literals cannot conflict with the constants 
 * defined for the LAN Emulation Control Opcodes in lane_private.h.
 */
#define LANE_IP_ARP		0x1000


/*
 * TCL LANE debug related stuff
 */

/*
 * Setting this TCL variable to TRUE from the command line will 
 * turn on debugging; setting it back to FALSE (or not setting it)
 * disables debugging.
 */
#define LANE_DEBUG_VAR		"lane_debug"

/*
 * Number of bytes to print in a row when the buffer is dumped.
 */
#define PKT_DUMP_WIDTH		20

#define ATMTEST_LANE_DEBUG_LEADER	"TCL DEBUG>> "


#endif _ATMTEST_LANE_CMDS_H_
