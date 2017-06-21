/* $Id: vtp.h,v 1.2.58.3 1996/06/18 00:50:58 cyoung Exp $
 * $Source: /release/112/cvs/Xsys/vtp/vtp.h,v $
 *------------------------------------------------------------------
 * vtp.h - VLAN transport protocol header file (portable module)
 *
 * Dec 1995, Dean Cheng 
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vtp.h,v $
 * Revision 1.2.58.3  1996/06/18  00:50:58  cyoung
 * CSCdi60069:  router crashes when writing vtp info to tftp file
 * CSCdi59372:  wrong behavior when conflicting vlan info is received
 * CSCdi59933:  RSP crashes for creating-deleting-recreating vtp-domains
 * CSCdi60044:  7000 image crash
 * CSCdi60394:  [sh vtp trunk] crashes 7000
 * CSCdi60528:  rsp can't change vtp database until someone else does
 * CSCdi60541:  Attempt to delete a vlan crashes image
 * CSCdi60632:  cannot set password for vtp management-domain
 * CSCdi60384:  configuration timestamp is not displayed correctly
 * Plus some Cat5k bug fixes rolled in as well as dependant code clean up.
 * Branch: California_branch
 *
 * Revision 1.2.58.2  1996/05/21  22:29:23  cyoung
 * Now it returns a code
 * CSCdi56442:  vtp_download_info needs return code
 * Branch: California_branch
 *
 * Revision 1.2.58.1  1996/05/09  14:48:27  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 1.2.84.1  1996/04/27  07:38:29  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.2.74.5  1996/04/18  21:26:49  percyk
 * added consistency checks for translational vlans via snmp.
 *
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.2.74.4  1996/04/17  21:56:56  cyoung
 * CSCdi53840:  error message should pop up once the error occurs
 * Use errmsg to provide timely VTP error logging.
 *
 * CORE - retrieve domain/vlan functions modify input parameters instead
 *        of affecting global structure.
 *        Convert macros to functions (change_to_client,
 *        change_to_transparent).
 *        domain/vlan notifications now pass before and after data so that
 *        transistion actions can be done.
 *        rip out duplicate add/changes from the verify/validate
 *        routines.
 *        vtp_vlan_change_notification is called without checking for
 *        changes.
 *        do not change to client as a result of failed vlan change
 *        notification of domain revision number change notification.
 *  DEP - support new function declarations, fix bug where trying to
 *        configure swidb when no more can be created.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.2.74.3  1996/04/11  21:36:18  percyk
 * vtp core code error return codes moved to esported header file.
 *
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.2.74.2  1996/04/11  20:50:42  cyoung
 * CSCdi53657:  missing TR default VLAN 1003
 * CSCdi53488:  4500 crashes when configed with a wrong vtp config file
 * MIB - Fix bug with SR VLAN info not showing up
 * CORE - Add syntax checking for domain info in vtp_download_info and
 *        vtp_add_domain, add syntax checking for VLANs in
 *        vtp_download_info
 * DEP - Make TFTP loading silent, zero length loaded file still okay but
 *       runt TFTP file now flagged as error, prevent loading of database
 *       from a revision number change
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.2.74.1  1996/04/08  02:42:18  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.2.64.4  1996/04/04  18:24:34  percyk
 * more error codes when configuring vlans.
 * more error checking for translational vlans.
 *
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.64.3  1996/04/01  18:52:00  percyk
 * vtp allows more than one eth vlan to be translated to fddi vlan.
 * vtp does not update fcp when a new translation is added.
 *
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.64.2  1996/03/25  02:21:46  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.64.1  1996/03/22  09:44:57  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.36.8  1996/03/22  16:22:16  cyoung
 * MIB: allow TR types to be created, check range on newly created vlan
 *      ids, add trap cli commands
 * VTP core: replace REV_LESS macro with function, include notification
 *           for deleted domain
 * VTP dep: clean up after deleted trunks and domains (including startup
 *          time), add some code for vlan modification notification.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.7  1996/03/13  17:27:56  cyoung
 * Get rid of vtp_edit_vlan, use download instead.
 * Add vtp_store_domain_info, one stop database storing.
 * Put in semaphore to avoid saving database being loaded in.
 * More TFTP support.
 * Add initial code for 'vtp configured' action routines.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.6  1996/03/07  02:35:07  cyoung
 * Added vtp network command.
 * Cleanup database subcommand looping.
 * Don't require ring keyword for fddi type vlans.
 * Choose better values for default VLAN fields.
 * Assign domain index before inserting a new domain into the list.
 * Assign correct state when going through vtp_edit_vlan.
 * Add support for removing configuration as a result of VLAN deletion.
 * Rename spanning tree defines to match the MIB.
 * nv_gen support for all VTP commands.
 * set the parent_valid field correctly.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.5  1996/03/05  00:54:21  percyk
 * transition back to null domain state on md5 digest errors.
 *
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.4  1996/03/04  23:48:37  percyk
 * define new platform dependent routine on rcving no response to req
 * packet.
 *
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.3  1996/03/04  22:03:00  percyk
 * change revision number logic.
 *
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.2  1996/03/02  21:12:46  percyk
 * include bug fixes done after branch sync.
 *
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.1  1996/02/27  21:20:22  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.4.16  1996/02/23  19:35:59  percyk
 * add accumulate/mirror logic for vtp counters.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.15  1996/02/23  17:10:32  percyk
 * misc bug fixes by cat5k group.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.14  1996/02/22  23:02:07  percyk
 * delete tlb entries from partner vlans when a vlan is deleted.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.13  1996/02/22  04:42:51  percyk
 * remove basic typedefs from core code for portability reasons.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.12  1996/02/19  22:26:41  percyk
 * fixed bug found in tlb testing.  more debugging added.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.11  1996/02/16  19:32:13  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * more code cleanup, add more vtp debugging.
 *
 * Revision 1.2.4.10  1996/02/15  17:49:47  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * more code cleanup.
 *
 * Revision 1.2.4.9  1996/02/15  02:30:25  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * move platform dependent stuff into interface files.
 *
 * Revision 1.2.4.8  1996/02/14  18:33:11  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * do not allow any changes to default vlans.
 *
 * Revision 1.2.4.7  1996/02/13  18:54:42  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * sync router files with cat5k
 *
 * Revision 1.2.4.6  1996/02/13  04:03:05  cyoung
 * move notification_enabled to vtp_dep.c, minor bug fixes, add code to
 * check for native vlan range and existance of trunk before allowing sets.
 * Also some code shuffle as a result of resolving conflicts.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.5  1996/02/12  22:52:05  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * define a new bootup flag.
 *
 * Revision 1.2.4.4  1996/02/12  21:32:34  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * split vtp.c into vtp.c and vtp_util.c
 *
 * Revision 1.2.4.3  1996/02/09  01:09:59  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Add tlb vlan support.  Various misc bug fixes.
 *
 * Revision 1.2.4.2  1996/01/23  00:47:13  percyk
 * Branch: LE_Cal_V111_0_16_branch
 * add default vlans at startup time.  add meat to debug files.
 *
 * Revision 1.2.4.1  1996/01/11  01:47:48  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.1.2.3  1996/01/06  00:37:39  dcheng
 * Update the VTP packet format to include ring number and parent VLAN for
 * FDDI/TR VLAN, bridge number and STP type for FDDI/TR net VLAN, and
 * translational bridged VLAN id (0, 1 or 2). Update the database and
 * user interface for these as well.
 * Branch: LE_Calif_branch
 *
 * Revision 1.1.2.2  1996/01/03  01:49:19  dcheng
 * Submit CLI code for VTP.
 * Branch: LE_Calif_branch
 *
 * Revision 1.1.2.1  1995/12/23  01:45:08  dcheng
 * Branch: LE_Calif_branch
 *
 * Revision 1.1  1995/12/23  01:42:58  dcheng
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __VTP_H__
#define __VTP_H__

/* 
 * Types Definitions.
 */

