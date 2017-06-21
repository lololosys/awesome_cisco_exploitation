/* $Id: dlsw_csm_db.h,v 3.3.52.3 1996/05/31 14:26:22 ravip Exp $
 *------------------------------------------------------------------
 * DLSw CSM Database header file.
 *
 * August 1994, Ravi Periasamy 
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Contains data structure definitions used by dlsw_csm_db module
 *------------------------------------------------------------------
 * $Log: dlsw_csm_db.h,v $
 * Revision 3.3.52.3  1996/05/31  14:26:22  ravip
 * CSCdi58834:  4500/4000/7000 routers crash on show dlsw reachability
 * Branch: California_branch
 *
 * Revision 3.3.52.2  1996/05/30  14:43:28  sbales
 * CSCdi59064:  DLSw+: show commands need to be more usable
 *              modify options for "sh dls reach", "sh dls cir", "sh dls
 *              loc", modify formatting for "show dls peer"
 * Branch: California_branch
 *
 * Revision 3.3.52.1  1996/05/17  10:45:12  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.40.2  1996/05/15  00:38:55  ravip
 * CSCdi50707:  dlsw the reachability search is linear, should be binary
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.40.1  1996/04/03  14:01:12  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3  1995/11/29  19:45:11  ravip
 * CSCdi40989:  crash: router tried to reference free memory in dlsw code
 *              use mem_lock() to lock memory, to avoid other routines
 *              from deleting the memory block in use.
 *              Use a flag to tag actions that would change the links.
 *
 * Revision 3.2  1995/11/17  09:02:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:23:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:26:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __DLSW_CSM_DB_H__
#define __DLSW_CSM_DB_H__

#include "../util/avl.h"

#define MAX_DBNAME_SIZE        32
#define NO_CSMDB_SIZE_LIMIT    0

/* Currently CSMDB debug is tied to dlsw_csm_debug */
#define CSMDB_DEBUG        if (dlsw_csm_debug) buginf

typedef enum {
    CSMDB_NO_ERROR = 0,
    CSMDB_DUPLICATE_KEY = 1,
    CSMDB_DB_FULL = 2,
    CSMDB_KEY_NOT_FOUND = 3,
    CSMDB_INVALID_PARMS = 4
} csmdb_error_type;

typedef struct db_master_ {
    ulong             max_records;
    ulong             cur_records;
    ulong             added_records;
    boolean           duplicate_permitted;
    int               record_size;
    int               key_size;
    char              db_name[MAX_DBNAME_SIZE];
    avl_node_type     *top_node;
    struct db_master_ *this;
    struct db_master_ *next;
    } db_master_t;

typedef struct db_data_ {
    void            *data;
    struct db_data_ *prev;
    struct db_data_ *next;
    avl_node_type   *frecord;  
    boolean         del_flag;           /* set to TRUE when deleted      */
    } db_data_t;
typedef struct db_data_ *record_handle;


typedef struct record_info_ {
    avl_node_type node_info;   /* must be the first entry in this struct */
    db_master_t   *db_id;      /* database this node is in - for xref    */
    char          *key;
    db_data_t     *db_data;
    } record_info_t;

#define RECORD_KEY(x)    (get_record_key(x))


/* db function prototypes */
extern db_master_t *create_db (char *, ulong, int, int, boolean);
extern db_master_t *remove_db (db_master_t *);
extern record_handle seek_record (db_master_t *, char *, void **);
extern record_handle seek_nb_name_record (db_master_t *, char *, void **);
extern record_handle read_next_record (db_master_t *, record_handle, 
                                       char *, void **);
extern record_handle add_record (db_master_t *, char *, void *);
extern record_handle delete_record_get_next (db_master_t *, record_handle);
extern boolean match_record_key (db_data_t *, char *);
extern char *get_record_key(db_data_t *);
extern void *rhandle_to_data (db_data_t *bptr);

#endif    /* __DLSW_CSM_DB_H__ */


