/* $Id: exec_show_atm_common.h,v 3.1.4.4 1996/08/13 17:52:06 rzagst Exp $
 * $Source: /release/112/cvs/Xsys/if/exec_show_atm_common.h,v $
 *------------------------------------------------------------------
 * exec_show_atm_common.h: SHOW command parsing
 *
 * March, 1996, Rob Zagst
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_atm_common.h,v $
 * Revision 3.1.4.4  1996/08/13  17:52:06  rzagst
 * CSCdi64228:  Show atm vc command is not modular with Cat5k
 * Use NUMBER_FUNCTION macro to allow largest vcd possible on interface.
 * Branch: California_branch
 *
 * Revision 3.1.4.3  1996/05/21  09:49:45  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.4.2  1996/05/07  00:17:45  rzagst
 * CSCdi56619:  arp-server showing up in show commands instead of show atm
 * commands
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/04/25  23:13:26  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1  1996/03/30  04:45:29  rzagst
 * Placeholder for exec_show_atm_common.h.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * function that co-works w/ NUMBER_FUNC macro to retrieve the current
 * range of the VCD.
 */
static void show_atm_vc_func (parseinfo *csb, uint *lower, uint *upper)
{
    uint max_vc_value;
    uint i;
    
	/*
	 * AIP has VC table w/ VCD in the range of [0..config.max_intf_vcs-1].
         * However, since VCD=0 is reserved by the AIP, usr is only allowed 
	 * to use VCD's in the range of [1..config.max_intf_vcs-1]. For
	 * compatability the other interfaces also don't use vcd=0.
	 */
    *lower = 1;
    max_vc_value = 0;
    /*
     * look through all interfaces to find largest max vcd
     */
    for (i = 0;i < MAX_ATM_INTFC;i++) {
	if ((atm_db[i].flags & ATM_FLAG_DB_INUSE) &&
	    (atm_db[i].config.max_intf_vcs > max_vc_value))
	    max_vc_value = atm_db[i].config.max_intf_vcs;
    }
	    
    *upper = max_vc_value - 1;
}
    


/******************************************************************
 * show ATM traffic
 */
EOLS	(show_atm_traffic_eol, atm_show_commands,   SHOW_ATM_TRAFFIC);
KEYWORD (show_atm_traffic, show_atm_traffic_eol, ALTERNATE,
            "traffic", "ATM statistics", PRIV_USER);

/******************************************************************
 * show ATM vc [<num>]
 * OBJ(int,1) = <vcnum>
 */
EOLS	(show_atm_vc_eol, atm_show_commands,        SHOW_ATM_VC);
/* <pvc no.> */
NUMBER_FUNC	(show_atm_pvcno, show_atm_vc_eol, show_atm_vc_eol,
	OBJ(int,1), show_atm_vc_func, "ATM VCD number");
KEYWORD (show_atm_vc, show_atm_pvcno, show_atm_traffic,
            "vc", "ATM VC information", PRIV_USER);

/******************************************************************
 * show ATM int [<atm int>]
 *
 * OBJ(idb,1) = <interface>
 */
EOLS	(show_atm_int_eol, atm_show_commands,        SHOW_ATM_INT);

INTERFACE (show_atm_get_interface, show_atm_int_eol, no_alt,
	   OBJ(idb,1), IFTYPE_ATM);

KEYWORD (show_atm_int, show_atm_get_interface, show_atm_vc,
            common_str_interface, "Interfaces and ATM information", PRIV_USER);

/******************************************************************
 * show ATM
 */
#undef	ALTERNATE
#define	ALTERNATE	show_atm_int