#ifndef __MASTER_H__
#define FALSE  0
#define TRUE   1
#define NULL ((void *) 0)
typedef unsigned char boolean;
#endif

/* These are duplicated from sr_snmpd.h which cannot be included */
#ifndef EXACT
#define EXACT 0xA0
#endif

#ifndef NEXT
#define NEXT  0xA1
#endif

#define VTP_SOURCE_ADVERT 1
#define VTP_SOURCE_EXTERN 2

/* ---- Double linked list and useful macros ---- */ 
typedef struct list {
    struct list *next;
    struct list *prev;
} dlist, *dlist_ptr;

#define LIST_INIT(E) (E)->next = (E); (E)->prev = (E);        

#define LIST_INSERT(E,L) (L)->prev->next = (E); (E)->next = (L); \
(E)->prev = (L)->prev;  (L)->prev = (E);        

#define LIST_DELETE(E) (E)->prev->next = (E)->next; (E)->next->prev = \
(E)->prev;           

#ifdef LIST_EMPTY
#undef LIST_EMPTY
#endif

#define LIST_EMPTY(L)  (((L)->next == (L)) && ((L)->prev == (L)))

/* ---- Name comparison macros ---- */
#define NAME_EQUAL           0
#define NAME_LESS           -1
#define NAME_GREATER         1


