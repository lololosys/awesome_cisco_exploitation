/* $Id: exec_atmsig.h,v 3.2 1995/11/17 08:46:42 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/atm/exec_atmsig.h,v $ 
 *------------------------------------------------------------------
 * parser/exec_atmsig.h:  EXEC command parsing code
 *
 * November, 1993,  Percy P. Khabardar 
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All Rights Reserved.
 *
 * Exec commands for ATM Signalling.
 *------------------------------------------------------------------
 * $Log: exec_atmsig.h,v $
 * Revision 3.2  1995/11/17  08:46:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:00:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/01  17:01:37  percyk
 * CSCdi39493: provide help screen for atmsig exec cmds.
 *
 * Revision 2.2  1995/07/04  22:34:45  percyk
 * CSCdi36587:  cannot manually clear a connection off a subinterface.
 *
 * Revision 2.1  1995/07/04  09:35:11  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize arap, atm, bstun
 *
 * Revision 2.1  1995/06/07  22:15:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************************
 *  atmsig status <interface> <vcnum>
 */
EOLS    (exec_atmsig_status_eol, atmSig_exec_commands, PARSER_ATMSIG_STATUS );
GENERAL_NUMBER(exec_atmsig_status_vcnum, exec_atmsig_status_eol, exec_atmsig_status_eol,
           OBJ(int,2), 1, 2048, "VC Number",
           (NUMBER_DEC | HEX_NO_ZEROX_OK | NUMBER_WS_OK | NUMBER_HELP_CHECK) );
INTERFACE(exec_atmsig_status_int, exec_atmsig_status_vcnum, no_alt,
          OBJ(idb,1), (IFTYPE_ATM | IFTYPE_SUBIF) );
KEYWORD (exec_atmsig_status, exec_atmsig_status_int, no_alt,
         "status", "check status of a circuit", PRIV_USER);


/***********************************************************************************/

/************************************************************************************
 *  atmsig close <interface> <vcnum>
 */
EOLS    (exec_atmsig_close_eol, atmSig_exec_commands, PARSER_ATMSIG_CLOSE );
SIGNED_DEC(exec_atmsig_close_vcnum, exec_atmsig_close_eol, exec_atmsig_close_eol,
           OBJ(int,2), 1, 2048, "VC Number");
INTERFACE(exec_atmsig_close_int, exec_atmsig_close_vcnum, no_alt,
          OBJ(idb,1), (IFTYPE_ATM | IFTYPE_SUBIF) );
KEYWORD (exec_atmsig_close, exec_atmsig_close_int, exec_atmsig_status,
         "close", "Close a circuit", PRIV_USER);


/***********************************************************************************/
KEYWORD (exec_atmsig, exec_atmsig_close, ALTERNATE,
         "atmsig", "Execute Atm Signalling Commands", PRIV_USER);

#undef  ALTERNATE
#define ALTERNATE       exec_atmsig


