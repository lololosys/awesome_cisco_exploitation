/* $Id: sdlc_alert.c,v 3.3.12.2 1996/05/17 10:49:25 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dspu/sdlc_alert.c,v $
 *------------------------------------------------------------------
 * SNA Network Management SDLC Alert Handler
 *
 * Jan 95, Silvana Zdravkovski
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sdlc_alert.c,v $
 * Revision 3.3.12.2  1996/05/17  10:49:25  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.2.2  1996/04/26  07:35:27  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.3.2.1  1996/04/03  14:03:50  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3.12.1  1996/03/18  19:34:09  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:46:06  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  00:47:30  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/02/13  21:27:35  rbatz
 * CSCdi30258:  Fixed the following:
 *                  Duplicate half-duplex commands.
 *                  Could not do half-duplex stun.
 *                  Could not tune half-duplex timers for bstun.
 *
 * Revision 3.2  1995/11/17  09:06:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/20  17:03:34  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 * Revision 2.1  1995/06/07 20:30:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include "../ibm/ibm_types.h"

#include "interface_generic.h"
#include "../ibm/sdlc.h"
#include "../wan/serial.h"
#include "dspu_fp.h"


#include "../snanm/sdlc_alert1.alth"
#include "../snanm/sdlc_alert2.alth"
#include "../snanm/sdlc_alert3.alth"
#include "../snanm/sdlc_alert4.alth"
#include "../snanm/sdlc_alert5.alth"
#include "../snanm/sdlc_alert6.alth"
#include "../snanm/sdlc_alert7.alth"
#include "../snanm/sdlc_alert8.alth"
#include "../snanm/sdlc_alert9.alth"
#include "../snanm/sdlc_alert10.alth"
#include "../snanm/sdlc_alert11.alth"
#include "../snanm/sdlc_alert12.alth"
#include "../snanm/sdlc_alert13.alth"
#include "../snanm/sdlc_alert14.alth"
#include "../snanm/sdlc_alert15.alth"
#include "../snanm/sdlc_alert16.alth"
#include "mv_sr.h"
#include "snautil.h"
#include "registry.h"
#include "../dspu/snanm_registry.regh"
#include "lan_alert.h"


static void SetSdlcParam(byte* buf, sdlc_data_block* sdb)
{
   byte* sv;
   byte* sf;
   short svLen;
   short sfLen;
   int i;
   int count = 0;
   hwidbtype* idb = sdb->sdlc_idb;
  
   ForAllSvInMV(((MV*)buf), sv, svLen){
        switch (sv[1]){

        case 0x96: /* Failure Cause */
            SetFailCause(idb, sv, "SDLC");
            break;

        case 0x52: /* LCS Configuration Data */
            ForAllSfInSV(sv, sf, sfLen){
                switch (sf[1]) {

	        case 0x02 : /* Remote Device Address Subfield */
                    sf[2] = sdb->sdlc_address;  
                    break;
	        case 0x04 : /* Local Device Address Subfield */ 
                    sf[2] = 0x00;
                    sf[3] = sdb->sdlc_address;
                    break;
	        case 0x06 : /* Link Station Role and Node Type Subfield */
                    if (ISSSDLC(idb))
                        sf[2] = 0x02;    /* secondary */
                    else if (ISPSDLC(idb))
                        sf[2] = 0x01;    /* primary */ 
                    else
                        sf[2] = 0x03;    /* negotiable */
                    sf[3] = 0x02;        /* remote node type always 2.0?? */ 
                    break;
	        case 0x07 : /* ConnectionType, Duplex, Dlc Protocol, PointToPoint or Multipoint */ 
                    sf[2] = 0x01; 
                    sf[3] = ((idb->hdx_flags & HDX_FLAG_ON) ? 0x01 : 0x02);
                                 /* half duplex not enabled = full duplex?? */ 
                    sf[4] = 0x01; /* sdlc */ 

                    for (i=SDLC_MIN_ADDRESS; i<=SDLC_MAX_ADDRESS ; i++)
                    if (idb->sdlc_data[i] != NULL)
                        count++;
                    if (count == 0)
                        sf[5] = 0;
                    else if (count == 1)
                        sf[5] = 0x01;
                    else
                        sf[5] = 0x02;
                    break;
	        } /* switch */
	    } /* ForAllSfInSV */
            break;
        case 0x8C : /* Link Station Data */
            ForAllSfInSV(sv, sf, sfLen){
                switch (sf[1]) {

                case 0x01 : /* Ns and Nr counts Subfield */
                    sf[2] = sdb->sdlc_vs;  
                    sf[3] = sdb->sdlc_vr;  
                    break;

                case 0x02 : /* Outstanding frames Subfield */
                    sf[2] = ((sdb->sdlc_vs - sdb->sdlc_rvr)&(SDLC_MODULO-1)); 
                    break;

                case 0x03 : /* Last control field received Subfield*/ 
                    sf[2] = sdb->sdlc_lastFailCtrlIn;
                    sf[3] = 0;  
                    break;
 
	        case 0x04 : /* Last control field sent Subfield */
                    sf[3] = 0;  
                    sf[2] = sdb->sdlc_lastFailCtrlOut;
                    break;
 
       	        case 0x05 : /* Modulus Subfield */
                    sf[2] = SDLC_MODULO;
                    break;

		case 0x06 : /* Link Station Role Subfield */
                    switch (sdb->sdlc_state) {
                        case SDLC_THEMBUSY:  
                            sf[2] = 0x40;/*local not busy,remote sent RNR*/
                            break;
                        case SDLC_USBUSY:
                            sf[2] = 0x80;/*local sent RNR,remote not busy*/
                            break;
                        case SDLC_BOTHBUSY:
                            sf[2] = 0xC0;/*both sent RNR*/
                            break;
                        default :
                            sf[2] = 0x00;/*neither is busy*/ 
                            break;
                    }
                    break;

		case 0x07:    /* Reply timeout count */
                   sf[2] = sdb->sdlc_lastFailReplyTO >> 8;
                   sf[3] = sdb->sdlc_lastFailReplyTO;
                   break;

		case 0x08 : /* Last Received Nr count Subfield */
                    sf[2] = sdb->sdlc_rvr;
                    break;  
                } /* switch sf[1] */
            } /* ForAllSfInSV */
            break;

        case 0x05 : /* Hierarchy/Resource List */
            SetHRname(idb, sv, "SDLC");
            break;
        }  
    }
}



