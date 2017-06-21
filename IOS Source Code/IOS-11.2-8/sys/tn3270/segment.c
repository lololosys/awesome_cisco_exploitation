/* $Id: segment.c,v 3.2 1995/11/17 18:51:37 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/tn3270/segment.c,v $
 *------------------------------------------------------------------
 * S E G M E N T . C
 *
 * July, 1994, Bill Palter
 * 
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: segment.c,v $
 * Revision 3.2  1995/11/17  18:51:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:37:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:08:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * 
 *	Module :	3270_segment
 *
 *	Author :	William Palter
 *
 *	Date   :	3-Nov-1994
 *
 * 	
 */
#include "tn3270.h"
#include "3270.h"
#include "proto.h"

long graphics_order_length (unsigned char *buffer);


void graphics_line_abs (screen_type *screen,unsigned char *buffer);


void graphics_segment (screen,segment)
screen_type *screen;
struct segment_type *segment;
{

	segment->exec = segment->segment_data;
	
	while (segment->exec < (segment->segment_data + segment->length)) {
		switch (segment->exec[0]) {
                 	case GSO_GLINE :
                 	case GSO_GCLINE:
			                 graphics_line_abs (screen,segment->exec);
					 break;
		}

		segment->exec += graphics_order_length (segment->exec);

	}





}

long graphics_order_length (buffer)
unsigned char *buffer;
{
	if ((buffer[0] == GSO_GESD) || (buffer[0] == GSO_GNOP1))
		return 1;
        else if (buffer[0] & GSO_TWO_BYTE_MASK)
		return 2;
	else
		return (buffer[1] + 2);
	
}



void graphics_line_abs (screen,buffer)
screen_type *screen;
unsigned char *buffer;
{
struct point_type points[255];
struct graphics_contex_type *graphics_contex = screen->graphics_contex;
short index;
short i;

	index = 0;

	if (buffer[0] == GSO_GCLINE) 
         	points[index++] = graphics_contex->current_point;

	for (i=0;i<buffer[1];i+=4) {
		points[index].x = ((buffer[i +0+2] << 8) | (buffer[i +1+2]));
		points[index].y = ((buffer[i +2+2] << 8) | (buffer[i +3+2]));
		index++;
	}

	graphics_contex->current_point = points[index-1];

	(screen->draw_lines) (screen,points,index);
}



