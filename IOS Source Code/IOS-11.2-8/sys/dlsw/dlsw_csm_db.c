/* $Id: dlsw_csm_db.c,v 3.6.20.4 1996/06/28 23:05:41 hampton Exp $
 *------------------------------------------------------------------
 * DLSw DLX - DataBase routines for CSM
 *
 * August 1994, Ravi Periasamy 
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Contain the database routines used by CSM to handle information
 *------------------------------------------------------------------
 * $Log: dlsw_csm_db.c,v $
 * Revision 3.6.20.4  1996/06/28  23:05:41  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.6.20.3  1996/05/31  14:26:20  ravip
 * CSCdi58834:  4500/4000/7000 routers crash on show dlsw reachability
 * Branch: California_branch
 *
 * Revision 3.6.20.2  1996/05/17  10:45:09  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.6.3  1996/05/15  00:38:53  ravip
 * CSCdi50707:  dlsw the reachability search is linear, should be binary
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.6.6.2  1996/04/26  07:33:58  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.6.6.1  1996/04/03  14:01:11  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.6.20.1  1996/03/18  19:31:33  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.6.2  1996/03/07  08:44:14  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.6.1  1996/02/20  00:45:39  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/01  06:01:10  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/12  17:31:14  ravip
 * CSCdi46357:  clean up dlsw csm debug messages
 *
 * Revision 3.4  1995/11/29  19:45:04  ravip
 * CSCdi40989:  crash: router tried to reference free memory in dlsw code
 *              use mem_lock() to lock memory, to avoid other routines
 *              from deleting the memory block in use.
 *              Use a flag to tag actions that would change the links.
 *
 * Revision 3.3  1995/11/17  09:02:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:21:27  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:23:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  20:55:12  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/06/21  08:54:29  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.2  1995/06/18  21:21:20  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:26:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "types.h"
#include "string.h"
#include "globs.h"
#include "../h/ieee.h"
#include <ciscolib.h>
#include "../dlsw/dlsw_csm_db.h"


/*
 * Global variables
 */
db_master_t      *db_list = NULL;
static csmdb_error_type csmdb_errno = 0;
static int db_data_size = 0;
static int record_info_size = 0;

/*
 * Function decl.
 */
int csmdb_delete_all_records (db_master_t *);
static enum avl_compare csmdb_key_compare (avl_node_type *, avl_node_type *);
static record_info_t *find_record (db_master_t *, char *);
static long get_word_aligned_size (long size);


/*
 * create database
 *
 * db_name     - Name of the database
 * max_records - Maximum number of records to store in db
 * record_size - size of each record
 * key_size    - size of keY
 * duplicate   - TRUE if duplicate key allowed
 *               FALSE if duplicate NOT allowed
 * key_compare_func - function pointer to compare two keys
 *
 */
db_master_t *create_db (char *db_name, ulong max_records, 
                        int record_size, int key_size,
                        boolean duplicate)
{

    db_master_t    *tmp_db_list = NULL, *db_ptr = NULL;

    csmdb_errno = 0;
    db_ptr = malloc(sizeof(db_master_t));
    if (db_ptr == NULL) {
        return NULL;
    }

    db_ptr->max_records = 0;
    db_ptr->cur_records = 0;
    db_ptr->added_records = 0;
    db_ptr->record_size = record_size;
    db_ptr->key_size = key_size;
    db_ptr->duplicate_permitted = duplicate;
    strncpy(db_ptr->db_name, db_name, (MAX_DBNAME_SIZE -1)); 
    db_ptr->db_name[MAX_DBNAME_SIZE] = 0;
    db_ptr->this = db_ptr;
    db_ptr->next = NULL;
    db_ptr->top_node = NULL;

    /* attach to the end of db_list */
    if (db_list) {
        tmp_db_list = db_list;
        while(tmp_db_list->next != NULL) {
            tmp_db_list = tmp_db_list->next;
        }
        tmp_db_list->next = db_ptr;
    } else {
        db_list = db_ptr;
    }

    /* 
     * setting up the following info is done here, for want of an
     * initialization routine
     */
    if (db_data_size == 0) {
        db_data_size = get_word_aligned_size(sizeof(db_data_t));
    }
    if (record_info_size == 0) {
        record_info_size = get_word_aligned_size(sizeof(record_info_t));
    }

    return db_ptr;

}


