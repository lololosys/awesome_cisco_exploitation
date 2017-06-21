/* $Id: write.c,v 3.3.44.2 1996/03/27 23:29:20 irfan Exp $
 * $Source: /release/111/cvs/Xsys/tn3270/write.c,v $
 *------------------------------------------------------------------
 * W R I T E . C
 *
 * July, 1994, Bill Palter
 * 
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: write.c,v $
 * Revision 3.3.44.2  1996/03/27  23:29:20  irfan
 * CSCdi51821:  tn3270 should allow typeahead
 * Branch: California_branch
 * Add typeahead buffer to hold keyboard input while keyboard appears
 * locked. Add a TN3270 UI knob to control this behaviour. Reduce the
 * amount
 * of whitespace painted
 *
 * Revision 3.3.44.1  1996/03/18  22:19:14  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.12.2  1996/03/07  10:54:20  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.12.1  1996/02/20  21:22:59  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/12/22  05:04:05  vandys
 * CSCdi43297:  CS-516s crash when all lines have ascii terms doing TN3270
 * Protect against NULL case in unstructured screen.  Fix
 * misbound else found as part of code inspection.
 *
 * Revision 3.2  1995/11/17  18:52:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:37:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/04  18:30:32  satya
 * CSCdi37917:  tn3270:slow memory leak
 *
 * Revision 2.2  1995/07/31  07:11:37  satya
 * CSCdi37917:  tn3270:slow memory leak
 *
 * Revision 2.1  1995/06/07  23:08:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *
 *	Module :	3270_write
 *
 *	Author :	William Palter
 *
 *	Date   :	6-Nov-1993
 *
 * 	3270 Write Commands
 */

#include "tn3270.h"			/* Defines CISCO */
#include "3270.h"
#include "proto.h"

#ifdef CISCO				/* CISCO platform only */

#include "master.h"
#include <string.h>
#include <ciscolib.h>

#endif					/* endif CISCO */


void cmd_erase_write (ScreenPtr screen, struct packet_type * packet)
{
    unsigned char wcc;

    wcc = *packet->exec++;
    tn3270_debug(screen, DEBUG_DATASTREAM, "CMD_EW %02x ", wcc);


    if (wcc & WCC_HIGH_BIT)
	tn3270_debug(screen, DEBUG_DATASTREAM, " High_bit");
    if (wcc & WCC_RESET)
	tn3270_debug(screen, DEBUG_DATASTREAM, " Reset");
    if (wcc & WCC_START_PRINTER)
	tn3270_debug(screen, DEBUG_DATASTREAM, " Start_Printer");
    if (wcc & WCC_KEYBOARD_RESTORE) {
	screen->keyboard_state = KEYBOARD_UNLOCKED;
	screen->error_state = ERROR_NONE;
	if (screen->transparent_mode == TRANS_OFF)
		(*screen->status_msg) (screen->interface_data, 
					MSG_STATUS, STATUS_NORMAL);
	tn3270_debug(screen, DEBUG_DATASTREAM, " Keyboard_restore");
    }
    if (wcc & WCC_RESET_MDT)
	tn3270_debug(screen, DEBUG_DATASTREAM, " Reset_MDT");

    tn3270_debug(screen, DEBUG_DATASTREAM, "\n");

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

    clear_screen(screen);

    (*screen->change_size) (screen->interface_data,
			    screen->current_width,
			    screen->current_height,
			    screen->current_cell_x,
			    screen->current_cell_y);


    screen->pt_contex = PT_CONTEX_LAST_ORDER;


    if ((int)(packet->field_len) > 6) {
    	if ((packet->exec[0] == ORDER_SBA) &&
    	    (packet->exec[1] == 0x5d)	   &&
    	    (packet->exec[2] == 0x7f)) {
        	if ((packet->exec[3] == ORDER_SBA) &&
    		    (packet->exec[4] == 0x00)	   &&
    	    	    (packet->exec[5] == 0x00)) {
                    /* Transaparent Write */
    	    	    transparent_write (screen,packet);
    	    	} else if ((packet->exec[3] == ORDER_SBA) &&
    		    (packet->exec[4] == 0x00)	   &&
    	    	    ((packet->exec[5] == 0x01) || (packet->exec[5] == 0x05))) {
                    /* Transaparent Read/Write */
    	    	    transparent_read_write (screen,packet);
    	    	} else {
    	    	    if (!(screen->transparent_mode == TRANS_OFF)) {
			transparent_mode_end (screen);
		    }
		}
        } else {
    	    	if (!(screen->transparent_mode == TRANS_OFF)) {
    	    	    transparent_mode_end (screen);
		}
	}
    } else {
    	if (!(screen->transparent_mode == TRANS_OFF)) {
    	    transparent_mode_end (screen);
	}
    }

    if (screen->transparent_mode  == TRANS_OFF) {
        write_data(screen, packet);
        update_screen(screen);
#ifdef DISFLD
        display_fields(screen);
#endif
        if (wcc & WCC_SOUND_ALARM)
        	(*screen->ring_bell) (screen->interface_data);
        (*screen->set_cursor_location) (screen->interface_data, 
                                        screen->cursor_location);
    }


}


