/* $Id: orders.c,v 3.5.20.1 1996/03/18 22:19:04 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/tn3270/orders.c,v $
 *------------------------------------------------------------------
 * O R D E R S . C
 *
 * July, 1994, Bill Palter
 * 
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: orders.c,v $
 * Revision 3.5.20.1  1996/03/18  22:19:04  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.12.2  1996/03/07  10:54:06  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.12.1  1996/02/20  21:22:34  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/01/25  10:03:02  irfan
 * CSCdi47466:  tn3270 does not check malloc return code..may modify
 * low-mem
 * review comment: make the process sleep and retry on malloc failures
 *
 * Revision 3.4  1996/01/25  08:37:43  irfan
 * CSCdi47466:  tn3270 does not check malloc return code..may modify
 * low-mem
 * get it to check malloc rc. pretty up some long statements
 *
 * Revision 3.3  1995/12/22  05:04:07  vandys
 * CSCdi43297:  CS-516s crash when all lines have ascii terms doing TN3270
 * Protect against NULL case in unstructured screen.  Fix
 * misbound else found as part of code inspection.
 *
 * Revision 3.2  1995/11/17  18:51:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:37:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:07:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *
 *	Module :	3270_orders
 *
 *	Author :	William Palter
 *
 *	Date   :	6-Nov-1993
 *
 * 	3270 order codes Commands
 */
#include <string.h>
#include "tn3270.h"			/* Defines CISCO */
#include "3270.h"
#include "proto.h"

#ifdef CISCO				/* CISCO platform only */

#include "master.h"

#endif					/* endif CISCO */




/*
 * Order SBA
 *
 *	Set our current working address in the screen buffer to the
 *	value specified in the order.
 */

void order_sba (ScreenPtr screen,
	        struct packet_type * packet)
{
    unsigned short add;

#ifdef notdef
    if ((packet->exec[1] == 0x5d) && packet->exec[2] == 0x7f) {
	packet->exec += 3;
	if (packet->exec[0] != ORDER_SBA)
	    return;
	/* transparent write */
	if ((packet->exec[1] == 0x00) && packet->exec[2] == 0x00) {
	    packet->exec += 3;
	    (*screen->trans_write) (screen->interface_data,
				    packet->exec,
			 (packet->data + packet->field_len) - packet->exec);
	    packet->exec = packet->data + packet->field_len;
	    QUEUE_BYTE(AID_PRINTER);
	    QUEUE_BYTE(0x40);
	    QUEUE_BYTE(0x40);
	    QUEUE_END_OF_TRANS;
	    return;
	}
    }

#endif /*notdef*/
    add = BUFFER_TO_ADDRESS(packet->exec[1], packet->exec[2]);

    tn3270_debug(screen, DEBUG_DATASTREAM, "ORDER SBA %02x%02x %04x\n", 
		 packet->exec[1], packet->exec[2], add); 


    screen->current_address = add % screen->current_size;
    packet->exec += 3;

    return;
}




/*
 * order SF
 *
 *	Add a Field to the buffer, using the old style command
 *	only the field_attribute can be set.
 *
 */


void order_sf (ScreenPtr screen,
	       struct packet_type * packet)
{
    struct field_attribute_type attribute;


    tn3270_debug(screen, DEBUG_DATASTREAM, "ORDER SF %02x %04x \n",
		 packet->exec[1], screen->current_address);

    memset(&attribute, 0, sizeof(attribute));

    /*
     * Clear the attribute
     */


    attribute.field_attribute = packet->exec[1];

    /*
     * If there already is an attribute at this screen location then modify
     * it, otherwise add a new field
     */

    if ((screen->screen[screen->current_address] == ORDER_SF) ||
	(screen->screen[screen->current_address] == ORDER_SFE)) {
	field_modify(screen, screen->current_address, &attribute); 
    } else {
	if (!field_add(screen, screen->current_address, &attribute)) {
	    return;
	}
    }

    /*
     * put the field order byte into the screen buffer, so that we can know
     * it when we need to build read buffers
     */

