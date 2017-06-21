/* $Id: sr_snmp-mib.h,v 3.48.6.2 1996/06/18 19:59:28 anke Exp $
 * $Source: /release/112/cvs/Xsys/snmp/sr_snmp-mib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1994, SNMP Research
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_snmp-mib.h,v $
 * Revision 3.48.6.2  1996/06/18  19:59:28  anke
 * CSCdi60480:  snmpTraps was removed from the OID translation table
 *              Add it back.
 * Branch: California_branch
 *
 * Revision 3.48.6.1  1996/04/15  03:08:47  jjohnson
 * CSCdi53343:  Remove support for the SNMPv2 Simplified Security
 * Conventions
 * Branch: California_branch
 *
 * Revision 3.48  1996/03/01  14:42:17  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.47  1996/02/29  20:28:59  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.46  1996/02/26  22:27:31  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.45  1996/02/26  21:08:35  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.44  1996/02/26  20:42:57  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.43  1996/02/26  19:54:59  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.42  1996/02/26  19:43:25  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.41  1996/02/23  21:39:01  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.40  1996/02/23  21:11:51  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.39  1996/02/23  20:33:36  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.38  1996/02/23  18:12:09  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *                 Correct problem with usage order of "system" reg. point
 *
 * Revision 3.37  1996/02/23  17:38:12  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.36  1996/02/23  17:08:49  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.35  1996/02/23  16:08:09  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.34  1996/02/23  15:20:23  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.33  1996/02/22  21:54:58  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.32  1996/02/22  21:40:03  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.31  1996/02/22  20:26:06  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.30  1996/02/22  18:44:27  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.29  1996/02/21  21:45:08  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.28  1996/02/21  21:23:43  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.27  1996/02/21  18:08:36  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.26  1996/02/21  17:47:08  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.25  1996/02/21  16:21:36  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.24  1996/02/20  22:47:35  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.23  1996/02/15  22:08:26  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.22  1996/02/15  20:28:23  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.21  1996/02/15  19:49:40  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.20  1996/02/15  18:03:44  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.19  1996/02/15  17:29:56  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.18  1996/02/14  22:02:29  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.17  1996/02/14  20:56:17  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.16  1996/02/14  19:37:23  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.15  1996/02/14  17:36:06  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.14  1996/02/14  16:30:31  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.13  1996/02/13  23:27:06  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.12  1996/02/12  21:32:00  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.11  1996/02/12  20:32:11  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.10  1996/02/12  19:29:04  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.9  1996/02/09  22:12:19  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.8  1996/02/09  20:15:02  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.7  1996/02/09  14:45:55  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.6  1996/02/08  23:00:39  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.5  1996/02/07  22:56:43  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.4  1996/02/07  21:54:36  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.3  1996/02/02  02:55:29  jjohnson
 * CSCdi47892:  SNMP oid translation table needs to be modularized
 * modularize the envmon and ds1 mib translation tables
 *
 * Revision 3.2  1995/11/17  19:01:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:17:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:26:04  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/08/09  19:08:29  rbatz
 * CSCdi38250:  CIP CSNA SNMP traps do not work
 *
 * Revision 2.1  1995/06/07  22:50:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/*
 *
 * Copyright (C) 1993 by SNMP Research, Incorporated.
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


/*
 * automatically generated by the the mib compiler -- do not edit
 */

#ifndef _SNMPMIB_H_
#define _SNMPMIB_H_

/*  SNMP-MIB translation table.  Subitem on left gets
 * translated into subitem on right.
 */

#define OID_DEF(a,b)	{ #a, #b, }

static const struct MIB_OID orig_mib_oid_table[] =
{
    OID_DEF(ccitt,                           0                              ),
    OID_DEF(iso,                             1                              ),
    OID_DEF(org,                             1.3                            ),
    OID_DEF(dod,                             1.3.6                          ),
    OID_DEF(internet,                        1.3.6.1                        ),
    OID_DEF(directory,                       1.3.6.1.1                      ),

    OID_DEF(mgmt,                            1.3.6.1.2                      ),

    OID_DEF(mib-2,                           1.3.6.1.2.1                    ),
    OID_DEF(mib_2,                           1.3.6.1.2.1                    ),

    /* system is here because of usage order problems */
    OID_DEF(system,                          1.3.6.1.2.1.1                  ),

    OID_DEF(transmission,                    1.3.6.1.2.1.10                 ),

    OID_DEF(dot1dSr,                         1.3.6.1.2.1.17.3               ),
    OID_DEF(dot1dSrPortTable,                1.3.6.1.2.1.17.3.1             ),
    OID_DEF(dot1dSrPortEntry,                1.3.6.1.2.1.17.3.1.1           ),

    OID_DEF(experimental,                    1.3.6.1.3                      ),

    OID_DEF(private,                         1.3.6.1.4                      ),

    OID_DEF(enterprises,                     1.3.6.1.4.1                    ),

    OID_DEF(cisco,                           1.3.6.1.4.1.9                  ),
    OID_DEF(products,                        1.3.6.1.4.1.9.1                ),
    OID_DEF(local,                           1.3.6.1.4.1.9.2                ),

    /* The following represent the fragmentation of the old system MIB */
    OID_DEF(lsystem,                         1.3.6.1.4.1.9.2.1              ),
    OID_DEF(lcpu,                            1.3.6.1.4.1.9.2.1              ),
    OID_DEF(lmem,                            1.3.6.1.4.1.9.2.1              ),
    OID_DEF(lenv,                            1.3.6.1.4.1.9.2.1              ),

    OID_DEF(temporary,                       1.3.6.1.4.1.9.3                ),

    OID_DEF(ciscoConfig,                     1.3.6.1.4.1.9.8                ),

    OID_DEF(ciscoMgmt,                       1.3.6.1.4.1.9.9                ),

    OID_DEF(ciscoExperiment,                 1.3.6.1.4.1.9.10               ),

    /* The following are here rather than sr_snmpv2mib-mib.h because
       of initialization ordering */
    OID_DEF(snmpStats,                       1.3.6.1.6.3.1.1.1              ),
    OID_DEF(snmpTraps,                       1.3.6.1.6.3.1.1.5              ),
    OID_DEF(snmpParties,                     1.3.6.1.6.3.3.2.1              ),


    { NULL, NULL, },
};

/* Don't put anything after this #endif */
#endif				/* _SNMPMIB_H_ */
