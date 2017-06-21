/* $Id: vtp_pruning_util.c,v 1.18 1996/10/08 02:29:35 bpan Exp $
 * $Source: /wbu-sw/dev/synergy/syn_core/nmp/nmpflash/vtp/vtp_pruning_util.c,v $
 *------------------------------------------------------------------
 * vtp_pruning_util.c
 *  Debugging/Engineering functions for vtp pruning on Catalyst 5000
 *
 * Jul 1996, Bider Pan
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vtp_pruning_util.c,v $
 * Revision 1.18  1996/10/08  02:29:35  bpan
 * Set management vlan to be always "vlan local assoc"
 *
 * Revision 1.17  1996/10/03  23:36:47  bpan
 * Obsoleted the engineering "set pruning en/disable" commands since it can be done
 * in normal "set vtp" command
 *
 * Revision 1.16  1996/10/03  20:23:47  bpan
 * Removed redundant "\n" from syslog msg
 *
 * Revision 1.15  1996/10/01  19:24:12  bpan
 * Cleanup and reorganized trace & syslog msg for vtp pruning
 *
 * Revision 1.14  1996/09/25  01:31:23  bpan
 * -Added error recovery support if pruning events dropped due to lack of mbuf
 * -Created VTP_PRUNING_LOG_xxx for vtp pruning
 *
 * Revision 1.13  1996/09/20  23:21:11  bpan
 * Fixes after testing "pruning mode" capability
 *
 * Revision 1.12  1996/09/18  00:34:22  bpan
 * Initially added "pruning en/disable" supports
 *
 * Revision 1.11  1996/09/14  04:54:08  bpan
 * -Changed vtp_add/change_domain prototypes to support "pruning dis/enable"
 * -Modified existing functions to support "pruning dis/enable"
 *
 * Revision 1.10  1996/09/13  21:10:25  bpan
 * Changed engineering function prototypes for pruning
 *
 * Revision 1.9  1996/09/09  23:17:00  bpan
 * Modifed tracing msgs to display more useful info
 *
 * Revision 1.8  1996/09/07  01:03:04  bpan
 * -Added more functions for engineering commands
 * -Modified function prototypes
 * -Modified tracing messages
 *
 * Revision 1.7  1996/08/30  01:34:35  bpan
 * Modified tracing messages
 *
 * Revision 1.6  1996/08/28  02:04:54  bpan
 * Changed vtp pruning debugging messages
 *
 * Revision 1.5  1996/08/23  22:08:42  bpan
 * Changed core function prototypes for status/statistics
 * Changed trace category to PRUNING_TRACE
 *
 * Revision 1.4  1996/08/16  22:09:07  bpan
 * Changed display format for "trunk not found" failures
 * Display CBL & LTL bits for debugging commands
 *
 * Revision 1.3  1996/08/11  06:04:50  bpan
 * Added changes for integration test
 *
 * Revision 1.2  1996/08/03  06:31:27  bpan
 * add more engineering command support
 *
 * Revision 1.1  1996/07/19  02:24:00  bpan
 * Initially checked in
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-------------------------------------------------------------------------
 * Note: See vtp.c for pre-cautions
 *------------------------------------------------------------------------*/
#include "vtp_debug.h"
#include "vtp_externs.h"
#include "vtp_interface.h"
#include "vtp_pub.h"

#include "vtp_private.h"
#include "vtp_to_mib.h"
#include "vtp_pruning.h"
#include "vtp_local.h"    /* especially for pruning now */


/**************************************************************************
 * 
 *		    Debugging/Engineering Command Support
 *	
 *	        Note: this part is local for catalyst 5000
 *		      it is not part of the general portable code
 *
 **************************************************************************/

/*-------------------------------------------------------------------------
 * Globals:
 *------------------------------------------------------------------------*/   
static unsigned char tmp_buf[ VLAN_BIT_STR_SIZ ];

static unsigned char *state_str[ FSM_STATES_NUM ] = { 
    "Null   ", "Disable", "NoPrune", "Joined ", "Pruned " };

static unsigned char *event_str[ FSM_ALL_EVENTS_NUM ] = { 
    "Null  ", "RxVer1", "RxJ1  ", "RxJ0  ", "Fwd   ", "NonFwd",
    "Tick  ", "Tmo   ", "LnkDn ", "LnkUp ",
    "Inelig", "Eligi ", "Trans ", "NoTran", "AddTrk", "Refrsh" };
    
static unsigned char *abbre_domain_str =
    "(PE=pruning eligible, LA=local assoc, E=exist, N-=Non)\n";

static unsigned char *abbre_trunk_str = 
    "(T=trunk, J=joining, F=forwarding, N-=Non)\n";

unsigned char *vtp_mode_str[4] = { "?","Client","Server","Transparent" };
unsigned char *vtp_pruning_mode_str[2] = { "Disable","Enable" };
unsigned char *link_state_str[2] = { "DN","UP" };
unsigned char *vlan_type[6] = { "?","ETHER","FDDI","TR","FDDI-NET","TR-NET" };


static unsigned char crlf[] = {"\n"};

/*-------------------------------------------------------------------------
 * Externals:
 *------------------------------------------------------------------------*/
extern unsigned long no_buf_cnt;