    if ((screen->screen[screen->current_address] != ORDER_SF) &&
	(screen->screen[screen->current_address] != ORDER_SFE) &&
	(screen->screen[screen->current_address] != EBCDIC_SPACE) &&
	(screen->screen[screen->current_address] != EBCDIC_NULL))
	screen->tags[screen->current_address] = 1;

    screen->screen[screen->current_address] = ORDER_SF;
  
    /*
     * Increment the buffer location, and the poinrt to the next byte in the
     * packet to try and execute
     */

    INCREMENT_ADDRESS(screen->current_address);
    packet->exec += 2;

    return;
}




/*
 * order SFE
 *
 *	Add a Field to the buffer, using the new style command
 *	any attributes can be set for the field.
 *
 */


void order_sfe (ScreenPtr screen,
	        struct packet_type * packet)
{
    unsigned char npairs;
    short i;
    struct field_attribute_type attribute;

    /*
     * See if we need to switch from base color mode to extended color mode
     */

    if ((screen->screen_clear != SCREEN_CLEAR) && (screen->color_mode == COLOR_MODE_BASE))
	screen->color_mode = COLOR_MODE_EXTENDED_DIRTY;
    else
	screen->color_mode = COLOR_MODE_EXTENDED;


    tn3270_debug(screen, DEBUG_DATASTREAM, "ORDER SFE %04x\n",
		 screen->current_address); 


    /*
     * Clear the attribute block
     */


    memset(&attribute, 0, sizeof(attribute));

    /*
     * Get the number of attribute/value pairs
     */

    npairs = packet->exec[1];

    /*
     * Scan thru the list setting the value in our attribute block
     */

    for (i = 0; i < npairs; i++) {
	switch (packet->exec[i * 2 + 2]) {
	  case EXT_ATT_3270_FIELD:
	    attribute.field_attribute = packet->exec[i * 2 + 3];
	    break;
	  case EXT_ATT_FIELD_VALIDATION:
	    attribute.field_validation = packet->exec[i * 2 + 3];
	    break;
	  case EXT_ATT_FIELD_OUTLINING:
	    attribute.field_outlining = packet->exec[i * 2 + 3];
	    break;
	  case EXT_ATT_HIGHLIGHTING:
	    attribute.extended_highlight = packet->exec[i * 2 + 3];
	    break;
	  case EXT_ATT_FOREGROUND_COLOR:
	    attribute.foreground_color = packet->exec[i * 2 + 3];
	    break;
	  case EXT_ATT_CHARACTER_SET:
	    attribute.character_set = packet->exec[i * 2 + 3];
	    break;
	  case EXT_ATT_BACKGROUND_COLOR:
	    attribute.background_color = packet->exec[i * 2 + 3];
	    break;
	  case EXT_ATT_TRANSPARENCY:
	    attribute.transparency = packet->exec[i * 2 + 3];
	    break;
	};
    }


    /*
     * Place the field into our list of fields, if this location is already a
     * field mark than just modify the current field instead of deleting and
     * recreating it
     */


    if ((screen->screen[screen->current_address] == ORDER_SF) ||
	(screen->screen[screen->current_address] == ORDER_SFE)) {
	field_modify(screen, screen->current_address, &attribute);
    } else {
	if (!field_add(screen, screen->current_address, &attribute)) {
	    return;
	}
    }

    if ((screen->screen[screen->current_address] != ORDER_SF) &&
	(screen->screen[screen->current_address] != ORDER_SFE) &&
	(screen->screen[screen->current_address] != EBCDIC_SPACE) &&
	(screen->screen[screen->current_address] != EBCDIC_NULL))
	screen->tags[screen->current_address] = 1;


    screen->screen[screen->current_address] = ORDER_SFE;
    memset(&screen->attribute[screen->current_address],
	   0,
	   sizeof(struct character_attribute_type));


    /* Increment the current buffer address */

    INCREMENT_ADDRESS(screen->current_address);


    /* account for the bytes that we have used from the datastream */

    packet->exec += 2 + npairs * 2;

    return;
}


void order_sa (ScreenPtr screen,
	       struct packet_type * packet)
{

    tn3270_debug(screen, DEBUG_DATASTREAM, "ORDER SA \n");

