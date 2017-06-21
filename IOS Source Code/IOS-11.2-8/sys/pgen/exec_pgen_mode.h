/* $Id: exec_pgen_mode.h,v 3.4.20.1 1996/05/21 10:00:27 thille Exp $
 * $Source: /release/112/cvs/Xsys/pgen/exec_pgen_mode.h,v $
 *------------------------------------------------------------------
 * E X E C _ P G E N _ M O D E . H
 *
 * Parse chain entries for PaGenT command mode
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_pgen_mode.h,v $
 * Revision 3.4.20.1  1996/05/21  10:00:27  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4  1996/02/01  23:34:22  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.3  1996/01/18  02:14:49  tli
 * CSCdi47063:  show ip cache cannot be restricted to subinterfaces
 * Change IFTYPE_ANY to IFTYPE_HWIDB
 *
 * Revision 3.2  1995/11/17  18:56:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:58:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/14  01:51:58  biren
 * CSCdi40312:  Move pakgen and pgen (Pagent 2.2) to seperate subsystems
 *
 * Revision 2.1  1995/06/07  22:33:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

/* Local macro to parse a packet range. A range can consist of a single
 * packet number, a single packet name, or two numbers/names separated
 * by a hyphen. The hyphen must be set off from the first packet name
 * by white space; otherwise it'll be treated as part of the name.
 * We don't do the symbolic vs. numeric parsing here; everything is
 * a string to us, and we let the command routine worry about how to
 * distinguish between a number and a symbolic name. If the second
 * string isn't specified, it doesn't get set.
 */

