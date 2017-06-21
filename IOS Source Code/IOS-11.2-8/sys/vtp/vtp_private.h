/* $Id: vtp_private.h,v 1.1 1996/08/28 23:50:22 cyoung Exp $
 * $Source: /release/112/cvs/Xsys/vtp/vtp_private.h,v $
 *------------------------------------------------------------------
 * One line description here.
 *
 * August 1996, Chris Young
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vtp_private.h,v $
 * Revision 1.1  1996/08/28  23:50:22  cyoung
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __VTP_PRIVATE_H__
#define __VTP_PRIVATE_H__

/* 
 * Types Definitions.
 */

#define VTP_SOURCE_ADVERT 1
#define VTP_SOURCE_EXTERN 2

/* ---- Name comparison macros ---- */
#define NAME_EQUAL           0
#define NAME_LESS           -1
#define NAME_GREATER         1


#define MAX_VTP_TX_QUEUE_SIZE        64 
#define MAX_VTP_LOCK_RETRY           100
#define VTP_MAC_ADDRESS_SIZE         6

/* ----- local constants ----- */
#define VTP_VLAN_BASE		0
#define MAX_NUM_VLANS		1024
#define VLAN_BYTE_STR_SIZ	MAX_NUM_VLANS       /* MAX_NUM_VLANS bytes */
#define VLAN_BIT_STR_SIZ	(MAX_NUM_VLANS / 8) /* MAX_NUM_VLANS bits */
#define FIRST_VLAN_MOD		0   /* modulo 8 of first vlan no in Join */
#define LAST_VLAN_MOD		7   /* module 8 of last vlan no in
				       Join */
/* ---- buffer queue element ---- */
typedef struct {
    unsigned short length;
    void *buff_p;
} buff_chain, *buff_chain_ptr;

/* ---- VLAN information database entry structure ---- */
typedef struct vlan_info_ent_ {
    struct vlan_info_ent_* isl_next;          /* ascending ordered
					       * list on ISL VLAN ID
					       */ 
    unsigned long said;                       /* VLAN Id */
    unsigned short isl_vlan_id;               /* ISL VLAN ID */
    unsigned char type;                       /* VLAN type */
#define VLAN_ETHER    0x01
#define VLAN_FDDI     0x02
#define VLAN_TR       0x03
#define VLAN_FDDI_NET 0x04
#define VLAN_TR_NET   0x05
    unsigned char status;                      /* status of the VLAN */
#define VLAN_OPERATIONAL              0x00     /* same format as in
						* VTP packet */ 
#define VLAN_SUSPENDED     	      0x01     /* same format as in
						*  VTP packet */
#define VLAN_MTU_TOO_BIG_FOR_DEVICE   0x02     /* TBD */
#define VLAN_MTU_TOO_BIG_FOR_TRUNK    0x03     /* TBD */
    unsigned short mtu;	                       /* MTU */
    unsigned char name_length;                 /* VLAN name length */
    unsigned char flags;                       /* misc. flags */
#define OLD_VLAN      0x01
#define NEW_VLAN      0x02
#define MODIFIED_VLAN 0x04
    unsigned char vlan_name[VLAN_MAX_NAME_LENGTH + 1];    

/* the following not contained in VTP packet yet */
    unsigned short ring_number;               /* for FDDI/TR type only */
    unsigned short bridge_number;             /* for FDDI-NET/TR_NET
					       * type only */ 
    unsigned short parent_vlan;               /* for FDDI/TR type only */
#define ENT_STP_SRT         1
#define ENT_STP_SRB         2
    unsigned short stp_type;      /* for FDDI-NET/TR_NET type only */
    vtp_boolean parent_valid;        /* for FDDI only */
    unsigned short tb_vlan_1;
    unsigned short tb_vlan_2;
} vlan_info_ent, *vlan_info_ent_ptr;

/* ---- VTP packet structures ---- */