    if ((screen->screen_clear != SCREEN_CLEAR) && (screen->color_mode == COLOR_MODE_BASE))
	screen->color_mode = COLOR_MODE_EXTENDED_DIRTY;
    else
	screen->color_mode = COLOR_MODE_EXTENDED;

    switch (packet->exec[1]) {
      case EXT_ATT_ALL:
	screen->default_attribute.character_set = 0;
	screen->default_attribute.extended_highlight = 0;
	screen->default_attribute.foreground_color = 0;
	screen->default_attribute.background_color = 0;
	screen->default_attribute.transparency = 0;
	break;
      case EXT_ATT_CHARACTER_SET:
	screen->default_attribute.character_set = packet->exec[2];
	break;
      case EXT_ATT_FOREGROUND_COLOR:
	screen->default_attribute.foreground_color = packet->exec[2];
	break;
      case EXT_ATT_BACKGROUND_COLOR:
	screen->default_attribute.background_color = packet->exec[2];
	break;
      case EXT_ATT_HIGHLIGHTING:
	screen->default_attribute.extended_highlight = packet->exec[2];
	break;
      case EXT_ATT_TRANSPARENCY:
	screen->default_attribute.transparency = packet->exec[2];
	break;
    };
    packet->exec += 3;

    return;
}

void order_mf (ScreenPtr screen
	      ,struct packet_type * packet)
{
    unsigned char npairs;

    if ((screen->screen_clear != SCREEN_CLEAR) && (screen->color_mode == COLOR_MODE_BASE))
	screen->color_mode = COLOR_MODE_EXTENDED_DIRTY;
    else
	screen->color_mode = COLOR_MODE_EXTENDED;

    tn3270_debug(screen, DEBUG_DATASTREAM, "ORDER MF \n");
    npairs = packet->exec[1];
    packet->exec += 2 + npairs * 2;


    return;
}


void order_ic (ScreenPtr screen,
	       struct packet_type * packet)
{

    screen->cursor_location = screen->current_address;

    tn3270_debug(screen, DEBUG_DATASTREAM, "ORDER IC %04x\n",
		 screen->cursor_location);

    packet->exec += 1;

    return;

}

void order_pt (ScreenPtr screen,
	       struct packet_type * packet)
{
    struct field_type *field;
    struct character_attribute_type char_att;
    unsigned short stop_address;

    tn3270_debug(screen, DEBUG_DATASTREAM, "ORDER PT \n");
    packet->exec += 1;

    /*
     * Find field.  Do nothing if the screen is unstructured.
     */
    field = field_find(screen, screen->current_address);
    if (field == NULL) {
	return;
    }
    RESET_CHAR_ATTRIBUTE(&char_att);

    /*
     * If we are on a field mark then just advance one place and stop
     */


    if ((field->start_address == screen->current_address) &&
	(!(field->attribute.field_attribute & FA_PROT_MASK))) {
	INCREMENT_ADDRESS(screen->current_address);
	return;
    }
    switch (screen->pt_contex) {

      case PT_CONTEX_LAST_DATA:
	stop_address = FIELD_END_ADDRESS(field);
	if (FIELD_LAST_FIELD(field) &&
	    (FIELD_END_ADDRESS(field) < screen->current_address))
	    stop_address = 0;
	while (screen->current_address != stop_address) {
	    if ((screen->screen[screen->current_address] != EBCDIC_NULL) ||
		(memcmp(&char_att,
			&(screen->attribute[screen->current_address]),
			sizeof(char_att)) != 0)) {
		screen->screen[screen->current_address] = 0;
		screen->attribute[screen->current_address] = char_att;
		screen->tags[screen->current_address] = 1;
	    }
	    INCREMENT_ADDRESS(screen->current_address);
	}
	if (stop_address == 0) {
	    screen->pt_contex = PT_CONTEX_LAST_DATA;
	    return;
	}
	field = FIELD_NEXT_FIELD(field);
	break;
      case PT_CONTEX_LAST_ORDER:
	if ((FIELD_LAST_FIELD(field) &&
	     (screen->current_address > FIELD_START_ADDRESS(field)))) {
	    screen->current_address = 0;
	    screen->pt_contex = PT_CONTEX_LAST_ORDER;
	    return;
	} else {
	    field = FIELD_NEXT_FIELD(field);
	}
	break;

    }