/*-------------------------------------------------------------------------
 * Macros:
 *------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * vtp_printVlans:
 *  swap the byte order 0,1,2 => 127,126,... b/c printVlans decodes the latter
 *------------------------------------------------------------------------*/
void
vtp_printVlans (unsigned char *bits, char *buf) {
}   /*vtp_printVlans*/

/*-------------------------------------------------------------------------
 * show_vtp_core_pruning_domain:
 *------------------------------------------------------------------------*/
void
show_vtp_core_pruning_domain (unsigned char dname_length, 
			      unsigned char *domain_name) {
    vlan_info_ent_ptr	vlan_info_p;
    vlan_domain_ptr	domain_p;
    vtp_trunk_itf_ptr	trunk_p;
    unsigned short	isl_vlan_id, offset;
    unsigned char	mask;
    
    domain_p = vtp_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	printf("Domain %s (%d) does not exist!\n", domain_name, dname_length);
	return;
    }
    
    printf("\nDomain %s pruning"
	   " info\n-------------------------------------\n",
	   domain_p->domain_name); 
    
    /* ----- print existing vlans ----- */
    printf("Existing Vlans:\t\t\t");
    memset(tmp_buf, 0, VLAN_BIT_STR_SIZ);
    for (vlan_info_p = domain_p->vlan_head; vlan_info_p; vlan_info_p =
	 vlan_info_p->isl_next) {
	isl_vlan_id = vlan_info_p->isl_vlan_id;
	
	/* make a bit string */
	offset = vlan_bit_offset(isl_vlan_id,VTP_VLAN_BASE);
	mask = vlan_bit_mask(isl_vlan_id,VTP_VLAN_BASE);
	tmp_buf[offset] |= mask;
				
    } /*for(vlans)*/
    vtp_printVlans(tmp_buf, (char *)NULL);
    printf(crlf);
    
    /* vlan_exist bits (for comparison & verify) */
    printf("Vlans in existing bit string:\t");
    vtp_printVlans(domain_p->vlan_exist, (char *)NULL);
    printf(crlf);
    
    /* ----- print existing trunks ----- */
    printf("Existing Trunks:\t\t");
    for ( trunk_p = domain_p->trunk_head; trunk_p; trunk_p =
	 trunk_p->trunk_next) {
	printf("%s ", trunk_p->trunk_name);
    } 
    printf(crlf);
    
    /* vlan local assoc */
    printf("Local Association on vlans:\t");
    vtp_printVlans(domain_p->vlan_local_assoc, (char *)NULL);
    printf(crlf);
    
    /* misc */
    printf("\nMin Vlan=%d, Max Vlan=%d, Status=", 
	   domain_p->min_vlan_no, domain_p->max_vlan_no);
    if (domain_p->status & DOMAIN_STS_ADD_DOMAIN)
      printf("AddDomain ");
    if (domain_p->status & DOMAIN_STS_ADD_VLAN)
      printf("AddVlan ");
    if (domain_p->status & DOMAIN_STS_ADD_TRUNK)
      printf("AddTrunk ");
    if (!domain_p->status)
      printf("0");
    printf("\nMode = %s, Pruning Mode = %s\n", vtp_mode_str[domain_p->mode], 
	   vtp_pruning_mode_str[domain_p->pruning_mode]);
    printf("no_domain_state = %x, transitioning = %x\n", no_domain_state,
	   domain_p->transitioning);
    printf("vlan_count = %d, No buffer count = %d\n", 
	   domain_p->vlan_count, no_buf_cnt);
    
}   /*show_vtp_core_pruning_domain*/

/*-------------------------------------------------------------------------
 * show_vtp_core_pruning_trunk:
 *------------------------------------------------------------------------*/
void
show_vtp_core_pruning_trunk (unsigned char dname_length, 
			     unsigned char *domain_name,
			     void *trunk_id, 
			     unsigned short start_vlan, 
			     unsigned short vlan_num) {
    vlan_domain_ptr	domain_p;
    vtp_trunk_itf_ptr	trunk_p;
    
    /* find domain */
    domain_p = vtp_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	printf("Domain %s (%d) does not exist!\n", domain_name, dname_length);
	return;
    }
    
    /* ----- show trunk info ----- */
    if ( trunk_id == NULL )
      goto show_all_trunks;
    
    /* find trunk link */
    trunk_p = vtp_find_trunk_link_in_domain(domain_p, trunk_id);
    if ( !trunk_p ) {
	printf("Trunk (%d/%d) does not exist in domain %s!\n", 
	       ((*(unsigned short *)trunk_id)>>6)+1, 
	       ((*(unsigned short *)trunk_id)&0x3f)+1, domain_p->domain_name);
	return;
    }
    
    printf(abbre_trunk_str);
    show_vtp_pruning_trunk_detail(domain_p, trunk_p, start_vlan, vlan_num);
    return;
    
 show_all_trunks:
    printf(abbre_trunk_str);
    for (trunk_p = domain_p->trunk_head; trunk_p; trunk_p =
	 trunk_p->trunk_next) { 
	show_vtp_pruning_trunk_detail(domain_p, trunk_p,
				      start_vlan, vlan_num);
	vtp_process_reschedule();
    }
    return;
    
}   /*show_vtp_core_pruning_trunk*/


/*-------------------------------------------------------------------------
 * show_vtp_core_pruning_vlan:
 *------------------------------------------------------------------------*/
