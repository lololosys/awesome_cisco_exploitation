/* $Id: keyboard.c,v 3.2.62.3 1996/07/20 00:22:08 snyder Exp $
 * $Source: /release/112/cvs/Xsys/tn3270/keyboard.c,v $
 *------------------------------------------------------------------
 * K E Y B O A R D . C
 *
 * July, 1994, Bill Palter
 * 
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: keyboard.c,v $
 * Revision 3.2.62.3  1996/07/20  00:22:08  snyder
 * CSCdi63593:  keys array in tn3270 needs const
 *              4464 out of data section, 12 out of image
 * Branch: California_branch
 *
 * Revision 3.2.62.2  1996/03/27  23:28:08  irfan
 * CSCdi51821:  tn3270 should allow typeahead
 * Branch: California_branch
 * Add typeahead buffer to hold keyboard input while keyboard appears
 * locked. Add a TN3270 UI knob to control this behaviour. Reduce the
 * amount
 * of whitespace painted
 *
 * Revision 3.2.62.1  1996/03/18  22:18:58  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  10:53:58  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  21:22:29  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:51:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:37:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/12  00:11:58  satya
 * CSCdi38677:  tn3270: cursor placement problems
 *
 * Revision 2.2  1995/06/13  23:47:27  satya
 * CSCdi34765:  tn3270 doesnt provide ATTN and SYSREQ key functionalities.
 *
 * Revision 2.1  1995/06/07  23:07:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *
 *	Module :	3270_keyboard
 *
 *	Author :	William Palter
 *
 *	Date   :	6-Nov-1993
 *
 * 	3270 keyboard functions
 */
#include "master.h"
#include <ctype.h>
#include <string.h>
#include "connect.h"
#include "ttysrv.h"
#include "../tcp/tcp.h"
#include "../tcp/telnet.h"
#include "tn3270.h"			/* Defines CISCO */
#include "3270.h"
#include "proto.h"

#ifdef CISCO				/* CISCO platform only */

#include "master.h"
#include <ciscolib.h>
#include "tn3270_private.h"

#endif					/* endif CISCO */


static void tn3270_field_btab (ScreenPtr screen, unsigned char arg1)
{
    struct field_type *field, *temp;
    unsigned short c_start;

    if (!screen->field_first)
	screen->cursor_location = 0;
    else {

	field = field_find(screen, screen->cursor_location);
	c_start = field->start_address;
	INCREMENT_ADDRESS(c_start);

	if ((screen->cursor_location == c_start) ||
	    (screen->cursor_location == field->start_address))
	    temp = FIELD_PREV_FIELD(field);
	else
	    temp = field;

	while (temp != field) {
	    if (!(temp->attribute.field_attribute & FA_PROT_MASK))
		break;
	    temp = FIELD_PREV_FIELD(temp);
	}
	screen->cursor_location = temp->start_address;
	INCREMENT_ADDRESS(screen->cursor_location);
    }

    (*screen->set_cursor_location) (screen->interface_data, screen->cursor_location);

}


void tn3270_redraw (ScreenPtr screen, unsigned char arg1)
{
    unsigned short i;
    struct character_attribute_type char_att;


    (*screen->clear_screen) (screen->interface_data);

    RESET_CHAR_ATTRIBUTE(&char_att);

    for (i = 0; i < screen->current_size; i++) {
	if ((screen->screen[i] != 0x00) ||
	    (memcmp(&char_att,
		    &(screen->attribute[i]),
		    sizeof(char_att)) != 0))
	    screen->tags[i] = 1;
    }

    field_mark_all_new(screen);

    update_screen(screen);
    (*screen->set_cursor_location) (screen->interface_data, screen->cursor_location);
    flush();

}


static void tn3270_erase_input (ScreenPtr screen, unsigned char arg1)
{


    struct character_attribute_type char_att;
    unsigned short stop_address;
    FieldPtr current_field;
    unsigned short i;

    RESET_CHAR_ATTRIBUTE(&char_att);

    stop_address = screen->current_size - 1;
    i = 0;

    field_reset_mdt_all(screen);

    current_field = field_find(screen, i);

    if (current_field)
	while (1) {
	    if ((screen->screen[i] == ORDER_SF) ||
		(screen->screen[i] == ORDER_SFE)) {
		current_field = field_find(screen, i);
	    } else if (!(FIELD_ATTRIBUTE(current_field) & FA_PROT_MASK)) {
		if ((screen->screen[i] != 0x00) ||
		    (memcmp(&char_att,
			    &(screen->attribute[i]),
			    sizeof(char_att)) != 0)) {
		    screen->screen[i] = 0x00;
		    screen->attribute[i] = char_att;
		    screen->tags[i] = 1;
		}
	    }
	    INCREMENT_ADDRESS(i);
	    if (i == stop_address)
		break;
	}
    else
	while (1) {
	    if ((screen->screen[i] != 0x00) ||
		(memcmp(&char_att,
			&(screen->attribute[i]),
			sizeof(char_att)) != 0)) {
		screen->screen[i] = 0x00;
		screen->attribute[i] = char_att;
		screen->tags[i] = 1;
	    }
	    INCREMENT_ADDRESS(i);
	    if (i == stop_address)
		break;
	}

    update_screen(screen);
    (*screen->set_cursor_location) (screen->interface_data, screen->cursor_location);
}






static void tn3270_erase_to_eof (ScreenPtr screen, unsigned char arg1)
{
    unsigned short end_address;
    FieldPtr field;
    unsigned short i;
    struct character_attribute_type char_att;

    end_address = 0;
    field = field_find(screen, screen->cursor_location);

    if (field) {
	end_address = FIELD_END_ADDRESS(field);
	if ((field->attribute.field_attribute & FA_PROT_MASK) ||
	    (screen->cursor_location == field->start_address))
	    return;
        field->attribute.field_attribute |= FA_DISPLAY_MDT_MASK;
    }
    i = screen->cursor_location;
    RESET_CHAR_ATTRIBUTE(&char_att);

    while (i != end_address) {
	if ((screen->screen[i] != 0x00) ||
	    (memcmp(&char_att,
		    &(screen->attribute[i]),
		    sizeof(char_att)) != 0)) {
	    screen->screen[i] = 0x00;
	    screen->attribute[i] = char_att;
	    screen->tags[i] = 1;
	}
	INCREMENT_ADDRESS(i);
    }

    update_screen(screen);
    (*screen->set_cursor_location) (screen->interface_data, screen->cursor_location);
}


