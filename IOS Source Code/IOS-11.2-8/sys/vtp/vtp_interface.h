/* $Id: vtp_interface.h,v 1.2.58.4 1996/06/20 19:50:27 cyoung Exp $
 * $Source: /release/112/cvs/Xsys/vtp/vtp_interface.h,v $
 *------------------------------------------------------------------
 * vtp_interface.h - Common header file for portable and dependent VTP module.
 *
 * Dec 1995, Dean Cheng 
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vtp_interface.h,v $
 * Revision 1.2.58.4  1996/06/20  19:50:27  cyoung
 * Use correct VLAN count to calculate TFTP file size
 * CSCdi60853:  wrong behaviour while reading tftp file --> vlan info
 * recovery fail Branch: California_branch
 * Also while bonnet is up: Put in a limiting depth in for waiting for
 * semaphore to be released.  Allow null domain to receive domain change
 * notifications.  Provide function to manually load TFTP database and
 * load the database not only when TFTP server or file change but also
 * VTP entity state (client --> server).
 *
 * Revision 1.2.58.3  1996/06/18  00:51:08  cyoung
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
 * Revision 1.2.58.2  1996/05/21  22:29:33  cyoung
 * Now it returns a code
 * CSCdi56442:  vtp_download_info needs return code
 * Branch: California_branch
 *
 * Revision 1.2.58.1  1996/05/09  14:48:37  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 1.2.84.1  1996/04/27  07:38:39  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.2.74.5  1996/04/17  21:57:02  cyoung
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
 * Revision 1.2.74.4  1996/04/11  21:36:20  percyk
 * vtp core code error return codes moved to esported header file.
 *
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.2.74.3  1996/04/11  20:50:46  cyoung
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
 * Revision 1.2.74.2  1996/04/09  04:24:06  cyoung
 * Report TFTP load failure back to VTP process.
 * CSCdi53758:  vtp server does not turn into client when fail to read
 * tftp file
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.2.74.1  1996/04/08  02:42:31  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.2.64.4  1996/04/04  18:24:38  percyk
 * more error codes when configuring vlans.
 * more error checking for translational vlans.
 *
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.64.3  1996/04/03  22:37:01  cyoung
 * New objects for TFTP support, ETR vlan type and trunk table creation
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MIB - TFTP support, ETR type allowed, still need work on Trunk creation
 * dep - use temp buf to determine add/mod mode instead of real database
 * core - add tooBig hook for trap generation
 *
 * Revision 1.2.64.2  1996/03/25  02:21:54  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.64.1  1996/03/22  09:45:06  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.36.6  1996/03/19  19:10:01  percyk
 * remove unused function
 *
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.5  1996/03/13  17:28:06  cyoung
 * Get rid of vtp_edit_vlan, use download instead.
 * Add vtp_store_domain_info, one stop database storing.
 * Put in semaphore to avoid saving database being loaded in.
 * More TFTP support.
 * Add initial code for 'vtp configured' action routines.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.4  1996/03/07  02:35:15  cyoung
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
 * Revision 1.2.36.3  1996/03/05  01:56:10  cyoung
 * Add vtp configured interface command.  Provide function to retrieve
 * SNMP stats. Fix problem which tries to access NULL pointer when
 * adding a domain after all have been deleted.  Make vtp_upload_info
 * more useful.  Put in beginning TFTP support.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.2  1996/03/04  23:48:46  percyk
 * define new platform dependent routine on rcving no response to req
 * packet.
 *
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.1  1996/02/27  21:20:33  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.4.14  1996/02/23  23:05:51  cyoung
 * Independant code put in for checking native vlan range.
 * Do not increment the domain index when adding the 'null domain'.
 * remove duplicate function declarations from vtp_interface.h
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.13  1996/02/22  04:42:57  percyk
 * remove basic typedefs from core code for portability reasons.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.12  1996/02/22  03:10:04  cyoung
 * Move include files around to make code more easily ported to Cat5000.
 * TruthValue false is 2 and not 0.
 * Roll function declarations from vtp_agent into vtp_interface and
 * vtp_externs (cut down on number of include files).
 * Add IOS_SPECIFIC compile flag to mark non-portable code.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.11  1996/02/16  19:32:16  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * more code cleanup, add more vtp debugging.
 *
 * Revision 1.2.4.10  1996/02/15  17:49:49  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * more code cleanup.
 *
 * Revision 1.2.4.9  1996/02/15  02:30:40  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * move platform dependent stuff into interface files.
 *
 * Revision 1.2.4.8  1996/02/13  18:54:44  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * sync router files with cat5k
 *
 * Revision 1.2.4.7  1996/02/13  04:03:10  cyoung
 * move notification_enabled to vtp_dep.c, minor bug fixes, add code to
 * check for native vlan range and existance of trunk before allowing sets.
 * Also some code shuffle as a result of resolving conflicts.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.6  1996/02/12  21:41:41  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * remove ios specific debugging from platform independent file.
 *
 * Revision 1.2.4.5  1996/02/12  21:32:38  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * split vtp.c into vtp.c and vtp_util.c
 *
 * Revision 1.2.4.4  1996/02/09  01:10:02  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Add tlb vlan support.  Various misc bug fixes.
 *
 * Revision 1.2.4.3  1996/02/08  04:12:37  cyoung
 * Set default time interval when creating or modifying management domain.
 * Add support for setting vlan filter on trunks and the native vlan.
 * Add support for transmitting adverts over LANE.
 * Add code for removing trunks from idblist.
 * Add interface for core code to notify dependant code when a mod has
 *  been made to the trunk list.
 * More code cleanup.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.2  1996/01/24  01:37:58  cyoung
 * VTP MIB support and dependant auto-config base. Part II
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.1  1996/01/11  01:47:52  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.1.2.4  1996/01/09  00:36:49  dcheng
 * Changed the HDLC type code for VTP.
 * Branch: LE_Calif_branch
 * Fixed an error in VLAN name length handling.
 *
 * Revision 1.1.2.3  1996/01/06  00:37:43  dcheng
 * Update the VTP packet format to include ring number and parent VLAN for
 * FDDI/TR VLAN, bridge number and STP type for FDDI/TR net VLAN, and
 * translational bridged VLAN id (0, 1 or 2). Update the database and
 * user interface for these as well.
 * Branch: LE_Calif_branch
 *
 * Revision 1.1.2.2  1996/01/03  01:49:22  dcheng
 * Submit CLI code for VTP.
 * Branch: LE_Calif_branch
 *
 * Revision 1.1.2.1  1995/12/23  01:52:58  dcheng
 * Branch: LE_Calif_branch
 *
 * Revision 1.1  1995/12/23  01:51:40  dcheng
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __VTP_INTERFACE_H__
#define __VTP_INTERFACE_H__


/* Type of VTP/VLAN operations  */
typedef enum {
    VTP_ADD = 1,
    VTP_DELETE,
    VTP_MODIFY
} Vtp_Op_t;

