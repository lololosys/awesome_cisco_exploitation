/* $Id: snmp_resource.c,v 3.2 1995/12/28 22:35:43 mordock Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/snmp/snmp_resource.c,v $
 *------------------------------------------------------------------
 * snmp_resource.c:  SNMP object resource utilities
 *
 * December 1995, Scott R. Mordock
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: snmp_resource.c,v $
 * Revision 3.2  1995/12/28  22:35:43  mordock
 * CSCdi46168:  improve snmp modularity
 * move some code and globals out of the kernel subsystem and back into
 * the snmp subsystem
 *
 * Revision 3.1  1995/12/28  21:57:40  mordock
 * placeholder for snmp development
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sr_snmpd.h"
#include "sr_sysmib2.h"
#include "sr_snmpv2mib.h"

/*
 * Object Resources table
 */
static long	resource_count;
queuetype	object_resourcesQ;
snmpOR_t	snmpORData;

void
snmp_resource_init(void)
{
    /*
     * initialize the snmp resource data
     */
    queue_init(&object_resourcesQ, 0);
    resource_count = 0;
    snmpORData.snmpORLastChange = 0;
}

/*
 * this routine allows method routines to tell snmp about the object
 * resources that are supported.  this information is used to to
 * fill the snmpORTable
 */
void
register_snmp_resource (const OID *resource_oid, const char *resource_descr)
{
    ortype     *resource;

    /*
     * this is pretty trivial.  We simply allocate a object resource
     * record, fill it up, and place it on the object resource queue.
     * No test for duplicate registrations is made.
     *
     * Note Well: The input OID and string pointers are used as is, i.e. the
     * associated data is not copied.  Hence if the caller has dynamicially
     * allocated these items, they should not be freed upon return.
     */

    resource = malloc(sizeof(ortype));
    if (resource) {
	snmpORData.snmpORLastChange = snmp_sysUpTime();
	resource->or_idx = ++resource_count;
	resource->or_oid = (OID *) resource_oid;
	resource->or_descr = (char *) resource_descr;
	resource->next = NULL;
	p_enqueue(&object_resourcesQ, resource);
    }
}