    screen->pt_contex = PT_CONTEX_LAST_ORDER;


    while (!FIELD_LAST_FIELD(field)) {
	if (!(field->attribute.field_attribute & FA_PROT_MASK)) {
	    screen->current_address = field->start_address;
	    INCREMENT_ADDRESS(screen->current_address);
	    return;
	}
	field = FIELD_NEXT_FIELD(field);
    }

    screen->current_address = 0;

    return;


}





/*
 * Order_ra - process an order ra in the 3270 datastream
 *
 * 	We do a little I/O optimization here, since RA is sometimes
 *	used to clear areas of the screen, we check to make sure that
 * 	we are changeing the data on the screen before taging the location
 *	as changed, so that we dont have to redraw the entrie area of the RA is
 *	we dont have to.....
 *
 */


void order_ra (ScreenPtr screen,
	       struct packet_type * packet)
{
    unsigned short stop_address;
    unsigned char byte;
    unsigned char current_byte;
    struct character_attribute_type char_att;

    char_att = screen->default_attribute;

    stop_address = BUFFER_TO_ADDRESS(packet->exec[1], packet->exec[2]);

    stop_address = stop_address % screen->current_size;

    tn3270_debug(screen, DEBUG_DATASTREAM, "ORDER RA \n");
    if (packet->exec[3] == ORDER_GE) {
	byte = packet->exec[4];
	char_att.character_set = EA_CHAR_SET_GE;
    } else {
	byte = packet->exec[3];
    }


    while (1) {
	if ((screen->screen[screen->current_address] == ORDER_SF) ||
	    (screen->screen[screen->current_address] == ORDER_SFE)) {
	    field_delete(screen, screen->current_address);
	    if (screen->display_fields)
		screen->tags[screen->current_address] = 1;
	}
	if ((byte == 0x00) || (byte == 0x40)) {
	    current_byte = screen->screen[screen->current_address];
	    if (((current_byte != 0x40) &&
		 (current_byte != 0x00) &&
		 (current_byte != ORDER_SF) &&
		 (current_byte != ORDER_SFE)) ||
		(screen->attribute[screen->current_address].extended_highlight) ||
		(char_att.extended_highlight))
		screen->tags[screen->current_address] = 1;
	    else if (!screen->tags[screen->current_address])
		screen->tags[screen->current_address] = 2;
	    screen->screen[screen->current_address] = byte;
	    screen->attribute[screen->current_address] = char_att;
	} else {
	    if ((screen->screen[screen->current_address] != byte) ||
		(memcmp(&char_att,
			&(screen->attribute[screen->current_address]),
			sizeof(char_att)) != 0)) 
		screen->tags[screen->current_address] = 1;
	    else if (!screen->tags[screen->current_address])
		screen->tags[screen->current_address] = 2;
	    screen->screen[screen->current_address] = byte;
	    screen->attribute[screen->current_address] = char_att;
	}
	INCREMENT_ADDRESS(screen->current_address);
	if (screen->current_address == stop_address)
	    break;
    }



    if (packet->exec[4] == ORDER_GE)
	packet->exec += 5;
    else
	packet->exec += 4;



}




void order_eua (ScreenPtr screen,
	        struct packet_type * packet)
{
    struct character_attribute_type char_att;
    unsigned short stop_address;
    FieldPtr current_field;

    tn3270_debug(screen, DEBUG_DATASTREAM, "ORDER EUA \n");

    RESET_CHAR_ATTRIBUTE(&char_att);
    stop_address = BUFFER_TO_ADDRESS(packet->exec[1], packet->exec[2]);

    stop_address = stop_address % screen->current_size;

    current_field = field_find(screen, screen->current_address);