/* Type of return errors 
 * used as the third parameter in the vtp_domain_change_notification() and
 * vtp_vlan_change_notification() call.
 */
typedef enum {
    VTP_NOTIFICATION_OK = 0,
    VTP_NOTIFICATION_NVR_ERR,
    VTP_GENERAL_ERR
} Vtp_Notif_Err_t;

/* configuration return codes */
typedef enum {
    VTP_OK = 0,
    VTP_ERROR_NOT_INITIALIZED,
    VTP_ERROR_ILLEGAL_INPUT,
    VTP_ERROR_NO_INPUT,
    VTP_ERROR_NO_DOMAIN,
    VTP_ERROR_NOT_SERVER,
    VTP_ERROR_VLAN_BADTYPE,
    VTP_ERROR_VLAN_BADSTATE,
    VTP_ERROR_VLAN_BADMTU,
    VTP_ERROR_VLAN_BADID,
    VTP_ERROR_VLAN_BADSAID,
    VTP_ERROR_VLAN_BADRING,
    VTP_ERROR_VLAN_BADBRIDGE,
    VTP_ERROR_VLAN_BADPARENT,
    VTP_ERROR_VLAN_BADSTPTYPE,
    VTP_ERROR_VLAN_BADTRANSLATION,    
    VTP_ERROR_DUPLICATE_VLAN_ID,
    VTP_ERROR_DUPLICATE_VLAN_NAME,
    VTP_ERROR_DUPLICATE_VLAN_SAID,
    VTP_ERROR_UNKNOWN_TLB,
    VTP_ERROR_SIMILAR_TLB,
    VTP_ERROR_INSERT_FAILURE,
    VTP_ERROR_TLB_INSERT_FAILURE,
    VTP_ERROR_NO_VLAN,
    VTP_ERROR_DEFAULT_VLAN_NAME_CHANGE,
    VTP_ERROR_DEFAULT_VLAN_TYPE_CHANGE,
    VTP_ERROR_DEFAULT_VLAN_MTU_CHANGE,
    VTP_ERROR_DEFAULT_VLAN_STATE_CHANGE,
    VTP_ERROR_INCONSISTENT_VLAN,
    VTP_ERROR_DEFAULT_VLAN,
    VTP_ERROR_NO_TLB,
    VTP_ERROR_MODE_UNKNOWN,
    VTP_ERROR_DOMAIN_NAME_LENGTH,
    VTP_ERROR_DOMAIN_NAME_DUP,
    VTP_ERROR_DOMAIN_NAME_UNPRINT,
    VTP_ERROR_DOMAIN_VLAN_COUNT,
    VTP_ERROR_NO_TRUNK,
    VTP_ERROR_TRUNK_DUP,
    VTP_ERROR_REVISION_ERROR,
    VTP_ERROR_DIGEST_ERROR,
    VTP_ERROR_DATABASE_LOCKED,
    VTP_ERROR_PRUNING_MODE_UNKNOWN,
    VTP_ERROR_NVSTORE_FAILURE
} Vtp_Err_t;

