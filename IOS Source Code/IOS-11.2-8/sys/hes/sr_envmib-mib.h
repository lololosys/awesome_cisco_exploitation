/* $Id: sr_envmib-mib.h,v 3.1 1996/02/02 02:55:02 jjohnson Exp $
 * $Source: /release/111/cvs/Xsys/hes/sr_envmib-mib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * January 1996, Jeffrey T. Johnson (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_envmib-mib.h,v $
 * Revision 3.1  1996/02/02  02:55:02  jjohnson
 * CSCdi47892:  SNMP oid translation table needs to be modularized
 * modularize the envmon and ds1 mib translation tables
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *
 * Copyright (C) 1994 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 52.227-7013
 *  and in similar clauses in the FAR and NASA FAR Supplement.
 *
 */

#ifndef _ENVMIBMIB_H_
#define _ENVMIBMIB_H_

/* SNMP-MIB translation table.  Subitem on left gets
 * translated into subitem on right.
 */

static const struct MIB_OID envmib_oid_table[] =
{
{    "ciscoEnvMonMIB",                  "1.3.6.1.4.1.9.9.13",},
{    "ciscoEnvMonObjects",              "1.3.6.1.4.1.9.9.13.1",},
{    "ciscoEnvMonVoltageStatusEntry",   "1.3.6.1.4.1.9.9.13.1.2.1",},
{    "ciscoEnvMonTemperatureStatusEntry", "1.3.6.1.4.1.9.9.13.1.3.1",},
{    "ciscoEnvMonFanStatusEntry",       "1.3.6.1.4.1.9.9.13.1.4.1",},
{    "ciscoEnvMonSupplyStatusEntry",    "1.3.6.1.4.1.9.9.13.1.5.1",},
{    "ciscoEnvMonMIBNotificationEnables", "1.3.6.1.4.1.9.9.13.2",},
{    "ciscoEnvMonMIBNotificationPrefix", "1.3.6.1.4.1.9.9.13.3",},
{    "ciscoEnvMonMIBNotifications",     "1.3.6.1.4.1.9.9.13.3.0",},
{    0, 0},
};



/* Don't put anything after this #endif */ 
#endif /* _ENVMIBMIB_H_ */
