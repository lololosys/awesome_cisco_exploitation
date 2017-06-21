/* $Id: fields.c,v 3.6.20.1 1996/03/18 22:18:57 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/tn3270/fields.c,v $
 *------------------------------------------------------------------
 * F I E L D S . C
 *
 * July, 1994, Bill Palter
 * 
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: fields.c,v $
 * Revision 3.6.20.1  1996/03/18  22:18:57  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  10:53:55  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  21:22:25  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/01/25  11:10:29  irfan
 * CSCdi47499:  memory leaks in tn3270
 *
 * Revision 3.5  1996/01/25  11:05:54  irfan
 * CSCdi47499:  memory leaks in tn3270
 *
 * Revision 3.4  1996/01/25  10:03:03  irfan
 * CSCdi47466:  tn3270 does not check malloc return code..may modify
 * low-mem
 * review comment: make the process sleep and retry on malloc failures
 *
 * Revision 3.3  1996/01/25  08:37:41  irfan
 * CSCdi47466:  tn3270 does not check malloc return code..may modify
 * low-mem
 * get it to check malloc rc. pretty up some long statements
 *
 * Revision 3.2  1995/11/17  18:51:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:37:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/04  18:24:36  satya
 * CSCdi37917:  tn3270:slow memory leak
 *
 * Revision 2.2  1995/06/14  23:31:36  satya
 * CSCdi35861:  TN3270 code doesnt reset the modified fields (regress
 * CSCdi33295)
 *
 * Revision 2.1  1995/06/07  23:07:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <string.h>
#include "tn3270.h"			/* Defines CISCO */
#include "3270.h"
#include "memtype.h"
#include "proto.h"

#ifdef CISCO

#include "master.h"

#else

#include <stdio.h>

#endif					/* endif CISCO */


struct field_type *field_find (ScreenPtr screen, unsigned short address)
{
    struct field_type *field;



    if (!screen->field_first)
	return 0;


    field = screen->field_first->prev_field;

    while (1) {
	if (field->start_address <= address)
	    break;
	field = field->prev_field;
	if (field->flags & FIELD_LAST)
	    break;
    }


    return field;

}

void field_delete_all (ScreenPtr screen)
{
    struct field_type *next;
    struct field_type *c_field;
    int last = 1;

    if (!screen->field_first)
	return;
    next = screen->field_first;

    while (1) {
	c_field = next;
	next = c_field->next_field;
	last = c_field->flags;
	memory_deallocate(screen, c_field);
	if (last & FIELD_LAST)
	    break;
    };

    screen->field_first = 0;
    return;
}



void field_mark_all_new (ScreenPtr screen)
{
    struct field_type *next;
    struct field_type *c_field;
    int last = 1;

    if (!screen->field_first)
	return;

    next = screen->field_first;

    while (last) {
	c_field = next;
	next = next->prev_field;
	if (c_field->flags & FIELD_LAST)
	    last = 0;
	c_field->new = 1;
    };


    return;
}




void field_reset_mdt_all (ScreenPtr screen)
{
    struct field_type *next;
    struct field_type *c_field;
    int last = 1;

    if (!screen->field_first)
	return;

    next = screen->field_first;

    while (last) {
	c_field = next;
	next = c_field->next_field;
	if (c_field->flags & FIELD_LAST)
	    last = 0;
	c_field->attribute.field_attribute &= ~FA_DISPLAY_MDT_MASK;
    };


    return;
}




void display_fields (ScreenPtr screen)
{
    struct field_type *next;
    struct interface_attribute_type att;
    unsigned char tmp;
    int last = 1;
    unsigned char fld_att, dis_att;

    if (!screen->field_first)
	return;

    next = screen->field_first;

    memset(&att, 0, sizeof(att));
    att.foreground_color = 0xfa;
    att.highlighting = 0 | INTERFACE_HIGHLIGHT_REVERSE;
    while (last) {

	fld_att = next->attribute.field_attribute;
	dis_att = fld_att & FA_DISPLAY_MASK;

	if (fld_att & FA_PROT_MASK) {
	    if ((fld_att & FA_SKIP_MASK) == FA_SKIP_MASK)
		if (dis_att == FA_DISPLAY_INTENSIFIED)
		    tmp = 0xc1;
		else
		    tmp = 0x81;
	    else if (dis_att == FA_DISPLAY_INTENSIFIED)
		tmp = 0xd7;
	    else
		tmp = 0x97;
	} else {
	    if (dis_att == FA_DISPLAY_INTENSIFIED)
		tmp = 0xe4;
	    else
		tmp = 0xa4;
	}
	(*screen->put_chars) (screen->interface_data, &tmp, 1,
			      next->start_address,
			      &att);
	if (next->flags & FIELD_LAST)
	    break;
	next = next->next_field;
    };

    return;

}