static void tn3270_erase_field (ScreenPtr screen, unsigned char arg1)
{
	
	tn3270_field_btab (screen,0);
	tn3270_erase_to_eof (screen,0);
}

static void tn3270_home_key (ScreenPtr screen, unsigned char arg1)
{
    struct field_type *field, *temp;
    
    if (!screen->field_first) {
	/*
	 * If there are no fields just move to the home row
	 */
	screen->cursor_location = screen->home * screen->current_width;
	
    } else {
	
	/* otherwise find the field that the home position is in */
	
	field = field_find(screen, screen->home * screen->current_width);
	screen->cursor_location = screen->home * screen->current_width;
	
	/* check to see if the field is an attribute field
	 *  if so, then move to next field */

	if (screen->cursor_location == field->start_address) {
	    INCREMENT_ADDRESS(screen->cursor_location);
	}

	/* If the location is unprotected then just mode the cursor there */
	
	if (field->attribute.field_attribute & FA_PROT_MASK) {
	    temp = field;
	    field = FIELD_NEXT_FIELD(field);
	    screen->cursor_location = screen->home * screen->current_width;
	    do {
		if (!(field->attribute.field_attribute & FA_PROT_MASK)) {
		    screen->cursor_location = field->start_address;
		    INCREMENT_ADDRESS(screen->cursor_location);
		    break;
		}
		field = FIELD_NEXT_FIELD(field);
	    } while (field != temp);
	}
    }
    
    (*screen->set_cursor_location) (screen->interface_data,
				    screen->cursor_location);
}



static void tn3270_sethom (ScreenPtr screen, unsigned char arg1)
{

    screen->home = ADDRESS_TO_ROW(screen->cursor_location);

}

static void tn3270_setmrg (ScreenPtr screen, unsigned char arg1)
{

    screen->left_margin = ADDRESS_TO_COL(screen->cursor_location);

}

static void tn3270_sysreq_key(ScreenPtr screen, unsigned char arg1)
{
    conntype 	   *socket;
    tcbtype        *tcb;

    socket = (conntype *) screen->socket;
    tcb = (tcbtype *) socket->tcb;

    (void) tcp_putbyte(tcb, TN_IAC, TRUE);
    (void) tcp_putbyte(tcb, TN_IP, TRUE);
    (void) tcp_putpush(tcb, TRUE, FALSE);

    screen->keyboard_state = KEYBOARD_LOCKED;
    screen->error_state = STATUS_SLOCK;
    (*screen->status_msg) (screen->interface_data,
			   MSG_STATUS, STATUS_SLOCK);
}

static void tn3270_attn_key(ScreenPtr screen, unsigned char arg1)
{
    (void) tnof((conntype *) screen->socket, BREAK_CHAR, TRUE, FALSE);
    screen->keyboard_state = KEYBOARD_LOCKED;
    screen->error_state = STATUS_SLOCK;
    (*screen->status_msg) (screen->interface_data,
			   MSG_STATUS, STATUS_SLOCK);
}

static void tn3270_coltab (ScreenPtr screen, unsigned char arg1)
{
    int i;
    unsigned short row;
    unsigned short col;


    row = ADDRESS_TO_ROW(screen->cursor_location);
    col = ADDRESS_TO_COL(screen->cursor_location);

    i = col + 1;

    while (i < screen->current_width) {
	if (screen->column_tabs[i])
	    break;
	i++;
    }

    if (i >= screen->current_width)
	i = screen->current_width - 1;


    screen->cursor_location =
	row * screen->current_width + i;

    (*screen->set_cursor_location) (screen->interface_data, screen->cursor_location);


}

static void tn3270_colbak (ScreenPtr screen, unsigned char arg1)
{
    short i;
    unsigned short row;
    unsigned short col;


    row = ADDRESS_TO_ROW(screen->cursor_location);
    col = ADDRESS_TO_COL(screen->cursor_location);

    i = col - 1;

    while (i > 0) {
	if (screen->column_tabs[i])
	    break;
	i--;
    }

    if (i < 0)
	i = 0;

    screen->cursor_location =
	row * screen->current_width + i;

    (*screen->set_cursor_location) (screen->interface_data, screen->cursor_location);


}




static void null_key (ScreenPtr screen, unsigned char arg1)
{
}


static void tn3270_settab(ScreenPtr screen, unsigned char arg1)
{

    screen->column_tabs[screen->cursor_location % screen->current_width] = 1;


}


void tn3270_deltab (ScreenPtr screen, unsigned char arg1)
{

    screen->column_tabs[screen->cursor_location % screen->current_width] = 0;


}

static void tn3270_clrtab (ScreenPtr screen, unsigned char arg1)
{

    memset(screen->column_tabs, 0, sizeof(screen->column_tabs));

}




static void tn3270_reset (ScreenPtr screen, unsigned char arg1)
{

    screen->keyboard_state = KEYBOARD_UNLOCKED;
    screen->error_state = STATUS_NORMAL;
    screen->insert_mode = INSERT_OFF;

    if (!(screen->transparent_mode == TRANS_OFF))
    	transparent_mode_end (screen);

    screen->typeahead_pointer = screen->typeahead_buffer;

    (*screen->status_msg) (screen->interface_data, MSG_CURSOR, CURSOR_NORMAL);
    (*screen->status_msg) (screen->interface_data, MSG_STATUS, STATUS_NORMAL);
}


/*
 * insert pos
 *
 *	Inserts an opened space at the cursor location
 *
 */