/* some constants */
#define VTP_VERSION           0x01
#define VTP_STORAGE_UNKNOWN      -1  
#define VTP_SUMMARY_ADV       0x01
#define VTP_SUBSET_ADV        0x02
#define VTP_ADV_RQST          0x03

/* VLAN packet information structure */
#define VTP_MAX_VLAN_INFO_SIZE         60
#define VTP_MIN_VLAN_INFO_SIZE         16
typedef struct {
    unsigned char info_length;    /* length in bytes */
    unsigned char status;         /* status */
#define PKT_OPERATIONAL    0x00
#define PKT_SUSPENDED      0x01
    unsigned char type;           /* type */
    unsigned char name_length;    /* VLAN name length in bytes */
    unsigned short isl_vlan_id;   /* VLAN ID */
    unsigned short mtu;           /* MTU size in bytes */
    unsigned long said;           /* IEEE802.10 index */
    unsigned char vlan_name[0];   /* (padded with zeros to multiple of
				   * 4 bytes) */ 
    /* followed by zero or more of vlan_tlv */
} vlan_info, *vlan_info_ptr;

/* VLAN TLV field definition */
typedef struct {
    unsigned char tag;	             /* tag field */
#define TAG_SR_RING         0x01
#define TAG_SR_BRIDGE       0x02
#define TAG_STP_TYPE        0x03
#define TAG_PARENT_VLAN     0x04
#define TAG_TB_VLAN         0x05
    unsigned char length;           /* in number of 4-byte words */
    unsigned short value;           /* the value */
} vlan_tlv, *vlan_tlv_ptr;

/* TLV for VTP_PRUNING: appended in summary adv */
typedef struct {
    unsigned char       tag;        /* 1 */
    unsigned char       length;     /* 1 */
    unsigned char       value;      /* pruning mode */
}   summary_pruning_tlv, *summary_pruning_tlv_ptr;

#define SUMMARY_PRUNING_TLV_SIZ sizeof(summary_pruning_tlv) /* 3 */
#define TAG_PRUNING_SUPPORT     0x01 /* for version 1+ (pruning
				      * supported) use 
				      */
#define TAG_PRUNING_SUPPORT_LEN 0x01 /* for version 1+ (pruning
				      * supported) use 
				      */
/* followed by value = VTP_PRUNING_EN(1)/DISABLE(0) */

/* VTP packet subset advertisment structure */
typedef struct {
    unsigned char version;       /* version number */
    unsigned char code;          /* the type */
    unsigned char dont_care;     /* sequence number */
    unsigned char dname_length;  /* the domain name length in bytes */
    unsigned char domain_name[VTP_MAX_DOMAIN_NAME_LENGTH];
                                 /* the domain name, padded by zeros */
} adv_header, *adv_header_ptr;

/* VTP packet subset advertisment structure */
typedef struct {
    unsigned char version;                   /* version number */
    unsigned char code;                      /* the type */
    unsigned char sequence;                  /* sequence number */
    unsigned char dname_length;	             /* the domain name length
					      * in bytes */ 
    unsigned char domain_name[VTP_MAX_DOMAIN_NAME_LENGTH];
					     /* the domain name,
					      * padded by zeros */ 

    unsigned long revision;	             /* the configuration
					      * revision number */ 
    /* followed by one or more vlan_info */
} subset_adv, *subset_adv_ptr;

/* VTP packet advertisement request message structure */
typedef struct {
    unsigned char version;              /* version number */
    unsigned char code;                 /* the type */
    unsigned char reserved;             /* reserved */
    unsigned char dname_length;         /* the domain name length in
					 * bytes */
    /* the domain name, padded by zeros */
    unsigned char domain_name[VTP_MAX_DOMAIN_NAME_LENGTH];  
    unsigned short start_value;	        /* start value of VLAN Id requested */
} adv_request, *adv_request_ptr;