void cmd_erase_write_alt (ScreenPtr screen, struct packet_type * packet)
{
    unsigned char wcc;

    wcc = *packet->exec++;
    tn3270_debug(screen, DEBUG_DATASTREAM, "CMD_EWA %02x", wcc);
    if (wcc & WCC_HIGH_BIT)
	tn3270_debug(screen, DEBUG_DATASTREAM, " High_bit");
    if (wcc & WCC_RESET)
	tn3270_debug(screen, DEBUG_DATASTREAM, " Reset");
    if (wcc & WCC_START_PRINTER)
	tn3270_debug(screen, DEBUG_DATASTREAM, " Start_Printer");

    if (wcc & WCC_KEYBOARD_RESTORE) {
	screen->keyboard_state = KEYBOARD_UNLOCKED;
	screen->error_state = ERROR_NONE;
	if (screen->transparent_mode == TRANS_OFF)
		(*screen->status_msg) (screen->interface_data,
					MSG_STATUS, STATUS_NORMAL);
	tn3270_debug(screen, DEBUG_DATASTREAM, " Keyboard_restore");
    }
    if (wcc & WCC_RESET_MDT)
	tn3270_debug(screen, DEBUG_DATASTREAM, " Reset_MDT");

    tn3270_debug(screen, DEBUG_DATASTREAM, "\n");



    screen->current_address = 0;
    screen->partition_mode = PARTITION_MODE_IMPLICIT;
    screen->color_mode = COLOR_MODE_BASE;
    screen->screen_clear = SCREEN_CLEAR;
    screen->current_width = screen->alternate_width;
    screen->current_height = screen->alternate_height;
    screen->current_size = screen->current_width * screen->current_height;
    screen->current_cell_x = screen->alternate_cell_x;
    screen->current_cell_y = screen->alternate_cell_y;
    screen->cursor_location = 0;
    screen->AID = AID_NOAID;

    clear_screen(screen);

    (*screen->change_size) (screen->interface_data,
			    screen->current_width,
			    screen->current_height,
			    screen->current_cell_x,
			    screen->current_cell_y);

    screen->pt_contex = PT_CONTEX_LAST_ORDER;


    if ((int)(packet->field_len)>6)  
    	if ((packet->exec[0] == ORDER_SBA) &&
    	    (packet->exec[1] == 0x5d)	   &&
    	    (packet->exec[2] == 0x7f)) {
        	if ((packet->exec[3] == ORDER_SBA) &&
    		    (packet->exec[4] == 0x00)	   &&
    	    	    (packet->exec[5] == 0x00)) {
                    /* Transaparent Write */
    	    	    transparent_write (screen,packet);
    	    	} else if ((packet->exec[3] == ORDER_SBA) &&
    		    (packet->exec[4] == 0x00)	   &&
    	    	    ((packet->exec[5] == 0x01) || (packet->exec[5] == 0x05))) {
                    /* Transaparent Read/Write */
    	    	    transparent_read_write (screen,packet);
    	    	} else
    	    	    if (!(screen->transparent_mode == TRANS_OFF))
    	    	    	    transparent_mode_end (screen);
        } else
    	    	if (!(screen->transparent_mode == TRANS_OFF))
    	    	    transparent_mode_end (screen);
    else
    	if (!(screen->transparent_mode == TRANS_OFF))
    	    transparent_mode_end (screen);



    if (screen->transparent_mode  == TRANS_OFF) {
        write_data(screen, packet);
        update_screen(screen);
#ifdef DISFLD
        display_fields(screen);
#endif
        if (wcc & WCC_SOUND_ALARM)
        	(*screen->ring_bell) (screen->interface_data);
        (*screen->set_cursor_location) (screen->interface_data, 
                                        screen->cursor_location);
    }

}

