/* $Id: vtp_dep.h,v 1.1.58.3 1996/06/20 19:50:25 cyoung Exp $
 * $Source: /release/112/cvs/Xsys/vtp/vtp_dep.h,v $
 *------------------------------------------------------------------
 * Header file for the VTP platform dependant code
 *
 * January, 1996 - Chris Young
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vtp_dep.h,v $
 * Revision 1.1.58.3  1996/06/20  19:50:25  cyoung
 * Use correct VLAN count to calculate TFTP file size
 * CSCdi60853:  wrong behaviour while reading tftp file --> vlan info
 * recovery fail Branch: California_branch
 * Also while bonnet is up: Put in a limiting depth in for waiting for
 * semaphore to be released.  Allow null domain to receive domain change
 * notifications.  Provide function to manually load TFTP database and
 * load the database not only when TFTP server or file change but also
 * VTP entity state (client --> server).
 *
 * Revision 1.1.58.2  1996/05/21  22:29:29  cyoung
 * Now it returns a code
 * CSCdi56442:  vtp_download_info needs return code
 * Branch: California_branch
 *
 * Revision 1.1.58.1  1996/05/09  14:48:33  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 1.1.86.1  1996/04/27  07:38:35  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.76.2  1996/04/09  04:24:04  cyoung
 * Report TFTP load failure back to VTP process.
 * CSCdi53758:  vtp server does not turn into client when fail to read
 * tftp file
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.1.76.1  1996/04/08  02:42:26  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.64.3  1996/03/27  23:41:35  cyoung
 * MIB - implement new trap formats (correct varbinds)
 * core - vtp_delete_domain crashes with trunk defined
 * depend - TFTP read needs its own process, implement VLAN state
 *          modifications, some cleanup
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.64.2  1996/03/25  02:21:50  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.64.1  1996/03/22  09:45:03  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.36.4  1996/03/19  00:21:21  percyk
 * vtp support on isl links.
 *
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.1.36.3  1996/03/13  17:28:01  cyoung
 * Get rid of vtp_edit_vlan, use download instead.
 * Add vtp_store_domain_info, one stop database storing.
 * Put in semaphore to avoid saving database being loaded in.
 * More TFTP support.
 * Add initial code for 'vtp configured' action routines.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.1.36.2  1996/03/08  18:19:02  cyoung
 * Remove 'vtp network' command.
 * Add tftp server and file name parameters to 'server' keyword
 *  and support.
 * Check managementDomainIndex range in MIB code.
 * Change to client on reception of any error (not just NVRAM error).
 * Set the updater when configured from the CLI.
 * Set the appropriate state of a VLAN in vtp_edit_vlan
 * Remove unneeded function declarations in vtp_dep.h
 * Fix memory corruption in vtp_upload_info.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.1.36.1  1996/02/27  21:20:28  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.1.18.8  1996/02/22  04:42:55  percyk
 * remove basic typedefs from core code for portability reasons.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.1.18.7  1996/02/15  17:49:48  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * more code cleanup.
 *
 * Revision 1.1.18.6  1996/02/15  02:30:36  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * move platform dependent stuff into interface files.
 *
 * Revision 1.1.18.5  1996/02/13  04:03:08  cyoung
 * move notification_enabled to vtp_dep.c, minor bug fixes, add code to
 * check for native vlan range and existance of trunk before allowing sets.
 * Also some code shuffle as a result of resolving conflicts.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.1.18.4  1996/02/12  21:VTP_MAX_DOMAIN_NAME_LENGTH:37  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * split vtp.c into vtp.c and vtp_util.c
 *
 * Revision 1.1.18.3  1996/02/01  19:17:36  cyoung
 * Start work on LANE encaps for VTP adverts.  Reset timer to zero when
 * freed.  Fix 'imaginative' cast for SAID value.  More basic cleanup.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.1.18.2  1996/01/31  02:34:05  cyoung
 * Provide function to allow variance in managementDomainEntry row
 * creation.
 * Remove redundant consistency checking.  Add hooks to correctly report
 * result of 'apply' operation.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.1.18.1  1996/01/23  01:57:11  cyoung
 * Branch: LE_Cal_V111_0_16_branch
 * fix the non-non-sync.
 *
 * Revision 1.1  1996/01/09  00:44:34  cyoung
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef VTP_DEP_H
#define VTP_DEP_H

#define VTP_CREATE_AND_WAIT 5
#define VTP_NOT_IN_SERVICE 2

#define VTP_NOTIFICATION_ENABLED 1
#define VTP_NOTIFICATION_DISABLED 2

typedef struct vtp_subif_subidb_ {
  boolean vtp_enabled;
  boolean is_pruned;
} vtp_subif_subidb;

#define VTP_TFTP_MAX_FILE_SIZE 66480 /* 1005 VLANs (66 bytes ea.) plus
					one domain info block */
#define VTP_TFTP_MIN_FILE_SIZE 480   /* 5 VLANs (default VLANs) plus
					one domain info block */

