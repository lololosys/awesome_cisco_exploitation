/* $Id: update.c,v 3.2.60.1 1996/03/18 22:19:12 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/tn3270/update.c,v $
 *------------------------------------------------------------------
 * U P D A T E . C
 *
 * July, 1994, Bill Palter
 * 
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: update.c,v $
 * Revision 3.2.60.1  1996/03/18  22:19:12  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.2  1996/03/07  10:54:18  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.24.1  1996/02/20  21:22:55  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:52:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:37:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:08:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <string.h>
#include "tn3270.h"			/* Defines CISCO */
#include "3270.h"
#include "proto.h"

#ifdef CISCO				/* CISCO platform only */

#include "master.h"

#else

#include <stdio.h>

#endif					/* endif CISCO */



void  optimize_and_output_characters(ScreenPtr screen,
                                     unsigned short i,
                                     unsigned short len,
                                     struct interface_attribute_type *attribute)
{
  int j;
  int ioindex;
  int iolen;
  int count;


  /* Get rid of any chunks largers than 5 characters that
   * dont need to be output, trim the begining and end
   * of the IO segment
   */


  while (len) {
    /* trim the begining */
    while (len && (screen->tags[i] == 2))
      len--,i++;
    ioindex = i;
    iolen = 0;
    count = 0;

    /* find the chucks, note that when we found one
     * we just break; the begining and end trimming
     * code will take care of it for us
     * the threshold of 5 may be tuned.....
     */
    while ((iolen < len) && (screen->tags[ioindex+iolen])) {
      if (screen->tags[ioindex+iolen] == 2)
        count++;
      else
        count = 0;
      if (count == 5) {
        break;
      }
      iolen++;
    }

    /* trim the end of the IO */
    j = i+iolen-1;
    while (iolen && (screen->tags[j] == 2)) {
      iolen--;
      j--;
    }


    (*screen->put_chars) (screen->interface_data,
                          &screen->screen[ioindex], iolen,
                          i,
                          attribute);
    len -= iolen;
    i = ioindex + iolen;
  }
}

#define ISBLANK(x)  ((screen->screen[i] == EBCDIC_NULL) ||     \
                     (screen->screen[i] == EBCDIC_SPACE) ||    \
                     (screen->screen[i] == ORDER_SF) ||        \
                     (screen->screen[i] == ORDER_SFE))


int find_end_of_displayed_data(ScreenPtr screen,int start,int end)
{
  int mod_count = 0;
  int i;
  struct field_type *field;
  struct field_attribute_type field_att;


  if (!screen->use_clear_to_end_of_line)
      return end;

  i = end-1;

  field = field_find (screen,i);
  if (!field || (field->start_address == (unsigned short) i))
    memset (&field_att,0,sizeof(struct field_attribute_type));
  else
    memcpy (&field_att,&field->attribute,sizeof(struct field_attribute_type));


  while (i >= start) {
    if ((!ISBLANK(i)) ||
        (screen->attribute[i].extended_highlight) ||
        (field_att.extended_highlight))
      break;
    if (screen->tags[i] == 1)
      mod_count++;
    if ((screen->screen[i] == ORDER_SFE) ||
	(screen->screen[i] == ORDER_SF)) {
      i--;
      field = field_find (screen,(unsigned short)i);
      if (!field || (field->start_address == (unsigned short)i))
        memset (&field_att,0,sizeof(struct field_attribute_type));
      else
        memcpy (&field_att,&field->attribute,sizeof(struct field_attribute_type)
);
    } else {
      i--;
    }
  }

  if (mod_count)
    return (++i);
  else
    return (end);

}
 

