/* $Id: ospf_dbase.h,v 3.2.60.2 1996/04/29 10:02:51 mdb Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/ospf_dbase.h,v $
 *------------------------------------------------------------------
 * ospf_dbase.h -- OSPF database related variables and definitions 
 *
 * October 4 1990, Phanindra Jujjavarapu
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ospf_dbase.h,v $
 * Revision 3.2.60.2  1996/04/29  10:02:51  mdb
 * The 96q1 gcc is much more particular about putting parentheses around
 * comparisons for some operands ("&", "^" and "|") [CSCdi56015].
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/04/16  19:01:44  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:35:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:09:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/10/13  01:30:30  myeung
 * CSCdi42048:  Boolean in OSPF db structure should be trimmed
 * - Replace db_sum_origin, db_where and other booleans with
 *   bit field variables.
 *
 * Revision 2.7  1995/10/12  11:50:53  myeung
 * CSCdi42047:  Clear up OSPF db flags
 * - Replace bit mask with bit field variable.
 *
 * Revision 2.6  1995/10/08  23:44:25  myeung
 * CSCdi41790:  Boolean db_free in db structure should be replaced by bit
 * field
 * - Backout the patch
 * - Decide to use bit variable instead of bit mask to replace boolean
 *
 * Revision 2.5  1995/10/08  22:49:37  myeung
 * CSCdi41790:  Boolean db_free in db structure should be replaced by bit
 * field
 *
 * Revision 2.4  1995/09/25  11:49:46  myeung
 * CSCdi40976:  OSPF should send back most recent copy when old lsa is
 * received
 *
 * Revision 2.3  1995/08/01  15:15:54  myeung
 * CSCdi36150:  OSPF database synchronization problem after route recovers
 * - Always take care of the elasped time when calculating LSA age
 * - The maximum age used for comparsion between LSA should be 3600
 *
 * Revision 2.2  1995/06/28  03:21:29  hampton
 * Use the passive timer macros instead of direct references to the system
 * clock.  [CSCdi36502]
 *
 * Revision 2.1  1995/06/07 21:09:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Storage
 */

#define HTBLSIZE        277
#define HTBLMOD		(HTBLSIZE)  /* sb prime (or there aboutst) */

#define XHASH(A1,A2)	(((A1 >> 2) ^ (A2 >> 3)) % HTBLMOD)

typedef struct sl_link_ {
   struct sl_link_ **prev;
   struct sl_link_ *next;
   ulong key;
} sl_link;

/*
 * structure for representing a path to the root
 * equal cost paths to root are represented by 
 * linked list of path blocks.
 */
typedef struct pathtype_ {
    struct pathtype_	*pb_next;	/* next equal cost path */
    lsdbtype		*pb_parent_ptr;	/* pointer to parent in spf tree */
    lsdbtype		*pb_firsthop_ptr;/* points to one of root's children */
    idbtype		*pb_firsthop_idb; /* idb to use */
    ipaddrtype		pb_gateway;  /* interface address of next hop */
} pathtype;

/*
 * structure to maintain list
 * of neighbors on retransmission
 * list for a db.
 */
typedef struct nbr_list_ { 
    struct nbr_list_ *list_next;
    struct nbrtype_ *list_nbr;
    short list_refcount; 
} nbr_list;

typedef struct lsidnodetype_ {
    treeLink  link;		/* link for the RBTree. It must be 
				   the first element in this structure.
				   note : link.key is used to store
				   the primary key - lsid */
    sl_link   *sl_list;		/* sorted list to db advertised by
				   different router */
    rbTree   *tree_belong;	/* point to the tree this node is on
				   so that db_free() can take only one
				   arg */
} lsidnodetype;

STRUCTURE_REF(ospf_worktype);

