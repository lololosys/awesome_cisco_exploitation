/* $Id$
 * $Source$
 *------------------------------------------------------------------
 * snmp_api.h:  Generic SNMP API include file
 *
 * December 1996, Jeffrey T. Johnson
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log$
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Every release of the SNMP Research agent code seems to rearrange
 * the number and placement of include files required to compile
 * subagents.  This then requires that all previously implemented
 * subagents be modified to use the new include files.  In order to
 * (hopefully) circumvent this requirement to modify the existing
 * implementations, this header file will be used to provide a
 * release-independent mechanism for subagents to include the files
 * necessary to compile.  The goal is that when a new SNMP Research
 * release is incorporated, it will only be necessary to modify this
 * file rather than modify all of the individual subagents
 */

#ifndef __SNMP_API_H__
#define __SNMP_API_H__

#include "sr_snmpd.h"

#endif __SNMP_API_H__