    if (current_field)
	while (1) {
	    if ((screen->screen[screen->current_address] == ORDER_SF) ||
		(screen->screen[screen->current_address] == ORDER_SFE)) {
		current_field = field_find(screen, screen->current_address);
	    } else if (!(FIELD_ATTRIBUTE(current_field) & FA_PROT_MASK)) {
		if ((screen->screen[screen->current_address] != 0x00) ||
		    (memcmp(&char_att,
			    &(screen->attribute[screen->current_address]),
			    sizeof(char_att)) != 0)) {
		    screen->screen[screen->current_address] = 0x00;
		    screen->attribute[screen->current_address] = char_att;
		    screen->tags[screen->current_address] = 1;
		}
	    }
	    INCREMENT_ADDRESS(screen->current_address);
	    if (screen->current_address == stop_address)
		break;
	}
    else
	while (1) {
	    if ((screen->screen[screen->current_address] != 0x00) ||
		(memcmp(&char_att,
			&(screen->attribute[screen->current_address]),
			sizeof(char_att)) != 0)) {
		screen->screen[screen->current_address] = 0x00;
		screen->attribute[screen->current_address] = char_att;
		screen->tags[screen->current_address] = 1;
	    }
	    INCREMENT_ADDRESS(screen->current_address);
	    if (screen->current_address == stop_address)
		break;
	}

    packet->exec += 3;
}


void order_ge (ScreenPtr screen, struct packet_type * packet)
{

    tn3270_debug(screen, DEBUG_DATASTREAM, "ORDER GE \n");
    screen->screen[screen->current_address] = packet->exec[1];
    screen->tags[screen->current_address] = 1;
    screen->attribute[screen->current_address] = screen->default_attribute;
    screen->attribute[screen->current_address].character_set = EA_CHAR_SET_GE;
    INCREMENT_ADDRESS(screen->current_address);

    packet->exec += 2;



}


void order_yale (ScreenPtr screen, struct packet_type * packet)
{
    int i;

    tn3270_debug(screen, DEBUG_DATASTREAM, "ORDER YALE %x\n",
		 packet->exec[2]);
    if (packet->exec[1] != YALE_DOLLAR) {
	tn3270_debug(screen, DEBUG_DATASTREAM, "Unknown Yale Code 1 %x\n",
		     packet->exec[1]);
	packet->exec += 1;
	return;
    }
    switch (packet->exec[2]) {
      case YALE_QUERY:
      case YALE_QUERY_ATTN:
	tn3270_debug(screen, DEBUG_DATASTREAM, "YALE QUERY\n");
	QUEUE_BYTE(AID_TRANS1);
	QUEUE_SHORT(0x4040);
	QUEUE_BYTE(ORDER_SBA);
	QUEUE_BYTE(0xC1);
	QUEUE_BYTE(0x50);
	QUEUE_BYTE(ORDER_YALE);
	QUEUE_BYTE(YALE_DOLLAR);
	QUEUE_BYTE(YALE_DOLLAR);

	QUEUE_BYTE(YALE_CLEAR_MARK);
	QUEUE_BYTE(YALE_IMPROVED_NULL);
	QUEUE_BYTE(YALE_ZONES_MODE_OFF);
	QUEUE_BYTE(YALE_RESTORE_ENTER_NL);
	QUEUE_BYTE(YALE_RESTORE_FLD_CLM_TAB);
	QUEUE_BYTE(YALE_DISABLE_ASYNC_STATUS);
	QUEUE_BYTE(YALE_3270_NUM_ONLY);
	QUEUE_BYTE(YALE_RESTORE_PACING);
	QUEUE_BYTE(YALE_APL_OFF);
	QUEUE_BYTE(YALE_3278_INSERT);
	QUEUE_BYTE(YALE_PRIMARY_ATT_DIS);
	QUEUE_BYTE(YALE_UNKNOWN1);
	QUEUE_BYTE(YALE_SET_HOME_LEFT_COL);
	for (i = 0; i < 30; i++)
	    QUEUE_BYTE(EBCDIC_SPACE);
	QUEUE_BYTE(YALE_DOLLAR);
	QUEUE_END_OF_TRANS;
	packet->exec += 3;
	return;
      default:
	tn3270_debug(screen, DEBUG_DATASTREAM, "Unknown Yale Code 2 %x\n",
		     packet->exec[2]);
	packet->exec += 2;
	return;

    }


    /* never reach */
    packet->exec += 1;

}
