/* $Id: chassismib_api.c,v 3.1.2.1 1996/05/21 06:38:33 mordock Exp $
 * $Source: /release/112/cvs/Xsys/snmp/chassismib_api.c,v $
 *------------------------------------------------------------------
 * chassismib.c:  chassis mib routines
 *
 * May 1996, Scott R. Mordock
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: chassismib_api.c,v $
 * Revision 3.1.2.1  1996/05/21  06:38:33  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.1  1996/05/20  17:02:09  mordock
 * placeholders for california
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "packet.h"
#include "subsys.h"
#include "../snmp/snmp_api.h"
#include "chassismib_api.h"
#include "chassismib_registry.h"


/* 
 * globals
 */
static unsigned long sysUpTimeAtLastChassisChange = 0L;

/*
 * forward refs
 */
void chassismib_add_sub_card_entry(int type, void *remkey, char *descr, 
				   uint serial, char *hw_version, 
				   char *sw_version, int slot_number, 
				   uint card_contained_by_index,
				   int card_slots);


/* 
 * chassis related service
 */

queuetype card_table;

card_entry *
chassismib_get_cep(int searchType, long *cardIndex)
{
    card_entry* cep;
    long index = *cardIndex;
 
    for (cep = (card_entry*) card_table.qhead; cep != NULL; cep = cep->next) {
        *cardIndex = cep->card_index;
        if (searchType != NEXT) {
            if (cep->card_index == index)
                return(cep);
            if (cep->card_index > index)
                return(NULL);
        } else {
            if (cep->card_index >= index)
                return(cep);
        }
    }
    return(NULL);
}

/*
 * chassismib_add_card_entry 
 *
 * Wrapper for chassismib_add_sub_card_entry which maintains original 
 * chassismib_add_card_entry interface while supporting Chassis MIB 
 * enhancements. Original functional description:
 *
 * Allocate a memory block to contain information about a single
 * physical card.  If called from interrupt level (or if potentially
 * callable from interupt level) this routine uses buffers instead of
 * memory blocks.  This is because it is not permissible to touch the
 * free memory while running at interrupt level.
 */
inline void
chassismib_add_card_entry (int type, void *remkey, char *descr, uint serial,
			    char *hw_version, char *sw_version,
			    int slot_number)
{
    chassismib_add_sub_card_entry (type, remkey, descr, serial,
				   hw_version, sw_version, slot_number, 0, 0);
}

/*
 * chassismib_add_sub_card_entry
 *
 * Perform the original chassismib_add_card_entry functions described above
 * with awareness of new cardContainedByIndex and cardSlots parameters
 * required for multiple layer card/component containment, such as VIP
 * cards. 
 */
void
chassismib_add_sub_card_entry (int type, void *remkey, char *descr,
				uint serial, char *hw_version, 
				char *sw_version, int slot_number,
				uint card_contained_by_index, int card_slots)
{
    char       *cp;
    card_entry *cep;
    uint	bufsize;
    static uint card_index = 0;
    static const char null_string[] = "";

    if (system_loading)
	return;

    /*
     * make sure the string pointers point at something
     */
    if (descr == NULL)
	descr = (char *) null_string;
    if (hw_version == NULL)
	hw_version = (char *) null_string;
    if (sw_version == NULL)
	sw_version = (char *) null_string;

    bufsize = sizeof(card_entry) + 1 + strlen(hw_version) +
				   1 + strlen(sw_version);
    if (remkey != NO_HOTSWAP) {
	paktype *pak;

	pak = getbuffer(bufsize);
	if (pak) {
	    cep = (card_entry *)pak->data_area;
	    cep->next = NULL;
	    cep->buffer = pak;
	} else {
	    cep = NULL;
	}
    } else {
	cep = malloc(bufsize);
    }
    if (cep == NULL) {
	return;
    }
    cep->card_index = ++card_index;
    cep->card_type = type;
    cep->card_remkey = remkey;
    cep->card_descr = descr;
    cep->card_serial = serial;
    cp = (char *) (cep + 1);
    cep->card_hw_version = cp;
    do
	*cp++ = *hw_version;
    while (*hw_version++);
    cep->card_sw_version = cp;
    do
	*cp++ = *sw_version;
    while (*sw_version++);
    cep->card_slot_number = slot_number;
    cep->card_contained_by_index = card_contained_by_index;
    cep->card_slots = card_slots;
    p_enqueue(&card_table, cep);
    sysUpTimeAtLastChassisChange = GetTimeNow();
    return;
}

