/* $Id: dspu_parse.c,v 3.5.18.3 1996/06/26 19:22:18 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dspu_parse.c,v $
 *------------------------------------------------------------------
 * DSPU Parser support
 *
 * April 94, Vivian Tseng
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dspu_parse.c,v $
 * Revision 3.5.18.3  1996/06/26  19:22:18  ppearce
 * CSCdi61351:  DSPU commands are not accepted on FDDI interface
 * Branch: California_branch
 *   Enable DSPU/FDDI configuration
 *
 * Revision 3.5.18.2  1996/05/17  10:47:53  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.4.5  1996/05/10  05:07:28  schiang
 * CSCdi57318:  dspu ncia configuration command needs default setting
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.4.4  1996/05/03  17:18:18  schiang
 * CSCdi56627:  DSPU/NCIA interface is broken
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.4.3  1996/04/26  07:35:08  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.5.4.2  1996/04/03  14:02:49  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.5.4.1  1996/03/28  08:20:27  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.5.18.1  1996/03/18  19:33:27  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.2  1996/03/07  08:45:28  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.14.1  1996/02/20  00:47:13  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/07  16:11:07  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.4  1996/02/01  06:01:58  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/12/15  22:10:58  ppearce
 * CSCdi45529:  DSPU/SNA commands should not be allowed on CIP interfaces
 *
 * Revision 3.2.4.1  1996/01/20  06:08:43  ppearce
 * CSCdi47227:  DLSw+ support for DSPU/SNANM via CLS/VDLC
 * Branch: Cal_Ibm_branch
 *
 * Revision 3.2  1995/11/17  09:04:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:28:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:56:27  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/06  23:15:40  ppearce
 * CSCdi36790:  Add interface parm to dspu host/pu config command
 *
 * Revision 2.1  1995/06/07  20:28:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include <string.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "interface_generic.h"
#include "config.h"
#include "parser.h"
#include "parser_defs_dspu.h"
#include "../parser/parser_defs_exec.h"
#include "ieee.h"
#include "dspu.h"
#include "dspuconf.h"
#include "dspu_notification.h"
#include "dspu_util.h"

void dspu_command (parseinfo *csb)
{

    uchar macAddr[IEEEBYTES];
    uchar x25Addr[ADDRLEN_X121];
    byte enableAddr = 0;

    memset(macAddr, 0, sizeof(macAddr));
    memset(x25Addr, 0, sizeof(x25Addr));

   /* 
    * If DSPU has not already been started 
    *   Start DSPU process and register with CLS 
    *
    * Don't start DSPU unless this is a config command 
    *   (i.e. csb->nvgen == 0)
    *
    */
    if (!IsDspuStarted() && !csb->nvgen)
        dspu_start();

    SetSnaMode(FALSE);
    switch (csb->which) {

      case SNA_RSRB:
        SetSnaMode(TRUE);
        if (csb->nvgen) {
            SnaShowConfig(SNA_RSRB);
            return;
        }
      /* fall down to DSPU_RSRB */

      case DSPU_RSRB:
	if (csb->nvgen) {
	   DspuShowConfig(DSPU_RSRB);
	   return;
	}

        if (GETOBJ(int,5)){      /* dspu rsrb start */
           if (csb->sense)
              DspuKeyRsrbStart(GETOBJ(string, 1));
           else 
              DspuNKeyStart(GETOBJ(string, 1));
        }
        else if (GETOBJ(int,6)){   /* dspu rsrb enable-host */
           if (csb->sense)
              DspuRsrbKeyEnable(GETOBJ(int,8), Upstream);
           else
              DspuRsrbNKeyEnable(GETOBJ(int,8));
        }
        else if (GETOBJ(int,7)){   /* dspu rsrb enable-pu */
           if (csb->sense)
              DspuRsrbKeyEnable(GETOBJ(int,8), Downstream);
           else
              DspuRsrbNKeyEnable(GETOBJ(int,8));
        }
        else {        /* dspu rsrb localRing bridgeNum targetRing macAddr */
	   ieee_copy(GETOBJ(hwaddr,1)->addr, macAddr);
	   if (csb->sense) {
	       DspuKeyRsrb(GETOBJ(int,1), GETOBJ(int,2), GETOBJ(int,3),
                              macAddr);
	   } else {
	       DspuNKeyRsrb(GETOBJ(int,1), GETOBJ(int,2), GETOBJ(int,3),
                              macAddr);
           }
	}
	break;

      case SNA_VDLC:
        SetSnaMode(TRUE);
        if (csb->nvgen) {
            SnaShowConfig(SNA_VDLC);
            return;
        }
      /* fall down to DSPU_VDLC */

      case DSPU_VDLC:
	if (csb->nvgen) {
	   DspuShowConfig(DSPU_VDLC);
	   return;
	}

        if (GETOBJ(int,5)){       /* dspu vdlc start */
           if (csb->sense)
              DspuKeyVdlcStart(GETOBJ(string, 1));
           else 
              DspuNKeyStart(GETOBJ(string, 1));
        }
        else if (GETOBJ(int,6)){  /* dspu vdlc enable-host */
           if (csb->sense)
              DspuVdlcKeyEnable(GETOBJ(int,8), Upstream);
           else
              DspuVdlcNKeyEnable(GETOBJ(int,8));
        }
        else if (GETOBJ(int,7)){  /* dspu vdlc enable-pu */
           if (csb->sense)
              DspuVdlcKeyEnable(GETOBJ(int,8), Downstream);
           else
              DspuVdlcNKeyEnable(GETOBJ(int,8));
        }
        else {                    /* dspu vdlc ringNumber macAddr */
	   ieee_copy(GETOBJ(hwaddr,1)->addr, macAddr);
	   if (csb->sense) {
	       DspuKeyVdlc(GETOBJ(int,1), macAddr);
	   } else {
	       DspuNKeyVdlc(GETOBJ(int,1), macAddr);
           }
	}
	break;

      case DSPU_NCIA:
	if (csb->nvgen) {
	   DspuShowConfig(DSPU_NCIA);
	   return;
	}

        if (GETOBJ(int,1)){  /* dspu ncia enable-pu */
           if (csb->sense)
              DspuNciaKeyEnable(GETOBJ(int,2), Downstream);
           else
              DspuNciaNKeyEnable(GETOBJ(int,2));
        } else {  /* dspu ncia server number */
	   if (csb->sense) {
	       DspuKeyNcia(GETOBJ(int,3));
	   } else {
	       DspuNKeyNcia(GETOBJ(int,3));
           }
	}

	break;

      case SNA_HOST:
        SetSnaMode(TRUE);
        if (csb->nvgen) {
            SnaShowConfig(SNA_HOST);
            return;
        }
      /* fall down to DSPU_HOST */

      case DSPU_HOST:
	if (csb->nvgen) {
	    DspuShowConfig(DSPU_HOST);
	    return;
	}
	else {
	    if (csb->sense) {

                if (GETOBJ(int,5)) {            /* X.25 address configured? */
                    memcpy(x25Addr, GETOBJ(paddr,1)->x121_addr,
                               sizeof(x25Addr));

		    /* qllc subaddress configured? */
		    if (GETOBJ(int, 15)) {
		        enableAddr = AsciiToQllcSubAddr(GETOBJ(string,3));
		    } else {
		        /* use the default value. */ 
		        enableAddr = 12;
		    }
                } else {
                    if (GETOBJ(int,6)) {      /* RMAC address configured? */
                        ieee_copy(GETOBJ(hwaddr,1)->addr, macAddr);
                    }    

                    /* Set lsap for dlc(s) other than x25. */
                    enableAddr = GETOBJ(int,8);
                }        
                  

		DspuKeyHost( 
                    GETOBJ(string, 1),                   /* Upstream PU name */
                    GETOBJ(string,2),                    /* XID              */
                   (GETOBJ(int,1) ? GETOBJ(int,2) : 0),  /* DLCI number      */
                   (GETOBJ(int,3) ? GETOBJ(int,4) : 0),  /* SDLC address     */
                   (GETOBJ(int,5) ? x25Addr : 0),        /* X.25 address     */
                   (GETOBJ(int,6) ? macAddr : 0),        /* Remote MAC addr  */
                    GETOBJ(int,7),                       /* Remote SAP addr  */
                    enableAddr,                          /* Local SAP addr   */
                   (GETOBJ(int,9) ? GETOBJ(idb,1) : 0),  /* Cfg'd interface  */
                    GETOBJ(int,10),                      /* Window size      */
                    GETOBJ(int,11),                      /* Max Iframe size  */
                    GETOBJ(int,12),                      /* Retry count      */
                    GETOBJ(int,13),                      /* Retry timeout    */
                    GETOBJ(int,14));                     /* FocalPoint PU    */
	    }
	    else {
		DspuNKeyHost(GETOBJ(string, 1));
	    }
	}
	break;

      case DSPU_ACTWINDOW:
	if (csb->nvgen) {
	    DspuShowConfig(DSPU_ACTWINDOW);
	    return;
	}
	else {
	    if (csb->sense) {
		DspuKeyActWindow(GETOBJ(int, 1));
	    }
	    else {
		DspuNKeyActWindow();
	    }
	}
	break;

      case DSPU_NOTIFICATION:
        if (csb->nvgen) {
            DspuShowConfig(DSPU_NOTIFICATION);
            return;
        }
        else {
            if (csb->sense)
                SetDspuNotificationLevel(GETOBJ(int, 1));
            else
                SetDspuNotificationLevel(DSPU_NOTIFY_LEVEL_DEFAULT);
        }
        break;

      case DSPU_POOL:
	if (csb->nvgen) {
	    DspuShowConfig(DSPU_POOL);
	    return;
	}
	else {
	    if (csb->sense) {
		DspuKeyPool(GETOBJ(string, 1), GETOBJ(string, 2), 
			GETOBJ(int, 1), GETOBJ(int, 2), GETOBJ(int,3) );
	    }
	    else {
		DspuNKeyPool(GETOBJ(string, 1));
	    }
	}
	break;

      case DSPU_PU:
	if (csb->nvgen) {
	    DspuShowConfig(DSPU_PU);
	    return;
	}
	else {
	    if (csb->sense) {

                if (GETOBJ(int,6)) {            /* X.25 address configured? */
                    memcpy(x25Addr, GETOBJ(paddr,1)->x121_addr,
                               sizeof(x25Addr));

		    /* qllc subaddress configured? */
		    if (GETOBJ(int,15)) {
		        enableAddr = AsciiToQllcSubAddr(GETOBJ(string,3));
		    } else {
		        /* use the default. */
		        enableAddr = 8;
		    }
                } else {
                    if (GETOBJ(int,7)) {      /* RMAC address configured? */
                        ieee_copy(GETOBJ(hwaddr,1)->addr, macAddr);
                    }    

                    /* Set Lsap for dlc(s) other than x25. */
                    enableAddr = GETOBJ(int,9);
                }

		DspuKeyPu(
                    GETOBJ(string, 1),                     /* Dnstream PU name*/
                   (GETOBJ(int,1) ? GETOBJ(string,2) : 0), /* XID             */
                   (GETOBJ(int,2) ? GETOBJ(int,3) : 0),    /* DLCI number     */
                   (GETOBJ(int,4) ? GETOBJ(int,5) : 0),    /* SDLC address    */
                   (GETOBJ(int,6) ? x25Addr : 0),          /* X.25 address    */
                   (GETOBJ(int,7) ? macAddr : 0),          /* Remote MAC addr */
                    GETOBJ(int,8),                         /* Remote SAP addr */
                    enableAddr,                            /* Local SAP addr  */
                   (GETOBJ(int,10) ? GETOBJ(idb,1) : 0),   /* Cfg'd interface */
                    GETOBJ(int,11),                        /* Window size     */
                    GETOBJ(int,12),                        /* Max Iframe size */
                    GETOBJ(int,13),                        /* Retry count     */
                    GETOBJ(int,14));                       /* Retry timeout   */
	    }
	    else {
		DspuNKeyPu(GETOBJ(string, 1));
	    }
	}
	break;

      case DSPU_LU:
	if (csb->nvgen) {
	    DspuShowConfig(DSPU_LU);
	    return;
	}
	else {
            char* poolName = GETOBJ(int,3)? GETOBJ(string,1): 0;
            char* hostName = GETOBJ(int,4)? GETOBJ(string,2): 0;
            char* puName = GETOBJ(int,6)? GETOBJ(string,3): 0;
	    if (csb->sense) {
		DspuKeyLu(GETOBJ(int,1),GETOBJ(int,2),poolName,
			hostName,puName,GETOBJ(int,5));
	    }
	    else {
		DspuNKeyLu(GETOBJ(int,1),GETOBJ(int,2),poolName,
			hostName,puName,GETOBJ(int,5));
	    }
	}
	break;

      case DSPU_DEFAULT_PU:
	if (csb->nvgen) {
	    DspuShowConfig(DSPU_DEFAULT_PU);
	    return;
	}
	else {
	    if (csb->sense) {
		DspuKeyDefaultPu(GETOBJ(int,1),GETOBJ(int,2));
	    }
	    else {
		DspuNKeyDefaultPu();
	    }
	}
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }

    if (!csb->nvgen)
        SetDspuLastConfigTime();   /* Set configuration timestamp */

    SetSnaMode(FALSE);

}

