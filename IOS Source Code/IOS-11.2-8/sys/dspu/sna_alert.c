/* $Id: sna_alert.c,v 3.3.58.4 1996/08/26 15:05:39 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dspu/sna_alert.c,v $
 *------------------------------------------------------------------
 * SNA Network Management Alert common routine
  *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sna_alert.c,v $
 * Revision 3.3.58.4  1996/08/26  15:05:39  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.3.58.3  1996/07/09  05:49:39  ppearce
 * CSCdi59527:  ascii string in inline wastes code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.3.58.2  1996/05/17  10:49:28  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.46.2  1996/04/26  15:17:32  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.46.1  1996/04/03  14:03:51  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3.58.1  1996/03/18  19:34:11  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.24.3  1996/03/14  00:54:06  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.3.24.2  1996/03/07  08:46:09  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.24.1  1996/01/24  21:42:47  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  09:06:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:22:56  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:29:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/20  17:03:35  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 * Revision 2.1  1995/06/07 20:30:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include <string.h>
#include "interface_private.h"
#include "../if/tring.h"
#include "../srt/srb_core.h"
#include "snautil.h"
#include "mv_sr.h"
#include "dspu_fp.h"
#include "../util/itemlist.h"



/*=====================================================================*
* SetX25Address
*======================================================================*
*
* Description:
*   Set 15 bytes of X-25 local DTE address in ebcdic
*/

void SetX25Address (byte* ebcdic, addrtype* x25Addr)
{
   int len = x25Addr->length;
   memset(ebcdic, 0x40, 15);          /* set x25 address */
   if (x25Addr->x121_addr[0] > 0){
        if (len > 15)
            len = 15;
       ConvertAsciiToEbcdic(x25Addr->x121_addr, ebcdic, len);
     }
}



/*=====================================================================*
* SetFailCause
*======================================================================*
*
* Description:
*   Set fail cause at detail data (0x82) NMVT
*/

void SetFailCause (hwidbtype* idb, byte* sv, const char* name)
{
   short sfLen;
   byte* sf;
   char  intName[16];
   hword errorCode = 0; 
   ONE_ITEM_LIST(tmp, TOKEN_HW_RING_STATUS, sizeof(hword), 0);

   (*idb->state_config)(idb, IDB_CONTROL_READ_STATE, &tmp_list);
   errorCode = tmp_item.u.s_value[0];

   ForAllSfInSV(sv,sf,sfLen) {
       if (sf[1] == 0x82) {
           switch (sf[3]) {
 	      case 0x07:   /* token ring error code */
                 sf[5] = errorCode >> 8;
                 sf[6] = errorCode;
                 break;

	      case 0x17: /* Ring status */
                 sf[5] = errorCode >> 8;
                 sf[6] = errorCode;
                 break;     

	       case 0x58: /* local dte address */
                  SetX25Address(&sf[5], idb->x25_address);
                  break;

              case 0x60:   /* port number */
                 sf[5] = idb->unit;
                 break;

              case 0x61: /* Adapter number */
                 sf[5] = idb->hw_if_index; 
                 break;

             case 0xF2:    /* Router Interface name */
                 sprintf(intName, "%sSerial%d ", name, idb->unit);
                 ConvertAsciiToEbcdic(intName, &sf[5], 13);
                 break;

	    }
	} /* if */

    }
}
/*=====================================================================*
* UpdateCpName, SetCpName
*======================================================================*
*
* Description:
*   Update CP name using hostname if hostname configured, else use
* focalPoint pu name. If focalPoint pu is not configured, use "Router"
*/

static void UpdateCpName(byte* cpVector)
{
   const char* cpName = 0;
   if (strcmp(hostname, "Router") == 0)
        cpName = GetFocalPuName();
   if (cpName == 0)
       cpName = hostname;
   ConvertToEbcdicName(cpName,cpVector);  /* cp name */
}

void SetCpName (byte *sv)
{
   byte* sf;
   short sfLen;
   ForAllSfInSV(sv,sf,sfLen) {
       if (sf[1] == 0x10) {      /* hierachy name list */
          byte* ssf = &sf[3];    /* length field of hierachy name list entry*/
          byte lenLeft = sf[0] - 3;
          while (lenLeft > 10){   /* each name list > 10 */
               if (ssf[10] == 0xf4)
                  UpdateCpName(&ssf[1]);
               ssf += 11;
               lenLeft -= 11;
	   }
       }
   }
}