/* VTP packet summary advertisement message structure */
#define VTP_MAX_CONFIG_INFO_SIZE       36
typedef struct {
    unsigned char version;              /* version number */
    unsigned char code;	                /* the type */
    unsigned char followers;            /* the number of subset_adv followed */
    unsigned char dname_length;	        /* the domain name length in
					 * bytes */
    /* the domain name, padded by zeros */
    unsigned char domain_name[VTP_MAX_DOMAIN_NAME_LENGTH]; 
    unsigned long revision;             /* the configuration revision number */
    unsigned long updater_id;           /* id (IP address) of updating
					 * device */ 
    unsigned char update_timestamp[VTP_TIMESTAMP_SIZE];	/* time stamp
							 * as update
							 */ 
    unsigned char md5_digest[VTP_MD5_DIGEST_SIZE];	/* check sum */
} summary_adv, *summary_adv_ptr;

#define VTP_ONE_SECOND               1000
#define VTP_FLOOD_INTERVAL           300     /* in seconds */
#define MAX_VTP_UPDATE_PENDING_TIME  30000   /* in ms, 30 second */

/* The trunk advt rxmt timer is jittered to including a random
 * number from 0 - 3 seconds.
 */
#define VTP_FLOOD_JITTERED(T, R) ((T) + (R) & 3)

typedef struct vtp_stats_ {
   unsigned long rx_summary_adv_count;        /* rx summary advt count */
   unsigned long rx_subset_adv_count;         /* rx subset advt count */
   unsigned long rx_request_adv_count;        /* rx request advt count */
   unsigned long tx_summary_adv_count;        /* tx summary advt count */
   unsigned long tx_subset_adv_count;         /* tx subset advt count */
   unsigned long tx_request_adv_count;        /* tx request advt count */
   unsigned long revision_error_count;        /* revision number err count */
   unsigned long digest_error_count;          /* MD5 digest err count */
} vtp_stats; 

struct vlan_domain_;

#define DEF_VLAN_STATUS_SIZ         128