static int tn3270_insert_pos (ScreenPtr screen)
{
    unsigned short i, j;
    unsigned short end_address;
    FieldPtr field;

    /*
     * if we are sitting on a NULL then jsut return
     */

    if (screen->screen[screen->cursor_location] == EBCDIC_NULL)
	return 1;

    i = screen->cursor_location;
    field = field_find(screen, i);
    if (field)
	end_address = FIELD_END_ADDRESS(field);
    else
	end_address = i;
    i++;

    /*
     * find a NULL
     */
    while (i != end_address) {
	if (screen->screen[i] == EBCDIC_NULL)
	    break;
	INCREMENT_ADDRESS(i);
    }

    /*
     * is we reached the end address there was no NULL
     */
    if (i == end_address)
	return 0;

    end_address = i;

    for (i = end_address; i != screen->cursor_location; DECREMENT_ADDRESS(i)) {
	j = i;
	DECREMENT_ADDRESS(j);
	if ((screen->screen[i] != screen->screen[j]) ||
	    (!COMP_CHAR_ATT((&(screen->attribute[i])),
			    (&(screen->attribute[j]))))) {
	    screen->screen[i] = screen->screen[j];
	    screen->attribute[i] = screen->attribute[j];
	    screen->tags[i] = 1;
	}
    }

    update_screen(screen);
    (*screen->set_cursor_location) (screen->interface_data, screen->cursor_location);


    return 1;
}


void tn3270_add_character (ScreenPtr screen, unsigned char character)
{
    struct field_type *field;
    struct interface_attribute_type attribute;
    unsigned char fld_att;


    memset(&attribute, 0, sizeof(attribute));
    field = field_find(screen, screen->cursor_location);


    if (field)
	if ((field->attribute.field_attribute & FA_PROT_MASK) ||
	    (screen->cursor_location == field->start_address)) {
	    screen->keyboard_state = KEYBOARD_LOCKED;
	    screen->error_state = STATUS_PROT_FIELD;
	    (*screen->status_msg) (screen->interface_data,
				   MSG_STATUS, STATUS_PROT_FIELD);
	    (*screen->ring_bell) (screen->interface_data);

	    return;

	}
    if (screen->insert_mode == INSERT_ON)
	if (!(tn3270_insert_pos(screen))) {
	    screen->keyboard_state = KEYBOARD_LOCKED;
	    screen->error_state = STATUS_INSERT_OVERFLOW;
	    (*screen->status_msg) (screen->interface_data,
				   MSG_STATUS, STATUS_INSERT_OVERFLOW);
	    (*screen->ring_bell) (screen->interface_data);
	    return;
	}
    if (field)
	field->attribute.field_attribute |= FA_DISPLAY_MDT_MASK;

    if (field)
	fld_att = field->attribute.field_attribute;
    else
	fld_att = 0;

    if (screen->color_mode == COLOR_MODE_BASE) {
	attribute.foreground_color = FIELD_ATT_TO_COLOR(fld_att);
	if (FIELD_ATT_DATA_INTENSIFIED(fld_att))
	  attribute.highlighting |= INTERFACE_HIGHLIGHT_INTENSIFY;
	else if (FIELD_ATT_DATA_INVISIBLE(fld_att))
	    attribute.highlighting |= INTERFACE_HIGHLIGHT_INVISIBLE;
    } else
	merge_attributes(screen, field, &(screen->default_attribute),
			 &attribute);





    screen->screen[screen->cursor_location] = character;
    screen->attribute[screen->cursor_location] = screen->default_attribute;


    (screen->put_chars) (screen->interface_data,
			 &screen->screen[screen->cursor_location], 1,
			 screen->cursor_location, &attribute);

    INCREMENT_ADDRESS(screen->cursor_location);

    /*
     * If we have incremented onto a field mark then autoskip if needed or
     * just skip the field mark
     */

    if ((screen->screen[screen->cursor_location] == ORDER_SF) ||
        (screen->screen[screen->cursor_location] == ORDER_SFE)) {
        field = field_find(screen, screen->cursor_location);
        if ((FIELD_ATTRIBUTE(field) & FA_SKIP_MASK) == FA_SKIP_MASK) {
            while ((screen->screen[screen->cursor_location] == ORDER_SF) ||
                   (screen->screen[screen->cursor_location] == ORDER_SFE)) {
                field = field_find(screen, screen->cursor_location);
                if (FIELD_ATTRIBUTE(field) & FA_PROT_MASK)
                    screen->cursor_location = FIELD_END_ADDRESS(field);
                else
                    INCREMENT_ADDRESS(screen->cursor_location);
            }
        } else
            INCREMENT_ADDRESS(screen->cursor_location);
    }

    (*screen->set_cursor_location) (screen->interface_data, screen->cursor_location);

}


void tn3270_right_arrow (ScreenPtr screen, unsigned char arg1)
{

    INCREMENT_ADDRESS(screen->cursor_location);
    (*screen->set_cursor_location) (screen->interface_data, screen->cursor_location);

}


void tn3270_left_arrow (ScreenPtr screen, unsigned char arg1)
{

    screen->cursor_location = (screen->cursor_location + screen->current_size - 1) % screen->current_size;
    (*screen->set_cursor_location) (screen->interface_data, screen->cursor_location);

}


void tn3270_up_arrow (ScreenPtr screen, unsigned char arg1)
{

    screen->cursor_location = (screen->cursor_location +
			       screen->current_size -
			       screen->current_width) % screen->current_size;
    (*screen->set_cursor_location) (screen->interface_data, screen->cursor_location);

}


void tn3270_down_arrow (ScreenPtr screen, unsigned char arg1)
{

    screen->cursor_location = (screen->cursor_location +
			       screen->current_width) % screen->current_size;
    (*screen->set_cursor_location) (screen->interface_data, screen->cursor_location);

}




void tn3270_enter_key (ScreenPtr screen, unsigned char arg1)
{
    screen->AID = AID_ENTER;
    read_modified(screen);
    QUEUE_END_OF_TRANS;

    screen->keyboard_state = KEYBOARD_LOCKED;
    screen->error_state = STATUS_SLOCK;
    (*screen->status_msg) (screen->interface_data,
			   MSG_STATUS, STATUS_SLOCK);

}


