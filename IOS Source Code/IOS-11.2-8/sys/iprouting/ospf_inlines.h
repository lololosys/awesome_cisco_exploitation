/* $Id$
 * $Source$
 *------------------------------------------------------------------
 * ospf_inlines.h - Inline routines for OSPF
 *
 * Feburary 1997, Derek M. Yeung
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log$
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "logger.h"			/* Required for next include */
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "../os/chunk.h"

extern chunk_type *ospf_lsdb_list_chunks;
extern chunk_type *ospf_nbr_list_chunks;
extern chunk_type *ospf_work_chunks;

/*
 * ospf_lsdb_list_malloc
 */
static inline lsdb_list *ospf_lsdb_list_malloc (void)
{
    lsdb_list *list;

    list = chunk_malloc(ospf_lsdb_list_chunks);
    if (list)
	list->list_refcount = 1;

    return list;
}

/*
 * ospf_lsdb_list_lock
 */
static inline void ospf_lsdb_list_lock (lsdb_list *list)
{
    if (list->list_refcount < 1) {
	errmsg(&msgsym(BADSHARE, SYS), "ospf_lsdb_list_lock", list,
	       list->list_refcount);
	list->list_refcount = 1;
	return;
    }
    list->list_refcount++;
}

/*
 * ospf_lsdb_list_free
 */
static inline void ospf_lsdb_list_free (lsdb_list *list)
{
    if (!list)
	return;

    if (list->list_refcount < 1) {
	errmsg(&msgsym(BADSHARE, SYS), "ospf_lsdb_list_lock", list,
	       list->list_refcount);
	return;
    }
    if (--list->list_refcount)
	return;
    chunk_free(ospf_lsdb_list_chunks, list);
}

/*
 * ospf_nbr_list_malloc
 */
static inline nbr_list *ospf_nbr_list_malloc (void)
{
    nbr_list *list;

    list = chunk_malloc(ospf_nbr_list_chunks);
    if (list)
	list->list_refcount = 1;

    return list;
}

/*
 * ospf_nbr_list_lock
 */
static inline void ospf_nbr_list_lock (nbr_list *list)
{
    if (list->list_refcount < 1) {
	errmsg(&msgsym(BADSHARE, SYS), "ospf_nbr_list_lock", list,
	       list->list_refcount);
	list->list_refcount = 1;
	return;
    }
    list->list_refcount++;
}

/*
 * ospf_nbr_list_free
 */
static inline void ospf_nbr_list_free (nbr_list *list)
{
    if (!list)
	return;

    if (list->list_refcount < 1) {
	errmsg(&msgsym(BADSHARE, SYS), "ospf_nbr_list_lock", list,
	       list->list_refcount);
	return;
    }
    if (--list->list_refcount)
	return;
    chunk_free(ospf_nbr_list_chunks, list);
}

/*
 * ospf_work_malloc
 */
static inline ospf_worktype *ospf_work_malloc (void)
{
    ospf_worktype *work;

    work = chunk_malloc(ospf_work_chunks);
    if (work)
	work->ow_refcount = 1;

    return work;
}

/*
 * ospf_work_lock
 */
static inline void ospf_work_lock (ospf_worktype *work)
{
    if (work->ow_refcount < 1) {
	errmsg(&msgsym(BADSHARE, SYS), "ospf_work_lock", work,
	       work->ow_refcount);
	work->ow_refcount = 1;
	return;
    }
    work->ow_refcount++;
}

/*
 * ospf_work_free
 */
static inline void ospf_work_free (ospf_worktype *work)
{
    if (!work)
	return;

    if (work->ow_refcount < 1) {
	errmsg(&msgsym(BADSHARE, SYS), "ospf_work_lock", work,
	       work->ow_refcount);
	return;
    }
    if (--work->ow_refcount)
	return;
    chunk_free(ospf_work_chunks, work);
}