/* ---- VTP trunk link interface structure ---- */
typedef struct vtp_trunk_itf_ {
    struct vtp_trunk_itf_* trunk_next;/* global trunk list */  
    struct vtp_trunk_itf_* next_trunk_in_domain; 
    struct vlan_domain_* domain_p;         /* poiner to domain structure */
    void *trunk_id;                   /* reference to trunk link */
    unsigned short mtu;               /* MTU on the trunk link */
#define DEFAULT_TRUNK_MTU  1500
    unsigned char vlans_enabled[VLANS_BITMASK_SIZE];
    unsigned short native_vlan;
    unsigned char encaps_type;
    void *advt_timer_p;              /* pointer to advt retransmit timer */
    unsigned long flood_timestamp;   /* timestamp at last flooding - in ms */
    unsigned long jittered_value;    /* random number in range 0-1000 ms */
    void *rqst_timer_p;              /* pointer to advt request timer */
    unsigned short rqst_start_value; /* first ISL VLAN Id to request */
#define SEND_ALL       0
#define SEND_NONE      0xffff
    unsigned short advt_start_value; /* first ISL VLAN Id to advt */
#define RQST_ALL       0
#define RQST_NONE      0xffff
    unsigned char rqst_retry_count;  /* rqst retry counter */
#define VTP_MAX_RQST_RETRY_COUNT  10
    unsigned char flags;             /* reserved */

    /* null terminated ascii string */
    unsigned char   trunk_name[VTP_MAX_TRUNK_NAME_LENGTH + 1]; 

    /*
     * "rx_join_flag added to distinguish exact reason causing timeout
     * two reasons cause timeout:
     *  (1) not rx any Join msg since tiemout started
     *  (2) rx at least one Join(0) msg
     * to use following bit (one bit per vlan) we can do it, algorithm is:
     *  -whenever rx Join(1), "rx_join_flag" is on, and T2 restarted
     *  -whenever rx Join(0), "rx_join_flag" is off
     *  -when timeout occurs, if rx_join_flag==1 => no Join ever rx
     *			      if rx_join_flag==0 => Join ever rx
     *  -whenever going pruned, rx_join_flag set to 0
     *  -note if only vtp version 1 msg or Join(0) shown,
     *   rx_join_flag==0 always 
     *  -note the scheme appllied to "Normal Joined (S3)" only, i.e.,
     *   there must be Join(1) rx to set rx_join_flag to 1
     */
    unsigned char   rx_join_flag[ VLAN_BIT_STR_SIZ ]; /*rx join or not */
    unsigned long   tmo_no_rx_join; /* # timeouts not rx any Join after S3 */
    unsigned long   tmo_cnt;   /* # timeouts not rx Join1/V1 after S3 */

    unsigned char   cur_join_bits[ VLAN_BIT_STR_SIZ ];
    unsigned char   vlan_pruning_eligibility[ VLAN_BIT_STR_SIZ ];
    unsigned char   spt_state[ VLAN_BIT_STR_SIZ ];
    unsigned char   vlan_fsm_state[ VLAN_BYTE_STR_SIZ ];
    unsigned char   vlan_fsm_event[ VLAN_BYTE_STR_SIZ ];
    unsigned char   t2[ VLAN_BYTE_STR_SIZ ];    /* ticks */
   
    unsigned short  t1;             /* ticks */
    unsigned char   link_state;     /* link up or down */
    unsigned char   status;         /* passing status btw. routines */
    vtp_trunk_stat  pruning_stat;
    void            *pruning_timer_p;
    unsigned long   join_jittered_value;
   
    /*
     * the following fields are used for vlan (pruning/joining) status
     * changes caused by FSM operations, all intermediate results are stored
     * until all events are handled. The array size is fixed.
     * If the array used up due to too many
     * vlan state changes, the current state changes will be notified to
     * dependent code before proceeding FSM for other vlans.
     * In most cases, array size 128 should be large enough.
     */
    vtp_vlan_status vlan_status[ DEF_VLAN_STATUS_SIZ ];
                                    /* vlan pruned/joined status array */
                                    /* used by pruning FSM to notify dep */
                                    /* the vlans changing pruning status */
    unsigned short  vlan_status_num; /* passing current index btw. routines */
                                    /* also indicate # vlans changing status */
    unsigned short  vlan_status_win;/* reserved */

} vtp_trunk_itf, *vtp_trunk_itf_ptr;

/* ---- VTP management domain structure ---- */