struct lsdbtype_ { 
    sl_link  link;      	/* sorted list link - it must be the
				   first element in this structure.
				   note : link.key is used to store 
				   the second key - adv router */
    lsidnodetype *lsidnode;     /* pointer to its own tree */
    lsdbtype *db_next;		/* used to link it in lists */
    lsdbtype *db_prev;		/* used to link it in lists */
    lsdbtype *db_rnext;		/* used to link it in delete lists */
    lsdbtype *db_rprev;		/* used to link it in delete lists */
    lsdbtype *db_mnext;		/* to link maxaged in delete lists */
    lsdbtype *db_mprev;		/* to link maxaged in delete lists */
    ospf_worktype *db_work;	/* point to the work description block */
    ulong      db_dist;		/* distance to root */
    ipaddrtype db_dest;		/* destination address for this db */
    ipaddrtype db_mask;		/* destination mask */
    pathtype  *db_path_list;	/* list of equal cost paths */
    pathtype  *db_dpath_list;	/* list delete paths from routing table */
    sys_timestamp db_time;	/* for keeping age - stamped when arrived */
    lsatype   *db_lsa_ptr;	/* pointer to advertisements */
    nbr_list  *db_retrans;	/* nbrs pointing to this lsdb */
    ulong      db_forw_dist;	/* used for external links */
    sys_timestamp db_free_time;	/* Last time this db is freed */
    ushort     db_ase_count;	/* nexthop for self-originated ase lsa */
    ushort     db_pdbindex;   	/* pdbindex that generated this */
    ushort     db_pdbdist;     	/* pdbdistance that generated this */
    ushort     db_ackcnt;	/* ack reference count - don't free until 0 */
    uchar      db_path_count;	/* number of equal cost paths */
    /* 
     * db flags 
     */
    uchar      db_flood_dly:1; 	/* flooding of lsa is delayed */
    uchar      db_rt_table:1;	/* db has contribute entry in routing table */
    uchar      db_ase_use:1;
    uchar      db_bdr_rtr:1;
    uchar      db_won_redist:1;
    uchar      db_ar_range:1;
    uchar      db_in_maxageq:1;	/* db is on MAXAGE queue */
    uchar      db_dest_match_ar_range:1;
    uchar      db_mask_change:1;/* db mask has change */	
    uchar      db_freeme:1;	/* flag to indicate to free this db */
    uchar      db_direct:1;	/* is the route directly connected */
    uchar      db_abr_flag:1;	/* For summary LSA, FALSE means this LSA 
				   should be advertised into another area at
				   SUM_LS_INFINITY metric.  TRUE means metric
				   will be determined in build_inter_route */
    uchar      db_sum_origin_intra:1;	/* meaningful for summary LSA, 
					   how is this db is generated. 
					   TRUE means INTRA or INTER
					   otherwise */
    uchar      db_where:2;	/* where this vertex is: */
    uchar      db_do_not_age:1;	/* DoNotAge bit */
    uchar      db_lsa_changed:1;	/* This LSA content change */
    uchar      db_rtr_reachable:1;	/* For type 1 and 4 LSA to indicate
					   that the router is reachable */
    uchar      db_partial:1;	/* Wait on partial processing Q */
    /*
     * reasons for not freeing db
     */
    uchar      db_nf_freeme:1;
    uchar      db_nf_retrans:1;
    uchar      db_nf_nbr_ex_load:1;
    uchar      db_nf_rt_table:1;
    uchar      db_nf_flood_dly:1;
    uchar      db_nf_ack_cnt:1;
    uchar      db_nf_dbase_lock:1;
    uchar      db_nf_in_maxageq:1;
    uchar      db_nf_partial:1;
};

#define	LSDB_UNINIT	0
#define	LSDB_CLIST	1
#define LSDB_STUB	2
#define LSDB_SPFTREE	3

#define DO_NOT_AGE	(0x8000)

#define DB_ELAPSE(db)	((ELAPSED_TIME((db)->db_time))/ONESEC)

#define ADV_AGE(db)     ((db)->db_lsa_ptr->lsh_age + DB_ELAPSE(db))

#define ADV_RTR(db) 	((db)->db_lsa_ptr->lsh_adv_rtr)

#define ROUTER_ID(area) ((area)->ar_pdb_ptr->ospf_rtr_id)

#define LSA_ID(db) 	((db)->db_lsa_ptr->lsh_id)

#define LSA_TYPE(db) 	((db)->db_lsa_ptr->lsh_type)

