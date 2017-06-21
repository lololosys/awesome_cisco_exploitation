/* $Id: cipllc_alert.c,v 3.3.20.2 1996/05/17 10:47:09 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dspu/cipllc_alert.c,v $
 *------------------------------------------------------------------
 * SNA Network Management CIP LLC alert handler
 *
 * May 95, Mike Otto
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cipllc_alert.c,v $
 * Revision 3.3.20.2  1996/05/17  10:47:09  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.6.2  1996/04/26  07:35:05  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.3.6.1  1996/04/03  14:02:25  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3.20.1  1996/03/18  19:33:15  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:45:18  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  00:47:02  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/01/29  22:00:50  motto
 * CSCdi46976:  CIP NetView alerts are not formatted correctly
 *
 * Revision 3.2  1995/11/17  09:04:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:27:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/20  17:02:35  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 * Revision 2.1  1995/06/07 20:28:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include "interface_generic.h"
#include "cipllc_types.h"

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
#include "../snanm/cipllc_dup_sap_alert.alth"
#include "../snanm/cipllc_conn_limit_exceeded_alert.alth"
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


/*======================================================================*
 * set_cip_llc_fail_cause
 *======================================================================*
 *
 * Description:
 *   Set fail cause at detail data (0x82) NMVT
 */

static void set_cip_llc_fail_cause (byte *sv, cip_llctype *cipllc) {
   short sfLen;
   byte *sf;
   byte ascii_len;

   ForAllSfInSV(sv,sf,sfLen) {
       if (sf[1] == 0x82) {
           switch (sf[3]) {
               case 0x60: /* port number*/
                   sf[5] = cipllc->idb->unit; 
                   break;
               case 0x61: /* Virtual Adapter */
                   sf[5] = cipllc->adapno; 
                   break;
               case 0x7D: /* CSNA subchannel */
                   sf[5] = 0; 
                   sf[6] = cipllc->csna_device; 
                   break;
               case 0x7E: /* CSNA path */
                   sf[5] = cipllc->csna_path >> 8; 
                   sf[6] = cipllc->csna_path; 
                   break;
               case 0xC8: /* Network interface name */
                   ascii_len = strlen(cipllc->idb->hw_short_namestring);
                   if (ascii_len > 5)
                       ascii_len = 5;
                   memset(&sf[5], 0x40, 5);
                   ConvertAsciiToEbcdic(cipllc->idb->hw_short_namestring, 
                                        &sf[5], ascii_len);
                   break;
           }
       }
   }
}


static void set_cip_llc_param (byte* buf, cip_llctype *cipllc)
{
    byte* sv;
    byte* sf;
    short svLen;
    short sfLen;

    ForAllSvInMV(((MV*)buf), sv, svLen){
        switch (sv[1]){

        case 0x96: /* Failure Cause */
            set_cip_llc_fail_cause(sv, cipllc);
            break;
        case 0x51 : /* LAN LCS Data */
            ForAllSfInSV(sv, sf, sfLen){
                switch (sf[1]) {
    
                case 0x01 : /* Virtual LAN identifier */
                    sf[2] = cipllc->lan_id;
                    break;

                case 0x02 : /* Ring/Segment Identifier */
                    sf[2] = cipllc->local_ring >> 8;  
                    sf[3] = cipllc->local_ring;
                    break;

                case 0x03 : /* Local individual MAC address */ 
                    memcpy(&sf[2], cipllc->local_macaddr, 6);  
                    break;

		case 0x04 : /* Remote individual MAC address */
                    memcpy(&sf[2], cipllc->remote_macaddr, 6);   
                    break;
		}
	    }
            break;
        case 0x52: /* LCS Configuration */
            ForAllSfInSV(sv, sf, sfLen){
                switch (sf[1]) {

	        case 0x02 : /* Remote Device Address Subfield - DSAP */
                    sf[2] = cipllc->remote_sap;  
                    break;
	        case 0x04 : /* Local Device Address Subfield - SSAP */ 
                    sf[2] = 0x00;
                    sf[3] = cipllc->local_sap;
                    break;
	        } /* switch */
	    } /* ForAllSfInSV */
            break;
        case 0x8C : /* Link Station Data */
            ForAllSfInSV(sv, sf, sfLen){
                switch (sf[1]) {

                case 0x01 : /* Ns and Nr counts Subfield */
                    sf[2] = cipllc->v_s;  
                    sf[3] = cipllc->v_r;  
                    break;

                case 0x02 : /* Outstanding frames Subfield */
                    sf[2] = (cipllc->v_s - cipllc->last_n_r_rxd - 1) & 7;   
                    break;

                case 0x03 : /* Last control field received Subfield*/
                    sf[2] = cipllc->last_ctrl_rxd_msb;
                    sf[3] = cipllc->last_ctrl_rxd_lsb;              
                    break;
 
	        case 0x04 : /* Last control field sent Subfield */
                    sf[2] = cipllc->last_ctrl_txd_msb;
                    sf[3] = cipllc->last_ctrl_txd_lsb;              
                    break;
 
       	        case 0x05 : /* Modulus Subfield */
                    sf[2] = 8; 
                    break;

		case 0x06 : /* Link Station Role Subfield */
                    sf[2] |= cipllc->lstn_role;
                    break;

		case 0x08 : /* Last Received Nr count Subfield */
                    sf[2] = cipllc->last_n_r_rxd;   
                    break;  
                } /* switch sf[1] */   
            } /* ForAllSfInSV */
            break;
        case 0x05: /* Hierarchy/Resource List */
            SetHRname(cipllc->idb, sv, "LLC2");
            break;   

        } /* switch */       
    } /* ForAllSvInMV */
} /* SetLanllcParam */