/* Definitions for IPC message code */
typedef enum {
    VTP_CONFIGURE_DOMAIN,
    VTP_CHANGE_DOMAIN_NAME,
    VTP_CONFIGURE_TRUNK,
    VTP_CONFIGURE_VLAN,
    VTP_DOWNLOAD_CONFIG,
    VTP_SHUTDOWN,
    VTP_CLEAR_STATISTICS,
    VTP_DELETE_DATABASE,
    VTP_LOAD_TFTP,
    VTP_TFTP_LOAD_FAILURE,
    VTP_PRUNING_TRUNK_UPDOWN,
    VTP_PRUNING_STP_CHANGE,
    VTP_PRUNING_ELIGIBLE,
    VTP_PRUNING_LOCAL,
    VTP_PRUNING_VLAN_ELIG
} vtp_message_t;
 
/* IPC structure to add/modify a VTP management domain */
typedef struct {
    vtp_message_t   message;
    unsigned char   operation;
    vtp_domain_info d_info;
} vtp_ipc_config_domain;

typedef struct {
    vtp_message_t message;
    unsigned char old_name_length;
    unsigned char old_name[VTP_MAX_DOMAIN_NAME_LENGTH];
    unsigned char new_name_length;
    unsigned char new_name[VTP_MAX_DOMAIN_NAME_LENGTH];
} vtp_ipc_change_domain_name;

/* IPC structure to add a trunk link */
typedef struct {
    vtp_message_t  message;
    unsigned char  operation;
    vtp_trunk_info t_info;
} vtp_ipc_config_trunk;
 
/* IPC structure to configure a VLAN */
typedef struct {
    vtp_message_t message;
    unsigned char dname_length;
    unsigned char domain_name[VTP_MAX_DOMAIN_NAME_LENGTH];
    unsigned char operation;
    vtp_vlan_info v_info;
} vtp_ipc_config_vlan;

/* IPC structure to download VTP configuration */
typedef struct {
    vtp_message_t message;
    unsigned char domain_name[VTP_MAX_DOMAIN_NAME_LENGTH];
    unsigned char dname_length;
    boolean       save_config;
    unsigned char *file;           
} vtp_ipc_download_config;
 
/* IPC structure shutdown VTP */
typedef struct {
    vtp_message_t message;
} vtp_ipc_shutdown;
 
/* IPC structure clear statistics */
typedef struct {
    vtp_message_t message;
    unsigned char clear_all;
    unsigned char dname_length;
    unsigned char domain_name[VTP_MAX_DOMAIN_NAME_LENGTH];
} vtp_ipc_clear_statistics;
 
/* IPC structure delete database */
typedef struct {
    vtp_message_t  message;
    unsigned char dname_length;
    unsigned char domain_name[VTP_MAX_DOMAIN_NAME_LENGTH];
} vtp_ipc_delete_database; 
 

/* IPC structure to load the VTP database via TFTP for a domain */ 
typedef struct {
    vtp_message_t message;
    unsigned char dname_length;
    unsigned char domain_name[VTP_MAX_DOMAIN_NAME_LENGTH];
} vtp_ipc_load_tftp;

typedef struct { 
    vtp_message_t message;
    unsigned char dname_length;
    unsigned char domain_name[VTP_MAX_DOMAIN_NAME_LENGTH];
} vtp_ipc_tftp_load_failure;

typedef struct {
    vtp_message_t message;
    unsigned char dname_length;
    unsigned char domain_name[VTP_MAX_DOMAIN_NAME_LENGTH];
    void*         trunk_id;
    unsigned char up_flag;
} vtp_ipc_prune_trunk_state;

typedef struct {
    vtp_message_t  message;
    unsigned char  dname_length;
    unsigned char  domain_name[VTP_MAX_DOMAIN_NAME_LENGTH];
    unsigned short vlan_id;
    void*          trunk_id;
    unsigned short span_state;
} vtp_ipc_prune_stp_state;

typedef struct {
    vtp_message_t message;
    void* trunk_id;
    unsigned char elig_mask[VLANS_BITMASK_SIZE];
} vtp_ipc_prune_eligible;

typedef struct {
    vtp_message_t message;
    unsigned char dname_length;
    unsigned char domain_name[VTP_MAX_DOMAIN_NAME_LENGTH];
    unsigned short vlan_id;
    unsigned char flag;
} vtp_ipc_prune_local;

typedef struct {
    vtp_message_t message;
    void* trunk_id;
    unsigned short vlan_id;
    boolean is_eligible;
} vtp_ipc_prune_vlan_elig;

