/* $Id: lanllc_alert.c,v 3.3.58.4 1996/08/26 15:05:36 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dspu/lanllc_alert.c,v $
 *------------------------------------------------------------------
 * SNA Network Management LAN alert handler
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lanllc_alert.c,v $
 * Revision 3.3.58.4  1996/08/26  15:05:36  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.3.58.3  1996/07/09  05:49:36  ppearce
 * CSCdi59527:  ascii string in inline wastes code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.3.58.2  1996/05/17  10:48:34  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.46.3  1996/04/26  15:17:31  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.46.2  1996/04/26  07:35:20  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.3.46.1  1996/04/03  14:03:20  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3.58.1  1996/03/18  19:33:53  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.24.2  1996/03/07  08:45:53  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.24.1  1996/02/20  13:48:19  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  09:05:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:22:53  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:29:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/20  17:03:28  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 * Revision 2.2  1995/06/21 03:25:47  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.1  1995/06/07  20:29:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include "interface_generic.h"
#include "packet.h"
#include "../llc2/llc2.h"
#include "../srt/srb_core.h"


#include "../snanm/lanllc_alert1.alth"
#include "../snanm/lanllc_alert2.alth"
#include "../snanm/lanllc_alert3.alth"
#include "../snanm/lanllc_alert4.alth"
#include "../snanm/lanllc_alert5.alth"
#include "../snanm/lanllc_alert6.alth"
#include "../snanm/lanllc_alert7.alth"
#include "../snanm/lanllc_alert8.alth"
#include "../snanm/lanllc_alert9.alth"
#include "../snanm/lanllc_alert10.alth"
#include "../snanm/lanllc_alert11.alth"
#include "mv_sr.h"
#include "snautil.h"
#include "registry.h"
#include "../dspu/snanm_registry.regh"
#include "dspu_fp.h"
#include "lan_alert.h"


/* Get lan name if defined, else get it from namestring. Only 8 characters
 * are allowed. Since the last digit is the interface number, so the last
 * digit will be used.
 */

static void SetLanllcParam (byte* buf, llctype *llc)
{
   byte* sv;
   byte* sf;
   short svLen;
   short sfLen;
   uchar* frameptr;
   hwidbtype *idb = (llc->idb)->hwptr;
   ushort srb_thisring;
  

    ForAllSvInMV(((MV*)buf), sv, svLen){
        switch (sv[1]){

        case 0x96: /* Failure Cause */
            SetFailCause(idb, sv, "LLC2");
            break;
        case 0x51 : /* LAN LCS Data */
            ForAllSfInSV(sv, sf, sfLen){
                switch (sf[1]) {
    
                case 0x02 : /* Ring/Segment Identifier */
                    srb_thisring = srbV.srb_sb_get_thisring(llc->idb);
                    sf[2] = srb_thisring >> 8;  
                    sf[3] = srb_thisring;   
                    break;

                case 0x03 : /* Local individual MAC address */ 
                    memcpy(&sf[2], llc2_get_lmac(llc), 6);  
                    break;

		case 0x04 : /* Remote individual MAC address */
                    memcpy(&sf[2], llc2_get_rmac(llc), 6);   
                    break;
		}
	    }
            break;
        case 0x52: /* LCS Configuration */
            ForAllSfInSV(sv, sf, sfLen){
                switch (sf[1]) {

	        case 0x02 : /* Remote Device Address Subfield - DSAP */
                    sf[2] = llc2_get_rsap(llc);  
                    break;
	        case 0x04 : /* Local Device Address Subfield - SSAP */ 
                    sf[2] = 0x00;
                    sf[3] = llc2_get_lsap(llc);
                    break;
	        } /* switch */
	    } /* ForAllSfInSV */
            break;
        case 0x8C : /* Link Station Data */
            ForAllSfInSV(sv, sf, sfLen){
                switch (sf[1]) {

                case 0x01 : /* Ns and Nr counts Subfield */
                    sf[2] = llc->v_s;  
                    sf[3] = llc->v_r;  
                    break;

                case 0x02 : /* Outstanding frames Subfield */
                    sf[2] = (llc->v_s - llc->last_n_r - 1)&7;   
                    break;

                /* control field sent and received is not supported by llc2. We tried
                 * the hard way to get the receive field. 
                 */
                case 0x03 : /* Last control field received Subfield*/
                    if (llc->inbuf != 0 && (frameptr = llc->inbuf->info_start) != 0){
                        sf[2] = frameptr[2];
                        sf[3] = frameptr[3];              
                    }
                    else {
                        sf[3] = llc->new_n_r + llc->p_f_val;
                        sf[2] = llc->new_n_s;
		     }
                    break;
 
	        case 0x04 : /* Last control field sent Subfield */
                    break;
 
       	        case 0x05 : /* Modulus Subfield */
                    sf[2] = 8; 
                    break;

		case 0x06 : /* Link Station Role Subfield */
                    if (llc->rem_busy)
                        sf[2] |= 0x40;
                    if (llc->state == LLS_BUSY)
                        sf[2] |= 0x80;
                    break;

		case 0x08 : /* Last Received Nr count Subfield */
                    sf[2] = llc->last_n_r;   
                    break;  
                } /* switch sf[1] */   
            } /* ForAllSfInSV */
            break;
        case 0x05: /* Hierarchy/Resource List */
            SetHRname(idb, sv, "LLC2");
            break;   

        } /* switch */       

   
    } /* ForAllSvInMV */
} /* SetLanllcParam */


