/* $Id: tree.h,v 3.4 1995/12/08 04:56:06 dstine Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/util/tree.h,v $
 *------------------------------------------------------------------
 * Red/black tree structure subroutines
 *
 * September 1991, David Edwards
 *
 * Copyright (c) 1991-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tree.h,v $
 * Revision 3.4  1995/12/08  04:56:06  dstine
 * CSCdi45233:  Namespace breakage for RBTree
 *         Just normalize all RBTree* names.
 *
 * Revision 3.3  1995/12/07  22:33:12  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.2  1995/11/17  17:56:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:43:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:34:09  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/06/10  12:24:13  dwong
 * CSCdi35099:  clear apple neighbor is not working
 *
 * Revision 2.1  1995/06/07  23:13:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __TREE_H__
#define __TREE_H__

/*******************************************************************
 * Trees
 *******************************************************************/

/*
 * Interval Tree data structures
 */

/*
 * The following enum is used to control how we look at the key in an RB tree.
 */
typedef enum {
    by_interval = 1,			/* 16 bit high, 16 bit low */
    by_long,				/* 32-bit long */
    by_longlong,			/* 64-bit long */
    by_idb_address,			/* idb MSByte, 24-bit address */
    by_void				/* oblique 32-bit value */
} treeKeyView;

/*
 * The interval structure - Handles two short keys which form a range
 */
typedef struct interval_ { 
    ushort high; 
    ushort low; 
} interval;

/*
 * The treeKey structure - Handles keys which are long, ranges, or ptrs
 */
typedef union treeKey_ {
    ulonglong ll;
    ulong l;
    interval i;
    void* p;
} treeKey;

/*
 * Color of a node.
 */
typedef enum nodeColor_ {
    RedNode,
    BlackNode
} nodeColor;

/*
 * The TreeLink structure - Tree node structure
 *
 * Note: Freed nodes are removed from tree and placed on free list since
 *       they may not be freed until busy is zero.  When deleting nodes,
 *	 p,l,r are set to NULL so that any operation performed on this node
 *       is safe. (This is why f is a separate link pointer.)
 */
typedef struct treeLink_ {
    struct treeLink_* parent;		/* Parent link */
    struct treeLink_* left;		/* Left link */
    struct treeLink_* right;		/* Right link */
    struct treeLink_* freeList;		/* Free link */
    struct treeLink_* nextl;		/* Next link */
    struct treeLink_* prevl;		/* Previous link */
    treeKey key;			/* Key values */
    treeKey subkey;			/* Max. value of subtree (incl this) */
    ushort size;			/* # of nodes in subtree (incl this) */
    nodeColor color;			/* Node color */
    ushort busy;			/* Node access count */
} treeLink;

typedef boolean (*treeProc)(treeLink*,void*);
typedef void    (*treeKeyPrint)(char*, treeKey);
typedef void    (*treeKeyMax)(treeKey*, treeKey, treeKey);
typedef int     (*treeKeyCompare)(treeKey, treeKey);

/*
 * The RedBlackTree structure - Tree global data
 *
 * Note: The active node count does not include nodes on the free list.
 */
struct rbTree_ {
    struct treeLink_* root;		/* Root of tree, possibly NULL */
    struct treeLink_* freeList;		/* List of free nodes */
    struct treelist_ *listhead;		/* head of list of nodes */
    char* lastErr;			/* Ptr to last err from TreeValid */
    sys_timestamp errTimer;			/* Time of last err report */
    ushort activeNodes;			/* Count of tree nodes */
    ushort freeNodes;			/* Count of nodes on free list */
    ushort addedNodes;			/* Count of node inserts */
    ushort deletedNodes;		/* Count of node deletes */
    ushort rotatedNodes;		/* Count of node rotates */
    ushort splicedNodes;		/* Count of node splices */
    ushort trimmedNodes;		/* Count of node trimmed (freed to os) */
    ushort maxNodeDepth;		/* Maximum node depth */
    /*
     * Per-tree statistics.
     */
    ulong findRequests;			/* # of finds were done */
    ulong findAccessed;			/* #nodes accessed for a find */
    ulong findDeepest;
    ulong findIRequests;		/* # of Interval finds done */
    ulong findIAccessed;		/* # of Interval nodes accessed */
    ulong findIDeepest;
    ulong gcRuns;			/* # of times tree has been GC'd */

