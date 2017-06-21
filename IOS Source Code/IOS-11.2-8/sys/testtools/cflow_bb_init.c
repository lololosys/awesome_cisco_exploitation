/* $Id: cflow_bb_init.c,v 3.3 1996/01/31 18:24:02 lakshman Exp $
 * $Source: /release/111/cvs/Xsys/testtools/cflow_bb_init.c,v $
 * ------------------------------------------------------------------
 * __bb_init_func builds a linked list of bb structures
 * 
 * June-9 1994, Lakshmankumar Mukkavilli
 * 
 * Copyright (c) 1993-1996 by cisco Systems, Inc. All rights reserved.
 * 
 * __bb_init_func is called when a module is entered, if the
 * initialised flag, * which is local to each module, is not set.
 * The name __bb_init_func is * chosen by the compiler. Compiler
 * generates code to call this function.  * bb_list_head points to
 * a linked list of bb structs which point to tables * containing
 * execution counts for instrumented arcs in each module. This
 * file should NEVER be instrumented for coverage.
 * 
 * ------------------------------------------------------------------
 * $Log: cflow_bb_init.c,v $
 * Revision 3.3  1996/01/31  18:24:02  lakshman
 * CSCdi47664:  Augment cflow with tracing capability
 *
 * Revision 3.2  1995/11/17  18:50:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:36:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:07:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * ------------------------------------------------------------------
 * $Endlog$
 */

#include "../testtools/cflow.h"

struct bb      *bb_list_head;

void
  __bb_init_func (struct bb * blocks)
{
    /*
     * User is supposed to check whether the first word is non-0, but
     * just in case....
     */
    
    if (blocks->zero_word)
      return;
    
    blocks->next = bb_list_head;
    bb_list_head = blocks;
    blocks->zero_word = 1;
}