#define MAX_VTP_TX_QUEUE_SIZE        64 


/* ---- buffer queue element ---- */
typedef struct {
    unsigned short length;
    void *buff_p;
} buff_chain, *buff_chain_ptr;

/* ---- VLAN information database entry structure ---- */
typedef struct {
    dlist sort_list;		              /* ascending ordered list on ISL VLAN ID */
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
						  VTP packet */ 
#define VLAN_SUSPENDED     	      0x01     /* same format as in
						  VTP packet */
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
						 type only */ 
    unsigned short parent_vlan;               /* for FDDI/TR type only */
#define ENT_STP_SRT         1
#define ENT_STP_SRB         2
    unsigned short tb_vlan_1;    /* if non-zero, translational bridged
				    vlan 1 */ 
    unsigned short tb_vlan_2;    /* if non-zero, translational bridged
				    vlan 1 */ 
    unsigned char stp_type;      /* for FDDI-NET/TR_NET type only */
    boolean parent_valid;        /* for FDDI only */
    unsigned short tb_vlan_ethernet;
    unsigned short tb_vlan_tr;
    unsigned short tb_vlan_fddi;
} vlan_info_ent, *vlan_info_ent_ptr;

/* ---- VTP packet structures ---- */

/* some constants */
#define VTP_VERSION           0x01
#define VTP_MAX_STORAGE       -1  
#define VTP_SUMMARY_ADV       0x01
#define VTP_SUBSET_ADV        0x02
#define VTP_ADV_RQST          0x03

/* VLAN packet information structure */
#define VTP_MAX_VLAN_INFO_SIZE         60
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
				     4 bytes) */ 
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
						in bytes */ 
    unsigned char domain_name[VTP_MAX_DOMAIN_NAME_LENGTH];
					     /* the domain name,
						padded by zeros */ 

    unsigned long revision;	             /* the configuration
						revision number */ 
    /* followed by one or more vlan_info */
} subset_adv, *subset_adv_ptr;

/* VTP packet advertisement request message structure */
typedef struct {
    unsigned char version;              /* version number */
    unsigned char code;                 /* the type */
    unsigned char reserved;             /* reserved */
    unsigned char dname_length;         /* the domain name length in bytes */
    unsigned char domain_name[32];	/* the domain name, padded by zeros */
    unsigned short start_value;	        /* start value of VLAN Id requested */
} adv_request, *adv_request_ptr;

/* VTP packet summary advertisement message structure */
#define VTP_MAX_CONFIG_INFO_SIZE       36
typedef struct {
    unsigned char version;              /* version number */
    unsigned char code;	                /* the type */
    unsigned char followers;            /* the number of subset_adv followed */
    unsigned char dname_length;	        /* the domain name length in bytes */
    unsigned char domain_name[32];      /* the domain name, padded by zeros */
    unsigned long revision;             /* the configuration revision number */
    unsigned long updater_id;           /* id (IP address) of updating device */
    unsigned char update_timestamp[12];	/* time stamp as update */
    unsigned char md5_digest[16];	/* check sum */
} summary_adv, *summary_adv_ptr;

#define VTP_FLOOD_INTERVAL           300     /* in seconds */
#define MAX_VTP_UPDATE_PENDING_TIME  30000   /* in ms, 30 second */

/* The trunk advt rxmt timer is jittered to including a random
   number from 0 - 3 seconds.
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

/* ---- VTP management domain structure ---- */