#ifdef NOBODY_CALLS_ME
/*
 * remove database
 */
db_master_t *remove_db (db_master_t *db_id)
{
    db_master_t     *tmp_ptr;

    csmdb_errno = 0;
    if ((db_id == NULL) || (db_id->this != db_id)) {
	CSMDB_DEBUG("\nCSMDB: parameter error %d, remove_db", __LINE__);
        return NULL;
    }

    if (db_list == NULL) {
        /* no more db to delete */
        return NULL;
    }
    csmdb_delete_all_records(db_id);
    if (db_id == db_list) {
        if (db_id->next == NULL) {
            db_list = NULL;
        } else {
            db_list = db_id->next;
        }
    } else {
        tmp_ptr = db_id;
        while ((tmp_ptr != NULL) && (tmp_ptr->next != db_id)) {
            tmp_ptr = tmp_ptr->next;
        }
        if (tmp_ptr) {
            tmp_ptr->next = db_id->next;
        } else {
	    CSMDB_DEBUG("\nCSMDB: parameter error %d", __LINE__);
            return NULL;
        }
    }
    free (db_id);
    return (db_id);

}
#endif


/*
 * to get a record with a specific key
 * for internal use. Assumes that the parameters are already checked
 */
static record_info_t *find_record (db_master_t *db_num, char *key_str)
{
    record_info_t     goal;

    goal.key = key_str;
    return ((record_info_t *) avl_search(db_num->top_node, 
                                         (avl_node_type *)&goal,
                                         csmdb_key_compare));

}


/*
 * seek record for a given key
 * Gets the first instance for the given key
 */
record_handle seek_record (db_master_t *db_num, char *key_str, void **db_data)
{
    record_info_t     *result;
    db_data_t         *ws_bptr;

    csmdb_errno = 0;
    if ((db_num == NULL) || (db_num->this != db_num)) {
	CSMDB_DEBUG("\nCSMDB: parameter error %d, seek_record1", __LINE__);
        return NULL;
    }

    if ((key_str == NULL) || (db_data == NULL)) {
	CSMDB_DEBUG("\nCSMDB: parameter error %d, seek_record2", __LINE__);
	return NULL;
    }

    result = find_record(db_num, key_str);
    if (result) {
        ws_bptr = result->db_data;
        *db_data = ws_bptr->data;
    } else {
        *db_data = NULL;
        ws_bptr = NULL;
    }
    return (ws_bptr);

}


/*
 * Used only for seeking netbios name record for 
 *   show dlsw reachablility netbios-name xxxxx exec command
 *
 * Searches the record with pad characters if needed. 
 */
record_handle seek_nb_name_record(db_master_t *db_num, char *match, void **cache_ptr)
{  
  char *match_key, try;
  int db_key_size, match_key_size;
  record_handle rhandle;

  match_key = string_getnext();
  match_key_size = strlen(match);
  db_key_size = db_num->key_size;
         
  if (match_key_size < db_key_size) {
    for (try = 0; try < 2; try++) {
      memcpy(match_key, match, match_key_size);
      if (try == 0)
        memset(match_key + match_key_size, 0x20, db_key_size - match_key_size); /* 0x20 is ' ' */
      else
        memset(match_key + match_key_size, 0x00, db_key_size - match_key_size);
      rhandle = seek_record(db_num, match_key, cache_ptr);
      if (rhandle) 
        return (rhandle);
    }
  }
  else {
    memcpy(match_key, match, db_key_size);
    rhandle = seek_record(db_num, match_key, cache_ptr);
    if (rhandle) 
      return (rhandle);
  }

  return NULL;

}


/* 
 * Read the record next to the pointer given. for the given key.
 */
