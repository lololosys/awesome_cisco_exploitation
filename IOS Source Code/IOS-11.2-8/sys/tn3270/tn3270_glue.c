/* $Id: tn3270_glue.c,v 3.9.6.5 1996/08/28 13:17:40 thille Exp $
 * $Source: /release/112/cvs/Xsys/tn3270/tn3270_glue.c,v $
 *------------------------------------------------------------------
 * T N 3 2 7 0 _ G L U E . C
 *
 * July, 1994, Bill Palter
 * 
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: tn3270_glue.c,v $
 * Revision 3.9.6.5  1996/08/28  13:17:40  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.9.6.4  1996/06/17  23:38:25  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.9.6.3  1996/03/28  21:02:11  irfan
 * CSCdi12246:  Customer wants visible cursor when in insert mode (7171
 * compatib.)
 * Branch: California_branch
 * Enable code and add default termcap strings for insert mode cursor
 *
 * Revision 3.9.6.2  1996/03/27  23:28:53  irfan
 * CSCdi51821:  tn3270 should allow typeahead
 * Branch: California_branch
 * Add typeahead buffer to hold keyboard input while keyboard appears
 * locked. Add a TN3270 UI knob to control this behaviour. Reduce the
 * amount
 * of whitespace painted
 *
 * Revision 3.9.6.1  1996/03/18  22:19:08  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.3  1996/03/16  07:44:08  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.2.2  1996/03/07  10:54:11  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.2.1  1996/02/20  21:22:48  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/03/07  00:37:35  irfan
 * CSCdi48513:  TN3270 sends excessive cursor-move strings
 * Add "[no] tn3270 optimize-cursor-move" & "[no] tn3270 status-message".
 *
 * Revision 3.8  1996/02/07  16:16:10  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.7  1996/02/01  06:10:48  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6  1996/01/25  11:05:59  irfan
 * CSCdi47499:  memory leaks in tn3270
 *
 * Revision 3.5  1996/01/23  09:05:21  irfan
 * CSCdi47316:  possible memory corruption in tn3270
 * Add an error message for the overflow case.
 *
 * Revision 3.4  1996/01/23  07:55:10  irfan
 * CSCdi47316:  possible memory corruption in tn3270
 * tn3270 copies data from telnet w/o bounds checking onto internal
 * bufferes. Force upper bounds.
 *
 * Revision 3.3  1996/01/17  00:57:15  vandys
 * CSCdi44586:  tn3270 crash when using wide terminals
 * Allocate buffer of appropriate size
 *
 * Revision 3.2  1995/11/17  18:51:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:37:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:08:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../tn3270/msg_tn3270.c"	/* Not a typo, see logger.h */
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <ciscolib.h>
#include "connect.h"
#include "ttysrv.h"
#include "../tcp/tcp.h"
#include "subsys.h"
#include "registry.h"
#include "../h/sys_registry.regh"
#include "../h/cs_registry.regh"
#include "config.h"
#include "parser.h"
#include "parser_defs_tn3270.h"

#define GLUE

#include "tn3270.h"			/* Defines CISCO */
#include "proto.h"
#include "vt_interface.h"
#include "3270.h"

#include "tn3270_private.h"		/* Needs defines from vt_interface */
#include "../termcap/termcap_public.h"

/*
 * Global variables...
 */

boolean tn3270_use_8bit_display;	  /* Use 8bit display */
boolean tn3270_use_8bit_transparent_mode; /* " 8bit transparent mode? */
boolean tn3270_use_extended_term;         /* " extended terminal type */
boolean tn3270_use_3270_null_processing;  /* " 3270 style null proc? */
boolean tn3270_require_reset_key;         /* Require reset on error? */
static const boolean tn3270_character_map_debug = FALSE;  /* Never set */
boolean tn3270_optimize_position;         /* Optimize Cursor-move strings */
boolean tn3270_status_msg;                /* output status msgs */
boolean tn3270_typeahead;                 /* Typeahead during screen lock */


tty_3270def    *keymap_list;		/* List of keymap definitions */
tty_3270def    *termcap_list;		/* List of termcap definitions */



static char *tn3270_terminal_type(tt_soc * tty)
{

    short           term_width;
    short           term_height;
    boolean         extended = tn3270_use_extended_term;

    term_width = tty->tty_width;
    term_height = tty->tty_length;

    if ((term_width >= 132) && (term_height >= 27)) {
	if (extended)
	    return ("IBM-3278-5-E");
	else
	    return ("IBM-3278-5");
    }
    if ((term_height >= 43) && (term_width >= 80)) {
	if (extended)
	    return ("IBM-3278-4-E");
	else
	    return ("IBM-3278-4");
    }
    if ((term_height >= 32) && (term_width >= 80)) {
	if (extended)
	    return ("IBM-3278-3-E");
	else
	    return ("IBM-3278-3");
    }
    /*
     * Default to 24x80 regardless of how terminal is set.
     */
    if (extended)
	return ("IBM-3278-2-E");
    else
	return ("IBM-3278-2");
}




/*
 * EBCDIC=>ASCII translation stuff
 */