void
show_vtp_core_pruning_vlan (unsigned char dname_length, 
			    unsigned char *domain_name, 
			    unsigned short start_vlan, 
			    unsigned short vlan_num) {
    vlan_domain_ptr	domain_p;
    vlan_info_ent_ptr	vlan_info_p;
    unsigned short	cnt;

    cnt = 0;
    
    /* find domain */
    domain_p = vtp_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	printf("Domain %s (%d) does not exist!\n", domain_name, dname_length);
	return;
    }
    
    printf(">>>>> Domain %s Vlan Info\n", domain_p->domain_name);
    printf(abbre_domain_str);
    printf(abbre_trunk_str);
    if ( !vlan_num )
      return;
    for (vlan_info_p = domain_p->vlan_head; vlan_info_p; vlan_info_p =
	 vlan_info_p->isl_next) {
	if ( vlan_info_p->isl_vlan_id >= start_vlan ) {
	    if ( vlan_info_p->isl_vlan_id >= VTP_FACTORY_DEFAULT_VLAN_FDDI_ISL
		&& start_vlan < VTP_FACTORY_DEFAULT_VLAN_FDDI_ISL )
		break;		    /* default vlan 1002- not shown usually */
	    
	    show_vtp_pruning_vlan_detail(domain_p, vlan_info_p);
	    if ( ++cnt >= vlan_num )
		break;
	    vtp_process_reschedule();
	}
	
    } /*for(vlans)*/
}   /*show_vtp_core_pruning_vlan*/

/*-------------------------------------------------------------------------
 * show_vtp_core_pruning_status:
 *------------------------------------------------------------------------*/
void
show_vtp_core_pruning_status (unsigned char dname_length, 
			      unsigned char *domain_name, 
			      void *trunk_id) {
    vlan_domain_ptr	domain_p;
    vtp_trunk_itf_ptr	trunk_p;
    
    /* find domain */
    domain_p = vtp_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	printf("Domain %s (%d) does not exist!\n", domain_name, dname_length);
	return;
    }
    
    if ( trunk_id == NULL )
      goto show_all_status;
    
    /* find trunk link */
    trunk_p = vtp_find_trunk_link_in_domain(domain_p, trunk_id);
    if ( !trunk_p ) {
	printf("Trunk (%d/%d) does not exist in domain %s!\n", 
	       ((*(unsigned short *)trunk_id)>>6)+1,
	       ((*(unsigned short *)trunk_id)&0x3f)+1, domain_p->domain_name);
	return;
    }
    
    printf("Trunk %s\n-------------------\n", trunk_p->trunk_name);
    show_vtp_pruning_status_detail(trunk_p);
    return;
    
 show_all_status:
    for (trunk_p = domain_p->trunk_head; trunk_p; trunk_p =
	 trunk_p->trunk_next) {
	printf("Trunk %s\n-------------------\n", trunk_p->trunk_name);
	show_vtp_pruning_status_detail(trunk_p);
    }
    return;
    
}   /*show_vtp_core_pruning_status*/


/*-------------------------------------------------------------------------
 * set_pruning_join_msg:
 *------------------------------------------------------------------------*/
void
set_pruning_join_msg (unsigned char dname_length, 
		      unsigned char *domain_name, void *trunk_id,
		      unsigned short min_vlan_no, unsigned short max_vlan_no, 
		      unsigned char *join_bits, 
		      void **buf, unsigned int *data_len) {
    vlan_domain_ptr	domain_p;
    void		*buff_p;
    join_msg_ptr	join_p;
    unsigned char	*bits;
    unsigned int	bitlen;
    unsigned		ix;
    
    /* find domain */
    domain_p = vtp_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	printf("Domain %s (%d) does not exist!\n", domain_name, dname_length);
	return;
    }
    
    /* ----- get buffer space ----- */
    buff_p = vtp_get_buffer( MAX_JOIN_MSG_LEN);
    if ( !buff_p ) {
	printf("no buffer to build Join\n");
	*buf = NULL;
	return;
    } else {
	*buf = buff_p;
    }
    
    /* ----- build a join msg ----- */
    join_p = vtp_buffer_payload( trunk_id, buff_p);
    join_p->version = VTP_VERSION;
    join_p->code = VTP_JOIN_MSG;
    join_p->reserved = 0;
    join_p->dname_length = dname_length;
    
    for( ix = 0; ix < dname_length; ix++ )
      join_p->domain_name[ix] = domain_name[ix];
    
    for( ; ix < VTP_MAX_DOMAIN_NAME_LENGTH; ix++ )
      join_p->domain_name[ix] = 0; 
    
    join_p->first_vlan = vtp_format_2_bytes(min_vlan_no);
    join_p->last_vlan  = vtp_format_2_bytes(max_vlan_no);
    
    /* ----- points to vlan joining string in Join msg ----- */
    bits = ((unsigned char *)join_p) + sizeof(join_msg);
    bitlen = (max_vlan_no - min_vlan_no + 1) /8;
    memcpy( bits, &(join_bits[ min_vlan_no/8 ]), bitlen );
    *data_len = sizeof(join_msg) + bitlen;
    
}   /*set_pruning_join_msg*/

/*-------------------------------------------------------------------------
 * set_pruning_ver1_msg:
 *  Simulate a ver1 or ver1+ summary adv pkt (for testing only)
 *------------------------------------------------------------------------*/
