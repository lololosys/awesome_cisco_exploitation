/* $Id: appn_mib.c,v 3.1.2.1 1996/05/02 02:44:14 vtseng Exp $
 * $Source: /release/112/cvs/Xsys/appn/appn_mib.c,v $
 *------------------------------------------------------------------
 * APPN mib, for APPN specific
 *
 * April 1996, Vivian Tseng
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: appn_mib.c,v $
 * Revision 3.1.2.1  1996/05/02  02:44:14  vtseng
 * CSCdi56145:  fix DLUR MIB code review comments
 * Branch: IbuMod_Calif_branch
 * Rename sr_appnmib.c to appn_mib.c to avoid naming confusion from
 * mib-compiler generated files.
 *
 * Revision 3.1  1996/05/02  01:10:09  vtseng
 * This is the new file to replace sr_appnmib.c for appn specific mib
 * subagent to avoid naming confusion with mib compiler generated files.
 * The contents remain the same as sr_appnmib.c.  The history of
 * sr_appnmib.c will be kept as comments in this file.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*------------------------------------------------------------------
 * These are the history of the file while it was under the name of
 * sr_appnmib.c
 *------------------------------------------------------------------
 *
 * $Log: appn_mib.c,v $
 * Revision 3.1.2.1  1996/05/02  02:44:14  vtseng
 * CSCdi56145:  fix DLUR MIB code review comments
 * Branch: IbuMod_Calif_branch
 * Rename sr_appnmib.c to appn_mib.c to avoid naming confusion from
 * mib-compiler generated files.
 *
 * Revision 3.1.2.2  1996/04/01  08:34:23  vtseng
 * Branch: IbuMod_Calif_branch
 * Add DLUR MIB support.
 *
 * Revision 3.1.2.1  1996/03/27  01:30:28  bchan
 * CSCdi50608:  makefile for mib incorrect
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1  1996/03/26  01:09:20  bchan
 * placeholder for appn specific mib subagent
 *
 *------------------------------------------------------------------
 */


#include "master.h"
#include "subsys.h"
#include "sr_ibmappnmib.h"
#include "sr_appndlurmib.h"
#include "appn_api.h"                    /* all appn functions,defines,consts */




static AppnFunc rpAppnFunc = {
  api_k_ibmappnGeneralInfoAndCaps_get,
  api_k_ibmappnNnUniqueInfoAndCaps_get,
  api_k_ibmappnEnUniqueCaps_get,
  api_k_ibmappnNodePortEntry_get,
  api_k_ibmappnNodePortIpEntry_get,
  api_k_ibmappnNodePortDlsEntry_get,
  api_k_ibmappnNodePortTrEntry_get,
  api_k_ibmappnNodePortDlcTraceEntry_get,
  api_k_ibmappnNodeLsEntry_get,
  api_k_ibmappnNodeLsIpEntry_get,
  api_k_ibmappnNodeLsDlsEntry_get,
  api_k_ibmappnNodeLsTrEntry_get,
  api_k_ibmappnNodeLsStatusEntry_get,
  api_k_ibmappnSnmpInformation_get,
  api_k_ibmappnMemoryUse_get,
  api_k_ibmappnXidInformation_get,
  api_k_ibmappnNnTopo_get,
  api_k_ibmappnNnTopoRouteEntry_get,
  api_k_ibmappnNnAdjNodeEntry_get,
  api_k_ibmappnNnTopologyEntry_get,
  api_k_ibmappnNnTgTopologyEntry_get,
  api_k_ibmappnNnTopologyFREntry_get,
  api_k_ibmappnNnTgTopologyFREntry_get,
  api_k_ibmappnLocalGeneral_get,
  api_k_ibmappnLocalNnSpecific_get,
  api_k_ibmappnLocalTgEntry_get,
  api_k_ibmappnLocalEnEntry_get,
  api_k_ibmappnLocalEnTgEntry_get,
  api_k_ibmappnDirPerf_get,
  api_k_ibmappnDirEntry_get,
  api_k_ibmappnCosModeEntry_get,
  api_k_ibmappnCosNameEntry_get,
  api_k_ibmappnCosNodeRowEntry_get,
  api_k_ibmappnCosTgRowEntry_get,
  api_k_ibmappnIsInGlobal_get,
  api_k_ibmappnIsInEntry_get
  };

static SubAgentFunc rpDlurFunc = {
   api_k_dlurNodeCapabilities_get,
   api_k_dlurPuEntry_get,
   api_k_dlurDlusEntry_get
};

void init_appnmib (subsystype *subsys)
{
  appnRP = &rpAppnFunc; /* add hook to sub agent */
  dlurRp = &rpDlurFunc; /* add hook to sub agent */
}

#define MAJVERSION_appnmib 1
#define MINVERSION_appnmib 0
#define EDITVERSION_appnmib 0

SUBSYS_HEADER(appnmib,
              MAJVERSION_appnmib,
              MINVERSION_appnmib,
              EDITVERSION_appnmib,
              init_appnmib,
              SUBSYS_CLASS_MANAGEMENT,
              NULL,
              "req: appn");

