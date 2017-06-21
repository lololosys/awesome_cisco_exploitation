/* $Id: ip_post_config.c,v 3.1.6.4 1996/09/06 00:00:46 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/src-68-alc/ip_post_config.c,v $
 *------------------------------------------------------------------
 * ip_post_config.c
 *
 * ?
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ip_post_config.c,v $
 * Revision 3.1.6.4  1996/09/06  00:00:46  rbadri
 * CSCdi64212:  martian table bogusly externed in a .c file.
 * Branch: California_branch
 * Use registry to adjust the martian table. Don't manipulate it
 * directly with redecalared data structures.
 *
 * Revision 3.1.6.3  1996/06/05  21:50:53  sakumar
 * If no PVCs are setup for ILMI and Signaling, setup PVCs using the
 * default values for VPI/VCI. However, if user has already configured
 * these PVCs in NVRAM then do not over-ride with the default values.
 * CSCdi57175:  C5000: cannot define Qsaal, ILMI on vpi other than 0
 * Branch: California_branch
 *
 * Revision 3.1.6.2  1996/05/21  10:03:45  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.6.1  1996/05/09  14:41:10  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.1.52.2  1996/04/29  22:21:47  sakumar
 * Changes to Cat5K ATM HW specific code for IOS to link after latest sync.
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.52.1  1996/04/27  07:25:24  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.40.2  1996/04/15  21:21:41  sakumar
 * Support for VTP on NMP creating/destroying LECs on Cat5K ATM Module.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.1.40.1  1996/04/08  02:08:58  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.1.24.3  1996/03/27  21:55:29  sakumar
 * Got rid of some compiler warnings.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.24.2  1996/03/22  23:40:12  rlowe
 * Give relative paths to wbu include files.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.24.1  1996/03/22  09:43:42  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.2.1  1996/03/13  03:18:39  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.1  1996/03/11  08:15:25  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.7.6.2.2.2  1996/02/29  07:00:10  rlowe
 * Changes needed for IOS-side sync to Arkansas V111_0_16
 * (Synalc3_Cal_V111_0_16_branch).
 *
 * Revision 1.7.6.2.2.1  1996/02/06  06:31:15  rlowe
 * Sync-in changes from ATM21_branch between ATM31bch_baseline_960202
 * (aka ATM21bch_VF2106_960130) and ATM21bch_baseline_960205 (aka head
 * of ATM21_branch).
 * Branch: ATM31_branch
 *
 * Revision 1.7.6.3  1996/02/02  07:10:05  rlowe
 * Fix so that SNMP Emanate can be backed-out of ACP IOS images.
 * Branch: ATM21_branch
 *
 * Revision 1.7.6.2  1996/01/26  20:24:43  gudur
 * CSCdi46796:  IP domain lookup should be disabled
 * Branch: ATM21_branch
 *
 * Revision 1.7.6.1  1996/01/17  22:01:04  gudur
 * CSCdi46796:  IP domain lookup should be disabled
 * Branch: ATM21_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "../../wbu/cat5k/alc_include/acp_basic_types.h"
#include "../../wbu/cat5k/alc_include/lcp_if.h"

#undef	DISABLE			/* Eliminate conflicts with IOS definitions. */
#undef	ENABLE			/* Eliminate conflicts with IOS definitions. */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h" 
#include "config.h"
#include "../parser/parser_actions.h"
#include "../if/atm.h"
#include "../ip/ip_registry.h"


#define BUF 2048
char synalc_ippc_buffer[BUF];	/* Do NOT allocate this huge thing on stack. */

extern  hwidbtype * atmSyn_hwidb;


/* These additional CLI commands fed to the parser after NVRAM has been read.
   These commands ensure that telnet will work on startup despite (possibly) 
    misconfigured info in NVRAM.             */

int tcpStateTmp = FALSE ;		/* Hack for SNMP Emanate ! */
					/* -- see Emanate for use. */
void post_config(void)
{

    tLCP_IPAddrU acpip, nmpip;
    tLCP_MacAddrU nmpMac; 
    char tmp_str[100];
    vcd_t vcd;
  
    /* from LCP into command buffer */
    if ((LCP_ACP_IP_Addr(&acpip) == NULL) ||
	(LCP_NMP_IP_Addr(&nmpip) == NULL) || 
	(LCP_NMP_MacAddr(&nmpMac) == NULL))
        return; 

    sprintf(synalc_ippc_buffer, "hostname ATM\n");
    strcat(synalc_ippc_buffer, "no ip domain-lookup\n");
    strcat(synalc_ippc_buffer, "interface Ethernet0\n"); 
    sprintf(tmp_str, "ip address %d.%d.%d.%d  255.0.0.0\n",
        acpip.ch[0], acpip.ch[1], acpip.ch[2], acpip.ch[3]);
    strcat(synalc_ippc_buffer, tmp_str); 
    strcat(synalc_ippc_buffer, "no shutdown\n");
    strcat(synalc_ippc_buffer, "line vty 0 4\n");
    strcat(synalc_ippc_buffer, "no login\n");
    sprintf(tmp_str, "arp %d.%d.%d.%d  %x.%x.%x ARPA\n",
        nmpip.ch[0], nmpip.ch[1], nmpip.ch[2], nmpip.ch[3],
        (uInt16)nmpMac.word[0], (uInt16)nmpMac.word[1], (uInt16)nmpMac.word[2]);
    strcat(synalc_ippc_buffer, tmp_str); 
    strcat(synalc_ippc_buffer, "interface atm0\n");

    /* Setup Signaling PVC if one has not been setup */
    if ((!atmSyn_hwidb->atm_db->sig_vcnum) && 
        (ATM_OUT_OF_VCDS != (vcd = atm_getvcnum(atmSyn_hwidb)))) {
        sprintf(tmp_str, "atm pvc %d 0 5 qsaal\n", vcd);
        strcat(synalc_ippc_buffer, tmp_str); 
    }

    /* Setup ILMI PVC if one has not been setup */
    if ((!atmSyn_hwidb->atm_db->ilmi_vc) && 
        (ATM_OUT_OF_VCDS != (vcd = atm_getvcnum(atmSyn_hwidb)))) {
        sprintf(tmp_str, "atm pvc %d 0 16 ilmi\n", vcd);
        strcat(synalc_ippc_buffer, tmp_str); 
    }

    strcat(synalc_ippc_buffer, "no shut\n\0");

    /* allow 127.x.x.x IP addresses */
    if (!reg_invoke_ip_adjust_martian(0x7F000000, 0xFF000000, FALSE)) {
        buginf("**** Could not adjust martian table ****\n");
    }
    /* Now feed it to parser */
    parse_configure(synalc_ippc_buffer, TRUE, RES_MANUAL, PRIV_ROOT); 

    tcpStateTmp = TRUE;
}
