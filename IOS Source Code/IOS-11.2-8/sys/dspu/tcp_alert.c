/* $Id: tcp_alert.c,v 3.2.60.2 1996/05/17 10:49:51 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dspu/tcp_alert.c,v $
 *------------------------------------------------------------------
 * SNA Network Management TCPIP Alert Handler
 *
 * Jan 95, Silvana Zdravkovski
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tcp_alert.c,v $
 * Revision 3.2.60.2  1996/05/17  10:49:51  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.2  1996/04/26  07:35:30  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.2.48.1  1996/04/03  14:04:08  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2.60.1  1996/03/18  19:34:26  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:46:19  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:49:02  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:06:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:30:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:31:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include "interface_generic.h"
#include "../snanm/tcp_ip_alert1.alth"
#include "mv_sr.h"
#include "connect.h"
#include "../tcp/tcp.h"
#include "snautil.h"
#include "registry.h"
#include "../dspu/snanm_registry.regh"
#include "lan_alert.h"

static const char ipInterface[] = "TO TCP/IP NETWORK";
void Tcp_Alert(conntype* connptr)
{
    byte* sv;
    byte* sf;
    short svLen;
    short sfLen;
    byte buffer[sizeof(struct TcpIp1)];
    byte buf[MaxAlertSize];
    tcbtype* tcbptr = (tcbtype*)connptr->tcb;

    memcpy(buf, (char*)&TCPIP1, sizeof(struct TcpIp1));
    
    ForAllSvInMV(((MV*)buf), sv, svLen){
        if (sv[1] == 0x96) {
            ForAllSfInSV(sv,sf,sfLen) {
                if (sf[1] == 0x82) {
                   switch (sf[3]) {
		     case 0xf2:     /* interfaces to network */
                        ConvertAsciiToEbcdic(ipInterface, &sf[5], 17);
                        break;

                     case 0x38: /* destination */ 
                        sprintf(buffer, "%i %d", tcbptr->foreignhost.ip_addr, tcbptr->foreignport);
                        memset(&sf[5], 0x40, 21);
                        ConvertAsciiToEbcdic(buffer, &sf[5], strlen(buffer));
                        break;
                      case 0x39: /* originator */ 
                        sprintf(buffer, "%i %d", tcbptr->localhost.ip_addr, tcbptr->localport);
                        memset(&sf[5], 0x40, 21);
                        ConvertAsciiToEbcdic(buffer, &sf[5], strlen(buffer));
                        break;

		    }
		}
	    }
	}
        else if (sv[1] == 0x05)
           SetCpName(sv);       /* set control point name */
    }
    MV_SendAlert((MV*)buf);
}