static int test_status = 1;

static void test_key (ScreenPtr screen, unsigned char arg1)
{
    if (test_status) {
	(*screen->status_msg) (screen->interface_data,
			       MSG_STATUS, STATUS_SLOCK);
	test_status = 0;
    } else {
	(*screen->status_msg) (screen->interface_data,
			       MSG_STATUS, STATUS_NORMAL);
	test_status = 1;
    }
}

/*
 * th3270 delete key
 *
 *	deletes the character under the cursor
 *
 *	note that we do not have to protect ourselves from wrapping around the
 *	screen since when you delete a character the null gets inserted
 *	at the end of the current line.
 */


static void tn3270_delete_key (ScreenPtr screen, unsigned char arg1)
{
    FieldPtr field;
    unsigned short end_of_line;
    unsigned short begining_of_line;
    unsigned short end_of_delete;
    unsigned short end_of_field;
    unsigned short i;
    struct character_attribute_type null_attribute;


    RESET_CHAR_ATTRIBUTE(&null_attribute);
    field = field_find(screen, screen->cursor_location);



    if (field)
	if ((field->attribute.field_attribute & FA_PROT_MASK) ||
	    (screen->cursor_location == field->start_address)) {
	    tn3270_debug(screen, DEBUG_INFO, "FIELD %04x\n",
			 field->start_address); 
	    (*screen->ring_bell) (screen->interface_data);
	    return;

	}
    if (field)
	field->attribute.field_attribute |= FA_DISPLAY_MDT_MASK;


    begining_of_line = ((screen->cursor_location / screen->current_width) *
			screen->current_width);
    end_of_line = begining_of_line + (screen->current_width);

    end_of_delete = end_of_line;

    if (field) {
	end_of_field = FIELD_END_ADDRESS(field);
	if ((end_of_field < end_of_line) &&
	    (end_of_field > begining_of_line))
	    end_of_delete = end_of_field;
    }
    for (i = screen->cursor_location; i < (end_of_delete - 1); i++) {
	if ((screen->screen[i] != screen->screen[i + 1]) ||
	    (!COMP_CHAR_ATT((&(screen->attribute[i])),
			    (&(screen->attribute[i + 1]))))) {
	    screen->screen[i] = screen->screen[i + 1];
	    screen->attribute[i] = screen->attribute[i + 1];
	    screen->tags[i] = 1;
	}
    }


    if ((screen->screen[i] != EBCDIC_NULL) ||
	(!COMP_CHAR_ATT((&(screen->attribute[i])),
			&null_attribute))) {
	screen->screen[i] = EBCDIC_NULL;
	screen->attribute[i] = null_attribute;
	screen->tags[i] = 1;
    }
    update_screen(screen);
    (*screen->set_cursor_location) (screen->interface_data, screen->cursor_location);

}



static void tn3270_erase_key (ScreenPtr screen, unsigned char arg1)
{
    unsigned short prev_pos;
    FieldPtr field;

    prev_pos = (screen->cursor_location + screen->current_size - 1) % screen->current_size;
    field = field_find(screen, prev_pos);
    if (field)
	if ((field->attribute.field_attribute & FA_PROT_MASK) ||
	    (prev_pos == field->start_address)) {
	    tn3270_debug(screen, DEBUG_INFO, "FIELD %04x\n",
			 field->start_address); 
	    (*screen->ring_bell) (screen->interface_data);
	    return;

	}
    tn3270_left_arrow(screen, 0);
    tn3270_delete_key(screen, 0);

}


void tn3270_pf_key (ScreenPtr screen, unsigned char key)
{

    screen->AID = key;
    read_modified(screen);
    QUEUE_END_OF_TRANS;
    screen->keyboard_state = KEYBOARD_LOCKED;
    screen->error_state = STATUS_SLOCK;
    (*screen->status_msg) (screen->interface_data,
			   MSG_STATUS, STATUS_SLOCK);

}


void tn3270_clear_key (ScreenPtr screen, unsigned char arg1)
{

    clear_screen(screen);
    screen->AID = AID_CLEAR;
    read_short(screen);
    QUEUE_END_OF_TRANS;
    screen->keyboard_state = KEYBOARD_LOCKED;
    screen->error_state = STATUS_SLOCK;
    (*screen->status_msg) (screen->interface_data,
			   MSG_STATUS, STATUS_SLOCK);

}


void tn3270_pa_key (ScreenPtr screen, unsigned char key)
{

    screen->AID = key;
    read_short(screen);
    QUEUE_END_OF_TRANS;
    screen->keyboard_state = KEYBOARD_LOCKED;
    screen->error_state = STATUS_SLOCK;
    (*screen->status_msg) (screen->interface_data,
			   MSG_STATUS, STATUS_SLOCK);
}






void tn3270_field_tab (ScreenPtr screen, unsigned char arg1)
{
    struct field_type *field, *temp;

    if (!screen->field_first)
	screen->cursor_location = 0;
    else {

	field = field_find(screen, screen->cursor_location);

	temp = FIELD_NEXT_FIELD(field);

	while (temp != field) {
	    if (!(temp->attribute.field_attribute & FA_PROT_MASK))
		break;
	    temp = FIELD_NEXT_FIELD(temp);
	}
	screen->cursor_location = temp->start_address;
	INCREMENT_ADDRESS(screen->cursor_location);
    }

    (*screen->set_cursor_location) (screen->interface_data, screen->cursor_location);

}