typedef struct {
    /* VLAN domain info and chain */
    dlist domain_list;                       /* list of domain */
    unsigned long index;                     /* domain index */
    unsigned char dname_length;              /* domain name length */
    unsigned char mode;                      /* mode of the VTP */
    unsigned char domain_name[VTP_MAX_DOMAIN_NAME_LENGTH + 1]; 

    /* trunk link list */
    dlist trunk_list;                        /* trunk list */
      
    /* VTP operating parameters */
    unsigned long revision;                  /* revision number */
    unsigned char md5_digest[VTP_MD5_DIGEST_SIZE];
    unsigned long updater;                   /* IP addr of VTP server
						responsible for change*/
    unsigned long my_update_id;             /* IP addr of this VTP
					       entity or zero */
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
    boolean vtp_updating;		     /* TRUE if in updating */
    unsigned char flags;                     /* misc flags */
#define UPDATE_RXMT_REQUESTED       0x01      
    unsigned char next_adv_src_mac[6];	     /* sender's MAC address */
    unsigned short last_value_learned;       /* last ISL VLAN Id just
						learned */ 
    void *update_pending_timer;              /* update pending timer */
    void *update_trunk_p;                    /* link where advt received now */
    void *scratch;      		     /* mass memory for
						received advt */ 
    unsigned long next_seq_expected;         /* next seq. to receive */
    vlan_info_ptr first_vlan_p;              /* point to the first */
    vlan_info_ptr last_vlan_p;               /* point to the last */
    unsigned long update_vlan_count;         /* number of VLAN updated */
    unsigned long update_vlan_length;        /* length of VLAN info */

    /* VLAN info database */
    dlist info_sorted_list;		     /* sorted on ISL VLAN Id
					      */
    dlist info_backup_list;                  /* the old database
						in case the new one is
						inconsistent */

    /* trasnmit packet queues */
    unsigned long tx_queue_count;            /* packet count in tx queue */ 
    vlan_info_ent_ptr tx_next_vlan_p;        /* pointer to the next VLAN */
    buff_chain tx_buff_queue[MAX_VTP_TX_QUEUE_SIZE]; /* tx queue */

    unsigned long vlan_count;	             /* VLAN count */
    vtp_stats acc;                           /* accumulated stats - snmp */
    vtp_stats mirror;                        /* mirror stats - cli */ 
    boolean busy;                            /* TRUE for busy */
    /* TRUE if transitioning from null 2 valid domain */
    boolean transitioning;  
} vlan_domain, *vlan_domain_ptr;


/* ---- VTP trunk link interface structure ---- */
typedef struct {
    dlist domain_list;       /* trunk list within a domain */
    dlist trunk_list;        /* global trunk list */  
    vlan_domain_ptr domain_p; /* poiner to domain structure */
    void *trunk_id;          /* reference to trunk link */
    unsigned short mtu;              /* MTU on the trunk link */
#define DEFAULT_TRUNK_MTU  1492
    unsigned char vlans_enabled[VLANS_BITMASK_SIZE];
    unsigned short native_vlan;
    unsigned char encaps_type;
    void *advt_timer_p;      /* pointer to advt retransmit timer */
    unsigned long flood_timestamp;   /* timestamp at last flooding - in ms */
    unsigned long jittered_value;    /* random number in range 0-1000 ms */
    void *rqst_timer_p;      /* pointer to advt request timer */
    unsigned short rqst_start_value; /* first ISL VLAN Id to request */
#define SEND_ALL       0
#define SEND_NONE      0xffff
    unsigned short advt_start_value; /* first ISL VLAN Id to advt */
#define RQST_ALL       0
#define RQST_NONE      0xffff
    unsigned char rqst_retry_count;  /* rqst retry counter */
#define VTP_MAX_RQST_RETRY_COUNT  10
    unsigned char flags;             /* reserved */
} vtp_trunk_itf, *vtp_trunk_itf_ptr;

/* ---- revision number comparison ---- */
#define VTP_BIG_REV          (unsigned long)0x80000000
extern boolean vtp_rev_less(unsigned long, unsigned long);
#define VTP_REV_EQUAL(A, B) (A == B)

#define ADJUST_LENGTH_LONG(x) ((x) = ((x)&3) != 0 ? (x)+4-((x)&3) : (x))

#define VTP_TIMER_JITTERED_PERCENT         10
#define VTP_TIMER_MAX_RANDOM_MS            1000  
#define VTP_REQUEST_INTERVAL               5000

#ifndef __MASTER_H__
extern unsigned char version;                      /* version number */
#endif


extern unsigned long   vtp_max_vlan_storage;       /* max VLAN storage */ 
extern unsigned long   vtp_id;
extern boolean no_domain_state;
extern boolean vtp_is_initialized;
extern unsigned long   vtp_domain_index;
extern unsigned char   vtp_version;

/* extern function declarations */

extern void 
vtp_add_default_vlans(unsigned char dname_length, 
		      unsigned char* domain_name);