typedef struct vlan_domain_ {
    /* VLAN domain info and chain */
    struct vlan_domain_* domain_next;             /* list of domain */
    unsigned long index;                     /* domain index */
    unsigned char dname_length;              /* domain name length */
    unsigned char mode;                      /* mode of the VTP */
    unsigned char domain_name[VTP_MAX_DOMAIN_NAME_LENGTH + 1]; 

    /* trunk link list */
    vtp_trunk_itf_ptr trunk_head;            /* trunk list */
      
    /* VTP operating parameters */
    unsigned long revision;                  /* revision number */
    unsigned char md5_digest[VTP_MD5_DIGEST_SIZE];
    unsigned long updater;                   /* IP addr of VTP server
					      * responsible for change*/
    unsigned long my_update_id;             /* IP addr of this VTP
					     * entity or zero */
    unsigned char timestamp[VTP_TIMESTAMP_SIZE];
    unsigned char password_length;          
    unsigned char password[VTP_MAX_PASSWD_LENGTH + 1];
    unsigned char secret_value[VTP_MD5_DIGEST_SIZE];
    unsigned long advt_interval;
    char          tftp_file_name[VTP_MAX_TFTP_FILENAME_LENGTH + 1];
    unsigned long tftp_server;

    /* VTP updating parameters */
    unsigned long next_revision;             /* revision number */
    unsigned char next_md5_digest[VTP_MD5_DIGEST_SIZE];
    unsigned long next_updater_id;           /* config server's IP addr */
    unsigned char next_update_timestamp[VTP_TIMESTAMP_SIZE]; 
    unsigned long next_num_adv_subset_count; /* number of subset advt */
    vtp_boolean vtp_updating;		     /* TRUE if in updating */
    unsigned char flags;                     /* misc flags */
#define UPDATE_RXMT_REQUESTED       0x01      
    unsigned char next_adv_src_mac[6];	     /* sender's MAC address */
    unsigned short last_value_learned;       /* last ISL VLAN Id just
					      * learned */ 
    void *update_pending_timer;              /* update pending timer */
    void *update_trunk_p;                    /* link where advt received now */
    void *scratch;      		     /* mass memory for
					      * received advt */ 
    unsigned long next_seq_expected;         /* next seq. to receive */
    vlan_info_ptr first_vlan_p;              /* point to the first */
    vlan_info_ptr last_vlan_p;               /* point to the last */
    unsigned long update_vlan_count;         /* number of VLAN updated */
    unsigned long update_vlan_length;        /* length of VLAN info */

    /* VLAN info database */
    vlan_info_ent_ptr vlan_head;	     /* sorted on ISL VLAN Id
					      */
    vlan_info_ent_ptr vlan_backup_head;      /* the old database
					      * in case the new one is
					      * inconsistent */

    /* trasnmit packet queues */
    unsigned long tx_queue_count;            /* packet count in tx queue */ 
    vlan_info_ent_ptr tx_next_vlan_p;        /* pointer to the next VLAN */
    buff_chain tx_buff_queue[MAX_VTP_TX_QUEUE_SIZE]; /* tx queue */

    unsigned long vlan_count;	             /* VLAN count */
    vtp_stats acc;                           /* accumulated stats - snmp */
    vtp_stats mirror;                        /* mirror stats - cli */ 
    vtp_boolean busy;                            /* TRUE for busy */
    /* TRUE if transitioning from null 2 valid domain */
    vtp_boolean transitioning;  

   unsigned char   vlan_local_assoc[ VLAN_BIT_STR_SIZ ];

   /* for quick lookup */
   unsigned char   vlan_exist[ VLAN_BIT_STR_SIZ ]; 

   /* first vlan no(aligned) in Join,1-based*/
   unsigned short  min_vlan_no;    

   /* last vlan no(aligned) in Join, 1-based*/
   unsigned short  max_vlan_no;    

   /* pass op status btw routines */
   unsigned char   status;         

   /* for pruning enable/disable use */
   unsigned char   pruning_mode;   

   /* temp store tlv verification result */
   vtp_boolean         tlv_ok;         

   /* temp storage of pruning mode */
   unsigned char   tmp_pruning_mode; 

   /* used together w/ next_revision... */
   unsigned char   next_pruning_mode; 
} vlan_domain, *vlan_domain_ptr;

/* for VTP_PRUNING: vlan_domain: status */
/*#define DOMAIN_STS_QUIT_NO_DOMAIN     0x01*/
#define DOMAIN_STS_ADD_DOMAIN           0x80
#define DOMAIN_STS_ADD_VLAN             0x40
#define DOMAIN_STS_ADD_TRUNK            0x20

/* for VTP_PRUNING: vlan_domain: status: test if during vtp operations */
#define vtp_pruning_not_complete(domain_p) ( (domain_p)->status & \
       (DOMAIN_STS_ADD_DOMAIN | DOMAIN_STS_ADD_VLAN | DOMAIN_STS_ADD_TRUNK) )

/* for VTP_PRUNING: get tlv:pruning_mode */
#define set_vtp_tlv_pruning_mode(domain_p,m) \
                { (domain_p)->tmp_pruning_mode = m; }
#define clr_vtp_tlv_pruning_mode(domain_p) \
                { (domain_p)->tmp_pruning_mode = 0; }