void Cipllc_Alert (Cipllc_AlertType alertType, cip_llctype *cipllc)
{
   byte buf[MaxAlertSize];
   boolean createAlert = FALSE;

   switch (alertType) {

      case CipllcAlert1: /* Retry count expired */
#ifdef LANLLCALERT1_H
        memcpy(buf, (char*)&LANLLCALERT1, sizeof(struct LanLlcAlert1));
        createAlert = TRUE;
#endif
         break;

        
      case CipllcAlert2: /* Remote sent DM */
#ifdef LANLLCALERT2_H
        memcpy(buf, (char*)&LANLLCALERT2, sizeof(struct LanLlcAlert2));
        createAlert = TRUE;
#endif
         break;

      case CipllcAlert3: /* Remote sent SABME while local in ABM */
#ifdef LANLLCALERT3_H
        memcpy(buf, (char*)&LANLLCALERT3, sizeof(struct LanLlcAlert3));
        createAlert = TRUE;
#endif
         break;

      case CipllcAlert4: /* Remote sent FRMR W */
#ifdef LANLLCALERT4_H
        memcpy(buf, (char*)&LANLLCALERT4, sizeof(struct LanLlcAlert4));
        createAlert = TRUE;
#endif
         break;

      case CipllcAlert5: /* Remote sent FRMR X */
#ifdef LANLLCALERT5_H
        memcpy(buf, (char*)&LANLLCALERT5, sizeof(struct LanLlcAlert5));
        createAlert = TRUE;
#endif
         break;

      case CipllcAlert6: /* Remote sent FRMR Z */
#ifdef LANLLCALERT6_H
        memcpy(buf, (char*)&LANLLCALERT6, sizeof(struct LanLlcAlert6));      
        createAlert = TRUE;
#endif
         break;

     case CipllcAlert7: /* Remote sent FRMR Y */
#ifdef LANLLCALERT7_H
        memcpy(buf, (char*)&LANLLCALERT7, sizeof(struct LanLlcAlert7));
        createAlert = TRUE;
#endif
         break;

      case CipllcAlert8: /* Local sent FRMR W */
#ifdef LANLLCALERT8_H
        memcpy(buf, (char*)&LANLLCALERT8, sizeof(struct LanLlcAlert8));       
        createAlert = TRUE;
#endif
         break;

      case CipllcAlert9: /* Local sent FRMR X */
#ifdef LANLLCALERT9_H
        memcpy(buf, (char*)&LANLLCALERT9, sizeof(struct LanLlcAlert9));
        createAlert = TRUE; 
#endif
         break;

      case CipllcAlert10: /* Local sent FRMR Z */
#ifdef LANLLCALERT10_H
        memcpy(buf, (char*)&LANLLCALERT10, sizeof(struct LanLlcAlert10));
        createAlert = TRUE;
#endif
         break;

      case CipllcAlert11: /* Local sent FRMR Y */
#ifdef LANLLCALERT11_H
        memcpy(buf, (char*)&LANLLCALERT11, sizeof(struct LanLlcAlert11));
        createAlert = TRUE; 
#endif
         break;

      case CipllcDupSapAlert: /* Duplicate SAP detected */
#ifdef CIPLLCDUPSAPALERT_H
        memcpy(buf, (char*)&CIPLLCDUPSAPALERT, 
               sizeof(struct CipLlcDupSapAlert));
        createAlert = TRUE; 
#endif
         break;

      case CipllcConnLimitExceededAlert: /* Connection limit exceeded */
#ifdef CIPLLCMAXCONNALERT_H
        memcpy(buf, (char*)&CIPLLCMAXCONNALERT, 
               sizeof(struct CipLlcConnLimitExceededAlert));
        createAlert = TRUE; 
#endif
         break;

      case CipllcUnknownSapAlert: /* Connection attempt to unknown SAP */
         break;
     }

     if (createAlert){
         set_cip_llc_param( buf, cipllc);
         MV_SendAlert((MV*)buf);
     }
     else
        printf("\nCIP LLC ALERT Alert%d not supported", alertType + 1);
}