void update_screen (ScreenPtr screen)
{
    unsigned short i, j;
    struct interface_attribute_type attribute;
    struct field_type *field;
    unsigned char fld_att;
    unsigned short row, start_address, end_of_line;
    int end_of_displayed_data;

    memset(&attribute, 0, sizeof(attribute));


    update_tags(screen);


    for (row = 0; row < screen->current_height; row++) {
	start_address = row * screen->current_width;
	i = start_address;
	end_of_line = screen->current_width + start_address;
	end_of_displayed_data = find_end_of_displayed_data(screen,
							   start_address,
							   end_of_line);
	while (i < end_of_displayed_data) {
	    if (screen->tags[i]) {
		j = i;
		field = field_find(screen, i);
		if (field)
		    fld_att = field->attribute.field_attribute;
		else
		    fld_att = 0;

		if ((screen->screen[j] == ORDER_SFE) && 
		    (field->attribute.extended_highlight)) {
		    /* this is an extended field and it has an
		     * attribute that can be express on a blank
		     * space, so we need to handle it specially
		     */
		    memset(&attribute, 0, sizeof(attribute));
		    (*screen->put_chars) (screen->interface_data,
					  (unsigned char *)" ",1,
					  i,&attribute);
		    i++;
		    continue;
		}
		 

		if (screen->color_mode == COLOR_MODE_BASE) {
		    memset(&attribute, 0, sizeof(attribute));
		    if ((fld_att & FA_DISPLAY_MASK) == FA_DISPLAY_INTENSIFIED)
			attribute.highlighting |= INTERFACE_HIGHLIGHT_INTENSIFY;
		    if (FIELD_ATT_DATA_INVISIBLE(fld_att))
			attribute.highlighting |= INTERFACE_HIGHLIGHT_INVISIBLE;
		    attribute.foreground_color = FIELD_ATT_TO_COLOR(fld_att);
		}
		else
		    merge_attributes(screen, field, &screen->attribute[i],
				     &attribute);
		
		j++;
		while ((j < end_of_line) &&
		       (screen->tags[j]) &&
		       (screen->screen[j] != ORDER_SF) &&
		       (screen->screen[j] != ORDER_SFE) &&
		       (COMP_CHAR_ATT((&(screen->attribute[i])),
				      (&(screen->attribute[j]))))) {
		    j++;

		}
		optimize_and_output_characters(screen,i,j-i,&attribute);

		while (i < j) {
		    screen->tags[i] = 0;
		    i++;
		}
		i = j;
	    } else
		i++;
	}
	if (end_of_displayed_data != end_of_line)
	    (*screen->erase_line)(screen->interface_data,end_of_displayed_data);
    }
    screen->screen_clear = SCREEN_NOT_CLEAR;

}




void merge_attributes (ScreenPtr screen,
		       struct field_type * field,
		       struct character_attribute_type * char_att,
		       struct interface_attribute_type * attribute)
{

    struct field_attribute_type *field_att = NULL;
    unsigned char highlighting = 0;

    if (!screen)
	fatal(666);

    if (field) {
	field_att = &(field->attribute);
	attribute->foreground_color = field_att->foreground_color;
	attribute->background_color = field_att->background_color;
	attribute->character_set = field_att->character_set;
	attribute->transparency = field_att->transparency;
	attribute->field_outlining = field_att->field_outlining;
	highlighting = field_att->extended_highlight;
    };

    if (char_att->extended_highlight)
	highlighting = char_att->extended_highlight;

    switch (highlighting) {

      case EA_HIGHLIGHT_BLINK:
	attribute->highlighting = INTERFACE_HIGHLIGHT_BLINK;
	break;
      case EA_HIGHLIGHT_REVERSE:
	attribute->highlighting = INTERFACE_HIGHLIGHT_REVERSE;
	break;
      case EA_HIGHLIGHT_INTENSIFY:
	attribute->highlighting = INTERFACE_HIGHLIGHT_INTENSIFY;
	break;
      case EA_HIGHLIGHT_UNDERSCORE:
	attribute->highlighting = INTERFACE_HIGHLIGHT_UNDERSCORE;
	break;
      default:
	attribute->highlighting = 0;
	break;

    };


    if (char_att->foreground_color)
	attribute->foreground_color = char_att->foreground_color;
    if (char_att->background_color)
	attribute->background_color = char_att->background_color;
    if (char_att->character_set)
	attribute->character_set = char_att->character_set;
    if (char_att->transparency)
	attribute->transparency = char_att->transparency;

    if (field)
	if ((field_att->field_attribute & FA_DISPLAY_MASK) == FA_DISPLAY_INTENSIFIED)
	    attribute->highlighting |= INTERFACE_HIGHLIGHT_INTENSIFY;

    if (field)
	if (FIELD_ATT_DATA_INVISIBLE(field_att->field_attribute))
	    attribute->highlighting |= INTERFACE_HIGHLIGHT_INVISIBLE;

}






