/* $Id: exec_vtp_load.h,v 1.1.2.1 1996/06/20 19:50:12 cyoung Exp $
 * $Source: /release/112/cvs/Xsys/vtp/exec_vtp_load.h,v $
 *------------------------------------------------------------------
 * exec_vtp_load.h - CLI VTP load database function header file
 *
 * June 1996, Chris Young
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_vtp_load.h,v $
 * Revision 1.1.2.1  1996/06/20  19:50:12  cyoung
 * Use correct VLAN count to calculate TFTP file size
 * CSCdi60853:  wrong behaviour while reading tftp file --> vlan info
 * recovery fail Branch: California_branch
 * Also while bonnet is up: Put in a limiting depth in for waiting for
 * semaphore to be released.  Allow null domain to receive domain change
 * notifications.  Provide function to manually load TFTP database and
 * load the database not only when TFTP server or file change but also
 * VTP entity state (client --> server).
 *
 * Revision 1.1  1996/06/20  19:40:02  cyoung
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * vtp load-database <domain-name>
 * 
 * OBJ(string,1) = <domain-name>
 *****************************************************************/
EOLS   (vtp_load_eol, vtp_cli_load_database, LOAD_VTP_DATABASE);

STRING (vtp_load_domain_name, vtp_load_eol, no_alt,
        OBJ(string,1), "A VTP domain name");
KEYWORD (vtp_load_database, vtp_load_domain_name, no_alt,
	 "load-database", "Load the VTP Database for a management"
	 " domain",PRIV_OPR);  

KEYWORD (vtp_commands, vtp_load_database, ALTERNATE,
	 "vtp", "Execute VTP commands", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	vtp_commands
