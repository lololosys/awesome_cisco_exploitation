/* $Id: cfg_int_bsend_tst.h,v 3.1.68.1 1996/05/17 10:41:09 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/bstun/cfg_int_bsend_tst.h,v $
 *------------------------------------------------------------------
 * BSEND interface configuration commands for BSC test
 *
 * Oct 95. Saritha Roach
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_bsend_tst.h,v $
 * Revision 3.1.68.1  1996/05/17  10:41:09  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.56.2  1996/04/11  08:19:51  jbalestr
 * CSCdi51807:  need a test command to transmit raw bisync frames
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.56.1  1996/04/03  13:50:49  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.1  1995/11/09  11:04:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/10  07:56:29  sroach
 * CSCdi41879:  Add BSC test command support
 *
 * Revision 2.1  1995/10/10  05:42:49  sroach
 * *** empty log message ***
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/************************************************************************
 * bsend add <control-unit-address>
 *
 */

EOLS	(ci_bsend_addeol, bsend_command, BSEND_CMD_ADD);
BSTUNADDR(ci_bsend_add_cuaddr, ci_bsend_addeol, no_alt,
	  OBJ(paddr,1), "Address of Control Unit to start polling");
KEYWORD (ci_bsend_add, ci_bsend_add_cuaddr, no_alt,
	 "add",
         "Start polling a Control Unit", PRIV_CONF|PRIV_NONVGEN);


/************************************************************************
 * bsend delete <control-unit-address>
 *
 */

EOLS	(ci_bsend_deleol, bsend_command, BSEND_CMD_DELETE);
BSTUNADDR(ci_bsend_del_cuaddr, ci_bsend_deleol, no_alt,
	  OBJ(paddr,1),"Address of Control Unit to stop polling");
KEYWORD (ci_bsend_del, ci_bsend_del_cuaddr, ci_bsend_add,
	 "delete",
         "Stop polling a Control Unit", PRIV_CONF|PRIV_NONVGEN);


/************************************************************************
 * bsend active <control-unit-address>
 *
 */

EOLS	(ci_bsend_acteol, bsend_command, BSEND_CMD_ACTIVE);
BSTUNADDR(ci_bsend_act_cuaddr, ci_bsend_acteol, no_alt,
	  OBJ(paddr,1),"Address of Control Unit that is active");
KEYWORD (ci_bsend_act, ci_bsend_act_cuaddr, ci_bsend_del,
	 "active",
         "Notify that a Control Unit is active", PRIV_CONF|PRIV_NONVGEN);


/************************************************************************
 * bsend inactive <control-unit-address>
 *
 */

EOLS	(ci_bsend_inacteol, bsend_command, BSEND_CMD_INACTIVE);
BSTUNADDR(ci_bsend_inact_cuaddr, ci_bsend_inacteol, no_alt,
	  OBJ(paddr,1),"Address of Control Unit that is inactive");
KEYWORD (ci_bsend_inact, ci_bsend_inact_cuaddr, ci_bsend_act,
	 "inactive",
         "Notify that a Control Unit is inactive", PRIV_CONF|PRIV_NONVGEN);


/************************************************************************
 * bsend data <control-unit-address> <device-address> <string>
 *
 */

EOLS	(ci_bsend_dataeol, bsend_command, BSEND_CMD_DATA);
TEXT    (ci_bsend_data_text, ci_bsend_dataeol, no_alt,
         OBJ(string,1), "Data to send to device");
BSTUNADDR(ci_bsend_data_devaddr, ci_bsend_data_text, no_alt,
	  OBJ(paddr,2),"Address of device to send data to");
BSTUNADDR(ci_bsend_data_cuaddr, ci_bsend_data_devaddr, no_alt,
	  OBJ(paddr,1),"Address of Control Unit for device to send data to");
KEYWORD (ci_bsend_data, ci_bsend_data_cuaddr, ci_bsend_inact,
	 "data",
         "Send 3270 data to a device", PRIV_CONF|PRIV_NONVGEN);

/************************************************************************
 * bsend raw <device-address> <string>
 *
 */

EOLS	(ci_bsend_raweol, bsend_command, BSEND_CMD_RAW);
TEXT    (ci_bsend_raw_text, ci_bsend_raweol, no_alt,
         OBJ(string,1), "raw data to send to device");
BSTUNADDR(ci_bsend_raw_cuaddr, ci_bsend_raw_text, no_alt,
	  OBJ(paddr,1),"Send raw data stream to device at <device-address>");
KEYWORD (ci_bsend_raw, ci_bsend_raw_cuaddr, ci_bsend_data,
	 "raw",
         "Send raw data stream to a device", PRIV_CONF|PRIV_NONVGEN);

/************************************************************************
 * bsend size <control-unit-address> <count>
 *
 */

EOLS	(ci_bsend_size_eol, bsend_command, BSEND_CMD_SIZE);
NUMBER  (ci_bsend_size_val, ci_bsend_size_eol, no_alt, OBJ(int,1), 
		1, 5000, "size of data frame to send to device" );
BSTUNADDR(ci_bsend_size_cuaddr, ci_bsend_size_val, no_alt,
	  OBJ(paddr,1)," Control Unit for device to send data to");
KEYWORD (ci_bsend_size, ci_bsend_size_cuaddr, ci_bsend_raw,
	 "size",
         "Send data frame of size to a device", PRIV_CONF|PRIV_NONVGEN);

/***********************************************************************
 * bsend
 *
 */

KEYWORD	(ci_bsend, ci_bsend_size, NONE,
         "bsend",
	 "BSEND (BSTUN test) interface subcommands", PRIV_CONF|PRIV_NONVGEN);

TESTVAR	(ci_bsend_no_test, ci_bsend, no_alt,
	 NONE, NONE, NONE, sense, TRUE);

ASSERT	(ci_bsend_test, ci_bsend_no_test, ALTERNATE,
	 (csb->interface->hwptr->status & IDB_SERIAL));

#undef  ALTERNATE
#define ALTERNATE ci_bsend_test