    /*
     *
     */
    treeKeyView view;			/* How do we look at the keys? */
    treeKeyPrint print_key;		/* Routine to print keys */
    boolean intervalkey;		/* Indicates how node info s/b kept */
    boolean thirtytwobitkey;		/* Key size is <= 32 bits */
    treeKeyMax keymax;			/* Routine to set max of two keys */
    treeKeyCompare keycompare;		/* Routine to compare keys */
    char* protoName;			/* Protocol name for debugging */
    char* protoAbbrev;			/* Protocol name for debugging */
    char* tableName;			/* Table name for debugging */
    char* fullName;			/* Full name for memory label */
    boolean *debug_flag;		/* Which debugging flag to use */
};


/*******************************************************************
 * tree.c
 *******************************************************************/
rbTree * 	RBTreeCreate(char *, char *, char *, treeKeyPrint, boolean *);
void		RBTreeNon32Bit(rbTree* , treeKeyMax, treeKeyCompare);
rbTree *	RBTreeDestroy(rbTree*, boolean *);
boolean 	RBTreeValid(rbTree* T,boolean quiet,char const * title);
void 		RBTreeStats(rbTree* T, char* name);
void		RBTreePrintKeyLong(char *, treeKey);
void 		RBTreePrint(treeLink*, ulong, rbTree*);
boolean 	RBTreeNodeDeleted(rbTree*, treeLink*);
treeLink * 	RBTreeLexiNode(rbTree* T,treeKey key);
treeLink * 	RBTreeBestNode(rbTree* T,treeKey key);
treeLink * 	RBTreeSearch(rbTree* T,treeKey key);
treeLink * 	RBTreeInsert(treeKey key, rbTree*, treeLink*);
treeLink * 	RBTreeIntSearch(rbTree* T,treeKey key);
treeLink * 	RBTreeIntInsert(ushort low,ushort high, rbTree*, treeLink*);
treeLink * 	RBTreeDelete(rbTree*,treeLink*);
treeLink *	RBTreeGetFreeNode(rbTree*);
treeLink *	RBTreeScavengeFreeNode(rbTree* , treeLink*);
boolean		RBTreeAddToFreeList(rbTree*,treeLink*);
boolean		RBTreeForEachNode(treeProc,void*,rbTree*,boolean);
boolean		RBTreeForEachNodeTilFalse(treeProc,void*,rbTree*,treeLink*,boolean);
boolean 	RBTreeNodeProtected(treeLink*);
treeLink * 	RBTreeFirstNode(rbTree* T);
treeLink * 	RBTreeNextNode(treeLink* x);
treeLink * 	RBTreeGetFreeNode(rbTree* T);
boolean		RBTreeTrimFreeList(rbTree* T);
int 		RBTreeReleasedNodeCount (rbTree* T);
int 		RBTreeFreeNodeCount (rbTree* T);
void 		RBTreeKeyMaxLongLong(treeKey*, treeKey, treeKey);
int 		RBTreeKeyCompareLongLong(treeKey, treeKey);
void		RBTreePrintKeyLongLong(char*, treeKey);
treeLink *	RBTreeGetNIL(void);

#ifdef NO_TREE_INLINES
boolean		RBTreeNodeProtect(treeLink *, boolean);
#else
static inline boolean RBTreeNodeProtect(treeLink* l,boolean flg) {
    if (flg)
	l->busy++;
    else if (l->busy > 0)
	l->busy--;
    return (l->busy > 0);
}
#endif

#endif /* __TREE_H__ */
