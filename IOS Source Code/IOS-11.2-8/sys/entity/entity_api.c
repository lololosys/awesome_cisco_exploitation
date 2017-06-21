/* $Id: entity_api.c,v 1.1.2.1 1996/07/12 23:28:53 jjohnson Exp $
 * $Source: /release/112/cvs/Xsys/entity/entity_api.c,v $
 *------------------------------------------------------------------
 * entity_api.c:  API for Physical and Logical Managed Entities
 *
 * June 1996, Jeffrey T. Johnson
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: entity_api.c,v $
 * Revision 1.1.2.1  1996/07/12  23:28:53  jjohnson
 * CSCdi62131:  Need a mechanism to support logical snmp entities
 * Branch: California_branch
 *
 * Revision 1.1  1996/06/20  16:31:13  jjohnson
 * Initial placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */



/*
 * NOTE WELL:  this is a very lightweight implementation of the logical
 * entity api.  A full scale implementation should coincide with the
 * implementation of the entity mib
 */


#include "master.h"
#include "subsys.h"
#include "../snmp/snmp_api.h"

/*
 * structure for maintaining logical entities
 */
#define	enttype struct ent
enttype {
    enttype *next;			/* ptr to next entry */
    char *entityID;			/* entity identifier */
    char *entityDescr;			/* entity description */
    OID *entityType;			/* entity type */
    SR_UINT32 entityIndex;		/* entity index */
};

static queuetype entityQ;
static SR_UINT32 next_logical_index;

static enttype *
lookup_entity (const char *entityID)
{
    enttype *entP;

    for (entP = entityQ.qhead; entP; entP = entP->next) {
	if (strcmp(entityID, entP->entityID) == 0) {
	    break;
	}
    }
    return (entP);
}

/*
 * snmp_add_logical_entity
 */
SR_UINT32
snmp_add_logical_entity (const char *entityID,
			 const char *entityDescr,
			 const OID *entityType)
{
    enttype *entP;

    entP = malloc(sizeof(enttype));
    if (entP == NULL) {
	return 0;
    }

    if (!lookup_entity(entityID))
	add_snmp_entity(entityID);

    setstring(&entP->entityID, entityID);
    setstring(&entP->entityDescr, entityDescr);
    entP->entityType = CloneOID((OID *)entityType);
    entP->entityIndex = next_logical_index++;
    enqueue(&entityQ, entP);

    return (entP->entityIndex);
}

/*
 * snmp_delete_logical_entity
 */
void
snmp_delete_logical_entity (const SR_UINT32 entityIndex)
{
    enttype *entP;

    for (entP = entityQ.qhead; entP; entP = entP->next) {
	if (entityIndex == entP->entityIndex)
	    break;
    }

    if (!entP)
	return;

    unqueue(&entityQ, entP);
    if (!lookup_entity(entP->entityID))
	delete_snmp_entity(entP->entityID);

    free(entP->entityID);
    free(entP->entityDescr);
    FreeOID(entP->entityType);
    free(entP);
}

/*
 * snmp_extract_logical_entity
 */
char *
snmp_extract_logical_entity (const ContextInfo *context)
{
    static char community[128];
    char *entity;

    /*
     * in the lightweight implementation, we only parse v1 contexts
     */
    if (context->CIType != CI_PARTY_COMMUNITY)
	return (NULL);

    memcpy(community,
	   context->u.PartyCommunity->partyAuthPrivate->octet_ptr,
	   context->u.PartyCommunity->partyAuthPrivate->length);
    community[context->u.PartyCommunity->partyAuthPrivate->length] = '\0';
    entity = strrchr(community, '@');
    if (entity) {
	if (!lookup_entity(++entity))
	    entity = NULL;
    }
    return (entity);
}

static void
init_logical_entity (subsystype *subsys)
{
    queue_init(&entityQ, 0);
    next_logical_index = 1;
}


#define MAJVERSION_entity 1
#define MINVERSION_entity 0
#define EDITVERSION_entity 0

SUBSYS_HEADER(entity,
              MAJVERSION_entity,
              MINVERSION_entity,
              EDITVERSION_entity,
              init_logical_entity,
              SUBSYS_CLASS_MANAGEMENT,
              NULL,
              NULL);
      