void 
set_pruning_ver1_msg (unsigned char dname_length, 
		      unsigned char *domain_name, void *trunk_id,
		      unsigned char join_support_flag,
		      void **buf, unsigned int *data_len) {
    vlan_domain_ptr	domain_p;
    void		*buff_p;
    summary_adv_ptr	summary_adv_pkt_p;
    unsigned long	ix;
    summary_pruning_tlv_ptr	tlv_p;

    /* find domain */
    domain_p = vtp_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	printf("Domain %s (%d) does not exist!\n", domain_name, dname_length);
	return;
    }
    
    /* get buffer space */
    buff_p = vtp_get_buffer( sizeof(summary_adv)+2);
    if (!buff_p) {
       printf("no buffer to build ver 1 Summary\n");
       return;
    }
    *buf = buff_p;

    summary_adv_pkt_p = vtp_buffer_payload(trunk_id, buff_p); 

    summary_adv_pkt_p->version      = VTP_VERSION;
    summary_adv_pkt_p->code         = VTP_SUMMARY_ADV;
    summary_adv_pkt_p->followers    = domain_p->tx_queue_count;
    summary_adv_pkt_p->dname_length = dname_length;

    for (ix = 0; ix < dname_length; ix++)
       summary_adv_pkt_p->domain_name[ix] = domain_name[ix];

    for (; ix < VTP_MAX_DOMAIN_NAME_LENGTH; ix++)
       summary_adv_pkt_p->domain_name[ix] = 0; 

    summary_adv_pkt_p->revision   = vtp_format_4_bytes(domain_p->revision);
    summary_adv_pkt_p->updater_id = vtp_format_4_bytes(domain_p->updater);

    memcpy(summary_adv_pkt_p->update_timestamp, domain_p->timestamp, 12);
    memcpy(summary_adv_pkt_p->md5_digest, domain_p->md5_digest, 16);

    /* append TLV for pruning support */
    tlv_p = (summary_pruning_tlv_ptr) (((unsigned char *)summary_adv_pkt_p) + 
				       sizeof(summary_adv));
    if ( join_support_flag ) {
	tlv_p->tag = TAG_PRUNING_SUPPORT;
	tlv_p->length = TAG_PRUNING_SUPPORT_LEN;
	tlv_p->value = domain_p->pruning_mode;
	*data_len = sizeof(summary_adv)+SUMMARY_PRUNING_TLV_SIZ;
    }
    else {
	tlv_p->tag = 0;
	tlv_p->length = 0;
	tlv_p->value = 0;
	*data_len = sizeof(summary_adv);
    }
 
}   /*set_pruning_ver1_msg*/


/**************************************************************************
 * 
 *		Utilities for above Debugging/Engineering Functions
 * 
 **************************************************************************/

/*-------------------------------------------------------------------------
 * show_vtp_pruning_trunk_detail:
 *  dump pruning details in a trunk structure
 *------------------------------------------------------------------------*/
void
show_vtp_pruning_trunk_detail (vlan_domain_ptr domain_p, 
			       vtp_trunk_itf_ptr trunk_p,
			       unsigned short start_vlan, unsigned
			       short vlan_num) {
    vlan_info_ent_ptr	vlan_info_p;
    unsigned char	active;
    unsigned int	i, cnt = 0;
    
    printf("\n<<<<< Trunk %s in domain %s pruning info >>>>>\n",
	   trunk_p->trunk_name, domain_p->domain_name); 
    
    /* print domain wide info */
    /* pruning INeligibility */
    printf("[ Pruning Ineligible on Vlans:\t");
    memcpy(tmp_buf, trunk_p->vlan_pruning_eligibility, VLAN_BIT_STR_SIZ);
    for(i = 0; i < VLAN_BIT_STR_SIZ; i++)
      tmp_buf[i] = ~tmp_buf[i];
    vtp_printVlans(tmp_buf, (char *)NULL);
    printf(" ]\n");
    
    /* vlan local assoc */
    printf("[ Local Association on vlans:\t");
    vtp_printVlans(domain_p->vlan_local_assoc, (char *)NULL);
    printf(" ]\n");
    
    /* spt state */
    printf("Vlans in SPT forwarding state:\t");
    vtp_printVlans(trunk_p->spt_state, (char *)NULL);
    printf(crlf);
    
    /* current & previous joining bits */
    show_vtp_pruning_status_detail(trunk_p);
				   
    /* statistics */
    printf("\nCounters: tx_join=%u, rx_join=%u, rx_ver1=%u\n", 
	   trunk_p->pruning_stat.tx_join, trunk_p->pruning_stat.rx_join,
	   trunk_p->pruning_stat.rx_no_pruning_support_msg);

    /* timer & jittered value */
    active = vtp_timer_active(trunk_p->pruning_timer_p);
    printf("Pruning Timer %s, last jitter=%u, t1=%d\n", 
	   (active)? "running":"stopped", trunk_p->join_jittered_value,
	   trunk_p->t1);
    
    /* status */
#define TRUNK_STS_JOIN_BIT_CHG_1    0x01  /* any vlan joining bit change to 1 */
#define TRUNK_STS_JOIN_STATE_CHG    0x02  /* any vlan state changed */
#define TRUNK_STS_JOIN_TRIG_PERIOD  0x04  /* during triggered 1-sec period */
    printf("Status=");
    if (trunk_p->status & TRUNK_STS_JOIN_BIT_CHG_1)
      printf("JoinBitTo1 ");
    if (trunk_p->status & TRUNK_STS_JOIN_STATE_CHG)
      printf("JoinStateChg ");
    if (trunk_p->status & TRUNK_STS_JOIN_TRIG_PERIOD)
      printf("JoinTrigPeriod ");
    if (!trunk_p->status)
      printf("0");
    
    /* vlan_status[] */
    printf("\ncurrent #vlan_status_array=%d, max"
	   " #vlan_status_array=%d\n", trunk_p->vlan_status_num,
	   trunk_p->vlan_status_win); 
    
    /* if no vlan needed, just quit */
    if (!vlan_num)
      return;
    
    /* FSM related: all vlans */
    printf("\n--->FSM\n");
    printf("t1=%d, link=%s\n\n", trunk_p->t1, 
	   link_state_str[trunk_p->link_state]);

    /* printf(abbre_trunk_str);	*/
    for (vlan_info_p = domain_p->vlan_head; vlan_info_p; vlan_info_p =
	 vlan_info_p->isl_next) {
	
	if (vlan_info_p->isl_vlan_id >= start_vlan ) {
	    show_vtp_pruning_trunk_vlan_detail(trunk_p, vlan_info_p, TRUE);
	    if (++cnt >= vlan_num)
		break;
	}
	
    } /*for(vlans)*/
    
}   /*show_vtp_pruning_trunk_detail*/

