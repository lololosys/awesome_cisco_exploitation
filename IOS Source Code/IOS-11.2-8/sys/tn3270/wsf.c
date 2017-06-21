/* $Id: wsf.c,v 3.2.62.1 1996/03/18 22:19:16 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/tn3270/wsf.c,v $
 *------------------------------------------------------------------
 * W S F . C
 *
 * July, 1994, Bill Palter
 * 
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: wsf.c,v $
 * Revision 3.2.62.1  1996/03/18  22:19:16  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  10:54:23  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  21:23:01  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:52:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:37:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/16  23:13:19  snyder
 * CSCdi42258:  spellink errors
 *              Insufficent -> Insufficient
 *
 * Revision 2.2  1995/08/25  00:30:53  satya
 * CSCdi39265:  tn3270 WSF X11 not corectly supported
 *
 * Revision 2.1  1995/06/07  23:08:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#undef REPLY_MODES
#undef FIELD_OUTLINING

/*
 *
 *	Module :	3270_wsf
 *
 *	Author :	William Palter
 *
 *	Date   :	6-Nov-1993
 *
 * 	3270 Write Structured Field operations are emulated here.
 */
#include <string.h>
#include "tn3270.h"			/* Defines CISCO */
#include "3270.h"
#include "proto.h"
#include "memtype.h"
#ifndef CISCO				/* If NOT CISCO */

#include <stdlib.h>

#else					/* If CISCO */

#include "master.h"

#endif				       /* endif CISCO */



/*
 * CMD_WSF
 *
 *	Process the entire WSF packet spliting up the seperate fields and
 *	passing them to the correct routine to be processed
 */

void cmd_wsf (ScreenPtr screen, struct packet_type * packet)
{
    unsigned int sf_length;
    unsigned short sf_code;
    unsigned char *saved_exec;

    while (packet->exec < (packet->data + packet->len)) {
        saved_exec = packet->exec;
	/*
	 * Find the length of this SF if it is zero it extends to the end of
	 * the packet
	 */

	/*
	 * First we make sure that we have enought bytes to get the length
	 */
	if (((packet->data + packet->len) - packet->exec) < 2) {
	    tn3270_debug(screen, DEBUG_DATASTREAM, 
			 "WSF Length Error (no Length)\n");
	    fatal(1);
	}
	sf_length = (packet->exec[0] << 8) + (packet->exec[1]);
	if (sf_length == 0)
	    sf_length = (packet->data + packet->len) - packet->exec;

	/*
	 * Make sure that the length field is valid
	 */

	if ((unsigned int) ((packet->data + packet->len) - packet->exec) < sf_length) {
	    tn3270_debug(screen, DEBUG_DATASTREAM, 
			 "WSF Length Error (Insufficient data for length) %d %d\n", 
			 packet->data + packet->len - packet->exec,
			 sf_length); 
	    sf_length = (packet->data + packet->len) - packet->exec;
	    fatal(1);
	}
	/*
	 * Find the SF Code
	 */

	if ((packet->exec[2] == 0x10) || (packet->exec[2] == 0x0f))
	    sf_code = (packet->exec[2] << 8) + packet->exec[3];
	else
	    sf_code = packet->exec[2];

	tn3270_debug(screen, DEBUG_DATASTREAM, "WSF Code = %x, length = %d\n",
		     sf_code, sf_length);
	tn3270_debug(screen, DEBUG_DATASTREAM, "CMD WSF ");

	switch (sf_code) {
	  case SF_READ_PARTITION:
	    sf_read_partition(screen, packet, sf_length);
	    break;

	  case SF_ERASE_RESET:
	    sf_erase_reset(screen, packet, sf_length);
	    break;

	  case SF_OUTBOUND_3270DS:
	    sf_outbound_3270ds(screen, packet, sf_length);
	    break;
#ifdef SEGMENT
	  case SF_OBJECT_PICTURE:
	    sf_object_picture(screen, packet, sf_length);
	    break;
#endif
	}
        packet->exec = saved_exec + sf_length;
    }

    tn3270_debug(screen, DEBUG_DATASTREAM, "\n");
}



/*
 * Data structures for the Query Reply Summary
 */





