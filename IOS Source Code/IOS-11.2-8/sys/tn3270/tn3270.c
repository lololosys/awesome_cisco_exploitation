/* $Id: tn3270.c,v 3.3.22.2 1996/03/27 23:28:28 irfan Exp $
 * $Source: /release/111/cvs/Xsys/tn3270/tn3270.c,v $
 *------------------------------------------------------------------
 * T N 3 2 7 0 . C
 *
 * July, 1994, Bill Palter
 * 
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: tn3270.c,v $
 * Revision 3.3.22.2  1996/03/27  23:28:28  irfan
 * CSCdi51821:  tn3270 should allow typeahead
 * Branch: California_branch
 * Add typeahead buffer to hold keyboard input while keyboard appears
 * locked. Add a TN3270 UI knob to control this behaviour. Reduce the
 * amount
 * of whitespace painted
 *
 * Revision 3.3.22.1  1996/03/18  22:19:06  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.2  1996/03/07  10:54:08  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.24.1  1996/02/20  21:22:42  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/01/23  07:55:05  irfan
 * CSCdi47316:  possible memory corruption in tn3270
 * tn3270 copies data from telnet w/o bounds checking onto internal
 * bufferes. Force upper bounds.
 *
 * Revision 3.2  1995/11/17  18:51:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:37:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/11  21:33:09  satya
 * CSCdi38665:  tn3270: Mod3, Mod4 and Mod5 terminals not working
 *
 * Revision 2.1  1995/06/07  23:08:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <string.h>
#include "tn3270.h"			/* Defines CISCO */
#include "3270.h"
#include "proto.h"

#ifdef CISCO
#include <master.h>
#include <ciscolib.h>
#endif

/*
 *
 * Main 3270 emulation entry point, at this point we have a
 * full 3270 datastream packet
 */



void tn3270_keystroke(ScreenPtr screen,int key);



void tn3270_packet (ScreenPtr screen, struct packet_type * packet)
{
    int *typeahead;


    switch (*packet->exec++) {
	case CMD_SNA_WSF:
	case CMD_WSF:
	cmd_wsf(screen, packet);
	break;

      case CMD_EW:
      case CMD_SNA_EW:
	cmd_erase_write(screen, packet);
	break;

      case CMD_EWA:
      case CMD_SNA_EWA:
	cmd_erase_write_alt(screen, packet);
	break;

      case CMD_SNA_W:
      case CMD_W:
	cmd_write(screen, packet);
	break;

      case CMD_SNA_RM:
      case CMD_RM:
      case CMD_SNA_RMA:
	cmd_read_modified(screen);
	break;
      case CMD_RB:
      case CMD_SNA_RB:
	cmd_read_buffer(screen);
	break;
    };

    /*
     * if the keyboard is unlocked and we have data pass it in...
     */
    if (screen->typeahead_pointer != screen->typeahead_buffer) {
	int left;

	typeahead = screen->typeahead_buffer;
	while ((screen->keyboard_state == KEYBOARD_UNLOCKED) &&
	       (typeahead < screen->typeahead_pointer)){
	    tn3270_keystroke(screen, *typeahead);
	    typeahead++;
	}

	/*
	 * Shift all the remaining typeahead data to the
	 * begining of the buffer.
	 * We need to adjust for the sizeof int here,
	 * int - *int = the int count difference
	 */
	left = screen->typeahead_pointer - typeahead;
	if (left) {
	    bcopy (typeahead, screen->typeahead_buffer, left * sizeof(int));
	    screen->typeahead_pointer = screen->typeahead_buffer + left;
	} else {
	    screen->typeahead_pointer = screen->typeahead_buffer;
	}
    }
}


/*
 * allocate_screen
 *
 * 	allocate a new screen structure and init it.
 */