/*-------------------------------------------------------------------------
 * show_vtp_pruning_vlan_detail:
 *------------------------------------------------------------------------*/
void
show_vtp_pruning_vlan_detail (vlan_domain_ptr domain_p, 
			      vlan_info_ent_ptr vlan_info_p) {
    vtp_trunk_itf_ptr	trunk_p;
    unsigned short isl_vlan_id = vlan_info_p->isl_vlan_id;
    unsigned short offset;
    unsigned char  mask;
    
    offset = vlan_bit_offset(isl_vlan_id,VTP_VLAN_BASE);
    mask = vlan_bit_mask(isl_vlan_id,VTP_VLAN_BASE);
    
    /* note: if vlan_name has 32 chars, it may print garbage */
    printf("Vlan %4d(%s,%s): ", vlan_info_p->isl_vlan_id, 
	   vlan_info_p->vlan_name, vlan_type[vlan_info_p->type]);
    
    if ( domain_p->vlan_local_assoc[offset] & mask ) {
	printf("LA  ");
    } else {
	printf("NLA ");
    }
	
    if ( domain_p->vlan_exist[offset] & mask ) {
	printf("E  ");
    } else {
	printf("NE ");	
    }
    
    /* more info about translational vlan */
    printf("tb1=%d tb2=%d", vlan_info_p->tb_vlan_1, vlan_info_p->tb_vlan_2);
    printf(crlf);
    
    /* trunk wide: spt state, current joining, prev joining */
    /* printf(abbre_trunk_str);	*/
    for (trunk_p = domain_p->trunk_head; trunk_p; trunk_p =
	 trunk_p->trunk_next) {
	show_vtp_pruning_trunk_vlan_detail(trunk_p, vlan_info_p, FALSE);
    }
    
}   /*show_vtp_pruning_vlan_detail*/ 

/*-------------------------------------------------------------------------
 * show_vtp_pruning_trunk_vlan_detail:
 *  if flag=TRUE, show prefix as: "Vlan n: "
 *  if flag=FALSE, show prefix as: "\tT <trunk name>-  "
 *------------------------------------------------------------------------*/
void
show_vtp_pruning_trunk_vlan_detail (vtp_trunk_itf_ptr trunk_p, 
				    vlan_info_ent_ptr vlan_info_p,
				    unsigned char flag) {
    unsigned short offset;
    unsigned short isl_vlan_id = vlan_info_p->isl_vlan_id;
    unsigned char  mask;
    
    if ( flag ) {
	printf("Vlan %4d(%s): ", isl_vlan_id, vlan_info_p->vlan_name);
    }
    else {
	printf("  T%s ", trunk_p->trunk_name);
    }
    
    /* print trunk name */
    offset = vlan_bit_offset(isl_vlan_id,VTP_VLAN_BASE);
    mask = vlan_bit_mask(isl_vlan_id,VTP_VLAN_BASE);
    
    /* spt state, cur join, prev join */
    if ( trunk_p->spt_state[offset] & mask )
      printf("F  ");
    else
      printf("NF ");
    
    if ( trunk_p->cur_join_bits[offset] & mask )
      printf("J  ");
    else
      printf("NJ ");
    
    /*
    if ( trunk_p->prev_join_bits[offset] & mask )
      printf("PJ  ");
    else
      printf("NPJ ");
    */
    
    /* fsm state & event & t2 */
    printf("st=%s,evt=%s,t2=%d", 
	   state_str[trunk_p->vlan_fsm_state[isl_vlan_id]],
	   event_str[trunk_p->vlan_fsm_event[isl_vlan_id]],
	   trunk_p->t2[isl_vlan_id]);
    
    /* ----- trunk wide info ----- */
    printf("[");
    if ( !flag )
      printf("t1=%d,%s,",trunk_p->t1, 
	     link_state_str[trunk_p->link_state]);
    
}   /*show_vtp_pruning_trunk_vlan_detail*/