static void tn3270_new_line (ScreenPtr screen, unsigned char arg1)
{
    struct field_type *field;
    unsigned short address;
    
    address = (( ((screen->cursor_location / screen->current_width) * screen->current_width) + 
		 screen->current_width) % screen->current_size);
    if (!screen->field_first)  {
	screen->cursor_location = address;
        (*screen->set_cursor_location) (screen->interface_data, screen->cursor_location);
        return;
    }
    else {
	field = field_find(screen, address);
	if ((field->start_address == address) &&
            (!(field->attribute.field_attribute & FA_PROT_MASK))) {
                INCREMENT_ADDRESS(address);
		screen->cursor_location = address;
        	(*screen->set_cursor_location) (screen->interface_data, screen->cursor_location);
         	return;
	}
        if (!(field->attribute.field_attribute & FA_PROT_MASK)) {
		screen->cursor_location = address;
        	(*screen->set_cursor_location) (screen->interface_data, screen->cursor_location);
         	return;
	}		
	screen->cursor_location = address;
	tn3270_field_tab (screen,0);
    }	
}


static void tn3270_insert_mode (ScreenPtr screen, unsigned char arg1)
{

    if (screen->insert_mode == INSERT_ON) {
	screen->insert_mode = INSERT_OFF;
	(*screen->status_msg) (screen->interface_data, MSG_CURSOR, CURSOR_NORMAL);
    } else {
	screen->insert_mode = INSERT_ON;
	(*screen->status_msg) (screen->interface_data, MSG_CURSOR, CURSOR_INSERT);
    }
}


static void tn3270_dup (ScreenPtr screen, unsigned char arg1)
{
    unsigned short location;

    location = screen->cursor_location;
    tn3270_add_character(screen, 0x1c);
    screen->cursor_location = location;
    tn3270_field_tab(screen, 0);
}





struct key_type {
    char *name;
    void (*routine) (ScreenPtr screen, unsigned char arg1);
    int arg1;
    int override;
};

