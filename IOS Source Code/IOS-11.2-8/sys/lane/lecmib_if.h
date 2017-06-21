/* $Id: lecmib_if.h,v 3.2.62.1 1996/07/11 20:34:49 cyoung Exp $
 * $Source: /release/112/cvs/Xsys/lane/lecmib_if.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 * ATMF LEC MIB utilities
 *     RFC 1573 IF-MIB support
 *
 * July 1995, Andy Bierman (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: lecmib_if.h,v $
 * Revision 3.2.62.1  1996/07/11  20:34:49  cyoung
 * Deregister RcvAddresses when client shutsdown/tearsdown.
 * CSCdi61990:  LANE CLIENT: memory leak
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:51:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:23:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:12:45  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/09  02:23:02  abierman
 * Placeholder for ATMF LEC MIB -- RFC 1573 (IF-MIB) support
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef LECMIB_IF_H
#define LECMIB_IF_H

#define IF_DESCR_AFLANE_8023      "AF-LANE 802.3 CLIENT"
#define IF_DESCR_AFLANE_8025      "AF-LANE 802.5 CLIENT"


/******************* V A R I A B L E S *********************/
extern unsigned         next_snmp_lec_index;


/****************** INLINE FUNCTIONS *******************/
static inline void 
lecmib_clear_cur_if_cntrs (lec_snmp_info_t *lec_snmp)
{
    lec_snmp->cache_serial_num = NOT_SET_FLAG;
    lec_snmp->in_octets = 0;            /* bits 0..31 */
    lec_snmp->in_octets_HC = 0;         /* bits 32..63 */
    lec_snmp->in_ucast_pkts = 0;
    lec_snmp->in_mcast_pkts = 0;
    lec_snmp->in_bcast_pkts = 0;
    lec_snmp->in_unknown_protos = 0;
    lec_snmp->in_errors = 0;
    lec_snmp->in_discards = 0;
    lec_snmp->out_octets = 0;            /* bits 0..31 */
    lec_snmp->out_octets_HC = 0;         /* bits 32..63 */
    lec_snmp->out_ucast_pkts = 0;
    lec_snmp->out_mcast_pkts = 0;
    lec_snmp->out_bcast_pkts = 0;
    lec_snmp->out_errors = 0;
    lec_snmp->out_discards = 0;
}

/***************** E X T E R N A L    F U N C T I O N S *************/
/* changed for registry function -- exported in lane_registry.reg
 *
 * extern void lecmib_count_deleted_SVC (lec_info_t *lec_info,
 *				      lec_vc_t   *lec_svc);
 */

extern void lecmib_count_deleted_SVC (void *lec_info_mask,
				      void *lec_svc_mask);

extern void lecmib_if_init(void);

extern void lecmib_if_register(lec_info_t  *lec_info, boolean init_lec_up);

extern void lecmib_if_complete_entry(lec_info_t  *lec_info); 
extern void lecmib_if_deregister_rcv_addr(lec_info_t *lec_info);

extern void lecmib_if_deregister(lec_info_t  *lec_info);
#endif  /* LECMIB_IF_H */