#define VTP_MAX_DOMAIN_NAME_LENGTH     32
#define VLAN_MAX_NAME_LENGTH           32
#define VTP_MAX_TRUNK_NAME_LENGTH      32

/*
 * Structure for VTP domain information block.
 */

#define VTP_MIN_VLANS 5           /* factory-default VLANs */
#define VTP_MAX_VLANS 1023

#define VTP_TIMESTAMP_SIZE 12
#define VTP_MD5_DIGEST_SIZE 16
#define VTP_MAX_PASSWD_LENGTH 64
#define VTP_MAX_TFTP_FILENAME_LENGTH 40

typedef struct  {
    unsigned char mode;          
#define VTP_CLIENT          1
#define VTP_SERVER          2
#define VTP_TRANSPARENT     3
    /* the following are needed only for client/server mode device */ 
    unsigned char domain_name_length;  /* length of domain name, 0-32 */
    unsigned char domain_name[VTP_MAX_DOMAIN_NAME_LENGTH + 1];     

    /* the following are needed only for server device */
    unsigned long revision;             /* config revision number */
    unsigned long updater;              /* id of the config server */
    unsigned long index;                /* domain index */
    unsigned char timestamp[VTP_TIMESTAMP_SIZE]; /* ascii string -
						  *  yymmddhhmmss */ 
    unsigned char md5_digest[VTP_MD5_DIGEST_SIZE];  
    unsigned short advt_interval;        /* advt interval (second) */
    unsigned char password_length;     /* password length */ 
    unsigned char password[VTP_MAX_PASSWD_LENGTH + 1];
    unsigned short vlan_count;           /* number of VLANs */
    char          tftp_file_name[VTP_MAX_TFTP_FILENAME_LENGTH + 1];
    unsigned long tftp_server;

    /* for pruning enable/disable control */
    unsigned char pruning_mode;     /* added for pruning dis/enable */
#define VTP_PRUNING_DISABLE 0
#define VTP_PRUNING_ENABLE  1
#define DEFAULT_VTP_PRUNING_MODE    VTP_PRUNING_DISABLE
} vtp_domain_info;

/*
 * Structure for device based VTP information block.
 */
typedef struct {
    unsigned char version;             /* VTP version number */
    short max_vlan_storage;              /* range from -1 - 1000 */ 
} vtp_device_info;


/*
 * Structure for VLAN based VTP information block.
 */
