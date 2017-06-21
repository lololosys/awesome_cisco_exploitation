/* $Id: radix.h,v 3.3.54.5 1996/08/06 08:28:25 gchristy Exp $
 * $Source: /release/112/cvs/Xsys/util/radix.h,v $
 *------------------------------------------------------------------
 * Radix Trie manipulation package
 *
 * January 1993, Paul Traina
 *
 * A small portion of this code is
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * See below for additional copyright notices.
 *
 * This file contains public (and private) definitions and function
 * prototypes from the 4.4BSD radix trie package.
 *
 *------------------------------------------------------------------
 * $Log: radix.h,v $
 * Revision 3.3.54.5  1996/08/06  08:28:25  gchristy
 * CSCdi64974:  IP Cache: delayed invalidation can create inconsistancies
 * Branch: California_branch
 *  - Back out changes made for CSCdi55725.
 *
 * Revision 3.3.54.4  1996/06/28  23:31:32  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.3.54.3  1996/06/24  16:05:47  myeung
 * CSCdi55725:  OSPF CPU HOG
 * Branch: California_branch
 * - Modify IP cache invalidation code to allow delayed invalidation
 *
 * Revision 3.3.54.2  1996/03/28  22:27:59  rchandra
 * CSCdi52880:  BGP: generate error message for unsupported route-map
 * options
 * Branch: California_branch
 * - generate error message when unsupported route-map option is used
 * - tread confederation EBGP neighbors similar to internal neighbors in
 *   best path selection
 * - fix the bug in bgp_compare_med, so that during MED comparison,
 *   confederation information is skipped properly
 * - confederation EBGP paths should be inserted into the IP table with
 *   internal distance
 * - do not consider common-AS neighbors as confederation neighbors
 * - during 'clear ip bgp dampening' avoid skipping paths
 * - undo the patch to the radix version walker
 *
 * Revision 3.3.54.1  1996/03/18  22:30:50  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.20.2  1996/03/07  11:00:03  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.20.1  1996/02/20  21:45:13  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/12/02  00:57:55  rchandra
 * CSCdi44862:  BGP: accessing some fields of battrtype causes alignment
 * fixups
 * - rearrange the fields of battrtype structure to be long word aligned
 * - fix rn_nextleaf_version to check the childs version number during
 *   tree walk.
 *
 * Revision 3.2  1995/11/17  17:56:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:43:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:34:06  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/08/24  22:34:49  bcole
 * CSCdi39279:  NHRP: Aggregation needed to provide purge notification
 *
 * Provide aggregated NHRP responses.
 * Cache now consists of radix tree(s) instead of hash table.
 * Allow NHRP resolution during encapsulation of NHRP purge messages.
 * For IP, NHRP now a routing process so as to receive callbacks.
 * Allow for radix tree walk where varargs have already been pre-collected.
 *
 * Revision 2.1  1995/06/07  23:13:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Copyright (c) 1988, 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)radix.h	8.1 (Berkeley) 6/10/93
 */

#ifndef _RADIX_H_
#define	_RADIX_H_

#ifdef	__STDC__
#define	__P(X) X
#else
#define	__P(X)
#endif

/*
 * Size of maximum sized key, plus length, plus 1 byte slop
 *
 * Currently set to 4 to match size of IP address, change at will,
 * do not reference other .h files!
 */
#define	RN_MAX_KEYLEN	(4+1+1)

/*
 * Radix search tree node layout.
 */