record_handle read_next_record (db_master_t *db_num, 
                                db_data_t *bptr, 
                                char *key_str, 
                                void **db_data)
{
    record_info_t     *tmp_record;
    db_data_t         *ws_bptr;

    csmdb_errno = 0;
    if ((db_num == NULL) || (db_num->this != db_num)) {
	CSMDB_DEBUG("\nCSMDB: parameter error %d, read_next_record", __LINE__);
        return NULL;
    }

    ws_bptr = NULL;
    tmp_record = NULL;
    if (bptr) {
        ws_bptr = bptr->next;
        if ((ws_bptr == NULL) && (key_str == NULL)) {
            tmp_record = (record_info_t *) bptr->frecord;
            if (tmp_record == NULL) {
                /* the record was probably deleted .... */
                return NULL;
            }
            tmp_record = (record_info_t *) 
                            avl_get_next(db_num->top_node, 
                                         (avl_node_type *) tmp_record,
                                         csmdb_key_compare);
            if (tmp_record) {
                ws_bptr = tmp_record->db_data;
            }
        }
    } else {
        if (key_str) {
            tmp_record = find_record(db_num, key_str);
        } else {
            tmp_record = (record_info_t *) avl_get_first(db_num->top_node);
        }
        if (tmp_record) {
            ws_bptr = tmp_record->db_data;
        }
    }

    if (db_data) {
        if (ws_bptr) {
            *db_data = ws_bptr->data;
        } else {
            *db_data = NULL;
        }
    }

    return (ws_bptr);
}


/*
 * given a number, get the next higher word aligned number
 */
static long get_word_aligned_size (long size)
{
    int      bal;

    bal = size % sizeof(long);
    if (bal) {
        size = size - bal + sizeof(long);
    }
    return(size);
}


/*
 * add a record
 */
record_handle add_record (db_master_t *db_num, char *key_str, void *db_data)
{
    record_info_t *new_record, *tmp_record;    
    db_data_t     *new_node, *tmp_node;
    boolean       balancing_needed;

    csmdb_errno = 0;
    if ((db_num == NULL) || (db_num->this != db_num)) {
	CSMDB_DEBUG("\nCSMDB: parameter error %d, add_record1", __LINE__);
        return NULL;
    }
    if ((db_data == NULL) || (key_str == NULL)) {
	CSMDB_DEBUG("\nCSMDB: parameter error %d, add_record2", __LINE__);
        return NULL;
    }

    new_node = malloc(db_data_size + db_num->record_size);
    if (new_node == NULL) {
        return NULL;
    }
    new_node->data = ((char *) new_node) + db_data_size;
    memcpy(new_node->data, db_data, db_num->record_size);
    new_node->prev = NULL;
    new_node->next = NULL;
    new_node->frecord = NULL;
    new_node->del_flag = FALSE;

    /* check if the record already exists */
    tmp_record = find_record(db_num, key_str);
    if (tmp_record) {
        if (db_num->duplicate_permitted) {
            new_node->frecord = (avl_node_type *) tmp_record;
            tmp_node = tmp_record->db_data;
            if (tmp_node) {
                while(tmp_node->next != NULL) {
                    tmp_node = tmp_node->next;
                }
                new_node->prev = tmp_node;
                tmp_node->next = new_node;
            } else {
                tmp_record->db_data = new_node;
            }
            ++db_num->cur_records;
            ++db_num->added_records;
        } else {
            free(new_node);
            return NULL;
        }
    } else {
        new_record = malloc(record_info_size + db_num->key_size);
        if (new_record == NULL) {
            free(new_node);
            return NULL;
        }
        new_record->key = (char *) new_record + record_info_size;
        new_record->db_id = db_num;
        memcpy(new_record->key, key_str, db_num->key_size);
        new_record->db_data = new_node;
        new_node->frecord = (avl_node_type *) new_record;
        balancing_needed = TRUE;
        tmp_record = (record_info_t *) avl_insert(&db_num->top_node, 
                                (avl_node_type *)new_record, 
                                &balancing_needed, csmdb_key_compare);
        if (tmp_record == NULL) {
            CSMDB_DEBUG("\nCSMDB: Error adding record to database");
            free(new_node);
            free(new_record);
            return NULL;
        }
        ++db_num->cur_records;
        ++db_num->added_records;
    }

    return (new_node);
}


