/* $Id: menus.c,v 3.8.10.4 1996/08/15 00:11:27 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ui/menus.c,v $
 *------------------------------------------------------------------
 * Menus.c  - Support admin-definable menus for accessing cisco comamnds.
 *
 * March, 1994  Bill Westfield
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This module imlements a subsystem for allowing thre creation and use
 * of simple menus.  Each menu consists of up to 9 items referenced by
 * a single digit 1 through 9, and a command to execute when that digit
 * is typed.  It was necessary to break apart exec.c to allow the portion
 * that parses commands to be separate from the rest of the exec logic.
 * Access to the menu is through the menu command, so the parser is
 * effectively recursive.
 *------------------------------------------------------------------
 * $Log: menus.c,v $
 * Revision 3.8.10.4  1996/08/15  00:11:27  widmer
 * CSCdi60637:  http://exec/show/whoami/cr page completes after --more--
 * timeout
 * Branch: California_branch
 *
 * Revision 3.8.10.3  1996/08/12  16:09:03  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.8.10.2  1996/05/21  10:07:47  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.8.10.1  1996/03/18  22:29:10  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.3  1996/03/16  07:45:54  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.2.2  1996/03/07  10:58:43  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.2.1  1996/02/20  21:26:31  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/22  14:39:36  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.7  1996/02/01  06:11:20  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6  1996/01/29  07:30:50  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.5  1996/01/18  15:54:37  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.4  1995/12/17  18:40:18  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/12/15  23:04:49  billw
 * CSCdi44869:  default behavior for a <CR> at a menu prompt should be
 * adjustable
 * add "menu <foo> default <n>"
 *
 * Revision 3.2  1995/11/17  17:48:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:41:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/06/28  09:32:12  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.2  1995/06/20  20:11:09  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  23:11:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "subsys.h"
#include "ttysrv.h"
#include "packet.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "stacks.h"
#include "../ui/command.h"
#include "../parser/parser_defs_config.h"
#include "../h/cs_registry.regh"

/*
 * Local definitions
 */
/*
 * Maximum number of items (choices) that can be in a menu
 * You have to change more than just this constant to change this,
 * since digits beyond "9" would have to be picked, and the case
 * statement in the user code explicitly references digits.
 *
 * Increasing this above 9 probably isn't a good idea anyway, no matter
 * how tempting, because it'ss bad user interface design (7 +/- 2, etc)
 */

#define MENU_MAXITEMS 18
#define MENU_MAXDOUBLESPACED 8	    /* max array index - NINE items! */

typedef struct menudata_ {
    struct menudata_ *next;
    NAMEDTHINGMACRO
    char * menu_title;
    char * menu_item[MENU_MAXITEMS];
    char * menu_cmd[MENU_MAXITEMS];
    ushort menu_flags;
    short menu_default;
} menudata;

enum {MENU_COMMAND, MENU_TEXT, MENU_TITLE, MENU_CLEAR, MENU_LINEMODE,
	  MENU_SINGLESPACE, MENU_STATUSLINE, MENU_DEFAULT};

#define MENUF_CLEAR    1		/* Clear screen before drawing */
#define MENUF_STATUS   2		/* Draw line status */
#define MENUF_CONFIRM  4		/* Require RETURN after selections */
#define MENUF_SINGLESPACE 8		/* single space the menus */
/*
 * Local storage
 */

static queuetype menu_q;

/*
 * menu_draw
 * Draw the menu on the screen, clearing the screen beforehand.
 * Each menu item is a numbered line of text  in a double-spaced
 * list.
 */


static void menu_draw (menudata *menu)
{
    int i;

    if (menu->menu_flags & MENUF_CLEAR)
	reg_invoke_clear_screen();
    if (menu->menu_flags & MENUF_STATUS)
	printf("Server \"%s\"    Line %t    Terminal-type %s   %s %s\n",
	       hostname, stdio->ttynum,
	       stdio->termtype ? stdio->termtype : "(unknown)",
	       stdio->keymaptype ? "Keymap":"", stdio->keymaptype);
    printf("%r", menu->menu_title);
    for (i = 0; i < MENU_MAXITEMS; i++)
	if (menu->menu_item[i])
	    printf("%s\n    %d     %r",
		   menu->menu_flags & MENUF_SINGLESPACE ? "" : "\n",
		   i+1, menu->menu_item[i]);
    printf("\n\n");
}

/*
 * menu_command
 * The user "menu <name>" command.
 * Do the work of drawing the menu and interpretting user input as references
 * to the menu instead of normal commands.  Call the parser (recursively) with
 * the specified command from the selected menu command.
 * A ^L character will redraw the menu.  Other illegal characters are ignored
 * and will beep at the user.  No CR is required after typing a selection.
 */

#define MENU_RECURSION_STACKLIMIT 85 /* Only let menus use 15% of stack */