/*-------------------------------------------------------------------------
 * show_vtp_pruning_status_detail:
 *------------------------------------------------------------------------*/
void
show_vtp_pruning_status_detail (vtp_trunk_itf_ptr trunk_p) {
    printf("Currently Joining Vlans:\t");
    vtp_printVlans(trunk_p->cur_join_bits, (char *)NULL);
    printf(crlf);
    
    /*
    printf("Previously Joining Vlans:\t");
    vtp_printVlans(trunk_p->prev_join_bits, (char *)NULL);
    printf(crlf);
    */
    
    /* joined/pruned status */
    vtp_pruning_build_joined_bits(trunk_p, tmp_buf);
    printf("Vlans in Joined State:\t\t");
    vtp_printVlans(tmp_buf, (char *)NULL);
    printf(crlf);
    
}   /*show_vtp_pruning_status_detail*/    
    
/*-------------------------------------------------------------------------
 * vtp_get_next_trunk:
 *  a quick way to get the next trunk w/o searching whole list
 *------------------------------------------------------------------------*/
vtp_trunk_itf_ptr vtp_get_next_trunk (unsigned char dname_length,
				      unsigned char *domain_name,  
				      vtp_trunk_itf_ptr cur_trunk_p, 
				      void **next_trunk_id) {
    vtp_trunk_itf_ptr	trunk_p;
    vlan_domain_ptr	domain_p;

    trunk_p = NULL;
    domain_p = vtp_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	VTP_PRUNING_LOG_WARNING(NODOMAIN, "%sCan't find domain %s(%d)"
				" in vtp_get_next_trunk", domain_name,
				dname_length);  
	return NULL;
    }
    
    if ( cur_trunk_p == NULL ) { /* get first trunk */
	trunk_p = domain_p->trunk_head;
    } else {			    /* find next based on current */
	trunk_p = cur_trunk_p->trunk_next;
    }    
	
    if (trunk_p) {
	*next_trunk_id = trunk_p->trunk_id;
    } else {			    /* no more trunk in this domain */
	*next_trunk_id = NULL;
    }

    return (trunk_p);
}   /*vtp_get_next_trunk*/


/**************************************************************************
 * 
 *	Debugging/Engineering Functions to update internal structures
 * 
 **************************************************************************/

/*-------------------------------------------------------------------------
 * set_pruning_eligible:
 *------------------------------------------------------------------------*/
void 
set_pruning_eligible (unsigned char dname_length, 
		      unsigned char *domain_name, void* trunk_id,
		      unsigned short isl_vlan_id, unsigned char flag) {
    vlan_domain_ptr	domain_p;
    vtp_trunk_itf_ptr  trunk_p;
    
    domain_p = vtp_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	printf("Domain %s (%d) does not exist!\n", domain_name, dname_length);
	return;
    }
    
    trunk_p = vtp_find_trunk_link_in_domain(domain_p, trunk_id);
    if (!trunk_p) {
	printf("Trunk does not exist!\n");
	return;
    }

    if (flag)
      trunk_p->vlan_pruning_eligibility[vlan_bit_offset(isl_vlan_id,VTP_VLAN_BASE)] |= 
	  vlan_bit_mask(isl_vlan_id,VTP_VLAN_BASE);
    else
      trunk_p->vlan_pruning_eligibility[vlan_bit_offset(isl_vlan_id,VTP_VLAN_BASE)] &= 
	  ~vlan_bit_mask(isl_vlan_id,VTP_VLAN_BASE);
    
}   /*set_pruning_eligible*/

/*-------------------------------------------------------------------------
 * set_pruning_local_assoc:
 *------------------------------------------------------------------------*/
void 
set_pruning_local_assoc (unsigned char dname_length, 
			 unsigned char *domain_name, 
			 unsigned short isl_vlan_id, 
			 unsigned char flag) {
    vlan_domain_ptr	domain_p;
    
    domain_p = vtp_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	printf("Domain %s (%d) does not exist!\n", domain_name, dname_length);
	return;
    }
    
    if (flag)
      domain_p->vlan_local_assoc[vlan_bit_offset(isl_vlan_id,VTP_VLAN_BASE)] |= 
	  vlan_bit_mask(isl_vlan_id,VTP_VLAN_BASE);
    else
      domain_p->vlan_local_assoc[vlan_bit_offset(isl_vlan_id,VTP_VLAN_BASE)] &= 
	  ~vlan_bit_mask(isl_vlan_id,VTP_VLAN_BASE);
    
}   /*set_pruning_local_assoc*/

/*-------------------------------------------------------------------------
 * set_pruning_vlan_exist:
 *------------------------------------------------------------------------*/
void 
set_pruning_vlan_exist (unsigned char dname_length, 
			unsigned char *domain_name, 
			unsigned short isl_vlan_id, 
			unsigned char flag) {
    vlan_domain_ptr	domain_p;
    
    domain_p = vtp_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	printf("Domain %s (%d) does not exist!\n", domain_name, dname_length);
	return;
    }
    
    if (flag)
      domain_p->vlan_exist[vlan_bit_offset(isl_vlan_id,VTP_VLAN_BASE)]
	  |= vlan_bit_mask(isl_vlan_id,VTP_VLAN_BASE); 
    else
      domain_p->vlan_exist[vlan_bit_offset(isl_vlan_id,VTP_VLAN_BASE)]
	  &= ~vlan_bit_mask(isl_vlan_id,VTP_VLAN_BASE);
    
}   /*set_pruning_vlan_exist*/