/*
 * Delete a record at the given location
 * return pointer the the next record. NULL if no more records in db
 */
record_handle delete_record_get_next (db_master_t *db_num, db_data_t *bptr)
{
    record_info_t *tmp_record, *next_record;    
    db_data_t     *tp, *tn;
    boolean       balancing_needed;

    csmdb_errno = 0;
    if ((db_num == NULL) || (db_num->this != db_num) || (bptr == NULL)) {
	CSMDB_DEBUG("\nCSMDB: parameter error %d, delete_record", __LINE__);
        return NULL;
    }

    tmp_record = (record_info_t *) bptr->frecord;
    if (tmp_record == NULL) {
        /* error */
        return NULL;
    }
    tp = bptr->prev;
    tn = bptr->next;

    if (tp != NULL) {
        tp->next = tn;
    } else {        /* top of list */
        tmp_record->db_data = tn;
    }
    if (tn != NULL) {
        tn->prev = tp;
    } 

    --db_num->cur_records;

    /*
     * db_data is freed automagically when we free
     * bptr - the malloc was done consolidating this.
     * set the prev and next pointers to NULL ... in case this
     * piece of memory was locked.
     */
    bptr->prev = NULL;
    bptr->next = NULL;
    bptr->del_flag = TRUE;
    bptr->frecord = NULL;
    free(bptr);

    if (tn == NULL) {
        next_record = (record_info_t *) 
                            avl_get_next(db_num->top_node, 
                                         (avl_node_type *) tmp_record,
                                         csmdb_key_compare);
        if (next_record) {
            tn = next_record->db_data;
        } 
    }
        
    /*
     * if this was the only record for the key, delete the record from
     * the tree
     */
    if (tmp_record->db_data == NULL) {
        balancing_needed = TRUE;
        tmp_record = (record_info_t *) avl_delete(&db_num->top_node, 
                                (avl_node_type *)tmp_record, 
                                &balancing_needed, csmdb_key_compare);
        if (tmp_record == NULL) {
            CSMDB_DEBUG("\nCSMDB: Error deleting record from database");
        } else {
            free(tmp_record);
        }
    }
    return tn;
}


/*
 * functions to compare two keys of various types
 */
static enum avl_compare csmdb_key_compare (avl_node_type *node1, avl_node_type *node2)
{
    int             rc;
    record_info_t   *rec1, *rec2;

    rec1 = (record_info_t *) node1;
    rec2 = (record_info_t *) node2;
    rc = memcmp(rec1->key, rec2->key, rec1->db_id->key_size);
    if (rc == 0) {
        return AVL_EQ;
    } else if (rc > 0) {
        return AVL_GT;
    } else {
        return AVL_LT;
    }
}



/*
 * find if the current record's key matches the key
 * return TRUE if match else return FALSE
 */
boolean match_record_key (db_data_t *node, char *key)
{
    record_info_t     *rec;

    if ((key == NULL) || (node == NULL)) {
        return FALSE;
    }

    rec = (record_info_t *) node->frecord;
    if (!(memcmp(rec->key, key, rec->db_id->key_size))) {
        return TRUE;
    } else {
        return FALSE;
    }

}



int csmdb_delete_all_records (db_master_t *db_id)
{

    record_info_t     *rec;
    db_data_t         *bptr;

    rec = (record_info_t *) db_id->top_node;
    bptr = rec->db_data;

    while(bptr) {
        bptr = delete_record_get_next(db_id, bptr);
    }
    return 0;

}



char *get_record_key (db_data_t *bptr)
{
    record_info_t    *record_info;

    record_info = (record_info_t *) bptr->frecord;
    if (record_info) {
        return(record_info->key);
    } else {
        return NULL;
    }
}


void *rhandle_to_data (db_data_t *bptr)
{
 
    if (bptr) {
        return bptr->data;
    } else {
        return NULL;
    }
 
}
 
 
