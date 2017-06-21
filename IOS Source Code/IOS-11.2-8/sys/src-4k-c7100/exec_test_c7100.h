/* $Id: exec_test_c7100.h,v 3.1.66.3 1996/08/12 16:07:23 widmer Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c7100/exec_test_c7100.h,v $
 *------------------------------------------------------------------
 * exec_test_c7100.h : C7100 specific test commands
 *
 * Novemeber 1995, Michael Beesley
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_test_c7100.h,v $
 * Revision 3.1.66.3  1996/08/12  16:07:23  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.66.2  1996/04/06  02:05:46  ssangiah
 * CSCdi53822:  Need to deal with product name change.
 * Make changes to deal with the change in the product name for Predator
 * from c7100 to c7200.
 * Branch: California_branch
 *
 * Revision 3.1.66.1  1996/03/21  23:48:22  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.3  1996/01/23  02:18:15  mbeesley
 * Branch: ELC_branch
 * Add support for the CPU ID EEPROM. Postpone PCI SRAM initialization
 * until the rev of the CPU card has been read and verified.
 *
 * Revision 3.1.2.2  1996/01/09  21:27:51  mbeesley
 * Branch: ELC_branch
 * Enhance/fix Midplane EEPROM utility. Protect against left over
 * interrupts from boot helper image. Fix some gt64010 register naming
 * mistakes.
 *
 * Revision 3.1.2.1  1995/11/22  21:43:32  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/11/21  01:37:53  mbeesley
 * Add placeholder files for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * test c7200 debug {off|on}
 *
 *	OBJ(int,1) = { OFF | ON }
 */
EOLNS	(c7100_debug_eol, c7100_test_debug_command);

KEYWORD_ID(c7100_debug_off, c7100_debug_eol, no_alt,
	   OBJ(int,1), TEST_C7100_DEBUG_STATE_OFF,
	   "off", "disable C7200 debug messages", 0);

KEYWORD_ID(c7100_debug_on, c7100_debug_eol, c7100_debug_off,
	   OBJ(int,1), TEST_C7100_DEBUG_STATE_ON,
	   "on", "enable C7200 debug messages", 0);

KEYWORD(test_c7100_debug, c7100_debug_on, no_alt, "debug",
	"Disable/Enable C7200 debug messages", 0);

/******************************************************************
 * test c7200 mpeeprom { read <addr> | write <addr> <byte> }
 *    OBJ(int,1) = { read | write }
 *    OBJ(int,2) = <addr>
 *    OBJ(int,3) = <byte>
 */

EOLNS	(mpeeprom_eol, test_c7100_mpeeprom);

GENERAL_NUMBER(mpeeprom_val, mpeeprom_eol, no_alt,
	       OBJ(int,3), 0, 0xff, "value",
	       NUMBER_HEX | HEX_NO_ZEROX_OK | NUMBER_WS_OK |
	       NUMBER_HELP_CHECK);

GENERAL_NUMBER(mpeeprom_addrval, mpeeprom_val, no_alt,
	       OBJ(int,2), 0, 0x1f, "address",
	       NUMBER_HEX | HEX_NO_ZEROX_OK | NUMBER_WS_OK |
	       NUMBER_HELP_CHECK);

KEYWORD_ID(mpeeprom_write, mpeeprom_addrval, no_alt,
	   OBJ(int,1), TEST_C7100_EEPROM_WRITE,
	   "write", "write a byte", 0);

GENERAL_NUMBER(mpeeprom_addr, mpeeprom_eol, no_alt,
	       OBJ(int,2), 0, 0x1f, "address",
	       NUMBER_HEX | HEX_NO_ZEROX_OK | NUMBER_WS_OK |
	       NUMBER_HELP_CHECK);

KEYWORD_ID(mpeeprom_read, mpeeprom_addr, mpeeprom_write,
	   OBJ(int,1), TEST_C7100_EEPROM_READ,
	   "read", "read a byte", 0);

KEYWORD(test_c7100_mpeeprom, mpeeprom_read, test_c7100_debug,
	"mpeeprom", "read/write Mid-plane EEPROM", 0);

/******************************************************************
 * test c7200 cpueeprom { read <addr> | write <addr> <byte> }
 *    OBJ(int,1) = { read | write }
 *    OBJ(int,2) = <addr>
 *    OBJ(int,3) = <byte>
 */

EOLNS	(cpueeprom_eol, test_c7100_cpueeprom);

GENERAL_NUMBER(cpueeprom_val, cpueeprom_eol, no_alt,
	       OBJ(int,3), 0, 0xff, "value",
	       NUMBER_HEX | HEX_NO_ZEROX_OK | NUMBER_WS_OK |
	       NUMBER_HELP_CHECK);

GENERAL_NUMBER(cpueeprom_addrval, cpueeprom_val, no_alt,
	       OBJ(int,2), 0, 0x1f, "address",
	       NUMBER_HEX | HEX_NO_ZEROX_OK | NUMBER_WS_OK |
	       NUMBER_HELP_CHECK);

KEYWORD_ID(cpueeprom_write, cpueeprom_addrval, no_alt,
	   OBJ(int,1), TEST_C7100_EEPROM_WRITE,
	   "write", "write a byte", 0);

GENERAL_NUMBER(cpueeprom_addr, cpueeprom_eol, no_alt,
	       OBJ(int,2), 0, 0x1f, "address",
	       NUMBER_HEX | HEX_NO_ZEROX_OK | NUMBER_WS_OK |
	       NUMBER_HELP_CHECK);

KEYWORD_ID(cpueeprom_read, cpueeprom_addr, cpueeprom_write,
	   OBJ(int,1), TEST_C7100_EEPROM_READ,
	   "read", "read a byte", 0);

KEYWORD(test_c7100_cpueeprom, cpueeprom_read, test_c7100_mpeeprom,
	"cpueeprom", "read/write CPU EEPROM", 0);

KEYWORD(test_c7100, test_c7100_cpueeprom, ALTERNATE,
	"c7200", NULL, PRIV_ROOT|PRIV_NOHELP);

#undef	ALTERNATE
#define	ALTERNATE	test_c7100

/* end of file */
