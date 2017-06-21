/* $Id: dspu_cps.h,v 3.2 1996/03/05 03:25:22 bchan Exp $
 * $Source: /release/111/cvs/Xsys/dspu/dspu_cps.h,v $
 *------------------------------------------------------------------
 * DSPU Child/Parent Services structures.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 * 
 * This defines the structure of (a) sibling block(s).
 *------------------------------------------------------------------
 * $Log: dspu_cps.h,v $
 * Revision 3.2  1996/03/05  03:25:22  bchan
 * CSCdi49448:  DSPU only allows 256 downstream PUs
 *
 * Revision 3.1  1995/11/09  11:28:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/20  17:02:46  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __CPS_H                         /* Allow multiple inclusion. */
#define __CPS_H

#include "dtypes.h"

#define MAX_SIBLINGS 16
#define BAD_SIBLING_INDEX 0xffff

/*---- Exported structures. */

typedef enum SiblingType {SINGLE = 1,     /* ls <-> pu */
                          MULTIPLE = 16,  
                          DSPU_INDEX = 64  /* downstream pu, max = 64*16 = 1024 */
                   } SiblingType;

/* Generic function CPS proper type declaration. To allow CPS  service
* have proper type. Each corresponding objects must defined a template.
* first. The following defines 2 types: Parent_t, Child_t
*/

#ifdef LU_TEMPLATE 
typedef Pu* Parent_t;
typedef void* Child_t;
#else
#ifdef PU_TEMPLATE 
typedef LinkStn* Parent_t;
typedef Lu* Child_t;
#else
#ifdef LS_TEMPLATE 
typedef Dspu* Parent_t;      /* down/up stream parent */
typedef Pu*  Child_t;
#else
#ifdef SAP_TEMPLATE 
typedef Adaptor* Parent_t;
typedef LinkStn*  Child_t;
#else
#ifdef ADAPTOR_TEMPLATE 
typedef Dspu* Parent_t;
typedef Sap*  Child_t;
#else
typedef void* Parent_t;
typedef void* Child_t;
#endif
#endif
#endif
#endif
#endif

typedef struct {
        word inuse;                     /* Indices in use. */
        Child_t idxP[MAX_SIBLINGS];       /* Location of index or data. */
        } SiblingIndex;


typedef struct {                        /* Sibling block. */
        SiblingType sibtype;            /* number of siblings. */
        Child_t idxP[MAX_SIBLINGS];         /* Location of index or data. */
        } Sibling;

typedef struct {                        /* Sibling block. */
        SiblingType sibtype;            /* Single  */
        Child_t idxP[SINGLE];                   /* Location of child. */
        } SingleChild;                /* single child       */

typedef struct {                        /* Sibling block. */
        SiblingType sibtype;            /* number of siblings. */
        Child_t idxP[DSPU_INDEX];         /* Location of index or data. */
        } DspuChild;                   /* Down stream PU        */

typedef void (*ProcChild)(Child_t);       /* Procedure to do all children. */
typedef void (*ProcChild1)(Child_t, hword); /* Procedure to do all children. */
typedef void (*ProcChild2)(Child_t, void*); /* Procedure to do all children. */
typedef boolean (*ProcQChild)(Child_t);
typedef boolean (*ProcQChild1)(Child_t, void*);

/*---- Exported interfaces. */

boolean                                 /* Success of initialization. */
InitChild (SiblingType sibType,         /* Single/Multiple siblings. */
           Sibling *childLocP);         /* Ptr to child locator structure. */

void *                                  /* Addr of child found. */
GetChild (Sibling *childLocP,           /* Ptr to child locator structure. */
          hword cindex);                /* Index of child of interest. */

void *                                  /* Addr of child found. */
GetFirstChild (Sibling *childLocP);     /* Ptr to child locator structure. */

void *                                  /* Addr of child found. */
GetNextChild (Sibling *childLocP,       /* Ptr to child locator structure. */
              hword cindex);            /* Index of child to start from. */

hword                                   /* Index where child added. */
AddChildFirstFree (Sibling *childLocP,  /* Ptr to child locator structure. */
                   void *childP,        /* Addr of struct to add. */
                   Parent_t parentP,     /* Ptr to parent of child. */
                   Parent_t* addrParentP); /* Address of ptr to parent in child. */

boolean                                 /* Success of addition. */
AddChild (Sibling *childLocP,           /* Ptr to child locator structure. */
          hword cindex,                 /* The actual child. */
          void *childP,                 /* Location of child. */
          Parent_t parentP,              /* Ptr to parent of child. */
          Parent_t* addrParentP);          /* Address of ptr to parent in child. */

boolean                                 /* Success of deletion. */
DelChild (Sibling *childLocP,           /* Ptr to child locator structure. */
          hword cindex,                 /* The child to delete. */
          Parent_t* addrParentP);          /* Address of ptr to parent in child. */

void
DoToAllChildren (Sibling   *this,       /* Ptr to child locator structure. */
                 ProcChild proc);       /* Procedure to execute in children. */

void
DoToAllChildrenParm (Sibling   *this,   /* Ptr to child locator structure. */
                     ProcChild1 proc,   /* Procedure to execute in children. */
                     hword param);      /* Parameter used w/ procedure. */

void
DoToAllChildrenParm2 (Sibling   *this,  /* Ptr to child locator structure. */
                      ProcChild2 proc,  /* Procedure to execute in children. */
                      void *parmP);     /* Parameter used w/ procedure. */

void*
QueryAllChildren (Sibling   *this,      /* Ptr to child locator structure. */
                  ProcQChild proc);     /* Procedure to execute in children. */

void*
QueryAllChildrenParm (Sibling   *this,  /* Ptr to child locator structure. */
                      ProcQChild1 proc,  /* Procedure to execute in children. */
                      void *parmP);     /* Addr of structure used w/ proc. */

boolean QueryAnyChild(Sibling* this);


#define GetSingleChild(cP) ((cP)->idxP[0])
/* Child_t GetSingleChild(Sibling* this) {return this->childP;} */
/*---- End include file: cps.h. */
#endif  /* __CPS_H. */