/*-------------------------------------------------------------------------
 * set_pruning_min_max_vlan:
 *------------------------------------------------------------------------*/
void 
set_pruning_min_max_vlan (unsigned char dname_length, 
			  unsigned char *domain_name, 
			  unsigned short vlan_no, 
			  unsigned char min_flag) {
    vlan_domain_ptr	domain_p;
    
    domain_p = vtp_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	printf("Domain %s (%d) does not exist!\n", domain_name, dname_length);
	return;
    }
    
    if (min_flag)
      domain_p->min_vlan_no = vlan_no;
    else
      domain_p->max_vlan_no = vlan_no;
    
}   /*set_pruning_min_max_vlan*/

/*-------------------------------------------------------------------------
 * set_pruning_operation_status:
 *------------------------------------------------------------------------*/
void 
set_pruning_operation_status (unsigned char dname_length, 
			      unsigned char *domain_name, 
			      unsigned char status) {
    vlan_domain_ptr	domain_p;
    
    domain_p = vtp_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	printf("Domain %s (%d) does not exist!\n", domain_name, dname_length);
	return;
    }
    
    domain_p->status = status;
    
}   /*set_pruning_operation_status*/

/*-------------------------------------------------------------------------
 * set_pruning_mode: 
 *------------------------------------------------------------------------*/
void 
set_pruning_mode (unsigned char dname_length, 
		  unsigned char *domain_name, unsigned mode) { 
}   /*set_pruning_mode*/

/*-------------------------------------------------------------------------
 * set_pruning_joining_bit:
 *------------------------------------------------------------------------*/
void 
set_pruning_joining_bit (unsigned char dname_length, 
			 unsigned char *domain_name, void *trunk_id,
			 unsigned short isl_vlan_id, 
			 unsigned char flag) {
    vlan_domain_ptr	domain_p;
    vtp_trunk_itf_ptr	trunk_p;
    
    domain_p = vtp_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	printf("Domain %s (%d) does not exist!\n", domain_name, dname_length);
	return;
    }
    
    /* find trunk link */
    trunk_p = vtp_find_trunk_link_in_domain(domain_p, trunk_id);
    if ( !trunk_p ) {
	printf("Trunk (%d/%d) does not exist in domain %s!\n", 
	       ((*(unsigned short *)trunk_id)>>6)+1,
	       ((*(unsigned short *)trunk_id)&0x3f)+1, domain_p->domain_name);
	return;
    }
    
    if (flag)
      trunk_p->cur_join_bits[vlan_bit_offset(isl_vlan_id,VTP_VLAN_BASE)] |= 
	  vlan_bit_mask(isl_vlan_id,VTP_VLAN_BASE);
    else
      trunk_p->cur_join_bits[vlan_bit_offset(isl_vlan_id,VTP_VLAN_BASE)] &= 
	  ~vlan_bit_mask(isl_vlan_id,VTP_VLAN_BASE);
    
}   /*set_pruning_joining_bit*/

/*-------------------------------------------------------------------------
 * set_pruning_spt_state:
 *------------------------------------------------------------------------*/
void 
set_pruning_spt_state (unsigned char dname_length, 
		       unsigned char *domain_name, void *trunk_id,
		       unsigned short isl_vlan_id, unsigned char flag) {
    vlan_domain_ptr	domain_p;
    vtp_trunk_itf_ptr	trunk_p;
    
    domain_p = vtp_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	printf("Domain %s (%d) does not exist!\n", domain_name, dname_length);
	return;
    }
    
    /* find trunk link */
    trunk_p = vtp_find_trunk_link_in_domain(domain_p, trunk_id);
    if ( !trunk_p ) {
	printf("Trunk (%d/%d) does not exist in domain %s!\n", 
	       ((*(unsigned short *)trunk_id)>>6)+1,
	       ((*(unsigned short *)trunk_id)&0x3f)+1, domain_p->domain_name);
	return;
    }
    
    if (flag)
      trunk_p->spt_state[vlan_bit_offset(isl_vlan_id,VTP_VLAN_BASE)]
	  |= vlan_bit_mask(isl_vlan_id,VTP_VLAN_BASE);
    else
      trunk_p->spt_state[vlan_bit_offset(isl_vlan_id,VTP_VLAN_BASE)]
	  &= ~vlan_bit_mask(isl_vlan_id,VTP_VLAN_BASE);
    
}   /*set_pruning_spt_state*/

/*-------------------------------------------------------------------------
 * set_pruning_fsm_state:
 *------------------------------------------------------------------------*/
void 
set_pruning_fsm_state (unsigned char dname_length, 
		       unsigned char *domain_name, void *trunk_id,
		       unsigned short isl_vlan_id, 
		       unsigned char state) {
    vlan_domain_ptr	domain_p;
    vtp_trunk_itf_ptr	trunk_p;
    
    domain_p = vtp_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	printf("Domain %s (%d) does not exist!\n", domain_name, dname_length);
	return;
    }
    
    /* find trunk link */
    trunk_p = vtp_find_trunk_link_in_domain(domain_p, trunk_id);
    if ( !trunk_p ) {
	printf("Trunk (%d/%d) does not exist in domain %s!\n", 
	       ((*(unsigned short *)trunk_id)>>6)+1,
	       ((*(unsigned short *)trunk_id)&0x3f)+1, domain_p->domain_name);
	return;
    }
    
    trunk_p->vlan_fsm_state[isl_vlan_id] = state;
    
}   /*set_pruning_fsm_state*/

