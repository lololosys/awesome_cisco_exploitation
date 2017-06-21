/* $Id: sr_modem_mgmtmib-mib.h,v 3.1.14.1 1996/06/16 21:16:48 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/les/sr_modem_mgmtmib-mib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * May 1996, Viet Nguyen (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_modem_mgmtmib-mib.h,v $
 * Revision 3.1.14.1  1996/06/16  21:16:48  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1996/05/20  00:10:39  jturner
 * Placeholder.
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
 
#ifndef _MODEM_MGMTMIBMIB_H_
#define _MODEM_MGMTMIBMIB_H_
 
/* SNMP-MIB translation table.  Subitem on left gets
 * translated into subitem on right.
 */
 
static const struct MIB_OID modem_mgmtmib_oid_table[] =
{
{    "ciscoModemMgmtMIB",               "1.3.6.1.4.1.9.9.47",},
{    "cmSystemInfo",                    "1.3.6.1.4.1.9.9.47.1.1",},
{    "cmGroupInfo",                     "1.3.6.1.4.1.9.9.47.1.2",},
{    "cmGroupEntry",                    "1.3.6.1.4.1.9.9.47.1.2.1.1",},
{    "cmGroupMemberEntry",              "1.3.6.1.4.1.9.9.47.1.2.2.1",},
{    "cmLineInfo",                      "1.3.6.1.4.1.9.9.47.1.3",},
{    "cmLineStatusEntry",               "1.3.6.1.4.1.9.9.47.1.3.1.1",},
{    "cmLineConfigEntry",               "1.3.6.1.4.1.9.9.47.1.3.2.1",},
{    "cmLineStatisticsEntry",           "1.3.6.1.4.1.9.9.47.1.3.3.1",},
{    "cmLineSpeedStatisticsEntry",      "1.3.6.1.4.1.9.9.47.1.3.4.1",},
{    0, 0},
};
 
 
 
/* Don't put anything after this #endif */
#endif /* _MODEM_MGMTMIBMIB_H_ */
