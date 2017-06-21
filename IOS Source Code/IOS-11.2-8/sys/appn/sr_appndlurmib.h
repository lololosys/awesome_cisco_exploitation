/* $Id: sr_appndlurmib.h,v 3.1.4.2 1996/07/17 06:52:28 bchan Exp $
 * $Source: /release/112/cvs/Xsys/appn/sr_appndlurmib.h,v $
 *------------------------------------------------------------------
 * DLUR snmn sub-agent code
 *
 * Jan 96, Barry Chan
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_appndlurmib.h,v $
 * Revision 3.1.4.2  1996/07/17  06:52:28  bchan
 * CSCdi63198:  Fix appn/dlur mib for tn3270 server
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/05/17  10:39:15  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.1  1996/03/21  03:31:02  bchan
 * CSCdi52147:  Add tn3270 server new files to IbuMod_Calif_branch
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1  1996/03/20  06:50:30  bchan
 * *** empty log message ***
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

#ifndef _appndlurmib_h_
#define _appndlurmib_h_

#define N_appnDlur          /* define OID parts for appn dlur */
#include "../snmp/snmp_api.h"
#include "sr_appndlurmibdefs.h"
#include "sr_appndlurmibsupp.h"
#include "sr_appndlurmibpart.h"
#include "sr_appndlurmibtype.h"

extern const ObjectInfo appndlurmib_OidList[];
extern const uint       appndlurmib_OidListNum;

typedef struct SubAgentFunc_ {
  dlurNodeCapabilities_t* (*t_dlurNodeCapabilities_get)(int, ContextInfo *, int);
  dlurPuEntry_t*     (*t_dlurPuEntry_get)(int, ContextInfo *, int, int, long);
  dlurDlusEntry_t*   (*t_dlurDlusEntry_get)(int, ContextInfo *, int, int, long);
}SubAgentFunc;


extern SubAgentFunc* dlurRp;
extern SubAgentFunc* dlurCip;

#endif  /* _appndlurmib_h_ */
      