void
chassismib_update_sub_card_entry (uint card_index, uint serial, 
				   char *hw_version, char *sw_version)
{
    char       *cp;
    card_entry *cep, *old_cep;
    uint        bufsize;
    static const char null_string[] = "";
 
    if (system_loading)
        return;

    for (old_cep = card_table.qhead; old_cep != NULL; old_cep = old_cep->next) {
        if (old_cep->card_index == card_index) {
            break;
        }
    }
    if (old_cep == NULL) 
	return;
 
    /*
     * make sure the string pointers point at something
     */
    if (hw_version == NULL)
        hw_version = (char *) null_string;
    if (sw_version == NULL)
        sw_version = (char *) null_string;
 
    bufsize = sizeof(card_entry) + 1 + strlen(hw_version) +
                                   1 + strlen(sw_version);
    if (old_cep->card_remkey != NO_HOTSWAP) {
        paktype *pak;
 
        pak = getbuffer(bufsize);
        if (pak) {
            cep = (card_entry *)pak->data_area;
            cep->buffer = pak;
        } else {
            cep = NULL;
        }
    } else {
        cep = malloc(bufsize);
    }
    if (cep == NULL) {
        return;
    }

    cep->card_index = old_cep->card_index;
    cep->card_type = old_cep->card_type;
    cep->card_remkey = old_cep->card_remkey;
    cep->card_descr = old_cep->card_descr;
    cep->card_serial = serial;
    cp = (char *) (cep + 1);
    cep->card_hw_version = cp;
    do
        *cp++ = *hw_version;
    while (*hw_version++);
    cep->card_sw_version = cp;
    do
        *cp++ = *sw_version;
    while (*sw_version++);
    cep->card_slot_number = old_cep->card_slot_number;
    cep->card_contained_by_index = old_cep->card_contained_by_index;
    cep->card_slots = old_cep->card_slots;

    /* 
     * The snmp subsystem doesn't include the new element in get-responses
     * if it is not in the correct position on the queue (card_index order). 
     */
    p_swapqueue(&card_table, cep, old_cep);

    if (old_cep->buffer)
        retbuffer(old_cep->buffer);
    else
        free(old_cep);

    sysUpTimeAtLastChassisChange = GetTimeNow();
    return;
}

void
chassismib_remove_card_entry (void *remkey)
{
    card_entry *cep, *next_cep;
    long card_index = 0L;

    for (cep = card_table.qhead; cep != NULL; cep = cep->next) {
	if ((cep->card_remkey == remkey) &&
				(cep->card_contained_by_index <= 0)) {
	    card_index = cep->card_index;
	    p_unqueue(&card_table, cep);
	    if (cep->buffer)
		retbuffer(cep->buffer);
	    else
		free(cep);
	    sysUpTimeAtLastChassisChange = GetTimeNow();
	    break;
	}
    }
    /*
     * This is workable only while we have <= 2 levels of card containment.
     */
    if (card_index && ((cep = card_table.qhead) != NULL)) {
	do {
            if (cep->card_contained_by_index == card_index) {
		next_cep = cep->next;
            	p_unqueue(&card_table, cep);
            	if (cep->buffer)
                    retbuffer(cep->buffer);
            	else
                    free(cep);
		cep = next_cep;
	    } else {
		cep = cep->next;
	    }
	    
	} while (cep != NULL); 
    }
    return;
}

int
chassismib_find_slot_card_index (int slot)
{
    card_entry *cep;

    for (cep = card_table.qhead; cep!=NULL; cep = cep->next) {
        if ((cep->card_slot_number == slot) && ((cep->card_contained_by_index == 0) || (cep->card_contained_by_index == -1)))
            return (cep->card_index);
    }
    return (-1);
}

int
chassismib_find_sub_card_index (long card_index, int bay)
{
    card_entry *cep;

    for (cep = card_table.qhead; cep!=NULL; cep = cep->next) {
        if ((cep->card_slot_number == bay) && 
		(cep->card_contained_by_index == card_index))
            return (cep->card_index);
    }
    return (-1);
}

static void
init_chassismib (subsystype *subsys)
{
    /*
     * create the chassis card queue
     */
    queue_init(&card_table, 0);

    /*
     * register service points
     */
    reg_add_chassismib_add_card_entry(chassismib_add_card_entry,
				      "chassismib_add_card_entry");
    reg_add_chassismib_add_sub_card_entry(chassismib_add_sub_card_entry,
					  "chassismib_add_sub_card_entry");
    reg_add_chassismib_update_sub_card_entry(chassismib_update_sub_card_entry,
					     "chassismib_update_sub_card_entry");
    reg_add_chassismib_remove_card_entry(chassismib_remove_card_entry,
					 "chassismib_remove_card_entry");
    reg_add_chassismib_find_slot_card_index(chassismib_find_slot_card_index,
					    "chassismib_find_slot_card_index");
    reg_add_chassismib_find_sub_card_index(chassismib_find_sub_card_index,
					   "chassismib_find_sub_card_index");
    reg_add_chassismib_get_cep(chassismib_get_cep,
			       "chassismib_get_cep");
}

unsigned long get_sysUpTimeAtLastChassisChange (void)
{
    return(sysUpTimeAtLastChassisChange);
}

/*
 * chassis mib subsystem header
 * this subsystems needs to be a kernel-class subsystem due to the fact
 * that various driver-class subsystems, when they initialize, use the
 * services to register cards/sub-cards.
 * sr_old_lchassismib.c (which is a part of this subsystem) uses
 * the ifmib services (thus the seq:)
 */

#define MAJVERSION_chassismib 1
#define MINVERSION_chassismib 0
#define EDITVERSION_chassismib 0

SUBSYS_HEADER(chassismib,
              MAJVERSION_chassismib,
              MINVERSION_chassismib,
              EDITVERSION_chassismib,
              init_chassismib,
              SUBSYS_CLASS_KERNEL,
              "seq: ifmib_api",
              NULL);
      
