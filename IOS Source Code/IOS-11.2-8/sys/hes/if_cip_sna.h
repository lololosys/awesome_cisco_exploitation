/* $Id: if_cip_sna.h,v 3.2 1995/11/17 09:17:37 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/if_cip_sna.h,v $
 *------------------------------------------------------------------
 * if_cip_sna.h - SNA support for the CIP.
 *
 * May 1995, Mike Otto
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_cip_sna.h,v $
 * Revision 3.2  1995/11/17  09:17:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:39:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/08/09  19:29:18  rbatz
 * CSCdi38250:  CIP CSNA SNMP traps do not work
 *
 * Revision 2.4  1995/07/20  13:33:59  motto
 * CSCdi37360:  CIP SNA LLC CC state display incorrect
 *
 * Revision 2.3  1995/07/18  21:46:28  motto
 * Update MIBs to current level of CIP support
 * Cleaned up CSNA show command support
 * CSCdi35714:  Some show commands for CIP-SNA are not working
 *
 * Revision 2.2  1995/06/21  20:28:10  motto
 * Fix 'show extended channel x/2 <llc2|csna|connection-map|
 * max-llc2-sessions> ....' commands for SNA support on the CIP card.
 * CSCdi35714:  Some show commands for CIP-SNA are not working
 *
 * Revision 2.1  1995/06/07  20:41:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IF_CIP_SNA_H__
#define __IF_CIP_SNA_H__

#include "sysmgt_snallc.h"
#include "sysmgt_cipcsna.h"
 
char *get_llc_cc_state( llc_cc_oper_t *cc_oper);
char *get_last_ww_cause(long last_ww_cause);
char *get_csna_status_text(csna_dev_t *csna_dev);
char *get_csna_slowdown_text(csna_dev_t *csna_dev);
csna_dev_t *csna_dev_rpc(hwidbtype *hwidb, cip_vctype *vc, 
                         csna_dev_t *csna_dev);
csna_dev_t *get_csna_dev(int search, long *slot, long *port,
                         long *path, long *dev, csna_dev_t *csna_dev);
boolean get_admin_llc2_sessions( int search, long *slot, long *sessions);
boolean get_from_cip_llc2_session_info( hwidbtype * hwidb, int search,
                                        llc2_sessions_t *llc2_sess);
boolean get_llc2_sessions_info( int search, long *slot, 
                                llc2_sessions_t *llc2_sess);
boolean get_llc2_conn(hwidbtype *hwidb, unsigned long csna_handle, 
                      csna_llc2_conn_t *csna_llc2_conn);

boolean get_cip_llc_port(int search, hwidbtype *hwidb, long *virtual_index, 
                         void *port_p, ushort size, ushort table_id);
boolean get_cip_llc_sap(int search, hwidbtype *hwidb, long *virtual_index, 
                        long local_sap, void *sap_p, ushort size, 
                        ushort table_id);
boolean get_cip_llc_cc(int search, hwidbtype *hwidb, 
                       long *virtual_index, long local_sap,
                       uchar *rem_mac, long rem_sap,
                       void *cc_p, ushort size, ushort table_id);

#endif