struct radix_node {
	struct	radix_mask *rn_mklist;	/* list of masks contained in subtree */
	struct	radix_node *rn_p;	/* parent */
	short	rn_b;			/* bit offset; -1-index(netmask) */
	char	rn_bmask;		/* node: mask for bit test*/
	u_char	rn_flags;		/* enumerated next */
#define RNF_NORMAL	1		/* leaf contains normal route */
#define RNF_ROOT	2		/* leaf is root leaf for tree */
#define RNF_ACTIVE	4		/* This node is alive (for rtfree) */
	union {
		struct {			/* leaf only data: */
			caddr_t	rn_Key;	/* object of search */
			caddr_t	rn_Mask;	/* netmask, if present */
			struct	radix_node *rn_Dupedkey;
		} rn_leaf;
		struct {			/* node only data: */
			int	rn_Off;		/* where to start compare */
			struct	radix_node *rn_L;/* progeny */
			struct	radix_node *rn_R;/* progeny */
		}rn_node;
	}		rn_u;
	u_long rn_version;		/* version of data in subtree */
#ifdef RN_DEBUG
	int rn_info;
	struct radix_node *rn_twin;
	struct radix_node *rn_ybro;
#endif
};

#define rn_dupedkey rn_u.rn_leaf.rn_Dupedkey
#define rn_key rn_u.rn_leaf.rn_Key
#define rn_mask rn_u.rn_leaf.rn_Mask
#define rn_off rn_u.rn_node.rn_Off
#define rn_l rn_u.rn_node.rn_L
#define rn_r rn_u.rn_node.rn_R

/*
 * Annotations to tree concerning potential routes applying to subtrees.
 */

extern struct radix_mask {
	short	rm_b;			/* bit offset; -1-index(netmask) */
	char	rm_unused;		/* cf. rn_bmask */
	u_char	rm_flags;		/* cf. rn_flags */
	struct	radix_mask *rm_mklist;	/* more masks to try */
	union	{
		caddr_t	rmu_mask;		/* the mask */
		struct	radix_node *rmu_leaf;	/* for normal routes */
	}	rm_rmu;
	int	rm_refs;		/* # of references to this struct */
} *rn_mkfreelist;

#define rm_mask rm_rmu.rmu_mask
#define rm_leaf rm_rmu.rmu_leaf		/* extra field would make 32 bytes */

#define MKGet(m) {\
	if (rn_mkfreelist) {\
		m = rn_mkfreelist; \
		rn_mkfreelist = (m)->rm_mklist; \
	} else \
		R_Malloc(m, struct radix_mask *, sizeof (*(m))); }\

#define MKFree(m) { (m)->rm_mklist = rn_mkfreelist; rn_mkfreelist = (m);}

typedef int (*rn_walk_function)(struct radix_node *, va_list ap);
typedef struct radix_node * (*rn_succ_function)(struct radix_node *, caddr_t);
typedef struct radix_node * (*rn_succ_ver_function)(struct radix_node *,
						    u_long, caddr_t);
#define	RN_TREE_AREA	(2*sizeof(rntype))

struct radix_node_head {
	struct	radix_node *rnh_treetop;
#ifdef	notusedyet
	int	rnh_addrsize;		/* permit, but not require fixed keys */
	int	rnh_pktsize;		/* permit, but not require fixed keys */
#endif
#ifdef	RADIX_HEAD_VECTORS
	struct	radix_node *(*rnh_addaddr)	/* add based on sockaddr */
		__P((void *v, void *mask,
		     struct radix_node_head *head, struct radix_node nodes[]));
	struct	radix_node *(*rnh_addpkt)	/* add based on packet hdr */
	        __P((void *v, void *mask,
		     struct radix_node_head *head, struct radix_node nodes[]));
	struct	radix_node *(*rnh_deladdr)	/* remove based on sockaddr */
		__P((void *v, void *mask, struct radix_node_head *head));
	struct	radix_node *(*rnh_delpkt)	/* remove based on packet hdr */
		__P((void *v, void *mask, struct radix_node_head *head));
	struct	radix_node *(*rnh_matchaddr)	/* locate based on sockaddr */
		__P((void *v, struct radix_node_head *head));
	struct	radix_node *(*rnh_lookup)	/* locate based on sockaddr */
		__P((void *v, void *mask, struct radix_node_head *head));
	struct	radix_node *(*rnh_matchpkt)	/* locate based on packet hdr */
		__P((void *v, struct radix_node_head *head));
	int	(*rnh_walktree)			/* traverse tree */
		__P((struct radix_node *rn, rn_walk_function f, ...));
	void	(*rnh_mark_parents)		/* color tree */
		__P((struct radix_node *rn, u_long version));
	int	(*rnh_walktree_version)		/* traverse colored tree */
		__P((struct radix_node *rn, u_long version,
		     rn_walk_function f, ...));
	struct	radix_node *(*rnh_search_m)	/* masking direct search */
		__P((void *v, struct radix_node *head, void *m));
#endif
	struct	radix_node rnh_nodes[3];	/* empty tree for common case */
};


