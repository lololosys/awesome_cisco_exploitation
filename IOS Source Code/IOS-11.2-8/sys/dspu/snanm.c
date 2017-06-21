/* $Id: snanm.c,v 3.2.60.2 1996/08/16 18:32:38 lmetzger Exp $
 * $Source: /release/112/cvs/Xsys/dspu/snanm.c,v $
 *------------------------------------------------------------------
 * SNA Network Management Initialization routine
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: snanm.c,v $
 * Revision 3.2.60.2  1996/08/16  18:32:38  lmetzger
 * CSCdi66255:  Add Modularity of RTR for DSPU/SNANM
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/05/17  10:49:30  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.1  1996/03/22  18:50:58  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.2  1995/11/17  09:06:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/20  17:03:36  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 * Revision 2.1  1995/06/07 20:30:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "ttysrv.h"

#include "mv_prodid.h"
#include "mv_datah.h"
#include "mv_sr.h"
#include "nm_runcmd.h"
#include "registry.h"
#include "dspu_fp.h"
#include "../dspu/snanm_registry.regh"
#include "lan_alert.h"

extern void snanm_parser_init(void);
extern void snanm_debug_init(void);
extern void VprodidInit(void);

static boolean focalPointConfigured = FALSE;


/* Registered Alert if FocalPoint is configured */

static void RegisterAlerts (void)
{
   if (!focalPointConfigured){
       focalPointConfigured = TRUE;
       reg_add_Ether_Alert(Ether_Alert, "Ether_Alert");
       reg_add_Qllc_Alert(Qllc_Alert,"Qllc_Alert");
       reg_add_Sdlc_Alert(Sdlc_Alert,"Sdlc_Alert");
       reg_add_Lanllc_Alert(Lanllc_Alert,"Lanllc_Alert");
       reg_add_Cipllc_Alert(Cipllc_Alert,"Cipllc_Alert");
       reg_add_TR_Alert(TR_Alert,"TR_Alert");
       reg_add_Lapb_Alert(Lapb_Alert,"Lapb_Alert");
       reg_add_Dlc_Alert(Dlc_Alert,"Dlc_Alert");
       reg_add_Tcp_Alert(Tcp_Alert,"Tcp_Alert");
       reg_add_X25_Alert(X25_Alert,"X25_Alert");
       reg_add_SendAlertToHost(SendAlertToHost,"SendAlertToHost");
       reg_add_MV_SendAlert(MV_SendAlert,"MV_SendAlert");
   }
}
void snanm_init (void)
{
   snanm_parser_init();
   snanm_debug_init();

   DataHeaderInit();
   ProductIdInit();
   MVSRinit();
   RegisterFpConfigured(RegisterAlerts);
/*   PuRegIdInit();  No pu registration supported */
   PH_Init();
   VprodidInit();   /* vital product initialization */
}