#define get_vtp_tlv_pruning_mode(domain_p) ((domain_p)->tmp_pruning_mode)

#define set_vtp_tlv_pruning_ok(domain_p) { (domain_p)->tlv_ok = TRUE; }
#define clr_vtp_tlv_pruning_ok(domain_p) { (domain_p)->tlv_ok = 0; }
#define is_vtp_tlv_pruning_ok(domain_p)  ((domain_p)->tlv_ok)

/* ---- VTP trunk link interface structure ---- */

/* for VTP_PRUNING: vtp_trunk_itf: status */
#define TRUNK_STS_JOIN_BIT_CHG_1    0x01  /* any vlan joining bit change to 1 */
#define TRUNK_STS_JOIN_STATE_CHG    0x02  /* any vlan state changed */
#define TRUNK_STS_JOIN_TRIG_PERIOD  0x04  /* during triggered 1-sec period */

/* for VTP_PRUNING: vtp_trunk_itf: vlan_status[] */
#define DEF_VLAN_STATUS_SIZ         128

/* ---- revision number comparison ---- */
#define VTP_BIG_REV          (unsigned long)0x80000000
extern vtp_boolean vtp_rev_less(unsigned long, unsigned long);
#define VTP_REV_EQUAL(A, B) (A == B)

#define ADJUST_LENGTH_LONG(x) (((x)&3) != 0 ? (x)+4-((x)&3) : (x))

#define VTP_TIMER_JITTERED_PERCENT         10
#define VTP_TIMER_MAX_RANDOM_MS            1000  
#define VTP_REQUEST_INTERVAL               5000

extern short vtp_max_vlan_storage; 
extern unsigned long   vtp_id;
extern vtp_boolean no_domain_state;
extern vtp_boolean vtp_is_initialized;
extern vlan_domain_ptr   vlan_domain_head;
extern vtp_trunk_itf_ptr vtp_trunk_head;
extern unsigned long   vtp_domain_index;
extern unsigned char   vtp_version;

/* extern function declarations */

extern void 
vtp_add_default_vlans(vlan_domain_ptr d_ptr);

extern void vtp_sort_vlan_entry(vlan_domain_ptr, vlan_info_ent_ptr);

extern vlan_info_ent_ptr vtp_insert_info_entry(vlan_domain_ptr,
					       vtp_vlan_info*); 

extern vlan_info_ent_ptr 
vtp_search_info_entry_by_vlan(vlan_domain_ptr, 
			      unsigned short vlan_id); 

extern vlan_info_ent_ptr 
vtp_search_info_entry_by_name(vlan_domain_ptr, 
			      unsigned char name_length, 
			      unsigned char* name); 

extern vlan_info_ent_ptr 
vtp_search_info_entry_by_said(vlan_domain_ptr, unsigned long said);

extern void vtp_flush_database(vlan_domain_ptr);

extern void vtp_flush_update_adv_info(vlan_domain_ptr);

extern void vtp_flush_tx_subset_adv_queue(vlan_domain_ptr);

extern void 
vtp_receive_summary_adv_packet(void *, void *, unsigned long, 
			       unsigned char *); 
extern void vtp_receive_subset_adv_packet(void *, void *, unsigned long, 
					  unsigned char *);
extern void 
vtp_receive_adv_request_packet(void *, void *, unsigned long, 
			       unsigned char *); 

extern vtp_boolean vtp_validate_subset_adv(vtp_trunk_itf_ptr, void *,
				       unsigned long); 

extern void vtp_process_subset_adv(vlan_domain_ptr,
				   vtp_trunk_itf_ptr);

extern void 
vtp_allocate_update_memory(vtp_trunk_itf_ptr, unsigned char);

extern void 
vtp_record_subset_adv(vtp_trunk_itf_ptr, void *, unsigned long);

extern void vtp_mark_database_old(vlan_domain_ptr);