#define STUB_NET(db)	(LSA_TYPE(db) == LSA_TYPE_STUB)

#define REAL_AGE(db)    ( (ADV_AGE(db) >= MAXAGE) ? MAXAGE : ADV_AGE(db)) 
#define OSPF_BASESEQ	(0x80000000)	/* host order */
#define OSPF_FIRSTSEQ	((-OSPF_BASESEQ) + 1)
#define OSPF_MAXSEQ	(OSPF_BASESEQ - 1)

#define INVALID_LSA_TYPE(type)	(((type) < LSA_TYPE_RTR) || \
				 ((type) > LSA_TYPE_7_ASE) || \
				 ((type) == LSA_TYPE_IGNORE))
/*
 * returns True if A is greater than B
 */
#define SEQCMP(A,B) ((A) > (B))

#define NEXTSEQ(S) (((S) == OSPF_MAXSEQ) ?  (OSPF_FIRSTSEQ) : ((S) + 1))

#define OMAX(A,B) (((A) > (B)) ? (A) : (B))
#define OMIN(A,B) (((A) < (B)) ? (A) : (B))
#define ODIFF(A,B) (OMAX((A),(B)) - OMIN((A),(B)))

#define VALID_AGE(age)	(((age) >= MAXAGE) ? MAXAGE : (age))

/* the logic here is 
 *  #define MORE_RECENT(A,B,Elapse) \
 *   ( T ||                  A's seq no. > B's   OR
 *     ( U && 		        A and B Seq no. are equal  AND
 *       ( (V && W) ||              checksum not equal and A's > B's  OR
 *	    X       ||       	    A's age is MAXAGE and B's is not  OR
 *	   (Y && Z) 		    diff in age is more than MAXDIFFAGE
 *	 )			     AND A's is smaller (younger)
 *     )
 *   )
 */
#define MORE_RECENT(A,B,Elapse) \
    ((SEQCMP((A)->lsh_seq, (B)->lsh_seq)) || \
      (((B)->lsh_seq == (A)->lsh_seq) && \
	 (((A)->lsh_chksum != (B)->lsh_chksum &&\
            ((A)->lsh_chksum > (B)->lsh_chksum)) ||\
      (((A)->lsh_age == MAXAGE) && \
       (VALID_AGE((B)->lsh_age + Elapse) != MAXAGE)) ||\
       ((ODIFF(VALID_AGE((B)->lsh_age + Elapse), ((A)->lsh_age)) > \
	 MAXDIFFAGE) && \
          ((A)->lsh_age < VALID_AGE((B)->lsh_age + Elapse))))))

#define SAME_INSTANCE(A,B,Elapse)\
        ( ((A)->lsh_seq == (B)->lsh_seq) &&\
          ((A)->lsh_chksum == (B)->lsh_chksum) &&\
          (!(((A)->lsh_age == MAXAGE) ^ \
	     (VALID_AGE((B)->lsh_age + Elapse) == MAXAGE))) &&\
          (ODIFF(VALID_AGE((B)->lsh_age + Elapse),((A)->lsh_age)) <= \
	   MAXDIFFAGE) )

#define REQ_MORE_RECENT(A,B) \
    ((SEQCMP((A)->req_seq, (B)->lsh_seq)) || \
      (((B)->lsh_seq == (A)->req_seq) && \
	 (((A)->req_chksum != (B)->lsh_chksum &&\
            ((A)->req_chksum > (B)->lsh_chksum)) ||\
       (((A)->req_age == MAXAGE) && ((B)->lsh_age != MAXAGE)) ||\
      ((ODIFF((B)->lsh_age,((A)->req_age)) > MAXDIFFAGE) && \
          ((A)->req_age < (B)->lsh_age)))))


#define REQ_SAME_INSTANCE(A,B)\
	( ((A)->req_seq == (B)->lsh_seq) &&\
	  ((A)->req_chksum == (B)->lsh_chksum) &&\
          (!(((A)->req_age == MAXAGE) ^ ((B)->lsh_age == MAXAGE))) &&\
	  (ODIFF(((B)->lsh_age),((A)->req_age)) <= MAXDIFFAGE) )