void Sdlc_Alert(Sdlc_AlertType alertType, sdlc_data_block *sdb)
{
   byte buf[MaxAlertSize];
   boolean createAlert = FALSE;
 
   switch(alertType){

      case SdlcAlert1: /* Retry count expired. */
#ifdef SDLCALERT1_H
        memcpy(buf, (char*)&SDLCALERT1, sizeof(struct SdlcAlert1));
        createAlert = TRUE;
#endif
         break;


      case SdlcAlert2: /* Secondary sent DM response to primary. */
#ifdef SDLCALERT2_H
        memcpy(buf, (char*)&SDLCALERT2, sizeof(struct SdlcAlert2));
        createAlert = TRUE;
#endif
         break;

      case SdlcAlert3: /* Primary sent SNRM while secondary in NRM. */
#ifdef SDLCALERT3_H
        memcpy(buf, (char*)&SDLCALERT3, sizeof(struct SdlcAlert3));
        createAlert = TRUE;
#endif
         break;

      case SdlcAlert4: /* Secondary sent FRMR W */
#ifdef SDLCALERT4_H
        memcpy(buf, (char*)&SDLCALERT4, sizeof(struct SdlcAlert4));
        createAlert = TRUE;
#endif
         break;

      case SdlcAlert5: /* Secondary sent FRMR X */
#ifdef SDLCALERT5_H
        memcpy(buf, (char*)&SDLCALERT5, sizeof(struct SdlcAlert5));
        createAlert = TRUE;
#endif
         break;

      case SdlcAlert6: /* Secondary sent FRMR Z */
#ifdef SDLCALERT6_H
        memcpy(buf, (char*)&SDLCALERT6, sizeof(struct SdlcAlert6));
        createAlert = TRUE; 
#endif
         break;

     case SdlcAlert7: /* Secondary sent FRMR Y */
#ifdef SDLCALERT7_H
        memcpy(buf, (char*)&SDLCALERT7, sizeof(struct SdlcAlert7));
        createAlert = TRUE;
#endif
         break;

      case SdlcAlert8: /* Primary sent FRMR W */
#ifdef SDLCALERT8_H
        memcpy(buf, (char*)&SDLCALERT8, sizeof(struct SdlcAlert8));
        createAlert = TRUE;
#endif
         break;

      case SdlcAlert9: /* Primary sent FRMR X */
#ifdef SDLCALERT9_H
        memcpy(buf, (char*)&SDLCALERT9, sizeof(struct SdlcAlert9));
        createAlert = TRUE; 
#endif
         break;

      case SdlcAlert10: /* Primary sent FRMR Z */
#ifdef SDLCALERT10_H
        memcpy(buf, (char*)&SDLCALERT10, sizeof(struct SdlcAlert10));
        createAlert = TRUE; 
#endif
         break;

      case SdlcAlert11: /* Receive window size exceeded */
#ifdef SDLCALERT11_H
        memcpy(buf, (char*)&SDLCALERT11, sizeof(struct SdlcAlert11));
        createAlert = TRUE;
#endif
         break;

      case SdlcAlert12: /* Secondary inactivity timer expired */
#ifdef SDLCALERT12_H
        memcpy(buf, (char*)&SDLCALERT12, sizeof(struct SdlcAlert12));
        createAlert = TRUE;
#endif
         break;

     case SdlcAlert13: /* Retry limit for XID exceeded */
#ifdef SDLCALERT13_H
        memcpy(buf, (char*)&SDLCALERT13, sizeof(struct SdlcAlert13));        
        createAlert = TRUE; 
#endif
         break;

     case SdlcAlert14: /* Secondary sent FRMR without IField */
#ifdef SDLCALERT14_H
        memcpy(buf, (char*)&SDLCALERT14, sizeof(struct SdlcAlert14));        
        createAlert = TRUE;
#endif
         break;

     case SdlcAlert15: /* Loss of EIA signal */
#ifdef SDLCALERT15_H
        if (sdb->sdlc_idb->failCause == sdb->sdlc_idb->lastFailCause)
           return;
        memcpy(buf, (char*)&SDLCALERT15, sizeof(struct SdlcAlert15));        
        createAlert = TRUE;
#endif
         break;

     case SdlcAlert16: /* Secondary linkstation sent a I frame too long */
#ifdef SDLCALERT16_H
        if (sdb->sdlc_idb->failCause == sdb->sdlc_idb->lastFailCause)
           return;
        memcpy(buf, (char*)&SDLCALERT16, sizeof(struct SdlcAlert16));        
        createAlert = TRUE;
#endif
         break;
      }
  
     if (createAlert){
         SetSdlcParam(buf, sdb);
         MV_SendAlert((MV*)buf);
     }
     else
        printf("\nSDLC  ALERT Alert%d not supported",alertType + 1);

}