static const unsigned char default_ebcdic_to_ascii_table[256] = {
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', '~', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\\', '.', '<', '(', '+', '|',
    '&', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '!', '$', '*', ')', ';', '^',
    '-', '/', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '|', ',', '%', '_', '>', '?',
    ' ', '^', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '`', ':', '#', '@', '\'', '=', '"',
    ' ', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', '~', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', ' ', ' ', ' ', '[', ' ', ' ',
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ']', ' ', ' ',
    '{', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', ' ', ' ', ' ', ' ', ' ', ' ',
    '}', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', ' ', ' ', ' ', ' ', ' ', ' ',
    '\\', ' ', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ' ', ' ', ' ', ' ', ' ', ' ',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ' ', ' ', ' ', ' ', ' ', ' ',
};

static unsigned char ebcdic_to_ascii_table[256] = {
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', '~', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\\', '.', '<', '(', '+', '|',
    '&', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '!', '$', '*', ')', ';', '^',
    '-', '/', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '|', ',', '%', '_', '>', '?',
    ' ', '^', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '`', ':', '#', '@', '\'', '=', '"',
    ' ', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', '~', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', ' ', ' ', ' ', '[', ' ', ' ',
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ']', ' ', ' ',
    '{', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', ' ', ' ', ' ', ' ', ' ', ' ',
    '}', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', ' ', ' ', ' ', ' ', ' ', ' ',
    '\\', ' ', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ' ', ' ', ' ', ' ', ' ', ' ',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ' ', ' ', ' ', ' ', ' ', ' ',
};

static void print_ebcdic_mapping (int ebc)

{
    printf("EBCDIC 0x%x => 0x%x ASCII \n", ebc,
	   ebcdic_to_ascii_table[ebc]); 
}

void tn3270_char_map_command (parseinfo *csb)

{
    int idx;

/*
 * If we're doing NV generation, do it and get out.
 */
    if (csb->nvgen) {
	for (idx = 0; idx < 255; idx++)
	    if (ebcdic_to_ascii_table[idx] != 
		default_ebcdic_to_ascii_table[idx])
		nv_write(TRUE, "%s 0x%x 0x%x", csb->nv_command, idx, 
			 ebcdic_to_ascii_table[idx]);
	return;
    }

/*
 * If we are doing a map, do it.  Otherwise either unset one or unset all.
 */
    if (csb->sense) {
	ebcdic_to_ascii_table[GETOBJ(int, 1)] = GETOBJ(int, 2);
	if (tn3270_character_map_debug)
	    buginf("\ntn3270 map:  Map EBCDIC 0x%x => ASCII 0x%x\n",
		   GETOBJ(int, 1), GETOBJ(int, 2));
    } else {
	if (GETOBJ(int, 3) == TRUE) { /* unmap all */
	    for (idx = 0; idx < 255; idx++)
		ebcdic_to_ascii_table[idx] =
		    default_ebcdic_to_ascii_table[idx];
	    if (tn3270_character_map_debug)
		buginf("\ntn3270 map:  Unmap all non-standard mappings\n");
	} else {
	    idx = GETOBJ(int, 1);
	    ebcdic_to_ascii_table[idx] = default_ebcdic_to_ascii_table[idx];
	    if (tn3270_character_map_debug)
		buginf("\ntn3270 map:  Unmap EBCDIC 0x%x\n", GETOBJ(int, 1));
	}
    }
}

void tn3270_show_command(parseinfo * csb)
{
    int idx = 0;

    switch (GETOBJ(int, 3)) {

      case TN3270_SHOW_CHARMAP:

	if (GETOBJ(int, 2) == TRUE) {
	    for (idx = 0; idx < 255; idx++)
		if (ebcdic_to_ascii_table[idx] != 
		    default_ebcdic_to_ascii_table[idx])
		    print_ebcdic_mapping(idx);
	} else
	    print_ebcdic_mapping(GETOBJ(int, 1));
	break;

      case TN3270_SHOW_ASCIIVAL:
	printf("\nPress key> ");
	flush();
	idx = getcwait(stdio);
	if (isprint(idx)) {
	    printf("%c", idx);
	} else
	    printf("not printable");
	printf(" - hexadecimal value is 0x%x.", idx);
	break;

      default:
	break;
    }
}