static void menu_command (parseinfo *csb)
{
    int input;
    int error;
    menudata *menu;

    menu = find_namedthing(&menu_q, GETOBJ(string,1));
    if (menu == NULL) {
	printf("\n%% No such menu \"%s\"", GETOBJ(string, 1));
	return;
    }
    if (stack_remaining() < MENU_RECURSION_STACKLIMIT) {
	printf("\n%% Too much menu nesting");
	return;
    }
    menu_draw(menu);
    while (TRUE) {
	if (csb->exit_all || modemcheck(stdio) ||
	    (stdio->statbits & IDLETIMEOUT)) {
            stdio->statbits |= CARDROP; /* drop any open connections */
	    return;
        }
	if (menu->menu_flags & MENUF_CONFIRM) {
	    printf("Selection: ");
	    input = '0' + get_num_in_range(NULL, 10, 1, 18, 0);
	} else {
	    input = getcwait(stdio);
	    switch (input) {
	      case CH_FF:
		input = 0;
		menu_draw(menu);
		continue;
	      case RETURN:
		if (menu->menu_default)
		    input = '0' + menu->menu_default;
		/* Fall through! */
	      case '1':
	      case '2':
	      case '3':
	      case '4':
	      case '5':
	      case '6':
	      case '7':
	      case '8':
	      case '9':
		if (menu->menu_cmd[input - '1'] != NULL)
		    break;
		/* Else fall through */
	      default: 
		input = 0;
		putc(stdio, BELL);
		continue;
	    }
	}
	if (input == '0')
	    input = menu->menu_default + '0';

	if (input > '0' && input <= ('0' + 18)) {
	    if ((int)(menu->menu_cmd[input - '1']) == -1) { /* menu-exit */
		stdio->tty_autocommand = NULL; /* don't hangup! */
		return;
	    }
	    if (menu->menu_cmd[input - '1']) {
		strcpy(csb->line, menu->menu_cmd[input - '1']);
		/*
		 * The following code is copied from exec.c, with
		 * extraneous code that handles config modes and such
		 * removed.  Since we don't have to deal with the
		 * various "special cases" of the main exec (empty lines,
		 * hostnames as commands, etc), things are much simplified.
		 */
		flush();
		csb->flags |= COMMAND_AUTHORIZE;
		if ((error = parse_cmd(csb)) != 0) {
		    printf("\nMenu \"%s\" has invalid command \"%s\"",
			   menu->name, menu->menu_cmd[input-'1']);
		    report_error(csb, error, TRUE);
		    csb->mode = exec_mode;
		    printf("\n--More--");
		    (void) getcwait(stdio);
		}
	    }
	    if (csb->exit_all || modemcheck(stdio))
		return;
	    menu_draw(menu);
	} else if (input == '0')
	    menu_draw(menu);
    }
}

/*
 * menu_cfgcommand
 * The "menu" config command implementation.
 * create and destory menus, save the menus to NVM, etc.
 * Because of the complexity of the menu command, it isn't possible
 * for the parser to automatically generate the config text, so we
 * do it in the old style.  Use caution when changing commands!
 */
static void menu_cfgcommand (parseinfo *csb)
{
    menudata *menu;
    int i;
    char *string = NULL;

    if (csb->nvgen) {
	boolean autosinglespace = FALSE;
	for (menu = (menudata *) menu_q.qhead; menu; menu = menu->next) {
	    nv_write(TRUE,"!");
	    nv_write((boolean) menu->menu_title,
		     "menu %s title \003%s\003", menu->name,
		     menu->menu_title);
	    for (i=0; i < MENU_MAXITEMS; i++) {
		if (i > MENU_MAXDOUBLESPACED && menu->menu_item[i])
		    autosinglespace = TRUE;
		nv_write(menu->menu_item[i] != NULL, "menu %s text %d %s",
			 menu->name, i+1, menu->menu_item[i]);
		if (menu->menu_cmd[i] == (char *)-1)
		    nv_write(TRUE, "menu %s command %d menu-exit",
			     menu->name, i+1);
		else
		    nv_write(menu->menu_cmd[i] != NULL,
			     "menu %s command %d %s",
			     menu->name, i+1, menu->menu_cmd[i]);
	    }
	    nv_write(menu->menu_flags & MENUF_CLEAR,
		     "menu %s clear-screen", menu->name);
	    nv_write(menu->menu_flags & MENUF_STATUS,
		     "menu %s status-line", menu->name);
	    nv_write(menu->menu_default, "menu %s default %d", menu->name,
		     menu->menu_default);
	    if (autosinglespace == FALSE) {
		nv_write(menu->menu_flags & MENUF_CONFIRM, "menu %s line-mode",
			 menu->name);
		nv_write(menu->menu_flags & MENUF_SINGLESPACE,
			 "menu %s single-space", menu->name);
	    }
	}
	return;
    }
    menu = find_namedthing(&menu_q, GETOBJ(string,2));
    if (csb->sense == FALSE) {
	if (menu == NULL)
	    return;
	unqueue(&menu_q, menu);
	for (i=0; i < MENU_MAXITEMS; i++) {
	    free(menu->menu_item[i]);
	    if (menu->menu_cmd[i] !=  (char *)-1)
		free(menu->menu_cmd[i]);
	}
	free(menu->menu_title);
	free(menu);
	return;
    }
    if (menu == NULL) {
	menu = malloc(sizeof(menudata));
	if (menu) {
	    sstrncpy(menu->name, GETOBJ(string,2), NAMEDTHINGNAMELEN);
	    enqueue(&menu_q, menu);
	}
    }
    if (menu) {
	int which = GETOBJ(int,2) - 1;

	switch(GETOBJ(int,1)) {
	  case MENU_CLEAR:
	    menu->menu_flags |= MENUF_CLEAR;
	    break;
	  case MENU_SINGLESPACE:
	    menu->menu_flags |= MENUF_SINGLESPACE;
	    break;
	  case MENU_LINEMODE:
	    menu->menu_flags |= MENUF_CONFIRM;
	    break;
	  case MENU_STATUSLINE:
	    menu->menu_flags |= MENUF_STATUS;
	    break;
	  case MENU_TITLE:
	    banner(csb, &string);
	    setstring(&menu->menu_title, string);
	    break;
	  case MENU_TEXT:
	    setstring(&menu->menu_item[which], GETOBJ(string,1));
	    if (which > MENU_MAXDOUBLESPACED) 
		menu->menu_flags |= MENUF_SINGLESPACE|MENUF_CONFIRM;
	    break;
	  case MENU_DEFAULT:
	    menu->menu_default = GETOBJ(int,2);
	    break;
	  case MENU_COMMAND:
	    if (!strcasecmp("menu-exit", GETOBJ(string,1))) {
		if (menu->menu_cmd[which] != (char *)-1)
		    free(menu->menu_cmd[which]);
		menu->menu_cmd[which] = (char *)-1;
	    }
	    else			       
		setstring(&menu->menu_cmd[which], GETOBJ(string,1));
	    break;
	  default:
	    /* Never! */
	    printf("\n%% Unknown menu cmd value %d", GETOBJ(int,1));
	    break;
	}
	return;
    }
    printf(nomemory);
}