typedef struct {
    unsigned char length;              /* length of the information
					* structure in bytes */ 
    unsigned char vlan_name_length;    /* length of VLAN name */
    unsigned char vlan_name[VLAN_MAX_NAME_LENGTH + 1];
    unsigned char type;                /* type of the VLAN */
#define VLAN_ETH_TYPE        1
#define VLAN_FDDI_TYPE       2
#define VLAN_TR_TYPE         3
#define VLAN_FDDI_NET_TYPE   4
#define VLAN_TR_NET_TYPE     5
    unsigned char state;               /* state of the VLAN */         
#define VLAN_OPERATIONAL_STATE            1 
#define VLAN_SUSPENDED_STATE              2
#define VLAN_MTU_TOO_BIG_FOR_DEVICE_STATE 3
#define VLAN_MTU_TOO_BIG_FOR_TRUNK_STATE  4
    unsigned short mtu;                /* MTU on this VLAN */
#define VLAN_MIN_MTU 1500
#define VLAN_MAX_MTU 18190
    unsigned short isl_vlan_id;        /* ISL VLAN ID */
    unsigned long said;                 /* IEEE 802.10 SAID */
    unsigned short ring_number;          /* for VLAN_FDDI/TR_TYPE only */
    unsigned char bridge_number;        /* for VLAN_FDDI/TR_NET_TYPE only */
    unsigned char stp_type;             /* for VLAN_FDDI/TR_NET_TYPE only */
#define VLAN_STP_IEEE         1
#define VLAN_STP_IBM          2
    unsigned short parent_vlan;          /* for VLAN_FDDI/TR_NET_TYPE only */
    unsigned short tb_vlan_1;          /* translational bridged VLAN 1 */ 
    unsigned short tb_vlan_2;          /* translational bridged VLAN 2 */ 
    unsigned char parent_valid;        /* TRUE/FALSE - for VLAN_FDDI only */
} vtp_vlan_info;

/* A chained structure of the above */
typedef struct _vtp_vlan_info_chain {
    struct _vtp_vlan_info_chain *next; 
    vtp_vlan_info info;
} vtp_vlan_info_chain;

/*
 * Structure for VTP trunk link information block.
 */
#define VLANS_BITMASK_SIZE 128
typedef struct {
    void *trunk_id;                     /* VTP trunk id */
    unsigned char trunk_name[VTP_MAX_TRUNK_NAME_LENGTH + 1];
    unsigned char domain_name[VTP_MAX_DOMAIN_NAME_LENGTH + 1];
    unsigned char dname_length;
    unsigned char vlans_enabled[VLANS_BITMASK_SIZE];
    unsigned char vlans_pruning_eligible[VLANS_BITMASK_SIZE];
    unsigned short native_vlan;
    unsigned char encaps_type;

/* Note that the following assignments must remain equal to the values
 * defined in sr_vtpmibdefs.h 
 */
#define VTP_TRUNK_ENCAPS_ISL     1
#define VTP_TRUNK_ENCAPS_DOT10   2
#define VTP_TRUNK_ENCAPS_LANE    3 
} vtp_trunk_info;


/* Default values for management domain */

#define VTP_DEFAULT_DOMAIN_NAME_LENGTH                   0
#define VTP_DEFAULT_DOMAIN_NAME         (unsigned char *)0
#define VTP_DEFAULT_LOCAL_MODE                  VTP_SERVER
#define VTP_DEFAULT_DOMAIN_INDEX                         0
#define VTP_DEFAULT_UPDATER                              0
#define VTP_DEFAULT_REVISION                             0
#define VTP_DEFAULT_PASSWORD_LENGTH                      0
#define VTP_DEFAULT_PASSWORD            (unsigned char *)0
#define VTP_DEFAULT_ADVT_INTERVAL                      310/* second */

/* factory default ISL VLAN-id's */
#define VTP_FACTORY_DEFAULT_VLAN_ETHERNET_ISL    0001
#define VTP_FACTORY_DEFAULT_VLAN_FDDI_ISL        1002
#define VTP_FACTORY_DEFAULT_VLAN_TR_ISL          1003
#define VTP_FACTORY_DEFAULT_VLAN_FDDINET_ISL     1004 
#define VTP_FACTORY_DEFAULT_VLAN_TRNET_ISL       1005

