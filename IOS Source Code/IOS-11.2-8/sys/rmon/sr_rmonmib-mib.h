/* $Id: sr_rmonmib-mib.h,v 3.1 1996/03/14 23:26:31 bstewart Exp $
 * $Source: /release/111/cvs/Xsys/rmon/sr_rmonmib-mib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * March 1996, Bob Stewart (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_rmonmib-mib.h,v $
 * Revision 3.1  1996/03/14  23:26:31  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
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

#ifndef _RMONMIBMIB_H_
#define _RMONMIBMIB_H_

/* SNMP-MIB translation table.  Subitem on left gets
 * translated into subitem on right.
 */

static const struct MIB_OID rmonmib_oid_table[] =
{
{    "rmon",                            "1.3.6.1.2.1.16",},
{    "statistics",                      "1.3.6.1.2.1.16.1",},
{    "etherStatsEntry",                 "1.3.6.1.2.1.16.1.1.1",},
{    "history",                         "1.3.6.1.2.1.16.2",},
{    "historyControlEntry",             "1.3.6.1.2.1.16.2.1.1",},
{    "etherHistoryEntry",               "1.3.6.1.2.1.16.2.2.1",},
{    "alarm",                           "1.3.6.1.2.1.16.3",},
{    "alarmEntry",                      "1.3.6.1.2.1.16.3.1.1",},
{    "hosts",                           "1.3.6.1.2.1.16.4",},
{    "hostControlEntry",                "1.3.6.1.2.1.16.4.1.1",},
{    "hostEntry",                       "1.3.6.1.2.1.16.4.2.1",},
{    "hostTimeEntry",                   "1.3.6.1.2.1.16.4.3.1",},
{    "hostTopN",                        "1.3.6.1.2.1.16.5",},
{    "hostTopNControlEntry",            "1.3.6.1.2.1.16.5.1.1",},
{    "hostTopNEntry",                   "1.3.6.1.2.1.16.5.2.1",},
{    "matrix",                          "1.3.6.1.2.1.16.6",},
{    "matrixControlEntry",              "1.3.6.1.2.1.16.6.1.1",},
{    "matrixSDEntry",                   "1.3.6.1.2.1.16.6.2.1",},
{    "matrixDSEntry",                   "1.3.6.1.2.1.16.6.3.1",},
{    "filter",                          "1.3.6.1.2.1.16.7",},
{    "filterEntry",                     "1.3.6.1.2.1.16.7.1.1",},
{    "channelEntry",                    "1.3.6.1.2.1.16.7.2.1",},
{    "capture",                         "1.3.6.1.2.1.16.8",},
{    "bufferControlEntry",              "1.3.6.1.2.1.16.8.1.1",},
{    "captureBufferEntry",              "1.3.6.1.2.1.16.8.2.1",},
{    "event",                           "1.3.6.1.2.1.16.9",},
{    "eventEntry",                      "1.3.6.1.2.1.16.9.1.1",},
{    "logEntry",                        "1.3.6.1.2.1.16.9.2.1",},
{    0, 0},
};



/* Don't put anything after this #endif */ 
#endif /* _RMONMIBMIB_H_ */