#ifdef VMS
readonly struct {
#else
struct {
#endif
    unsigned char qr_code;
    unsigned long em_type;
} static const qr_summary_list[] = {

    {
	QR_SUMMARY, EM_BASE
    },
    {
	QR_USABLE_AREA, EM_BASE
    },
    {
	QR_CHARACTER_SETS, EM_BASE
    },
    {
	QR_COLOR, EM_BASE
    },
    {
	QR_HIGHLIGHTING, EM_BASE
    },
#ifdef REPLY_MODES
    {
	QR_REPLY_MODES, EM_BASE
    },
#endif
#ifdef FIELD_OUTLINING
    {
	QR_FIELD_OUTLINING, EM_BASE
    },
#endif
    {
	QR_IMPLICIT_PARTITION, EM_BASE
    },
    {
	QR_STORAGE_POOLS, EM_GRAPHICS
    },
    {
	QR_TRANSPARENCY, EM_GRAPHICS
    },
    {
	QR_SEGMENT, EM_GRAPHICS
    },
    {
	QR_PROCEDURE, EM_GRAPHICS
    },
    {
	QR_LINE_TYPE, EM_GRAPHICS
    },
    {
	QR_GRAPHICS_COLOR, EM_GRAPHICS
    },
    {
	QR_GRAPHICS_SYMBOL_SETS, EM_GRAPHICS
    },
    {
	0, 0
    }
};

/*
 * The following defines the QR codes that need to be returned
 * on a standard query and query list equiv....
 */


#ifdef VMS
readonly unsigned char qr_equiv_list[] = {
#else
const unsigned char qr_equiv_list[] = {
#endif
    QR_SUMMARY,
    QR_TEXT_PARTITIONS,
    QR_ALPHANUMERIC_PARTITIONS,
    QR_CHARACTER_SETS,
    QR_COLOR,
    QR_HIGHLIGHTING,
    QR_IMPLICIT_PARTITION,
    QR_REPLY_MODES,
    QR_USABLE_AREA,
    QR_FIELD_VALIDATION,
    QR_MSR_CONTROL,
    QR_FIELD_OUTLINING,
    QR_OEM_AUXILIARY_DEVICE,
    QR_FORMAT_PRESENTATION,
    QR_DBCS_ASIA,
    QR_FORMAT_STORAGE_AUXILIARY_DEVICE,
    QR_DISTRIBUTED_DATA_MANAGEMENT,
    QR_DOCUMENT_INTERCHANGE_ARCHITECTURE,
    QR_DATA_CHAINING,
    QR_AUXILIARY_DEVICE,
    QR_3270_IPDS,
    QR_IBM_AUXILIARY_DEVICE,
    QR_DEVICE_CHARACTERISTICS,
    QR_RPQ_NAMES,
    QR_DATA_STREAMS,
    QR_PAPER_FEED_TECHNIQUES,
    QR_COOPERATIVE_PROC_REQUESTOR,
    0
};




void sf_read_partition (ScreenPtr screen,
		        struct packet_type * packet,
		        unsigned long sf_length)
{
    unsigned char qr_sent[256];
    unsigned char reply_count = 0;
    unsigned int i;
    unsigned int j;

    tn3270_debug(screen, DEBUG_DATASTREAM, "READ PARTITION ");
    /*
     * Make sure we have enough bytes for a Read Query Read Query List Read
     * Modified Read Buffer Read Modifed All
     * 
     * Sub commands
     */


    if (sf_length < 5) {
	tn3270_debug(screen, DEBUG_DATASTREAM, 
		     "Read Partition Length Error\n");
	fatal(1);
    }
    if ((packet->exec[4] == SF_RP_QUERY_LIST) && (sf_length < 6)) {
	tn3270_debug(screen, DEBUG_DATASTREAM, 
		     "Read Partition Query List Length Error\n");
	fatal(1);
    }
    /*
     * Clear out the array we use to prevent duplicating query replies and
     * set the QR_NULL so we never send it out thru the normal processing
     * loop
     */
    memset(qr_sent, 0, sizeof(qr_sent));

    switch (packet->exec[4]) {
      case SF_RP_QUERY:
	QUEUE_BYTE(AID_SF);
	for (i = 0; qr_equiv_list[i]; i++) {
	    if (!qr_sent[qr_equiv_list[i]]) {
		j = sf_send_qr(screen,
			       qr_equiv_list[i]);
		qr_sent[qr_equiv_list[i]] = j;
	    } else
		j = 0;
	    reply_count += j;
	}
	if (!reply_count)
	    sf_send_qr(screen, QR_NULL);
	QUEUE_END_OF_TRANS;
	break;
      case SF_RP_QUERY_LIST:
	QUEUE_BYTE(AID_SF);
	for (i = 6; i < sf_length; i++) {
	    if (!qr_sent[packet->exec[i]]) {
		j = sf_send_qr(screen,
			       packet->exec[i]);
		qr_sent[packet->exec[i]] = j;
	    } else
		j = 0;
	    reply_count += j;
	}
	if (!reply_count)
	    sf_send_qr(screen, QR_NULL);
	QUEUE_END_OF_TRANS;
	break;

      default:
	tn3270_debug(screen, DEBUG_DATASTREAM, "Unknown Read Partition %d\n",
		     packet->exec[4]);
	break;
    }
}





int sf_send_qr (ScreenPtr screen, unsigned char qr_code)
{

    switch (qr_code) {
	case QR_SUMMARY:
	return (qr_summary(screen));
      case QR_COLOR:
	return (qr_color(screen));
      case QR_HIGHLIGHTING:
	return (qr_highlighting(screen));
      case QR_CHARACTER_SETS:
	return (qr_character_sets(screen));
#ifdef FIELD_OUTLINING
      case QR_FIELD_OUTLINING:
	return (qr_field_outlining(screen));
#endif
#ifdef REPLY_MODES
      case QR_REPLY_MODES:
	return (qr_reply_modes(screen));
#endif
      case QR_IMPLICIT_PARTITION:
	return (qr_implicit_partition(screen));
      case QR_USABLE_AREA:
	return (qr_usable_area(screen));
      case QR_STORAGE_POOLS:
	return (qr_storage_pools(screen));
      case QR_TRANSPARENCY:
	return (qr_transparency(screen));
      case QR_SEGMENT:
	return (qr_segment(screen));
      case QR_PROCEDURE:
	return (qr_procedure(screen));
      case QR_LINE_TYPE:
	return (qr_line_type(screen));
      case QR_GRAPHICS_COLOR:
	return (qr_graphics_color(screen));
      case QR_GRAPHICS_SYMBOL_SETS:
	return (qr_graphics_symbol_sets(screen));
      case QR_NULL:
	return (qr_null(screen));

      default:
	return 0;
    }
}

int qr_summary (ScreenPtr screen)
{
    int i;


    QUEUE_START_SF(SF_QUERY_REPLY);
    QUEUE_BYTE(QR_SUMMARY);
    for (i = 0; qr_summary_list[i].qr_code; i++)
	if (screen->emulation_mask & qr_summary_list[i].em_type)
	    QUEUE_BYTE(qr_summary_list[i].qr_code);

    /*
     * length =    ((unsigned char *) screen->IBM_Buf_Ptr - (unsigned char *)
     * screen->SF_Len_Ptr) - screen->SF_expand;
     * 
     * tmp = (unsigned char *) screen->SF_Len_Ptr;
     * 
     * tmp++ =   (unsigned char) ((length & 0xff00) >> 8); tmp   =   (unsigned
     * char) (length & 0xff);
     * 
     */
    QUEUE_END_SF;

    return (1);
}

int qr_character_sets (ScreenPtr screen)
{

    QUEUE_START_SF(SF_QUERY_REPLY);
    QUEUE_BYTE(QR_CHARACTER_SETS);
    QUEUE_SHORT(0xb000);	       /* Flags : GE Supported No MultiID
				        * Load PSSF LOAD PS Extended One Slot
				        * Size No Two Byte Character Sets NO
				        * CGCSGID Load PS Slot size match req
				        * No CCSID */

    QUEUE_BYTE(0x09);		       /* Default Slot Width  */
    QUEUE_BYTE(0x10);		       /* Default Slow Height */
    QUEUE_LONG(0x40000000);	       /* Load PS Formats     */
    QUEUE_BYTE(0x03);		       /* Descriptor Lengths  */
    /* Descriptor # 1 */
    QUEUE_BYTE(0x00);		       /* Set ID 0 	 */
    QUEUE_BYTE(0x00);		       /* Flags : Not Loadable Single Plane
				        * Single Byte LCID Compare */
    QUEUE_BYTE(0x00);		       /* LCID 	 */

#ifdef APL_SUPPORTED
    /* Descriptor # 2 */
    QUEUE_BYTE(0x01);		       /* Set ID 1 	 */
    QUEUE_BYTE(0x00);		       /* Flags : Not Loadable Single Plane
				        * Single Byte LCID Compare */
    QUEUE_BYTE(0xf1);		       /* LCID 	 */
#endif

    QUEUE_END_SF;
    return (1);

}





int qr_color (ScreenPtr screen)
{

    QUEUE_START_SF(SF_QUERY_REPLY);
    QUEUE_BYTE(QR_COLOR);
    QUEUE_BYTE(0x00);		       /* Flags : none */
    QUEUE_BYTE(0x08);		       /* Eight Color Pairs */

    QUEUE_SHORT(0x00f4);	       /* Default = GREEN */
    QUEUE_SHORT(0xf1f1);	       /* Blue      */
    QUEUE_SHORT(0xf2f2);	       /* Red       */
    QUEUE_SHORT(0xf3f3);	       /* Pink      */
    QUEUE_SHORT(0xf4f4);	       /* Green     */
    QUEUE_SHORT(0xf5f5);	       /* Turquoise */
    QUEUE_SHORT(0xf6f6);	       /* Yellow    */
    QUEUE_SHORT(0xf7f7);	       /* Neutral   */
    QUEUE_END_SF;
    return (1);

}


int qr_highlighting (ScreenPtr screen)
{

    QUEUE_START_SF(SF_QUERY_REPLY);
    QUEUE_BYTE(QR_HIGHLIGHTING);
    QUEUE_BYTE(0x04);		       /* Four Pairs */
    QUEUE_SHORT(0x00f0);	       /* Default = Normal */
    QUEUE_SHORT(0xf1f1);	       /* Blink         */
    QUEUE_SHORT(0xf2f2);	       /* Reverse Video */
    QUEUE_SHORT(0xf4f4);	       /* Underscore    */
    QUEUE_END_SF;
    return (1);

}


int qr_implicit_partition (ScreenPtr screen)
{

    QUEUE_START_SF(SF_QUERY_REPLY);
    QUEUE_BYTE(QR_IMPLICIT_PARTITION);
    QUEUE_SHORT(0x0000);	       /* Flags : none */


    QUEUE_START_SDP(QR_IP_SDP_PARTITION_SIZES);
    QUEUE_BYTE(0x00);		       /* Flags : none */
    QUEUE_SHORT(screen->primary_width);/* Default Width */
    QUEUE_SHORT(screen->primary_height);	/* Default Height */
    QUEUE_SHORT(screen->alternate_width);	/* Alternate Width   */
    QUEUE_SHORT(screen->alternate_height);	/* Alternate Height */
    QUEUE_END_SDP;

    QUEUE_START_SDP(QR_IP_SDP_CELL_SIZES);	/* Character Cell Sizes */
    QUEUE_BYTE(0x00);		       /* Flags none */
    QUEUE_SHORT(screen->primary_cell_x);	/* Default Height */
    QUEUE_SHORT(screen->primary_cell_y);	/* Alternate Width */
    QUEUE_SHORT(screen->alternate_cell_x);	/* Alternate Height */
    QUEUE_SHORT(screen->alternate_cell_y);	/* Alternate Height */
    QUEUE_END_SDP;
    QUEUE_END_SF;
    return (1);

}




int qr_usable_area (ScreenPtr screen)
{

    QUEUE_START_SF(SF_QUERY_REPLY);
    QUEUE_BYTE(QR_USABLE_AREA);
    QUEUE_SHORT(0x0100);	       /* Flags : 12/14 bit addressing */
    QUEUE_SHORT(screen->primary_width);/* Width of UA = 80  */
    QUEUE_SHORT(screen->primary_height);	/* Height of UA = 24 */
    QUEUE_BYTE(0x01);		       /* Units = Millimeters */
    QUEUE_SHORT(0x0001);	       /* Delta X Numerator   */
    QUEUE_SHORT(0x0003);	       /* Delta X Denominator */
    QUEUE_SHORT(0x0001);	       /* Delta Y Numerator   */
    QUEUE_SHORT(0x0003);	       /* Delta Y Denominator */

    QUEUE_BYTE(screen->primary_cell_x);/* Default Cell X = 9  */
    QUEUE_BYTE(screen->primary_cell_y);/* Default Cell Y = 21 */
    QUEUE_SHORT(0x0000);	       /* Default Buffer Size 0 = User
				        * Alphanumeric Partition QR */
    QUEUE_BYTE(0x00);		       /* X MIN	 */
    QUEUE_BYTE(0x00);		       /* Y MIN	 */
    QUEUE_BYTE(0x00);		       /* X MAX	 */
    QUEUE_BYTE(0x00);		       /* Y MAX	 */

    QUEUE_START_SDP(QR_UA_SDP_ALTERNATE_USABLE_AREA);	/* Alternate Usable area */
    QUEUE_BYTE(0x00);		       /* Reserved */
    QUEUE_BYTE(0x01);		       /* Identifier */
    QUEUE_SHORT(screen->alternate_width);	/* Width  = 80 */
    QUEUE_SHORT(screen->alternate_height);	/* Height = 24 */
    QUEUE_BYTE(0x01);		       /* Units = Millimeters */
    QUEUE_SHORT(0x0001);	       /* Delta X Numerator   */
    QUEUE_SHORT(0x0003);	       /* Delta X Denominator */
    QUEUE_SHORT(0x0001);	       /* Delta Y Numerator   */
    QUEUE_SHORT(0x0003);	       /* Delta Y Denominator */
    QUEUE_BYTE(screen->alternate_cell_x);	/* Default Cell X = 9  */
    QUEUE_BYTE(screen->alternate_cell_y);	/* Default Cell Y = 16 */
    QUEUE_END_SDP;
    QUEUE_END_SF;
    return (1);

}


int qr_reply_modes (ScreenPtr screen)
{

    QUEUE_START_SF(SF_QUERY_REPLY);
    QUEUE_BYTE(QR_REPLY_MODES);
    QUEUE_BYTE(0x00);		       /* Field Mode */
    QUEUE_BYTE(0x01);		       /* Extended Field Mode */
    QUEUE_BYTE(0x02);		       /* Character Mode */
    QUEUE_END_SF;
    return (1);

}


void sf_erase_reset(screen, packet, sf_length)
    ScreenPtr screen;
    struct packet_type *packet;
    unsigned long sf_length;
{

    tn3270_debug(screen, DEBUG_DATASTREAM, "ERASE/RESET");

    if (sf_length != 4) {
	tn3270_debug(screen, DEBUG_DATASTREAM, "Length Error\n");
	return;
    }
    if (packet->exec[3] & SF_ER_ALT_SIZE_MASK) {
	screen->current_width = screen->alternate_width;
	screen->current_height = screen->alternate_height;
	screen->current_size = screen->current_width * screen->current_height;
	screen->current_cell_x = screen->alternate_cell_x;
	screen->current_cell_y = screen->alternate_cell_y;
    } else {
	screen->current_width = screen->primary_width;
	screen->current_height = screen->primary_height;
	screen->current_size = screen->current_width * screen->current_height;
	screen->current_cell_x = screen->primary_cell_x;
	screen->current_cell_y = screen->primary_cell_y;
    }

    screen->current_address = 0;
    screen->partition_mode = PARTITION_MODE_IMPLICIT;
    screen->color_mode = COLOR_MODE_BASE;
    screen->screen_clear = SCREEN_CLEAR;
    screen->default_attribute.character_set = 0;
    screen->default_attribute.extended_highlight = 0;
    screen->default_attribute.foreground_color = 0;
    screen->default_attribute.background_color = 0;
    screen->default_attribute.transparency = 0;
    memset(screen->screen, 0, sizeof(screen->screen));
    memset(screen->tags, 0, sizeof(screen->tags));
    memset(screen->attribute, 0, sizeof(screen->attribute));

    field_delete_all(screen);

    (screen->change_size) (screen->interface_data,
			   screen->current_width,
			   screen->current_height,
			   screen->current_cell_x,
			   screen->current_cell_y);

}



void sf_outbound_3270ds(screen, packet, sf_length)
    ScreenPtr screen;
    struct packet_type *packet;
    unsigned long sf_length;
{
    unsigned char wcc;

    tn3270_debug(screen, DEBUG_DATASTREAM, "OUTBOUND 3270 \n");


    packet->field_len = packet->exec -
	packet->data + (unsigned int) sf_length;

    wcc = packet->exec[5];
    tn3270_debug(screen, DEBUG_DATASTREAM, "CMD_EW %02x ", wcc);


    if (wcc & WCC_HIGH_BIT)
	tn3270_debug(screen, DEBUG_DATASTREAM, " High_bit");
    if (wcc & WCC_RESET)
	tn3270_debug(screen, DEBUG_DATASTREAM, " Reset");
    if (wcc & WCC_START_PRINTER)
	tn3270_debug(screen, DEBUG_DATASTREAM, " Start_Printer");
    if (wcc & WCC_KEYBOARD_RESTORE)
	tn3270_debug(screen, DEBUG_DATASTREAM, " Keyboard_restore");
    if (wcc & WCC_RESET_MDT)
	tn3270_debug(screen, DEBUG_DATASTREAM, " Reset_MDT");

    tn3270_debug(screen, DEBUG_DATASTREAM, "\n");

    if ((packet->exec[4] == CMD_SNA_EW) ||
	(packet->exec[4] == CMD_SNA_EWA)) {

	field_delete_all(screen);
	(screen->clear_screen) (screen->interface_data);
	screen->color_mode = COLOR_MODE_BASE;
	screen->screen_clear = SCREEN_CLEAR;
	memset(screen->screen, 0, sizeof(screen->screen));
	memset(screen->tags, 0, sizeof(screen->tags));
	memset(screen->attribute, 0, sizeof(screen->attribute));
    }
    screen->current_address = 0;
    screen->pt_contex = PT_CONTEX_LAST_ORDER;

    RESET_CHAR_ATTRIBUTE(&(screen->default_attribute));

    packet->exec += 6;
    write_data(screen, packet);

    update_screen(screen);

    /* display_fields (screen); */
    (*screen->set_cursor_location) (screen->interface_data, screen->cursor_location);

}

int qr_field_outlining (ScreenPtr screen)
{

    QUEUE_START_SF(SF_QUERY_REPLY);
    QUEUE_BYTE(QR_FIELD_OUTLINING);
    QUEUE_BYTE(0x00);
    QUEUE_BYTE(0x00);
    QUEUE_BYTE(0x00);
    QUEUE_BYTE(0x00);
    QUEUE_BYTE(0x00);
    QUEUE_BYTE(0x00);
    QUEUE_END_SF;
    return (1);

}


int qr_null (ScreenPtr screen)
{

    QUEUE_START_SF(SF_QUERY_REPLY);
    QUEUE_BYTE(QR_NULL);
    QUEUE_END_SF;
    return (1);

}



int qr_storage_pools (ScreenPtr screen)
{


    if (!(screen->emulation_mask & EM_GRAPHICS))
	return (0);

    QUEUE_START_SF(SF_QUERY_REPLY);
    QUEUE_BYTE(QR_STORAGE_POOLS);
    QUEUE_START_SDP(QR_SP_SDP_STORAGE_POOLS);
    QUEUE_BYTE(0x01);		       /* Pool ID	 	 */
    QUEUE_LONG(0x0000B381);	       /* Pool size Around 45K	 */
    QUEUE_LONG(0x0000B381);	       /* Available Space	 */
    QUEUE_SHORT(0x0007);	       /* Can be used for symbols */
    QUEUE_END_SDP;
    QUEUE_END_SF;
    return (1);

}



int qr_transparency (ScreenPtr screen)
{

    if (!(screen->emulation_mask & EM_GRAPHICS))
	return (0);

    QUEUE_START_SF(SF_QUERY_REPLY);
    QUEUE_BYTE(QR_TRANSPARENCY);
    QUEUE_BYTE(0x02);
    QUEUE_SHORT(0x00F0);
    QUEUE_SHORT(0xFFFF);
    QUEUE_END_SF;
    return (1);

}

int qr_segment (ScreenPtr screen)
{

    if (!(screen->emulation_mask & EM_GRAPHICS))
	return (0);

    QUEUE_START_SF(SF_QUERY_REPLY);
    QUEUE_BYTE(QR_SEGMENT);
    QUEUE_BYTE(0x00);		       /* Reserved */
    QUEUE_SHORT(0x0200);	       /* Drawing order Subset level */
    QUEUE_SHORT(0x0000);	       /* Max storable segment */
    QUEUE_SHORT(0xffff);	       /* Max segment size */

    QUEUE_START_SDP(QR_SEGMENT_SDP_PROPERTY_VECTOR);
    QUEUE_BYTE(QR_SEGMENT_PROPERTY_DRAWING_ATT)
	QUEUE_BYTE(0x80)	       /* Underpaint Honered */
	QUEUE_BYTE(0x00)
	QUEUE_END_SDP;
    /* Area Boundry */
    QUEUE_START_SDP(QR_SEGMENT_SDP_PROPERTY_VECTOR);
    QUEUE_BYTE(QR_SEGMENT_PROPERTY_PATTERN_ATT)
	QUEUE_BYTE(0x80)	       /* Area Boundry drawn */
	QUEUE_BYTE(0x00)
	QUEUE_END_SDP;


    QUEUE_END_SF;
    return (1);

}

int qr_procedure (ScreenPtr screen)
{

    if (!(screen->emulation_mask & EM_GRAPHICS))
	return (0);

    QUEUE_START_SF(SF_QUERY_REPLY);
    QUEUE_BYTE(QR_PROCEDURE);
    QUEUE_BYTE(0x00);		       /* Flags */
    QUEUE_SHORT(0x0100);	       /* Procedure subset level */
    QUEUE_SHORT(0x0000);	       /* Max storable procedure size */
    QUEUE_SHORT(0xffff);	       /* Max procedure size */
    QUEUE_SHORT(0x0600);	       /* Maximum size of Data Store */
    QUEUE_SHORT(0x0600);	       /* Maximum size of data unit */

    QUEUE_START_SDP(QR_PROCEDURE_SDP_BUILT_IN);	/* Built in procedure */
    QUEUE_LONG(0xfffffff0);	       /* BPI for output device  */
    QUEUE_END_SDP;

    QUEUE_END_SF;
    return (1);


}

int qr_line_type (ScreenPtr screen)
{

    if (!(screen->emulation_mask & EM_GRAPHICS))
	return (0);

    QUEUE_START_SF(SF_QUERY_REPLY);
    QUEUE_BYTE(QR_LINE_TYPE);
    QUEUE_SHORT(0x0009);

    QUEUE_SHORT(0x0007);
    QUEUE_SHORT(0x0101);
    QUEUE_SHORT(0x0202);
    QUEUE_SHORT(0x0303);
    QUEUE_SHORT(0x0404);
    QUEUE_SHORT(0x0505);
    QUEUE_SHORT(0x0606);
    QUEUE_SHORT(0x0707);
    QUEUE_SHORT(0x0808);
    QUEUE_END_SF;
    return (1);



}

int qr_graphics_color (ScreenPtr screen)
{

    if (!(screen->emulation_mask & EM_GRAPHICS))
	return (0);

    QUEUE_START_SF(SF_QUERY_REPLY);
    QUEUE_BYTE(QR_GRAPHICS_COLOR);
    QUEUE_BYTE(RESERVED_BYTE);
    QUEUE_BYTE(0x04);		       /* Default Foreground */
    QUEUE_BYTE(0x00);		       /* Default Background */

    if (screen->emulation_mask & EM_16_COLORS) {
	QUEUE_BYTE(0x03);	       /* Intensity Levels   */
    } else {
	QUEUE_BYTE(0x01);	       /* Intensity Levels   */
    }

    QUEUE_BYTE(0xff);		       /* Table ID */

    if (screen->emulation_mask & EM_16_COLORS) {
	QUEUE_SHORT(0x0010);	       /* Number of entries */
	QUEUE_SHORT(0x0010);	       /* Number of intensity defs */
    } else {
	QUEUE_SHORT(0x0008);	       /* Number of entries */
	QUEUE_SHORT(0x0008);	       /* Number of intensity defs */
    };

    QUEUE_SHORT(0x0000);
    QUEUE_LONG(0x00000000);	       /* Black */
    QUEUE_SHORT(0x0001);
    QUEUE_LONG(0x00010000);	       /* Blue  */
    QUEUE_SHORT(0x0002);
    QUEUE_LONG(0x01000000);	       /* Red */
    QUEUE_SHORT(0x0003);
    QUEUE_LONG(0x01010000);	       /* Pink */
    QUEUE_SHORT(0x0004);
    QUEUE_LONG(0x00000100);	       /* Green */
    QUEUE_SHORT(0x0005);
    QUEUE_LONG(0x00010100);	       /* Turquoise */
    QUEUE_SHORT(0x0006);
    QUEUE_LONG(0x01000100);	       /* Yellow */
    QUEUE_SHORT(0x0007);
    QUEUE_LONG(0x01010100);	       /* White */

    if (screen->emulation_mask & EM_16_COLORS) {
	QUEUE_SHORT(0x0008);
	QUEUE_LONG(0x02000100);	       /* Brown */
	QUEUE_SHORT(0x0009);
	QUEUE_LONG(0x00020000);	       /* Dark Blue */
	QUEUE_SHORT(0x000A);
	QUEUE_LONG(0x03000100);	       /* Orange  */
	QUEUE_SHORT(0x000B);
	QUEUE_LONG(0x03020000);	       /* Purple */
	QUEUE_SHORT(0x000C);
	QUEUE_LONG(0x00000200);	       /* Dark Green */
	QUEUE_SHORT(0x000D);
	QUEUE_LONG(0x00020300);	       /* Dark Turquoise */
	QUEUE_SHORT(0x000E);
	QUEUE_LONG(0x03000200);	       /* Mustard */
	QUEUE_SHORT(0x000F);
	QUEUE_LONG(0x02020200);	       /* Grey */
    }
    QUEUE_END_SF;
    return (1);



}

int qr_graphics_symbol_sets (ScreenPtr screen)
{

    if (!(screen->emulation_mask & EM_GRAPHICS))
	return (0);
#ifdef notdef

    QUEUE_START_SF(SF_QUERY_REPLY);
    QUEUE_BYTE(QR_GRAPHICS_SYMBOL_SETS);
    QUEUE_BYTE(QR_GSS_MASK_MULTIID | QR_GSS_MASK_LOADPS | QR_GSS_MASK_LOADPS_EXT);
    QUEUE_BYTE(QR_GSS_MASK_MULTI_PLANE);
    QUEUE_BYTE(screen->current_cell_x);
    QUEUE_BYTE(screen->current_cell_y);
    QUEUE_SHORT(0x0100);	       /* VSS Subset */
    QUEUE_BYTE(0x02);		       /* Number of format groups */
    QUEUE_LONG(0x00000000);
    QUEUE_LONG(0x70000000);


    QUEUE_START_SDP(QR_GSS_SDP_SYMBOL_STORE);
    QUEUE_BYTE(0x00);		       /* Store ID */
    QUEUE_BYTE(0x00);		       /* Flags */
    QUEUE_BYTE(0x0F);		       /* LCID */
    QUEUE_BYTE(QR_GSS_MASK_USE_PRECISION_2);	/* Capibilities */
    QUEUE_BYTE(QR_GSS_MASK_USE_PRECISION_2);	/* Current Use */
    QUEUE_BYTE(0x00);		       /* Load format */
    QUEUE_BYTE(0x00);		       /* Current Load format */
    QUEUE_BYTE(0x00);		       /* Width */
    QUEUE_BYTE(0x00);		       /* Height */
    QUEUE_BYTE(RESERVED_BYTE);
    QUEUE_LONG(0x00650025);	       /* GCGID */
    QUEUE_END_SDP;

    QUEUE_START_SDP(QR_GSS_SDP_SYMBOL_STORE);
    QUEUE_BYTE(0x00);		       /* Store ID */
    QUEUE_BYTE(0x00);		       /* Flags */
    QUEUE_BYTE(0x0F);		       /* LCID */
    QUEUE_BYTE(QR_GSS_MASK_USE_PRECISION_3 | QR_GSS_MASK_USE_NOPATTERNS);	/* Capibilities */
    QUEUE_BYTE(QR_GSS_MASK_USE_PRECISION_3 | QR_GSS_MASK_USE_NOPATTERNS);	/* Current Use */
    QUEUE_BYTE(0x00);		       /* Load format */
    QUEUE_BYTE(0x00);		       /* Current Load format */
    QUEUE_BYTE(0x00);		       /* Width */
    QUEUE_BYTE(0x00);		       /* Height */
    QUEUE_BYTE(RESERVED_BYTE);
    QUEUE_LONG(0x00650025);	       /* GCGID */
    QUEUE_END_SDP;


    QUEUE_END_SF;
#endif
    return (1);
}

 /* MARK */