void dspuif_command (parseinfo *csb)
{

  byte enableAddr = 0;
  DspuDLCType_e dlcType;        

   /* 
    * If DSPU has not already been started 
    *   Start DSPU process and register with CLS 
    *
    * Don't start DSPU unless this is a config command 
    *   (i.e. csb->nvgen == 0)
    *
    */
    if (!IsDspuStarted() && !csb->nvgen)
        dspu_start();


    if ((csb->interface->hwptr->status & IDB_ETHER) != 0) {
        dlcType = DSPU_DLC_ETHERNET;
        enableAddr = GETOBJ(int,2);             /* LSAP address */
    } 
    else if ((csb->interface->hwptr->status & IDB_TR) != 0) {
        dlcType = DSPU_DLC_TOKEN_RING;
        enableAddr = GETOBJ(int,2);             /* LSAP address */
    }  
    else if ((csb->interface->hwptr->status & IDB_FDDI) != 0) {
        dlcType = DSPU_DLC_FDDI;
        enableAddr = GETOBJ(int,2);             /* LSAP address */
    }  
    else if ((csb->interface->hwptr->status & IDB_SERIAL) != 0) {
        switch (csb->interface->hwptr->enctype) {
            case ET_SDLC:
                dlcType = DSPU_DLC_SDLC;
                enableAddr = GETOBJ(int,4);     /* SDLC address */
                break;
            case ET_X25:
                dlcType = DSPU_DLC_QLLC;
                enableAddr = AsciiToQllcSubAddr(GETOBJ(string,1));    /* X25 subaddress */
                break;
            case ET_FRAME_RELAY:
                dlcType = DSPU_DLC_FRAME_RELAY;
                enableAddr = GETOBJ(int,2);     /* LSAP address */
                break;
            default:
                dlcType = DSPU_DLC_UNDEFINED;
                enableAddr = 0;
                break; 
        } /* End switch(enctype) */
    } 
    else {
        enableAddr = 0;
        dlcType = DSPU_DLC_UNDEFINED;
    }

    SetSnaMode(FALSE);
    switch (csb->which) {

      case SNA_ENABLE_HOST:
        SetSnaMode(TRUE);
        /* fall down to DSPU_ENABLE */

      case DSPU_ENABLE:
      case DSPU_ENABLE_HOST:
	if (csb->nvgen) {
          DspuEnableWrite(dlcType, csb->interface, csb->which == DSPU_ENABLE);
	    return;
	} else {

	    if (csb->sense) {
              DspuKeyEnable(dlcType, csb->interface, 
                              enableAddr,  csb->which == DSPU_ENABLE);

	    }
	    else {
              DspuNKeyEnable(dlcType, csb->interface, enableAddr);

	    }
	}
	break;
      
    case SNA_START:
    case DSPU_START:
        SetSnaMode(csb->which == SNA_START);
	if (csb->nvgen) {
	    return;
	}
	else {
	    if (csb->sense)
              DspuKeyStart(GETOBJ(string,1), dlcType, csb->interface);
	    else
		DspuNKeyStart(GETOBJ(string,1));
	}
	break;
      
     default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }

    if (!csb->nvgen)
        SetDspuLastConfigTime();   /* Set configuration timestamp */

    SetSnaMode(FALSE);

}

void dspu_show_command (parseinfo *csb)
{
    char *str = GETOBJ(string,1);
    if (strlen(str) == 0) {
        str = NULL;
    }
    switch (csb->which) {
      case SHOW_DSPU:	    /* dspu enable command */
         if (GETOBJ(int,3))   /* if pool command */
	     DspuShowPool(str, GETOBJ(int,2));
         else
	     DspuShow(str, GETOBJ(int,2));
         break;
         
     default:
	 bad_parser_subcommand(csb, csb->which);
	 break;
    }
}