struct field_type *field_add (ScreenPtr screen,
	    unsigned int address,
	    struct field_attribute_type * attribute)
{
    struct field_type *field = NULL;
    struct field_type *temp = NULL;
    int new_first_field = 0;


    while (NULL == (field = memory_allocate(screen, MEM_TYPE_FIELD))) {
	process_sleep_for(TN3270_MEMWAIT_SLEEP_SECS * ONESEC);
    }
    field->next_field = 0;
    field->prev_field = 0;
    field->flags = 0;
    field->new = 1;
    field->start_address = address;
    memcpy(&(field->attribute), attribute, sizeof(struct field_attribute_type));
    memset(&(field->prev_att), 0, sizeof(struct field_attribute_type));


    if (!screen->field_first) {
	field->flags = FIELD_FIRST | FIELD_LAST;
	screen->field_first = field;
	field->next_field = field;
	field->prev_field = field;
	return field;
    }
    screen->field_first->flags = 0;
    screen->field_first->prev_field->flags = 0;

    temp = screen->field_first->prev_field;


    while (1) {
	if (temp->start_address > field->start_address)
	    if (temp == screen->field_first) {
		new_first_field = 1;
		break;
	    } else
		temp = temp->prev_field;
	else
	    break;
    }



    if (new_first_field) {
	field->next_field = temp;
	field->prev_field = temp->prev_field;
	temp->prev_field = field;
	field->prev_field->next_field = field;
	screen->field_first = field;
    } else {
	field->next_field = temp->next_field;
	temp->next_field = field;
	field->prev_field = temp;
	field->next_field->prev_field = field;
    }
    screen->field_first->prev_field->flags |= FIELD_LAST;
    screen->field_first->flags |= FIELD_FIRST;

    if (field->prev_field->new) {
	memcpy(&(field->prev_att), &(field->prev_field->prev_att),
	       sizeof(struct field_attribute_type));
    } else {
	memcpy(&(field->prev_att), &(field->prev_field->attribute),
	       sizeof(struct field_attribute_type));
    }

    return field;
}


int field_modify (ScreenPtr screen,
		  unsigned short address,
		  struct field_attribute_type * attribute)
{
    struct field_type *field;

    field = field_find(screen, address);
    if (field == 0) {
	printf("field modify error \n");
	return 0;
    }
    field->new = 1;
    memcpy(&(field->prev_att), &(field->attribute),
	   sizeof(struct field_attribute_type));
    memcpy(&(field->attribute), attribute,
	   sizeof(struct field_attribute_type));

    return 1;
}






/*MARK*/
void field_delete (ScreenPtr screen, unsigned short address)
{
    struct field_type *field;
    struct field_attribute_type fld_att;
    unsigned char status;
    unsigned short i;

    field = field_find(screen, address);
    if (address != field->start_address) {
	tn3270_debug(screen, DEBUG_BUGS, "Fields Delete Error %04x %04x \n", 
		     address, field->start_address); 
	return;
    };

    if (FIELD_NEXT_FIELD(field) == field) {
	screen->field_first = 0;
	memset(&fld_att, 0, sizeof(fld_att));
	status = compare_attributes(&(field->attribute), &fld_att);
	if (status == COMPARE_DISPLAY_ALL)
	    for (i = 0; i < screen->current_size; i++)
		screen->tags[i] = 1;
	else if (status == COMPARE_DISPLAY_TEXT)
	    for (i = 0; i < screen->current_size; i++)
		if ((screen->screen[i] != 0x00) &&
		    (screen->screen[i] != 0x40) &&
		    (screen->screen[i] != ORDER_SF) &&
		    (screen->screen[i] != ORDER_SFE))
		    screen->tags[i] = 1;
	memory_deallocate(screen, field);
	return;
    }
    status = compare_attributes(&(field->attribute), &(field->prev_field->attribute));
    if (status == COMPARE_DISPLAY_ALL)
	for (i = field->start_address; i != FIELD_END_ADDRESS(field); INCREMENT_ADDRESS(i)) {
	    screen->tags[i] = 1;
	}
    else if (status == COMPARE_DISPLAY_TEXT)
	for (i = field->start_address; i != FIELD_END_ADDRESS(field); INCREMENT_ADDRESS(i))
	    if ((screen->screen[i] != 0x00) &&
		(screen->screen[i] != 0x40) &&
		(screen->screen[i] != ORDER_SF) &&
		(screen->screen[i] != ORDER_SFE)) {
		screen->tags[i] = 1;
	    }
    if (screen->field_first == field)
	screen->field_first = FIELD_NEXT_FIELD(field);


    field->prev_field->next_field = field->next_field;
    field->next_field->prev_field = field->prev_field;

    if (field->flags & FIELD_FIRST)
	field->next_field->flags |= FIELD_FIRST;

    if (field->flags & FIELD_LAST)
	field->prev_field->flags |= FIELD_LAST;


    memory_deallocate(screen, field);
    return;


}

