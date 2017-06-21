/* $Id: read.c,v 3.2 1995/11/17 18:51:34 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/tn3270/read.c,v $
 *------------------------------------------------------------------
 * R E A D . C
 *
 * July, 1994, Bill Palter
 * 
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: read.c,v $
 * Revision 3.2  1995/11/17  18:51:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:37:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:07:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "tn3270.h"			/* Defines CISCO */
#include "3270.h"
#include "proto.h"

#ifdef CISCO
#include "master.h"

#else

#include <stdio.h>

#endif



unsigned const char code_table[64] = {
    0x40, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
    0xC8, 0xC9, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
    0x50, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
    0xD8, 0xD9, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
    0x60, 0x61, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
    0xE8, 0xE9, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
    0xF8, 0xF9, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
};



void read_modified (ScreenPtr screen)
{
    struct field_type *field;
    unsigned short i;
    unsigned short space_count = 0;

    QUEUE_BYTE(screen->AID);
    QUEUE_BYTE(ADDRESS_TO_3270_0(screen->cursor_location));
    QUEUE_BYTE(ADDRESS_TO_3270_1(screen->cursor_location));

    field = screen->field_first;

    /*
     * If there is no first field on the screen, it must be unformatted so do
     * an unformatted read operation and return
     */
    if (field == NULL) {
	for (i = 0; i < screen->current_size; i++)
	    if (screen->screen[i]) {
		if (space_count) {
		    while (space_count--)
			QUEUE_BYTE(EBCDIC_SPACE);
		    space_count = 0;
		}
		QUEUE_BYTE(screen->screen[i]);
	    } else if (screen->yale_improved_null == YALE_ON)
		space_count++;
	return;
    }
    while (1) {
	if (field->attribute.field_attribute & FA_DISPLAY_MDT_MASK)
	    send_field(screen, field);

	if (field->flags & FIELD_LAST)
	    break;

	field = FIELD_NEXT_FIELD(field);
    }

}



void send_field (ScreenPtr screen, struct field_type * field)
{
    unsigned short i;
    unsigned short start_address;
    unsigned short space_count = 0;
    unsigned char data_seen = 0;

    start_address = field->start_address;
    INCREMENT_ADDRESS(start_address);
    QUEUE_BYTE(ORDER_SBA);
    QUEUE_BYTE(ADDRESS_TO_3270_0(start_address));
    QUEUE_BYTE(ADDRESS_TO_3270_1(start_address));

    for (i = start_address; i != FIELD_END_ADDRESS(field); INCREMENT_ADDRESS(i))
	if (screen->screen[i]) {
	    if (space_count) {
		while (space_count--)
		    QUEUE_BYTE(EBCDIC_SPACE);
		space_count = 0;
	    }
	    QUEUE_BYTE(screen->screen[i]);
	    data_seen = 1;
	} else if (screen->yale_improved_null == YALE_ON)
	    if (data_seen)
	    	space_count++;
}



void read_short (ScreenPtr screen)
{

    QUEUE_BYTE(screen->AID);

}



static void read_buffer (ScreenPtr screen)
{
    unsigned short i;
    struct field_type *field;

    QUEUE_BYTE(screen->AID);
    QUEUE_BYTE(ADDRESS_TO_3270_0(screen->cursor_location));
    QUEUE_BYTE(ADDRESS_TO_3270_1(screen->cursor_location));
    for (i = 0; i < screen->current_size; i++)
	if ((screen->screen[i] == ORDER_SF) ||
	    (screen->screen[i] == ORDER_SFE)) {
	    field = field_find(screen, i);
	    QUEUE_BYTE(ORDER_SF);
	    QUEUE_BYTE(FIELD_ATTRIBUTE(field));
	} else
	    QUEUE_BYTE(screen->screen[i]);


}



void cmd_read_buffer (ScreenPtr screen)
{

    read_buffer(screen);
    QUEUE_END_OF_TRANS;
}

void cmd_read_modified (ScreenPtr screen)
{

    read_modified(screen);
    QUEUE_END_OF_TRANS;
}