/* IPC general structure union */
typedef union {
    vtp_message_t message;
    vtp_ipc_config_domain         config_domain;
    vtp_ipc_change_domain_name    dname_change;
    vtp_ipc_config_trunk          config_trunk;
    vtp_ipc_config_vlan           config_vlan;
    vtp_ipc_download_config       download_config;
    vtp_ipc_shutdown              shutdown;
    vtp_ipc_clear_statistics      clear_statistics;
    vtp_ipc_delete_database       delete_database;
    vtp_ipc_tftp_load_failure     load_failure;
    vtp_ipc_prune_trunk_state     prune_trunk_state;
    vtp_ipc_prune_stp_state       prune_span_state;
    vtp_ipc_prune_eligible        prune_eligible;
    vtp_ipc_prune_local           prune_local;
    vtp_ipc_prune_vlan_elig       vlan_elig;
} vtp_ipc_message;


/*
 *
 * Timer Service.
 *
 */
 
/* Definition for private timer structure */
typedef struct _vtp_timer {
    struct _vtp_timer *next;
    struct _vtp_timer *prev;
    mgd_timer the_timer;
    int (*exp_func)(void *);
    void *context;
} vtp_timer;

extern void vtp_media_registry(void);
extern void vtp_process_packet_queue(void);
extern void vtp_process_timer(void);
extern void vtp_process_message(void);
extern void vtp_process_teardown(void);
extern void vtp_root_timer_init(void);

extern void vtp_storage_exceeded(unsigned char*, unsigned char);
extern void vtp_mtu_too_big(vtp_domain_info*, vtp_vlan_info*,
			    vtp_trunk_info*);
extern void vtp_digest_error_detected(unsigned char*, unsigned char);
extern void vtp_revnum_error_detected(unsigned char*, unsigned char);
extern void vtp_report_apply_status(vtp_domain_info* , long);
extern boolean vtp_configure_isl_on_idb(idbtype *, vtp_vlan_info *);
extern idbtype *vtp_find_idb_for_isl(unsigned short, idbtype *);
extern void vtp_configure_sde_on_idb(idbtype *, vtp_vlan_info *);
extern idbtype *vtp_find_sde_on_idb(unsigned long, idbtype *); 

/* 
 * Functions defined in the IOS                   
 */
extern boolean vtp_running;
extern boolean vtp_explicitly_disarmed;

extern void vtp_start_process (void);
extern void vtp_enqueue (paktype *);
extern process vtp_process_main (void);
extern void vtp_terminate (void);
extern void vtp_debug_init(void);

extern unsigned char vtp_multicast[];
extern unsigned char vtp_tr_func_addr[];

/* 
 * Functions used for CLI/SNMP/TFTP configuration 
 */

extern void vtp_config_commands(parseinfo *csb);

extern void vtp_post_configure_domain(unsigned char mode,
				      vtp_domain_info* d_info); 

extern void 
vtp_post_change_vlan_local_assoc(unsigned char dname_length, 
				 unsigned char* domain_name, 
				 unsigned short vlan_id, 
				 unsigned char flag);

extern void vtp_post_change_domain_name(unsigned char old_length,
					unsigned char* old_name,
					unsigned char new_length,
					unsigned char* new_name);

extern void vtp_post_configure_trunk(unsigned char mode,
				     vtp_trunk_info* t_info);
extern void 
vtp_post_change_trunk_link_state(unsigned char dname_length, 
				 unsigned char* domain_name, 
				 void* trunk_id, 
				 unsigned char up_flag);

extern void
vtp_post_change_trunk_vlan_stp_state(unsigned char dname_length, 
				     unsigned char* domain_name,
				     unsigned short isl_vlan_id,
				     void* trunk_id, 
				     unsigned short new_stp_state);

extern void
vtp_post_change_vlan_pruning_eligible(void* trunk_id, 
				      unsigned char* eligibility);

extern void
vtp_post_set_vlan_eligible_on_trunk(void* trunk_id, unsigned short vlan_id,
				    boolean is_eligible);

extern void vtp_post_configure_vlan(unsigned char dname_length, 
				    unsigned char* domain_name,
				    unsigned char mode,
				    vtp_vlan_info* v_info); 

extern void vtp_post_download_config(unsigned char* domain_name,
				     unsigned char dname_length,
				     boolean save_config, unsigned
				     char need_swap, unsigned  
				     char *file);
extern void vtp_post_shutdown(void);
extern void vtp_start_process(void);
extern void vtp_post_clear_statistics(unsigned char clear_all,
				      unsigned char dname_length,
				      unsigned char *domain_name); 
extern void vtp_parser_init(void);
extern void vtp_post_delete_database(unsigned char dname_length,
				     unsigned char *domain_name); 
extern boolean vtp_cli_database_find(vtp_vlan_info* info, unsigned short
				     vlan_id);
extern int vtp_cli_database_init(vtp_domain_info* d_info);
extern int vtp_cli_database_editing(unsigned char mode, 
				    vtp_vlan_info* vlan_info); 
extern void vtp_cli_database_apply(vtp_domain_info* d_info, int res_method);
extern void vtp_cli_database_abort(void); 
extern unsigned char vtp_load_database(vtp_domain_info* d_info);

#endif