#ifndef KERNEL
#define Bcmp(a, b, n) bcmp(((char *)(a)), ((char *)(b)), (n))
#define Bcopy(a, b, n) bcopy(((char *)(a)), ((char *)(b)), (unsigned)(n))
#define Bzero(p, n) memset((p), 0, (int)(n));
#define R_Malloc(p, t, n) (p = (t) malloc((unsigned int)(n)))
#define Free(p) free(p);
#else
#define Bcmp(a, b, n) bcmp(((caddr_t)(a)), ((caddr_t)(b)), (unsigned)(n))
#define Bcopy(a, b, n) bcopy(((caddr_t)(a)), ((caddr_t)(b)), (unsigned)(n))
#define Bzero(p, n) memset((p), 0, (unsigned)(n));
#define R_Malloc(p, t, n) (p = (t) malloc((unsigned long)(n), M_RTABLE, M_DONTWAIT))
#define Free(p) free(p, M_RTABLE);

#endif /*KERNEL*/

int    rn_inithead __P((void **, int));
int    rn_walktree __P((struct radix_node *rn, rn_walk_function f, ...));
struct radix_node
       *rn_addroute __P((void *, void *, struct radix_node_head *,
                      struct radix_node [2])),
       *rn_delete __P((void *, void *, struct radix_node_head *)),
       *rn_match_unsafe __P((void *, struct radix_node_head *)),
       *rn_match __P((void *, struct radix_node_head *)),
       *rn_lookup __P((void *, void *, struct radix_node_head *)),
       *rn_search __P((void *, struct radix_node *)),
       *rn_search_m __P((void *, struct radix_node *, void *));

void   rn_mark_parents      __P((struct radix_node *rn, u_long version));
int    rn_walktree_blocking __P((struct radix_node *rn, 
			         rn_walk_function f, ...));
int    rn_walktree_blocking_list __P((struct radix_node *rn, 
			         rn_walk_function f, va_list ap));
int    rn_walktree_version  __P((struct radix_node_head *head, u_long version,
			         rn_walk_function f,
				 rn_succ_ver_function s, ...));
int    rn_walktree_timed    __P((struct radix_node_head *,
			         rn_walk_function f,
				 rn_succ_function s, ...));

#undef	__P

/*
 * rn_nextleaf
 *
 * Find the next leaf node in the tree given the current node.
 */
static inline struct radix_node *
rn_nextleaf (struct radix_node *rn)
{
	/* If at right child go back up, otherwise, go right */
	while (rn->rn_p->rn_r == rn && (rn->rn_flags & RNF_ROOT) == 0)
	    rn = rn->rn_p;

	/* Find the next *leaf* since next node might vanish, too */
	for (rn = rn->rn_p->rn_r; rn->rn_b >= 0;)
	    rn = rn->rn_l;

	return (rn);
}

/*
 * rn_nextleaf_version
 *
 * Find the next leaf node in the tree given the current node.
 */
static inline struct radix_node *
rn_nextleaf_version (struct radix_node *rn, u_long version)
{
	/* If at right child go back up, otherwise, go right */
	while (rn->rn_p->rn_r == rn && (rn->rn_flags & RNF_ROOT) == 0)
	    rn = rn->rn_p;

	/* Find the next *leaf* since next node might vanish, too */
	for (rn = rn->rn_p->rn_r; rn->rn_b >= 0;) {
	    if (rn->rn_version < version)
		rn = rn->rn_r;
	    else
		rn = rn->rn_l;
	}

	return (rn);
}

#endif /* _RADIX_H_ */