void cmd_write (ScreenPtr screen, struct packet_type * packet)
{
    unsigned char wcc;

    wcc = *packet->exec++;
    tn3270_debug(screen, DEBUG_DATASTREAM, "CMD_W %02x", wcc);
    if (wcc & WCC_HIGH_BIT)
	tn3270_debug(screen, DEBUG_DATASTREAM, " High_bit");
    if (wcc & WCC_RESET)
	tn3270_debug(screen, DEBUG_DATASTREAM, " Reset");
    if (wcc & WCC_START_PRINTER)
	tn3270_debug(screen, DEBUG_DATASTREAM, " Start_Printer");
    if (wcc & WCC_RESET_MDT)
	tn3270_debug(screen, DEBUG_DATASTREAM, " Reset_MDT");


    if (wcc & WCC_KEYBOARD_RESTORE) {
	screen->keyboard_state = KEYBOARD_UNLOCKED;
	screen->error_state = ERROR_NONE;
	if (screen->transparent_mode == TRANS_OFF)
		(*screen->status_msg) (screen->interface_data,
					MSG_STATUS, STATUS_NORMAL);
	tn3270_debug(screen, DEBUG_DATASTREAM, " Keyboard_restore");
    }
    tn3270_debug(screen, DEBUG_DATASTREAM, "\n");




    if (wcc & WCC_RESET_MDT)
	field_reset_mdt_all(screen);

    screen->pt_contex = PT_CONTEX_LAST_ORDER;

    if ((int)(packet->field_len)>6)   {
    	if ((packet->exec[0] == ORDER_SBA) &&
    	    (packet->exec[1] == 0x5d)	   &&
    	    (packet->exec[2] == 0x7f)) {
        	if ((packet->exec[3] == ORDER_SBA) &&
    		    (packet->exec[4] == 0x00)	   &&
    	    	    (packet->exec[5] == 0x00)) {
                    /* Transaparent Write */
    	    	    transparent_write (screen,packet);
    	    	} else if ((packet->exec[3] == ORDER_SBA) &&
    		    (packet->exec[4] == 0x00)	   &&
    	    	    ((packet->exec[5] == 0x01) || (packet->exec[5] == 0x05))) {
                    /* Transaparent Read/Write */
    	    	    transparent_read_write (screen,packet);
    	    	} else
    	    	    if (!(screen->transparent_mode == TRANS_OFF))
    	    	    	    transparent_mode_end (screen);
        } else
    	    	if (!(screen->transparent_mode == TRANS_OFF))
    	    	    transparent_mode_end (screen);
    else
    	if (!(screen->transparent_mode == TRANS_OFF))
    	    transparent_mode_end (screen);
    }



    if (screen->transparent_mode  == TRANS_OFF) {
        write_data(screen, packet);
        update_screen(screen);
#ifdef DISFLD
        display_fields(screen);
#endif
        if (wcc & WCC_SOUND_ALARM)
        	(*screen->ring_bell) (screen->interface_data);
        (*screen->set_cursor_location) (screen->interface_data, 
                                        screen->cursor_location);
    }

}