extern unsigned char vtp_delete_old_vlans(vlan_domain_ptr);

extern void vtp_transmit_adv_request_packet(vtp_trunk_itf_ptr,
					    unsigned short); 

extern void vtp_do_trunk_flooding(vtp_trunk_itf_ptr, unsigned short);

extern void vtp_transmit_summary_adv_packet(vtp_trunk_itf_ptr,
					    unsigned char);

extern void vtp_generate_subset_adv_packet(vlan_domain_ptr,
					   vtp_trunk_itf_ptr);

extern void vtp_transmit_subset_adv_packet(vtp_trunk_itf_ptr); 

extern void vtp_trunk_link_rqst_timeout(vtp_trunk_itf_ptr);

extern vlan_domain_ptr vtp_search_domain(unsigned char, 
					 unsigned char*); 

extern unsigned long vtp_get_index(void); 

extern vtp_trunk_itf_ptr vtp_find_trunk_link(void*);

extern 
vtp_trunk_itf_ptr vtp_find_trunk_link_in_domain(vlan_domain_ptr,
						void*);

extern void vtp_insert_domain_trunk(vlan_domain_ptr,
				    vtp_trunk_itf_ptr);

extern unsigned char vtp_precheck_vlan(vtp_vlan_info* v_info); 

extern unsigned char vtp_test_vlan(vlan_domain_ptr domain_p,
				   unsigned char mode,
				   vtp_vlan_info* v_info);

extern unsigned char 
vtp_configure_vlan_in_database(vlan_domain_ptr domain_p,
			       unsigned char mode,
			       vtp_vlan_info* v_info);

extern void vtp_do_flooding(vlan_domain_ptr, void *, unsigned short);
extern void vtp_trunk_link_flood_timeout(void* context);

extern void vtp_create_new_revision(vlan_domain_ptr, vtp_boolean);

extern vtp_boolean vtp_validate_md5_digest(vlan_domain_ptr,
				       vtp_trunk_itf_ptr);

extern void vtp_generate_md5_digest(vlan_domain_ptr);

extern unsigned long vtp_get_random_jittered_timer_value(void);

extern void vtp_relay_packet(vtp_trunk_itf_ptr, void *, 
			     unsigned long);

extern vtp_boolean vtp_enter_tlb_entries(vlan_domain_ptr, vtp_vlan_info*);

extern void vtp_delete_tlb_entries(vlan_domain_ptr, vtp_vlan_info*);

extern void retrieve_vlan_info(vlan_info_ent_ptr, vtp_vlan_info*);
extern void retrieve_domain_info(vlan_domain_ptr, vtp_domain_info*);
extern void retrieve_trunk_info(vtp_trunk_itf_ptr, vtp_trunk_info*);

extern vtp_boolean vtp_is_default_vlan(vlan_info_ent_ptr);

extern long name_cmp(unsigned char *, unsigned char *, unsigned long,
		     unsigned long); 

extern void vtp_rcvd_lower_rev_number(vtp_trunk_itf_ptr);

extern void vtp_rcvd_equal_rev_number(vtp_trunk_itf_ptr, void *);

extern void vtp_rcvd_higher_rev_number(vtp_trunk_itf_ptr, void *,
				       unsigned char *); 

extern void vtp_update_revision_number(vlan_domain_ptr);

extern void vtp_change_to_server(vlan_domain_ptr);

extern void vtp_change_to_client(vlan_domain_ptr);

extern void vtp_change_to_transparent(vlan_domain_ptr);

extern vtp_boolean vtp_update_revision_less_than_domain(unsigned long,
						    unsigned long); 
extern vtp_boolean vtp_update_revision_equal_to_domain(unsigned long,
						   unsigned long); 

extern unsigned char vtp_check_domain_info(vtp_domain_info*);

extern void change_to_client(vlan_domain_ptr domain_p);
#endif __VTP_PRIVATE_H__




