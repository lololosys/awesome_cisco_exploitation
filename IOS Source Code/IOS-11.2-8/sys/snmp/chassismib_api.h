/* $Id: chassismib_api.h,v 3.1.2.1 1996/05/21 06:38:34 mordock Exp $
 * $Source: /release/112/cvs/Xsys/snmp/chassismib_api.h,v $
 *------------------------------------------------------------------
 * Header file for api functions to Chassis MIB
 *
 * May 1996, Scott R. Mordock
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: chassismib_api.h,v $
 * Revision 3.1.2.1  1996/05/21  06:38:34  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.1  1996/05/20  17:02:10  mordock
 * placeholders for california
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *                    Cisco CHASSIS-MIB Internal API
 * The purpose of these functions and data definitions is to provide
 * external access to the Cisco CHASSIS-MIB implementation, where needed.
 * These functions are intended to be used by the modules that service
 * chassis-related mib requests.  All other modules should use the
 * services provided by the chassis mib registry.
 */

#ifndef	 __CHASSISMIB_API_H__ 
#define	 __CHASSISMIB_API_H__ 

#include "sr_old_lchassismibdefs.h"   

typedef uint chassis_type_t;	/* specified as a D_chassisType_* define */
typedef uint card_type_t;	/* specified as a D_cardType_* define */

typedef struct card_entry_ {
    struct card_entry_* next;
    uint card_index;
    void* card_remkey;
    card_type_t card_type;
    char* card_descr;
    uint card_serial;
    char* card_hw_version;
    char* card_sw_version;
    short card_slot_number;
    uint card_contained_by_index;
    short card_slots;
    paktype *buffer;
    uchar :8;  /* DOES THIS NEED REALIGNMENT ??? */
} card_entry;

#define NO_HOTSWAP	((void*) NULL)
#define NO_SERIALNUM	0
#define NO_HWVERSION	""
#define NO_SWVERSION	""
#define NO_SLOTNUMBER	-1

/* referenced by sr_old_lchassismib.c */
card_entry * chassismib_get_cep(int,long*);
extern unsigned long get_sysUpTimeAtLastChassisChange(void);

#endif