void write_data (ScreenPtr screen, struct packet_type * packet)
{

    screen->pt_contex = PT_CONTEX_LAST_ORDER;

    while (packet->exec < (packet->data + packet->field_len)) {
	switch (*packet->exec) {

	  case ORDER_SBA:
	    order_sba(screen, packet);
	    screen->pt_contex = PT_CONTEX_LAST_ORDER;
	    break;
	  case ORDER_SF:
	    order_sf(screen, packet);
	    screen->pt_contex = PT_CONTEX_LAST_ORDER;
	    break;
	  case ORDER_SFE:
	    order_sfe(screen, packet);
	    screen->pt_contex = PT_CONTEX_LAST_ORDER;
	    break;
	  case ORDER_SA:
	    order_sa(screen, packet);
	    screen->pt_contex = PT_CONTEX_LAST_ORDER;
	    break;
	  case ORDER_MF:
	    order_mf(screen, packet);
	    screen->pt_contex = PT_CONTEX_LAST_ORDER;
	    break;
	  case ORDER_IC:
	    order_ic(screen, packet);
	    screen->pt_contex = PT_CONTEX_LAST_ORDER;
	    break;
	  case ORDER_PT:
	    order_pt(screen, packet);
	    break;
	  case ORDER_RA:
	    order_ra(screen, packet);
	    screen->pt_contex = PT_CONTEX_LAST_ORDER;
	    break;
	  case ORDER_EUA:
	    order_eua(screen, packet);
	    screen->pt_contex = PT_CONTEX_LAST_ORDER;
	    break;
	  case ORDER_GE:
	    order_ge(screen, packet);
	    screen->pt_contex = PT_CONTEX_LAST_ORDER;
	    break;
	  case ORDER_YALE:
	    order_yale(screen, packet);
	    screen->pt_contex = PT_CONTEX_LAST_ORDER;
	    break;
	  case 0x00:
	  case 0x40:
	    /*
	     * If this is a field attribute delete it
	     */
	    if ((screen->screen[screen->current_address] == ORDER_SF) ||
		(screen->screen[screen->current_address] == ORDER_SFE))
		field_delete(screen, screen->current_address);
	    /*
	     * If this location is already a blank and it has no
	     * extended character attributes that effect blank space
	     * then dont update the tags
	     */
	    if (((screen->screen[screen->current_address] != 0x00) &&
		 (screen->screen[screen->current_address] != 0x40) &&
		 (screen->screen[screen->current_address] != ORDER_SF) &&
		 (screen->screen[screen->current_address] != ORDER_SFE)) ||
		(screen->default_attribute.extended_highlight) ||
		(screen->attribute[screen->current_address].extended_highlight)) {
		    screen->tags[screen->current_address] = 1;
	    } else {
		if (!screen->tags[screen->current_address])
		    screen->tags[screen->current_address] = 2;
	    }

	    screen->screen[screen->current_address] = *packet->exec++;
	    screen->attribute[screen->current_address] =
		screen->default_attribute;
	    INCREMENT_ADDRESS(screen->current_address);
	    screen->pt_contex = PT_CONTEX_LAST_DATA;
	    break;

         default:
	    if ((screen->screen[screen->current_address] == ORDER_SF) ||
		(screen->screen[screen->current_address] == ORDER_SFE)) {

		field_delete(screen, screen->current_address);
	    }
	    if ((screen->screen[screen->current_address] != *packet->exec) ||
		(!COMP_CHAR_ATT(&screen->attribute[screen->current_address], &screen->default_attribute))) {

		screen->tags[screen->current_address] = 1;
	    } else {
		if (!screen->tags[screen->current_address])
		    screen->tags[screen->current_address] = 2; 
	    }

	    screen->screen[screen->current_address] = *packet->exec++;
	    screen->attribute[screen->current_address] =
		screen->default_attribute;
	    INCREMENT_ADDRESS(screen->current_address);
	    screen->pt_contex = PT_CONTEX_LAST_DATA;
	    break;

	}
    }

}

void clear_screen (ScreenPtr screen)
{

    screen->default_attribute.character_set = 0;
    screen->default_attribute.extended_highlight = 0;
    screen->default_attribute.foreground_color = 0;
    screen->default_attribute.background_color = 0;
    screen->default_attribute.transparency = 0;

    memset(screen->screen, 0, sizeof(screen->screen));
    memset(screen->tags, 0, sizeof(screen->tags));
    memset(screen->attribute, 0, sizeof(screen->attribute));

    field_delete_all(screen);

    (screen->clear_screen) (screen->interface_data);
}


void transparent_write (ScreenPtr screen,PacketPtr packet)
{
    screen->transparent_mode = TRANS_WRITE;
    (*screen->trans_mode) (screen->interface_data,TRANS_WRITE);

    packet->exec += 6;
    (*screen->trans_write) (screen->interface_data,
			packet->exec,
			(packet->data + packet->field_len) - packet->exec);
    packet->exec = packet->data + packet->field_len;
    QUEUE_BYTE(AID_PRINTER);
    QUEUE_BYTE(0x40);
    QUEUE_BYTE(0x40);
    QUEUE_END_OF_TRANS;
}


void transparent_read_write (ScreenPtr screen,PacketPtr packet)
{
    screen->transparent_mode = TRANS_READ;
    (*screen->trans_mode) (screen->interface_data,TRANS_READ);
    packet->exec += 6;
    (*screen->trans_write) (screen->interface_data,
			packet->exec,
			(packet->data + packet->field_len) - packet->exec);
    packet->exec = packet->data + packet->field_len;
    QUEUE_BYTE(AID_PRINTER);
    QUEUE_BYTE(0x40);
    QUEUE_BYTE(0x40);

}




void transparent_mode_end (ScreenPtr screen) {


    screen->transparent_mode = TRANS_OFF;
    (*screen->trans_mode) (screen->interface_data,TRANS_OFF);

    if (screen->IBM_Buf_Ptr != screen->IBM_Buf)
    	    QUEUE_END_OF_TRANS;

}