const struct key_type keys[] = {

    {"null_key", null_key, 0, 0},
    {"ebcdic_40", tn3270_add_character, 0x40, 0},
    {"ebcdic_41", tn3270_add_character, 0x41, 0},
    {"ebcdic_42", tn3270_add_character, 0x42, 0},
    {"ebcdic_43", tn3270_add_character, 0x43, 0},
    {"ebcdic_44", tn3270_add_character, 0x44, 0},
    {"ebcdic_45", tn3270_add_character, 0x45, 0},
    {"ebcdic_46", tn3270_add_character, 0x46, 0},
    {"ebcdic_47", tn3270_add_character, 0x47, 0},
    {"ebcdic_48", tn3270_add_character, 0x48, 0},
    {"ebcdic_49", tn3270_add_character, 0x49, 0},
    {"ebcdic_4a", tn3270_add_character, 0x4a, 0},
    {"centsign", tn3270_add_character, 0x4a, 0},
    {"ebcdic_4b", tn3270_add_character, 0x4b, 0},
    {"ebcdic_4c", tn3270_add_character, 0x4c, 0},
    {"ebcdic_4d", tn3270_add_character, 0x4d, 0},
    {"ebcdic_4e", tn3270_add_character, 0x4e, 0},
    {"ebcdic_4f", tn3270_add_character, 0x4f, 0},

    {"ebcdic_50", tn3270_add_character, 0x50, 0},
    {"ebcdic_51", tn3270_add_character, 0x51, 0},
    {"ebcdic_52", tn3270_add_character, 0x52, 0},
    {"ebcdic_53", tn3270_add_character, 0x53, 0},
    {"ebcdic_54", tn3270_add_character, 0x54, 0},
    {"ebcdic_55", tn3270_add_character, 0x55, 0},
    {"ebcdic_56", tn3270_add_character, 0x56, 0},
    {"ebcdic_57", tn3270_add_character, 0x57, 0},
    {"ebcdic_58", tn3270_add_character, 0x58, 0},
    {"ebcdic_59", tn3270_add_character, 0x59, 0},
    {"ebcdic_5a", tn3270_add_character, 0x5a, 0},
    {"ebcdic_5b", tn3270_add_character, 0x5b, 0},
    {"ebcdic_5c", tn3270_add_character, 0x5c, 0},
    {"ebcdic_5d", tn3270_add_character, 0x5d, 0},
    {"ebcdic_5e", tn3270_add_character, 0x5e, 0},
    {"ebcdic_5f", tn3270_add_character, 0x5f, 0},

    {"ebcdic_60", tn3270_add_character, 0x60, 0},
    {"ebcdic_61", tn3270_add_character, 0x61, 0},
    {"ebcdic_62", tn3270_add_character, 0x62, 0},
    {"ebcdic_63", tn3270_add_character, 0x63, 0},
    {"ebcdic_64", tn3270_add_character, 0x64, 0},
    {"ebcdic_65", tn3270_add_character, 0x65, 0},
    {"ebcdic_66", tn3270_add_character, 0x66, 0},
    {"ebcdic_67", tn3270_add_character, 0x67, 0},
    {"ebcdic_68", tn3270_add_character, 0x68, 0},
    {"ebcdic_69", tn3270_add_character, 0x69, 0},
    {"ebcdic_6a", tn3270_add_character, 0x6a, 0},
    {"ebcdic_6b", tn3270_add_character, 0x6b, 0},
    {"ebcdic_6c", tn3270_add_character, 0x6c, 0},
    {"ebcdic_6d", tn3270_add_character, 0x6d, 0},
    {"ebcdic_6e", tn3270_add_character, 0x6e, 0},
    {"ebcdic_6f", tn3270_add_character, 0x6f, 0},

    {"ebcdic_70", tn3270_add_character, 0x70, 0},
    {"ebcdic_71", tn3270_add_character, 0x71, 0},
    {"ebcdic_72", tn3270_add_character, 0x72, 0},
    {"ebcdic_73", tn3270_add_character, 0x73, 0},
    {"ebcdic_74", tn3270_add_character, 0x74, 0},
    {"ebcdic_75", tn3270_add_character, 0x75, 0},
    {"ebcdic_76", tn3270_add_character, 0x76, 0},
    {"ebcdic_77", tn3270_add_character, 0x77, 0},
    {"ebcdic_78", tn3270_add_character, 0x78, 0},
    {"ebcdic_79", tn3270_add_character, 0x79, 0},
    {"ebcdic_7a", tn3270_add_character, 0x7a, 0},
    {"ebcdic_7b", tn3270_add_character, 0x7b, 0},
    {"ebcdic_7c", tn3270_add_character, 0x7c, 0},
    {"ebcdic_7d", tn3270_add_character, 0x7d, 0},
    {"ebcdic_7e", tn3270_add_character, 0x7e, 0},
    {"ebcdic_7f", tn3270_add_character, 0x7f, 0},

    {"ebcdic_80", tn3270_add_character, 0x80, 0},
    {"ebcdic_81", tn3270_add_character, 0x81, 0},
    {"ebcdic_82", tn3270_add_character, 0x82, 0},
    {"ebcdic_83", tn3270_add_character, 0x83, 0},
    {"ebcdic_84", tn3270_add_character, 0x84, 0},
    {"ebcdic_85", tn3270_add_character, 0x85, 0},
    {"ebcdic_86", tn3270_add_character, 0x86, 0},
    {"ebcdic_87", tn3270_add_character, 0x87, 0},
    {"ebcdic_88", tn3270_add_character, 0x88, 0},
    {"ebcdic_89", tn3270_add_character, 0x89, 0},
    {"ebcdic_8a", tn3270_add_character, 0x8a, 0},
    {"ebcdic_8b", tn3270_add_character, 0x8b, 0},
    {"ebcdic_8c", tn3270_add_character, 0x8c, 0},
    {"ebcdic_8d", tn3270_add_character, 0x8d, 0},
    {"ebcdic_8e", tn3270_add_character, 0x8e, 0},
    {"ebcdic_8f", tn3270_add_character, 0x8f, 0},

    {"ebcdic_90", tn3270_add_character, 0x90, 0},
    {"ebcdic_91", tn3270_add_character, 0x91, 0},
    {"ebcdic_92", tn3270_add_character, 0x92, 0},
    {"ebcdic_93", tn3270_add_character, 0x93, 0},
    {"ebcdic_94", tn3270_add_character, 0x94, 0},
    {"ebcdic_95", tn3270_add_character, 0x95, 0},
    {"ebcdic_96", tn3270_add_character, 0x96, 0},
    {"ebcdic_97", tn3270_add_character, 0x97, 0},
    {"ebcdic_98", tn3270_add_character, 0x98, 0},
    {"ebcdic_99", tn3270_add_character, 0x99, 0},
    {"ebcdic_9a", tn3270_add_character, 0x9a, 0},
    {"ebcdic_9b", tn3270_add_character, 0x9b, 0},
    {"ebcdic_9c", tn3270_add_character, 0x9c, 0},
    {"ebcdic_9d", tn3270_add_character, 0x9d, 0},
    {"ebcdic_9e", tn3270_add_character, 0x9e, 0},
    {"ebcdic_9f", tn3270_add_character, 0x9f, 0},

    {"ebcdic_a0", tn3270_add_character, 0xa0, 0},
    {"ebcdic_a1", tn3270_add_character, 0xa1, 0},
    {"ebcdic_a2", tn3270_add_character, 0xa2, 0},
    {"ebcdic_a3", tn3270_add_character, 0xa3, 0},
    {"ebcdic_a4", tn3270_add_character, 0xa4, 0},
    {"ebcdic_a5", tn3270_add_character, 0xa5, 0},
    {"ebcdic_a6", tn3270_add_character, 0xa6, 0},
    {"ebcdic_a7", tn3270_add_character, 0xa7, 0},
    {"ebcdic_a8", tn3270_add_character, 0xa8, 0},
    {"ebcdic_a9", tn3270_add_character, 0xa9, 0},
    {"ebcdic_aa", tn3270_add_character, 0xaa, 0},
    {"ebcdic_ab", tn3270_add_character, 0xab, 0},
    {"ebcdic_ac", tn3270_add_character, 0xac, 0},
    {"ebcdic_ad", tn3270_add_character, 0xad, 0},
    {"ebcdic_ae", tn3270_add_character, 0xae, 0},
    {"ebcdic_af", tn3270_add_character, 0xaf, 0},

    {"ebcdic_b0", tn3270_add_character, 0xb0, 0},
    {"ebcdic_b1", tn3270_add_character, 0xb1, 0},
    {"ebcdic_b2", tn3270_add_character, 0xb2, 0},
    {"ebcdic_b3", tn3270_add_character, 0xb3, 0},
    {"ebcdic_b4", tn3270_add_character, 0xb4, 0},
    {"ebcdic_b5", tn3270_add_character, 0xb5, 0},
    {"ebcdic_b6", tn3270_add_character, 0xb6, 0},
    {"ebcdic_b7", tn3270_add_character, 0xb7, 0},
    {"ebcdic_b8", tn3270_add_character, 0xb8, 0},
    {"ebcdic_b9", tn3270_add_character, 0xb9, 0},
    {"ebcdic_ba", tn3270_add_character, 0xba, 0},
    {"ebcdic_bb", tn3270_add_character, 0xbb, 0},
    {"ebcdic_bc", tn3270_add_character, 0xbc, 0},
    {"ebcdic_bd", tn3270_add_character, 0xbd, 0},
    {"ebcdic_be", tn3270_add_character, 0xbe, 0},
    {"ebcdic_bf", tn3270_add_character, 0xbf, 0},

    {"ebcdic_c0", tn3270_add_character, 0xc0, 0},
    {"ebcdic_c1", tn3270_add_character, 0xc1, 0},
    {"ebcdic_c2", tn3270_add_character, 0xc2, 0},
    {"ebcdic_c3", tn3270_add_character, 0xc3, 0},
    {"ebcdic_c4", tn3270_add_character, 0xc4, 0},
    {"ebcdic_c5", tn3270_add_character, 0xc5, 0},
    {"ebcdic_c6", tn3270_add_character, 0xc6, 0},
    {"ebcdic_c7", tn3270_add_character, 0xc7, 0},
    {"ebcdic_c8", tn3270_add_character, 0xc8, 0},
    {"ebcdic_c9", tn3270_add_character, 0xc9, 0},
    {"ebcdic_ca", tn3270_add_character, 0xca, 0},
    {"ebcdic_cb", tn3270_add_character, 0xcb, 0},
    {"ebcdic_cc", tn3270_add_character, 0xcc, 0},
    {"ebcdic_cd", tn3270_add_character, 0xcd, 0},
    {"ebcdic_ce", tn3270_add_character, 0xce, 0},
    {"ebcdic_cf", tn3270_add_character, 0xcf, 0},

    {"ebcdic_d0", tn3270_add_character, 0xd0, 0},
    {"ebcdic_d1", tn3270_add_character, 0xd1, 0},
    {"ebcdic_d2", tn3270_add_character, 0xd2, 0},
    {"ebcdic_d3", tn3270_add_character, 0xd3, 0},
    {"ebcdic_d4", tn3270_add_character, 0xd4, 0},
    {"ebcdic_d5", tn3270_add_character, 0xd5, 0},
    {"ebcdic_d6", tn3270_add_character, 0xd6, 0},
    {"ebcdic_d7", tn3270_add_character, 0xd7, 0},
    {"ebcdic_d8", tn3270_add_character, 0xd8, 0},
    {"ebcdic_d9", tn3270_add_character, 0xd9, 0},
    {"ebcdic_da", tn3270_add_character, 0xda, 0},
    {"ebcdic_db", tn3270_add_character, 0xdb, 0},
    {"ebcdic_dc", tn3270_add_character, 0xdc, 0},
    {"ebcdic_dd", tn3270_add_character, 0xdd, 0},
    {"ebcdic_de", tn3270_add_character, 0xde, 0},
    {"ebcdic_df", tn3270_add_character, 0xdf, 0},

    {"ebcdic_e0", tn3270_add_character, 0xe0, 0},
    {"ebcdic_e1", tn3270_add_character, 0xe1, 0},
    {"ebcdic_e2", tn3270_add_character, 0xe2, 0},
    {"ebcdic_e3", tn3270_add_character, 0xe3, 0},
    {"ebcdic_e4", tn3270_add_character, 0xe4, 0},
    {"ebcdic_e5", tn3270_add_character, 0xe5, 0},
    {"ebcdic_e6", tn3270_add_character, 0xe6, 0},
    {"ebcdic_e7", tn3270_add_character, 0xe7, 0},
    {"ebcdic_e8", tn3270_add_character, 0xe8, 0},
    {"ebcdic_e9", tn3270_add_character, 0xe9, 0},
    {"ebcdic_ea", tn3270_add_character, 0xea, 0},
    {"ebcdic_eb", tn3270_add_character, 0xeb, 0},
    {"ebcdic_ec", tn3270_add_character, 0xec, 0},
    {"ebcdic_ed", tn3270_add_character, 0xed, 0},
    {"ebcdic_ee", tn3270_add_character, 0xee, 0},
    {"ebcdic_ef", tn3270_add_character, 0xef, 0},

    {"ebcdic_f0", tn3270_add_character, 0xf0, 0},
    {"ebcdic_f1", tn3270_add_character, 0xf1, 0},
    {"ebcdic_f2", tn3270_add_character, 0xf2, 0},
    {"ebcdic_f3", tn3270_add_character, 0xf3, 0},
    {"ebcdic_f4", tn3270_add_character, 0xf4, 0},
    {"ebcdic_f5", tn3270_add_character, 0xf5, 0},
    {"ebcdic_f6", tn3270_add_character, 0xf6, 0},
    {"ebcdic_f7", tn3270_add_character, 0xf7, 0},
    {"ebcdic_f8", tn3270_add_character, 0xf8, 0},
    {"ebcdic_f9", tn3270_add_character, 0xf9, 0},
    {"ebcdic_fa", tn3270_add_character, 0xfa, 0},
    {"ebcdic_fb", tn3270_add_character, 0xfb, 0},
    {"ebcdic_fc", tn3270_add_character, 0xfc, 0},
    {"ebcdic_fd", tn3270_add_character, 0xfd, 0},
    {"ebcdic_fe", tn3270_add_character, 0xfe, 0},
    {"ebcdic_ff", tn3270_add_character, 0xff, 0},
    {"pfk1", tn3270_pf_key, AID_PF1, 0},
    {"pfk2", tn3270_pf_key, AID_PF2, 0},
    {"pfk3", tn3270_pf_key, AID_PF3, 0},
    {"pfk4", tn3270_pf_key, AID_PF4, 0},
    {"pfk5", tn3270_pf_key, AID_PF5, 0},
    {"pfk6", tn3270_pf_key, AID_PF6, 0},
    {"pfk7", tn3270_pf_key, AID_PF7, 0},
    {"pfk8", tn3270_pf_key, AID_PF8, 0},
    {"pfk9", tn3270_pf_key, AID_PF9, 0},
    {"pfk10", tn3270_pf_key, AID_PF10, 0},
    {"pfk11", tn3270_pf_key, AID_PF11, 0},
    {"pfk12", tn3270_pf_key, AID_PF12, 0},
    {"pfk13", tn3270_pf_key, AID_PF13, 0},
    {"pfk14", tn3270_pf_key, AID_PF14, 0},
    {"pfk15", tn3270_pf_key, AID_PF15, 0},
    {"pfk16", tn3270_pf_key, AID_PF16, 0},
    {"pfk17", tn3270_pf_key, AID_PF17, 0},
    {"pfk18", tn3270_pf_key, AID_PF18, 0},
    {"pfk19", tn3270_pf_key, AID_PF19, 0},
    {"pfk20", tn3270_pf_key, AID_PF20, 0},
    {"pfk21", tn3270_pf_key, AID_PF21, 0},
    {"pfk22", tn3270_pf_key, AID_PF22, 0},
    {"pfk23", tn3270_pf_key, AID_PF23, 0},
    {"pfk24", tn3270_pf_key, AID_PF24, 0},
    {"pfk25", tn3270_pf_key, AID_PF25, 0},
    {"pfk26", tn3270_pf_key, AID_PF26, 0},
    {"pfk27", tn3270_pf_key, AID_PF27, 0},
    {"pfk28", tn3270_pf_key, AID_PF28, 0},
    {"pfk29", tn3270_pf_key, AID_PF29, 0},
    {"pfk30", tn3270_pf_key, AID_PF30, 0},
    {"pfk31", tn3270_pf_key, AID_PF31, 0},
    {"pfk32", tn3270_pf_key, AID_PF32, 0},
    {"pfk33", tn3270_pf_key, AID_PF33, 0},
    {"pfk34", tn3270_pf_key, AID_PF34, 0},
    {"pfk35", tn3270_pf_key, AID_PF35, 0},
    {"pfk36", tn3270_pf_key, AID_PF36, 0},
    {"pa1", tn3270_pa_key, AID_PA1, 0},
    {"pa2", tn3270_pa_key, AID_PA2, 0},
    {"pa3", tn3270_pa_key, AID_PA3, 0},
    {"enter", tn3270_enter_key, 0, 0},
    {"tab", tn3270_field_tab, 0, 0},
    {"clear", tn3270_clear_key, 0, 0},
    {"up", tn3270_up_arrow, 0, 0},
    {"down", tn3270_down_arrow, 0, 0},
    {"left", tn3270_left_arrow, 0, 0},
    {"right", tn3270_right_arrow, 0, 0},
    {"delete", tn3270_delete_key, 0, 0},
    {"erase", tn3270_erase_key, 0, 0},
    {"insrt", tn3270_insert_mode, 0, 0},
    {"master_reset", tn3270_reset, 0, 1},
    {"reset", tn3270_reset, 0, 1},
    {"settab", tn3270_settab, 0, 0},
    {"clrtab", tn3270_clrtab, 0, 0},
    {"deltab", tn3270_settab, 0, 0},
    {"coltab", tn3270_coltab, 0, 0},
    {"colbak", tn3270_colbak, 0, 0},
    {"sethom", tn3270_sethom, 0, 0},
    {"setmrg", tn3270_setmrg, 0, 0},
    {"btab", tn3270_field_btab, 0, 0},
    {"eeof", tn3270_erase_to_eof, 0, 0},
    {"einp", tn3270_erase_input, 0, 0},
    {"reshow", tn3270_redraw, 0, 1},
    {"redraw", tn3270_redraw, 0, 1},
    {"redisplay", tn3270_redraw, 0, 1},
    {"dp", tn3270_dup, 0, 0},
    {"fm", tn3270_add_character, 0x1e, 0},
    {"nl", tn3270_new_line,0x00,0},
    {"ferase", tn3270_erase_field,0x00,0},
    {"home", tn3270_home_key,0,0},
    {"sysreq", tn3270_sysreq_key,0,0},
    {"attn", tn3270_attn_key,0,0},
/* test keys */
    {"testkey", test_key, 0, 0},
/* keys to implement latter */
    {"help", null_key,0,0},
    {"capture", null_key,0,0},
    {"cursel", null_key,0,0},
    {"testreq", null_key,0,0},
    {"treq", null_key,0,0},
/* Unimplemented keys for compatibility */
    {"indent", null_key, 0, 0},
    {"undent", null_key, 0, 0},
    {"escape", null_key, 0, 0},
    {"flinp", null_key, 0, 0},
    {"xon", null_key, 0, 0},
    {"xoff", null_key, 0, 0},
    {"synch", null_key, 0, 0},

    {0, 0, 0, 0}
};