#define PRANGE(name, accept, alternate, string1, string2) \
  STRING (PRS2 ## name, accept, no_alt, string2, \
	  "Name or number of packet that ends range ($ for last packet)"); \
  WHITESPACE (PRWS ## name, PRS2 ## name, PRS2 ## name); \
  CHARACTER (PRHY ## name, PRWS ## name, accept, '-'); \
  HELP (PRH1 ## name, PRHY ## name, \
	"  - END\tGive ending packet for range\n"); \
  STRING (name, PRH1 ## name, alternate, string1, \
	  "Name or number of packet that starts range (* for all)")

/******************************************************************
 * version
 */
EOLS (pgen_version_eol, pgen_commands, PGEN_VERSION);
KEYWORD (pgen_version, pgen_version_eol, ALTERNATE,
	 "version", "Display the PaGent version number", PRIV_ROOT);

/******************************************************************
 * add <number> [name <name>] [time <time>] [repeat <repeat>]
 *    [interface <interface>] [length <length>]
 *    [byte <hex-offset> [<hex-byte> ...]]
 *    [interval <nsec-interval]
 *
 * patch <range> [name <name>] [time <time>] [repeat <repeat>]
 *    [interface <interface>] [length <length>]
 *    [byte <hex-offset> [<hex-byte> ...] | 
 *     insert <hex-offset> [<hex-byte> ...] |
 *     delete <hex-offset> <hex-length>]
 *     [interval <nsec-interval]
 *
 *  OBJ(int,1) = Code for "add" or "patch"
 *  OBJ(int,4) = time-to-send; -1 if none parsed
 *  OBJ(int,5) = Code for "byte", "insert", "delete", or nothing
 *  OBJ(int,6) = offset; -1 if no "byte", "insert", or "delete"
 *  OBJ(int,7) = length of data for "byte" or "insert"; length for "delete"
 *  OBJ(int,8) = length from "length" keyword (forced length); -1 for none
 *  OBJ(int,9) = repeat count; -1 for none
 *  OBJ(int,10) = nsec interval between packets; default -1 for unchanged
 *  OBJ(string,1) = name/number of first packet in range
 *  OBJ(string,2) = name/number of second packet in range, if any
 *  OBJ(string,3) = packet name (for setup or rename, not to select packet)
 *  OBJ(string,4) = data for "byte" or "insert", if any specified
 *  OBJ(idb,1) = interface (from common_str_interface keyword); NULL if none parsed
 */

pdecl (pgen_patch_length);	/* Start of repeatable keywords */

EOLS (pgen_patch_eol, pgen_commands, PGEN_ADDPATCH);

/* "byte" and "insert" keywords; both take offsets followed by data. If
 * no data are specified on the initial line, we read in a multiline
 * stream of data terminated by a blank line. The hex data are put into
 * a buffer in the packet generator context instead of into a parser
 * object. We use EVAL calls to accomplish this.
 */

HEXDATA (pgen_patch_data, pgen_patch_eol, pgen_patch_eol,
	 OBJ(string,4), OBJ(int,7), "Hex data", PARSEBUF);

NUMBER (pgen_patch_data_offset, pgen_patch_data, no_alt,
	     OBJ(int,6), 0, -1, "Offset at which to place data");

KEYWORD_ID (pgen_patch_insert, pgen_patch_data_offset, pgen_patch_eol,
	    OBJ(int,5), PGEN_EDINS,
	    "insert",
	    "Insert data at specified offset (must be last on line)",
	    PRIV_ROOT);

KEYWORD_ID (pgen_patch_byte, pgen_patch_data_offset, pgen_patch_insert,
	    OBJ(int,5), PGEN_EDBYTE,
	    "byte",
	    "Overwrite data at specified offset (must be last on line)",
	    PRIV_ROOT);

/* "delete" - takes an offset and a length */
NUMBER (pgen_patch_del_len, pgen_patch_length, no_alt,
	OBJ(int,7), 0, -1, "Number of bytes to delete");

NUMBER (pgen_patch_del_offset, pgen_patch_del_len, no_alt,
	     OBJ(int,6), 0, -1, "Offset at which to start deleting");

KEYWORD_ID (pgen_patch_delete, pgen_patch_del_offset, pgen_patch_byte,
	    OBJ(int,5), PGEN_EDDEL,
	    "delete", "Delete data at specified offset", PRIV_ROOT);

/* "name" */
STRING (pgen_patch_name_str, pgen_patch_length, no_alt,
	OBJ(string,3), "New packet name; alphanumeric, must start with alpha");

KEYWORD (pgen_patch_name, pgen_patch_name_str, pgen_patch_delete,
	 "name", "Give packet a name", PRIV_ROOT);

/* common_str_interface */
INTERFACE (pgen_patch_int_name, pgen_patch_length, no_alt,
	   OBJ(idb,1), IFTYPE_HWIDB);

KEYWORD (pgen_patch_int, pgen_patch_int_name, pgen_patch_name,
	 common_str_interface, "Assign an output interface for packet", PRIV_ROOT);

/* "time". Note number range is restricted to avoid user typing the
 * "magic" -1 value.
 */
NUMBER (pgen_patch_time_val, pgen_patch_length, no_alt,
	OBJ(int,4), 0, -2,
	"Time to send in usecs relative to replay start (usually ~4ms res.)");

KEYWORD (pgen_patch_time, pgen_patch_time_val, pgen_patch_int,
	 "time", "Set packet's time-to-send field", PRIV_ROOT);

/* "interval". Again, -1 has special meaning. */
NUMBER (pgen_patch_interval_val, pgen_patch_length, no_alt,
	OBJ(int,10), 0, -2, "Nanosecond interval between packets");

KEYWORD (pgen_patch_interval, pgen_patch_interval_val, pgen_patch_time,
	 "interval", "Set packet's interval count", PRIV_ROOT);

/* "repeat". Again, -1 has special meaning. */
NUMBER (pgen_patch_repeat_val, pgen_patch_length, no_alt,
	OBJ(int,9), 0, -2, "Number of times to repeat packet");

KEYWORD (pgen_patch_repeat, pgen_patch_repeat_val, pgen_patch_interval,
	 "repeat", "Set packet's repeat count", PRIV_ROOT);

/* "length". Once again, -1 would mean no keyword encountered. */

NUMBER (pgen_patch_length_val, pgen_patch_length, no_alt,
	OBJ(int,8), 0, -2, "Length to which to truncate/extend packet");

KEYWORD (pgen_patch_length, pgen_patch_length_val, pgen_patch_repeat,
	 "length", "Force packet's length to a set value", PRIV_ROOT);

/* The first argument to "add" or "patch" is a packet number, name,
 * or hyphenated range (either member of which may be either a number
 * or a name). The following parses it out.
 */
PRANGE (pgen_patch_rng, pgen_patch_length, no_alt,
	OBJ(string,1), OBJ(string,2));

/* Actions below set up default (or "unparsed") values for objects that
 * get set up by the various keywords. Note that we don't mess with int 1,
 * which got set up by our initial keyword.
 */
SET (pgen_patch_init_int10, pgen_patch_rng, OBJ(int,10), -1);
SET (pgen_patch_init_int9, pgen_patch_init_int10, OBJ(int,9), -1);
SET (pgen_patch_init_int8, pgen_patch_init_int9, OBJ(int,8), -1);
SET (pgen_patch_init_int7, pgen_patch_init_int8, OBJ(int,7), 0);
SET (pgen_patch_init_int6, pgen_patch_init_int7, OBJ(int,6), -1);
SET (pgen_patch_init_int5, pgen_patch_init_int6, OBJ(int,5), PGEN_EDNONE);
SET (pgen_patch_init_int4, pgen_patch_init_int5, OBJ(int,4), -1);
SET (pgen_patch_init_int3, pgen_patch_init_int4, OBJ(int,3), -1);
SET (pgen_patch_init_int2, pgen_patch_init_int3, OBJ(int,2), -1);
SET (pgen_patch_init_idb1, pgen_patch_init_int2, OBJ(idb,1), NULL);

KEYWORD_ID (pgen_patch, pgen_patch_init_idb1, pgen_version,
	    OBJ(int,1), PGEN_PATCH,
	    "patch", "Modify an existing packet or packets", PRIV_ROOT);
KEYWORD_ID (pgen_add, pgen_patch_init_idb1, pgen_patch,
	    OBJ(int,1), PGEN_ADD,
	    "add", "Create/initialize a new packet or packets", PRIV_ROOT);

/******************************************************************
 * clear <range>
 *
 *  OBJ(string,1) = name/number of first packet in range
 *  OBJ(string,2) = name/number of second packet in range
 */

EOLS (pgen_clear_eol, pgen_commands, PGEN_CLEAR);
PRANGE (pgen_clear_rng, pgen_clear_eol, no_alt,
	OBJ(string,1), OBJ(string,2));
KEYWORD (pgen_clear, pgen_clear_rng, pgen_add,
	 "clear", "Delete a packet or packets", PRIV_ROOT);

/******************************************************************
 * count {on|off|status} [<interface>]
 *
 *  OBJ(string,1) = Either "on" or "off" to put/remove i/f in count mode
 *  OBJ(idb,2) = Interface to take this action on
 */

EOLS (pgen_count_eol, pgen_commands, PGEN_COUNT);
INTERFACE (pgen_count_int, pgen_count_eol, pgen_count_eol,
	   OBJ(idb,1), IFTYPE_HWIDB);
KEYWORD_ID (pgen_count_status, pgen_count_int, no_alt,
	    OBJ(int,1), PGEN_COUNT_STATUS, 
	    "status", "Print interface mode", PRIV_ROOT);
KEYWORD_ID (pgen_count_off, pgen_count_int, pgen_count_status,
	    OBJ(int,1), PGEN_COUNT_OFF, 
	    "off", "Put interface in normal mode", PRIV_ROOT);
KEYWORD_ID (pgen_count_on, pgen_count_int, pgen_count_off,
	    OBJ(int,1), PGEN_COUNT_ON, 
	    "on", "Put interface in count-only mode", PRIV_ROOT);
KEYWORD (pgen_count, pgen_count_on, pgen_clear,
	 "count", "Turn count-only mode on/off on given interface", PRIV_ROOT);


/******************************************************************
 * examine <range> [first <n>]
 *
 *  OBJ(string,1) = name/number of first packet in range
 *  OBJ(string,2) = name/number of second packet in range
 *  OBJ(int,1) = number of bytes to examine from each packet (-1 for all)
 */

EOLS (pgen_examine_eol, pgen_commands, PGEN_EXAMINE);
NUMBER (pgen_examine_first_val, pgen_examine_eol, no_alt,
	OBJ(int,1), 0, -2, "Number of bytes to display in each packet");
KEYWORD (pgen_examine_first, pgen_examine_first_val, pgen_examine_eol,
	 "first", "Show only part of each packet", PRIV_ROOT);
PRANGE (pgen_examine_rng, pgen_examine_first, no_alt,
	OBJ(string,1), OBJ(string,2));
KEYWORD_ID(pgen_examine, pgen_examine_rng, pgen_count,
	   OBJ(int,1), -1,
	   "examine", "Display packet contents/attributes", PRIV_ROOT);

/******************************************************************
 * interface [<interface>]
 *
 *  OBJ(idb,1) = interface to use
 */

EOLS (pgen_interface_eol, pgen_commands, PGEN_INTERFACE);
INTERFACE (pgen_interface_val, pgen_interface_eol, pgen_interface_eol,
	   OBJ(idb,1), IFTYPE_HWIDB);
KEYWORD (pgen_interface, pgen_interface_val, pgen_examine,
	 common_str_interface, "Set/show default interface for new packets", PRIV_ROOT);


/******************************************************************
 * sort <range>
 *
 *  OBJ(string,1) = name/number of first packet in range
 *  OBJ(string,2) = name/number of second packet in range
 */

EOLS (pgen_sort_eol, pgen_commands, PGEN_SORT);
PRANGE (pgen_sort_rng, pgen_sort_eol, no_alt,
	OBJ(string,1), OBJ(string,2));
KEYWORD (pgen_sort, pgen_sort_rng, pgen_interface,
	 "sort", "Sort packets into time-to-send order", PRIV_ROOT);

/******************************************************************
 * buffer [<size>]
 *
 *  OBJ(int,2) = new size for TFTP buffers; -1 if none given
 */
EOLS (pgen_buffer_eol, pgen_commands, PGEN_BUFFER);
NUMBER (pgen_buffer_val, pgen_buffer_eol, pgen_buffer_eol,
	OBJ(int,2), 0, -2, "New size for TFTP buffers");
KEYWORD_ID(pgen_buffer, pgen_buffer_val, pgen_sort,
	   OBJ(int,2), -1,
	   "buffer", "Set size of RAM buffer for incoming TFTP transfers",
	   PRIV_ROOT);

/******************************************************************
 * dump commands {(tftp|rcp) <filename> <server> | tty} [<range>]
 *
 *  OBJ(int,1) = Code for TFTP, RCP, or TTY dump
 *  OBJ(string,1) = name/number of first packet in range
 *  OBJ(string,2) = name/number of second packet in range
 *  OBJ(string,3) = file name
 *  OBJ(paddr,1) = address of TFTP server
 */
EOLS (pgen_dump_cmds_eol, pgen_commands, PGEN_DUMPCMDS);
PRANGE (pgen_dump_cmds_rng, pgen_dump_cmds_eol, no_alt,
	OBJ(string,1), OBJ(string,2));
IPADDR_NAME (pgen_dump_cmds_tftp_svr, pgen_dump_cmds_rng, no_alt,
	     OBJ(paddr,1), "Name or address of server to dump onto");
STRING (pgen_dump_cmds_tftp_fname, pgen_dump_cmds_tftp_svr, no_alt,
	OBJ(string,3), "File name into which to dump");
KEYWORD_ID (pgen_dump_cmds_rcp, pgen_dump_cmds_tftp_fname, no_alt,
	    OBJ(int,1), PGEN_DMPTRCP,
	    "rcp", "Dump to a file on a server using RCP", PRIV_ROOT);
KEYWORD_ID (pgen_dump_cmds_tftp, pgen_dump_cmds_tftp_fname, pgen_dump_cmds_rcp,
	    OBJ(int,1), PGEN_DMPTFTP,
	    "tftp", "Dump to a file on a TFTP server", PRIV_ROOT);
KEYWORD_ID (pgen_dump_cmds_tty, pgen_dump_cmds_rng, pgen_dump_cmds_tftp,
	    OBJ(int,1), PGEN_DMPTTY,
	    "tty", "Dump to controlling TTY (\"examine\" is more readable)",
	    PRIV_ROOT);
KEYWORD (pgen_dump_cmds, pgen_dump_cmds_tty, no_alt,
	 "commands", "Dump as list of commands (your only choice right now)",
	 PRIV_ROOT);
KEYWORD (pgen_dump, pgen_dump_cmds, pgen_buffer,
	 "dump", "Save packets in a form designed for retrieval", PRIV_ROOT);


/******************************************************************
 * load {commands|sniffer} [offset-time <usecs>] {tftp|rcp} <filename> <server>
 *
 *  OBJ(int,1) = 0 for commands, 1 for sniffer
 *  OBJ(int,2) = Code for TFTP, RCP, or TTY dump
 *  OBJ(int,4) = offset time
 *  OBJ(string,3) = filename
 *  OBJ(paddr,1) = address of TFTP server
 */

EOLS (pgen_load_eol, pgen_commands, PGEN_LOAD);
IPADDR_NAME (pgen_load_tftp_svr, pgen_load_eol, no_alt,
	     OBJ(paddr,1), "Name or address of TFTP server to load from");
STRING (pgen_load_tftp_fname, pgen_load_tftp_svr, no_alt,
	OBJ(string,3), "File name from which to load");
KEYWORD_ID (pgen_load_rcp, pgen_load_tftp_fname, no_alt,
         OBJ(int,2), PGEN_DMPTRCP,
	 "rcp", "Load from a file on a server using RCP",
	 PRIV_ROOT);
KEYWORD_ID (pgen_load_tftp, pgen_load_tftp_fname, pgen_load_rcp,
         OBJ(int,2), PGEN_DMPTFTP,
	 "tftp", "Load from a file on a TFTP server",
	 PRIV_ROOT);
NUMBER (pgen_load_offset_val, pgen_load_tftp, no_alt,
	OBJ(int,4), 0, -1,
	"Offset to subtract from time-to-sends in loaded data (uSecs)");
KEYWORD (pgen_load_offset, pgen_load_offset_val, pgen_load_tftp,
	 "offset-time", "Offset time-to-send fields while loading", PRIV_ROOT);
KEYWORD_ID (pgen_load_sniffer, pgen_load_offset, no_alt,
	    OBJ(int,1), PGEN_LDSNIF,
	    "sniffer", "Load a Network General Sniffer format binary file",
	    PRIV_ROOT);
KEYWORD_ID (pgen_load_commands, pgen_load_offset, pgen_load_sniffer,
	    OBJ(int,1), PGEN_LDCMDS,
	    "commands", "Load/execute a text file containing commands",
	    PRIV_ROOT);
KEYWORD (pgen_load, pgen_load_commands, pgen_dump,
	 "load", "Retrieve packets created or saved on a host",
	 PRIV_ROOT);

/******************************************************************
 * replay [<range>] [repeat <count>] [delay <usecs>] [offset-time [<usecs>]]
 *
 *  OBJ(int,4) = offset; -1 for none
 *  OBJ(int,5) = delay; -1 for none
 *  OBJ(int,6) = repeat count; -1 for none
 *  OBJ(string,1) = name of first packet in range if name used to specify
 *  OBJ(string,2) = name of second packet in range if name used to specify
 */

pdecl (pgen_replay_repeat);

EOLS (pgen_replay_eol, pgen_commands, PGEN_REPLAY);
NUMBER (pgen_replay_offset_val, pgen_replay_repeat, no_alt,
	OBJ(int,4), 0, -2,
	"Offset to subtract from time-to-send fields while sending (uSecs)");
KEYWORD (pgen_replay_offset, pgen_replay_offset_val, pgen_replay_eol,
	 "offset-time", "Offset time-to-send fields while sending", PRIV_ROOT);
NUMBER (pgen_replay_delay_val, pgen_replay_repeat, no_alt,
	OBJ(int,5), 0, -2, "Time to delay between repeated replays (uSecs)");
KEYWORD (pgen_replay_delay, pgen_replay_delay_val, pgen_replay_offset,
	 "delay", "Delay between repeated replays", PRIV_ROOT);
NUMBER (pgen_replay_repeat_val, pgen_replay_repeat, no_alt,
	OBJ(int,6), 0, -2, "Number of times to repeat replay");
KEYWORD (pgen_replay_repeat, pgen_replay_repeat_val, pgen_replay_delay,
	 "repeat", "Repeated replay", PRIV_ROOT);
PRANGE (pgen_replay_rng, pgen_replay_repeat, no_alt,
	OBJ(string,1), OBJ(string,2));
SET (pgen_replay_init_int6, pgen_replay_rng, OBJ(int,6), -1);
SET (pgen_replay_init_int5, pgen_replay_init_int6, OBJ(int,5), -1);
SET (pgen_replay_init_int4, pgen_replay_init_int5, OBJ(int,4), -1);
KEYWORD (pgen_replay, pgen_replay_init_int4, pgen_load,
	 "replay", "Send packets from the workspace onto the wire(s)",
	 PRIV_ROOT);

/******************************************************************
 * quit [clear]
 *
 *  OBJ(boolean,1) = 1 if clear specified, 0 otherwise
 */
EOLS (pgen_quit_eol, pgen_commands, PGEN_QUIT);
KEYWORD_ID (pgen_quit_val, pgen_quit_eol, pgen_quit_eol,
         OBJ(int,1), TRUE, "clear", "Free workspace on exit", PRIV_ROOT);
KEYWORD (pgen_quit, pgen_quit_val, pgen_replay,
	 "quit", "Exit the packet generator", PRIV_ROOT);

/* Glue this file to the next (if any) */
#undef ALTERNATE
#define ALTERNATE pgen_quit