ScreenPtr allocate_screen (int mode)
{
    ScreenPtr screen;
    short i;

    /*
     * Allocate memory for the screen structure and the queue of dta to go to
     * the IBM system
     */


    screen = malloc(sizeof(screen_type));
    if (!screen)
	return screen;

    screen->IBM_Buf = malloc(SCREEN_INPUT_BUFF_SIZE);
    screen->IBM_Buf_Size = SCREEN_INPUT_BUFF_SIZE;
    screen->IBM_Buf_Ptr = screen->IBM_Buf;
    screen->SF_Len_Ptr = 0;
    if (!screen->IBM_Buf) {
	free(screen);
	return 0;
    }
    /*
     * Clear the field list header
     */

    screen->field_first = 0;
    /*
     * no graphics contex or interface contex
     */
    screen->graphics_contex = 0;
    screen->interface_data = 0;

    /*
     * Setup some default fields, just in case they are not filled in later
     */

    screen->emulation_mask = EM_BASE;


    switch (mode) {

      default:
	screen->primary_width = 80;
	screen->primary_height = 24;
	screen->primary_cell_x = 9;
	screen->primary_cell_y = 16;
	screen->alternate_width = 80;
	screen->alternate_height = 24;
	screen->alternate_cell_x = 9;
	screen->alternate_cell_y = 16;
	break;
      case TN3270_MOD5:
        screen->primary_width = 80;
        screen->primary_height = 24;
        screen->primary_cell_x = 9;
        screen->primary_cell_y = 16;
        screen->alternate_width = 132;
        screen->alternate_height = 27;
        screen->alternate_cell_x = 9;
        screen->alternate_cell_y = 16;
        break;
      case TN3270_MOD4:
        screen->primary_width = 80;
        screen->primary_height = 24;
        screen->primary_cell_x = 9;
        screen->primary_cell_y = 16;
        screen->alternate_width = 80;
        screen->alternate_height = 43;
        screen->alternate_cell_x = 9;
        screen->alternate_cell_y = 16;
        break;
      case TN3270_MOD3:
        screen->primary_width = 80;
        screen->primary_height = 24;
        screen->primary_cell_x = 9;
        screen->primary_cell_y = 16;
        screen->alternate_width = 80;
        screen->alternate_height = 32;
        screen->alternate_cell_x = 9;
        screen->alternate_cell_y = 16;
        break;
    }

    screen->yale_improved_null = YALE_ON;
    screen->typeahead_pointer = screen->typeahead_buffer;
    screen->insert_type = INSERT_TYPE_3277;
    screen->debug = 0;


    /*
     * Set our state to something known
     */

    screen->current_address = 0;
    screen->partition_mode = PARTITION_MODE_IMPLICIT;

    screen->color_mode = COLOR_MODE_BASE;
    screen->current_width = screen->primary_width;
    screen->current_height = screen->primary_height;
    screen->current_size = screen->current_width * screen->current_height;
    screen->current_cell_x = screen->primary_cell_x;
    screen->current_cell_y = screen->primary_cell_y;
    screen->screen_clear = SCREEN_CLEAR;
    screen->cursor_location = 0;
    screen->AID = AID_NOAID;
    screen->default_attribute.character_set = 0;
    screen->default_attribute.extended_highlight = 0;
    screen->default_attribute.foreground_color = 0;
    screen->default_attribute.background_color = 0;
    screen->default_attribute.transparency = 0;
    screen->error_state = ERROR_NONE;
    screen->keyboard_state = KEYBOARD_UNLOCKED;
    screen->insert_mode = INSERT_OFF;
    screen->transparent_mode = TRANS_OFF;
    screen->pref_reset = ON;
    screen->display_fields = 0;
    memset(screen->screen, 0, sizeof(screen->screen));
    memset(screen->tags, 0, sizeof(screen->tags));
    memset(screen->attribute, 0, sizeof(screen->attribute));

    /*
     * set some default  column tabs and home  line
     */

    for (i = 0; i < 132; i++)
	if (i % 8 == 0)
	    screen->column_tabs[i] = 1;
	else
	    screen->column_tabs[i] = 0;

    screen->home = 0;
    screen->left_margin = 0;

    return screen;
}



void deallocate_screen (ScreenPtr screen)
{


    if (screen) {
	if (screen->IBM_Buf)
	    free(screen->IBM_Buf);
	free(screen);
    }
}