/*-------------------------------------------------------------------------
 * set_pruning_t2:
 *------------------------------------------------------------------------*/
void 
set_pruning_t2 (unsigned char dname_length, 
		unsigned char *domain_name, void *trunk_id,
		unsigned short isl_vlan_id, unsigned char t2_value) {
    vlan_domain_ptr	domain_p;
    vtp_trunk_itf_ptr	trunk_p;
    
    domain_p = vtp_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	printf("Domain %s (%d) does not exist!\n", domain_name, dname_length);
	return;
    }
    
    /* find trunk link */
    trunk_p = vtp_find_trunk_link_in_domain(domain_p, trunk_id);
    if ( !trunk_p ) {
	printf("Trunk (%d/%d) does not exist in domain %s!\n", 
	       ((*(unsigned short *)trunk_id)>>6)+1,
	       ((*(unsigned short *)trunk_id)&0x3f)+1, domain_p->domain_name);
	return;
    }
    
    trunk_p->t2[isl_vlan_id] = t2_value;
    
}   /*set_pruning_t2*/

/*-------------------------------------------------------------------------
 * set_pruning_link_state:
 *------------------------------------------------------------------------*/
void 
set_pruning_link_state (unsigned char dname_length, 
			unsigned char *domain_name, void *trunk_id,
			unsigned char flag) {
    vlan_domain_ptr	domain_p;
    vtp_trunk_itf_ptr	trunk_p;
    
    domain_p = vtp_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	printf("Domain %s (%d) does not exist!\n", domain_name, dname_length);
	return;
    }
    
    /* find trunk link */
    trunk_p = vtp_find_trunk_link_in_domain(domain_p, trunk_id);
    if ( !trunk_p ) {
	printf("Trunk (%d/%d) does not exist in domain %s!\n", 
	       ((*(unsigned short *)trunk_id)>>6)+1,
	       ((*(unsigned short *)trunk_id)&0x3f)+1, domain_p->domain_name);
	return;
    }
    
    if (flag)
      trunk_p->link_state = TRUNK_LINK_UP;
    else
      trunk_p->link_state = TRUNK_LINK_DOWN;
    
}   /*set_pruning_link_state*/

/*-------------------------------------------------------------------------
 * set_pruning_t1:
 *------------------------------------------------------------------------*/
void 
set_pruning_t1 (unsigned char dname_length, 
		unsigned char *domain_name, void *trunk_id,
		unsigned short t1_value) {
    vlan_domain_ptr	domain_p;
    vtp_trunk_itf_ptr	trunk_p;
    
    domain_p = vtp_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	printf("Domain %s (%d) does not exist!\n", domain_name, dname_length);
	return;
    }
    
    /* find trunk link */
    trunk_p = vtp_find_trunk_link_in_domain(domain_p, trunk_id);
    if ( !trunk_p ) {
	printf("Trunk (%d/%d) does not exist in domain %s!\n", 
	       ((*(unsigned short *)trunk_id)>>6)+1,
	       ((*(unsigned short *)trunk_id)&0x3f)+1, domain_p->domain_name);
	return;
    }
    
    trunk_p->t1 = t1_value;
    
}   /*set_pruning_t1*/

/*-------------------------------------------------------------------------
 * set_pruning_status_array_num:
 *------------------------------------------------------------------------*/
void 
set_pruning_status_array_num (unsigned char dname_length, 
			      unsigned char *domain_name, void *trunk_id,
			      unsigned short num) {
    vlan_domain_ptr	domain_p;
    vtp_trunk_itf_ptr	trunk_p;
    
    domain_p = vtp_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	printf("Domain %s (%d) does not exist!\n", domain_name, dname_length);
	return;
    }
    
    /* find trunk link */
    trunk_p = vtp_find_trunk_link_in_domain(domain_p, trunk_id);
    if ( !trunk_p ) {
	printf("Trunk (%d/%d) does not exist in domain %s!\n", 
	       ((*(unsigned short *)trunk_id)>>6)+1,
	       ((*(unsigned short *)trunk_id)&0x3f)+1, domain_p->domain_name);
	return;
    }
    
    trunk_p->vlan_status_num = num;
    
}   /*set_pruning_status_array_num*/

/*-------------------------------------------------------------------------
 * set_pruning_busy: change domain_p->busy
 *------------------------------------------------------------------------*/
void
set_pruning_busy (unsigned char dname_length, 
		  unsigned char *domain_name, unsigned int flag) {
    vlan_domain_ptr	domain_p;
    
    domain_p = vtp_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	printf("Domain %s (%d) does not exist!\n", domain_name, dname_length);
	return;
    }
    
    domain_p->busy = flag;
    
}   /*set_pruning_busy*/

/*-------------------------------------------------------------------------
 * set_pruning_T3:
 *------------------------------------------------------------------------*/
void 
set_pruning_T3 (unsigned char T3)
{
}   /*set_pruning_t1*/

