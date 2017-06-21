/* $Id: reent_init.c,v 3.1.10.1 1996/03/18 20:48:52 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/reent/reent_init.c,v $
 *------------------------------------------------------------------
 * reent_init.c -- initialize a reentrancy block
 *
 * January 1996, Mark D. Baushke
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: reent_init.c,v $
 * Revision 3.1.10.1  1996/03/18  20:48:52  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:19  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  03:15:53  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        reent_init---create an initialize a reentrancy block

ANSI_SYNOPSIS
        #include <reent.h>
        void *reent_init();

DESCRIPTION
        Reentrancy is a characteristic of library functions which
        allows multiple processes to use the same address space with
        assurance that the values stored in those spaces will remain
        constant between calls.

RETURNS
        reent_init returns a pointer to an initialized reentrancy
        block for this process/thread/context.

PORTABILITY
        reent_init is not ANSI C.
*/
#include <reent.h>
#include "reent_private.h"
#include <stdlib.h>
#include <string.h>

static _reent_t _reent_block = {
    NULL, 0, NULL, 0,
    { '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
      '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
      '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
      '\0', '\0' }
};



static void
_reent_set (_reent_t *rp)
{
    rp->_errno = 0;
    rp->_scanpoint = NULL;
    rp->_next = 0;
    memset(rp->_asctime, '\0', sizeof(rp->_asctime));
}

void *
_reent_init (void)
{
    _reent_t *rp;

    rp = malloc(sizeof(_reent_t));
    if (rp != NULL) {
        _reent_set(rp);
        rp->_rp = _reent_block._rp;
        _reent_block._rp = rp;
    }
    else {
        /*
         * XXX Fix me: Should probably print an error message, the
         * problem is that this is probably running in a bad context
         * to do so.
         */
        rp = &_reent_block;     /* malloc failed -- use last resort */
    }
    return((void *)rp);
}

void
_reent_cleanup (void *blockptr)
{
    _reent_t *rp;
    _reent_t *tmp;

    rp = blockptr;

    if (rp != NULL && rp != &_reent_block) {
        /*
         * look for rp in list of allocated blocks
         */
        for (tmp = &_reent_block; tmp != NULL; tmp = tmp->_rp) {
            if (rp == tmp->_rp) {
                /*
                 * found rp, clean it up and free it
                 */
                tmp->_rp = tmp->_rp->_rp;
                rp->_rp = NULL;
                _reent_set(rp);
                free(rp);
            }
        }
    }
}