#define VTP_MAXIMUM_VLAN_ISL                     1005
#define VTP_MAXIMUM_VLAN_SAID                    0xFFFFFFFE
#define VTP_MAXIMUM_VLAN_RING_NUMBER             4095
#define VTP_MAXIMUM_VLAN_BRIDGE_NUMBER           15

/* Default values for VLAN 
 * Note - NAME_P and LENGTH_P are pointer to unsigned char,
 * vlan name array address and name length, respectively.
 */
#define VTP_VLAN_DEFAULT_NAME(VLAN_ID, NAME_P, LENGTH_P)   \
{NAME_P[0] = 'V';                     \
     NAME_P[1] = 'L';                     \
     NAME_P[2] = 'A';                     \
     NAME_P[3] = 'N';                     \
     NAME_P[4] = (VLAN_ID)/1000 + '0';    \
     NAME_P[5] = (VLAN_ID)%1000/100 + '0';\
     NAME_P[6] = (VLAN_ID)%100/10   + '0';\
     NAME_P[7] = (VLAN_ID)%10 + '0';      \
     *LENGTH_P = 8; }
#define VTP_VLAN_DEFAULT_SAID(VLAN_ID) VLAN_ID + 100000
#define VTP_VLAN_DEFAULT_TYPE   VLAN_ETH_TYPE
#define VTP_VLAN_DEFAULT_MTU    1500           
#define VTP_VLAN_DEFAULT_STATUS VLAN_OPERATIONAL_STATE
#define VTP_VLAN_NO_RING_NUMBER                            0
#define VTP_VLAN_DEFAULT_RING_NUMBER                       1
#define VTP_VLAN_NO_PARENT_VLAN                            0
#define VTP_VLAN_DEFAULT_PARENT_VLAN                    1005
#define VTP_VLAN_NO_BRIDGE_NUMBER                          0
#define VTP_VLAN_DEFAULT_BRIDGE_NUMBER                     1
#define VTP_VLAN_NO_STP_TYPE                               0
#define VTP_VLAN_DEFAULT_STP_TYPE              VLAN_STP_IEEE
#define VTP_VLAN_NO_TB_VLAN                                0 

/* determine the byte offset and bit mask for a vlan in a bit string 
 * v = isl_vlan_id, s = starting (first) vlan id 
 */

#define vlan_bit_offset(v,s)    ( ((v)-(s)) / 8 ) 
#define vlan_bit_mask(v,s)      ( 0x80 >> (((v)-(s)) % 8) )

/* macros for vtp mode */
#define is_valid_pruning_mode(mode) ((mode)==VTP_PRUNING_ENABLE || \
                                     (mode)==VTP_PRUNING_DISABLE)
#define is_pruning_mode_stopped(d)  (((d)->pruning_mode==VTP_PRUNING_DISABLE) \
                                     || ((d)->mode == VTP_TRANSPARENT))
#define is_pruning_mode_transparent(d) (((d)->mode == VTP_TRANSPARENT))
#define is_pruning_mode_enable(d)   (((d)->pruning_mode) == VTP_PRUNING_ENABLE)
#define is_pruning_mode_disable(d)  (((d)->pruning_mode) == VTP_PRUNING_DISABLE)

/* Interface functions defined in the core software */

extern void vtp_init(void);
extern void vtp_terminate(void);

extern Vtp_Err_t vtp_configure_domain(Vtp_Op_t mode,
					  vtp_domain_info* d_info);

extern Vtp_Err_t vtp_change_domain_name(unsigned char old_length,
					    unsigned char* old_name, 
					    unsigned char new_length,
					    unsigned char* new_name);

extern Vtp_Err_t vtp_set_update_id(unsigned char dname_length, 
				   unsigned char* domain_name,
				   unsigned long update_id);

/* numbers chosen to coincide with SR research functionality */
typedef enum {
    VTP_FIRST = 0x00,
    VTP_NAME,
    VTP_EXACT = 0xA0,
    VTP_NEXT = 0xA1
} Vtp_Search_t;

extern Vtp_Err_t vtp_get_domain_info(vtp_domain_info* d_info,
				     Vtp_Search_t type);

extern void vtp_report_load_failure(unsigned char*, unsigned char);

extern vtp_boolean vtp_test_database_busy(unsigned char dname_length, 
				      unsigned char* domain_name); 
