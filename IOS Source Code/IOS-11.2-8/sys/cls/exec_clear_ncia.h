/* $Id: exec_clear_ncia.h,v 3.1.2.4 1996/08/12 16:00:48 widmer Exp $
 * $Source: /release/112/cvs/Xsys/cls/exec_clear_ncia.h,v $
 *------------------------------------------------------------------
 * exec_clear_ncia.h - Definitions for NCIA clear command
 *
 * March 1996, Steve Chiang
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_clear_ncia.h,v $
 * Revision 3.1.2.4  1996/08/12  16:00:48  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.2.3  1996/06/12  06:51:36  schiang
 * CSCdi60121:  ncia keepalive cant be turned through configuration
 * clean up code so that no line exceeds 80 characters.
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/06/01  21:49:17  jolee
 * CSCdi59245:  Added clear ncia client registered & show ncia client
 * saplist
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/05/17  10:43:08  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.4.3  1996/05/14  04:31:46  jolee
 * CSCdi57570:  Added clear client support, removed backup mac suppport
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.2  1996/05/03  22:06:13  schiang
 * CSCdi56629:  ncia server code needs to clean up
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.1  1996/04/05  06:05:36  schiang
 * Branch: IbuMod_Calif_branch
 * merge ncia server code
 *
 * Revision 3.1  1996/03/29  07:10:51  schiang
 * CSCdi53012:  add placeholder for NCIA files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * clear ncia {circuit | client}
 *
 * OBJ(int,1) = command type NCIA_CLEAR_{CIRCUIT,CLIENT}
 *
 * If command is "clear ncia client registered"
 * -------------------------------------------
 * options:
 * { <ip-address> | <cr> for all}
 *
 * OBJ(paddr,1) = ip address of remote registered client, ctl blk be deleted
 *
 * If command is "clear ncia client"
 * ---------------------------------
 * options:
 * { <ip-address> | <cr> for all}
 *
 * OBJ(paddr,1) = ip address of remote client to be cleared
 *
 * If command is "clear ncia circuit"
 * --------------------------------------
 * options :
 * { <circuit ID> | <cr> for all}
 *
 * OBJ(int,2) = <circuit ID>
 *
 */

EOLNS    (clear_ncia_eol, clear_ncia);

IPADDR(clear_client_ip, clear_ncia_eol, clear_ncia_eol,
       OBJ(paddr,1), "Client IP address");

KEYWORD_ID(clear_client_reg, clear_client_ip, clear_client_ip,
           OBJ(int,2), TRUE, "registered",
           "Clear NCIA registered client, free control block", PRIV_USER);
 
SET     (clear_client_init, clear_client_reg, OBJ(paddr,1), 
         NCIA_CLEAR_ALL_CLIENTS);

KEYWORD_ID(clear_ncia_client, clear_client_init, no_alt,
           OBJ(int,1), NCIA_CLEAR_CLIENT, "clients",
           "Clear NCIA clients", PRIV_USER);

HEXNUM(clear_ckt_hex, clear_ncia_eol, clear_ncia_eol,
       OBJ(int,2), "Circuit ID");

SET     (clear_ckt_init, clear_ckt_hex, OBJ(int,2), NCIA_CLEAR_ALL_CIRCUITS);

KEYWORD_ID(clear_ncia_circuit, clear_ckt_init, clear_ncia_client,
           OBJ(int,1), NCIA_CLEAR_CIRCUIT, "circuits",
           "Clear NCIA circuits", PRIV_USER);

/******************************************************************
 * clear ncia
 */
KEYWORD (clear_ncia, clear_ncia_circuit, no_alt,
         "ncia", "Native Client Interface Architecture (NCIA) ",
         PRIV_USER /*|PRIV_HIDDEN*/);


#undef  ALTERNATE
#define ALTERNATE       clear_ncia