void Lanllc_Alert (Lanllc_AlertType alertType, llctype *llc)
{
   byte buf[MaxAlertSize];
   boolean createAlert = FALSE;

   switch(alertType){

      case LanllcAlert1: /* Retry count expired */
#ifdef LANLLCALERT1_H
        memcpy(buf, (char*)&LANLLCALERT1, sizeof(struct LanLlcAlert1));                                                             
        createAlert = TRUE;
#endif
         break;

        
      case LanllcAlert2: /* Remote sent DM */
#ifdef LANLLCALERT2_H
        memcpy(buf, (char*)&LANLLCALERT2, sizeof(struct LanLlcAlert2));                                                             
        createAlert = TRUE;
#endif
         break;

      case LanllcAlert3: /* Remote sent SABME while local already initialised */
#ifdef LANLLCALERT3_H
        memcpy(buf, (char*)&LANLLCALERT3, sizeof(struct LanLlcAlert3));                                                             
        createAlert = TRUE;
#endif
         break;

      case LanllcAlert4: /* Remote sent FRMR W */
#ifdef LANLLCALERT4_H
        memcpy(buf, (char*)&LANLLCALERT4, sizeof(struct LanLlcAlert4));                                                             
        createAlert = TRUE;
#endif
         break;

      case LanllcAlert5: /* Remote sent FRMR X */
#ifdef LANLLCALERT5_H
        memcpy(buf, (char*)&LANLLCALERT5, sizeof(struct LanLlcAlert5));                                                             
        createAlert = TRUE;
#endif
         break;

      case LanllcAlert6: /* Remote sent FRMR Z */
#ifdef LANLLCALERT6_H
        memcpy(buf, (char*)&LANLLCALERT6, sizeof(struct LanLlcAlert6));                                                             
        createAlert = TRUE;
#endif
         break;

     case LanllcAlert7: /* Remote sent FRMR Y */
#ifdef LANLLCALERT7_H
        memcpy(buf, (char*)&LANLLCALERT7, sizeof(struct LanLlcAlert7));                                                             
        createAlert = TRUE;
#endif
         break;

      case LanllcAlert8: /* Local sent FRMR W */
#ifdef LANLLCALERT8_H
        memcpy(buf, (char*)&LANLLCALERT8, sizeof(struct LanLlcAlert8));                                                             
        createAlert = TRUE;
#endif
         break;

      case LanllcAlert9: /* Local sent FRMR X */
#ifdef LANLLCALERT9_H
        memcpy(buf, (char*)&LANLLCALERT9, sizeof(struct LanLlcAlert9));                                                             
        createAlert = TRUE; 
#endif
         break;

      case LanllcAlert10: /* Local sent FRMR Z */
#ifdef LANLLCALERT10_H
        memcpy(buf, (char*)&LANLLCALERT10, sizeof(struct LanLlcAlert10));                                                             
        createAlert = TRUE;
#endif
         break;

      case LanllcAlert11: /* Local sent FRMR Y */
#ifdef LANLLCALERT11_H
        memcpy(buf, (char*)&LANLLCALERT11, sizeof(struct LanLlcAlert11));                                                             
        createAlert = TRUE; 
#endif
         break;
     }

     if (createAlert){
         SetLanllcParam(buf, llc);
         MV_SendAlert((MV*)buf);
     }
     else
        printf("\nlan llc  ALERT Alert%d not supported",alertType + 1);
}

