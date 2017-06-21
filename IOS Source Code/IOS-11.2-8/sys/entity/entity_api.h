/* $Id: entity_api.h,v 1.1.2.1 1996/07/12 23:28:53 jjohnson Exp $
 * $Source: /release/112/cvs/Xsys/entity/entity_api.h,v $
 *------------------------------------------------------------------
 * entity_api.h:  API for Physical and Logical Managed Entities
 *
 * June 1996, Jeffrey T. Johnson
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: entity_api.h,v $
 * Revision 1.1.2.1  1996/07/12  23:28:53  jjohnson
 * CSCdi62131:  Need a mechanism to support logical snmp entities
 * Branch: California_branch
 *
 * Revision 1.1  1996/06/07  17:45:48  jjohnson
 * Initial placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __ENTITY_API_H__
#define __ENTITY_API_H__

/*
 * name:  snmp_add_logical_entity
 *
 * inputs:
 *
 *   entityID -- textual identifier assigned to the entity.
 *   identifiers should be chosen in such a way as to be
 *   descriptive of the entity functionality while at the
 *   same time being easy for a human operator to remember.
 *
 *   entityDescr -- textual description of the logical entity.
 *   see the entLogicalDescr mib object for a full description
 *
 *   entityType -- object identifier which indicates the type of
 *   logical entity.  see the entLogicalType mib object for a full
 *   description
 *
 * outputs:
 *
 *   SR_UINT32 value representing entLogicalIndex, a value which
 *   uniquely identifies the logical entity.
 */
extern SR_UINT32 snmp_add_logical_entity(const char *entityID,
					 const char *entityDescr,
					 const OID *entityType);

/*
 * name:  snmp_delete_logical_entity
 *
 * inputs:
 *
 *   entityIndex -- The entLogicalIndex value for the entity to be
 *   deleted.  This value was returned by snmp_add_logical_entity
 *   when the entity was created.
 *
 * outputs:
 *
 *   none
 */
extern void snmp_delete_logical_entity(const SR_UINT32 entityIndex);

/*
 * name:  snmp_extract_logical_entity
 *
 * inputs:
 *
 *   context -- The context information that was passed from the
 *   master agent to the subagent implementing a logical entity.
 *
 * outputs:
 *
 *   a pointer to the character string representing the entityID
 *   to which the snmp pdu was addressed, or NULL if no logical
 *   entity was specified
 */
extern char *snmp_extract_logical_entity(const ContextInfo *context);

#endif /* __ENTITY_API_H__ */