/*
 * show_whoami
 * Show status of line and pause.  Useful for user support.
 */
static void show_menu_whoami (parseinfo *csb)
{
    printf("\n\n\nComm Server \"%s\", Line %t at %dbps.  Location \"%s\"",
	   hostname, stdio->ttynum, stdio->tty_txspeed, stdio->location);
    if (!null(GETOBJ(string,1)))
	printf("\nAdditional data: %s", GETOBJ(string,1));
    if (!(csb->flags & CONFIG_HTTP)) {
	printf("\n\n--More--");
	(void) getcwait(stdio);
    }
}

static void dumb_clearscreen (void)
{
    int i;

    for (i=0; i<=stdio->tty_length; i++)
	printf("\n");
}

/*
 * Parser chains for menus.
 * Include these in menus.c to simplify makefile changes necessary to
 * add menus to an image.
 */

#include "../parser/parser_actions.h"
#include "../parser/macros.h"


/*
 * Parser chains and definitions for menus subsystem
 */

/*
 * Parse chains for Menus show commands
 * Not yet implemented.
 */
#define ALTERNATE       NONE
#if 0
#include "../parser/exec_show_menus.h"
#endif

EOLNS	(show_whoami_eol, show_menu_whoami);
TEXT	(show_whoami_data, show_whoami_eol, show_whoami_eol,
	 OBJ(string,1), "Additional data");
KEYWORD (show_whoami, show_whoami_data, ALTERNATE,
	 "whoami", "Info on current tty line", PRIV_USER|PRIV_USER_HIDDEN);
LINK_POINT(menus_show_commands, show_whoami);
#undef	ALTERNATE

/*
 * Parse chains for "menu" user command
 */
#define ALTERNATE       NONE
#include "exec_menu.h"
LINK_POINT(exec_menu_cmds, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for Menus config commands
 */
#define	ALTERNATE	NONE
#include "cfg_menus.h"
LINK_POINT(menus_config_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * menu_defined
 * function used by the parser to determin whether there are any menus
 * defined.  If there are no menus defined, the "menu" command does not
 * show up in user help.
 */

boolean menu_defined (void)
{
    return(menu_q.qhead != NULL);
}

/*
 * Parse chain registration array for Menus
 */
static parser_extension_request menus_chain_init_table[] = {
    { PARSE_ADD_EXEC_CMD, &pname(exec_menu_cmds) },
    { PARSE_ADD_SHOW_CMD, &pname(menus_show_commands) },
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(menus_config_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * Menus subsystem header and initialization.
 */
#define MENUS_MAJVERSION 1
#define MENUS_MINVERSION 0
#define MENUS_EDITVERSION  1

/*
 * menus_init
 * Initialize the menu user interface.  This consists primarilly of
 * tying the commands into the parser at the appropriate points.
 */

static void menus_init (subsystype *subsys)
{
    queue_init(&menu_q, 0);
    parser_add_command_list(menus_chain_init_table, "menus");
    reg_add_clear_screen(dumb_clearscreen, "clear screen");
}

SUBSYS_HEADER(menus, MENUS_MAJVERSION, MENUS_MINVERSION, MENUS_EDITVERSION,
	      menus_init, SUBSYS_CLASS_PROTOCOL,
	      NULL, NULL);