void tn3270_trans_keystroke (ScreenPtr screen, int key)
{

    QUEUE_BYTE ((unsigned char) key);
    if (key == '\r') {
     	QUEUE_END_OF_TRANS;
    	transparent_mode_end (screen);
    }

}




void tn3270_keystroke (ScreenPtr screen, int key)
{

    if (!(screen->transparent_mode == TRANS_OFF)) {
	if (keys[key].override)
	    (*keys[key].routine) (screen, keys[key].arg1);
	return;
    }

    if ((screen->keyboard_state == KEYBOARD_UNLOCKED) ||
	keys[key].override)
	(*keys[key].routine) (screen, keys[key].arg1);
    else {
	if ((screen->pref_reset == ON) &&
	    (screen->error_state != STATUS_SLOCK)) {
	    tn3270_reset(screen, 0);
	    (*keys[key].routine) (screen, keys[key].arg1);
	} else {
	    if (tn3270_typeahead &&
		(screen->error_state == STATUS_SLOCK) &&
		(screen->typeahead_pointer <
		 (screen->typeahead_buffer+
		  TN3270_TYPEAHEAD_BUF_SIZE))) {
		/*
		 * add the keystroke to the typeahead buffer 
		 */
		*screen->typeahead_pointer++ = key;
	    } else {
		(*screen->ring_bell) (screen->interface_data);
	    }
	}
    }
}


int keyname_to_int(char *string)
{
    int i = 0;

    while (keys[i].name) {
	if (strcasecmp(string, keys[i].name) == 0)
	    return i;
	else
	    i++;
    }
    printf("Key %s not found\n", string);return 0;
}




char *int_to_keyname(int key)
{
    static char *unknown="unknown";

    if (key > (sizeof(keys) / sizeof(struct key_type)))
	return (unknown);

    if (keys[key].name)
	return (keys[key].name);
    else
	return (unknown);

}

#ifdef NO_USTRCMP
int ustrcmp(string1, string2)
    register char *string1;
    register char *string2;
{
    register int c1, c2;

    while ((c1 = (unsigned char) *string1++) != 0) {
	if (isupper(c1)) {
	    c1 = tolower(c1);
	}
	c2 = (unsigned char) *string2++;
	if (isupper(c2)) {
	    c2 = tolower(c2);
	}
	if (c1 < c2) {
	    return (-1);
	} else if (c2 && c1 > c2) {
	    return (1);
	} else if (!c2 && ((c1 == '|') || (c1 == '{'))) {
	    return (0);
	} else if (!c2) {
	    return (1);
	}
    }
    if (*string2) {
	return (-1);
    } else {
	return (0);
    }
}

#endif
