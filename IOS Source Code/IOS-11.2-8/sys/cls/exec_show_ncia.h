/* $Id: exec_show_ncia.h,v 3.1.2.5 1996/08/12 16:00:53 widmer Exp $
 * $Source: /release/112/cvs/Xsys/cls/exec_show_ncia.h,v $
 *------------------------------------------------------------------
 * exec_show_ncia.h - Definitions for NCIA show command
 *
 * March 1996, Steve Chiang
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_ncia.h,v $
 * Revision 3.1.2.5  1996/08/12  16:00:53  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.2.4  1996/06/23  05:52:59  schiang
 * CSCdi61120:  split exec_show_ncia.h into exec_show_ncia.h and
 * exec_ncia.h
 * Branch: California_branch
 *
 * Revision 3.1.2.3  1996/06/12  06:51:39  schiang
 * CSCdi60121:  ncia keepalive cant be turned through configuration
 * clean up code so that no line exceeds 80 characters.
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/06/01  21:49:19  jolee
 * CSCdi59245:  Added clear ncia client registered & show ncia client
 * saplist
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/05/17  10:43:16  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.4.2  1996/05/03  22:06:15  schiang
 * CSCdi56629:  ncia server code needs to clean up
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.1  1996/04/05  06:05:38  schiang
 * Branch: IbuMod_Calif_branch
 * merge ncia server code
 *
 * Revision 3.1  1996/03/29  07:12:26  schiang
 * CSCdi53012:  add placeholder for NCIA files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show ncia {circuits | clients | servers}
 *
 * OBJ(int,1) = command type NCIA_SHOW_{CIRCUIT,CLIENT,SERVER}  
 *
 * If command is "show ncia client saplist"
 * ---------------------------------
 * options:
 * { <ip-address> | <cr> for all}
 *
 * OBJ(paddr,1) = ip address of remote client's saplist to display
 *
 * If command is "show ncia clients"
 * ---------------------------------
 * options:
 * { <ip-address> | <cr> for all | saplist}
 *
 * OBJ(paddr,1) = ip address of remote client to display
 *
 * If command type is "show ncia circuit" 
 * --------------------------------------
 * options:
 * { <circuit ID> | <cr> for all}
 *
 * OBJ(int,3) = <circuit ID>
 *
 * If command type is "show ncia server"
 * --------------------------------------
 * options:
 * { <server num> | <cr> for all}
 *
 * OBJ(int,3) = <server num>
 */

EOLNS	(show_ncia_eol, show_ncia);

NUMBER(show_server_num, show_ncia_eol, show_ncia_eol,
       OBJ(int,2), 0, 0xFF, "Server number");

SET     (show_server_def, show_server_num, OBJ(int,2), NCIA_SHOW_ALL_SERVERS);

KEYWORD_ID(show_ncia_server, show_server_def, no_alt,
           OBJ(int,1), NCIA_SHOW_SERVER, "server",
           "Display NCIA server information", PRIV_USER);

IPADDR(show_client_ipaddr, show_ncia_eol, show_ncia_eol,
       OBJ(paddr,1), "Remote Client IP address");

KEYWORD_ID(show_client_sap, show_client_ipaddr, show_client_ipaddr,
           OBJ(int,2), TRUE, "saplist",
           "Display NCIA client saplist information", PRIV_USER);
 
SET     (show_client_init, show_client_sap, OBJ(paddr,1), 
         NCIA_SHOW_ALL_CLIENTS);

KEYWORD_ID(show_ncia_clients, show_client_init, show_ncia_server,
           OBJ(int,1), NCIA_SHOW_CLIENT, "clients", 
           "Display NCIA client information", PRIV_USER);

HEXNUM(show_ckt_id, show_ncia_eol, show_ncia_eol,
       OBJ(int,2), "Circuit ID");
 
SET     (show_ckt_init, show_ckt_id, OBJ(int,2), NCIA_SHOW_ALL_CIRCUITS);

KEYWORD_ID(show_ncia_circuits, show_ckt_init, show_ncia_clients,
           OBJ(int,1), NCIA_SHOW_CIRCUIT, "circuits", 
           "Display NCIA circuit information", PRIV_USER);

KEYWORD (show_ncia, show_ncia_circuits, no_alt,
	 "ncia", "Native Client Interface Architecture",
	 PRIV_USER);

TEST_BOOLEAN(show_ncia_test, show_ncia, NONE, ALTERNATE, bridge_enable);

#undef	ALTERNATE
#define	ALTERNATE	show_ncia_test