extern void vtp_sort_vlan_entry(vlan_domain_ptr, vlan_info_ent_ptr);
extern vlan_info_ent_ptr vtp_insert_info_entry(vlan_domain_ptr,
					       vtp_vlan_info*); 
extern vlan_info_ent_ptr vtp_search_info_entry_by_vlan(vlan_domain_ptr, 
						       unsigned short vlan_id);
extern vlan_info_ent_ptr vtp_search_info_entry_by_name(vlan_domain_ptr, 
						       unsigned char name_length,
						       unsigned char* name);
extern vlan_info_ent_ptr vtp_search_info_entry_by_said(vlan_domain_ptr, 
						       unsigned long said);

extern void vtp_delete_database(unsigned char, unsigned char *);

extern void vtp_flush_database(vlan_domain_ptr);
extern void vtp_flush_update_adv_info(vlan_domain_ptr);
extern void vtp_flush_tx_subset_adv_queue(vlan_domain_ptr);

extern void vtp_receive_summary_adv_packet(void *, void *, unsigned long, 
					   unsigned char *);
extern void vtp_receive_subset_adv_packet(void *, void *, unsigned long, 
					  unsigned char *);
extern void vtp_receive_adv_request_packet(void *, void *, unsigned long, 
					   unsigned char *);
extern boolean vtp_validate_subset_adv(vtp_trunk_itf_ptr, void *,
				       unsigned long); 
extern void vtp_process_subset_adv(vlan_domain_ptr,
				   vtp_trunk_itf_ptr);
extern void vtp_allocate_update_memory(vtp_trunk_itf_ptr, unsigned char);
extern void vtp_record_subset_adv(vtp_trunk_itf_ptr, void *, unsigned
				  long);

extern void vtp_mark_database_old(vlan_domain_ptr);
extern unsigned char vtp_delete_old_vlans(vlan_domain_ptr);

extern void vtp_transmit_adv_request_packet(vtp_trunk_itf_ptr, unsigned short);
extern void vtp_do_trunk_flooding(vtp_trunk_itf_ptr, unsigned short);
extern void vtp_transmit_summary_adv_packet(vtp_trunk_itf_ptr, unsigned char);
extern void vtp_generate_subset_adv_packet(vlan_domain_ptr, vtp_trunk_itf_ptr);
extern void vtp_transmit_subset_adv_packet(vtp_trunk_itf_ptr);
extern void vtp_trunk_link_rqst_timeout(vtp_trunk_itf_ptr);

extern vlan_domain_ptr vtp_search_domain(unsigned char, unsigned char *);
extern unsigned long vtp_get_index(void);
extern vtp_trunk_itf_ptr vtp_search_trunk_link(vlan_domain_ptr, void *);
extern void vtp_insert_domain_trunk(vlan_domain_ptr,
				    vtp_trunk_itf_ptr);

extern unsigned char vtp_precheck_vlan(vtp_vlan_info* v_info);
extern unsigned char vtp_test_vlan(unsigned char dname_length, 
				   unsigned char* domain_name, 
				   unsigned char mode,
				   vtp_vlan_info* v_info);
extern unsigned char 
vtp_configure_vlan_in_database(unsigned char dname_length, 
			       unsigned char* domain_name, 
			       unsigned char mode,
			       vtp_vlan_info* v_info);
extern void vtp_do_flooding(vlan_domain_ptr, void *, unsigned short);

extern void vtp_create_new_revision(vlan_domain_ptr, boolean);
extern boolean vtp_validate_md5_digest(vlan_domain_ptr, vtp_trunk_itf_ptr);
extern void vtp_generate_md5_digest(vlan_domain_ptr);

extern unsigned long vtp_get_random_jittered_timer_value(void);
extern void vtp_relay_packet(vtp_trunk_itf_ptr, void *, unsigned long);
extern void retrieve_vlan_info(vlan_info_ent_ptr, vtp_vlan_info*);
extern void retrieve_domain_info(vlan_domain_ptr, vtp_domain_info*);
extern void retrieve_trunk_info(vtp_trunk_itf_ptr, vtp_trunk_info*);
extern boolean vtp_is_default_vlan(vlan_info_ent_ptr);
extern boolean vtp_is_default_vlan_id(unsigned short isl_vlan_id);
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
extern boolean vtp_update_revision_less_than_domain(unsigned long,
						    unsigned long); 
extern boolean vtp_update_revision_equal_to_domain(unsigned long,
						   unsigned long); 
extern unsigned char vtp_check_domain_info(vtp_domain_info*);
#endif __VTP_H__