void tn3270_config_command(parseinfo *csb)
{
    /*
     * If we're doing NV generation, do it and get out.
     */
    if (csb->nvgen) {
	switch (csb->which) {
	  case TN3270_8_BIT_CMD:
	    if (GETOBJ(int, 1)) {
		nv_write(tn3270_use_8bit_display, "%s", csb->nv_command);
	    } else if (GETOBJ(int, 2)) {
		nv_write(tn3270_use_8bit_transparent_mode,
			 "%s", csb->nv_command);
	    }
	    break;
	  case TN3270_DATASTREAM:
	    nv_write(tn3270_use_extended_term, "%s", csb->nv_command);
	    break;
	  case TN3270_NULL_PROCESSING:
	    nv_write(tn3270_use_3270_null_processing, "%s", csb->nv_command);
	    break;
	  case TN3270_RESET_REQUIRED:
	    nv_write(tn3270_require_reset_key, "%s", csb->nv_command);
	    break;
	  case TN3270_OPTIMIZE_POSITION:
	    nv_write(tn3270_optimize_position, "%s", csb->nv_command);
	    break;
	  case TN3270_STATUS_MSG:
	    nv_write((!tn3270_status_msg), "no %s", csb->nv_command);
	    break;
          case TN3270_TYPEAHEAD:
	    nv_write(tn3270_typeahead, "%s", csb->nv_command);
	    break;
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    /*
     * If they specified non-standard option, then turn boolean on or
     * off per csb->sense.  If they specified default option, then
     * turn off boolean if they didn't say "no".
     */

    switch (csb->which) {
      case TN3270_8_BIT_CMD:
	if (GETOBJ(int, 1)) {
	    tn3270_use_8bit_display = csb->sense;
	} else if (GETOBJ(int, 2)) {
	    tn3270_use_8bit_transparent_mode = csb->sense;
	}
	break;
      case TN3270_DATASTREAM:
	if (GETOBJ(int, 1)) 
	    tn3270_use_extended_term = csb->sense;
	else {
	    tn3270_use_extended_term = FALSE;
	}	    
	break;
      case TN3270_NULL_PROCESSING:
	if (GETOBJ(int, 1)) 
	    tn3270_use_3270_null_processing = csb->sense;
	else {
	    tn3270_use_3270_null_processing = FALSE;
	}
	break;
      case TN3270_RESET_REQUIRED:
	tn3270_require_reset_key = csb->sense;
	break;
      case TN3270_OPTIMIZE_POSITION:
	tn3270_optimize_position = csb->sense;
	break;
      case TN3270_STATUS_MSG:
	tn3270_status_msg = csb->set_to_default || csb->sense;
	break;
      case TN3270_TYPEAHEAD:
	tn3270_typeahead = csb->sense;
	break;
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}


static void _putchar(char c, conntype *conn)
{
    char s[2];

    s[0]=c;
    s[1]=0;

#   ifdef DEBUG
    ttyprintf(CONTTY, "0x%x \n",c);
#   endif
    if (c == '\n') {
	putc(conn->tty, '\r');
	putc(conn->tty, c);
    } else
	putc(conn->tty, c);
}

static int tn3270_conn_init1(conntype * conn)
{
    struct vtinfo_type *vtinfo;
    ScreenPtr       screen;
    char           *tb_ptr;
    int             status;
    char           *tbuf_ptr;
    char           *terminal_type = 0;
    short           term_width;
    short           term_height;


    conn->proto.tn.termcapdata = memallocTermcap();

    if (!conn->proto.tn.termcapdata)
	return 0;

    term_width = conn->tty->tty_width;
    term_height = conn->tty->tty_length;

    if ((term_width >= 132) && (term_height >= 27))
	screen = allocate_screen(TN3270_MOD5);
    else if ((term_height >= 43) && (term_width >= 80))
	screen = allocate_screen(TN3270_MOD4);
    else if ((term_height >= 32) && (term_width >= 80))
	screen = allocate_screen(TN3270_MOD3);
    else
	screen = allocate_screen(TN3270_MOD2);



    if (!screen) {
	tn3270_debug(0, 0, "malloc for screen failed\n");
	freeTermcap(conn->proto.tn.termcapdata);
	conn->proto.tn.termcapdata = NULL;
	return 0;
    }

    if (tn3270_require_reset_key)
        screen->pref_reset = OFF;	/* Set the auto reset feature off */

    if (tn3270_use_3270_null_processing)
        screen->yale_improved_null = YALE_OFF; /* Set the 7171 null mode off */

    conn->proto.tn.tn3270data = screen;
    screen->socket = conn;

    (void *) screen->clear_screen = (void *) vt_clear_screen;
    (void *) screen->change_size = (void *) vt_change_size;
    (void *) screen->put_chars = (void *) vt_put_chars;
    (void *) screen->set_cursor_location = (void *) vt_set_cursor_location;
    (void *) screen->ring_bell = (void *) vt_ring_bell;
    (void *) screen->status_msg = (void *) vt_status_msg;
    (void *) screen->trans_write = (void *) vt_trans_write;
    (void *) screen->trans_mode = (void *) vt_trans_mode;
    (void *) screen->erase_line = (void *) vt_erase_line;


    screen->interface_data = malloc(sizeof(struct vtinfo_type));

    if (!screen->interface_data) {
	tn3270_debug(0, 0, "Interface data malloc failed\n");
	return 0;
    }
    vtinfo = (struct vtinfo_type *) screen->interface_data;


    (struct vtinfo_type *) vtinfo->connection = conn;

    init_keymap(vtinfo);

    vtinfo->input_buffer_ptr = vtinfo->input_buffer;


    terminal_type = conn->tty->termtype ? conn->tty->termtype : "default";
    if (terminal_type[0] == '\000')
	terminal_type = "default";

    tb_ptr = vtinfo->termcap_buffer;
    status = Ctgetent(&tb_ptr, terminal_type, conn, TRUE);

    if (status == 0) {
	printf("Invalid terminal type %s\n", terminal_type);
	return 0;
    }
    tbuf_ptr = vtinfo->tc_strings;
    vtinfo->cm = Ctgetstr("cm", &tbuf_ptr, conn);
    vtinfo->cl = Ctgetstr("cl", &tbuf_ptr, conn);
    vtinfo->me = Ctgetstr("me", &tbuf_ptr, conn);
    vtinfo->md = Ctgetstr("md", &tbuf_ptr, conn);
    vtinfo->us = Ctgetstr("us", &tbuf_ptr, conn);
    vtinfo->mb = Ctgetstr("mb", &tbuf_ptr, conn);
    vtinfo->mr = Ctgetstr("mr", &tbuf_ptr, conn);
    vtinfo->x0 = Ctgetstr("x0", &tbuf_ptr, conn);
    vtinfo->x1 = Ctgetstr("x1", &tbuf_ptr, conn);
    vtinfo->x2 = Ctgetstr("x2", &tbuf_ptr, conn);
    vtinfo->x3 = Ctgetstr("x3", &tbuf_ptr, conn);
    vtinfo->x4 = Ctgetstr("x4", &tbuf_ptr, conn);
    vtinfo->x5 = Ctgetstr("x5", &tbuf_ptr, conn);
    vtinfo->x6 = Ctgetstr("x6", &tbuf_ptr, conn);
    vtinfo->x7 = Ctgetstr("x7", &tbuf_ptr, conn);
    vtinfo->is = Ctgetstr("is", &tbuf_ptr, conn);
    vtinfo->rs = Ctgetstr("rs", &tbuf_ptr, conn);
    vtinfo->ti = Ctgetstr("ti", &tbuf_ptr, conn);
    vtinfo->vs = Ctgetstr("vs", &tbuf_ptr, conn);
    vtinfo->te = Ctgetstr("te", &tbuf_ptr, conn);
    vtinfo->ve = Ctgetstr("ve", &tbuf_ptr, conn);
    vtinfo->ce = Ctgetstr("ce", &tbuf_ptr, conn);
    if (vtinfo->ce && strlen(vtinfo->ce))
	screen->use_clear_to_end_of_line = TRUE;
    else
	screen->use_clear_to_end_of_line = FALSE;

    vtinfo->transparent_mode = TRANS_OFF;
    vtinfo->do_status_msg = tn3270_status_msg;
    vtinfo->optimize_position = tn3270_optimize_position;
    vtinfo->current_cursor_location = BAD_SCREEN_COORDINATES;
    vtinfo->cache_highlight = -1;
    vtinfo->cache_foreground_color = -1;

    conn->proto.tn.tn3270inited = TRUE;

    return(TRUE);
}



static void tn3270_conn_init2(conntype * conn)
{
    ScreenPtr screen = conn->proto.tn.tn3270data;
    struct vtinfo_type *vtinfo = (struct vtinfo_type *)screen->interface_data; 

    Ctputs(vtinfo->rs, 0, _putchar, vtinfo->connection);
    Ctputs(vtinfo->is, 0, _putchar, vtinfo->connection);
    Ctputs(vtinfo->ti, 0, _putchar, vtinfo->connection);
    /*  Ctputs(vtinfo->vs, 0, _putchar, vtinfo->connection);   */

    /* Dont much with the cursor here*/
    Ctputs(vtinfo->ve, 0, _putchar, vtinfo->connection); /* Make the cursor
                                                          * appear normal */

    return;
}

static void tn3270_conn_terminate(conntype * conn, boolean DoScreen)
{
    ScreenPtr screen = conn->proto.tn.tn3270data;
    struct vtinfo_type *vtinfo;



    if (screen) {
	
	 field_delete_all (screen);
          
	if (screen->interface_data) {
	    vtinfo = (struct vtinfo_type *) screen->interface_data;
	    /* Reset the attribute */
	    if (vtinfo->me)
		Ctputs(vtinfo->me, 0, _putchar, vtinfo->connection);
	    Ctputs(vtinfo->ve, 0, _putchar, vtinfo->connection);
	    Ctputs(vtinfo->te, 0, _putchar, vtinfo->connection);
	    FreeNode(vtinfo->head);
	}
	free(screen->interface_data);
	deallocate_screen(screen);
    }
    if (conn->proto.tn.termcapdata)
	freeTermcap(conn->proto.tn.termcapdata);

    return;
}



static void tn3270_conn_resume(conntype * conn)
{
    ScreenPtr           screen = conn->proto.tn.tn3270data;
    struct vtinfo_type *vtinfo = (struct vtinfo_type *) screen->interface_data;

    vtinfo->cache_foreground_color = -1;
    vtinfo->cache_highlight = -1;


    /*
     * force automore off
     */
    Ctputs(vtinfo->is, 0, _putchar, vtinfo->connection);
    tn3270_redraw(screen, 0);
    return;


}


static void tn_puts (struct vtinfo_type * vtinfo, const uchar *string)
{

    boolean cursor_pos_good = TRUE;
    uchar c;

    while ((c = *string++) != '\0') {      /* have characters ? */
	if (cursor_pos_good) {             /* still keeping track ? */
	    if (c >= ' ') {                /* legible ascii ? */
		vtinfo->current_cursor_location++;  /* move cursor */
	    } else {
                /*
		 * Control characters seen. Reset track of position
		 */
		vtinfo->current_cursor_location = BAD_SCREEN_COORDINATES;
		cursor_pos_good = FALSE;
	    }
	}
	putc((vtinfo->connection)->tty, c);
    }
}



static void tn3270_network_data(uchar * Buffer, int count, int control,
			 conntype * conn) 
{
    ScreenPtr       screen = conn->proto.tn.tn3270data;
    struct vtinfo_type *vtinfo = (struct vtinfo_type *) screen->interface_data;
    int             input_room;

    /*
     * establish and enforce an upper bound to this write
     */
    input_room = SCREEN_INPUT_BUFF_SIZE - (vtinfo->input_buffer_ptr
					   - vtinfo->input_buffer);
    if (count > input_room) {
	count = input_room;
	errmsg(&msgsym(INP_OVF1, TN3270));
    }
    bcopy(Buffer, vtinfo->input_buffer_ptr, count);
    vtinfo->input_buffer_ptr += count;
}



static void tn3270_end_of_packet (conntype * conn)
{
    struct packet_type packet;
    ScreenPtr       screen = conn->proto.tn.tn3270data;
    struct vtinfo_type *vtinfo = (struct vtinfo_type *)screen->interface_data; 

    packet.data = vtinfo->input_buffer;
    packet.exec = packet.data;
    packet.len = vtinfo->input_buffer_ptr - vtinfo->input_buffer;
    packet.field_data = packet.data;
    packet.field_len = packet.len;
    tn3270_packet(screen, &packet);

    vtinfo->input_buffer_ptr = vtinfo->input_buffer;

    return;
}



static void tn3270_user_keypress(char ch, conntype * conn)
{
    ScreenPtr       screen = conn->proto.tn.tn3270data;
    struct vtinfo_type *vtinfo;
    if (!screen->interface_data)
    	return;
    vtinfo  = (struct vtinfo_type *)screen->interface_data; 

    /*
     * Allow cm to be sent for control characters
     * Because some applications use Ctrl chars to
     *   resynchronize and expect a fresh cm string.
     */
    if (ch < ' ') {
	vtinfo->current_cursor_location = BAD_SCREEN_COORDINATES;
    }
    if (vtinfo->transparent_mode == TRANS_READ)
    	tn3270_trans_keystroke (screen,ch);
    else	
        tt_keystroke(screen, (unsigned char) ch);
    return;
}




void fatal(int x)
{
    tn3270_debug(0, 0, "\nTN3270 Datastream error\n");
}


int send(conntype * socket, unsigned char *string, int length,
	 unsigned int flags) 
{
    unsigned char   ch;
    tcbtype        *tcb;
    int             stat;

    tcb = (tcbtype *) socket->tcb;

    while (length) {
	ch = *string;
	stat = tcp_putbyte(tcb, ch, TRUE);

	if (!stat) {
	    tn3270_debug(0, 0, "Error Writing, %d bytes remaining\n", length);
	    return 0;
	}
	length--;
	string++;
    }
    tcp_putpush(tcb, TRUE, FALSE);

    return 1;
}




void vt_clear_screen(struct vtinfo_type * vtinfo)
{
    vtinfo->screen_reversed = 0;
    if (vtinfo->screen_clear == VT_SCREEN_DIRTY) {
	memset(vtinfo->text, ' ', sizeof(vtinfo->text));
	memset(vtinfo->attribute, 0, sizeof(vtinfo->attribute));
	vtinfo->screen_clear = VT_SCREEN_CLEAR;
    }
    Ctputs(vtinfo->cl, 0, _putchar, vtinfo->connection);
}



void vt_ring_bell(struct vtinfo_type * vtinfo)
{
    unsigned char   bell_seq[] = "\007";

    tn_puts(vtinfo, bell_seq);
}

void vt_change_size(struct vtinfo_type * vtinfo,
		    unsigned short new_width,
		    unsigned short new_height,
		    unsigned short new_cell_x,
		    unsigned short new_cell_y)
{
    unsigned short  height;
    unsigned short  width;

    if (vtinfo->screen_clear == VT_SCREEN_DIRTY) {
	memset(vtinfo->text, ' ', sizeof(vtinfo->text));
	memset(vtinfo->attribute, 0, sizeof(vtinfo->attribute));
	vtinfo->screen_clear = VT_SCREEN_CLEAR;

	Ctputs(vtinfo->cl, 0, _putchar, vtinfo->connection);
    }
    if ((new_width != vtinfo->width) ||
	(new_height != vtinfo->height)) {

	width = new_width * new_cell_x;
	height = new_height * new_cell_y + 4;

	vtinfo->width = new_width;
	vtinfo->height = new_height;
    }
}




void vt_set_cursor_location(struct vtinfo_type * vtinfo,
			    unsigned short address)
{
    int             x, y;
    char           *cp;

    if (vtinfo->width == 0)
	return;

    vtinfo->cursor_location = address;

    /*
     * Attempt to avoid sending cursor move string, if already
     * there. Use current_cursor_location, for this hack. Not
     * sure what other side-effects, the protocol would be using
     * vtinfo->cursor_location.
     */
    if (vtinfo->optimize_position &&
	(address == vtinfo->current_cursor_location)) {
	return;
    }

    y = IADDRESS_TO_ROW(address);
    x = IADDRESS_TO_COL(address);

    cp = Ctgoto(vtinfo->cm, y, x, vtinfo->connection);

    Ctputs(cp, 0, _putchar, vtinfo->connection);

    vtinfo->current_cursor_location = address;
}


void vt_erase_line(struct vtinfo_type * vtinfo, unsigned short
		   address)
{
    unsigned short  x, y;
    char           *cp;

    if ((!vtinfo->optimize_position) ||
	(address != vtinfo->current_cursor_location)) {
	
	y = IADDRESS_TO_ROW(address);
	x = IADDRESS_TO_COL(address);

	cp = Ctgoto(vtinfo->cm, y, x, vtinfo->connection);
	Ctputs(cp, 0, _putchar, vtinfo->connection);
    }

    if (vtinfo->ce)
	Ctputs(vtinfo->ce, 0, _putchar, vtinfo->connection);    

}

void vt_put_chars(struct vtinfo_type * vtinfo, unsigned char *string,
		  int len,
		  unsigned short address,
		  struct interface_attribute_type * attribute)
{
    unsigned short  x, y;
    int             col, row;
    int             i;
    unsigned char  *color_ptr;
    unsigned char   *buffer;
    char           *cp;
    const int slop = 32;

    if (vtinfo->width == 0)
	return;

    /*
     * Get a buffer for the line.  Always get at least 128 bytes;
     * this reflects the old buffer size when it was a stack
     * variable (paranoia).
     */
    i = len + slop;
    if (i < 128) {
	i = 128;
    }
    buffer = malloc_named(i, "tn3270 line");
    if (buffer == NULL) {
	return;
    }

    vtinfo->screen_clear = VT_SCREEN_DIRTY;

    row = IADDRESS_TO_ROW(address);
    col = IADDRESS_TO_COL(address);

    if (vtinfo->screen_reversed)
	row = (vtinfo->width - row) - len;

    memcpy(&vtinfo->text[address], string, len);

    for (i = 0; i < len; i++) {
	vtinfo->attribute[i + address] = *attribute;
	buffer[i] = ebcdic_to_ascii_table[string[i]];
    }

    buffer[len] = 0;

    if (attribute->highlighting & INTERFACE_HIGHLIGHT_INVISIBLE) {
	memset(buffer, ' ', len);
    }

    if ((attribute->highlighting != vtinfo->cache_highlight) ||
	(attribute->foreground_color !=
	 vtinfo->cache_foreground_color)) {

	/* reset the attribute */
	if (vtinfo->me)
	    Ctputs(vtinfo->me, 0, _putchar, vtinfo->connection);

	if (attribute->highlighting & INTERFACE_HIGHLIGHT_REVERSE) {
	    if (vtinfo->mr)
		Ctputs(vtinfo->mr, 0, _putchar, vtinfo->connection);
	}
	if (attribute->highlighting & INTERFACE_HIGHLIGHT_BLINK) {
	    if (vtinfo->mb)
		Ctputs(vtinfo->mb, 0, _putchar, vtinfo->connection);
	}
	if (attribute->highlighting & INTERFACE_HIGHLIGHT_INTENSIFY) {
	    if (vtinfo->md)
		Ctputs(vtinfo->md, 0, _putchar, vtinfo->connection);
	}
	if (attribute->highlighting & INTERFACE_HIGHLIGHT_UNDERSCORE) {
	    if (vtinfo->us)
		Ctputs(vtinfo->us, 0, _putchar, vtinfo->connection);
	}
	
	if ((color_ptr = vt_3270_color_to_vt_color(attribute->foreground_color, 
						   vtinfo)))
	    Ctputs(color_ptr, 1, _putchar, vtinfo->connection);
	vtinfo->cache_highlight = attribute->highlighting;
	vtinfo->cache_foreground_color = attribute->foreground_color;

    }

    /*
     * move cursor if no_optimize   or  if address is
     * not the same as current position.
     */
    if ((!vtinfo->optimize_position) ||
	(address != vtinfo->current_cursor_location)) {

	y = IADDRESS_TO_ROW(address);
	x = IADDRESS_TO_COL(address);

	cp = Ctgoto(vtinfo->cm, y, x, vtinfo->connection);
	Ctputs(cp, 0, _putchar, vtinfo->connection);
    }

    tn_puts(vtinfo, buffer);

    /*
     * Done with the buffer
     */
    free(buffer);
}



char *vt_3270_color_to_vt_color(int color, struct vtinfo_type * vtinfo)
{
    switch (color) {

      case EA_COLOR_BLACK:
      case EA_COLOR_NEUTRAL_BLACK:
	return vtinfo->x0;

      case EA_COLOR_DEEP_BLUE:
      case EA_COLOR_BLUE:
	return vtinfo->x1;

      case EA_COLOR_ORANGE:
      case EA_COLOR_RED:
	return vtinfo->x2;

      case EA_COLOR_PURPLE:
      case EA_COLOR_PINK:
	return vtinfo->x3;

      case EA_COLOR_PALE_GREEN:
      case EA_COLOR_DEFAULT:
      case EA_COLOR_GREEN:
	return vtinfo->x4;

      case EA_COLOR_PALE_TURQUOISE:
      case EA_COLOR_TURQUOISE:
	return vtinfo->x5;

      case EA_COLOR_YELLOW:
	return vtinfo->x6;

      case EA_COLOR_GREY:
      case EA_COLOR_WHITE:
      case EA_COLOR_NEUTRAL_WHITE:
	return vtinfo->x7;

      default:
	return vtinfo->x4;
    };
}











void            tt_keystroke(ScreenPtr screen, unsigned char input)
{
    struct vtinfo_type *vtinfo = (struct vtinfo_type *) screen->interface_data;
    struct state_type *state;


    state = vtinfo->state;

    while (state)
	if ((unsigned char)state->match == input)
	    break;
	else
	    state = NEXTSTATE(state);


    if (state == NULL) {
	/*printf(" illegal keystroke\n");*/
	vtinfo->state = vtinfo->head;
	return;
    }
    if (state->result == STATE_GOTO) {
	vtinfo->state = state->address;
	return;
    }
    tn3270_keystroke(screen, state->result);

    vtinfo->state = vtinfo->head;

}




void            vt_status_msg(struct vtinfo_type * vtinfo, unsigned long msg_type,
			                      unsigned long msg_code)
{

    switch (msg_type) {
	case MSG_CURSOR:
	vt_cursor_msg(vtinfo, msg_code);
	break;

      case MSG_STATUS:
	vt_info_area(vtinfo, msg_code);
	break;
    }
}



void            vt_cursor_msg(struct vtinfo_type * vtinfo, unsigned long msg_code)
{

    switch (msg_code) {

      case CURSOR_NORMAL:
	/*
	 * Make the cursor normal
	 */
	Ctputs(vtinfo->ve, 0, _putchar, vtinfo->connection);
	break;

      case CURSOR_INSERT:
	/*
	 * Make the cursor stand out
	 */
	Ctputs(vtinfo->vs, 0, _putchar, vtinfo->connection);
	break;
    }

}

static const char null_msg[] = "                 ";
static const char prot_field[] = " Protected Field ";
static const char insrt_over[] = " Insert Overflow ";
static const char system_lock[] = "   System Lock   ";


static const char *const info_msgs[] = {null_msg,
    prot_field,
    insrt_over,
    system_lock,
};


void            vt_info_area(struct vtinfo_type * vtinfo, unsigned long msg_code)
{
    unsigned short  x = 24;
    unsigned short  y = 0;
    unsigned short  msg_size;
    unsigned short  i;
    unsigned short  address;
    unsigned short  end_address;
    unsigned short  len;
    char           *cp;

    msg_size = strlen(info_msgs[0]);
    address = (x - 1) * vtinfo->width;
    switch (msg_code) {

      case STATUS_NORMAL:
	if (vtinfo->do_status_msg) {
	    end_address = address + msg_size;
	    i = address;
	    len = 1;
	    while (i < end_address) {
		while (((i + len) < end_address) &&
		       (memcmp(&(vtinfo->attribute[i]),
			       &(vtinfo->attribute[i + len]),
			       sizeof(struct interface_attribute_type)) == 0))
		    len++;
		vt_put_chars(vtinfo, &(vtinfo->text[i]),
			     len, i, &(vtinfo->attribute[i]));
		i += len;
		len = 1;
	    }
	}
	break;
      case STATUS_SLOCK:
      default:
	if (!vtinfo->do_status_msg) {
	    /* NO status messages. Just move the
	     * cursor to down bottom.
	     */
	    cp = Ctgoto(vtinfo->cm, y, x-1, vtinfo->connection);
	    Ctputs(cp, 0, _putchar, vtinfo->connection);
	    vtinfo->current_cursor_location = BAD_SCREEN_COORDINATES;
	    return;
	}
	vtinfo->cache_highlight = -1;
	vtinfo->cache_foreground_color = -1;
	if (vtinfo->me)
	    Ctputs(vtinfo->me, 0, _putchar, vtinfo->connection);
	if (vtinfo->mr)
	    Ctputs(vtinfo->mr, 0, _putchar, vtinfo->connection);
	cp = Ctgoto(vtinfo->cm, y, x-1, vtinfo->connection);
	Ctputs(cp, 0, _putchar, vtinfo->connection);
	tn_puts(vtinfo, info_msgs[msg_code]);
	vtinfo->current_cursor_location = BAD_SCREEN_COORDINATES;
	break;
    }

    vt_set_cursor_location(vtinfo, vtinfo->cursor_location);
}



void vt_trans_write(struct vtinfo_type * vtinfo,
		    unsigned char *buffer, int len)
{
    boolean cursor_pos_good = TRUE;

    while (len) {
	if (cursor_pos_good) {       /* We attempt to keep track */
	    if (*buffer >= ' ') {    /*   of position until Ctrl chars */
		vtinfo->current_cursor_location++;
	    } else {
		vtinfo->current_cursor_location = BAD_SCREEN_COORDINATES;
		cursor_pos_good = FALSE;
	    }
	}
	_putchar (*buffer++,vtinfo->connection);
    	len--;
    }
}

void  vt_trans_mode(struct vtinfo_type * vtinfo, unsigned char flag)
{

    vtinfo->transparent_mode = flag;

}

/*
 * From tn_debug.c
 */

int  tn3270_debug (ScreenPtr screen,
		   unsigned long type,
		   char *format_spec,
		   ...)
{
    va_list         new_list;
    int             stat = 0;
    char            buffer[1024];
    unsigned char  *cp;


    if (type == DEBUG_DATASTREAM)
	return 0;


    va_start(new_list, format_spec);

    stat = _doprnt(buffer, NULL, format_spec, new_list, FALSE);

    va_end(new_list);

    buffer[stat] = '\0';

    cp = buffer;
    while (*cp)
	console_putc(*cp++);

    return (stat);

}


void queue_start_sf (ScreenPtr screen,unsigned char x)
{
     screen->SF_Len_Ptr = (unsigned short *)screen->IBM_Buf_Ptr;      
    *screen->IBM_Buf_Ptr++ = 0;                     
    *screen->IBM_Buf_Ptr++ = 0;                     
     screen->SF_expand = 0;				
     QUEUE_BYTE(x);				        
}

void queue_start_sdp (ScreenPtr screen, unsigned char x)
{

     screen->SF_SDP_Len_Ptr = screen->IBM_Buf_Ptr;   
    *screen->IBM_Buf_Ptr++ = 0;                     
     screen->SF_SDP_Expand_Base = screen->SF_expand;	
     QUEUE_BYTE(x);				        

}

void queue_end_sdp (ScreenPtr screen)
{

     *screen->SF_SDP_Len_Ptr = 	((char *)screen->IBM_Buf_Ptr - 
                                 (char *)screen->SF_SDP_Len_Ptr) - 	
		                ((char *) screen->SF_expand - 
                                 (char *)screen->SF_SDP_Expand_Base);
      screen->SF_SDP_Len_Ptr = (unsigned char *) 0;
}

void queue_end_sf (ScreenPtr screen)
{
    unsigned short length;  

    length =    (((unsigned char *) screen->IBM_Buf_Ptr -   
  		     (unsigned char *) screen->SF_Len_Ptr) -	
                     screen->SF_expand); 				
    *((unsigned char *) screen->SF_Len_Ptr) ++ = (unsigned char) ((length & 0xff00) >> 8);
    *((unsigned char *) screen->SF_Len_Ptr) ++ = (unsigned char) (length &  0x00ff);
    screen->SF_Len_Ptr = 0;					  

}
void queue_flush (ScreenPtr screen)
{

    if (screen->SF_Len_Ptr)                         
        tn3270_debug(screen,DEBUG_BUGS,"Flush needed while in SD\n");
    send ((conntype *) screen->socket,screen->IBM_Buf,		
    screen->IBM_Buf_Ptr-screen->IBM_Buf,0);		
    screen->IBM_Buf_Ptr = screen->IBM_Buf;
}

static void tn3270_print_copyright (void)
{
    printf("TN3270 Emulation software.\n");
}


/*
 * tn3270_init
 * Initialize the global values on startup.
 */

void tn3270_init(subsystype * subsys)
{
    tn3270_use_8bit_display = FALSE;
    tn3270_use_8bit_transparent_mode = FALSE;
    tn3270_use_3270_null_processing = FALSE;
    tn3270_use_extended_term = FALSE;
    tn3270_require_reset_key = FALSE;
    tn3270_optimize_position = FALSE;
    tn3270_status_msg = TRUE;

    /*
     * Initialize the registered functions...
     */

    reg_add_tn3270_conn_init1(tn3270_conn_init1, "tn3270_conn_init1");
    reg_add_tn3270_conn_init2(tn3270_conn_init2, "tn3270_conn_init2");
    reg_add_tn3270_conn_terminate(tn3270_conn_terminate, "tn3270_conn_terminate");
    reg_add_tn3270_user_keypress(tn3270_user_keypress,
				    "tn3270_user_keypress");
    reg_add_tn3270_network_data(tn3270_network_data, "tn3270_network_data");
    reg_add_tn3270_end_of_packet(tn3270_end_of_packet, "tn3270_end_of_packet");
    reg_add_tn3270_conn_resume(tn3270_conn_resume, "tn3270_conn_resume");
    reg_add_tn3270_terminal_type(tn3270_terminal_type,"tn3270_terminal_type");
    reg_add_clear_screen(termcap_clearscreen, "termcap_clearscreen");
    reg_add_print_copyright(tn3270_print_copyright, 
			    "tn3270_print_copyright");

    /*
     * Initialize tn3270 parser support
     */
    tn3270_parser_init();
}

/*
 * TN3270 subsystem header
 */

#define TN3270_MAJVERSION 1
#define TN3270_MINVERSION 0
#define TN3270_EDITVERSION  1

SUBSYS_HEADER(tn3270, TN3270_MAJVERSION, TN3270_MINVERSION, TN3270_EDITVERSION,
	      tn3270_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: iphost, menus",
	      NULL);