void SetHRname (hwidbtype* idb,  byte* sv, const char* adaptorName)
{
    idbtype     *swidb;
    const char* lanName;
    byte* sf;
    short sfLen;
    char name[10];
    name[8] = 0;       /* ensure name is null terminated */  
    ForAllSfInSV(sv,sf,sfLen) {
       if (sf[1] == 0x10) {      /* hierachy name list */
          byte* ssf = &sf[3];    /* length field of hierachy name list entry*/
          byte lenLeft = sf[0] - 3;
          while (lenLeft > 4){   /* each name list > 4 */
              switch (ssf[ssf[0] + 1]) {
	         case 0x39:         /* lan name */
                    if (*idb->firstsw->lanName){
                        memcpy(name, idb->firstsw->lanName, 8);
                        lanName = name;
                    }
                    else if (strlen(idb->hw_namestring) > 8)
                        lanName = idb->hw_short_namestring;
                    else
                        lanName = idb->hw_namestring;
                    ConvertToEbcdicName(lanName, &ssf[1]);
                    break;
 
		 case 0xf4:       /* control point name */
                    UpdateCpName(&ssf[1]);
                    break; 

	         case 0x2e:        /* ring name */
                    swidb = firstsw_or_null(idb);
                    sprintf(name, "RING%d", srbV.srb_sb_get_thisring(swidb));
                    ConvertToEbcdicName(name, &ssf[1]);
                    break;

		 case 0x32:       /* cbus name */
                    ConvertToEbcdicName("UNKNOWN", &ssf[1]);
                    break;

		 case 0x21:      /* adaptor name */
                    sprintf(name, "%s-%d",adaptorName,idb->hw_if_index); 
                    ConvertToEbcdicName(name, &ssf[1]); 
                    break;

		  case 0x3f:     /* port */
                    sprintf(name,"SERIAL%d",idb->unit) ;
                    ConvertToEbcdicName(name, &ssf[1]); 
                    break;  

               }
               lenLeft -= (ssf[0] + 2);
               ssf += ssf[0] + 2;    /* next name list */
	    }
        }
    }
}
                            


void SetLanParam (byte* buf, hwidbtype* idb, const char* name)
{
   byte* sv;
   byte* sf;
   short svLen;
   short sfLen;
   idbtype *swidb;
   ushort srb_thisring;

  
    ForAllSvInMV(((MV*)buf), sv, svLen){
        switch (sv[1]){

        case 0x96:       /* Failure Cause */
        case 0x94:       /* User Cause */
            SetFailCause(idb, sv, name);
            break;
        case 0x51: /* LAN LCS Data */
            ForAllSfInSV(sv,sf,sfLen) {
               switch(sf[1]){
		  case 2:      /* ringOrBusNum */
                     swidb = firstsw_or_null(idb);
                     srb_thisring = srbV.srb_sb_get_thisring(swidb);
                     sf[2] = srb_thisring >> 8;
                     sf[3] = srb_thisring;
                     break;
		   case 3:     /* local individual mac address */
                     memcpy(&sf[2], idb->hardware, 6);
                     break;

                  case 4:     /* remote individual mac address */
                     memset(&sf[2], 0, 6);
                     break; 

  		  case 0x06: /* TR fault domain description */
                      memset(&sf[2], 0, 6);
                      memset(&sf[8], 0, 6);
                      break;

                  case 0x07: /* Beacon data */
		     if (is_tokenring(idb)) {
			 hword ring_status;
			 ONE_ITEM_LIST(tmp, TOKEN_HW_RING_STATUS,
				       sizeof(hword), 0);
		     
			 (*idb->state_config)(idb, IDB_CONTROL_READ_STATE,
					      &tmp_list);
			 ring_status = tmp_item.u.s_value[0];
			 
			 sf[2] = ((ring_status & RNG_RECOVERY) ? 1 : 2);
		     } else {
			 sf[2] = 2;
		     }
                     break;

                  case 8:     /* single Mac address */
                     memcpy(&sf[2], idb->bia, 6);
                     break;

                  case 0x28:  /* mac name           */
                     memset(&sf[2], 0x40, 8);
                     break; 
                 }
	    }
            break;
	case 0x05: /* Hierarchy/Resource Name List */
            SetHRname(idb, sv, name);
            break;
	}
    }
}

void SendAlertToHost (byte* buf, hwidbtype*   idb)
{
   if (idb){
      byte* sv;
      short svLen;
  
      ForAllSvInMV(((MV*)buf), sv, svLen){
         if (sv[1] == 0x5)
            SetHRname(idb, sv, "");
      }
   }
   MV_SendAlert((MV*)buf);
}