extern void vtp_set_database_busy(unsigned char, unsigned char *);
extern void vtp_reset_database_busy(unsigned char, unsigned char *);
extern void vtp_delete_database(unsigned char, unsigned char *);

extern void vtp_receive_packet(void *, void *, unsigned long, 
			       unsigned char *); 

extern Vtp_Err_t vtp_configure_trunk(Vtp_Op_t mode,
				     vtp_trunk_info* t_info);

extern Vtp_Err_t vtp_get_trunk_info(vtp_trunk_info*,
				    Vtp_Search_t);
extern Vtp_Err_t vtp_get_trunk_by_domain(unsigned char dname_length, 
					 unsigned char* domain_name,
					 vtp_trunk_info* t_info_p,
					 Vtp_Search_t search_type);

extern unsigned long vtp_get_time_until_summary(void* trunk_id);  

extern Vtp_Err_t vtp_configure_vlan(unsigned char dname_length,
				    unsigned char* domain_name,
				    Vtp_Op_t mode,
				    vtp_vlan_info* v_info);

extern Vtp_Err_t vtp_get_vlan_info(unsigned char, unsigned char *,
				   vtp_vlan_info *, Vtp_Search_t);

extern void vtp_get_device_info(vtp_device_info *);
extern void vtp_set_max_storage(short size);
extern Vtp_Err_t vtp_upload_info (unsigned char, unsigned char *,
				  void **, unsigned long *); 
extern Vtp_Err_t vtp_download_info(vtp_boolean need_save, 
				   vtp_boolean from_network,
				   void* info); 

#define VTP_STATS_RELATIVE 1
#define VTP_STATS_FIXED    2
extern Vtp_Err_t vtp_get_statistics(unsigned char, unsigned char *,
				    unsigned long *, unsigned long *,
				    unsigned long *, unsigned long *,
				    unsigned long *, unsigned long *,
				    unsigned long *, unsigned long *, 
				    unsigned char); 

extern void vtp_clear_statistics(vtp_boolean, unsigned char,
				 unsigned char *); 

extern vtp_boolean vtp_is_default_vlan_id(unsigned short isl_vlan_id);

/* Interface functions defined in dependent software */

extern void* vtp_get_buffer(unsigned long);
extern void* vtp_buffer_duplicate(void *);
extern void vtp_free_buffer(void *);
extern void* vtp_buffer_payload(void *, void *);
extern void* vtp_create_timer(void (*func), void *);
extern void vtp_delete_timer(void *);
extern void vtp_start_timer(void *, unsigned long);
extern void vtp_stop_timer(void *);
extern vtp_boolean vtp_timer_active(void *);
extern unsigned long vtp_timer_time_left(void*);
extern unsigned long vtp_get_system_uptime(void);
extern void vtp_get_timestamp(unsigned char *);
extern vtp_boolean vtp_is_print(unsigned char*, unsigned char);
extern unsigned long vtp_format_4_bytes(unsigned long);
extern unsigned short vtp_format_2_bytes(unsigned short);

extern Vtp_Notif_Err_t vtp_store_domain_info(unsigned char, unsigned char*,
					     void*, unsigned long);
extern Vtp_Notif_Err_t vtp_domain_change_notification(Vtp_Op_t,
						      vtp_domain_info*,
						      vtp_domain_info*);
extern Vtp_Notif_Err_t vtp_vlan_change_notification(Vtp_Op_t,
						    unsigned char, 
						    unsigned char*,
						    vtp_vlan_info*,
						    vtp_vlan_info*);
extern Vtp_Notif_Err_t vtp_trunk_change_notification(Vtp_Op_t, 
						     vtp_trunk_info*,
						     vtp_trunk_info*);
extern void vtp_error_notification(unsigned char, unsigned char *,
				   Vtp_Err_t);

extern int vtp_transmit_packet(void *, void *, unsigned long);
extern void vtp_get_md5_digest(unsigned char *, unsigned long,
			       unsigned char *); 
extern void vtp_get_secret_value(unsigned char, unsigned char *,
				 unsigned char *); 
extern unsigned long vtp_get_random_number(void);
extern void vtp_process_reschedule(void);

extern void vtp_no_request_response(void);

#endif __VTP_INTERFACE_H__