int compare_attributes (att1, att2)
    struct field_attribute_type *att1, *att2;
{
    unsigned char mask;
    unsigned char fld_att1, fld_att2;
    unsigned char force_display_all = 0;

    /*
     * First we compare the field outlining attributes, we only check the
     * over and underlines since the right and left lines only affect the
     * field attribute locations
     */
    mask = (EA_OUTLINE_MASK_OVERLINE | EA_OUTLINE_MASK_UNDERLINE);

    if ((att1->field_outlining & mask) != (att2->field_outlining & mask))
	return COMPARE_DISPLAY_ALL;


    /*
     * If there is an extended highlight attribute and it is REVERSE or
     * UNDERSCORE (thoes affect all locations) then if the EH has changes
     * then we must update all locations in the field otherwise set a flag so
     * that if the color changes we can force all the locations to be
     * updated...
     */



    if ((att1->extended_highlight == EA_HIGHLIGHT_REVERSE) ||
	(att2->extended_highlight == EA_HIGHLIGHT_REVERSE) ||
	(att1->extended_highlight == EA_HIGHLIGHT_UNDERSCORE) ||
	(att2->extended_highlight == EA_HIGHLIGHT_UNDERSCORE)) {
	if (att1->extended_highlight != att2->extended_highlight)
	    return COMPARE_DISPLAY_ALL;
	else
	    force_display_all = 1;
    } else if (att1->extended_highlight != att2->extended_highlight)
	return COMPARE_DISPLAY_TEXT;



    if (att1->foreground_color != att2->foreground_color)
	if (force_display_all)
	    return COMPARE_DISPLAY_ALL;
	else
	    return COMPARE_DISPLAY_TEXT;


    if (att1->background_color != att2->background_color)
	if (force_display_all)
	    return COMPARE_DISPLAY_ALL;
	else
	    return COMPARE_DISPLAY_TEXT;

    if (att1->character_set != att2->character_set)
	return COMPARE_DISPLAY_TEXT;

    if (att1->transparency != att2->transparency)
	return COMPARE_DISPLAY_TEXT;


    fld_att1 = att1->field_attribute;
    fld_att2 = att2->field_attribute;

    if ((fld_att1 & FA_DISPLAY_MASK) != (fld_att2 & FA_DISPLAY_MASK))
	return COMPARE_DISPLAY_TEXT;



    if (fld_att1 != fld_att2)
	return COMPARE_DISPLAY_SAME;

    if (att1->field_validation != att2->field_validation)
	return COMPARE_DISPLAY_SAME;

    return COMPARE_EQUAL;
}




void update_tags (screen)
    screen_type *screen;

{
    unsigned short i;
    struct field_type *field;
    unsigned char att_comp;

    /*
     * If we switched from base color mode to extended color mode and there
     * is data on the screen in this operation, then mark all the existing
     * data as needing a refresh
     */


    if (screen->color_mode == COLOR_MODE_EXTENDED_DIRTY) {
	for (i = 0; i < screen->current_size; i++)
	    if ((screen->screen[i] != 0x00) &&
		(screen->screen[i] != 0x40) &&
		(screen->screen[i] != ORDER_SF) &&
		(screen->screen[i] != ORDER_SFE))
		screen->tags[i] = 1;

	screen->color_mode = COLOR_MODE_EXTENDED;
    }
    field = screen->field_first;

    /*
     * If the screen is unformatted than return
     */

    if (!field)
	return;


    while (1) {
	if (field->new) {
	    att_comp = compare_attributes(&(field->attribute), &(field->prev_att));
	    if (att_comp == COMPARE_DISPLAY_ALL) {
		for (i = field->start_address;
		     i != FIELD_END_ADDRESS(field);
		     INCREMENT_ADDRESS(i))
		    screen->tags[i] = 1;
	    } else {
		if (att_comp == COMPARE_DISPLAY_TEXT)
		    for (i = field->start_address;
			 i != FIELD_END_ADDRESS(field);
			 INCREMENT_ADDRESS(i))
			if ((screen->screen[i] != 0x00) &&
			    (screen->screen[i] != 0x40) &&
			    (screen->screen[i] != ORDER_SF) &&
			    (screen->screen[i] != ORDER_SFE))
			    screen->tags[i] = 1;
	    }
	    field->new = 0;
	}
	if (FIELD_LAST_FIELD(field))
	    break;
	field = FIELD_NEXT_FIELD(field);
    }
}
