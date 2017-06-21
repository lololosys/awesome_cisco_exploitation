/* $Id: sr_modem_mgmtmib.h,v 3.1.42.1 1996/06/16 21:16:52 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/les/sr_modem_mgmtmib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * January 1996, Viet Nguyen (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_modem_mgmtmib.h,v $
 * Revision 3.1.42.1  1996/06/16  21:16:52  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1996/01/25  00:47:54  vnguyen
 * placeholder files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *
 * Copyright (C) 1992-1995 by SNMP Research, Incorporated.
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

#ifndef _modem_mgmtmib_h_
#define _modem_mgmtmib_h_

#include "../snmp/snmp_api.h"
#include "sr_modem_mgmtmibdefs.h"
#include "sr_modem_mgmtmibsupp.h"
#include "sr_modem_mgmtmibpart.h"
#include "sr_modem_mgmtmibtype.h"

extern const ObjectInfo modem_mgmtmib_OidList[];
extern const uint       modem_mgmtmib_OidListNum;

/*
 * these externs are added here because for some unknown reasons, the MIB
 * compiler did not automatically generate them in the sr_modem_mgmtmibtype.h
 */
extern int cmLineStatusEntry_test SR_PROTOTYPE((OID *, ObjectInfo *, ObjectSyntax *,doList_t *, doList_t *, ContextInfo *));
extern int cmLineStatusEntry_set SR_PROTOTYPE((doList_t *, doList_t *, ContextInfo *));
extern int cmLineStatusEntry_undo SR_PROTOTYPE((doList_t *, doList_t *, ContextInfo *));

extern int k_cmLineStatusEntry_test SR_PROTOTYPE((ObjectInfo *, ObjectSyntax *, doList_t *, ContextInfo *));
extern int k_cmLineStatusEntry_set SR_PROTOTYPE((cmLineStatusEntry_t *, ContextInfo *, int));
extern int k_cmLineStatusEntry_ready SR_PROTOTYPE((ObjectInfo *, ObjectSyntax *, doList_t *, doList_t *));

#endif  /* _modem_mgmtmib_h_ */
      
