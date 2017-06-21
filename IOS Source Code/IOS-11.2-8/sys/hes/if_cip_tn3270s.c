/* $Id: if_cip_tn3270s.c,v 3.1.4.20 1996/09/02 08:19:17 bchan Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_cip_tn3270s.c,v $
 *------------------------------------------------------------------
 * if_cip_tn3270s.c Channel Interface Processor TN3270 Server support
 *
 * September 1995, Karen Mitchell
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_cip_tn3270s.c,v $
 * Revision 3.1.4.20  1996/09/02  08:19:17  bchan
 * CSCdi67808:  Change tn3270-server mib
 * Branch: California_branch
 *
 * Revision 3.1.4.19  1996/08/29  01:48:59  mkirk
 * CSCdi62250:  Change maximum-lus to 2100; add license and threshold
 * messages
 * Branch: California_branch
 *
 * Revision 3.1.4.18  1996/08/28  12:44:50  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.4.17  1996/08/16  06:14:34  bchan
 * CSCdi66094:  add show lu history for tn3270 server
 * Branch: California_branch
 *
 * Revision 3.1.4.16  1996/08/07  08:59:02  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.1.4.15  1996/08/07  08:28:47  bchan
 * CSCdi64701:  crash when doing getmany tn3270sGlobalTable
 * Branch: California_branch
 *
 * Revision 3.1.4.14  1996/08/07  07:35:34  bchan
 * CSCdi64701:  crash when doing getmany tn3270sGlobalTable
 * Branch: California_branch
 *
 * Revision 3.1.4.13  1996/07/17  06:52:03  bchan
 * CSCdi63198:  Fix appn/dlur mib for tn3270 server
 * Branch: California_branch
 *
 * Revision 3.1.4.12  1996/07/15  08:27:34  bchan
 * CSCdi62509:  Change tn3270 server mib
 * Branch: California_branch
 *
 * Revision 3.1.4.10  1996/06/27  04:52:07  mkirk
 * CSCdi59893:  TN3270 Server DLUR SHOW commands incomplete/incorrect
 * Branch: California_branch
 *
 * Revision 3.1.4.9  1996/06/19  19:28:33  snyder
 * CSCdi60807:  spelllink errors
 * Branch: California_branch
 *         fixed it once, I'll fix it again, sigh.
 *
 *         hierachical -> hierarchical
 *
 * Revision 3.1.4.8  1996/06/17  23:45:11  mkirk
 * CSCdi59333:  Fixed tn3270 SHOW looping when tcp-port=65535
 * Branch: California_branch
 *
 * Revision 3.1.4.7  1996/06/05  21:11:17  ronnie
 * CSCdi39527:  wavl_walk calls avl_walk with wrong parameter
 * Branch: California_branch
 *
 * Revision 3.1.4.6  1996/06/03  04:48:49  bchan
 * CSCdi57469:  change tn3270smib and show tn3270
 * Branch: California_branch
 *
 * Revision 3.1.4.5  1996/05/29  04:35:04  mkirk
 * CSCdi50956:  The same idblk/idnum of dlur pu didnt get rejected
 * Branch: California_branch
 *
 * Revision 3.1.4.4  1996/05/28  07:44:26  bchan
 * CSCdi57469:  change tn3270smib and show tn3270
 * Branch: California_branch
 *
 * Revision 3.1.4.3  1996/05/28  06:21:35  bchan
 * CSCdi58128:  add timing mark configuration and change keepalive
 * Branch: California_branch
 *
 * Revision 3.1.4.2  1996/05/22  18:09:09  snyder
 * CSCdi58423:  spellink errors
 * Branch: California_branch
 *
 * hierachical -> hierarchical
 *
 * Revision 3.1.4.1  1996/05/17  11:17:59  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.3  1996/05/12  08:10:04  bchan
 * CSCdi57469:  change tn3270smib and show tn3270
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.2.2  1996/03/28  11:28:39  bchan
 * CSCdi52724:  merge tn3280 server code
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.2.1  1996/03/21  03:21:37  bchan
 * CSCdi52147:  Add tn3270 server new files to IbuMod_Calif_branch
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1  1996/03/20  07:01:40  bchan
 * *** empty log message ***
 *
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "if_cip.h"
#include "if_cip_vlan.h"
#include "../if/channel.h"
#include "logger.h"
#include "../hes/msg_cbus.c"		/* Not a typo, see logger.h */
#include "../if/network.h"
#include "config.h"
#include "parser.h"
#include "../os/free.h"
#include "../parser/parser_defs_channel.h"
#include "../ui/debug.h"
#include "../if/channel_debug.h"
#include "if_cip_tn3270s.h"
#include "if_tn3270s.h"
#include "channel_sb.h"
#include "../ibm/libibm_externs.h"
#include "../h/ieee.h"
#include "if_cmd.h"
#include "../util/wavl.h"
#include "../iprouting/route.h"
#include "../ip/ip_registry.h"
#include "../ip/ip.h"
#include "sr_tn3270servermibdefs.h"
#include "sr_snanaumibdefs.h"
#include "sysmgt_snanaumib.h"
#include "stdarg.h"
#include "../snmp/snmp_api.h"

#define DEBUG_TN3270S        TRUE


/*
 * -----------------------------------------------------------------------------
 * TN3270 Server Data Structures
 * -----------------------------------------------------------------------------
 */
typedef struct tn3270s_max_lu_ {
    ulong    correlator;
    boolean  use_default;
    ushort   val;
} tn3270s_max_lu_t;

typedef struct tn3270s_timing_mark_ {
    ulong    correlator;
    boolean  val;
} tn3270s_timing_mark_t;

typedef struct siftdown_ {
    boolean       use_default;
    ushort        val;
} siftdown_t;

typedef struct tn3270s_siftdown_ {
    ulong      correlator;
    siftdown_t tcp_port;
    siftdown_t idle_time; 
    siftdown_t keepalive; 
    siftdown_t unbind_act;
    siftdown_t generic_pool;
} tn3270s_siftdown_t;

typedef struct tn3270s_lu_dns_ {
    ulong    correlator;
    boolean  use_default;
    char     dns_fqdn[MAX_FQDN_SIZE+1];
    char     rev_dns_fqdn[MAX_FQDN_SIZE+1];
} tn3270s_lu_dns_t;

typedef struct tn3270s_svr_t_ {
    ulong              correlator;
    ushort             state;
    tn3270s_timing_mark_t timing_mark;
    tn3270s_max_lu_t   max_lu;
    tn3270s_lu_dns_t   lu_dns;
    tn3270s_siftdown_t siftdown;
} tn3270s_svr_t;

typedef struct tn3270s_pu_dns_ {
    ulong    correlator;
    boolean  use_default;
    char     name[MAX_FQDN_SIZE+1];
} tn3270s_pu_dns_t;

enum {
    AVL_PU_ASCENDING	= 0,
    AVL_PU_DESCENDING	= 1,
    NUM_PU_AVL_THREADS
};

enum tn3270s_putype_t_ {
    TN3270S_PUTYPE_DIRECT,
    TN3270S_PUTYPE_DLUR
};
typedef enum tn3270s_putype_t_ tn3270s_putype_t;

struct tn3270s_pu_ {
    wavl_node_type     wavl[NUM_PU_AVL_THREADS];
    ulong              correlator;
    ushort             state;
    tn3270s_putype_t   type;
    boolean            new;
    char               name[MAX_NAME_SIZE+1];
    ulong              index;
    ulong              xid;
    ipaddrtype         ipaddr;
    ushort             vc;
    ushort             lan_type;
    ushort             adapno;
    uchar              lsap;
    uchar              rmac[IEEEBYTES];
    uchar              rsap;
    uchar              lu_seed[MAX_NAME_SIZE+1];
    tn3270s_pu_dns_t   pu_dns;
    tn3270s_siftdown_t siftdown;
} ;

struct tn3270s_direct_pu_t_ {
    tn3270s_pu_t         *pPU;
};

struct tn3270s_dlur_pu_ {
    tn3270s_pu_t         *pPU;
};

typedef struct tn3270s_dlus_ {
    ulong    correlator;
    boolean  use_default;
    char     name[MAX_FQNN_SIZE+1];
} tn3270s_dlus_t;

typedef struct tn3270s_NNsvr_ {
    ulong    correlator;
    boolean  use_default;
    char     name[MAX_FQNN_SIZE+1];
} tn3270s_NNsvr_t;

typedef struct tn3270s_vrn_ {
    ulong    correlator;
    boolean  use_default;
    char     name[MAX_FQNN_SIZE+1];
} tn3270s_vrn_t;

enum {
    AVL_LINK_NAME_ASCENDING	= 0,
    AVL_LINK_SAPMAC_ASCENDING	= 1,
    NUM_LINK_AVL_THREADS
};

typedef struct tn3270s_link_ {
    wavl_node_type     wavl[NUM_LINK_AVL_THREADS];
    ulong              correlator;
    ushort             state;
    char               name[MAX_NAME_SIZE+1];
    uchar              rmac[IEEEBYTES];
    uchar              rsap;
    /* need either the lan_type, adapno of sap owner or pSAP */
    ushort             lan_type;
    ushort             adapno;
    void              *pSAP;
} tn3270s_link_t;

enum {
    AVL_SAP_ASCENDING	= 0,
    AVL_SAP_DESCENDING	= 1,
    NUM_SAP_AVL_THREADS
};

typedef struct tn3270s_dlur_sap_ {
    wavl_node_type     wavl[NUM_SAP_AVL_THREADS];
    ulong              correlator;
    ushort             state;
    ushort             lan_type;
    ushort             adapno;
    uchar              lsap;
    tn3270s_vrn_t      dlur_sap_vrn;
} tn3270s_dlur_sap_t;

typedef struct tn3270s_dlur_ {
    ulong                 correlator;
    ushort                state;
    char                  dlur_fq_cpname[MAX_FQNN_SIZE+1];
    char                  dlur_fq_dlusname[MAX_FQNN_SIZE+1];
    tn3270s_dlus_t        dlur_dlus_backup;
    tn3270s_NNsvr_t       dlur_nn_svr;
    tn3270s_dlur_pu_t     dlur_pu;
    wavl_handle           sap_tree;
    wavl_handle           link_tree;
} tn3270s_dlur_t;

typedef struct tn3270s_ipvc_t_ {
    ipaddrtype         ipaddr;
    ushort             vc;
    ushort             vc_users;
    ushort             sr_users;
    boolean            inuse;
} tn3270s_ipvc_t;

/*
 * VC, ipaddress mapping defines
 *      (min) 512+96  cip->vc_base (256*2) + csna VCs (96)
 *      (max) 512+255 cip->vc_base (256*2) + CIP_PER_INT_MAX_VC
 */
#define TN3270S_MIN_VC 608
#define TN3270S_MAX_VC 767
#define TN3270S_NUM_VC (TN3270S_MAX_VC - TN3270S_MIN_VC + 1)
#define TN3270S_NO_VC  (TN3270S_MAX_VC+1)
typedef struct tn3270s_ {
    ulong                 tn3270s_correlator;
    ulong                 tn3270sUpTime;
    boolean               pu_configured;
    wavl_handle           pu_tree;
    tn3270s_svr_t         svr;
    tn3270s_direct_pu_t   direct_pu;
    tn3270s_dlur_t        dlur;
    tn3270s_ipvc_t        vc_array[TN3270S_NUM_VC];
} tn3270s_t;


/*
 * ---------------------------------------------------------------------------
 * Local Variables and Defines
 * ---------------------------------------------------------------------------
 */
#define TN3270S_CORRELATOR_HUH  0
#define TN3270S_CORRELATOR_INIT 1

#define TN3270S_SNMP_INDEX_HUH  0
#define TN3270S_SNMP_INDEX_INIT 1

/*
 * State field defines
 */
#define TN3270S_STATE_UNUSED   0x0000    /* Used only to reset STATE          */
#define TN3270S_STATE_INUSE    0x0001    /*                                   */
#define TN3270S_STATE_CFGING   0x0002    /*                                   */
#define TN3270S_STATE_CREATE   0x0004    /*                                   */
#define TN3270S_STATE_UPDATE   0x0008    /*                                   */
#define TN3270S_STATE_DELETE   0x0010    /*                                   */
#define TN3270S_STATE_PNDING   0x0020    /*                                   */
#define TN3270S_STATE_ACKED    0x0040    /* unused by TN3270S                 */
#define TN3270S_STATE_VC_SR    0x0080    /* applies to PUs                    */
#define TN3270S_STATE_SHUT     0x0100    /*                                   */
#define TN3270S_STATE_CIP_SHUT 0x0200    /*                                   */

/* 
 * Parser anchor points for User Data Block
 * TN3270 Server does not use nested subcommand contexts
 * Therefore, only a single context pointer is needed
 */
#define UDB_TN3270S_CONTEXT 0  /* Uses the 1st element in udb[] */
#define CSB_PTR_TN3270S     1  /* Uses XXXOBJ(udb,1)            */

#define UDB_SVR  0
#define UDB_PU     1
#define UDB_DLUR   1
#define UDB_DLUR_PU  2
#define UDB_DLUR_SAP 2
#define TN3270S_MAX_SUBMODES 3

/* nvgen usage: SETOBJ(udb,x) and GETOBJ(udb,x) */
#define CSB_PTR_SVR  1
#define CSB_PTR_PU     2
#define CSB_PTR_DLUR   2
#define CSB_PTR_DLUR_PU  3
#define CSB_PTR_DLUR_SAP 3

#define TN3270S_SVR_PROMPT_BASE      "cfg-tn3270"
#define TN3270S_PU_PROMPT_BASE       "tn3270-pu"
#define TN3270S_DLUR_PROMPT_BASE     "tn3270-dlur"
#define TN3270S_DLUR_PU_PROMPT_BASE  "tn3270-dlur-pu"
#define TN3270S_DLUR_SAP_PROMPT_BASE "tn3270-dlur-lsap"

/* 
 * Array of prompt lines for sub command modes
 */
struct prompt_t_ {
    char prompt_line[MAXPROMPTLEN+1];
};
typedef struct prompt_t_ prompt_t;
prompt_t prompt_lines[TN3270S_MAX_SUBMODES];

char *const pTn3270sSvrPrompt = prompt_lines[UDB_SVR].prompt_line;
char *const pTn3270sPuPrompt = prompt_lines[UDB_PU].prompt_line;
char *const pTn3270sDlurPrompt = prompt_lines[UDB_DLUR].prompt_line;
char *const pTn3270sDlurPuPrompt = prompt_lines[UDB_DLUR_PU].prompt_line;
char *const pTn3270sDlurSapPrompt = prompt_lines[UDB_DLUR_SAP].prompt_line;

static char *const nv_indent[TN3270S_MAX_SUBMODES] = {
    " ",
    "  ",
    "   "
};

/*
 * Router-wide TN3270 Server variables
 */
static idblist_t tn3270s_idb_list = IDBLIST_MAX;
ulong     tn3270s_snmp_pu_index;

static char odd_sap_reject_msg[] = "\nOdd values of sap are not legal;Please use Even values only";


/*
 *  The CIP tn3270 server uses "ff00.0000.0000" to designate a
 *  loopback mac address.
 */
#define MAX_MACADDR_STRING_SIZE 14    /* Size when printed in %e format */
char macaddr_string[MAX_MACADDR_STRING_SIZE+1];
static uchar const loopback_mac[IEEEBYTES] = {0xff,0x00,0x00,0x00,0x00,0x00};

/* forward declaration */

typedef enum lu_detail {
        TN3270S_NO_LU_DETAIL,
        TN3270S_LU_DETAIL,
        TN3270S_LU_DETAIL_HISTORY
}lu_detail;

static void tn3270s_show_lu(idbtype           *swidb,
			 cip_idb_infotype  *cip,
			 tn3270s_t         *pTn3270s, 
			 tn3270s_show_lu_t *pShowLU,
			 lu_detail            detail);


/*
 * ===========================================================================
 * Primitive data type operations
 *   PU wavl tree
 *   SAP wavl tree
 *   Link wavl tree
 *   IPVC array 
 * ===========================================================================
 */
/*
 * ---------------------------------------------------------------
 * PU wavl tree operations
 * ---------------------------------------------------------------
 */
/*
 *  Convert a wavl pointer to the entry type which contains it.  Because
 *  these are guaranteed to be the same, we just do a cast.  This keeps
 *  the code rigorously typed.
 */

static inline tn3270s_pu_t *
wavl_to_pu (wavl_node_type *wavl)
{
    return (tn3270s_pu_t *)wavl;
}

static inline wavl_node_type *
pu_to_wavl (tn3270s_pu_t *pPU)
{
    return (wavl_node_type *)pPU;
}

/*
 * ---------------------------------------------------------------
 * comparison routines: pu tree
 * ---------------------------------------------------------------
 */
static enum avl_compare
compare_puname_asc (avl_node_type *n1, avl_node_type *n2)
{
    tn3270s_pu_t *node1;
    tn3270s_pu_t *node2;
    int compare;
    
    node1 = wavl_to_pu(wavl_normalize(n1, AVL_PU_ASCENDING));
    node2 = wavl_to_pu(wavl_normalize(n2, AVL_PU_ASCENDING));
    
    compare = strcmp(node1->name, node2->name);
    if (compare < 0)
	return(AVL_LT);
    if (compare > 0)
	return(AVL_GT);
    return(AVL_EQ);
}

static enum avl_compare
compare_puname_desc (avl_node_type *n1, avl_node_type *n2)
{
    tn3270s_pu_t *node1;
    tn3270s_pu_t *node2;
    int compare;
    
    node1 = wavl_to_pu(wavl_normalize(n1, AVL_PU_DESCENDING));
    node2 = wavl_to_pu(wavl_normalize(n2, AVL_PU_DESCENDING));

    compare = strcmp(node1->name, node2->name);
    if (compare < 0)
	return(AVL_GT);
    if (compare > 0)
	return(AVL_LT);
    return(AVL_EQ);
}

static boolean
tn3270s_pu_tree_create (tn3270s_t *pTn3270s)
{
    if (wavl_init(&pTn3270s->pu_tree, NUM_PU_AVL_THREADS, NULL,
	      compare_puname_asc, compare_puname_desc)) {
	return FALSE;
    }
    return TRUE;
}

static tn3270s_pu_t *
tn3270s_pu_entry_create (tn3270s_t *pTn3270s,
			 char      *pName)
{
    tn3270s_pu_t *pPU;
    
    pPU = malloc(sizeof(tn3270s_pu_t));
    sstrncpy(pPU->name, pName, MAX_NAME_SIZE+1);
    pPU->state = TN3270S_STATE_INUSE;

    /* Insert into both threads of the tree */
    if (!wavl_insert(&pTn3270s->pu_tree, pu_to_wavl(pPU))){
	free(pPU);
	return NULL;
    }
    return pPU;
}

static void
tn3270s_pu_entry_delete (tn3270s_t    *pTn3270s,
			 tn3270s_pu_t *pPU)
{
    if (!pPU)
	return;
    
    /* Delete entry from the tree */
    wavl_delete(&pTn3270s->pu_tree, pu_to_wavl(pPU));

    free(pPU);
}

static void
tn3270s_pu_tree_destroy (tn3270s_t *pTn3270s)
{
    tn3270s_pu_t *pPU;

    while ((pPU = wavl_to_pu(wavl_get_first(&pTn3270s->pu_tree,
					    AVL_PU_ASCENDING))) != NULL)
	tn3270s_pu_entry_delete(pTn3270s, pPU);

/* Absolutely destroy the tree */
    wavl_finish(&pTn3270s->pu_tree);
    pTn3270s->pu_configured = FALSE;
}

static tn3270s_pu_t *
tn3270s_pu_entry_get_first (tn3270s_t *pTn3270s)
{
    tn3270s_pu_t *pPU;
    
    if (! pTn3270s->pu_configured) {
       return NULL;
    }
    pPU = wavl_to_pu(wavl_get_first(&pTn3270s->pu_tree,
				    AVL_PU_ASCENDING));
    return pPU;
}

static tn3270s_pu_t *
tn3270s_pu_entry_get_next (tn3270s_t    *pTn3270s,
			   tn3270s_pu_t *pPU)
{
    tn3270s_pu_t *pNext;
    
    pNext = wavl_to_pu(wavl_get_next(&pTn3270s->pu_tree,
				     pu_to_wavl(pPU), AVL_PU_ASCENDING));
    return pNext;
}

static tn3270s_pu_t *
tn3270s_pu_entry_get_last (tn3270s_t *pTn3270s)
{
    tn3270s_pu_t *pPU;
    
    pPU = wavl_to_pu(wavl_get_first(&pTn3270s->pu_tree,
				    AVL_PU_DESCENDING));
    return pPU;
}

static tn3270s_pu_t *
tn3270s_pu_entry_get_prev (tn3270s_t    *pTn3270s,
			   tn3270s_pu_t *pPU)
{
    tn3270s_pu_t *pPrev;
    
    pPrev = wavl_to_pu(wavl_get_next(&pTn3270s->pu_tree,
				     pu_to_wavl(pPU), AVL_PU_DESCENDING));
    return pPrev;
}

/*
 * ---------------------------------------------------------------
 * tn3270s_pu_entry_locate_by_name
 * ---------------------------------------------------------------
 */
static tn3270s_pu_t *
tn3270s_pu_entry_locate_by_name (tn3270s_t *pTn3270s,
				 char      *pName)
{
    tn3270s_pu_t target;
    
    sstrncpy(target.name, pName, MAX_NAME_SIZE+1);
    
    return(wavl_to_pu(wavl_search(&pTn3270s->pu_tree,
				  pu_to_wavl(&target),
				  AVL_PU_ASCENDING)));

}
/*
 * ---------------------------------------------------------------
 * SAP wavl tree operations
 * ---------------------------------------------------------------
 */

static inline tn3270s_dlur_sap_t *
wavl_to_sap (wavl_node_type *wavl)
{
    return (tn3270s_dlur_sap_t *)wavl;
}

static inline wavl_node_type *
sap_to_wavl (tn3270s_dlur_sap_t *pSAP)
{
    return (wavl_node_type *)pSAP;
}

/*
 * ---------------------------------------------------------------
 * comparison routines: sap tree
 * ---------------------------------------------------------------
 */
/*
 * SAP lan_type adapno (lsap)
 *     current support is for only one lsap per lan_type, adapno pair
 */
static enum avl_compare
compare_sap_asc (avl_node_type *n1, avl_node_type *n2)
{
    tn3270s_dlur_sap_t *node1;
    tn3270s_dlur_sap_t *node2;
    int compare;
    
    node1 = wavl_to_sap(wavl_normalize(n1, AVL_SAP_ASCENDING));
    node2 = wavl_to_sap(wavl_normalize(n2, AVL_SAP_ASCENDING));
    
    compare = node1->lan_type - node2->lan_type;
    if (compare < 0)
	return(AVL_LT);
    if (compare > 0)
	return(AVL_GT);
    compare = node1->adapno - node2->adapno;
    if (compare < 0)
	return(AVL_LT);
    if (compare > 0)
	return(AVL_GT);
    return(AVL_EQ);
}

static enum avl_compare
compare_sap_desc (avl_node_type *n1, avl_node_type *n2)
{
    tn3270s_dlur_sap_t *node1;
    tn3270s_dlur_sap_t *node2;
    int compare;
    
    node1 = wavl_to_sap(wavl_normalize(n1, AVL_SAP_DESCENDING));
    node2 = wavl_to_sap(wavl_normalize(n2, AVL_SAP_DESCENDING));

    compare = node2->lan_type - node1->lan_type;
    if (compare < 0)
	return(AVL_LT);
    if (compare > 0)
	return(AVL_GT);
    compare = node2->adapno - node1->adapno;
    if (compare < 0)
	return(AVL_LT);
    if (compare > 0)
	return(AVL_GT);
    return(AVL_EQ);
}

static boolean
tn3270s_sap_tree_create (tn3270s_t *pTn3270s)
{
    tn3270s_dlur_t *pDLUR;
    
    pDLUR = &pTn3270s->dlur;

    if (wavl_init(&pDLUR->sap_tree, NUM_SAP_AVL_THREADS, NULL,
		  compare_sap_asc, compare_sap_desc)) {
	return FALSE;
    }
    return TRUE;
}

static tn3270s_dlur_sap_t *
tn3270s_sap_entry_create (tn3270s_t *pTn3270s,
			  ushort     lan_type,
			  ushort     adapno,
			  uchar      lsap)
{
    tn3270s_dlur_t     *pDLUR;
    tn3270s_dlur_sap_t *pSAP;
    
    pDLUR = &pTn3270s->dlur;
    if (!(pDLUR->state & TN3270S_STATE_INUSE))
	return NULL;

    pSAP = malloc(sizeof(tn3270s_dlur_sap_t));
    pSAP->lan_type = lan_type;
    pSAP->adapno = adapno;
    pSAP->lsap = lsap;
    pSAP->state = TN3270S_STATE_INUSE;

    /* Insert into both threads of the tree */
    if (!wavl_insert(&pDLUR->sap_tree, sap_to_wavl(pSAP))){
	free(pSAP);
	return NULL;
    }
    return pSAP;
}

static void
tn3270s_sap_entry_delete (tn3270s_t          *pTn3270s,
			  tn3270s_dlur_sap_t *pSAP)
{
    tn3270s_dlur_t     *pDLUR;
    
    pDLUR = &pTn3270s->dlur;
    if (!(pDLUR->state & TN3270S_STATE_INUSE))
	return;

    if (!pSAP)
	return;
    
    /* Delete entry from the tree */
    wavl_delete(&pDLUR->sap_tree, sap_to_wavl(pSAP));
    
    free(pSAP);
}

static void
tn3270s_sap_tree_destroy (tn3270s_t *pTn3270s)
{
    tn3270s_dlur_t     *pDLUR;
    tn3270s_dlur_sap_t *pSAP;
    
    pDLUR = &pTn3270s->dlur;
    if (!(pDLUR->state & TN3270S_STATE_INUSE))
	return;

    while ((pSAP = wavl_to_sap(wavl_get_first(&pDLUR->sap_tree,
					      AVL_SAP_ASCENDING))) != NULL)
	tn3270s_sap_entry_delete(pTn3270s, pSAP);

/* Absolutely destroy the tree */
    wavl_finish(&pDLUR->sap_tree);
}

static tn3270s_dlur_sap_t *
tn3270s_sap_entry_get_first (tn3270s_t *pTn3270s)
{
    tn3270s_dlur_t     *pDLUR;
    tn3270s_dlur_sap_t *pSAP;
    
    pDLUR = &pTn3270s->dlur;
    if (!(pDLUR->state & TN3270S_STATE_INUSE))
	return NULL;
    
    pSAP = wavl_to_sap(wavl_get_first(&pDLUR->sap_tree,
				    AVL_SAP_ASCENDING));
    return pSAP;
}

static tn3270s_dlur_sap_t *
tn3270s_sap_entry_get_next (tn3270s_t          *pTn3270s,
			    tn3270s_dlur_sap_t *pSAP)
{
    tn3270s_dlur_t     *pDLUR;
    tn3270s_dlur_sap_t *pNext;
    
    pDLUR = &pTn3270s->dlur;
    if (!(pDLUR->state & TN3270S_STATE_INUSE))
	return NULL;
    
    pNext = wavl_to_sap(wavl_get_next(&pDLUR->sap_tree,
				      sap_to_wavl(pSAP), AVL_SAP_ASCENDING));
    return pNext;
}

static tn3270s_dlur_sap_t *
tn3270s_sap_entry_get_last (tn3270s_t *pTn3270s)
{
    tn3270s_dlur_t     *pDLUR;
    tn3270s_dlur_sap_t *pSAP;
    
    pDLUR = &pTn3270s->dlur;
    if (!(pDLUR->state & TN3270S_STATE_INUSE))
	return NULL;
    
    pSAP = wavl_to_sap(wavl_get_first(&pDLUR->sap_tree,
				      AVL_SAP_DESCENDING));
    return pSAP;
}

static tn3270s_dlur_sap_t *
tn3270s_sap_entry_get_prev (tn3270s_t          *pTn3270s,
			    tn3270s_dlur_sap_t *pSAP)
{
    tn3270s_dlur_t     *pDLUR;
    tn3270s_dlur_sap_t *pPrev;
    
    pDLUR = &pTn3270s->dlur;
    if (!(pDLUR->state & TN3270S_STATE_INUSE))
	return NULL;
    
    pPrev = wavl_to_sap(wavl_get_next(&pDLUR->sap_tree,
				      sap_to_wavl(pSAP), AVL_SAP_DESCENDING));
    return pPrev;
}

/*
 * ---------------------------------------------------------------
 * tn3270s_sap_entry_locate
 * ---------------------------------------------------------------
 */
static tn3270s_dlur_sap_t *
tn3270s_sap_entry_locate (tn3270s_t *pTn3270s,
			  ushort     lan_type,
			  ushort     adapno,
			  uchar      lsap)
{
    tn3270s_dlur_t     *pDLUR;
    tn3270s_dlur_sap_t target;
    
    pDLUR = &pTn3270s->dlur;
    if (!(pDLUR->state & TN3270S_STATE_INUSE))
	return NULL;

    target.lan_type = lan_type;
    target.adapno = adapno;
    target.lsap = lsap;
    
    return(wavl_to_sap(wavl_search(&pDLUR->sap_tree,
				   sap_to_wavl(&target),
				   AVL_SAP_ASCENDING)));
    
}

/*
 * ---------------------------------------------------------------
 * Link wavl tree operations
 * ---------------------------------------------------------------
 */
static inline tn3270s_link_t *
wavl_to_link (wavl_node_type *wavl)
{
    return (tn3270s_link_t *)wavl;
}

static inline wavl_node_type *
link_to_wavl (tn3270s_link_t *pLink)
{
    return (wavl_node_type *)pLink;
}

/*
 * ---------------------------------------------------------------
 * comparison routines: link tree
 * ---------------------------------------------------------------
 */
static enum avl_compare
compare_link_name_asc (avl_node_type *n1, avl_node_type *n2)
{
    tn3270s_link_t *node1;
    tn3270s_link_t *node2;
    int compare;
    
    node1 = wavl_to_link(wavl_normalize(n1, AVL_LINK_NAME_ASCENDING));
    node2 = wavl_to_link(wavl_normalize(n2, AVL_LINK_NAME_ASCENDING));
        
    compare = strcmp(node1->name, node2->name);
    if (compare < 0)
	return(AVL_LT);
    if (compare > 0)
	return(AVL_GT);
    return(AVL_EQ);
}

static enum avl_compare
compare_link_sapmac_asc (avl_node_type *n1, avl_node_type *n2)
{
    tn3270s_link_t *node1;
    tn3270s_link_t *node2;
    int compare;
    
    node1 = wavl_to_link(wavl_normalize(n1, AVL_LINK_SAPMAC_ASCENDING));
    node2 = wavl_to_link(wavl_normalize(n2, AVL_LINK_SAPMAC_ASCENDING));

    compare = node1->lan_type - node2->lan_type;
    if (compare < 0)
	return(AVL_LT);
    if (compare > 0)
	return(AVL_GT);
    compare = node1->adapno - node2->adapno;
    if (compare < 0)
	return(AVL_LT);
    if (compare > 0)
	return(AVL_GT);
    compare = ieee_compare(node1->rmac, node2->rmac);
    if (compare < 0)
	return(AVL_LT);
    if (compare > 0)
	return(AVL_GT);
    compare = node1->rsap - node2->rsap;
    if (compare < 0)
	return(AVL_LT);
    if (compare > 0)
	return(AVL_GT);
    return(AVL_EQ);
}

static boolean
tn3270s_link_tree_create (tn3270s_t *pTn3270s)
{
    tn3270s_dlur_t *pDLUR;
    
    pDLUR = &pTn3270s->dlur;

    if (wavl_init(&pDLUR->link_tree, NUM_LINK_AVL_THREADS, NULL,
		  compare_link_name_asc,
		  compare_link_sapmac_asc)) {
	return FALSE;
    }
    return TRUE;
}

static tn3270s_link_t *
tn3270s_link_entry_create (tn3270s_t          *pTn3270s,
			   tn3270s_dlur_sap_t *pSAP,
			   char               *pName,
			   char               *rmac,
			   uchar               rsap)
{
    tn3270s_dlur_t *pDLUR;
    tn3270s_link_t *pLink;
    
    pDLUR = &pTn3270s->dlur;
    if (!(pDLUR->state & TN3270S_STATE_INUSE))
	return NULL;

    pLink = malloc(sizeof(tn3270s_link_t));
    if (!pLink)
	return NULL;

    sstrncpy(pLink->name, pName, MAX_NAME_SIZE+1);
    ieee_copy(rmac, pLink->rmac);
    pLink->rsap = rsap;
    pLink->lan_type = pSAP->lan_type;
    pLink->adapno = pSAP->adapno;
    pLink->state = TN3270S_STATE_INUSE;
    pLink->pSAP = pSAP;

    /* Insert into both threads of the tree */
    if (!wavl_insert(&pDLUR->link_tree, link_to_wavl(pLink))){
	free(pLink);
	return NULL;
    }
    return pLink;
}

static void
tn3270s_link_entry_delete (tn3270s_t      *pTn3270s,
			   tn3270s_link_t *pLink)
{
    tn3270s_dlur_t     *pDLUR;
    
    pDLUR = &pTn3270s->dlur;
    if (!(pDLUR->state & TN3270S_STATE_INUSE))
	return;
    
    if (!pLink)
	return;

    /* Delete entry from the tree */
    wavl_delete(&pDLUR->link_tree, link_to_wavl(pLink));
    
    free(pLink);
}

static void
tn3270s_link_tree_destroy (tn3270s_t *pTn3270s)
{
    tn3270s_dlur_t *pDLUR;
    tn3270s_link_t *pLink;
    
    pDLUR = &pTn3270s->dlur;
    if (!(pDLUR->state & TN3270S_STATE_INUSE))
	return;
    
    while ((pLink = wavl_to_link(wavl_get_first(&pDLUR->link_tree,
						AVL_LINK_NAME_ASCENDING))) != NULL)
	tn3270s_link_entry_delete(pTn3270s, pLink);
    
    /* Absolutely destroy the tree */
    wavl_finish(&pDLUR->link_tree);
}

static tn3270s_link_t *
tn3270s_link_entry_get_first_by_name (tn3270s_t *pTn3270s)
{
    tn3270s_dlur_t     *pDLUR;
    tn3270s_link_t     *pLink;
    
    pDLUR = &pTn3270s->dlur;
    if (!(pDLUR->state & TN3270S_STATE_INUSE))
	return NULL;
    
    pLink = wavl_to_link(wavl_get_first(&pDLUR->link_tree,
					AVL_LINK_NAME_ASCENDING));
    return pLink;
}

static tn3270s_link_t *
tn3270s_link_entry_get_next_by_name (tn3270s_t      *pTn3270s,
				     tn3270s_link_t *pLink)
{
    tn3270s_dlur_t *pDLUR;
    tn3270s_link_t *pNext;
    
    pDLUR = &pTn3270s->dlur;
    if (!(pDLUR->state & TN3270S_STATE_INUSE))
	return NULL;
    
    pNext = wavl_to_link(wavl_get_next(&pDLUR->link_tree,
				       link_to_wavl(pLink),
				       AVL_LINK_NAME_ASCENDING));
    return pNext;
}

static tn3270s_link_t *
tn3270s_link_entry_get_first_by_sapmac (tn3270s_t *pTn3270s)
{
    tn3270s_dlur_t *pDLUR;
    tn3270s_link_t *pLink;
    
    pDLUR = &pTn3270s->dlur;
    if (!(pDLUR->state & TN3270S_STATE_INUSE))
	return NULL;
    
    pLink = wavl_to_link(wavl_get_first(&pDLUR->link_tree,
					AVL_LINK_SAPMAC_ASCENDING));
    return pLink;
}

static tn3270s_link_t *
tn3270s_link_entry_get_next_by_sapmac (tn3270s_t      *pTn3270s,
				       tn3270s_link_t *pLink)
{
    tn3270s_dlur_t *pDLUR;
    tn3270s_link_t *pNext;
    
    pDLUR = &pTn3270s->dlur;
    if (!(pDLUR->state & TN3270S_STATE_INUSE))
	return NULL;
    
    pNext = wavl_to_link(wavl_get_next(&pDLUR->link_tree,
				       link_to_wavl(pLink),
				       AVL_LINK_SAPMAC_ASCENDING));
    return pNext;
}

enum tn3270s_link_context_t_ {
    TN3270S_LINK_CONTEXT_ALL,
    TN3270S_LINK_CONTEXT_THIS_SAP
};
typedef enum tn3270s_link_context_t_ tn3270s_link_context_t;

static tn3270s_link_t *
tn3270s_link_entry_get_first (tn3270s_t             *pTn3270s,
			      tn3270s_dlur_sap_t    *pSAP,
			      tn3270s_link_context_t context)
{
    tn3270s_link_t *pLink;
    
    switch (context) {
    case TN3270S_LINK_CONTEXT_ALL:
	pLink = tn3270s_link_entry_get_first_by_name(pTn3270s);
	return pLink;
    case TN3270S_LINK_CONTEXT_THIS_SAP:
	if (!pSAP)
	    return NULL;
	/* Skip over any entries not belonging to this sap */
	pLink = tn3270s_link_entry_get_first_by_sapmac(pTn3270s);
	while (pLink) {
	    if ((pLink->lan_type == pSAP->lan_type) &&
		(pLink->adapno == pSAP->adapno))
		return pLink;
	    pLink = tn3270s_link_entry_get_next_by_sapmac(pTn3270s,
							  pLink);
	}
	return pLink;
    }
    return NULL;
}

static tn3270s_link_t *
tn3270s_link_entry_get_next (tn3270s_t             *pTn3270s,
			     tn3270s_dlur_sap_t    *pSAP,
			     tn3270s_link_t        *pLink,
			     tn3270s_link_context_t context)
{
    tn3270s_link_t *pNext;
    
    switch (context) {
    case TN3270S_LINK_CONTEXT_ALL:
	pNext = tn3270s_link_entry_get_next_by_name(pTn3270s,
						    pLink);
	return pNext;
    case TN3270S_LINK_CONTEXT_THIS_SAP:
	if (!pSAP)
	    return NULL;
	/* Get the next entry, but return NULL if not this sap */
	pNext = tn3270s_link_entry_get_next_by_sapmac(pTn3270s,
						      pLink);
	if (pNext) {
	    if ((pNext->lan_type == pSAP->lan_type) &&
		(pNext->adapno == pSAP->adapno))
		return pNext;
	    return NULL;
	}
	return pNext;
    }
    return NULL;
}

static tn3270s_link_t *
tn3270s_link_entry_locate_by_name (tn3270s_t *pTn3270s,
				   char      *pName)
{
    tn3270s_dlur_t     *pDLUR;
    tn3270s_link_t     target;
    
    pDLUR = &pTn3270s->dlur;
    if (!(pDLUR->state & TN3270S_STATE_INUSE))
	return NULL;
    
    sstrncpy(target.name, pName, MAX_NAME_SIZE+1);
    
    return(wavl_to_link(wavl_search(&pDLUR->link_tree,
				    link_to_wavl(&target),
				    AVL_LINK_NAME_ASCENDING)));
}

static tn3270s_link_t *
tn3270s_link_entry_locate_by_rmac (tn3270s_t          *pTn3270s,
				   tn3270s_dlur_sap_t *pSAP,
				   char               *rmac,
				   uchar               rsap)
{
    tn3270s_dlur_t     *pDLUR;
    tn3270s_link_t     target;
    
    pDLUR = &pTn3270s->dlur;
    if (!(pDLUR->state & TN3270S_STATE_INUSE))
	return NULL;

    ieee_copy(rmac, target.rmac);
    target.lan_type = pSAP->lan_type;

    target.adapno = pSAP->adapno;
    target.rsap = rsap;
    
    return(wavl_to_link(wavl_search(&pDLUR->link_tree,
				    link_to_wavl(&target),
				    AVL_LINK_SAPMAC_ASCENDING)));
}

/*
 * ---------------------------------------------------------------
 * IPVC array operations
 * ---------------------------------------------------------------
 */
static void
tn3270s_ipvc_entry_reset (tn3270s_ipvc_t *pIPVC)
{ 
    pIPVC->inuse = FALSE;
    pIPVC->ipaddr = 0;
    pIPVC->vc_users = 0;
    pIPVC->sr_users = 0;
}

static void
tn3270s_ipvc_create (tn3270s_t *pTn3270s)
{
    tn3270s_ipvc_t *pIPVC;
    int i;
    
    for (i = 0; i < TN3270S_NUM_VC; i++) {
	pIPVC = &pTn3270s->vc_array[i];
	tn3270s_ipvc_entry_reset(pIPVC);
	pIPVC->vc = TN3270S_MIN_VC + i;
    }
}    

static void
tn3270s_ipvc_destroy (tn3270s_t *pTn3270s)
{
}    

static tn3270s_ipvc_t *
tn3270s_ipvc_entry_create (tn3270s_t *pTn3270s,
			   ipaddrtype ipaddr)
{
    tn3270s_ipvc_t *pIPVC;
    int i;
    
    for (i = 0; i < TN3270S_NUM_VC; i++) {
	pIPVC = &pTn3270s->vc_array[i];
	if (!pIPVC->inuse) {
	    pIPVC->inuse = TRUE;
	    pIPVC->ipaddr = ipaddr;
	    return pIPVC;
	}
    }
    return NULL;
}    

static tn3270s_ipvc_t *
tn3270s_ipvc_entry_locate_by_ipaddr (tn3270s_t *pTn3270s,
				     ipaddrtype ipaddr)
{
    int i;
    
    for (i = 0; i < TN3270S_NUM_VC; i++)
	if (pTn3270s->vc_array[i].ipaddr == ipaddr)
	    return &pTn3270s->vc_array[i];
    return NULL;
}

static tn3270s_ipvc_t *
tn3270s_ipvc_entry_locate_by_vc (tn3270s_t *pTn3270s,
				 ushort     vc)
{
    int i;
    
    for (i = 0; i < TN3270S_NUM_VC; i++)
	if (pTn3270s->vc_array[i].vc == vc)
	    return &pTn3270s->vc_array[i];
    return NULL;
}

static void
tn3270s_ipvc_entry_add_vc_user (tn3270s_t *pTn3270s,
				ushort     vc)
{
    tn3270s_ipvc_t *pIPVC;
    
    pIPVC = tn3270s_ipvc_entry_locate_by_vc(pTn3270s, vc);
    if (!pIPVC)
	return;

    pIPVC->vc_users++;
}

static void
tn3270s_ipvc_entry_del_vc_user (tn3270s_t *pTn3270s,
				ushort     vc)
{
    tn3270s_ipvc_t *pIPVC;
    
    pIPVC = tn3270s_ipvc_entry_locate_by_vc(pTn3270s, vc);
    if (!pIPVC)
	return;

    pIPVC->vc_users--;
    if (!pIPVC->vc_users) {
	tn3270s_ipvc_entry_reset(pIPVC);
    }
}

static void
tn3270s_ipvc_entry_add_sr_user (idbtype   *swidb,
				tn3270s_t *pTn3270s,
				ipaddrtype ipaddr)
{
    addrtype proto_addr;
    tn3270s_ipvc_t *pIPVC;
    
    pIPVC = tn3270s_ipvc_entry_locate_by_ipaddr(pTn3270s, ipaddr);
    if (!pIPVC)
	return;
    
    if (!pIPVC->sr_users) {
	bzero(&proto_addr, sizeof(addrtype));
	proto_addr.type = ADDR_IP;
	proto_addr.length = ADDRLEN_IP;
	proto_addr.ip_addr = pIPVC->ipaddr;
	reg_invoke_ip_add_static_route(pIPVC->ipaddr, MASK_HOST, 0,
				       swidb, 0, NET_ATTACHED|NET_INTERNAL,
				       STATIC_DISTANCE, NULL);
	channel_ipaddr_insert(pIPVC->vc, &proto_addr);
    }
    pIPVC->sr_users++;
}

static void
tn3270s_ipvc_entry_del_sr_user (idbtype   *swidb,
				tn3270s_t *pTn3270s,
				ipaddrtype ipaddr)
{
    tn3270s_ipvc_t *pIPVC;
    
    pIPVC = tn3270s_ipvc_entry_locate_by_ipaddr(pTn3270s, ipaddr);
    if (!pIPVC)
	return;
    
    pIPVC->sr_users--;
    
    if (!pIPVC->sr_users) {
	reg_invoke_ip_delete_static_route(pIPVC->ipaddr, MASK_HOST, swidb);
	channel_ipaddr_delete(pIPVC->ipaddr);
    }
}

static void
tn3270s_pu_add_sr (idbtype      *swidb,
		   tn3270s_t    *pTn3270s,
		   tn3270s_pu_t *pPU)
{
    if (pPU->state & TN3270S_STATE_SHUT)
	return;

    if (!(pPU->state & TN3270S_STATE_VC_SR)) {
	tn3270s_ipvc_entry_add_sr_user(swidb, pTn3270s, pPU->ipaddr);
	pPU->state |=  TN3270S_STATE_VC_SR;
    }
}

static void
tn3270s_pu_del_sr (idbtype      *swidb,
		   tn3270s_t    *pTn3270s,
		   tn3270s_pu_t *pPU)
{
    if (pPU->state & TN3270S_STATE_VC_SR) {
	tn3270s_ipvc_entry_del_sr_user(swidb, pTn3270s, pPU->ipaddr);
	pPU->state &= ~TN3270S_STATE_VC_SR;
    }
}
/* 
 * given an ipaddr, return the matching vc
 * if no vc is currently assigned to the ipaddr,
 * create one
 */
static ushort
tn3270s_ipvc_assign_vc (tn3270s_t *pTn3270s,
			ipaddrtype ipaddr)
{
    tn3270s_ipvc_t *pIPVC;
    
    /*
     * If this ipaddr is already assigned a vc,
     */
    pIPVC = tn3270s_ipvc_entry_locate_by_ipaddr(pTn3270s, ipaddr);
    
    if (!pIPVC) {
	pIPVC = tn3270s_ipvc_entry_create(pTn3270s, ipaddr);
	if (!pIPVC)
	    return TN3270S_NO_VC;
    }
    
    tn3270s_ipvc_entry_add_vc_user(pTn3270s, pIPVC->vc);
    return pIPVC->vc;
    
}

/*
 * ===========================================================================
 * Primitive data type operations: Derivations
 * ===========================================================================
 */
/*
 * ---------------------------------------------------------------
 * tn3270s_pu_entry_locate_by_xsap_xmac
 *  Assure uniqueness of:
 *     lan_type, adapno, lsap, rmac, rsap
 *  across all Direct PU
 * ---------------------------------------------------------------
 */
static tn3270s_pu_t *
tn3270s_pu_entry_locate_by_xsap_xmac (tn3270s_t *pTn3270s,
				      ushort     lan_type,
				      ushort     adapno,
				      uchar      lsap,
				      uchar     *rmac,
				      uchar      rsap)
{
    tn3270s_pu_t     *pPU;
    
    pPU = tn3270s_pu_entry_get_first(pTn3270s);
    while (pPU) {
	if (pPU->type == TN3270S_PUTYPE_DIRECT) {
	    if ((pPU->lan_type == lan_type) &&
		(pPU->adapno == adapno) &&
		(pPU->lsap == lsap) &&
		(ieee_equal(pPU->rmac, rmac)) &&
		(pPU->rsap == rsap))
		return (pPU);
	}
	pPU = tn3270s_pu_entry_get_next(pTn3270s, pPU);
    }
    return (NULL);
}
/*
 * ---------------------------------------------------------------
 * tn3270s_pu_entry_locate_by_xsap
 *  Assure uniqueness of:
 *     lan_type, adapno, lsap
 *  across all Direct PU
 * ---------------------------------------------------------------
 */
static tn3270s_pu_t *
tn3270s_pu_entry_locate_by_xsap (tn3270s_t *pTn3270s,
				 ushort     lan_type,
				 ushort     adapno,
				 uchar      lsap)
{
    tn3270s_pu_t     *pPU;
    
    pPU = tn3270s_pu_entry_get_first(pTn3270s);
    while (pPU) {
	if (pPU->type == TN3270S_PUTYPE_DIRECT) {
	    if ((pPU->lan_type == lan_type) &&
		(pPU->adapno == adapno) &&
		(pPU->lsap == lsap))
		return (pPU);
	}
	pPU = tn3270s_pu_entry_get_next(pTn3270s, pPU);
    }
    return (NULL);
}
/*
 * ---------------------------------------------------------------
 * tn3270s_pu_entry_locate_by_snmp_index
 * ---------------------------------------------------------------
 */
static tn3270s_pu_t *
tn3270s_pu_entry_locate_by_snmp_index (tn3270s_t *pTn3270s,
				       ulong      index)
{
    tn3270s_pu_t     *pPU;
    tn3270s_pu_t     *bestPU;
    pPU = tn3270s_pu_entry_get_first(pTn3270s);
    bestPU = NULL;
    while (pPU) {
	if (pPU->index >= index){
            if (pPU->index == index) {
               return (pPU);
	    }
            else if (bestPU == NULL || bestPU->index > pPU->index){
               bestPU = pPU;
           }
        }
	pPU = tn3270s_pu_entry_get_next(pTn3270s, pPU);
    }
    return (bestPU);
}

/*
 * ---------------------------------------------------------------
 * tn3270s_sap_entry_locate_by_xsap_xmac
 * ---------------------------------------------------------------
 */
static tn3270s_dlur_sap_t *
tn3270s_sap_entry_locate_by_xsap_xmac (idbtype   *swidb,
				       tn3270s_t *pTn3270s,
				       uchar     *lmac,
				       uchar      lsap)
{
    tn3270s_dlur_sap_t  *pSAP;
    void                *pVlan;
    void                *pVadapt;
    ulong                lan_id;
    ulong                lan_type;
    
    pSAP = tn3270s_sap_entry_get_first(pTn3270s);
    while (pSAP) {
	if (pSAP->lsap == lsap) {
	    /*
	     * Get the first lan of this type
	     * then get lmac of the matching adapter
	     */
	    lan_id = 0;
	    lan_type = pSAP->lan_type;
	    do {
		pVlan = cip_get_vlan(swidb->hwptr, TN3270S_FIND_NEXT,
				     lan_type, lan_id);
		if (pVlan) {
		    lan_type = get_vlan_lan_type(pVlan);
		    lan_id = get_vlan_lan_id(pVlan);
		    if (lan_type == pSAP->lan_type) {
			pVadapt = cip_get_vadapt_from_adapno(swidb->hwptr,
							     pVlan,
							     TN3270S_FIND_MATCH,
							     pSAP->adapno);
			if (pVadapt) {
			    if (ieee_equal(get_vadapt_mac_address(pVadapt),
					   lmac))
				return (pSAP);
			}
		    }
		    lan_id++;
		}
	    } while (pVlan && (lan_type == pSAP->lan_type));
	}
    pSAP = tn3270s_sap_entry_get_next(pTn3270s, pSAP);
    }
    return (NULL);
}

/*
 * ===========================================================================
 * Creation, destruction and initialization of the TN3270 data structures
 * ===========================================================================
 */
/*
 * -------------------------------------------------------------------
 * Svr
 * -------------------------------------------------------------------
 */
/*
 * Initialize the Tn3270s Server fields 
 */
static boolean
tn3270s_svr_create (tn3270s_t *pTn3270s)
{
    /* create the AVL tree of PU entries */
    pTn3270s->pu_configured = tn3270s_pu_tree_create(pTn3270s);
    if (!pTn3270s->pu_configured)
	return FALSE;

    tn3270s_ipvc_create(pTn3270s);

    /* This is the creation of the feature */
    pTn3270s->svr.state = TN3270S_STATE_INUSE;
    return TRUE;
}    

static void
tn3270s_svr_delete (tn3270s_t *pTn3270s)
{
    /* destroy the AVL tree and all its PU entries */
    tn3270s_pu_tree_destroy(pTn3270s);

    tn3270s_ipvc_destroy(pTn3270s);

    pTn3270s->svr.state = TN3270S_STATE_UNUSED;
}    

/*
 * Initialize the Tn3270s Server fields 
 */
static void
tn3270s_svr_init (tn3270s_t *pTn3270s)
{
    /*
     * Initialize the configuration correlator 
     */
    pTn3270s->tn3270s_correlator = TN3270S_CORRELATOR_INIT;
    
    pTn3270s->svr.max_lu.use_default = TRUE;
    pTn3270s->svr.timing_mark.val = FALSE;
    pTn3270s->svr.max_lu.val = TN3270S_SVR_MAX_LU_DEF;
    
    pTn3270s->svr.siftdown.tcp_port.use_default = TRUE;
    pTn3270s->svr.siftdown.tcp_port.val = TN3270S_TCP_PORT_DEF;
    
    pTn3270s->svr.siftdown.idle_time.use_default = TRUE;
    pTn3270s->svr.siftdown.idle_time.val = TN3270S_IDLE_TIME_DEF;
    
    pTn3270s->svr.siftdown.keepalive.use_default = TRUE;
    pTn3270s->svr.siftdown.keepalive.val = TN3270S_KEEPALIVE_DEF;
    
    pTn3270s->svr.siftdown.unbind_act.use_default = TRUE;
    pTn3270s->svr.siftdown.unbind_act.val = TN3270S_UNBIND_ACT_DEF;
    
    pTn3270s->svr.siftdown.generic_pool.use_default = TRUE;
    pTn3270s->svr.siftdown.generic_pool.val = TN3270S_GPOOL_DEF;
    
    pTn3270s->svr.lu_dns.use_default = TRUE;
    pTn3270s->svr.lu_dns.dns_fqdn[0] = '\0';
    pTn3270s->svr.lu_dns.rev_dns_fqdn[0] = '\0';
}    

/*
 * -------------------------------------------------------------------
 * PU
 * -------------------------------------------------------------------
 */
/*
 * Initialize a new PU
 */
static void
tn3270s_pu_init (tn3270s_t    *pTn3270s,
		 tn3270s_pu_t *pPU)
{
    pPU->new = TRUE;

    /* Assign a unique snmp query index */
    pPU->index = tn3270s_snmp_pu_index++;

    pPU->siftdown.tcp_port.use_default = TRUE;
    pPU->siftdown.tcp_port.val = pTn3270s->svr.siftdown.tcp_port.val;
    
    pPU->siftdown.idle_time.use_default = TRUE;
    pPU->siftdown.idle_time.val = pTn3270s->svr.siftdown.idle_time.val;
    
    pPU->siftdown.keepalive.use_default = TRUE;
    pPU->siftdown.keepalive.val = pTn3270s->svr.siftdown.keepalive.val;
    
    pPU->siftdown.unbind_act.use_default = TRUE;
    pPU->siftdown.unbind_act.val = pTn3270s->svr.siftdown.unbind_act.val;
    
    pPU->siftdown.generic_pool.use_default = TRUE;
    pPU->siftdown.generic_pool.val = pTn3270s->svr.siftdown.generic_pool.val;
    
    pPU->pu_dns.use_default = TRUE;
    pPU->pu_dns.name[0] = '\0';
}    
/*
 * -------------------------------------------------------------------
 * DLUR
 * -------------------------------------------------------------------
 */
static boolean
tn3270s_dlur_create (tn3270s_t *pTn3270s)
{
    tn3270s_dlur_t *pDLUR;
    
    pDLUR = &pTn3270s->dlur;

    /* create the AVL trees for SAP and Link entries */
    if (!tn3270s_sap_tree_create(pTn3270s))
	return FALSE;

    if (!tn3270s_link_tree_create(pTn3270s))
	return FALSE;

    /* This is the creation of the feature */
    pDLUR->state = TN3270S_STATE_INUSE;
    return TRUE;
}    

static void
tn3270s_dlur_delete (tn3270s_t *pTn3270s)
{
    tn3270s_dlur_t *pDLUR;
    
    pDLUR = &pTn3270s->dlur;

    /* destroy the AVL trees */
    tn3270s_sap_tree_destroy(pTn3270s);
    tn3270s_link_tree_destroy(pTn3270s);

    pDLUR->state = TN3270S_STATE_UNUSED;
}    

/*
 * Initialize the Tn3270s dlur fields 
 */
static void
tn3270s_dlur_init (tn3270s_t *pTn3270s)
{
    tn3270s_dlur_t *pDLUR;
    
    pDLUR = &pTn3270s->dlur;

    pDLUR->dlur_fq_cpname[0] = '\0';
    pDLUR->dlur_fq_dlusname[0] = '\0';

    pDLUR->dlur_dlus_backup.use_default = TRUE;
    pDLUR->dlur_dlus_backup.name[0] = '\0';

    pDLUR->dlur_nn_svr.use_default = TRUE;
    pDLUR->dlur_nn_svr.name[0] = '\0';
    
    /* pDLUR->dlur_pu and pDLUR->dlur_sap */
}    
/*
 * -------------------------------------------------------------------
 * SAP
 * -------------------------------------------------------------------
 */
static void  tn3270s_dlur_sap_init (tn3270s_t          *pTn3270s,
			     tn3270s_dlur_sap_t *pSAP)
{

    pSAP->dlur_sap_vrn.use_default = TRUE;
    pSAP->dlur_sap_vrn.name[0] = '\0';
    /* sap links should be NULLed - this is malloced memory */
}
/*
 * -------------------------------------------------------------------
 * Link
 * -------------------------------------------------------------------
 */
static void 
tn3270s_link_init (tn3270s_t      *pTn3270s,
		   tn3270s_link_t *pLink)
{

}

/*
 * ===========================================================================
 * Send configuration commands to the CIP.
 *  If the resource (or a higher level resource) is shut,
 *  acknowledge the command locally without sending it to the CIP 
 * ===========================================================================
 */
static inline boolean
tn3270s_query_shut (ushort state,
		    ushort shut_flags)
{
    if (state & shut_flags)
	return TRUE;
    return FALSE;
}
static inline boolean
tn3270s_query_shut_svr (tn3270s_t *pTn3270s)
{
    return tn3270s_query_shut(pTn3270s->svr.state,
			      TN3270S_STATE_SHUT | TN3270S_STATE_CIP_SHUT);
}
static inline boolean
tn3270s_query_cipshut_svr (tn3270s_t *pTn3270s)
{
    return tn3270s_query_shut(pTn3270s->svr.state,
			      TN3270S_STATE_CIP_SHUT);
}
static inline boolean
tn3270s_query_shut_dlur (tn3270s_t *pTn3270s)
{
    return tn3270s_query_shut(pTn3270s->dlur.state,
			      TN3270S_STATE_SHUT | TN3270S_STATE_CIP_SHUT);
}
static inline boolean
tn3270s_query_cipshut_dlur (tn3270s_t *pTn3270s)
{
    return tn3270s_query_shut(pTn3270s->dlur.state,
			      TN3270S_STATE_CIP_SHUT);
}
static inline boolean
tn3270s_query_shut_pu (tn3270s_pu_t *pPU)
{
    return tn3270s_query_shut(pPU->state,
			      TN3270S_STATE_SHUT | TN3270S_STATE_CIP_SHUT);
}
static inline boolean
tn3270s_query_cipshut_pu (tn3270s_pu_t *pPU)
{
    return tn3270s_query_shut(pPU->state,
			      TN3270S_STATE_CIP_SHUT);
}
static inline boolean
tn3270s_query_shut_dlur_sap (tn3270s_dlur_sap_t *pSAP)
{
    return tn3270s_query_shut(pSAP->state,
			      TN3270S_STATE_SHUT | TN3270S_STATE_CIP_SHUT);
}
static inline boolean
tn3270s_query_cipshut_dlur_sap (tn3270s_dlur_sap_t *pSAP)
{
    return tn3270s_query_shut(pSAP->state,
			      TN3270S_STATE_CIP_SHUT);
}


/*
 * -------------------------------------------------------------------
 * Routines to send cfgcmds to the CIP
 * -------------------------------------------------------------------
 */
/* Configurations to the CIP */
enum tn3270s_cfg_t_ {
    TN3270S_CFGTYPE_CREATE,
    TN3270S_CFGTYPE_UPDATE,
    TN3270S_CFGTYPE_NOSHUT,
    TN3270S_CFGTYPE_SHUT,
    TN3270S_CFGTYPE_DELETE
};
typedef enum tn3270s_cfg_t_ tn3270s_cfg_t;

#define OFFSET_CFG_CMDCODE 0
static void
tn3270s_cfgcmd (idbtype      *swidb,
		void         *p,
		tn3270s_cfg_t cfgtype,
		uchar        *buf,
		ushort        bytes)
{
    cip_idb_infotype   *cip;
    tn3270s_t          *pTn3270s;
    ushort             *pState;
    tn3270s_pu_t       *pPU;
    tn3270s_dlur_t     *pDLUR;
    tn3270s_dlur_sap_t *pSAP;
    tn3270s_link_t     *pLink;
    uchar               cfg_cmd;
    ushort              num_preQ;
    boolean             local;
    
    cip = channel_sb_get_cip_info(swidb->hwptr);
    pTn3270s = cip->pTn3270sData;
    pPU    = p;
    pDLUR  = p;
    pSAP   = p;
    pLink  = p;
    
    cfg_cmd = buf[OFFSET_CFG_CMDCODE] & ~CIP_TEARDOWN_VC;

    /* If this command has state associated with it
     * reflect the PNDING configuration type in the state flags
     */
    pState = NULL;
    switch (cfg_cmd) {
    case CIP_TN3270S_SVR          :
	pState = &pTn3270s->svr.state;
	break;
    case CIP_TN3270S_PU_NEW       :
    case CIP_TN3270S_PU_UPD       :
    case CIP_TN3270S_DLUR_PU_NEW  :
    case CIP_TN3270S_DLUR_PU_UPD  :
	pState = &pPU->state;
	break;
    case CIP_TN3270S_DLUR         :
	pState = &pDLUR->state;
	break;
    case CIP_TN3270S_DLUR_SAP     :
	pState = &pSAP->state;
	break;
    case CIP_TN3270S_DLUR_SAP_LNK :
	pState = &pLink->state;
	pSAP = tn3270s_sap_entry_locate(pTn3270s, pLink->lan_type,
					pLink->adapno, 0);
	break;
    }
    if (pState) {
	switch (cfgtype) {
	case TN3270S_CFGTYPE_CREATE:
	    *pState |= TN3270S_STATE_CREATE;
	    break;
	case TN3270S_CFGTYPE_SHUT:
	case TN3270S_CFGTYPE_NOSHUT:
	case TN3270S_CFGTYPE_UPDATE:
	    break;
	case TN3270S_CFGTYPE_DELETE:
	    *pState |= TN3270S_STATE_DELETE;
	    break;
	}
	/* Mark the configuration as PENDING acknowledgment from CIP */    
	*pState |= TN3270S_STATE_PNDING;
    }

    /*
     * Determine whether to send the command to the CIP
     */
    local = tn3270s_query_shut_svr(pTn3270s);
    /*
     * If this is a 'shutdown' command, send it
     * unless the resource is CIPSHUT or if a higher
     * level resource is shut
     */
    if (cfgtype == TN3270S_CFGTYPE_SHUT) {
	if (cfg_cmd == CIP_TN3270S_SVR)
	    local = tn3270s_query_cipshut_svr(pTn3270s);
	if (!local)
	    switch (cfg_cmd) {
	    case CIP_TN3270S_SVR          :	
		break;
	    case CIP_TN3270S_PU_NEW       :
		local = tn3270s_query_cipshut_pu(pPU);
		break;
	    case CIP_TN3270S_DLUR_PU_NEW  :
		if (tn3270s_query_shut_dlur(pTn3270s))
		    local = TRUE;
		else
		    local = tn3270s_query_cipshut_pu(pPU);
		break;
	    case CIP_TN3270S_DLUR         :
		local = tn3270s_query_cipshut_dlur(pTn3270s);
		break;
	    case CIP_TN3270S_DLUR_SAP     :
		if (tn3270s_query_shut_dlur(pTn3270s))
		    local = TRUE;
		else
		    local = tn3270s_query_cipshut_dlur_sap(pSAP);
		break;
	    }
    } else {
	/*
	 * This is not a 'shutdown' command, so send it to the CIP
	 * unless the resource is shut or if a higher
	 * level resource is shut
	 */
	if (!local)
	    switch (cfg_cmd) {
	    case CIP_TN3270S_SVR          :	
	    case CIP_TN3270S_SVR_LU_DNS   :
	    case CIP_TN3270S_SVR_MAX_LU   :
	    case CIP_TN3270S_SVR_TIMING_MARK :
	    case CIP_TN3270S_SVR_SIFTDOWN :
		break;
	    case CIP_TN3270S_PU_UPD       :
	    case CIP_TN3270S_PU_NEW       :
		local = tn3270s_query_shut_pu(pPU);
		break;
	    case CIP_TN3270S_DLUR_PU_NEW  :
	    case CIP_TN3270S_DLUR_PU_UPD  :
		if (tn3270s_query_shut_dlur(pTn3270s))
		    local = TRUE;
		else
		    local = tn3270s_query_shut_pu(pPU);
		break;
	    case CIP_TN3270S_DLUR         :
	    case CIP_TN3270S_DLUR_DLUS    :
	    case CIP_TN3270S_DLUR_NN_SVR  :
		local = tn3270s_query_shut_dlur(pTn3270s);
		break;
	    case CIP_TN3270S_DLUR_SAP     :
	    case CIP_TN3270S_DLUR_SAP_VRN :
	    case CIP_TN3270S_DLUR_SAP_LNK :
		if (tn3270s_query_shut_dlur(pTn3270s))
		    local = TRUE;
		else
		    local = tn3270s_query_shut_dlur_sap(pSAP);
		break;
	    }
    }    
    if (local) {
	cip_tn3270s_cfg_acknowledge(swidb->hwptr, CIP_CFG_ACK_LOCAL,
				    CIP_CFG_ACK, buf, bytes);
	return;
    }
    
    /* possibly move the adding of the correlator fields here */
    /* possibly move the incrementing of tn3270s_correlator here */
    
    num_preQ = 1;
    cip_send_or_ack(swidb->hwptr, cip, num_preQ, buf, bytes);
}

/* ---------------------------------------------------------------
 * indx                               CREATE  UPDATE  DELETE  
 *   0 cfg_code            [1]
 *     CIP_TN3270S_SVR                   X 
 *     CIP_TN3270S_SVR | x80                             X 
 *   1 align_me            [1]           X       X       X
 *   2 cfg_correlator      [4]           X       X       X
 *   6 cip_correlator      [2]           X       X       X
 *    -----------------------------------------------------------
 *   8 CFG_CMDBLOCK_SIZE
 */
#define OFFSET_CFG_CMDCODE                0
#define OFFSET_CFG_SVR_CFG_CORRELATOR     2
#define OFFSET_CFG_SVR_RESET_CORRELATOR   6
#undef  CFG_CMDBLOCK_SIZE
#define CFG_CMDBLOCK_SIZE                 8
static void
tn3270s_cfgcmd_svr (idbtype          *swidb,
		    cip_idb_infotype *cip,
		    tn3270s_t        *pTn3270s,
		    tn3270s_cfg_t     cfgtype)
{
    uchar       buf[CFG_CMDBLOCK_SIZE];
    int         bytes;

    bzero(&buf[0], CFG_CMDBLOCK_SIZE);
    
    bytes = CFG_CMDBLOCK_SIZE;
    
    switch (cfgtype) {
    case TN3270S_CFGTYPE_CREATE:
    case TN3270S_CFGTYPE_NOSHUT:
	buf[OFFSET_CFG_CMDCODE] = CIP_TN3270S_SVR;
	break;
    case TN3270S_CFGTYPE_SHUT  :
    case TN3270S_CFGTYPE_DELETE:
	buf[OFFSET_CFG_CMDCODE] = CIP_TN3270S_SVR | CIP_TEARDOWN_VC;
	break;
    case TN3270S_CFGTYPE_UPDATE:
	return;
    }
    
    /* Add the correlators */
    pTn3270s->svr.correlator = pTn3270s->tn3270s_correlator++;
    PUTLONG(&buf[OFFSET_CFG_SVR_CFG_CORRELATOR],
	    pTn3270s->svr.correlator);

    PUTSHORT(&buf[OFFSET_CFG_SVR_RESET_CORRELATOR],
	     cip->reset_correlator);

    tn3270s_cfgcmd(swidb, pTn3270s, cfgtype,
		   buf, bytes);
}

/* ---------------------------------------------------------------
 * indx                               CREATE  UPDATE  DELETE  
 *   0 cfg_code            [1]
 *     CIP_TN3270S_SVR_MAX_LU            X 
 *   1 align_me            [1]
 *   2 max_lu              [2]
 *   4 cfg_correlator      [4]           X       X       X
 *   8 cip_correlator      [2]           X       X       X
 *    -----------------------------------------------------------
 *  10 CFG_CMDBLOCK_SIZE
 */
#define OFFSET_CFG_CMDCODE                       0
#define OFFSET_CFG_SVR_MAX_LU_CFG_MAX_LU         2

#define OFFSET_CFG_SVR_MAX_LU_CFG_CORRELATOR     4
#define OFFSET_CFG_SVR_MAX_LU_RESET_CORRELATOR   8
#undef  CFG_CMDBLOCK_SIZE
#define CFG_CMDBLOCK_SIZE                       10

static void
tn3270s_cfgcmd_svr_max_lu(idbtype          *swidb,
			  cip_idb_infotype *cip,
			  tn3270s_t        *pTn3270s,
			  tn3270s_cfg_t     cfgtype)
{
    uchar       buf[CFG_CMDBLOCK_SIZE];
    int         bytes;

    bzero(&buf[0], CFG_CMDBLOCK_SIZE);
    
    bytes = CFG_CMDBLOCK_SIZE;
    buf[OFFSET_CFG_CMDCODE] = CIP_TN3270S_SVR_MAX_LU;

    PUTSHORT(&buf[OFFSET_CFG_SVR_MAX_LU_CFG_MAX_LU],
	     pTn3270s->svr.max_lu.val);
    
    /* Add the correlators */
    pTn3270s->svr.max_lu.correlator = pTn3270s->tn3270s_correlator++;
    PUTLONG(&buf[OFFSET_CFG_SVR_MAX_LU_CFG_CORRELATOR],
	    pTn3270s->svr.max_lu.correlator);

    PUTSHORT(&buf[OFFSET_CFG_SVR_MAX_LU_RESET_CORRELATOR],
	     cip->reset_correlator);

    tn3270s_cfgcmd(swidb, pTn3270s, cfgtype,
		   buf, bytes);
}
/* ---------------------------------------------------------------
 * indx                               CREATE  UPDATE  DELETE  
 *   0 cfg_code            [1]
 *     CIP_TN3270S_SVR_TIMING_MARK            X 
 *   1 align_me            [1]
 *   2 timing_mark              [2]
 *   4 cfg_correlator      [4]           X       X       X
 *   8 cip_correlator      [2]           X       X       X
 *    -----------------------------------------------------------
 *  10 CFG_CMDBLOCK_SIZE
 */
#define OFFSET_CFG_CMDCODE                       0
#define OFFSET_CFG_SVR_TIMING_MARK_CFG_TIMING_MARK         2

#define OFFSET_CFG_SVR_TIMING_MARK_CFG_CORRELATOR     4
#define OFFSET_CFG_SVR_TIMING_MARK_RESET_CORRELATOR   8
#undef  CFG_CMDBLOCK_SIZE
#define CFG_CMDBLOCK_SIZE                       10

static void
tn3270s_cfgcmd_svr_timing_mark(idbtype          *swidb,
			  cip_idb_infotype *cip,
			  tn3270s_t        *pTn3270s,
			  tn3270s_cfg_t     cfgtype)
{
    uchar       buf[CFG_CMDBLOCK_SIZE];
    int         bytes;

    bzero(&buf[0], CFG_CMDBLOCK_SIZE);
    
    bytes = CFG_CMDBLOCK_SIZE;
    buf[OFFSET_CFG_CMDCODE] = CIP_TN3270S_SVR_TIMING_MARK;

    PUTSHORT(&buf[OFFSET_CFG_SVR_TIMING_MARK_CFG_TIMING_MARK],
	     pTn3270s->svr.timing_mark.val);
    
    /* Add the correlators */
    pTn3270s->svr.timing_mark.correlator = pTn3270s->tn3270s_correlator++;
    PUTLONG(&buf[OFFSET_CFG_SVR_TIMING_MARK_CFG_CORRELATOR],
	    pTn3270s->svr.timing_mark.correlator);

    PUTSHORT(&buf[OFFSET_CFG_SVR_TIMING_MARK_RESET_CORRELATOR],
	     cip->reset_correlator);

    tn3270s_cfgcmd(swidb, pTn3270s, cfgtype,
		   buf, bytes);
}
 

/* ---------------------------------------------------------------
 * MAX_FQDN_SIZE 64
 * indx                               CREATE  UPDATE  DELETE  
 *   0 cfg_code            [1]
 *     CIP_TN3270S_SVR_LU_DNS   
 *   1 align_me            [1]
 *   2 dns_fqdn           [64]
 *  66 rev_dns_fqdn       [64]
 * 130 cfg_correlator      [4]  
 * 134 cip_correlator      [2]  
 *    -----------------------------------------------------------
 * 136 CFG_CMDBLOCK_SIZE
 */
#define OFFSET_CFG_CMDCODE                       0
#define OFFSET_CFG_SVR_LU_DNS_FQDN               2
#define OFFSET_CFG_SVR_LU_REV_DNS_FQDN          66
#define OFFSET_CFG_SVR_LU_DNS_CFG_CORRELATOR   130
#define OFFSET_CFG_SVR_LU_DNS_RESET_CORRELATOR 134
#undef  CFG_CMDBLOCK_SIZE
#define CFG_CMDBLOCK_SIZE                      136

static void
tn3270s_cfgcmd_svr_lu_dns (idbtype          *swidb,
			   cip_idb_infotype *cip,
			   tn3270s_t        *pTn3270s,
			   tn3270s_cfg_t     cfgtype)
{
    uchar       buf[CFG_CMDBLOCK_SIZE];
    int         bytes;

    bzero(&buf[0], CFG_CMDBLOCK_SIZE);
    
    bytes = CFG_CMDBLOCK_SIZE;
    buf[OFFSET_CFG_CMDCODE] = CIP_TN3270S_SVR_LU_DNS;

    if (!(pTn3270s->svr.lu_dns.use_default))
	bcopy(pTn3270s->svr.lu_dns.dns_fqdn,
	      &buf[OFFSET_CFG_SVR_LU_DNS_FQDN],
	      MAX_FQDN_SIZE);
    
    if (!(pTn3270s->svr.lu_dns.use_default))
	bcopy(pTn3270s->svr.lu_dns.rev_dns_fqdn,
	      &buf[OFFSET_CFG_SVR_LU_REV_DNS_FQDN],
	      MAX_FQDN_SIZE);
    
    pTn3270s->svr.lu_dns.correlator = pTn3270s->tn3270s_correlator++;
    PUTLONG(&buf[OFFSET_CFG_SVR_LU_DNS_CFG_CORRELATOR],
	    pTn3270s->svr.lu_dns.correlator);

    PUTSHORT(&buf[OFFSET_CFG_SVR_LU_DNS_RESET_CORRELATOR],
	     cip->reset_correlator);

    tn3270s_cfgcmd(swidb, pTn3270s, cfgtype,
		   buf, bytes);
}

/* ---------------------------------------------------------------
 * indx                               CREATE  UPDATE  DELETE  
 *   0 cfg_code            [1]
 *     CIP_TN3270S_SVR_SIFTDOWN          X 
 *   1 tcp_port_flag       [1]
 *   2 tcp_port            [2]
 *   4 idle_time_flag      [1]
 *   5 idle_time           [2]
 *   7 keepalive_flag      [1]
 *   8 keepalive           [2]
 *  10 unbind_act_flag     [1]
 *  11 unbind_act          [2]
 *  13 generic_pool_flag   [1]
 *  14 generic_pool        [2]
 *  16 cfg_correlator      [4]
 *  20 cip_correlator      [2]                    
 *    -----------------------------------------------------------
 *  22 CFG_CMDBLOCK_SIZE
 */
#define OFFSET_CFG_CMDCODE                        0
#define OFFSET_CFG_SVR_SIFTDOWN_TCP_PORT_FLAG     1
#define OFFSET_CFG_SVR_SIFTDOWN_TCP_PORT          2
#define OFFSET_CFG_SVR_SIFTDOWN_IDLE_TIME_FLAG    4
#define OFFSET_CFG_SVR_SIFTDOWN_IDLE_TIME         5
#define OFFSET_CFG_SVR_SIFTDOWN_KEEPALIVE_FLAG    7
#define OFFSET_CFG_SVR_SIFTDOWN_KEEPALIVE         8 
#define OFFSET_CFG_SVR_SIFTDOWN_UNBIND_ACT_FLAG   10
#define OFFSET_CFG_SVR_SIFTDOWN_UNBIND_ACT        11
#define OFFSET_CFG_SVR_SIFTDOWN_GENERIC_POOL_FLAG 13
#define OFFSET_CFG_SVR_SIFTDOWN_GENERIC_POOL      14
#define OFFSET_CFG_SVR_SIFTDOWN_CFG_CORRELATOR    16
#define OFFSET_CFG_SVR_SIFTDOWN_RESET_CORRELATOR  20
#undef  CFG_CMDBLOCK_SIZE
#define CFG_CMDBLOCK_SIZE                         22

#define SIFTDOWN_NO_CHANGE     0
#define SIFTDOWN_USE_VALUE     1
#define SIFTDOWN_APPLY_DEFAULT 2

static void
tn3270s_cfgcmd_svr_siftdown(idbtype          *swidb,
			    cip_idb_infotype *cip,
			    tn3270s_t        *pTn3270s,
			    tn3270s_cfg_t     cfgtype)
{
    uchar       buf[CFG_CMDBLOCK_SIZE];
    int         bytes;

    bzero(&buf[0], CFG_CMDBLOCK_SIZE);
    
    bytes = CFG_CMDBLOCK_SIZE;
    buf[OFFSET_CFG_CMDCODE] = CIP_TN3270S_SVR_SIFTDOWN;

    /* For now, set all flags to SIFTDOWN_USE_VALUE */
    /* Later, pass in a MASK of which have changed (all or one) */

    buf[OFFSET_CFG_SVR_SIFTDOWN_TCP_PORT_FLAG] = SIFTDOWN_USE_VALUE;
    PUTSHORT(&buf[OFFSET_CFG_SVR_SIFTDOWN_TCP_PORT],
	     pTn3270s->svr.siftdown.tcp_port.val);

    buf[OFFSET_CFG_SVR_SIFTDOWN_IDLE_TIME_FLAG] = SIFTDOWN_USE_VALUE;
    PUTSHORT(&buf[OFFSET_CFG_SVR_SIFTDOWN_IDLE_TIME],
	     pTn3270s->svr.siftdown.idle_time.val);

    buf[OFFSET_CFG_SVR_SIFTDOWN_KEEPALIVE_FLAG] = SIFTDOWN_USE_VALUE;
    PUTSHORT(&buf[OFFSET_CFG_SVR_SIFTDOWN_KEEPALIVE],
	     pTn3270s->svr.siftdown.keepalive.val);

    buf[OFFSET_CFG_SVR_SIFTDOWN_UNBIND_ACT_FLAG] = SIFTDOWN_USE_VALUE;
    PUTSHORT(&buf[OFFSET_CFG_SVR_SIFTDOWN_UNBIND_ACT],
	     pTn3270s->svr.siftdown.unbind_act.val);

    buf[OFFSET_CFG_SVR_SIFTDOWN_GENERIC_POOL_FLAG] = SIFTDOWN_USE_VALUE;
    PUTSHORT(&buf[OFFSET_CFG_SVR_SIFTDOWN_GENERIC_POOL],
	     pTn3270s->svr.siftdown.generic_pool.val);
    
    /* Add the correlators */

    pTn3270s->svr.siftdown.correlator = pTn3270s->tn3270s_correlator++;
    PUTLONG(&buf[OFFSET_CFG_SVR_SIFTDOWN_CFG_CORRELATOR],
	    pTn3270s->svr.siftdown.correlator);

    PUTSHORT(&buf[OFFSET_CFG_SVR_SIFTDOWN_RESET_CORRELATOR],
	     cip->reset_correlator);

    tn3270s_cfgcmd(swidb, pTn3270s, cfgtype,
		   buf, bytes);
}


/* ---------------------------------------------------------------
 * indx                               CREATE  UPDATE  DELETE  
 *   0 cfg_code            [1]
 *     CIP_TN3270S_PU_NEW
 *   1 name                [8]
 *   9 index               [4]
 *  13 xid                 [4]
 *  17 ip_address          [4]
 *  21 vc                  [2]
 * - - - - - - - - - - - - - - -      
 *  23 lan_type            [2]
 *  25 adapno              [2]
 *  27 lsap                [1]
 *  28 rmac                [6]
 *  34 rsap                [1]
 *  35 lu_seed             [8]
 * - - - - - - - - - - - - - - -
 *  43  1 tcp_port_flag       [1]
 *  44  2 tcp_port            [2]
 *  46  4 idle_time_flag      [1]
 *  47  5 idle_time           [2]
 *  49  7 keepalive_flag      [1]
 *  50  8 keepalive           [2]
 *  52 10 unbind_act_flag     [1]
 *  53 11 unbind_act          [2]
 *  55 13 generic_pool_flag   [1]
 *  56 14 generic_pool        [2]
 * - - - - - - - - - - - - - - -      
 *  58   name (pu dns_fqdn)  [64]
 * - - - - - - - - - - - - - - -      
 * 122 cfg_correlator      [4]
 * 126 cip_correlator      [2]
 *    -----------------------------------------------------------
 * 128 CFG_CMDBLOCK_SIZE
 */
#define OFFSET_CFG_CMDCODE                   0
#define OFFSET_CFG_PU_NEW_PUNAME             1
#define OFFSET_CFG_PU_NEW_INDEX              9
#define OFFSET_CFG_PU_NEW_XID               13
#define OFFSET_CFG_PU_NEW_IPADDR            17
#define OFFSET_CFG_PU_NEW_VC                21

/* Fields only for for Direct PU */
#define OFFSET_CFG_PU_NEW_LANTYPE           23 
#define OFFSET_CFG_PU_NEW_ADAPNO            25 
#define OFFSET_CFG_PU_NEW_LSAP              27 
#define OFFSET_CFG_PU_NEW_RMAC              28 
#define OFFSET_CFG_PU_NEW_RSAP              34
#define OFFSET_CFG_PU_NEW_LUSEED            35

#define OFFSET_CFG_PU_SIFTDOWN_DIRECT                    43
#define OFFSET_CFG_PU_SIFTDOWN_DLUR                      23

#define REL_OFFSET_CFG_PU_NEW_SIFTDOWN_TCP_PORT_FLAG      0
#define REL_OFFSET_CFG_PU_NEW_SIFTDOWN_TCP_PORT           1
#define REL_OFFSET_CFG_PU_NEW_SIFTDOWN_IDLE_TIME_FLAG     3
#define REL_OFFSET_CFG_PU_NEW_SIFTDOWN_IDLE_TIME          4
#define REL_OFFSET_CFG_PU_NEW_SIFTDOWN_KEEPALIVE_FLAG     6
#define REL_OFFSET_CFG_PU_NEW_SIFTDOWN_KEEPALIVE          7
#define REL_OFFSET_CFG_PU_NEW_SIFTDOWN_UNBIND_ACT_FLAG    9
#define REL_OFFSET_CFG_PU_NEW_SIFTDOWN_UNBIND_ACT        10
#define REL_OFFSET_CFG_PU_NEW_SIFTDOWN_GENERIC_POOL_FLAG 12
#define REL_OFFSET_CFG_PU_NEW_SIFTDOWN_GENERIC_POOL      13

#define REL_OFFSET_CFG_PU_NEW_DNS_FQDN           15

#define REL_OFFSET_CFG_PU_NEW_CFG_CORRELATOR     79
#define REL_OFFSET_CFG_PU_NEW_RESET_CORRELATOR   83
#undef  CFG_CMDBLOCK_SIZE
#define CFG_CMDBLOCK_SIZE                       (85 + OFFSET_CFG_PU_SIFTDOWN_DIRECT)

static void
tn3270s_cfgcmd_pu(idbtype          *swidb,
		  cip_idb_infotype *cip,
		  tn3270s_t        *pTn3270s,
		  tn3270s_pu_t     *pPU,
		  tn3270s_cfg_t     cfgtype)
{
    uchar       buf[CFG_CMDBLOCK_SIZE];
    int         bytes;
    ushort      indx;

    bzero(&buf[0], CFG_CMDBLOCK_SIZE);
    
    bytes = CFG_CMDBLOCK_SIZE;
    
    switch (cfgtype) {
    case TN3270S_CFGTYPE_NOSHUT:
    case TN3270S_CFGTYPE_CREATE:
	/*	
	 * This could happen on initial create:
         * pu foo xid ....
         *    shut
         */
	/* Change new indication */
	pPU->new = FALSE;
	buf[OFFSET_CFG_CMDCODE] = (pPU->type == TN3270S_PUTYPE_DIRECT) ?
	    CIP_TN3270S_PU_NEW : CIP_TN3270S_DLUR_PU_NEW;
	break;
    case TN3270S_CFGTYPE_UPDATE:
	buf[OFFSET_CFG_CMDCODE] = (pPU->type == TN3270S_PUTYPE_DIRECT) ?
	    CIP_TN3270S_PU_UPD : CIP_TN3270S_DLUR_PU_UPD;
	break;
    case TN3270S_CFGTYPE_SHUT  :
    case TN3270S_CFGTYPE_DELETE:
	tn3270s_pu_del_sr(swidb, pTn3270s, pPU);
	buf[OFFSET_CFG_CMDCODE] = (pPU->type == TN3270S_PUTYPE_DIRECT) ?
	    CIP_TN3270S_PU_NEW : CIP_TN3270S_DLUR_PU_NEW;
	buf[OFFSET_CFG_CMDCODE] |= CIP_TEARDOWN_VC;
	break;
    }
    
    bcopy(pPU->name, &buf[OFFSET_CFG_PU_NEW_PUNAME],
	  MAX_NAME_SIZE);

    PUTLONG( &buf[OFFSET_CFG_PU_NEW_INDEX],     pPU->index);
    PUTLONG( &buf[OFFSET_CFG_PU_NEW_XID],       pPU->xid);
    PUTLONG( &buf[OFFSET_CFG_PU_NEW_IPADDR],    pPU->ipaddr);
    PUTSHORT(&buf[OFFSET_CFG_PU_NEW_VC],        pPU->vc);
    
    if (pPU->type == TN3270S_PUTYPE_DIRECT) {
	PUTSHORT(&buf[OFFSET_CFG_PU_NEW_LANTYPE],   pPU->lan_type);
	PUTSHORT(&buf[OFFSET_CFG_PU_NEW_ADAPNO],    pPU->adapno);
	buf[OFFSET_CFG_PU_NEW_LSAP] = pPU->lsap;
	bcopy(pPU->rmac, &buf[OFFSET_CFG_PU_NEW_RMAC],
	      IEEEBYTES);
	buf[OFFSET_CFG_PU_NEW_RSAP] = pPU->rsap;
	bcopy(pPU->lu_seed, &buf[OFFSET_CFG_PU_NEW_LUSEED],
	      MAX_NAME_SIZE);
	indx = OFFSET_CFG_PU_SIFTDOWN_DIRECT;                  
    } else {
	indx = OFFSET_CFG_PU_SIFTDOWN_DLUR;
    }
    
    /* Siftdown */

    buf[indx+REL_OFFSET_CFG_PU_NEW_SIFTDOWN_TCP_PORT_FLAG] = 
	(pPU->siftdown.tcp_port.use_default ? 
	 SIFTDOWN_APPLY_DEFAULT :
	 SIFTDOWN_USE_VALUE);
    PUTSHORT(&buf[indx+REL_OFFSET_CFG_PU_NEW_SIFTDOWN_TCP_PORT],
	     pPU->siftdown.tcp_port.val);
    
    buf[indx+REL_OFFSET_CFG_PU_NEW_SIFTDOWN_IDLE_TIME_FLAG] =
	(pPU->siftdown.idle_time.use_default ? 
	 SIFTDOWN_APPLY_DEFAULT :
	 SIFTDOWN_USE_VALUE);
    PUTSHORT(&buf[indx+REL_OFFSET_CFG_PU_NEW_SIFTDOWN_IDLE_TIME],
	     pPU->siftdown.idle_time.val);
    
    buf[indx+REL_OFFSET_CFG_PU_NEW_SIFTDOWN_KEEPALIVE_FLAG] =
	(pPU->siftdown.keepalive.use_default ? 
	 SIFTDOWN_APPLY_DEFAULT :
	 SIFTDOWN_USE_VALUE);
    PUTSHORT(&buf[indx+REL_OFFSET_CFG_PU_NEW_SIFTDOWN_KEEPALIVE],
	     pPU->siftdown.keepalive.val);
    
    buf[indx+REL_OFFSET_CFG_PU_NEW_SIFTDOWN_UNBIND_ACT_FLAG] =
	(pPU->siftdown.unbind_act.use_default ? 
	 SIFTDOWN_APPLY_DEFAULT :
	 SIFTDOWN_USE_VALUE);
    PUTSHORT(&buf[indx+REL_OFFSET_CFG_PU_NEW_SIFTDOWN_UNBIND_ACT],
	     pPU->siftdown.unbind_act.val);
    
    buf[indx+REL_OFFSET_CFG_PU_NEW_SIFTDOWN_GENERIC_POOL_FLAG] =
	(pPU->siftdown.generic_pool.use_default ? 
	 SIFTDOWN_APPLY_DEFAULT :
	 SIFTDOWN_USE_VALUE);
    PUTSHORT(&buf[indx+REL_OFFSET_CFG_PU_NEW_SIFTDOWN_GENERIC_POOL],
	     pPU->siftdown.generic_pool.val);
    
    
    /* pu-dns-fqdn */
    if (!(pPU->pu_dns.use_default))
	bcopy(pPU->pu_dns.name, &buf[indx+REL_OFFSET_CFG_PU_NEW_DNS_FQDN],
	      MAX_FQDN_SIZE);
    
    /* Add the correlators */
    pPU->correlator = pTn3270s->tn3270s_correlator++;
    PUTLONG(&buf[indx+REL_OFFSET_CFG_PU_NEW_CFG_CORRELATOR],
	    pPU->correlator);
    PUTSHORT(&buf[indx+REL_OFFSET_CFG_PU_NEW_RESET_CORRELATOR],
	     cip->reset_correlator);

    bytes = indx+REL_OFFSET_CFG_PU_NEW_RESET_CORRELATOR+
	sizeof(cip->reset_correlator);

    tn3270s_cfgcmd(swidb, pPU, cfgtype,
		   buf, bytes);
}

/* ---------------------------------------------------------------
 * indx                
 *   0 cfg_code            [1]
 *     CIP_TN3270S_DLUR
 *   1 align_me            [1]
 *   2 dlur_cpname        [17]
 *  19 dlur_dlusname      [17]
 *  36 cfg_correlator      [4]
 *  40 cip_correlator      [2]
 *    -----------------------------------------------------------
 *  42 CFG_CMDBLOCK_SIZE
 */
#define OFFSET_CFG_CMDCODE                   0
#define OFFSET_CFG_DLUR_CPNAME               2
#define OFFSET_CFG_DLUR_DLUSNAME            19

#define OFFSET_CFG_DLUR_CORRELATOR_FIELD    36
#define OFFSET_CFG_NO_DLUR_CORRELATOR_FIELD  2

#define REL_OFFSET_CFG_DLUR_CFG_CORRELATOR      0
#define REL_OFFSET_CFG_DLUR_RESET_CORRELATOR    4
#undef  CFG_CMDBLOCK_SIZE
#define CFG_CMDBLOCK_SIZE                   42

static void
tn3270s_cfgcmd_dlur(idbtype          *swidb,
		    cip_idb_infotype *cip,
		    tn3270s_t        *pTn3270s,
		    tn3270s_cfg_t     cfgtype)
{
    uchar       buf[CFG_CMDBLOCK_SIZE];
    int         bytes;
    tn3270s_dlur_t *pDLUR;
    ushort      indx;

    pDLUR = &pTn3270s->dlur;

    bzero(&buf[0], CFG_CMDBLOCK_SIZE);
    
    bytes = CFG_CMDBLOCK_SIZE;
    switch (cfgtype) {
    case TN3270S_CFGTYPE_NOSHUT:
    case TN3270S_CFGTYPE_CREATE:
    case TN3270S_CFGTYPE_UPDATE:
	buf[OFFSET_CFG_CMDCODE] = CIP_TN3270S_DLUR;
	break;
    case TN3270S_CFGTYPE_SHUT  :
    case TN3270S_CFGTYPE_DELETE:
	buf[OFFSET_CFG_CMDCODE] = CIP_TN3270S_DLUR | CIP_TEARDOWN_VC;
	break;
    }
    
    /* ??? No fields on a [no] dlur command ??? */
    
    if (!(buf[OFFSET_CFG_CMDCODE] & CIP_TEARDOWN_VC)) {
	bcopy(pDLUR->dlur_fq_cpname, &buf[OFFSET_CFG_DLUR_CPNAME],
	      MAX_FQNN_SIZE);
	bcopy(pDLUR->dlur_fq_dlusname, &buf[OFFSET_CFG_DLUR_DLUSNAME],
	      MAX_FQNN_SIZE);
    	indx = OFFSET_CFG_DLUR_CORRELATOR_FIELD;
    } else {
	indx = OFFSET_CFG_NO_DLUR_CORRELATOR_FIELD;
    }
    
    /* Add the correlators */
    pDLUR->correlator = pTn3270s->tn3270s_correlator++;
    PUTLONG(&buf[indx+REL_OFFSET_CFG_DLUR_CFG_CORRELATOR],
	    pDLUR->correlator);
    PUTSHORT(&buf[indx+REL_OFFSET_CFG_DLUR_RESET_CORRELATOR],
	     cip->reset_correlator);
    
    bytes = indx+REL_OFFSET_CFG_DLUR_RESET_CORRELATOR+
	sizeof(cip->reset_correlator);
    
    tn3270s_cfgcmd(swidb, pDLUR, cfgtype,
		   buf, bytes);
}

/* ---------------------------------------------------------------
 * indx                
 *   0 cfg_code            [1]
 *     CIP_TN3270S_DLUR_DLUS
 *   1 name               [17]
 *  18 cfg_correlator      [4]
 *  22 cip_correlator      [2]
 *    -----------------------------------------------------------
 *  26 CFG_CMDBLOCK_SIZE
 */
#define OFFSET_CFG_CMDCODE                      0
#define OFFSET_CFG_DLUR_DLUS_NAME               1 

#define OFFSET_CFG_DLUR_DLUS_CFG_CORRELATOR    18
#define OFFSET_CFG_DLUR_DLUS_RESET_CORRELATOR  22
#undef  CFG_CMDBLOCK_SIZE
#define CFG_CMDBLOCK_SIZE                      24

static void
tn3270s_cfgcmd_dlur_dlus(idbtype          *swidb,
			 cip_idb_infotype *cip,
			 tn3270s_t        *pTn3270s,
			 tn3270s_cfg_t     cfgtype)
{
    uchar       buf[CFG_CMDBLOCK_SIZE];
    int         bytes;
    tn3270s_dlur_t *pDLUR;
    
    pDLUR = &pTn3270s->dlur;

    bzero(&buf[0], CFG_CMDBLOCK_SIZE);
    
    bytes = CFG_CMDBLOCK_SIZE;
    buf[OFFSET_CFG_CMDCODE] = CIP_TN3270S_DLUR_DLUS;

    /* ??? Leave field null if set to use default ?? */
    if (!pDLUR->dlur_dlus_backup.use_default)
	bcopy(pDLUR->dlur_dlus_backup.name, &buf[OFFSET_CFG_DLUR_DLUS_NAME],
	      MAX_FQNN_SIZE);
    
    /* Add the correlators */
    pDLUR->dlur_dlus_backup.correlator = pTn3270s->tn3270s_correlator++;
    PUTLONG(&buf[OFFSET_CFG_DLUR_DLUS_CFG_CORRELATOR],
	    pDLUR->dlur_dlus_backup.correlator);
    PUTSHORT(&buf[OFFSET_CFG_DLUR_DLUS_RESET_CORRELATOR],
	     cip->reset_correlator);

    tn3270s_cfgcmd(swidb, pDLUR, cfgtype,
		   buf, bytes);
}

/* ---------------------------------------------------------------
 * indx                
 *   0 cfg_code            [1]
 *     CIP_TN3270S_DLUR_NN_SVR
 *   1 name               [17]
 *  18 cfg_correlator      [4]
 *  22 cip_correlator      [2]
 *    -----------------------------------------------------------
 *  26 CFG_CMDBLOCK_SIZE
 */
#define OFFSET_CFG_CMDCODE                        0
#define OFFSET_CFG_DLUR_NN_SVR_NAME               1 

#define OFFSET_CFG_DLUR_NN_SVR_CFG_CORRELATOR    18
#define OFFSET_CFG_DLUR_NN_SVR_RESET_CORRELATOR  22
#undef  CFG_CMDBLOCK_SIZE
#define CFG_CMDBLOCK_SIZE                        24

static void
tn3270s_cfgcmd_dlur_nn_svr(idbtype          *swidb,
			   cip_idb_infotype *cip,
			   tn3270s_t        *pTn3270s,
			   tn3270s_cfg_t     cfgtype)
{
    uchar       buf[CFG_CMDBLOCK_SIZE];
    int         bytes;
    tn3270s_dlur_t *pDLUR;
    
    pDLUR = &pTn3270s->dlur;

    bzero(&buf[0], CFG_CMDBLOCK_SIZE);
    
    bytes = CFG_CMDBLOCK_SIZE;

    buf[OFFSET_CFG_CMDCODE] = CIP_TN3270S_DLUR_NN_SVR;

    /* ??? Leave field null if set to use default ?? */
    if (!pDLUR->dlur_nn_svr.use_default)
	bcopy(pDLUR->dlur_nn_svr.name, &buf[OFFSET_CFG_DLUR_NN_SVR_NAME],
	      MAX_FQNN_SIZE);
    
    /* Add the correlators */
    pDLUR->dlur_nn_svr.correlator = pTn3270s->tn3270s_correlator++;
    PUTLONG(&buf[OFFSET_CFG_DLUR_NN_SVR_CFG_CORRELATOR],
	    pDLUR->dlur_nn_svr.correlator);
    PUTSHORT(&buf[OFFSET_CFG_DLUR_NN_SVR_RESET_CORRELATOR],
	     cip->reset_correlator);

    tn3270s_cfgcmd(swidb, pDLUR, cfgtype,
		   buf, bytes);
}


/* ---------------------------------------------------------------
 * indx                               CREATE  UPDATE  DELETE  
 *   0 cfg_code            [1]
 *     CIP_TN3270S_DLUR_SAP
 * - - - - - - - - - - - - - - -      
 *   1 lan_type            [2]
 *   3 adapno              [2]
 *   5 lsap                [1]
 * - - - - - - - - - - - - - - -      
 *   6 cfg_correlator      [4]
 *  10 cip_correlator      [2]
 *    -----------------------------------------------------------
 *  12 CFG_CMDBLOCK_SIZE
 */
#define OFFSET_CFG_CMDCODE                     0
#define OFFSET_CFG_DLUR_SAP_LANTYPE            1 
#define OFFSET_CFG_DLUR_SAP_ADAPNO             3 
#define OFFSET_CFG_DLUR_SAP_LSAP               5 

#define OFFSET_CFG_DLUR_SAP_CFG_CORRELATOR     6 
#define OFFSET_CFG_DLUR_SAP_RESET_CORRELATOR  10 
#undef  CFG_CMDBLOCK_SIZE
#define CFG_CMDBLOCK_SIZE                     12

static void
tn3270s_cfgcmd_dlur_sap(idbtype            *swidb,
			cip_idb_infotype   *cip,
			tn3270s_t          *pTn3270s,
			tn3270s_dlur_sap_t *pSAP,
			tn3270s_cfg_t       cfgtype)
{
    uchar       buf[CFG_CMDBLOCK_SIZE];
    int         bytes;

    bzero(&buf[0], CFG_CMDBLOCK_SIZE);
    
    bytes = CFG_CMDBLOCK_SIZE;
    
    switch (cfgtype) {
    case TN3270S_CFGTYPE_CREATE:
    case TN3270S_CFGTYPE_UPDATE:
    case TN3270S_CFGTYPE_NOSHUT:
	buf[OFFSET_CFG_CMDCODE] = CIP_TN3270S_DLUR_SAP;
	break;
    case TN3270S_CFGTYPE_SHUT  :
    case TN3270S_CFGTYPE_DELETE:
	buf[OFFSET_CFG_CMDCODE] = CIP_TN3270S_DLUR_SAP | CIP_TEARDOWN_VC;
	break;
    }

    PUTSHORT(&buf[OFFSET_CFG_DLUR_SAP_LANTYPE],   pSAP->lan_type);
    PUTSHORT(&buf[OFFSET_CFG_DLUR_SAP_ADAPNO],    pSAP->adapno);
    buf[OFFSET_CFG_DLUR_SAP_LSAP] = pSAP->lsap;
    
    /* Add the correlators */
    pSAP->correlator = pTn3270s->tn3270s_correlator++;
    PUTLONG(&buf[OFFSET_CFG_DLUR_SAP_CFG_CORRELATOR],
	    pSAP->correlator);
    PUTSHORT(&buf[OFFSET_CFG_DLUR_SAP_RESET_CORRELATOR],
	     cip->reset_correlator);

    tn3270s_cfgcmd(swidb, pSAP, cfgtype,
		   buf, bytes);
}

/* ---------------------------------------------------------------
 * indx                               CREATE  UPDATE  DELETE  
 *   0 cfg_code            [1]
 *     CIP_TN3270S_DLUR_SAP_VRN
 *   1 unused              [1]
 * - - - - - - - - - - - - - - -      
 *   2 lan_type            [2]
 *   4 adapno              [2]
 *   6 lsap                [1]
 * - - - - - - - - - - - - - - -      
 *   7 vrn_name           [17]
 * - - - - - - - - - - - - - - -      
 *  24 cfg_correlator      [4]
 *  28 cip_correlator      [2]
 *    -----------------------------------------------------------
 *  30 CFG_CMDBLOCK_SIZE
 */
#define OFFSET_CFG_CMDCODE                     0
#define OFFSET_CFG_SAP_VRN_LANTYPE            2 
#define OFFSET_CFG_SAP_VRN_ADAPNO             4 
#define OFFSET_CFG_SAP_VRN_LSAP               6 
#define OFFSET_CFG_SAP_VRN_NAME               7

#define OFFSET_CFG_SAP_VRN_CFG_CORRELATOR    24 
#define OFFSET_CFG_SAP_VRN_RESET_CORRELATOR  28
#undef  CFG_CMDBLOCK_SIZE
#define CFG_CMDBLOCK_SIZE                    30

static void
tn3270s_cfgcmd_dlur_sap_vrn(idbtype            *swidb,
			    cip_idb_infotype   *cip,
			    tn3270s_t          *pTn3270s,
			    tn3270s_dlur_sap_t *pSAP,
			    tn3270s_cfg_t       cfgtype)
{
    uchar       buf[CFG_CMDBLOCK_SIZE];
    int         bytes;

    bzero(&buf[0], CFG_CMDBLOCK_SIZE);
    
    bytes = CFG_CMDBLOCK_SIZE;
    
    buf[OFFSET_CFG_CMDCODE] = CIP_TN3270S_DLUR_SAP_VRN;

    PUTSHORT(&buf[OFFSET_CFG_SAP_VRN_LANTYPE],   pSAP->lan_type);
    PUTSHORT(&buf[OFFSET_CFG_SAP_VRN_ADAPNO],    pSAP->adapno);
    buf[OFFSET_CFG_SAP_VRN_LSAP] = pSAP->lsap;
    
    /* ??? Leave field null if set to use default ?? */
    if (!pSAP->dlur_sap_vrn.use_default)
	bcopy(pSAP->dlur_sap_vrn.name, &buf[OFFSET_CFG_SAP_VRN_NAME],
	      MAX_FQNN_SIZE);

    /* Add the correlators */
    pSAP->dlur_sap_vrn.correlator = pTn3270s->tn3270s_correlator++;
    PUTLONG(&buf[OFFSET_CFG_SAP_VRN_CFG_CORRELATOR],
	    pSAP->dlur_sap_vrn.correlator);
    PUTSHORT(&buf[OFFSET_CFG_SAP_VRN_RESET_CORRELATOR],
	     cip->reset_correlator);

    tn3270s_cfgcmd(swidb, pSAP, cfgtype,
		   buf, bytes);

}

/* ---------------------------------------------------------------
 * indx                               CREATE  UPDATE  DELETE  
 *   0 cfg_code            [1]
 *     CIP_TN3270S_DLUR_SAP_LNK
 *   1 unused              [1]
 * - - - - - - - - - - - - - - -      
 *   2 lan_type            [2]
 *   4 adapno              [2]
 *   6 lsap                [1]
 * - - - - - - - - - - - - - - -      
 *   7 name                [8]
 *  15 rmac                [6]
 *  21 rsap                [1]
 * - - - - - - - - - - - - - - -      
 *  22 cfg_correlator      [4]
 *  26 cip_correlator      [2]
 *    -----------------------------------------------------------
 *  28 CFG_CMDBLOCK_SIZE
 */
#define OFFSET_CFG_CMDCODE                     0
#define OFFSET_CFG_SAP_LINK_LANTYPE            2 
#define OFFSET_CFG_SAP_LINK_ADAPNO             4 
#define OFFSET_CFG_SAP_LINK_LSAP               6 
#define OFFSET_CFG_SAP_LINK_NAME               7
#define OFFSET_CFG_SAP_LINK_RMAC              15
#define OFFSET_CFG_SAP_LINK_RSAP              21

#define OFFSET_CFG_SAP_LINK_CFG_CORRELATOR    22 
#define OFFSET_CFG_SAP_LINK_RESET_CORRELATOR  26 
#undef  CFG_CMDBLOCK_SIZE
#define CFG_CMDBLOCK_SIZE                      28

static void
tn3270s_cfgcmd_dlur_sap_link(idbtype            *swidb,
			     cip_idb_infotype   *cip,
			     tn3270s_t          *pTn3270s,
			     tn3270s_dlur_sap_t *pSAP,
			     tn3270s_link_t     *pLink,
			     tn3270s_cfg_t       cfgtype)
{
    uchar       buf[CFG_CMDBLOCK_SIZE];
    int         bytes;

    bzero(&buf[0], CFG_CMDBLOCK_SIZE);
    
    bytes = CFG_CMDBLOCK_SIZE;
    
    switch (cfgtype) {
    case TN3270S_CFGTYPE_CREATE:
    case TN3270S_CFGTYPE_UPDATE:
    case TN3270S_CFGTYPE_NOSHUT:
	buf[OFFSET_CFG_CMDCODE] = CIP_TN3270S_DLUR_SAP_LNK;
	break;
    case TN3270S_CFGTYPE_SHUT  :
    case TN3270S_CFGTYPE_DELETE:
	buf[OFFSET_CFG_CMDCODE] = CIP_TN3270S_DLUR_SAP_LNK | CIP_TEARDOWN_VC;
	break;
    }

    PUTSHORT(&buf[OFFSET_CFG_SAP_LINK_LANTYPE],   pSAP->lan_type);
    PUTSHORT(&buf[OFFSET_CFG_SAP_LINK_ADAPNO],    pSAP->adapno);
    buf[OFFSET_CFG_SAP_LINK_LSAP] = pSAP->lsap;

    bcopy(pLink->name, &buf[OFFSET_CFG_SAP_LINK_NAME],
	  MAX_NAME_SIZE);
 
    bcopy(pLink->rmac, &buf[OFFSET_CFG_SAP_LINK_RMAC],
	  IEEEBYTES);
    buf[OFFSET_CFG_SAP_LINK_RSAP] = pLink->rsap;

    /* Add the correlators */
    pLink->correlator = pTn3270s->tn3270s_correlator++;
    PUTLONG(&buf[OFFSET_CFG_SAP_LINK_CFG_CORRELATOR],
	    pLink->correlator);
    PUTSHORT(&buf[OFFSET_CFG_SAP_LINK_RESET_CORRELATOR],
	     cip->reset_correlator);

    tn3270s_cfgcmd(swidb, pLink, cfgtype,
		   buf, bytes);

}

/*
 * ==============================================================
 * Miscellaneous utility functions
 * ==============================================================
 */
/*
 * ---------------------------------------------------------------
 *  The CIP tn3270 server uses "ff00.0000.0000" to designate a
 *  loopback mac.
 *
 * tn3270s_is_loopback_mac
 *  - returns TRUE if the passed in macaddress is a loopback mac
 * tn3270s_loopback_mac
 *  - initializes macaddress to loopback mac
 * tn3270s_rmac_string
 *  - returns a formatted string containing the macaddress
 *    passed in or the character representation of the loopback mac
 * ---------------------------------------------------------------
 */
static boolean
tn3270s_is_loopback_mac (uchar const *addr)
{
   return(ieee_equal(addr, loopback_mac));
}

static void
tn3270s_loopback_mac (uchar *dst)
{
    ieee_copy(loopback_mac, dst);
}

static const char *
tn3270s_rmac_string (uchar *rmac)
{
    if (tn3270s_is_loopback_mac(rmac))
	sstrncpy(macaddr_string, TN3270S_SHOW_STR_LOOPBACK,
		 sizeof(macaddr_string));
    else
	sprintf(macaddr_string, "%14e", rmac);
    return(macaddr_string);
}
/*
 * ---------------------------------------------------------------
 * tn3270s_lan_type_string
 *  return the parser KEYWORD that corresponds to the lan_type
 * ---------------------------------------------------------------
 */
static const char *
tn3270s_lan_type_string (ushort lan_type)
{
    switch (lan_type) {
    case CSNA_VLAN_ETHER :
	return TN3270S_KEY_ETHER_ADAPTER;
    case CSNA_VLAN_TOKEN :
	return TN3270S_KEY_TOKEN_ADAPTER;
    case CSNA_VLAN_FDDI  :
	return TN3270S_KEY_FDDI_ADAPTER;
    default              :
	return "?";
    }
}
/*
 * ---------------------------------------------------------------
 * tn3270s_port_type_string
 *  return a root APPN NM 'port' name given the lan_type
 * ---------------------------------------------------------------
 */
static const char *
tn3270s_port_type_string (ushort lan_type)
{
    switch (lan_type) {
    case CSNA_VLAN_ETHER :
	return "ETPort";
    case CSNA_VLAN_TOKEN :
	return "TRPort";
    case CSNA_VLAN_FDDI  :
	return "FDPort";
    default              :
	return "XXPort";
    }
}







/*
 * ---------------------------------------------------------------
 * tn3270s_name_from_fqname 
 *   - return a pointer to the name portion of a fully
 *     qualified SNA network name, or to the name itself
 *     if not fully qualified.
 *
 *   Search backward and return ptr to first char after '.'
 *   If no '.' found, return start of string
 * ---------------------------------------------------------------
 */
static char *
tn3270s_name_from_fqname (char *pFQname)
{
    char *p;
    p = strrchr(pFQname,'.');
    if (p)
	p++;
    else
	p = pFQname;

    return p;
}

/*
 * ===========================================================================
 * Reset, Send or Delete a resource and its sub resources
 * ===========================================================================
 *
 *   o _reset_() routines
 *       The interface has been reset, or
 *       A shut has been done on a resource.
 *       take care of any sub resource concerns, eg. delete static routes
 *
 *   o _send_() routines
 *       The interface has been reset, or
 *       A [no] shut has been done on a resource.
 *       send the configuration of all sub resources.
 *
 *   o _delete_() routines
 *       A [no] has been done on a resource.
 *       all sub resources are to be deleted before sending
 *       the command to the CIP
 *
 * ===========================================================================
 */
/*
 * TN3270S_STATE_UNUSED   0x0000
 * TN3270S_STATE_INUSE    0x0001
 * TN3270S_STATE_CFGING   0x0002
 * TN3270S_STATE_CREATE   0x0004
 * TN3270S_STATE_UPDATE   0x0008
 * TN3270S_STATE_DELETE   0x0010
 * TN3270S_STATE_PNDING   0x0020
 * TN3270S_STATE_ACKED    0x0040
 * TN3270S_STATE_VC_SR    0x0080
 * TN3270S_STATE_SHUT     0x0100
 * TN3270S_STATE_CIP_SHUT 0x0200
 */
#define BLANKS "..."
typedef void (*output_t)(const char *, ...);

static void
tn3270s_print_state_flags (ushort   state,
			   char    *pString,
			   output_t output)
{
    char  buf[128];
    sprintf(buf,"%04x %s %s %s %s %s %s %s %s %s",
	    state,
	    (state & TN3270S_STATE_INUSE )   ? "INU" : BLANKS,
	    (state & TN3270S_STATE_CFGING)   ? "CFG" : BLANKS,
	    (state & TN3270S_STATE_CREATE)   ? "CRE" : BLANKS,
	    (state & TN3270S_STATE_UPDATE)   ? "UPD" : BLANKS,
	    (state & TN3270S_STATE_DELETE)   ? "DEL" : BLANKS,
	    (state & TN3270S_STATE_PNDING)   ? "PND" : BLANKS,
	    (state & TN3270S_STATE_VC_SR )   ? "SR " : BLANKS,
	    (state & TN3270S_STATE_SHUT)     ? "SHUT": "....",
	    (state & TN3270S_STATE_CIP_SHUT) ? "CIP-SHUT" : "........");
    
    (*output)("%s%s", pString, buf);
}
static void
tn3270s_buginf_svr_state_flags (tn3270s_t *pTn3270s)
{
    if (DEBUG_TN3270S && channel_debug) {
	tn3270s_print_state_flags(pTn3270s->svr.state,
				  "\ntn3270-server state:",
				  buginf);
    }
}

static void
tn3270s_buginf_pu_state_flags (tn3270s_pu_t *pPU)
{
    if (DEBUG_TN3270S && channel_debug) {
	buginf("\nPU %8s state:",
	       pPU->name);
	tn3270s_print_state_flags(pPU->state, "",
				  buginf);
    }
}

static void
tn3270s_buginf_dlur_state_flags (tn3270s_dlur_t *pDLUR)
{
    if (DEBUG_TN3270S && channel_debug) {
	buginf("\nDLUR %s %s state:",
               pDLUR->dlur_fq_cpname,
	       pDLUR->dlur_fq_dlusname);
	
	tn3270s_print_state_flags(pDLUR->state, "",
				  buginf);
    }
}
static void
tn3270s_buginf_sap_show (char               *pString,
			 tn3270s_dlur_sap_t *pSAP)
{	
    
    if (!pSAP)  {
	buginf("%s%s",
	       pString,
	       "** none **");
    }    else {
	buginf("%s%s %-2d %02X",
	       pString,
	       tn3270s_lan_type_string(pSAP->lan_type),
	       pSAP->adapno,
	       pSAP->lsap);
    }   
}

static void
tn3270s_reset_this_link_cfg (idbtype            *swidb,
			     cip_idb_infotype   *cip,
			     tn3270s_t          *pTn3270s,
			     tn3270s_dlur_sap_t *pSAP,
			     tn3270s_link_t     *pLink)
{
    if (DEBUG_TN3270S && channel_debug)
	buginf("\n\tReset Link %s",
	       pLink->name);
    
    if (pLink->state & TN3270S_STATE_DELETE) {
	tn3270s_link_entry_delete(pTn3270s, pLink);
    } else {
	pLink->state      &= ~TN3270S_STATE_CIP_SHUT;
	pLink->state      &= ~TN3270S_STATE_PNDING;
	pLink->state      &= ~TN3270S_STATE_CREATE;
    }
}

static void
tn3270s_reset_all_link_cfg (idbtype            *swidb,
			    cip_idb_infotype   *cip,
			    tn3270s_t          *pTn3270s,
			    tn3270s_dlur_sap_t *pSAP)
{
    tn3270s_link_t     *pLink;
    tn3270s_link_t     *pNext;

    if (DEBUG_TN3270S && channel_debug)
	buginf("\nReset configuration for tn3270-server"
	       " DLUR SAP Link entries");
    
    /* For all links on this SAP */
    pLink = tn3270s_link_entry_get_first(pTn3270s, pSAP,
					 TN3270S_LINK_CONTEXT_THIS_SAP);
    while (pLink) {
	pNext = tn3270s_link_entry_get_next(pTn3270s, pSAP, pLink,
					    TN3270S_LINK_CONTEXT_THIS_SAP);
	tn3270s_reset_this_link_cfg(swidb, cip, pTn3270s, pSAP, pLink);
	pLink = pNext;
    }
}

static void
tn3270s_reset_this_sap_cfg (idbtype            *swidb,
			    cip_idb_infotype   *cip,
			    tn3270s_t          *pTn3270s,
			    tn3270s_dlur_sap_t *pSAP)
    
{
    if (DEBUG_TN3270S && channel_debug)
	tn3270s_buginf_sap_show("\n\tReset SAP ",
				pSAP);
    if (pSAP->state & TN3270S_STATE_DELETE) {
	tn3270s_sap_entry_delete(pTn3270s, pSAP);
    } else {
	/* Take out of PNDING and remove CIP_SHUT status */
	pSAP->state      &= ~TN3270S_STATE_CIP_SHUT;
	pSAP->state      &= ~TN3270S_STATE_PNDING;
	pSAP->state      &= ~TN3270S_STATE_CREATE;
	/* Reset all configured links on this SAP */
	tn3270s_reset_all_link_cfg(swidb, cip, pTn3270s, pSAP);
    }
}   

static void
tn3270s_reset_all_sap_cfg (idbtype          *swidb,
			   cip_idb_infotype *cip,
			   tn3270s_t        *pTn3270s)
{
    tn3270s_dlur_sap_t     *pSAP;
    tn3270s_dlur_sap_t     *pNext;

    if (DEBUG_TN3270S && channel_debug)
	buginf("\nReset configuration for tn3270-server"
	       " DLUR SAP entries");

    pSAP = tn3270s_sap_entry_get_first(pTn3270s);
    while (pSAP) {
	pNext = tn3270s_sap_entry_get_next(pTn3270s, pSAP);
	tn3270s_reset_this_sap_cfg(swidb, cip, pTn3270s, pSAP);
	pSAP = pNext;
    }
}    
    

/* Reset all configurations of specific PU type */
static void
tn3270s_reset_this_pu_cfg (idbtype          *swidb,
			   cip_idb_infotype *cip,
			   tn3270s_t        *pTn3270s, 
			   tn3270s_pu_t     *pPU)
{
    if (DEBUG_TN3270S && channel_debug)
	buginf("\n\tReset %s PU %s",
	       (pPU->type == TN3270S_PUTYPE_DIRECT) ? "DIRECT" : "DLUR",
	       pPU->name);

    tn3270s_buginf_pu_state_flags(pPU);

    /* If pending delete, then delete it */
    if (pPU->state & TN3270S_STATE_DELETE) {
	tn3270s_ipvc_entry_del_vc_user(pTn3270s, pPU->vc);
	tn3270s_pu_entry_delete(pTn3270s, pPU);
    } else {
	/* Otherwise, remove the Static route */
	tn3270s_pu_del_sr(swidb, pTn3270s, pPU);
	
	/* Take out of PNDING and remove CIP_SHUT status */
	pPU->state &= ~TN3270S_STATE_PNDING;
	pPU->state &= ~TN3270S_STATE_CREATE;
	pPU->state &= ~TN3270S_STATE_CIP_SHUT;
    }
}

/* Reset all configurations of specific PU type */
static void
tn3270s_reset_all_pu_cfg (idbtype          *swidb,
			  cip_idb_infotype *cip,
			  tn3270s_t        *pTn3270s, 
			  tn3270s_putype_t  pu_type)
{
    tn3270s_pu_t     *pPU;
    tn3270s_pu_t     *pNext;

    if (DEBUG_TN3270S && channel_debug)
	buginf("\nReset configuration for tn3270-server"
	       " %s PU entries",
	       (pu_type == TN3270S_PUTYPE_DIRECT) ? "DIRECT" : "DLUR");
    
    pPU = tn3270s_pu_entry_get_first(pTn3270s);
    while (pPU) {
	pNext = tn3270s_pu_entry_get_next(pTn3270s, pPU);
	if (pPU->type == pu_type) {
	    tn3270s_reset_this_pu_cfg(swidb, cip, pTn3270s, pPU);
	}
	pPU = pNext;
    }
}    
    
static void
tn3270s_reset_all_dlur_cfg (idbtype          *swidb,
			    cip_idb_infotype *cip,
			    tn3270s_t        *pTn3270s)
{
    tn3270s_dlur_t     *pDLUR;

    if (DEBUG_TN3270S && channel_debug)
	buginf("\nReset configuration for tn3270-server"
	       " DLUR ");

    /* Reset all DLUR resources*/
    pDLUR = &pTn3270s->dlur;
    tn3270s_buginf_dlur_state_flags(pDLUR);

    if (pDLUR->state & TN3270S_STATE_DELETE) {
	tn3270s_dlur_delete(pTn3270s);
    }
    if (!(pDLUR->state & TN3270S_STATE_INUSE))
	return;
    
    /* Take out of PNDING and remove CIP_SHUT status */
    pDLUR->state      &= ~TN3270S_STATE_CIP_SHUT;
    pDLUR->state      &= ~TN3270S_STATE_PNDING;
    pDLUR->state      &= ~TN3270S_STATE_CREATE;
    
    /* Reset all DLUR PUs resources*/
    tn3270s_reset_all_pu_cfg(swidb, cip, pTn3270s, TN3270S_PUTYPE_DLUR);
    
    /* Reset all DLUR SAP resources*/
    tn3270s_reset_all_sap_cfg(swidb, cip, pTn3270s);
}

/* Either the interface is being reset, or the svr is being shut */

static void
tn3270s_reset_all_svr_cfg (idbtype          *swidb,
			   cip_idb_infotype *cip,
			   tn3270s_t        *pTn3270s)
{    
    if (DEBUG_TN3270S && channel_debug)
	buginf("\nReset configuration for tn3270-server");
    
    /* Take out of PNDING and remove CIP_SHUT status */
    pTn3270s->svr.state      &= ~TN3270S_STATE_CIP_SHUT;
    pTn3270s->svr.state      &= ~TN3270S_STATE_PNDING;
    pTn3270s->svr.state      &= ~TN3270S_STATE_CREATE;

    /* Reset all Direct PUs */
    tn3270s_reset_all_pu_cfg(swidb, cip, pTn3270s, TN3270S_PUTYPE_DIRECT);
    
    tn3270s_reset_all_dlur_cfg(swidb, cip, pTn3270s);
}

static void
tn3270s_send_this_link_cfg (idbtype            *swidb,
			    cip_idb_infotype   *cip,
			    tn3270s_t          *pTn3270s,
			    tn3270s_dlur_sap_t *pSAP,
			    tn3270s_link_t     *pLink)
{
    if (DEBUG_TN3270S && channel_debug)
	buginf("\n\tSend Link %s",
	       pLink->name);
    
    if (!(pLink->state & TN3270S_STATE_SHUT))
	tn3270s_cfgcmd_dlur_sap_link(swidb, cip, pTn3270s,
				     pSAP, pLink, TN3270S_CFGTYPE_NOSHUT);
}

static void
tn3270s_send_all_link_cfg (idbtype            *swidb,
			   cip_idb_infotype   *cip,
			   tn3270s_t          *pTn3270s,
			   tn3270s_dlur_sap_t *pSAP)
{
    tn3270s_link_t     *pLink;

    if (DEBUG_TN3270S && channel_debug)
	buginf("\nSending configuration for tn3270-server"
	       " DLUR SAP Link entries");
    
    /* For all links on this SAP */
    pLink = tn3270s_link_entry_get_first(pTn3270s, pSAP,
					 TN3270S_LINK_CONTEXT_THIS_SAP);
    while (pLink) {
	tn3270s_send_this_link_cfg(swidb, cip, pTn3270s, pSAP, pLink);
	pLink = tn3270s_link_entry_get_next(pTn3270s, pSAP, pLink,
					    TN3270S_LINK_CONTEXT_THIS_SAP);
    }
}
static void
tn3270s_send_this_sap_cfg (idbtype            *swidb,
			   cip_idb_infotype   *cip,
			   tn3270s_t          *pTn3270s,
			   tn3270s_dlur_sap_t *pSAP)
{
    if (DEBUG_TN3270S && channel_debug)
	tn3270s_buginf_sap_show("\n\tSend SAP ",
				pSAP);
    if (!(pSAP->state & TN3270S_STATE_SHUT)) {
	tn3270s_cfgcmd_dlur_sap(swidb, cip, pTn3270s,
				pSAP, TN3270S_CFGTYPE_NOSHUT);
	/* send vrn if not defaulted */
	if (!pSAP->dlur_sap_vrn.use_default)
	    tn3270s_cfgcmd_dlur_sap_vrn(swidb, cip, pTn3270s,
					pSAP, TN3270S_CFGTYPE_UPDATE);
	
	/* And send all configured links on this SAP */
	tn3270s_send_all_link_cfg(swidb, cip, pTn3270s, pSAP);
    }
}

static void
tn3270s_send_all_sap_cfg (idbtype          *swidb,
			  cip_idb_infotype *cip,
			  tn3270s_t        *pTn3270s)
{
    tn3270s_dlur_sap_t     *pSAP;

    if (DEBUG_TN3270S && channel_debug)
	buginf("\nSending configuration for tn3270-server"
	       " DLUR SAP entries");

    pSAP = tn3270s_sap_entry_get_first(pTn3270s);
    while (pSAP) {
	tn3270s_send_this_sap_cfg(swidb, cip, pTn3270s, pSAP);
	pSAP = tn3270s_sap_entry_get_next(pTn3270s, pSAP);
    }
}    

static void
tn3270s_send_this_pu_cfg (idbtype          *swidb,
			  cip_idb_infotype *cip,
			  tn3270s_t        *pTn3270s, 
			  tn3270s_pu_t     *pPU)
{
    if (DEBUG_TN3270S && channel_debug)
	buginf("\n\tSend %s PU %s",
	       (pPU->type == TN3270S_PUTYPE_DIRECT) ? "DIRECT" : "DLUR",
	       pPU->name);
    
    if (!(pPU->state & TN3270S_STATE_SHUT)) {
	/* Siftdown and pu-dns are sent in this as well */
	tn3270s_cfgcmd_pu(swidb, cip, 
			  pTn3270s, pPU, TN3270S_CFGTYPE_NOSHUT);
    }
}    
/* Send all configurations of specific PU type */
static void
tn3270s_send_all_pu_cfg (idbtype          *swidb,
			 cip_idb_infotype *cip,
			 tn3270s_t        *pTn3270s, 
			 tn3270s_putype_t  pu_type)
{
    tn3270s_pu_t     *pPU;

    if (DEBUG_TN3270S && channel_debug)
	buginf("\nSending configuration for tn3270-server"
	       " %s PU entries",
	       (pu_type == TN3270S_PUTYPE_DIRECT) ? "DIRECT" : "DLUR");
    
    pPU = tn3270s_pu_entry_get_first(pTn3270s);
    while (pPU) {
	    if (pPU->type == pu_type) {
		tn3270s_send_this_pu_cfg(swidb, cip, pTn3270s, pPU);
	    }
	pPU = tn3270s_pu_entry_get_next(pTn3270s, pPU);
    }
}

static void
tn3270s_send_all_dlur_cfg (idbtype          *swidb,
			   cip_idb_infotype *cip,
			   tn3270s_t        *pTn3270s)
{
    tn3270s_dlur_t     *pDLUR;

    if (DEBUG_TN3270S && channel_debug)
	buginf("\nSending configuration for tn3270-server"
	       " DLUR ");
    
    /* Send all non-shut DLUR resources*/
    pDLUR = &pTn3270s->dlur;
    
    if (!(pDLUR->state & TN3270S_STATE_INUSE))
	return;
    
    if (pDLUR->state & TN3270S_STATE_SHUT)
	return;
    
    tn3270s_cfgcmd_dlur(swidb, cip, 
			pTn3270s, TN3270S_CFGTYPE_NOSHUT);
    
    /* Need to send dlus-backup and pref-NN-svr if not defaulted */
    if (!pDLUR->dlur_dlus_backup.use_default)
	tn3270s_cfgcmd_dlur_dlus(swidb, cip, 
				 pTn3270s, TN3270S_CFGTYPE_UPDATE);

    if (!pDLUR->dlur_nn_svr.use_default)
	tn3270s_cfgcmd_dlur_nn_svr(swidb, cip, 
				   pTn3270s, TN3270S_CFGTYPE_UPDATE);

    /* Send all non-shut DLUR PUs resources*/
    tn3270s_send_all_pu_cfg(swidb, cip, pTn3270s, TN3270S_PUTYPE_DLUR);
    
    /* Send all non-shut DLUR SAP resources*/
    tn3270s_send_all_sap_cfg(swidb, cip, pTn3270s);
}

static void
tn3270s_send_all_svr_cfg (idbtype          *swidb,
			  cip_idb_infotype *cip,
			  tn3270s_t        *pTn3270s)
{    
    if (DEBUG_TN3270S && channel_debug)
	buginf("\nSending configuration for tn3270-server");

    if (!(pTn3270s->svr.state & TN3270S_STATE_INUSE))
	return;
    
    if (pTn3270s->svr.state & TN3270S_STATE_SHUT)
	return;

    /* tn3270-server */    
    tn3270s_cfgcmd_svr(swidb, cip, 
		       pTn3270s, TN3270S_CFGTYPE_NOSHUT);

    /* lu-dns-domain-stem */
    if (!pTn3270s->svr.lu_dns.use_default)
	tn3270s_cfgcmd_svr_lu_dns(swidb, cip, 
				  pTn3270s, TN3270S_CFGTYPE_UPDATE);
    /* tcp-port idle-time keepalive unbind-action generic-pool */
    if ((!pTn3270s->svr.siftdown.tcp_port.use_default) ||
	(!pTn3270s->svr.siftdown.idle_time.use_default) ||
	(!pTn3270s->svr.siftdown.keepalive.use_default) ||
	(!pTn3270s->svr.siftdown.unbind_act.use_default) ||
	(!pTn3270s->svr.siftdown.generic_pool.use_default))
	
	tn3270s_cfgcmd_svr_siftdown(swidb, cip, 
				    pTn3270s, TN3270S_CFGTYPE_UPDATE);
    /* max-lu */
    if (!pTn3270s->svr.max_lu.use_default)
	tn3270s_cfgcmd_svr_max_lu(swidb, cip, 
				  pTn3270s, TN3270S_CFGTYPE_UPDATE);
 
   tn3270s_cfgcmd_svr_timing_mark(swidb, cip, 
				  pTn3270s, TN3270S_CFGTYPE_UPDATE);
    
    /* Send all non-shut Direct PUs */
    tn3270s_send_all_pu_cfg(swidb, cip, pTn3270s, TN3270S_PUTYPE_DIRECT);
    
    tn3270s_send_all_dlur_cfg(swidb, cip, pTn3270s);
}

static void
tn3270s_delete_this_link_cfg (idbtype            *swidb,
			      cip_idb_infotype   *cip,
			      tn3270s_t          *pTn3270s,
			      tn3270s_dlur_sap_t *pSAP,
			      tn3270s_link_t     *pLink)
{
    if (DEBUG_TN3270S && channel_debug)
	buginf("\n\tDelete Link %s",
	       pLink->name);
 
    tn3270s_link_entry_delete(pTn3270s, pLink);
}

static void
tn3270s_delete_all_link_cfg (idbtype            *swidb,
			     cip_idb_infotype   *cip,
			     tn3270s_t          *pTn3270s,
			     tn3270s_dlur_sap_t *pSAP)
{
    tn3270s_link_t     *pLink;
    tn3270s_link_t     *pNext;

    if (DEBUG_TN3270S && channel_debug)
	buginf("\nDeleting configuration for tn3270-server"
	       " DLUR SAP Link entries");
    
    /* For all links on this SAP */
    pLink = tn3270s_link_entry_get_first(pTn3270s, pSAP,
					 TN3270S_LINK_CONTEXT_THIS_SAP);
    while (pLink) {
	pNext = tn3270s_link_entry_get_next(pTn3270s, pSAP, pLink,
					    TN3270S_LINK_CONTEXT_THIS_SAP);
	tn3270s_delete_this_link_cfg(swidb, cip, pTn3270s, pSAP, pLink);
	pLink = pNext;
    }
}
    
static void
tn3270s_delete_this_sap_cfg (idbtype            *swidb,
			     cip_idb_infotype   *cip,
			     tn3270s_t          *pTn3270s,
			     tn3270s_dlur_sap_t *pSAP)
    
{
    if (DEBUG_TN3270S && channel_debug)
	tn3270s_buginf_sap_show("\n\tDelete SAP ",
				pSAP);
    
    /* Delete all configured links on this SAP */
    tn3270s_delete_all_link_cfg(swidb, cip, pTn3270s, pSAP);
    tn3270s_sap_entry_delete(pTn3270s, pSAP);
}

static void
tn3270s_delete_all_sap_cfg (idbtype          *swidb,
			    cip_idb_infotype *cip,
			    tn3270s_t        *pTn3270s)
{
    tn3270s_dlur_sap_t     *pSAP;
    tn3270s_dlur_sap_t     *pNext;

    if (DEBUG_TN3270S && channel_debug)
	buginf("\nDeleting configuration for tn3270-server"
	       " DLUR SAP entries");

    pSAP = tn3270s_sap_entry_get_first(pTn3270s);
    while (pSAP) {
	pNext = tn3270s_sap_entry_get_next(pTn3270s, pSAP);
	tn3270s_delete_this_sap_cfg(swidb, cip, pTn3270s, pSAP);
	pSAP = pNext;
    }
}    
    
/* Delete all configurations of specific PU type */
static void
tn3270s_delete_this_pu_cfg (idbtype          *swidb,
			    cip_idb_infotype *cip,
			    tn3270s_t        *pTn3270s, 
			    tn3270s_pu_t     *pPU)
{
    if (DEBUG_TN3270S && channel_debug)
	buginf("\n\tDelete %s PU %s",
	       (pPU->type == TN3270S_PUTYPE_DIRECT) ? "DIRECT" : "DLUR",
	       pPU->name);
    
    tn3270s_pu_del_sr(swidb, pTn3270s, pPU);
    tn3270s_ipvc_entry_del_vc_user(pTn3270s, pPU->vc);
    tn3270s_pu_entry_delete(pTn3270s, pPU);
}    
/* Delete all configurations of specific PU type */
static void
tn3270s_delete_all_pu_cfg (idbtype          *swidb,
			   cip_idb_infotype *cip,
			   tn3270s_t        *pTn3270s, 
			   tn3270s_putype_t  pu_type)
{
    tn3270s_pu_t     *pPU;
    tn3270s_pu_t     *pNext;

    if (DEBUG_TN3270S && channel_debug)
	buginf("\nDeleting configuration for tn3270-server"
	       " %s PU entries",
	       (pu_type == TN3270S_PUTYPE_DIRECT) ? "DIRECT" : "DLUR");
    
    pPU = tn3270s_pu_entry_get_first(pTn3270s);
    while (pPU) {
	pNext = tn3270s_pu_entry_get_next(pTn3270s, pPU);
	if (pPU->type == pu_type) {
	    tn3270s_delete_this_pu_cfg(swidb, cip, pTn3270s, pPU);
	}
	pPU = pNext;
    }
}    

static void
tn3270s_delete_all_dlur_cfg (idbtype          *swidb,
			     cip_idb_infotype *cip,
			     tn3270s_t        *pTn3270s)
{
    tn3270s_dlur_t     *pDLUR;

    if (DEBUG_TN3270S && channel_debug)
	buginf("\nDeleting configuration for tn3270-server"
	       " DLUR ");
    
    /* Delete all DLUR resources*/
    pDLUR = &pTn3270s->dlur;
    
    if (!(pDLUR->state & TN3270S_STATE_INUSE))
	return;

    /* Delete all DLUR PUs resources*/
    tn3270s_delete_all_pu_cfg(swidb, cip, pTn3270s, TN3270S_PUTYPE_DLUR);
    
    /* Delete all DLUR SAP resources*/
    tn3270s_delete_all_sap_cfg(swidb, cip, pTn3270s);
}

static void
tn3270s_delete_all_svr_cfg (idbtype          *swidb,
			    cip_idb_infotype *cip,
			    tn3270s_t        *pTn3270s)
{    
    if (DEBUG_TN3270S && channel_debug)
	buginf("\nDeleting configuration for tn3270-server");

    /* Delete all Direct PUs */
    tn3270s_delete_all_pu_cfg(swidb, cip, pTn3270s, TN3270S_PUTYPE_DIRECT);
    
    tn3270s_delete_all_dlur_cfg(swidb, cip, pTn3270s);

    /* Now delete dlur */
    tn3270s_dlur_delete(pTn3270s);

}
/*
 * ===========================================================================
 * Process configuration acknowledgements from the CIP.
 * ===========================================================================
 */
/*
 * ---------------------------------------------------------------------
 * tn3270s_cfg_identify
 *    Find the specific resource that is being acknowledged
 * ---------------------------------------------------------------------
 */
static void *
tn3270s_cfg_identify (uchar     cfg_cmd,
		      uchar     *buf,
		      tn3270s_t *pTn3270s)
{
    void               *p;
    tn3270s_dlur_t     *pDLUR;
    tn3270s_dlur_sap_t *pSAP;
    tn3270s_link_t     *pLink;
    tn3270s_pu_t       *pPU;
    char               *pName;
    ushort              lan_type;
    ushort              adapno;
    uchar               lsap;
					  
    switch (cfg_cmd & ~CIP_TEARDOWN_VC) {
    case CIP_TN3270S_SVR         : 
    case CIP_TN3270S_SVR_LU_DNS  : 
    case CIP_TN3270S_SVR_MAX_LU  : 
    case CIP_TN3270S_SVR_TIMING_MARK  : 
    case CIP_TN3270S_SVR_SIFTDOWN: 
	p = &pTn3270s->svr;
	return p;
    case CIP_TN3270S_PU_NEW      : 
    case CIP_TN3270S_DLUR_PU_NEW : 
    case CIP_TN3270S_PU_UPD      : /* Not sure yet about UPD correlator */
    case CIP_TN3270S_DLUR_PU_UPD : 
	/* find by name */
        pName = &buf[OFFSET_CFG_PU_NEW_PUNAME];
	pPU = tn3270s_pu_entry_locate_by_name(pTn3270s, pName);
	return pPU;
	break;
    case CIP_TN3270S_DLUR        : 
    case CIP_TN3270S_DLUR_DLUS   : 
    case CIP_TN3270S_DLUR_NN_SVR : 
	pDLUR = &pTn3270s->dlur;
	return pDLUR;
    case CIP_TN3270S_DLUR_SAP    : 
	lan_type = GETSHORT(&buf[OFFSET_CFG_DLUR_SAP_LANTYPE]);
	adapno = GETSHORT(&buf[OFFSET_CFG_DLUR_SAP_ADAPNO]);
	lsap = buf[OFFSET_CFG_DLUR_SAP_LSAP];
	pSAP = tn3270s_sap_entry_locate(pTn3270s, lan_type,
					adapno, lsap);
	return pSAP;
    case CIP_TN3270S_DLUR_SAP_VRN:  
	lan_type = GETSHORT(&buf[OFFSET_CFG_SAP_VRN_LANTYPE]);
	adapno = GETSHORT(&buf[OFFSET_CFG_SAP_VRN_ADAPNO]);
	lsap = buf[OFFSET_CFG_SAP_VRN_LSAP];
	pSAP = tn3270s_sap_entry_locate(pTn3270s, lan_type,
					adapno, lsap);
	return pSAP;
    case CIP_TN3270S_DLUR_SAP_LNK: 
	/* find by name */
        pName = &buf[OFFSET_CFG_SAP_LINK_NAME];
	pLink = tn3270s_link_entry_locate_by_name(pTn3270s, pName);
	return pLink;
    default:
	buginf("\nhuh cfg_cmd %d", cfg_cmd);
    }
    return NULL;
}

/*
 * ---------------------------------------------------------------------
 * tn3270s_cfg_correlator
 *    Lookup the current config correlator
 * ---------------------------------------------------------------------
 */
static ulong
tn3270s_cfg_correlator (uchar     cfg_cmd,
			void      *p,
			tn3270s_t *pTn3270s)
{
    tn3270s_dlur_t     *pDLUR;
    tn3270s_dlur_sap_t *pSAP;
    tn3270s_link_t     *pLink;
    tn3270s_pu_t       *pPU;

    if (!p)
	return TN3270S_CORRELATOR_HUH;
    
    pDLUR = p;
    pSAP = p;
    pLink = p;
    pPU = p;
    
    switch (cfg_cmd & ~CIP_TEARDOWN_VC) {
    case CIP_TN3270S_SVR         : 
	return pTn3270s->svr.correlator;
    case CIP_TN3270S_SVR_LU_DNS  : 
	return pTn3270s->svr.lu_dns.correlator;
    case CIP_TN3270S_SVR_MAX_LU  : 
	return pTn3270s->svr.max_lu.correlator;
    case CIP_TN3270S_SVR_TIMING_MARK :
	return pTn3270s->svr.timing_mark.correlator;
    case CIP_TN3270S_SVR_SIFTDOWN: 
	return pTn3270s->svr.siftdown.correlator;
    case CIP_TN3270S_PU_NEW      : 
    case CIP_TN3270S_DLUR_PU_NEW : 
    case CIP_TN3270S_PU_UPD      :
    case CIP_TN3270S_DLUR_PU_UPD : 
	return pPU->correlator;
    case CIP_TN3270S_DLUR        : 
	return pDLUR->correlator;
    case CIP_TN3270S_DLUR_DLUS   : 
	return pDLUR->dlur_dlus_backup.correlator;
    case CIP_TN3270S_DLUR_NN_SVR : 
	return pDLUR->dlur_nn_svr.correlator;
    case CIP_TN3270S_DLUR_SAP    : 
	return pSAP->correlator;
    case CIP_TN3270S_DLUR_SAP_VRN:  
	return pSAP->dlur_sap_vrn.correlator;
    case CIP_TN3270S_DLUR_SAP_LNK: 
	return pLink->correlator;
    default:
	buginf("\nhuh cfg_cmd %d", cfg_cmd);
    }
    return 0;
}

/*
 * ---------------------------------------------------------------------
 * tn3270s_cfg_accepted
 *    Positive acknowledgement of a subchannel cfg
 *
 *    o Add static route if IVORIE or DP,
 *      CLAW and OFFLOAD static routes will be added
 *      explicitly by CIP request
 * ---------------------------------------------------------------------
 */
static void
tn3270s_cfg_accepted (hwidbtype  *hwidb,
		      ushort      ack_flags,
		      ushort      cfg_cmd,
		      void       *p,
		      tn3270s_t  *pTn3270s)
{
    tn3270s_dlur_t     *pDLUR;
    tn3270s_dlur_sap_t *pSAP;
    tn3270s_link_t     *pLink;
    tn3270s_pu_t       *pPU;

    pDLUR = p;
    pSAP = p;
    pLink = p;
    pPU = p;

    switch (cfg_cmd) {
    case CIP_TN3270S_SVR         : 
	pTn3270s->svr.state &= ~TN3270S_STATE_PNDING;
	pTn3270s->svr.state &= ~TN3270S_STATE_CREATE;
	break;
    case CIP_TN3270S_SVR_LU_DNS  : 
	break;
    case CIP_TN3270S_SVR_MAX_LU  : 
    case CIP_TN3270S_SVR_TIMING_MARK  : 
	break;
    case CIP_TN3270S_SVR_SIFTDOWN: 
	break;
    case CIP_TN3270S_PU_NEW      : 
    case CIP_TN3270S_DLUR_PU_NEW : 
    case CIP_TN3270S_PU_UPD      : 
    case CIP_TN3270S_DLUR_PU_UPD : 
	pPU->state &= ~TN3270S_STATE_PNDING;
	pPU->state &= ~TN3270S_STATE_CREATE;

	/* If this is a [no] shut or create and acked by CIP */
	/* could delete this check if pu_add_sr checked for SHUT */
	if ((cfg_cmd == CIP_TN3270S_PU_NEW) || 
	    (cfg_cmd == CIP_TN3270S_DLUR_PU_NEW))
	    if (ack_flags & CIP_CFG_ACK_CIP) {
		tn3270s_pu_add_sr(hwidb->firstsw, pTn3270s, pPU);
	    }
	break;
    case CIP_TN3270S_DLUR        : 
	pDLUR->state &= ~TN3270S_STATE_PNDING;
	pDLUR->state &= ~TN3270S_STATE_CREATE;
	break;
    case CIP_TN3270S_DLUR_DLUS   : 
	break;
    case CIP_TN3270S_DLUR_NN_SVR : 
	break;
    case CIP_TN3270S_DLUR_SAP    : 
	pSAP->state &= ~TN3270S_STATE_PNDING;
	pSAP->state &= ~TN3270S_STATE_CREATE;
	break;
    case CIP_TN3270S_DLUR_SAP_VRN:  
	break;
    case CIP_TN3270S_DLUR_SAP_LNK: 
	pLink->state &= ~TN3270S_STATE_PNDING;
	pLink->state &= ~TN3270S_STATE_CREATE;
	break;
    }
}

/*
 * ---------------------------------------------------------------------
 * tn3270s_nack_report
 *    Negative acknowledgement
 * ---------------------------------------------------------------------
 */
static void
tn3270s_nack_report (hwidbtype *hwidb,
		     boolean    teardown,
		     ushort     cfg_cmd,
		     void      *cfginfo)
{
    cip_idb_infotype *cip;
    char errbuf[32];
    
    cip = channel_sb_get_cip_info(hwidb);
    if (!cip)
	return;
    
    /* We are here because either the CIP rejected the cfg command,
     * or becuase the cfg command could not be sent to the CIP 
     * due to some 'unusual' reason, eg. the CIP cbus timeout 
     */
    
    switch (cfg_cmd) {
    default:
	sprintf(errbuf, " cmd %d",
		cfg_cmd);
	break;
    }
    errmsg(&msgsym(CIPCFGFAIL, CBUS), 
	   hwidb->hw_namestring,
	   cip_cfg_name_str(cfg_cmd), 
	   errbuf);
    return;
}

/*
 * ---------------------------------------------------------------------
 * tn3270s_cfg_rejected
 *    Negative acknowledgement of a subchannel cfg
 * ---------------------------------------------------------------------
 */
static void
tn3270s_cfg_rejected (hwidbtype  *hwidb,
		      ushort      ack_flags,
		      ushort      cfg_cmd,
		      void       *p,
		      tn3270s_t  *pTn3270s)
{
    tn3270s_dlur_t     *pDLUR;
    tn3270s_dlur_sap_t *pSAP;
    tn3270s_link_t     *pLink;
    tn3270s_pu_t       *pPU;

    pDLUR = p;
    pSAP = p;
    pLink = p;
    pPU = p;

    tn3270s_nack_report(hwidb, (cfg_cmd & CIP_TEARDOWN_VC),
		    (cfg_cmd & ~CIP_TEARDOWN_VC), pTn3270s);

    switch (cfg_cmd) {
    case CIP_TN3270S_SVR         : 
	pTn3270s->svr.state &= ~TN3270S_STATE_PNDING;
	pTn3270s->svr.state &= ~TN3270S_STATE_CREATE;
	pTn3270s->svr.state |= TN3270S_STATE_CIP_SHUT;
	break;
    case CIP_TN3270S_SVR_LU_DNS  : 
	break;
    case CIP_TN3270S_SVR_MAX_LU  : 
    case CIP_TN3270S_SVR_TIMING_MARK  : 
	break;
    case CIP_TN3270S_SVR_SIFTDOWN: 
	break;
    case CIP_TN3270S_PU_NEW      : 
    case CIP_TN3270S_DLUR_PU_NEW : 
	pPU->state &= ~TN3270S_STATE_PNDING;
	pPU->state &= ~TN3270S_STATE_CREATE;
	pPU->state |= TN3270S_STATE_CIP_SHUT;
	break;
    case CIP_TN3270S_PU_UPD      : 
    case CIP_TN3270S_DLUR_PU_UPD : 
	break;
    case CIP_TN3270S_DLUR        : 
	pDLUR->state &= ~TN3270S_STATE_PNDING;
	pDLUR->state &= ~TN3270S_STATE_CREATE;
	pDLUR->state |= TN3270S_STATE_CIP_SHUT;
	break;
    case CIP_TN3270S_DLUR_DLUS   : 
	break;
    case CIP_TN3270S_DLUR_NN_SVR : 
	break;
    case CIP_TN3270S_DLUR_SAP    : 
	pSAP->state &= ~TN3270S_STATE_PNDING;
	pSAP->state &= ~TN3270S_STATE_CREATE;
	pSAP->state |= TN3270S_STATE_CIP_SHUT;
	break;
    case CIP_TN3270S_DLUR_SAP_VRN:  
	break;
    case CIP_TN3270S_DLUR_SAP_LNK: 
	pLink->state &= ~TN3270S_STATE_PNDING;
	pLink->state &= ~TN3270S_STATE_CREATE;
	pLink->state |= TN3270S_STATE_CIP_SHUT;
	break;
    }
}
/*
 * ---------------------------------------------------------------------
 * State change dependent on cfg acknowledgement 
 *    o Remove the asynchronous state change PREQ element
 * ---------------------------------------------------------------------
 *             ACK                             NACK
 *  STATE_INUSE  ==>                 STATE_INUSE  ==>               
 *  STATE_CREATE ==> ~STATE_CREATE   STATE_CREATE ==> ~STATE_CREATE
 *  STATE_PNDING ==> ~STATE_PNDING   STATE_PNDING ==> ~STATE_PNDING
 *                                                ==>  STATE_CIP_SHUT
 * delete                         
 *  STATE_INUSE  ==>  STATE_UNUSED   STATE_INUSE  ==>  ???
 *  STATE_CREATE ==>                 STATE_CREATE ==>  ???         
 *  STATE_PNDING ==>                 STATE_PNDING ==>  ???         
 * 
 * TN3270S_STATE_UNUSED   0x0000
 * TN3270S_STATE_INUSE    0x0001
 * TN3270S_STATE_CFGING   0x0002
 * TN3270S_STATE_CREATE   0x0004
 * TN3270S_STATE_UPDATE   0x0008
 * TN3270S_STATE_DELETE   0x0010
 * TN3270S_STATE_PNDING   0x0020
 * TN3270S_STATE_SHUT     0x0040
 * TN3270S_STATE_CIP_SHUT 0x0080
 */
/*
 * ---------------------------------------------------------------------
 * tn3270s_del_accepted
 *    Positive acknowledgement of a subchannel cfg delete
 *
 *    o Finish the teardown by calling cip_teardown_vc
 *    o Remove the asynchronous state change PREQ element
 * ---------------------------------------------------------------------
 */
static void
tn3270s_del_accepted (hwidbtype  *hwidb,
		      ushort      ack_flags,
		      ushort      cfg_cmd,
		      void       *p,
		      tn3270s_t  *pTn3270s)
{
    idbtype            *swidb;
    cip_idb_infotype   *cip;
    tn3270s_dlur_t     *pDLUR;
    tn3270s_dlur_sap_t *pSAP;
    tn3270s_link_t     *pLink;
    tn3270s_pu_t       *pPU;

    cip = channel_sb_get_cip_info(hwidb);
    swidb = hwidb->firstsw;
    
    pDLUR = p;
    pSAP = p;
    pLink = p;
    pPU = p;
    
    switch (cfg_cmd & ~CIP_TEARDOWN_VC) {
    case CIP_TN3270S_SVR         :
	/* PNDING delete or PNDING shut ? */
	pTn3270s->svr.state &= ~TN3270S_STATE_PNDING;
	if (pTn3270s->svr.state & TN3270S_STATE_DELETE) {
	    tn3270s_svr_delete(pTn3270s);
	}
	break;
    case CIP_TN3270S_SVR_LU_DNS  : 
	break;
    case CIP_TN3270S_SVR_MAX_LU  : 
    case CIP_TN3270S_SVR_TIMING_MARK  : 
	break;
    case CIP_TN3270S_SVR_SIFTDOWN: 
	break;
    case CIP_TN3270S_PU_NEW      : 
    case CIP_TN3270S_DLUR_PU_NEW : 
	pPU->state &= ~TN3270S_STATE_PNDING;
	if (pPU->state & TN3270S_STATE_DELETE) {
	    tn3270s_delete_this_pu_cfg(swidb, cip, pTn3270s, pPU);
	}
	break;
    case CIP_TN3270S_PU_UPD      : 
    case CIP_TN3270S_DLUR_PU_UPD :
	break;
    case CIP_TN3270S_DLUR        : 
	pDLUR->state &= ~TN3270S_STATE_PNDING;
	if (pDLUR->state & TN3270S_STATE_DELETE) {
	    tn3270s_dlur_delete(pTn3270s);
	}
	break;
    case CIP_TN3270S_DLUR_DLUS   : 
	break;
    case CIP_TN3270S_DLUR_NN_SVR : 
	break;
    case CIP_TN3270S_DLUR_SAP    : 
	pSAP->state &= ~TN3270S_STATE_PNDING;
	if (pSAP->state & TN3270S_STATE_DELETE) {
	    tn3270s_delete_this_sap_cfg(swidb, cip, pTn3270s, pSAP);
	}
	break;
    case CIP_TN3270S_DLUR_SAP_VRN:  
	break;
    case CIP_TN3270S_DLUR_SAP_LNK: 
	pLink->state &= ~TN3270S_STATE_PNDING;
	    tn3270s_delete_this_link_cfg(swidb, cip, pTn3270s, pSAP, pLink);
	break;
    }

}
/*
 * ---------------------------------------------------------------------
 * tn3270s_del_rejected
 *    Negative acknowledgement of a subchannel cfg delete
 *    This may be called if the delete request could not be sent to 
 *    the CIP
 *
 *    o Undo the initial teardown attempt:
 *      Reset the CIP_TEARDOWN_VC bit.
 *      Note: static route will already have been removed.
 * ---------------------------------------------------------------------
 */
static void
tn3270s_del_rejected (hwidbtype  *hwidb,
		      ushort      ack_flags,
		      ushort      cfg_cmd,
		      void       *p,
		      tn3270s_t  *pTn3270s)
{
    tn3270s_dlur_t     *pDLUR;
    tn3270s_dlur_sap_t *pSAP;
    tn3270s_link_t     *pLink;
    tn3270s_pu_t       *pPU;

    pDLUR = p;
    pSAP = p;
    pLink = p;
    pPU = p;

    /* Hmmm... Nacking a delete */
    tn3270s_nack_report(hwidb, (cfg_cmd & CIP_TEARDOWN_VC),
			(cfg_cmd & ~CIP_TEARDOWN_VC), pTn3270s);
}

/*
 * ---------------------------------------------------------------------
 * tn3270s_unexpected_cfg_ack
 *    Negative acknowledgement
 * ---------------------------------------------------------------------
 */
static void
tn3270s_unexpected_cfg_ack (idbtype *swidb,
			    char    *pDescription,
			    uchar   *buf,
			    ulong    len)
{
    if (channel_debug) {
	buginf("\nGot unexpected acknowledgment: %s, %s",
	       swidb->namestring, pDescription);
	if (channel_debug_blocks)
	    memory_dump(buf, &buf[len-1],
			FALSE, MEMORY_DUMP_DATA_TYPE_ASCII, buginf);
    }
}
/*
 * ---------------------------------------------------------------
 * A configuration has been acknowledged by the CIP 
 * --- this is called at process level ---
 *
 * NOTE: Also handle the messages (via errmsg) for all failures:
 *       if not positive ack, and locally called, then the send 
 *       failed.
 *       If not positive ack, and came from CIP , the CIP also
 *       (probably) sent up a logger message explaining why
 *
 * Values of ack_flags
 *        CIP_CFG_ACK_CIP      0x0001   CIP   acknowledgement
 *        CIP_CFG_ACK_LOCAL    0x0002   local acknowledgement
 *
 * Values of code:
 *        CIP_CFG_NACK    -- Negative response to a cfg command
 *        CIP_CFG_ACK     -- Positive response to a cfg command
 *
 * ---------------------------------------------------------------
 */
static void
tn3270s_cfg_acknowledge (hwidbtype *hwidb,
			 ushort     ack_flags,
			 ulong      code,
			 uchar     *buf,
			 ulong      len)
{
    idbtype          *swidb;
    cip_idb_infotype *cip;
    uchar             cfg_cmd;
    boolean           teardown;
    boolean           positive_ack;
    tn3270s_t        *pTn3270s;
    ulong             cfg_correlator;
    ulong             exp_correlator;
    ushort            correlator;
    void             *p;
    
    swidb = hwidb->firstsw;
    cip = channel_sb_get_cip_info(hwidb);
    if (!cip)
	return;
    
    if (!cip->pTn3270sData) {
	tn3270s_unexpected_cfg_ack(swidb,"No Tn3270s context",
				   buf, len);
	return;
    }
    
    pTn3270s = cip->pTn3270sData;
    
    cfg_cmd = buf[OFFSET_CFG_CMDCODE];

    if (cfg_cmd & CIP_TEARDOWN_VC)
	teardown = TRUE;
    else
	teardown = FALSE;
       
    if (code == CIP_CFG_ACK)
	positive_ack = TRUE;
    else
	positive_ack = FALSE;
    /*
     * First get the expected cfg correlator
     * and a pointer to the specific cfg item that is being acked
     */
    p = tn3270s_cfg_identify(cfg_cmd, buf, pTn3270s);
    
    exp_correlator = tn3270s_cfg_correlator(cfg_cmd, p, pTn3270s);

    switch (cfg_cmd & ~CIP_TEARDOWN_VC) {
    case CHANNEL_NODEID:
	/* currently no state info kept on this beast           */
        /* No action for either positive or negative acknowledgment */
	return;
    case CIP_TN3270S_SVR         : 
    case CIP_TN3270S_SVR_LU_DNS  : 
    case CIP_TN3270S_SVR_MAX_LU  : 
    case CIP_TN3270S_SVR_TIMING_MARK  : 
    case CIP_TN3270S_SVR_SIFTDOWN: 
    case CIP_TN3270S_PU_NEW      : 
    case CIP_TN3270S_PU_UPD      : 
    case CIP_TN3270S_DLUR_PU_NEW : 
    case CIP_TN3270S_DLUR_PU_UPD : 
    case CIP_TN3270S_DLUR        : 
    case CIP_TN3270S_DLUR_DLUS   : 
    case CIP_TN3270S_DLUR_NN_SVR : 
    case CIP_TN3270S_DLUR_SAP    : 
    case CIP_TN3270S_DLUR_SAP_VRN:  
    case CIP_TN3270S_DLUR_SAP_LNK: 
	correlator = GETSHORT(&buf[len-sizeof(correlator)]);
	cfg_correlator = GETLONG(&buf[len-sizeof(correlator)-sizeof(cfg_correlator)]);

	if (channel_debug) 
	    buginf("\n%s:(%04X,%04X) (%08X,%08X) Received %s ack for %s%s",
		   hwidb->hw_namestring, cip->reset_correlator, correlator,
		   exp_correlator, cfg_correlator,
		   positive_ack ? "POSITIVE" : "NEGATIVE",
		   (teardown) ? "no " : "",
		   cip_cfg_name_str(cfg_cmd));
	 	
	if (cip->reset_correlator != correlator) {
	    tn3270s_unexpected_cfg_ack(swidb,"Correlator mismatch", 
				       buf, len);
	    return;
	}

	/* 
	 * Check for nonexistant cfg resource and mismatched correlatr
	 */
	if (!p) {
	    tn3270s_unexpected_cfg_ack(swidb,"Resource does not exist", 
				       buf, len);
	    return;
	}
	
	if (exp_correlator != cfg_correlator) {
	    tn3270s_unexpected_cfg_ack(swidb,
				       "Ignoring cfg_correlator mismatch", 
				       buf, len);
	    return;
	}
	   
	/* Turn off the PNDING status bit */
	/* No PNDING status bit for tn3270s */

	if (teardown) {
	    if (positive_ack) {
		tn3270s_del_accepted(hwidb, ack_flags, cfg_cmd,
				     p, pTn3270s);
	    } else {
		tn3270s_del_rejected(hwidb, ack_flags, cfg_cmd,
				     p, pTn3270s);
	    }
	    return;
	}
	
        /* Otherwise new or update */
	if (positive_ack) {
	    tn3270s_cfg_accepted(hwidb, ack_flags, cfg_cmd,
				 p, pTn3270s);
	} else {
	    tn3270s_cfg_rejected(hwidb, ack_flags, cfg_cmd,
				 p, pTn3270s);
	}
	break;
    default:
	tn3270s_unexpected_cfg_ack(swidb,"Unexpected command code",
				   buf, len);
	return;
    }
}

/*
 * ==============================================================
 * Parser action routines: utility functions
 * ==============================================================
 */
/*
 * ---------------------------------------------------------------
 * tn3270s_default
 * ---------------------------------------------------------------
 */
static boolean
tn3270s_default (boolean  override,
		 ushort   val, 
		 ushort  *pVal,
		 boolean *pUseDefault)
{
    if (override) {
	/* Return FALSE if new value matches existing value */
	if (*pVal == val)
	    return FALSE;
	
	*pUseDefault = FALSE;
	*pVal = val;
	return TRUE;
    }
    
    /* Return FALSE if already using the Default */
    if (*pUseDefault)
	return FALSE;
    
    *pUseDefault = TRUE;
    *pVal = val;
    return TRUE;
}
/*
 * ---------------------------------------------------------------
 * tn3270s_shut_state_change
 *   determine if the desired state change
 *     represents a change
 * ---------------------------------------------------------------
 */
static boolean
tn3270s_shut_state_change (boolean go_shut,
			   ushort *pState)
{
    if (go_shut) {
	/* Check for a shut when it is already shut */
	if (*pState & TN3270S_STATE_SHUT) 
	    return FALSE;
	*pState |= TN3270S_STATE_SHUT;
	if (*pState & TN3270S_STATE_CIP_SHUT)
	    return FALSE;
	return TRUE;
    } else {
	if ((*pState & TN3270S_STATE_SHUT) ||
	    (*pState & TN3270S_STATE_CIP_SHUT)) {
	    *pState &= ~TN3270S_STATE_SHUT;
	    *pState &= ~TN3270S_STATE_CIP_SHUT;
	    return TRUE;
	}
	return FALSE;
    }
}
/*
 * ---------------------------------------------------------------
 * tn3270s_string_param
 *   general utility function which does the following:
 *     - verifies length of source against a specified max length
 *     - copies source string to destination
 *     - uppercases the dest string
 *   note: length verification and uppercase controlled by flag param:
 *     STRING_PARAM_UPPERCASE
 *     STRING_PARAM_VERIFY_LEN
 *   returns TRUE if length verification failed;
 * ---------------------------------------------------------------
 */
#define STRING_PARAM_UPPERCASE  0x0001
#define STRING_PARAM_VERIFY_LEN 0x0002
static boolean
tn3270s_string_param (uchar  *pDest,
		      uchar  *pSrc,
		      ushort  maxlen,
		      ushort  flags)
{
    if (flags & STRING_PARAM_VERIFY_LEN) {
	if (cip_len_exceeded(pSrc, maxlen))
	    return TRUE;
    }
    
    sstrncpy(pDest, pSrc, maxlen+1);
    
    if (flags & STRING_PARAM_UPPERCASE) {
	uppercase(pDest);
    }
    return FALSE;
}

/*
 * ==============================================================
 * Parser action routines: tn3270s-server command and subcommands
 * ==============================================================
 */
static void
tn3270s_svr_reconfig (parseinfo        *csb,
		      idbtype          *swidb,
		      cip_idb_infotype *cip,
		      tn3270s_t        *pTn3270s)
{
    if (csb->sense) {
	pTn3270s->svr.state |= TN3270S_STATE_CFGING;
	/* Go into subcommand mode */
	sprintf(pTn3270sSvrPrompt, "%s",
		TN3270S_SVR_PROMPT_BASE);
	set_mode_byname(&csb->mode, TN3270S_MODENAME_SVR, MODE_VERBOSE);
    } else {
	/* It's a delete of the Tn3270 server 
	 * Need to remove all sub resources 
	 * o All direct PUs
	 * o DLUR and DLUR sub resources
	 */
	tn3270s_delete_all_svr_cfg(swidb, cip, pTn3270s);

	tn3270s_cfgcmd_svr(swidb, cip, 
			   pTn3270s, TN3270S_CFGTYPE_DELETE);
	/* Actual destruction in cfg_ack routine */
    }
}

/*
 * -----------------------------------------------------------------------------
 * tn3270s_svr
 * ---------------------------------------------------------------
 *                        TN3270S_CFG_SVR
 * ---------------------------------------------------------------
 * [no] tn3270-server
 *
 */
static void
tn3270s_svr (parseinfo        *csb,
	     idbtype          *swidb,
	     cip_idb_infotype *cip,
	     tn3270s_t        *pTn3270s)
{
    /*
     * Do not allow tn3270s to be concurrently configured;
     * alert user to the fact
     */
    if (pTn3270s->svr.state & TN3270S_STATE_CFGING) {
	printf("\n%% The TN3270-server feature is currently being configured");
	return;
    }
    
    /* Take out of PNDING Delete state, if necessary */    
    if (pTn3270s->svr.state & TN3270S_STATE_DELETE) {
	tn3270s_svr_delete(pTn3270s);
    }
    
    if (pTn3270s->svr.state & TN3270S_STATE_INUSE) { 
	tn3270s_svr_reconfig(csb, swidb, cip, pTn3270s);
	return;
    }
    if (!csb->sense) {
	/* Check for a [no] when it doesn't even exist */
	printf("\n%% tn3270s-server is not configured.");
	return;
    }

    /* Here we are to create the tn3270s-server */
    if (!tn3270s_svr_create(pTn3270s)) {
	printf("\n%% Unable to allocate resources for tn3270-server");
	return;
    }
    
    pTn3270s->tn3270sUpTime = GetTimeNow();       /* save the time that tn3270 is up */
    /* Initialize siftdown parameters */
    tn3270s_svr_init(pTn3270s);
    
    /* Tell the CIP */
    tn3270s_cfgcmd_svr(swidb, cip, 
		       pTn3270s, TN3270S_CFGTYPE_CREATE);

    /* call reconfig */
    tn3270s_svr_reconfig(csb, swidb, cip, pTn3270s);
}

/*
 * ---------------------------------------------------------------
 * tn3270s_shut
 * ---------------------------------------------------------------
 *                        TN3270S_CFG_SHUT
 * tn3270-svr ----------------------------------------------------
 * tn3270-svr pu--------------------------------------------------
 * tn3270-svr dlur------------------------------------------------
 * tn3270-svr dlur pu --------------------------------------------
 * tn3270-svr dlur sap --------------------------------------------
 *      shutdown
 * [no] shutdown
 *
 */
static void
tn3270s_shut (parseinfo        *csb,
	      idbtype          *swidb,
	      cip_idb_infotype *cip,
	      tn3270s_t        *pTn3270s,
	      ushort            tn3270s_mode)
{
    boolean             changed;
    ushort             *pState;
    tn3270s_pu_t       *pPU;
    tn3270s_dlur_t     *pDLUR;
    tn3270s_dlur_sap_t *pSAP;

    pPU = NULL;
    pPU = csb->userDataBlock[UDB_TN3270S_CONTEXT];
    pSAP = csb->userDataBlock[UDB_TN3270S_CONTEXT];
    
    switch (tn3270s_mode) {
    case TN3270S_MODE_INTERFACE:
	return;
    case TN3270S_MODE_SVR     :
	pState = &pTn3270s->svr.state;
	break;
    case TN3270S_MODE_PU      :
	pPU = csb->userDataBlock[UDB_TN3270S_CONTEXT];
	pState = &pPU->state;
	break;
    case TN3270S_MODE_DLUR    :
	pDLUR =  &pTn3270s->dlur;
	pState = &pDLUR->state;
	break;
    case TN3270S_MODE_DLUR_PU :
	pPU = csb->userDataBlock[UDB_TN3270S_CONTEXT];
	pState = &pPU->state;
	break;
    case TN3270S_MODE_DLUR_SAP:
	pSAP = csb->userDataBlock[UDB_TN3270S_CONTEXT];
	pState = &pSAP->state;
	break;
    default:
	return;
    }
    
    changed = tn3270s_shut_state_change(csb->sense, pState);
    if (!changed)
	return;

    if (csb->sense) {
	/*
	 * shut the resource; also reset any sub resources
	 * in a manner similar to that which occurs at interface reset
	 *  o Process any PNDING states
	 *  o Delete any static routes
	 */
	switch (tn3270s_mode) {
	case TN3270S_MODE_INTERFACE:
	    break;
	case TN3270S_MODE_SVR     :
	    tn3270s_reset_all_svr_cfg(swidb, cip, pTn3270s);
	    tn3270s_cfgcmd_svr(swidb, cip, pTn3270s,
			       TN3270S_CFGTYPE_SHUT);
	    break;
	case TN3270S_MODE_DLUR_PU :
	case TN3270S_MODE_PU      :
	    /* If new PU, then this must be a shut; don't send */
	    if (pPU->new)
		return;
	    tn3270s_reset_this_pu_cfg(swidb, cip, pTn3270s, pPU);
	    tn3270s_cfgcmd_pu(swidb, cip, pTn3270s, pPU,
			      TN3270S_CFGTYPE_SHUT);
	    break;
	case TN3270S_MODE_DLUR    :
	    tn3270s_reset_all_dlur_cfg(swidb, cip, pTn3270s);
	    tn3270s_cfgcmd_dlur(swidb, cip, pTn3270s,
				TN3270S_CFGTYPE_SHUT);
	    break;
	case TN3270S_MODE_DLUR_SAP:
	    tn3270s_reset_this_sap_cfg(swidb, cip, pTn3270s, pSAP);
	    tn3270s_cfgcmd_dlur_sap(swidb, cip, pTn3270s, pSAP,
				    TN3270S_CFGTYPE_SHUT);
	    break;
	default:
	    return;
	}
	return;
    }
    /*
     * Otherwise, no shut; Send cfg for all sub commands also
     * do this by calling the send_all_cfg() ?
     */
    switch (tn3270s_mode) {
    case TN3270S_MODE_INTERFACE:
	break;
    case TN3270S_MODE_SVR     :
	tn3270s_send_all_svr_cfg(swidb, cip, pTn3270s);
	break;
    case TN3270S_MODE_DLUR_PU :
    case TN3270S_MODE_PU      :
	tn3270s_send_this_pu_cfg(swidb, cip, pTn3270s, pPU);
	break;
    case TN3270S_MODE_DLUR    :
	tn3270s_send_all_dlur_cfg(swidb, cip, pTn3270s);
	break;
    case TN3270S_MODE_DLUR_SAP:
	tn3270s_send_this_sap_cfg(swidb, cip, pTn3270s, pSAP);
	break;
    default:
	return;
    }
}
/*
 * ---------------------------------------------------------------
 * tn3270s_svr_max_lu
 * ---------------------------------------------------------------
 *                        TN3270S_CFG_SVR_MAXLU
 * tn3270-svr ----------------------------------------------------
 *      maximum-lus <number> 
 * [no] maximum-lus
 *
 * OBJ(int,1) = <max_lu>
 */
static void
tn3270s_svr_max_lu (parseinfo        *csb,
		    idbtype          *swidb,
		    cip_idb_infotype *cip,
		    tn3270s_t        *pTn3270s)
{
    ushort  val;
    ushort  *pVar;
    boolean *pDef;
    boolean changed;
    
    val  = (csb->sense) ? GETOBJ(int,1) : TN3270S_SVR_MAX_LU_DEF;

    /* To configure more than the default number of LUs requires a different license.
    *  If the value is being changed from below or at the default value to above it,
    *  then we print a licencing reminder.
    */
    if (   val > TN3270S_SVR_MAX_LU_DEF
        && pTn3270s->svr.max_lu.val <= TN3270S_SVR_MAX_LU_DEF)
    {
        printf ("\n\nIf you intend running more that %d TN3270 sessions concurrently on this\n
"
                "CIP then please check that your cisco license is for unlimited sessions\n",
                TN3270S_SVR_MAX_LU_DEF);
    }

    pVar = &pTn3270s->svr.max_lu.val;
    pDef = &pTn3270s->svr.max_lu.use_default;
    changed = tn3270s_default(csb->sense, val, pVar, pDef);
    
    if (changed) {
	/* Tell the CIP */
	tn3270s_cfgcmd_svr_max_lu(swidb, cip, 
				  pTn3270s, TN3270S_CFGTYPE_UPDATE);
    }
}
/*
 * ---------------------------------------------------------------
 * tn3270s_svr_timing_mark
 * ---------------------------------------------------------------
 *                        TN3270S_CFG_SVR_MAXLU
 * tn3270-svr ----------------------------------------------------
 *      maximum-lus <number> 
 * [no] maximum-lus
 *
 * OBJ(int,1) = <timing_mark>
 */
static void
tn3270s_svr_timing_mark (parseinfo        *csb,
		    idbtype          *swidb,
		    cip_idb_infotype *cip,
		    tn3270s_t        *pTn3270s)
{
    if (pTn3270s->svr.timing_mark.val != csb->sense) {
	/* Tell the CIP */
        pTn3270s->svr.timing_mark.val = csb->sense;
	tn3270s_cfgcmd_svr_timing_mark(swidb, cip, 
				  pTn3270s, TN3270S_CFGTYPE_UPDATE);
    }
}


/*
 * ---------------------------------------------------------------
 * tn3270s_svr_lu_dns
 * ---------------------------------------------------------------
 *                        TN3270S_CFG_SVR_LU_DNS
 * tn3270-svr ----------------------------------------------------
 *      lu-dns-domain-stem <dns-fqdn><rev-dns-fqdn> 
 * [no] lu-dns-domain-stem
 *
 * OBJ(string,1) = <dns-fqdn>
 * OBJ(string,2) = <rev-dns-fqdn>
 */
static void
tn3270s_svr_lu_dns (parseinfo        *csb,
		    idbtype          *swidb,
		    cip_idb_infotype *cip,
		    tn3270s_t        *pTn3270s)
{
    if (csb->sense) {
	if ((cip_len_exceeded(GETOBJ(string,1), MAX_FQDN_SIZE)) ||
	    (cip_len_exceeded(GETOBJ(string,2), MAX_FQDN_SIZE))) 
	    return;
	
	sstrncpy(pTn3270s->svr.lu_dns.dns_fqdn,
		 GETOBJ(string,1), MAX_FQDN_SIZE+1);
	
	sstrncpy(pTn3270s->svr.lu_dns.rev_dns_fqdn,
		 GETOBJ(string,2), MAX_FQDN_SIZE+1);
	
	uppercase(pTn3270s->svr.lu_dns.dns_fqdn);
	uppercase(pTn3270s->svr.lu_dns.rev_dns_fqdn);

	pTn3270s->svr.lu_dns.use_default = FALSE;
	
    } else {
	if (pTn3270s->svr.lu_dns.use_default)
	    return;

	pTn3270s->svr.lu_dns.use_default = TRUE;
	bzero(pTn3270s->svr.lu_dns.dns_fqdn,
	      sizeof(pTn3270s->svr.lu_dns.dns_fqdn));
	
	bzero(pTn3270s->svr.lu_dns.rev_dns_fqdn,
	      sizeof(pTn3270s->svr.lu_dns.rev_dns_fqdn));
    }
    /* Tell the CIP */
    tn3270s_cfgcmd_svr_lu_dns(swidb, cip, 
			      pTn3270s, TN3270S_CFGTYPE_UPDATE);
}

/*
 * ---------------------------------------------------------------
 * tn3270s_siftdown
 * ---------------------------------------------------------------
 *                        TN3270S_CFG_
 * tn3270-svr ----------------------------------------------------
 *      tcp-port <port_num> 
 * [no] tcp-port
 * OBJ(int,1) = <port_num>
 *
 *      idle-time <seconds> 
 * [no] idle-time
 * OBJ(int,1) = <seconds>
 *
 *      keepalive [<seconds>]
 * [no] keepalive
 * OBJ(int,1) = <seconds>
 *
 *      unbind-action { keep | disconnect } 
 * [no] unbind-action
 * OBJ(int,1) = <unbind_action> TN3270S_UNBIND_KEEP |
 *                              TN3270S_UNBIND_DISCON
 *      generic-pool { permit | deny } 
 * [no] generic-pool
 * OBJ(int,1) = <generic_pool> TN3270S_GPOOL_PERMIT |
 *                             TN3270S_GPOOL_DENY
 *
 */

static void
tn3270s_siftdown (parseinfo        *csb,
		  idbtype          *swidb,
		  cip_idb_infotype *cip,
		  tn3270s_t        *pTn3270s,
		  ushort            tn3270s_mode)
{
    ushort   val;
    ushort  *pVar;
    boolean *pDef;
    boolean  changed;
    tn3270s_pu_t *pPU;
    tn3270s_siftdown_t *pSift;
    tn3270s_cfg_t     cfgtype;


    if (tn3270s_mode == TN3270S_MODE_SVR) {
	pSift = &pTn3270s->svr.siftdown;
	switch (csb->which) {
	case TN3270S_CFG_TCP_PORT   :
	    val  = (csb->sense) ? GETOBJ(int,1) : TN3270S_TCP_PORT_DEF;
	    pVar = &pSift->tcp_port.val;
	    pDef = &pSift->tcp_port.use_default;
	    break;
	case TN3270S_CFG_IDLE_TIME  :
	    val  = (csb->sense) ? GETOBJ(int,1) : TN3270S_IDLE_TIME_DEF;
	    pVar = &pSift->idle_time.val;
	    pDef = &pSift->idle_time.use_default;
	    break;
	case TN3270S_CFG_KEEPALIVE  :
	    val  = (csb->sense) ? GETOBJ(int,1) : TN3270S_KEEPALIVE_DEF;
	    pVar = &pSift->keepalive.val;
	    pDef = &pSift->keepalive.use_default;
	    break;
	case TN3270S_CFG_UNBIND_ACT :
	    val  = (csb->sense) ? GETOBJ(int,1) : TN3270S_UNBIND_ACT_DEF;
	    pVar = &pSift->unbind_act.val;
	    pDef = &pSift->unbind_act.use_default;
	    break;
	case TN3270S_CFG_GPOOL      :
	    val  = (csb->sense) ? GETOBJ(int,1) : TN3270S_GPOOL_DEF;
	    pVar = &pSift->generic_pool.val;
	    pDef = &pSift->generic_pool.use_default;
	    break;
	default:
	    return;
	}
	changed = tn3270s_default(csb->sense, val, pVar, pDef);
	if (changed) {
	    /* Tell the CIP */
	    tn3270s_cfgcmd_svr_siftdown(swidb, cip, 
					pTn3270s, TN3270S_CFGTYPE_UPDATE);
	}
	return;
    }
    /* Otherwise, siftdown for direct or dlur PU */
    /* The behavior for these is a bit different than the typical handling
     * of the default case ( '[no]'):
     * [no] means to inherit the value of the SVR context siftdown parm
     *      send the command to the CIP if this PU was previously not
     *      inheritng the default.
     * override means to ABSOLUTELY use the value,
     *      it is written to nvram even if it happens to match the
     *      current default (which is the value of the SVR context 
     *      siftdown parm.
     */

    pPU = csb->userDataBlock[UDB_TN3270S_CONTEXT];
    pSift = &pPU->siftdown;

    val  = GETOBJ(int,1);
    switch (csb->which) {
    case TN3270S_CFG_TCP_PORT   :
	pVar = &pSift->tcp_port.val;
	pDef = &pSift->tcp_port.use_default;
	break;
    case TN3270S_CFG_IDLE_TIME  :
	pVar = &pSift->idle_time.val;
	pDef = &pSift->idle_time.use_default;
	break;
    case TN3270S_CFG_KEEPALIVE  :
	pVar = &pSift->keepalive.val;
	pDef = &pSift->keepalive.use_default;
	break;
    case TN3270S_CFG_UNBIND_ACT :
	pVar = &pSift->unbind_act.val;
	pDef = &pSift->unbind_act.use_default;
	break;
    case TN3270S_CFG_GPOOL      :
	pVar = &pSift->generic_pool.val;
	pDef = &pSift->generic_pool.use_default;
	break;
    default:
	return;
    }
    /* if override and currently inheriting default */
    if ((csb->sense) && (*pDef)) {
	*pDef = FALSE;    /* Don't inherit default */
	*pVar = val;      /* Use specified value   */
	changed = TRUE;   /* send change           */
    } else {
	changed = tn3270s_default(csb->sense, val, pVar, pDef);
    }
    
    if (changed) {
	/* Tell the CIP */
	cfgtype = pPU->new ? TN3270S_CFGTYPE_CREATE : TN3270S_CFGTYPE_UPDATE;
	tn3270s_cfgcmd_pu(swidb, cip, pTn3270s,
			  pPU, cfgtype);
    }
}

/*
 * ---------------------------------------------------------------
 * tn3270s_svr_pu_reconfig
 * ---------------------------------------------------------------
 *                        TN3270S_CFG_PU
 * tn3270-svr ----------------------------------------------------
 *      pu <pu-name>
 * [no] pu <pu-name>
 *
 * if already exists, only <pu-name> is required
 *
 * OBJ(string,1)= <pu-name>
 */

static void
tn3270s_svr_pu_reconfig (parseinfo        *csb,
			 idbtype          *swidb,
			 cip_idb_infotype *cip,
			 tn3270s_t        *pTn3270s,
			 tn3270s_pu_t     *pPU)
{

    if (csb->sense) {
	/* Go into subcommand mode */
	/* Need to initialize prompt base ? */
	sprintf(pTn3270sPuPrompt, "%s",
		TN3270S_PU_PROMPT_BASE);
	set_mode_byname(&csb->mode, TN3270S_MODENAME_PU, MODE_VERBOSE);
	csb->userDataBlock[UDB_TN3270S_CONTEXT] = pPU;

    } else {
	/* Delete it */
	/* Tell the CIP */
	tn3270s_cfgcmd_pu(swidb, cip, pTn3270s,
			  pPU, TN3270S_CFGTYPE_DELETE);
    }

}

/*
 *------------------------------------------------------------------
 * To do list
 * NOPREFIX : nuance, or desired:
 *      [no] allows ANYTHING after the NOPREFIX to be successfully
 *           parsed and tossed.
 *
 *  auburn(cfg-tn3270)#pu puname garbage garbage
 *                                     ^
 *  % Invalid input detected at '^' marker.
 *
 *  auburn(cfg-tn3270)#no pu puname garbage garbage
 *  % PU PUNAME is not configured
 *  auburn(cfg-tn3270)#
 *
 *  On a reconfiguration, where the user types extra parameters
 *    PU name [xid ipaddr......]
 *  It would be acceptable to go into subcommand mode, but
 *   with a message, rather than staying at the svr level or the
 *   dlur level.
 *   - Reason: if reconfiging from net, the resource may already
 *             exist; the siftdown parameters will then be applied
 *             to the svr level.
 *
 *------------------------------------------------------------------
 */

/*
 * ---------------------------------------------------------------
 * tn3270s_svr_pu
 * ---------------------------------------------------------------
 *                        TN3270S_CFG_PU
 * tn3270-svr ----------------------------------------------------
 *      pu <pu-name> <idblk-idnum> <ip-address> <type> <adapno> <lsap>
 *         [rmac <rmac>] [rsap <rsap>] [lu-seed <lu-name-stem>]
 *
 *      pu <pu-name>
 * [no] pu <pu-name>
 *
 * if already exists, only <pu-name> is required
 *
 * OBJ(string,1)= <pu-name>
 * OBJ(int,1)   = <remaining parameters specified> TRUE | FALSE
 * OBJ(string,2)= <idblk-idnum>
 * OBJ(paddr,1) = <ip-address>
 * OBJ(int,2)   = <type> CSNA_VLAN_ETHER |
 *                       CSNA_VLAN_TOKEN |
 *                       CSNA_VLAN_FDDI
 * OBJ(int,3)   = <adapno>
 * OBJ(int,4)   = <lsap>
 * OBJ(hwaddr,1)= <rmac>   DEFAULT 0000.0000.0000
 * OBJ(int,5)   = <rsap>   DEFAULT 4
 * OBJ(string,3)= <lu-seed>
 */

static void
tn3270s_svr_pu (parseinfo        *csb,
		idbtype          *swidb,
		cip_idb_infotype *cip,
		tn3270s_t        *pTn3270s)
{
    boolean       pu_reconfig;
    char          name[MAX_NAME_SIZE+1];
    ulong         xid;
    ushort        lan_type;
    ushort        adapno;
    uchar         lsap;
    uchar         rmac[IEEEBYTES];
    uchar         rsap;
    
    tn3270s_dlur_sap_t *pSAP;
    tn3270s_pu_t       *pPU;

    if (cip_len_exceeded(GETOBJ(string,1), MAX_NAME_SIZE))
	return;
    
    /* See if specified PU exists */
    sstrncpy(name, GETOBJ(string,1), MAX_NAME_SIZE+1);
    uppercase(name);

    pPU = tn3270s_pu_entry_locate_by_name(pTn3270s, name);
 
    /* Take out of PNDING Delete state, if necessary */
    if ((pPU) && (pPU->state & TN3270S_STATE_DELETE)) {
	tn3270s_delete_this_pu_cfg(swidb, cip, pTn3270s, pPU);
	pPU = NULL;
    }

    /* Verify this is a direct PU if it exists */
    if (pPU)
	if (pPU->type != TN3270S_PUTYPE_DIRECT) {
	    printf("\n%% PU %s is configured as a DLUR PU",
		   pPU->name);
	    return;
	}
    
    /* Is this a reconfigure or delete ? */
    pu_reconfig = !(GETOBJ(int,1));

    if (pu_reconfig) {
	if (!pPU) {
	    /* cannot delete or go into sub command mode if not configured */
	    printf("\n%% PU %s is not configured",
		   name);
	    return;
	}
	tn3270s_svr_pu_reconfig(csb, swidb, cip, pTn3270s, pPU);
	return;
    } else {
	/* They specified all the parameters */
	if (pPU) {
	    printf("\n%% PU %s exists; to reconfigure"
		   " or delete the PU, specify only its name.",
		   name);
	    return;
	}
	/* Check for a [no] when the PU doesn't exist and all parms specified */
	if (!csb->sense) {
	    /* could also moan about excess parameters, too */
	    printf("\n%% PU %s is not configured",
		   name);
	    return;
	}
    }

    /* Here we are to create a PU */
    /* First validate values */
    if (!cip_validate_ipaddr(swidb, csb->which, GETOBJ(paddr,1)->ip_addr))
	return;

    /* Format of the XID string */
    if (!valid_XID(GETOBJ(string,2), (uchar *) &xid, sizeof(ulong)))
	return;
    
    /* Length of the lu_seed, if specified */
    if (cip_len_exceeded(GETOBJ(string,3), MAX_NAME_SIZE))
	return;
    
    /*
     * Assure uniqueness of:
     *   lan_type, adapno, lsap, across all DLUR SAPs
     * and
     *   lan_type, adapno, lsap, rmac and rsap across all direct PUs, 
     */
    lan_type = GETOBJ(int,2);
    adapno = GETOBJ(int,3);
    lsap = GETOBJ(int,4);

    if (lsap & 0x01) {
      printf(odd_sap_reject_msg);
      return;
    }
    
    ieee_copy((char *)GETOBJ(hwaddr,1)->addr, rmac);
    if (is_ieee_zero(rmac))
	tn3270s_loopback_mac(rmac);
    
    rsap = GETOBJ(int,5);

    if (rsap & 0x01) {
      printf(odd_sap_reject_msg);
      return;
    }

    pSAP = tn3270s_sap_entry_locate(pTn3270s, lan_type,
				    adapno, lsap);
    if (pSAP && (pSAP->lsap == lsap)) {
	printf("\n%% %s %d %02X is already configured for use by DLUR",
	       tn3270s_lan_type_string(lan_type),
	       adapno,
	       lsap);
	return;
    }
    
    pPU = tn3270s_pu_entry_locate_by_xsap_xmac(pTn3270s, lan_type,
					       adapno, lsap, rmac, rsap);
    if (pPU) {
	printf("\n%% %s %d %02X (rmac %s rsap %02X)"
	       " is already configured for use by PU %s",
	       tn3270s_lan_type_string(lan_type),
	       adapno,
	       lsap,
	       tn3270s_rmac_string(rmac),
	       rsap,
	       pPU->name);
	return;
    }
    
    pPU = tn3270s_pu_entry_create(pTn3270s, name);
    /* check for NULL */
    if (!pPU) {
	printf("\n%% Unable to create entry for PU %s",
	       name);
	return;
    }
    tn3270s_pu_init(pTn3270s, pPU);
    
    pPU->xid = xid;
    pPU->ipaddr = GETOBJ(paddr,1)->ip_addr;
    
    pPU->vc = tn3270s_ipvc_assign_vc(pTn3270s, pPU->ipaddr);
    /* handle error */
    pPU->type = TN3270S_PUTYPE_DIRECT;
    
    pPU->lan_type = lan_type;
    pPU->adapno = adapno;
    pPU->lsap = lsap;

    ieee_copy(rmac, pPU->rmac);
    pPU->rsap = rsap;

    sstrncpy(pPU->lu_seed,
	     GETOBJ(string,3), MAX_NAME_SIZE+1);
    uppercase(pPU->lu_seed);
    
    /* call reconfig */
    tn3270s_svr_pu_reconfig(csb, swidb, cip, pTn3270s, pPU);
}

/*
 * ---------------------------------------------------------------
 * tn3270s_pu_dns
 * ---------------------------------------------------------------
 *                        TN3270S_CFG_PU_DNS
 * tn3270-svr ----------------------------------------------------
 * tn3270-svr pu--------------------------------------------------
 * tn3270-svr dlur pu --------------------------------------------
 *      pu-dns-name <pu-fqdn>
 * [no] pu-dns-name
 * OBJ(string,1) = <pu-fqdn>
 */
static void
tn3270s_pu_dns (parseinfo        *csb,
		idbtype          *swidb,
		cip_idb_infotype *cip,
		tn3270s_t        *pTn3270s,
		ushort            tn3270s_mode)
{
    tn3270s_cfg_t     cfgtype;
    tn3270s_pu_t     *pPU;
    
    pPU = csb->userDataBlock[UDB_TN3270S_CONTEXT];
    if (csb->sense) {
	if (cip_len_exceeded(GETOBJ(string,1), MAX_FQDN_SIZE))
	    return;
	
	sstrncpy(pPU->pu_dns.name,
		 GETOBJ(string,1), MAX_FQDN_SIZE+1);
	
	uppercase(pPU->pu_dns.name);
	pPU->pu_dns.use_default = FALSE;
	
    } else {
	if (pPU->pu_dns.use_default)
	    return;
	
	pPU->pu_dns.use_default = TRUE;
	bzero(pPU->pu_dns.name,
	      sizeof(pPU->pu_dns.name));
    }

    /* Tell the CIP */
    cfgtype = pPU->new ? TN3270S_CFGTYPE_CREATE : TN3270S_CFGTYPE_UPDATE;
    tn3270s_cfgcmd_pu(swidb, cip, pTn3270s, 
		      pPU, cfgtype);
}

/*
 * ---------------------------------------------------------------
 * tn3270s_dlur_reconfig
 * ---------------------------------------------------------------
 *                        TN3270S_CFG_DLUR
 * tn3270-svr ----------------------------------------------------
 *      dlur <fq-cpname> <fq-dlusname>
 * [no] dlur
 *
 * if already exists, <fq-cpname> and <fq-dlusname> are optional
 * otherwise required
 */

static void
tn3270s_dlur_reconfig (parseinfo        *csb,
		       idbtype          *swidb,
		       cip_idb_infotype *cip,
		       tn3270s_t        *pTn3270s)
{
    tn3270s_dlur_t *pDLUR;
    
    pDLUR = &pTn3270s->dlur;

    if (csb->sense) {
	/* Go into subcommand mode */
	sprintf(pTn3270sDlurPrompt, "%s",
		TN3270S_DLUR_PROMPT_BASE);
	set_mode_byname(&csb->mode, TN3270S_MODENAME_DLUR, MODE_VERBOSE);

    } else {
	/* It's a delete of the DLUR sub component
	 * Need to remove all sub resources 
	 * o All dlur PUs
	 * o All SAPs and SAP sub resources
	 */
	tn3270s_delete_all_dlur_cfg(swidb, cip, pTn3270s);

	/* Tell the CIP */
	tn3270s_cfgcmd_dlur(swidb, cip, 
			    pTn3270s, TN3270S_CFGTYPE_DELETE);
    }
}

/*
 * ---------------------------------------------------------------
 * tn3270s_dlur
 * ---------------------------------------------------------------
 *                        TN3270S_CFG_DLUR
 * tn3270-svr ----------------------------------------------------
 *      dlur <fq-cpname> <fq-dlusname>
 * [no] dlur
 *
 * if already exists, <fq-cpname> and <fq-dlusname> are optional
 * otherwise required
 *
 * OBJ(int,1)   = <remaining parameters specified> TRUE | FALSE
 *
 * OBJ(string,1)= <fq-cpname>
 * OBJ(string,2)= <fq-dlusname>
 *
 *           already validated for NETID.NAME format
 */
static void
tn3270s_dlur (parseinfo        *csb,
	      idbtype          *swidb,
	      cip_idb_infotype *cip,
	      tn3270s_t        *pTn3270s)
{
    tn3270s_dlur_t *pDLUR;
    boolean         dlur_reconfig;
    
    pDLUR = &pTn3270s->dlur;

    /* Take out of PNDING Delete state, if necessary */    
    if (pDLUR->state & TN3270S_STATE_DELETE) {
	tn3270s_dlur_delete(pTn3270s);
    }

    /* Is this a reconfigure or delete ? */
    dlur_reconfig = !(GETOBJ(int,1));

    if (dlur_reconfig) {
	if (!(pDLUR->state & TN3270S_STATE_INUSE)) {
	    /* cannot delete or go into sub command mode if not configured */
	    printf("\n%% DLUR is not configured");
	    if (csb->sense)
		printf(" must specify %s and %s",
		       "<fq-cpname>", "<fq-dlusname>");
	    return;
	}
	tn3270s_dlur_reconfig(csb, swidb, cip, pTn3270s);
	return;
    } else {
	/* They specified all the parameters */
	if (pDLUR->state & TN3270S_STATE_INUSE) {
	    printf("\n%% DLUR (%s %s) is already configured; to reconfigure"
		   " or delete, do not respecify names.",
		   pDLUR->dlur_fq_cpname,
		   pDLUR->dlur_fq_dlusname);
	    return;
	}
    }

    /* Here we are to create the DLUR component */
    if (!tn3270s_dlur_create(pTn3270s)) {
	printf("\n%% Unable to allocate resources for tn3270-server dlur");
	return;
    }
    
    /* Initialize */
    tn3270s_dlur_init(pTn3270s);
    
    sstrncpy(pDLUR->dlur_fq_cpname,
	     GETOBJ(string,1), MAX_FQNN_SIZE+1);
    
    sstrncpy(pDLUR->dlur_fq_dlusname,
	     GETOBJ(string,2), MAX_FQNN_SIZE+1);
    
    /* Tell the CIP */
    tn3270s_cfgcmd_dlur(swidb, cip, 
			pTn3270s, TN3270S_CFGTYPE_CREATE);
    /* call reconfig */
    tn3270s_dlur_reconfig(csb, swidb, cip, pTn3270s);
}

/*
 * ---------------------------------------------------------------
 * tn3270s_dlur_dlus
 * ---------------------------------------------------------------
 *                        TN3270S_CFG_DLUR_DLUS
 * tn3270-svr ----------------------------------------------------
 * tn3270-svr dlur------------------------------------------------
 *      dlus-backup <dlusname2>
 * [no] dlus-backup
 *
 * OBJ(string,1) = <dlusname2>
 *
 *           already validated for NETID.NAME format
 */

static void
tn3270s_dlur_dlus (parseinfo        *csb,
		   idbtype          *swidb,
		   cip_idb_infotype *cip,
		   tn3270s_t        *pTn3270s)
{
    tn3270s_dlur_t *pDLUR;
    
    pDLUR = &pTn3270s->dlur;

    if (csb->sense) {
	sstrncpy(pDLUR->dlur_dlus_backup.name,
		 GETOBJ(string,1), MAX_FQNN_SIZE+1);
	
	pDLUR->dlur_dlus_backup.use_default = FALSE;
	
    } else {
	if (pDLUR->dlur_dlus_backup.use_default)
	    return;
	
	pDLUR->dlur_dlus_backup.use_default = TRUE;
	
	bzero(pDLUR->dlur_dlus_backup.name,
	      sizeof(pDLUR->dlur_dlus_backup.name));
    }
    /* Tell the CIP */
    tn3270s_cfgcmd_dlur_dlus(swidb, cip, 
			     pTn3270s, TN3270S_CFGTYPE_UPDATE);
}

/*
 * ---------------------------------------------------------------
 * tn3270s_dlur_nn_svr
 * ---------------------------------------------------------------
 *                        TN3270S_CFG_DLUR_NN_SVR
 * tn3270-svr ----------------------------------------------------
 * tn3270-svr dlur------------------------------------------------
 *
 *      preferred-NNserver <NNserver>
 * [no] preferred-NNserver
 *
 * OBJ(string,1) = <NNserver>
 *
 *           already validated for NETID.NAME format
 */

static void
tn3270s_dlur_nn_svr (parseinfo        *csb,
		     idbtype          *swidb,
		     cip_idb_infotype *cip,
		     tn3270s_t        *pTn3270s)
{
    tn3270s_dlur_t *pDLUR;
    
    pDLUR = &pTn3270s->dlur;

    if (csb->sense) {
	sstrncpy(pDLUR->dlur_nn_svr.name,
		 GETOBJ(string,1), MAX_FQNN_SIZE+1);
	
	pDLUR->dlur_nn_svr.use_default = FALSE;
	
    } else {
	if (pDLUR->dlur_nn_svr.use_default)
	    return;
	
	pDLUR->dlur_nn_svr.use_default = TRUE;
	
	bzero(pDLUR->dlur_nn_svr.name,
	      sizeof(pDLUR->dlur_nn_svr.name));
    }
    /* Tell the CIP */
    tn3270s_cfgcmd_dlur_nn_svr(swidb, cip, 
			       pTn3270s, TN3270S_CFGTYPE_UPDATE);
}

/*
 * ---------------------------------------------------------------
 * tn3270s_dlur_pu_reconfig
 * ---------------------------------------------------------------
 *                        TN3270S_CFG_DLUR_PU
 * tn3270-svr ----------------------------------------------------
 * tn3270-svr-dlur -----------------------------------------------
 *      pu <pu-name>
 * [no] pu <pu-name>
 *
 * if already exists, only <pu-name> is required
 *
 * OBJ(string,1)= <pu-name>
 */

static void
tn3270s_dlur_pu_reconfig (parseinfo        *csb,
			  idbtype          *swidb,
			  cip_idb_infotype *cip,
			  tn3270s_t        *pTn3270s,
			  tn3270s_pu_t     *pPU)
{
    
    if (csb->sense) {
	/* Go into subcommand mode */
	/* Need to initialize prompt base ? */
	sprintf(pTn3270sDlurPuPrompt, "%s",
		TN3270S_DLUR_PU_PROMPT_BASE);
	set_mode_byname(&csb->mode, TN3270S_MODENAME_DLUR_PU, MODE_VERBOSE);
	csb->userDataBlock[UDB_TN3270S_CONTEXT] = pPU;

    } else {
	/* Delete it */
	/* Tell the CIP */
	tn3270s_cfgcmd_pu(swidb, cip, pTn3270s, 
			  pPU, TN3270S_CFGTYPE_DELETE);
    }
}

/*
 * ---------------------------------------------------------------
 * tn3270s_dlur_pu
 * ---------------------------------------------------------------
 *                        TN3270S_CFG_DLUR_PU
 * tn3270-svr ----------------------------------------------------
 *      pu <pu-name> <idblk-idnum> <ip-address>
 *
 *      pu <pu-name>
 * [no] pu <pu-name>
 *
 * if already exists, only <pu-name> is required
 *
 * OBJ(string,1)= <pu-name>
 * OBJ(int,1)   = <remaining parameters specified> TRUE | FALSE
 * OBJ(string,2)= <idblk-idnum>
 * OBJ(paddr,1) = <ip-address>
 */

static void
tn3270s_dlur_pu (parseinfo        *csb,
		 idbtype          *swidb,
		 cip_idb_infotype *cip,
		 tn3270s_t        *pTn3270s)
{
    boolean       pu_reconfig;
    char          name[MAX_NAME_SIZE+1];
    ulong         xid;
    tn3270s_pu_t  *pPU, *searchPU;

    if (cip_len_exceeded(GETOBJ(string,1), MAX_NAME_SIZE))
	return;
    
    /* See if specified PU exists */
    sstrncpy(name, GETOBJ(string,1), MAX_NAME_SIZE+1);
    uppercase(name);

    pPU = tn3270s_pu_entry_locate_by_name(pTn3270s, name);

    /* Take out of PNDING Delete state, if necessary */
    if ((pPU) && (pPU->state & TN3270S_STATE_DELETE)) {
	tn3270s_delete_this_pu_cfg(swidb, cip, pTn3270s, pPU);
	pPU = NULL;
    }

    /* Verify this is a dlur PU if it exists */
    if (pPU)
	if (pPU->type != TN3270S_PUTYPE_DLUR) {
	    printf("\n%% PU %s is configured as a direct PU",
		   pPU->name);
	    return;
	}

    /* Is this a reconfigure or delete ? */
    pu_reconfig = !(GETOBJ(int,1));

    if (pu_reconfig) {
	if (!pPU) {
	    /* cannot delete or go into sub command mode if not configured */
	    printf("\n%% PU %s is not configured",
		   name);
	    return;
	}
	tn3270s_dlur_pu_reconfig(csb, swidb, cip, pTn3270s, pPU);
	return;
    } else {
	/* They specified all the parameters */
	if (pPU) {
	    printf("\n%% PU %s exists; to reconfigure"
		   " or delete the PU, specify only its name.",
		   name);
	    return;
	}
	/* Check for a [no] when the PU doesn't exist and all parms specified */
	if (!csb->sense) {
	    /* could also moan about excess parameters, too */
	    printf("\n%% PU %s is not configured",
		   name);
	    return;
	}
    }
    
    /* Here we are to create a PU */
    /* First validate values */
    if (!cip_validate_ipaddr(swidb, csb->which, GETOBJ(paddr,1)->ip_addr))
	return;
    
    if (!valid_XID(GETOBJ(string,2), (uchar *) &xid, sizeof(ulong)))
	return;

    /* Check that there isn't another DLUR PU on this CIP with the same the 
     * idblk (xid):
     */ 
    for (searchPU = tn3270s_pu_entry_get_first (pTn3270s);
         searchPU != NULL;
         searchPU = tn3270s_pu_entry_get_next (pTn3270s, searchPU)
        )
    {
        if (searchPU->xid == xid && searchPU->type == TN3270S_PUTYPE_DLUR
                        && searchPU != pPU)
        {
            printf("\n%% invalid xid value, (used by another dlur PU)");
            return;
        }
    }


    pPU = tn3270s_pu_entry_create(pTn3270s, name);
    /* check for NULL */
    if (!pPU) {
	printf("\n%% Unable to create entry for PU %s",
	       name);
	return;
    }
    tn3270s_pu_init(pTn3270s, pPU);
    
    pPU->xid = xid;
    pPU->ipaddr = GETOBJ(paddr,1)->ip_addr;
    
    pPU->vc = tn3270s_ipvc_assign_vc(pTn3270s, pPU->ipaddr);
    /* handle error */
    pPU->type = TN3270S_PUTYPE_DLUR;

    /* call reconfig */
    tn3270s_dlur_pu_reconfig(csb, swidb, cip, pTn3270s, pPU);
}

static void
tn3270s_dlur_sap_reconfig (parseinfo          *csb,
			   idbtype            *swidb,
			   cip_idb_infotype   *cip,
			   tn3270s_t          *pTn3270s,
			   tn3270s_dlur_sap_t *pSAP)
{
    if (csb->sense) {
	/* Go into subcommand mode */
	/* Need to initialize prompt base ? */
	sprintf(pTn3270sDlurSapPrompt, "%s",
		TN3270S_DLUR_SAP_PROMPT_BASE);
	set_mode_byname(&csb->mode, TN3270S_MODENAME_DLUR_SAP, MODE_VERBOSE);
	csb->userDataBlock[UDB_TN3270S_CONTEXT] = pSAP;

    } else {
	/* It's a delete of a SAP
	 * Need to remove all sub resources 
	 * o All links
	 */
	tn3270s_delete_all_link_cfg(swidb, cip, pTn3270s, pSAP);
	
	/* Tell the CIP */
	tn3270s_cfgcmd_dlur_sap(swidb, cip, pTn3270s,
				pSAP, TN3270S_CFGTYPE_DELETE);
    }
}

/*
 * ---------------------------------------------------------------
 * tn3270s_dlur_sap
 * ---------------------------------------------------------------
 *                        TN3270S_CFG_DLUR_SAP
 * tn3270-svr ----------------------------------------------------
 * tn3270-svr-dlur -----------------------------------------------
 *
 *      sap <type> <adapno> [<lsap>]
 * [no] sap <type> <adapno> [<lsap>]
 *
 * OBJ(int,1)   = <lsap specified> TRUE | FALSE
 * OBJ(int,2)   = <type> CSNA_VLAN_ETHER |
 *                       CSNA_VLAN_TOKEN |
 *                       CSNA_VLAN_FDDI
 * OBJ(int,3)   = <adapno>
 * OBJ(int,4)   = <lsap>   DEFAULT xC0
 *
 * -----------------------------------------------------------------
 *
 *                  lsap specified?
 * SAP exists  |     No    |  Yes       |
 * ------------|-----------|------------|
 *             |  CREATE   |  CREATE    |
 *      No     |  use lsap | use lsap   |
 *             |   default | specified  |
 * ------------|-----------|------------|
 *     Yes     |  reconfig | lsap matches?
 *             |     or    |    Yes: reconfig or complain
 *             |   delete  |     No: error (sap exists already)
 * -----------------------------------------------------------------
 */
static void
tn3270s_dlur_sap (parseinfo        *csb,
		  idbtype          *swidb,
		  cip_idb_infotype *cip,
		  tn3270s_t        *pTn3270s)
{
    tn3270s_pu_t       *pPU;
    tn3270s_dlur_sap_t *pSAP;
    ushort              lan_type;
    ushort              adapno;
    uchar               lsap;
    boolean             lsap_specified;

    /* See if specified SAP exists */
    lan_type = GETOBJ(int,2);
    adapno = GETOBJ(int,3);
    lsap = GETOBJ(int,4);

    if (lsap & 0x01) {
      printf(odd_sap_reject_msg);
      return;
    }

    pSAP = tn3270s_sap_entry_locate(pTn3270s, lan_type,
				    adapno, lsap);

    /* Take out of PNDING Delete state, if necessary */
    if ((pSAP) && (pSAP->state & TN3270S_STATE_DELETE)) {
	tn3270s_delete_this_sap_cfg(swidb, cip, pTn3270s, pSAP);
	pSAP = NULL;
    }
    /* Is this a reconfigure or delete ? Or a create with defaulted lsap ? */
    lsap_specified = GETOBJ(int,1);
    
    if (pSAP) {
	if ((!lsap_specified) || (pSAP->lsap == lsap)) {
	    /*	    
	     * lan_type and adapno match an existing; no lsap specified, or
	     * an lsap was specified and it matches
	     */
	    tn3270s_dlur_sap_reconfig(csb, swidb, cip, pTn3270s, pSAP);
	    return;
	} else {
	    /*
	     * The specified lsap doesn't match the existing lsap
	     */
	    if (csb->sense) {
		printf("\n%% This DLUR %s is configured to use lsap %02X;"
		       " delete it first to assign a different lsap value.",
		       TN3270S_KEY_DLUR_SAP,
		       pSAP->lsap);
		return;
	    }
	}
    }
    
    if (!csb->sense) {
	/* Check for a [no] when it doesn't even exist */
	printf("\n%% %s doesn't exist",
	       TN3270S_KEY_DLUR_SAP);
	return;
    }
    
    /* Here we are to create a SAP */
    /*
     * Assure uniqueness of:
     *   lan_type, adapno, lsap, across all Direct PUs
     */
    
    pPU = tn3270s_pu_entry_locate_by_xsap(pTn3270s, lan_type,
					  adapno, lsap);
    if (pPU) {
	printf("\n%% %s %d %02X is already configured for use by PU %s",
	       tn3270s_lan_type_string(lan_type),
	       adapno,
	       lsap,
	       pPU->name);
	return;
    }

    pSAP = tn3270s_sap_entry_create(pTn3270s, lan_type,
				    adapno, lsap);
    if (!pSAP) {
	printf("\n%% Unable to create SAP");
	return;
    }
    
    /* Initialize */
    tn3270s_dlur_sap_init(pTn3270s, pSAP);
    
    /* Tell the CIP */
    tn3270s_cfgcmd_dlur_sap(swidb, cip, pTn3270s,
			    pSAP, TN3270S_CFGTYPE_CREATE);
    /* call reconfig */
    tn3270s_dlur_sap_reconfig(csb, swidb, cip, pTn3270s, pSAP);
}

/*
 * ---------------------------------------------------------------
 * tn3270s_dlur_sap_vrn
 * ---------------------------------------------------------------
 *                        TN3270S_CFG_DLUR_SAP_VRN
 * tn3270-svr ----------------------------------------------------
 * tn3270-svr dlur------------------------------------------------
 * tn3270-svr dlur sap --------------------------------------------
 *      vrn <VRN-name>
 * [no] vrn
 *
 * OBJ(string,1) = <VRN-name>
 *
 *           already validated for NETID.NAME format
 */
static void
tn3270s_dlur_sap_vrn (parseinfo        *csb,
		      idbtype          *swidb,
		      cip_idb_infotype *cip,
		      tn3270s_t        *pTn3270s)
{
    tn3270s_dlur_sap_t *pSAP;
    
    pSAP = csb->userDataBlock[UDB_TN3270S_CONTEXT];

    if (csb->sense) {
	sstrncpy(pSAP->dlur_sap_vrn.name,
		 GETOBJ(string,1), MAX_FQNN_SIZE+1);
	
	pSAP->dlur_sap_vrn.use_default = FALSE;
	
    } else {
	if (pSAP->dlur_sap_vrn.use_default)
	    return;
	
	pSAP->dlur_sap_vrn.use_default = TRUE;
	
	bzero(pSAP->dlur_sap_vrn.name,
	      sizeof(pSAP->dlur_sap_vrn.name));
    }
    /* Tell the CIP */
    tn3270s_cfgcmd_dlur_sap_vrn(swidb, cip, pTn3270s,
				pSAP, TN3270S_CFGTYPE_UPDATE);
}

/*
 * ---------------------------------------------------------------
 * tn3270s_dlur_sap_link
 * ---------------------------------------------------------------
 *                        TN3270S_CFG_DLUR_SAP_LINK
 * tn3270-svr ----------------------------------------------------
 * tn3270-svr dlur------------------------------------------------
 * tn3270-svr dlur sap --------------------------------------------
 *      link <name> [rmac <rmac>] [rsap <rsap>]
 * [no] link <name>
 *
 * if already exists, only <name> is required
 *
 * OBJ(string,1)= <name>
 * OBJ(int,1)   = <remaining parameters specified> TRUE | FALSE
 * OBJ(hwaddr,1)= <rmac>   DEFAULT 0000.0000.0000
 * OBJ(int,2)   = <rsap>   DEFAULT 4
 *
 * ------------------------------------------------------------
 * Note about the links avl tree:
 *  There is a single tree, anchored in the DLUR data 
 *  structure, which all saps use.  
 *   - Names are assured consistent across the DLUR
 *     context, not just a sap context.  If there were
 *     not a single tree, then all saps would be cycled
 *     to assure the name didn't exist for a link defined
 *     on a different sap.
 *   - The number of links that will be defined per sap 
 *     doesn't justify a separate tree for each sap.  
 *     Searching through a single tree each time a
 *     link needs to be located or created will not
 *     be a performance concern.
 * 
 * ------------------------------------------------------------
 */
static void
tn3270s_dlur_sap_link (parseinfo        *csb,
		       idbtype          *swidb,
		       cip_idb_infotype *cip,
		       tn3270s_t        *pTn3270s)
{
    tn3270s_dlur_sap_t  *pSAP;
    tn3270s_link_t      *pLink;
    boolean              name_only;
    uchar                name[MAX_NAME_SIZE+1];
    uchar                rmac[IEEEBYTES];
    uchar                rsap;
    tn3270s_cfg_t        cfgtype;
    
    pSAP = csb->userDataBlock[UDB_TN3270S_CONTEXT];
    
    if (cip_len_exceeded(GETOBJ(string,1), MAX_NAME_SIZE))
	return;
    
    /* See if specified link exists */
    sstrncpy(name, GETOBJ(string,1), MAX_NAME_SIZE+1);
    uppercase(name);
    
    ieee_copy((char *)GETOBJ(hwaddr,1)->addr, rmac);
    if (is_ieee_zero(rmac))
	tn3270s_loopback_mac(rmac);
    
    rsap = GETOBJ(int,2);

    if (rsap & 0x01) {
      printf(odd_sap_reject_msg);
      return;
    }

    pLink = tn3270s_link_entry_locate_by_name(pTn3270s, name);
 
    /* Take out of PNDING Delete state, if necessary */
    if ((pLink) && (pLink->state & TN3270S_STATE_DELETE)) {
	tn3270s_link_entry_delete(pTn3270s, pLink);
	pLink = NULL;
    }

    /* Assure that if this name is defined, it exists on this SAP */
    if ((pLink) && (!((pLink->lan_type == pSAP->lan_type) &&
		      (pLink->adapno == pSAP->adapno)))) {
	printf("\n%% Link %s is defined on %s %s %d",
	       pLink->name,
	       TN3270S_KEY_DLUR_SAP,
	       tn3270s_lan_type_string(pLink->lan_type),
	       pLink->adapno);
	return;
    }
    
    name_only = !(GETOBJ(int,1));

    if (csb->sense) {
	if (pLink) {
	    printf("\n%% link %s already exists.",
		   name);
	    return;
	}
	/* Verify no other link on this SAP is using this rmac, rsap pair */
	pLink = tn3270s_link_entry_locate_by_rmac(pTn3270s, pSAP,
						  rmac, rsap);
	/* Take out of PNDING Delete state, if necessary */
	if ((pLink) && (pLink->state & TN3270S_STATE_DELETE)) {
	    tn3270s_link_entry_delete(pTn3270s, pLink);
	    pLink = NULL;
	}
	
	if (pLink) {
	    printf("\n%% link %s is already configured to use "
		   "rmac, rsap (%s, %02X)",
		   pLink->name, tn3270s_rmac_string(pLink->rmac), pLink->rsap);
	    return;
	}
	
	pLink = tn3270s_link_entry_create(pTn3270s, pSAP,
					  name, rmac, rsap);
	if (!pLink) {
	    printf("\n%% Cannot create link %s.",
		   name);
	    return;
	}
	tn3270s_link_init(pTn3270s, pLink);
	cfgtype = TN3270S_CFGTYPE_CREATE;
    } else {
	if (!pLink) {
	    printf("\n%% link %s does not exist.",
		   name);
	    return;
	}
	if (!name_only) {
	    printf("\n%% To remove a link specify only its name.");
	    return;
	}
	cfgtype = TN3270S_CFGTYPE_DELETE;
    }
    /* Tell the CIP */
    tn3270s_cfgcmd_dlur_sap_link(swidb, cip, pTn3270s,
				 pSAP, pLink, cfgtype);
}

/*
 * ==============================================================
 * Parser action routine  Generic nvgen
 * ==============================================================
 */

static void
tn3270s_nvgen (parseinfo *csb,
	       ushort     tn3270s_mode)
{
/* indentation for nvgen subcommand mode indication */
 
    idbtype            *swidb;
    cip_idb_infotype   *cip;
    tn3270s_t          *pTn3270s;
    tn3270s_pu_t       *pPU;
    tn3270s_dlur_t     *pDLUR;
    tn3270s_dlur_sap_t *pSAP;
    tn3270s_link_t     *pLink;
    tn3270s_siftdown_t *pSift;
    ushort              state;
    char               *pIndentation;

    swidb = csb->interface;
    cip = channel_sb_get_cip_info(swidb->hwptr);
    if (!cip)
        return;

    pTn3270s = cip->pTn3270sData;

    /* Get mode context pointer */
    pPU = GETOBJ(udb,1);
    pDLUR = GETOBJ(udb,1);
    pSAP = GETOBJ(udb,1);
    pSift = NULL;
    state = 0;

    switch (tn3270s_mode) {
    case TN3270S_MODE_INTERFACE:
	pIndentation = "";
	break;
    case TN3270S_MODE_SVR     :
	pSift = &pTn3270s->svr.siftdown;
	state = pTn3270s->svr.state;
	pIndentation = nv_indent[UDB_SVR];
	break;
    case TN3270S_MODE_PU      :
	pSift = &pPU->siftdown;
	state = pPU->state;
	pIndentation = nv_indent[UDB_PU];
	break;
    case TN3270S_MODE_DLUR    :
	state = pDLUR->state;
	pIndentation = nv_indent[UDB_DLUR];
	break;
    case TN3270S_MODE_DLUR_PU :
	pSift = &pPU->siftdown;
	state = pPU->state;
	pIndentation = nv_indent[UDB_DLUR_PU];
	break;
    case TN3270S_MODE_DLUR_SAP:
	state = pSAP->state;
	pIndentation = nv_indent[UDB_DLUR_SAP];
	break;
    default:
	return;
    }

    switch (csb->which) {
    case TN3270S_CFG_SVR   :
	if (!(pTn3270s->svr.state & TN3270S_STATE_INUSE))
	    return;
	if (pTn3270s->svr.state & TN3270S_STATE_DELETE)
	    return;
	nv_write(TRUE, "%s", csb->nv_command);
	
	if (set_mode_byname(&csb->mode, TN3270S_MODENAME_SVR,
			    MODE_SILENT)) {
	    /* clear out before doing subcommands */
	    *csb->nv_command = '\0';
	    push_node(csb, get_mode_nv(csb));
	}
	return;
    case TN3270S_CFG_SHUT       :
	nv_write((state & TN3270S_STATE_SHUT),
		 "%s%s", pIndentation, csb->nv_command);
	return;
    case TN3270S_CFG_SVR_MAX_LU :
	nv_write(!(pTn3270s->svr.max_lu.use_default),
		 "%s%s %d", pIndentation, csb->nv_command,
		 pTn3270s->svr.max_lu.val);
	return;
    case TN3270S_CFG_SVR_TIMING_MARK :
	nv_write(pTn3270s->svr.timing_mark.val,
		 "%s%s", pIndentation, csb->nv_command);
	return;
    case TN3270S_CFG_SVR_LU_DNS :
	nv_write(!(pTn3270s->svr.lu_dns.use_default),
		 "%s%s %s %s", pIndentation, csb->nv_command,
		 pTn3270s->svr.lu_dns.dns_fqdn,
		 pTn3270s->svr.lu_dns.rev_dns_fqdn);
	return;
    case TN3270S_CFG_TCP_PORT   : 
	nv_write(!(pSift->tcp_port.use_default),
		 "%s%s %d ", pIndentation, csb->nv_command,
		 pSift->tcp_port.val);
	return;
    case TN3270S_CFG_IDLE_TIME  :
	nv_write(!(pSift->idle_time.use_default),
		 "%s%s %d ", pIndentation, csb->nv_command,
		 pSift->idle_time.val);
	return;
    case TN3270S_CFG_KEEPALIVE  :
        nv_write( !(pSift->keepalive.use_default),
                  "%s%s %d ", pIndentation, csb->nv_command,
                  pSift->keepalive.val);
        return;
    case TN3270S_CFG_UNBIND_ACT :
	nv_write(!(pSift->unbind_act.use_default),
		 "%s%s %s ", pIndentation, csb->nv_command,
		 (pSift->unbind_act.val ==
		  TN3270S_UNBIND_ACT_KEEP) ? TN3270S_KEY_UNBIND_KEEP :
		 TN3270S_KEY_UNBIND_DISC);
	return;
    case TN3270S_CFG_GPOOL      :
	nv_write(!(pSift->generic_pool.use_default),
		 "%s%s %s ", pIndentation, csb->nv_command,
		 (pSift->generic_pool.val ==
		  TN3270S_GPOOL_PERMIT) ? TN3270S_KEY_GPOOL_PERMIT :
		 TN3270S_KEY_GPOOL_DENY);
	return;
	/* On these, have to go into subcommand mode also */
    case TN3270S_CFG_PU         :
	nv_write(TRUE,
		 "%s%s %8s %08X %15i %s %2d %02X",pIndentation, csb->nv_command,
		 pPU->name,
		 pPU->xid,
		 pPU->ipaddr,
		 tn3270s_lan_type_string(pPU->lan_type),
		 pPU->adapno,
		 pPU->lsap);
	nv_add(!tn3270s_is_loopback_mac(pPU->rmac),
	       " %s %14e", TN3270S_KEY_RMAC, pPU->rmac);
	nv_add((pPU->rsap != TN3270S_RSAP_DEF),
	       " %s %02X", TN3270S_KEY_RSAP, pPU->rsap);
	nv_add((pPU->lu_seed[0]),
	       " %s %s", TN3270S_KEY_PU_LUSEED, pPU->lu_seed);

	if (set_mode_byname(&csb->mode, TN3270S_MODENAME_PU,
			    MODE_SILENT)) {
	    csb->userDataBlock[UDB_TN3270S_CONTEXT] = pPU;
	    
	    /* clear out before doing subcommands */
	    *csb->nv_command = '\0';
	    push_node(csb, get_mode_nv(csb));
	}
	return;
    case TN3270S_CFG_PU_DNS     :
	nv_write(!(pPU->pu_dns.use_default),
		 "%s%s %s ", pIndentation, csb->nv_command,
		 pPU->pu_dns.name);
	return;
    case TN3270S_CFG_DLUR       :
	/* should this check go here or in the sub command nvgen setup */
	if (!(pDLUR->state & TN3270S_STATE_INUSE))
	    return;
	if (pDLUR->state & TN3270S_STATE_DELETE)
	    return;
	nv_write(TRUE,
		 "%s%s %s %s", pIndentation, csb->nv_command,
		 pDLUR->dlur_fq_cpname,
		 pDLUR->dlur_fq_dlusname);
	
	if (set_mode_byname(&csb->mode, TN3270S_MODENAME_DLUR,
			    MODE_SILENT)) {
	    /* clear out before doing subcommands */
	    *csb->nv_command = '\0';
	    push_node(csb, get_mode_nv(csb));
	}
	return;
    case TN3270S_CFG_DLUR_DLUS  :
	nv_write(!(pDLUR->dlur_dlus_backup.use_default),
		 "%s%s %s ", pIndentation, csb->nv_command,
		 pDLUR->dlur_dlus_backup.name);
	return;
    case TN3270S_CFG_DLUR_NN_SVR:
	nv_write(!(pDLUR->dlur_nn_svr.use_default),
		 "%s%s %s ", pIndentation, csb->nv_command,
		 pDLUR->dlur_nn_svr.name);
	return;
    case TN3270S_CFG_DLUR_PU    :
	nv_write(TRUE,
		 "%s%s %8s %08X %15i", pIndentation, csb->nv_command,
		 pPU->name,
		 pPU->xid,
		 pPU->ipaddr);
	
	if (set_mode_byname(&csb->mode, TN3270S_MODENAME_DLUR_PU,
			    MODE_SILENT)) {
	    csb->userDataBlock[UDB_TN3270S_CONTEXT] = pPU;
	    
	    /* clear out before doing subcommands */
	    *csb->nv_command = '\0';
	    push_node(csb, get_mode_nv(csb));
	}
	return;
    case TN3270S_CFG_DLUR_SAP   :
	nv_write(TRUE,
		 "%s%s %s %2d", pIndentation, csb->nv_command,
		 tn3270s_lan_type_string(pSAP->lan_type),
		 pSAP->adapno);
	nv_add((pSAP->lsap != TN3270S_LSAP_DEF),
	       " %02X", pSAP->lsap);
	
	if (set_mode_byname(&csb->mode, TN3270S_MODENAME_DLUR_SAP,
			    MODE_SILENT)) {
	    csb->userDataBlock[UDB_TN3270S_CONTEXT] = pSAP;
	    
	    /* clear out before doing subcommands */
	    *csb->nv_command = '\0';
	    push_node(csb, get_mode_nv(csb));
	}
	return;
    case TN3270S_CFG_DLUR_SAP_VRN:
	nv_write(!(pSAP->dlur_sap_vrn.use_default),
		 "%s%s %s ", pIndentation, csb->nv_command,
		 pSAP->dlur_sap_vrn.name);
	return;
    case TN3270S_CFG_DLUR_SAP_LINK:
	pLink = tn3270s_link_entry_get_first(pTn3270s, pSAP,
					     TN3270S_LINK_CONTEXT_THIS_SAP);
	while (pLink) {
	    /* Skip links that are pending delete */
	    if (!(pLink->state & TN3270S_STATE_DELETE)) {
		nv_write(TRUE,
			 "%s%s %8s", pIndentation, csb->nv_command,
			 pLink->name);
		nv_add(!tn3270s_is_loopback_mac(pLink->rmac),
		       " %s %14e", TN3270S_KEY_RMAC, pLink->rmac);
		nv_add((pLink->rsap != TN3270S_RSAP_DEF),
		       " %s %02X", TN3270S_KEY_RSAP, pLink->rsap);
	    }
	    pLink = tn3270s_link_entry_get_next(pTn3270s, pSAP, pLink,
					        TN3270S_LINK_CONTEXT_THIS_SAP);
	}	 
	break;
    }
}

/*
 * ==============================================================
 * Parser action routine: TN3270S cfg command
 * ==============================================================
 */
static void
tn3270s_command (parseinfo *csb)
/*
 * ---------------------------------------------------------------
 * tn3270s_command
 * ---------------------------------------------------------------
 *
 * OBJ(int,20)   = <subcommand mode> 
 *                  TN3270S_MODE_INTERFACE|
 *                  TN3270S_MODE_SVR      |
 *                  TN3270S_MODE_PU       | 
 *                  TN3270S_MODE_DLUR     |
 *                  TN3270S_MODE_DLUR_PU  |
 *                  TN3270S_MODE_DLUR_SAP
 */
{
    idbtype          *swidb;
    cip_idb_infotype *cip;
    tn3270s_t        *pTn3270s;
    ushort            tn3270s_mode;    

    /* Do setup on behalf of all commands */

    swidb = csb->interface;
    cip = channel_sb_get_cip_info(swidb->hwptr);
    if (!cip)
        return;

    /* Can Tn3270s-server be configured on this interface ? */
    if (!cip_cmd_valid_on_interface(swidb->hwptr, CIP_TN3270S_SVR,
				    !csb->nvgen))
	return;

    pTn3270s = cip->pTn3270sData;
    
    /* Process the Ready Queue ? */
    
    tn3270s_mode = GETOBJ(int,20);
    
    if (csb->nvgen) {
	tn3270s_nvgen(csb, tn3270s_mode);
	return;
    }
    
    switch (csb->which) {
	
    case TN3270S_CFG_SVR        :
	tn3270s_svr(csb, swidb, cip, pTn3270s);
	break;
    case TN3270S_CFG_SHUT       :
	tn3270s_shut(csb, swidb, cip, pTn3270s, tn3270s_mode);
	break;
    case TN3270S_CFG_SVR_MAX_LU :
	tn3270s_svr_max_lu(csb, swidb, cip, pTn3270s);
	break;
    case TN3270S_CFG_SVR_TIMING_MARK :
	tn3270s_svr_timing_mark(csb, swidb, cip, pTn3270s);
	break;
    case TN3270S_CFG_SVR_LU_DNS :
	tn3270s_svr_lu_dns(csb, swidb, cip, pTn3270s);
	break;
    case TN3270S_CFG_TCP_PORT   :
    case TN3270S_CFG_IDLE_TIME  :
    case TN3270S_CFG_KEEPALIVE  :
    case TN3270S_CFG_UNBIND_ACT :
    case TN3270S_CFG_GPOOL      :
	tn3270s_siftdown(csb, swidb, cip, pTn3270s, tn3270s_mode);
	break;
    case TN3270S_CFG_PU         :
	tn3270s_svr_pu(csb, swidb, cip, pTn3270s);
	break;
    case TN3270S_CFG_PU_DNS     :
	tn3270s_pu_dns(csb, swidb, cip, pTn3270s, tn3270s_mode);
	break;
    case TN3270S_CFG_DLUR       :
	tn3270s_dlur(csb, swidb, cip, pTn3270s);
	break;
    case TN3270S_CFG_DLUR_DLUS  :
	tn3270s_dlur_dlus(csb, swidb, cip, pTn3270s);
	break;
    case TN3270S_CFG_DLUR_NN_SVR:
	tn3270s_dlur_nn_svr(csb, swidb, cip, pTn3270s);
	break;
    case TN3270S_CFG_DLUR_PU    :
	tn3270s_dlur_pu(csb, swidb, cip, pTn3270s);
	break;
    case TN3270S_CFG_DLUR_SAP   :
	tn3270s_dlur_sap(csb, swidb, cip, pTn3270s);
	break;
    case TN3270S_CFG_DLUR_SAP_VRN:
	tn3270s_dlur_sap_vrn(csb, swidb, cip, pTn3270s);
	break;
    case TN3270S_CFG_DLUR_SAP_LINK:
	tn3270s_dlur_sap_link(csb, swidb, cip, pTn3270s);
	break;
    }
}

/*
 * ---------------------------------------------------------------------------
 * Subcommand entry function
 *
 * tn3270s_svr_subcommand
 * tn3270s_pu_subcommand
 * tn3270s_dlur_subcommand
 * tn3270s_dlur_pu_subcommand
 * tn3270s_dlur_sap_subcommand
 * ---------------------------------------------------------------------------
 */

/*
 * tn3270s_subcommand
 *     We are about to enter sub command mode.
 *     - First push the alternate parse token.  This allows the parser
 *       to continue on in the event that this parse fails.
 *     - If we are doing nvgen, there are two cases:
 *          
 */
static void
tn3270s_subcommand (parseinfo  *csb,
		    transition *mine,
		    ushort      csb_which)
{
    idbtype            *swidb;
    cip_idb_infotype   *cip;
    tn3270s_t          *pTn3270s;
    tn3270s_pu_t       *pPU;
    tn3270s_dlur_t     *pDLUR;
    tn3270s_dlur_sap_t *pSAP;
    tn3270s_putype_t    pu_type;

    swidb = csb->interface;
    cip = channel_sb_get_cip_info(swidb->hwptr);
    if (!cip)
        return;
    pTn3270s = cip->pTn3270sData;

    /*
     * Setup for the alternate transition
     */
    push_node(csb, mine->alternate);
    
    pPU = NULL;
    pDLUR = NULL;
    pSAP = NULL;

    /*
     * Set up to generate config commands for nvram 
     */
    if (csb->nvgen) {
	/* Set mode context pointer and push instances, where applicable */
	switch (csb_which) {
	case TN3270S_CFG_SVR     :
	    /* Make sure this is the Virtual interface */
	    if (!cip_cmd_valid_on_interface(swidb->hwptr, CIP_TN3270S_SVR,
					    !csb->nvgen))
		return;
	    /* Also return if not configured or PNDING DELETE ? */
	    break;
	case TN3270S_CFG_PU      :
	case TN3270S_CFG_DLUR_PU :
	    /* Push backwards */
	    pu_type = (csb_which == TN3270S_CFG_PU) ?
		TN3270S_PUTYPE_DIRECT : TN3270S_PUTYPE_DLUR;

	    pPU = tn3270s_pu_entry_get_last(pTn3270s);
	    while (pPU) {
		if (!(pPU->state & TN3270S_STATE_DELETE) &&
		    (pPU->type == pu_type)) {
		    SETOBJ(udb,1) = pPU;
		    push_node(csb, mine->accept);
		}
		pPU = tn3270s_pu_entry_get_prev(pTn3270s, pPU);
	    }
	    return;
	case TN3270S_CFG_DLUR    :
	    /* generalized nvgen cmd looks for ptr here */
	    pDLUR = &pTn3270s->dlur;
	    /* should these checks be here ? */
	    if (!(pDLUR->state & TN3270S_STATE_INUSE))
		return;
	    if (pDLUR->state & TN3270S_STATE_DELETE)
		return;
	    SETOBJ(udb,1) = pDLUR;
	    break;
	case TN3270S_CFG_DLUR_SAP:
	    pSAP = tn3270s_sap_entry_get_last(pTn3270s);
	    while (pSAP) {
		if (!(pSAP->state & TN3270S_STATE_DELETE)) {
		    SETOBJ(udb,1) = pSAP;
		    push_node(csb, mine->accept);
		}
		pSAP = tn3270s_sap_entry_get_prev(pTn3270s, pSAP);
	    }
	    return;
	default:
	    break;
	}
	/* For modes that don't have instances, just push accepting node */
	push_node(csb, mine->accept);
	return;
    }
    push_node(csb, mine->accept);
}
/*
 * ---------------------------------------------------------------------------
 * Subcommand exit
 *   csb->which indicates which subcommand we are leaving:
 *               TN3270S_CFG_SVR
 *               TN3270S_CFG_PU
 *               TN3270S_CFG_DLUR
 *               TN3270S_CFG_DLUR_PU
 *               TN3270S_CFG_DLUR_SAP
 * ---------------------------------------------------------------------------
 */
static void
tn3270s_exit_subcommand (parseinfo *csb)
{
    idbtype          *swidb;
    cip_idb_infotype *cip;
    tn3270s_t        *pTn3270s;
    tn3270s_pu_t     *pPU;
    ushort            mode_index;

    /* [no] exit is [non]sense :-) */
    if (!csb->sense)
	return;

    /* debuggery */
    mode_index = GETOBJ(int,20);

    if (csb->nvgen) {
	/* Set the mode we are returning to */
	switch (csb->which) {
	case TN3270S_CFG_SVR:
	    SETOBJ(int,20) = TN3270S_MODE_INTERFACE;
	    break;
	case TN3270S_CFG_PU:
	    SETOBJ(int,20) = TN3270S_MODE_SVR;
	    break;
	case TN3270S_CFG_DLUR:
	    SETOBJ(int,20) = TN3270S_MODE_SVR;
	    break;
	case TN3270S_CFG_DLUR_PU:
	    SETOBJ(int,20) = TN3270S_MODE_DLUR;
	    break;
	case TN3270S_CFG_DLUR_SAP:
	    SETOBJ(int,20) = TN3270S_MODE_DLUR;
	    break;
	}
	/* Set the current mode to one up */
	csb->mode = get_alt_mode(csb->mode);
	return;
    }
    
    swidb = csb->interface;
    cip = channel_sb_get_cip_info(swidb->hwptr);
    if (!cip)
        return;

    pTn3270s = cip->pTn3270sData;
    
    /* Do any subcommand exit clean-up */
    switch (csb->which) {
    case TN3270S_CFG_SVR     :
    	pTn3270s->svr.state &= ~TN3270S_STATE_CFGING;
	break;
    case TN3270S_CFG_DLUR_PU :
    case TN3270S_CFG_PU      :
	pPU = csb->userDataBlock[UDB_TN3270S_CONTEXT];
	if (pPU->new)
	    tn3270s_cfgcmd_pu(swidb, cip, pTn3270s,
			      pPU, TN3270S_CFGTYPE_CREATE);
	break;
    case TN3270S_CFG_DLUR    :
	break;
    case TN3270S_CFG_DLUR_SAP:
	break;
    default:
	return;
    }
    /* Set the current mode to one up */
    csb->mode = get_alt_mode(csb->mode);
}

/*
 * ---------------------------------------------------------------------------
 * Subcommand set vars, reset vars
 *
 * tn3270s_svr_set_vars
 * tn3270s_pu_set_vars
 * tn3270s_dlur_set_vars
 * tn3270s_dlur_pu_set_vars
 * tn3270s_dlur_sap_set_vars
 * ---------------------------------------------------------------------------
 */
/*
 * ---------------------------------------------------------------------------
 * tn3270s_set_vars
 *    We are leaving this mode in search of validity
 *    at a higher level.
 *  - Give the parser a pointer to the current context
 *    in case the command is not valid at a higer level
 *    and we return back to this mode
 * ---------------------------------------------------------------------------
 */

static void *
tn3270s_set_vars (parseinfo *csb,
		  ushort     tn3270s_mode)
{
    void             *p;
    idbtype          *swidb;
    cip_idb_infotype *cip;
    tn3270s_t        *pTn3270s;
    ushort            mode_index;
    tn3270s_pu_t     *pPU;

    swidb = csb->interface;
    cip = channel_sb_get_cip_info(swidb->hwptr);
    if (!cip)
        return NULL;

    pTn3270s = cip->pTn3270sData;
    
    switch (tn3270s_mode) {
    case TN3270S_MODE_INTERFACE:
	break;
    case TN3270S_MODE_SVR     :
    	pTn3270s->svr.state &= ~TN3270S_STATE_CFGING;
	break;
    case TN3270S_MODE_DLUR_PU :
    case TN3270S_MODE_PU      :

	pPU = csb->userDataBlock[UDB_TN3270S_CONTEXT];
	if (pPU->new)
	    tn3270s_cfgcmd_pu(swidb, cip, pTn3270s, 
			      pPU, TN3270S_CFGTYPE_CREATE);
	break;
    case TN3270S_MODE_DLUR    :
	break;
    case TN3270S_MODE_DLUR_SAP:
	break;
    default:
	return NULL;
    }

    /* No nested subcommands */
    mode_index = UDB_TN3270S_CONTEXT;
    
    p = csb->userDataBlock[mode_index];

    csb->userDataBlock[mode_index] = NULL;
    return(p);
}

/*
 * ---------------------------------------------------------------------------
 * tn3270s_reset_vars
 *
 *    ...."We're baaaaack.....
 *
 *    The command parse at a higher level was not successful
 *  - The parser is giving us back the context pointer that we
 *    gave in the tn3270s_set_vars routine
 *
 *  - Reenter the subcommand mode
 * ---------------------------------------------------------------------------
 */
static void
tn3270s_reset_vars (parseinfo *csb,
		    void      *var,
		    ushort     tn3270s_mode)
{
    idbtype          *swidb;
    cip_idb_infotype *cip;
    tn3270s_t        *pTn3270s;
    ushort            mode_index;

    swidb = csb->interface;
    cip = channel_sb_get_cip_info(swidb->hwptr);
    if (!cip)
        return;

    pTn3270s = cip->pTn3270sData;
    
    switch (tn3270s_mode) {
    case TN3270S_MODE_INTERFACE:
	break;
    case TN3270S_MODE_SVR     :
    	pTn3270s->svr.state |= TN3270S_STATE_CFGING;
	break;
    case TN3270S_MODE_PU      :
	break;
    case TN3270S_MODE_DLUR    :
	break;
    case TN3270S_MODE_DLUR_PU :
	break;
    case TN3270S_MODE_DLUR_SAP:
	break;
    default:
	return;
    }
    mode_index = UDB_TN3270S_CONTEXT;
    csb->userDataBlock[mode_index] = var;
}

/*
 * ==============================================================
 * Show commands: Query information from the CIP
 * ==============================================================
 */
static boolean
tn3270s_query_next_link (idbtype             *swidb,
			 boolean              detail,
			 tn3270s_dlur_sap_t  *pSAP,
			 tn3270s_show_link_t *pShowLink)
{
    tn3270s_t        *pTn3270s;
    boolean           success;
    cip_idb_infotype *cip;
    
    cip = channel_sb_get_cip_info(swidb->hwptr);
    if (!cip)
        return FALSE;
    pTn3270s = cip->pTn3270sData;

    success = FALSE;
    
    pShowLink->status = TN3270S_STATUS_NOTQRYD;
    
    if (tn3270s_query_shut_svr(pTn3270s))
	return success;
    if (tn3270s_query_shut_dlur(pTn3270s))
	return success;
    /*
     * Query may be for a dynamic link
     * in which case a NULL pSAP is passed in 
     */
    if (pSAP)
	if (tn3270s_query_shut_dlur_sap(pSAP))
	    return success;
    
    pShowLink->status = TN3270S_STATUS_NONE;
    success = tn3270s_cip_query(swidb->hwptr,
				TN3270S_FIND_NEXT, detail,
				TN3270S_CIP_QUERY_DLUR_LINK,
				pShowLink);
    return success;
}

static boolean
tn3270s_query_this_link (idbtype             *swidb,
			 boolean              detail,
			 tn3270s_dlur_sap_t  *pSAP,
			 tn3270s_link_t      *pLink,
			 tn3270s_show_link_t *pShowLink)
{
    tn3270s_t        *pTn3270s;
    boolean           success;
    
    cip_idb_infotype *cip;
    
    cip = channel_sb_get_cip_info(swidb->hwptr);
    if (!cip)
        return FALSE;
    pTn3270s = cip->pTn3270sData;
    success = FALSE;

    pShowLink->status = TN3270S_STATUS_NOTQRYD;
    if (pLink) {
	pShowLink->rsap = pLink->rsap;
	ieee_copy(pLink->rmac, pShowLink->rmac);
    }
    
    if (tn3270s_query_shut_svr(pTn3270s))
	return success;
    if (tn3270s_query_shut_dlur(pTn3270s))
	return success;
    /*
     * Query may be for a dynamic link
     * in which case a NULL pSAP is passed in 
     */
    if (pSAP)
	if (tn3270s_query_shut_dlur_sap(pSAP))
	    return success;
    
    pShowLink->status = TN3270S_STATUS_NONE;
    success = tn3270s_cip_query(swidb->hwptr,
				TN3270S_FIND_MATCH, detail,
				TN3270S_CIP_QUERY_DLUR_LINK,
				pShowLink);
    return success;	
}

static boolean
tn3270s_query_this_sap (idbtype                 *swidb,
			boolean                  detail,
			tn3270s_dlur_sap_t      *pSAP,
			tn3270s_show_dlur_sap_t *pShowSAP)
{
    tn3270s_t        *pTn3270s;
    boolean           success;
    cip_idb_infotype *cip;
    
    cip = channel_sb_get_cip_info(swidb->hwptr);
    if (!cip)
        return FALSE;
    pTn3270s = cip->pTn3270sData;

    success = FALSE;

    pShowSAP->index = 0;
    pShowSAP->status = TN3270S_STATUS_NOTQRYD;
    
    /* 
     * Form 'port' name from adapter type and number. 
     * Note that this APPN/DLUR implementation assumes
     * a single port (sap) per adapter
     */

    sprintf(pShowSAP->name, "%s%02d", 
            tn3270s_port_type_string(pSAP->lan_type), pSAP->adapno);
    
    if (tn3270s_query_shut_svr(pTn3270s))
	return success;
    if (tn3270s_query_shut_dlur(pTn3270s))
	return success;
    if (tn3270s_query_shut_dlur_sap(pSAP))
	return success;

    pShowSAP->status = TN3270S_STATUS_NONE;
    success = tn3270s_cip_query(swidb->hwptr,
				TN3270S_FIND_MATCH, detail,
				TN3270S_CIP_QUERY_DLUR_SAP,
				pShowSAP);
    return success;	
}

static boolean
tn3270s_query_next_client_lu (idbtype           *swidb,
			      boolean            detail,
			      tn3270s_show_lu_t *pShowLU)
{
    tn3270s_t        *pTn3270s;
    boolean           success;
    
    cip_idb_infotype *cip;
    
    cip = channel_sb_get_cip_info(swidb->hwptr);
    if (!cip)
        return FALSE;
    pTn3270s = cip->pTn3270sData;

    success = FALSE;
    
    if (tn3270s_query_shut_svr(pTn3270s))
	return success;
    
    success = tn3270s_cip_query(swidb->hwptr,
				TN3270S_FIND_NEXT, detail,
				TN3270S_CIP_QUERY_CLIENT_LU,
				pShowLU);
    return success;	
}


static boolean
tn3270s_query_pu (idbtype* swidb, tn3270s_pu_t *pPU)
{

    tn3270s_t *pTn3270s;
    cip_idb_infotype *cip;
    
    cip = channel_sb_get_cip_info(swidb->hwptr);
    if (!cip)
        return FALSE;
 
    pTn3270s = cip->pTn3270sData;

    if (tn3270s_query_shut_svr(pTn3270s))
	return FALSE;
    
    if (pPU->type == TN3270S_PUTYPE_DLUR)
	if (tn3270s_query_shut_dlur(pTn3270s))
	    return FALSE;
    
    if (tn3270s_query_shut_pu(pPU))
	return FALSE;

    return TRUE;              /* pu is defined and opened */
}

static boolean
tn3270s_query_this_pu (idbtype           *swidb,
		       boolean            detail,
		       tn3270s_pu_t      *pPU,
		       tn3270s_show_pu_t *pShowPU)
{
    tn3270s_t *pTn3270s;
    cip_idb_infotype *cip;
    
    cip = channel_sb_get_cip_info(swidb->hwptr);
    if (!cip)
        return FALSE;
    pTn3270s = cip->pTn3270sData;

    pShowPU->status = TN3270S_STATUS_NOTQRYD;
    pShowPU->dlurStatus = TN3270S_STATUS_NONE;        /* this is not dlur */
    pShowPU->index = pPU->index;
    pShowPU->xid = pPU->xid;

    pShowPU->ipaddr = pPU->ipaddr;
    pShowPU->lsap = pPU->lsap;
    ieee_copy(pPU->rmac, pShowPU->rmac);
    pShowPU->rsap = pPU->rsap;
    pShowPU->siftdown.tcp_port.val = pPU->siftdown.tcp_port.val;

    /* Bypass initialization of fields if not detailed request */
    if (detail) {
	pShowPU->siftdown.idle_time.val = pPU->siftdown.idle_time.val;
	pShowPU->siftdown.keepalive.val = pPU->siftdown.keepalive.val;

        /* TN3270 MIB values and TN3270_XXX defines mismatched for the following two elements */
	pShowPU->siftdown.unbind_act.val =   (pPU->siftdown.unbind_act.val == TN3270S_UNBIND_ACT_KEEP) ?
                                              D_tn3270sPuUnbindAction_keep : D_tn3270sPuUnbindAction_disconnect;    
	pShowPU->siftdown.generic_pool.val = (pPU->siftdown.generic_pool.val == TN3270S_GPOOL_PERMIT) ?
                                              D_tn3270sPuGenericPool_permit : D_tn3270sPuGenericPool_deny;

	sstrncpy(pShowPU->lu_seed, pPU->lu_seed, MAX_NAME_SIZE+1);
    }
    /*
     * Note: if the tn3270s-server is down, or the PU is down
     *       don't query from the CIP; display configured vals
     */
    if (!tn3270s_query_pu(swidb, pPU))
          return FALSE;

    pShowPU->status = TN3270S_STATUS_NONE;  /* if status not updated */
   
    return tn3270s_cip_query(swidb->hwptr,
				TN3270S_FIND_MATCH, detail,
				(pPU->type == TN3270S_PUTYPE_DIRECT) ?
				TN3270S_CIP_QUERY_PU : TN3270S_CIP_QUERY_DLUR_PU,
				pShowPU);
}

static boolean
tn3270s_query_this_dlur (idbtype             *swidb,
			 boolean              detail,
			 tn3270s_show_dlur_t *pShowDLUR, int index)
{
    tn3270s_t        *pTn3270s;
    tn3270s_dlur_t   *pDLUR;
    boolean           success;
    cip_idb_infotype *cip;
    
    cip = channel_sb_get_cip_info(swidb->hwptr);
    if (!cip)
        return FALSE;
    pTn3270s = cip->pTn3270sData;

    pDLUR = &pTn3270s->dlur;
    success = FALSE;
    
    pShowDLUR->index = index;    
    pShowDLUR->dlur_dlus_status = TN3270S_STATUS_NOTQRYD;
    pShowDLUR->cp_cp_status = TN3270S_STATUS_NOTQRYD;

    sstrncpy(pShowDLUR->dlur_fq_cpname,
	     pDLUR->dlur_fq_cpname, MAX_FQNN_SIZE+1);
    sstrncpy(pShowDLUR->dlur_fq_dlusname,
	     pDLUR->dlur_fq_dlusname, MAX_FQNN_SIZE+1);
    sstrncpy(pShowDLUR->dlur_dlus_backup_name,
	     pDLUR->dlur_dlus_backup.name, MAX_FQNN_SIZE+1);
    sstrncpy(pShowDLUR->dlur_nn_svr,
	     pDLUR->dlur_nn_svr.name, MAX_FQNN_SIZE+1);

    if (tn3270s_query_shut_svr(pTn3270s))
	return success;
    if (tn3270s_query_shut_dlur(pTn3270s))
	return success;

    pShowDLUR->dlur_dlus_status = TN3270S_STATUS_NONE;
    success = tn3270s_cip_query(swidb->hwptr,
				TN3270S_FIND_MATCH, detail,
				TN3270S_CIP_QUERY_DLUR,
				pShowDLUR);
    return success;	
}

static boolean
tn3270s_query_this_svr (idbtype            *swidb,
			boolean             detail,
			tn3270s_t          *pTn3270s,
			tn3270s_show_svr_t *pShowSvr)
{
    boolean success;
    
    success = FALSE;

    pShowSvr->max_lu = pTn3270s->svr.max_lu.val;
    pShowSvr->timingMarkSupported = pTn3270s->svr.timing_mark.val;
    pShowSvr->siftdown.tcp_port.val = pTn3270s->svr.siftdown.tcp_port.val;
    pShowSvr->siftdown.idle_time.val = pTn3270s->svr.siftdown.idle_time.val;
    pShowSvr->siftdown.keepalive.val = pTn3270s->svr.siftdown.keepalive.val;

    /* TN3270 MIB values and TN3270_XXX defines mismatched for the following two elements */
    pShowSvr->siftdown.unbind_act.val =   (pTn3270s->svr.siftdown.unbind_act.val == TN3270S_UNBIND_ACT_KEEP) ? 
                                           D_tn3270sPuUnbindAction_keep : D_tn3270sPuUnbindAction_disconnect;  
    pShowSvr->siftdown.generic_pool.val = (pTn3270s->svr.siftdown.generic_pool.val == TN3270S_GPOOL_PERMIT) ?
                                           D_tn3270sPuGenericPool_permit : D_tn3270sPuGenericPool_deny;

    sstrncpy(pShowSvr->lu_dns_fqdn, 
	     pTn3270s->svr.lu_dns.dns_fqdn, MAX_FQDN_SIZE+1);
    sstrncpy(pShowSvr->lu_rev_dns_fqdn, 
	     pTn3270s->svr.lu_dns.rev_dns_fqdn, MAX_FQDN_SIZE+1);
    pShowSvr->index = 0;

    if (tn3270s_query_shut_svr(pTn3270s))
	return success;

    success = tn3270s_cip_query(swidb->hwptr,
				TN3270S_FIND_MATCH, detail,
				TN3270S_CIP_QUERY_SVR,
				pShowSvr);
    return success;	
}
/*
 * ==============================================================
 * Show commands: Display state and status strings
 * ==============================================================
 */
static const char *
tn3270s_state_string (ushort state)
{
    if (state & (TN3270S_STATE_CREATE | TN3270S_STATE_PNDING))
	return "PND-CRE";

    if (state & (TN3270S_STATE_DELETE | TN3270S_STATE_PNDING))
	return "PND-DEL";
    
    if (state & (TN3270S_STATE_UPDATE | TN3270S_STATE_PNDING))
    	return "PND-UPD";
    
    if (state & TN3270S_STATE_VC_SR)
	return "SROUTE";
    
    if (state & TN3270S_STATE_SHUT)
	return "SHUT";
    
    if (state & TN3270S_STATE_CIP_SHUT)
	return "CIP-SHUT";
    
    if (state & TN3270S_STATE_CFGING)
	return "CFGING";
    
    return "";
}

static const char *
tn3270s_dlur_status_string (ushort status)
{
    switch (status) {
    case TN3270S_STATUS_NONE:	
	return TN3270S_STATUS_STR_NONE;
    case TN3270S_STATUS_NOTQRYD:
	return TN3270S_STATUS_STR_NOTQRYD;
    case TN3270S_STATUS_NOTKNOWN:
	return TN3270S_STATUS_STR_NOTKNOWN;

    case  TN3270S_STATUS_DLUR_RESET      :
	return TN3270S_STATUS_STR_RESET;
    case  TN3270S_STATUS_DLUR_PENDACTIVE :
	return TN3270S_STATUS_STR_PENDACTIVE;
    case  TN3270S_STATUS_DLUR_ACTIVE     :
	return TN3270S_STATUS_STR_ACTIVE;
    case  TN3270S_STATUS_DLUR_PENDINACT  :
	return TN3270S_STATUS_STR_PENDINACT;
    default:
	return TN3270S_STATUS_STR_UNKNOWN;
    }
}


static const char *
tn3270s_linksap_status_string (ushort status)
{
    switch (status) {
    case TN3270S_STATUS_NONE:	
	return TN3270S_STATUS_STR_NONE;
    case TN3270S_STATUS_NOTQRYD:
	return TN3270S_STATUS_STR_NOTQRYD;
    case TN3270S_STATUS_NOTKNOWN:
	return TN3270S_STATUS_STR_NOTKNOWN;

    case  TN3270S_STATUS_LINK_INACTIVE   :
	return TN3270S_STATUS_STR_INACTIVE;
    case  TN3270S_STATUS_LINK_ACTIVE     :
	return TN3270S_STATUS_STR_ACTIVE;
    case  TN3270S_STATUS_LINK_PENDACTIVE :
	return TN3270S_STATUS_STR_PENDACTIVE;
    case  TN3270S_STATUS_LINK_PENDINACT :
	return TN3270S_STATUS_STR_PENDINACT;
    default:
	return TN3270S_STATUS_STR_UNKNOWN;
    }
}
static const char *
Dlur_status_string (ushort status)
{
    switch (status) {
    case TN3270S_STATUS_DLUR_PU_RESET:
	return "RESET";
    case TN3270S_STATUS_DLUR_PU_PND_REQ_ACT_RSP:
	return "P-RQACTPU-R";
    case TN3270S_STATUS_DLUR_PU_PND_ACTPU:
	return "P-ACTPU";
	
    case TN3270S_STATUS_DLUR_PU_ACTIVE:
	return "P-ACTIVE";    /* not possible, pu is not active ? */
	
    case TN3270S_STATUS_DLUR_PU_PND_DACTPU_RSP:
        return "P-DACTPU";
        
    default:
        return TN3270S_STATUS_STR_UNKNOWN;
    }
}


static const char *
tn3270s_pu_status_string (ushort status, ushort dlurStatus)
{
    switch (status) {
    case TN3270S_STATUS_NONE:	
	return "WAIT";
    case TN3270S_STATUS_NOTQRYD:
	return "SHUT";
	
    case D_tn3270sPuState_reset:
        return (dlurStatus == 0) ? "RESET" : Dlur_status_string(dlurStatus);
            
    case D_tn3270sPuState_inactive:
        return (dlurStatus == 0) ? "P-ACTPU" : Dlur_status_string(dlurStatus);
            
    case D_tn3270sPuState_pActpu:
        return "P-ACTPU";

    case D_tn3270sPuState_active:
	return "ACTIVE";

    case  D_tn3270sPuState_actBusy:
        return "ACT/BUSY";

    case D_tn3270sPuState_test:
        return "TEST";

    case D_tn3270sPuState_xid:
        return "XID";
        
    default:
        return "UNKNOWN";
    }
}


/*
 * ---------------------------------------------------------------
 * tn3270s_show_this_link_cfg
 * show extended chan <slot>/<port> tn3270-server dlur
 * show extended chan <slot>/<port> tn3270-server dlurlink <name>
 *
....5....1....5....2....5.v..3....5....4....5....5..v.5....6....5....7....5....8
 * ---------------------------------------------------------------
 * show extended chan <slot>/<port> tn3270-server dlur
 * show extended chan <slot>/<port> tn3270-server dlurlink <name>
auburn#show ext c 2/2 tn dlurlink p390
link P390                 remote 4000.7470.00e7 08  status INACTIVE          
partner                   tgn          0            maxdata      0
sessions     0            maxin        0            maxout       0
 * ---------------------------------------------------------------
 */  

static void
tn3270s_show_this_link_cfg (idbtype             *swidb,
			    cip_idb_infotype    *cip,
			    tn3270s_t           *pTn3270s,
			    tn3270s_show_link_t *pShowLink,
			    boolean              detail)
{
    printf("\n"
	   "%4s %8s"
	   "%13s",	   
	   TN3270S_SHOW_STR_DLUR_SAP_LINK,
	   pShowLink->name,
	   "");
    printf("%s %14s",
	   TN3270S_SHOW_STR_REMOTE, tn3270s_rmac_string(pShowLink->rmac));
    printf("%1s"
	   "%02X"
	   "%2s"
	   "%s %8s"
	   "%1s %8s",
	   "",
	   pShowLink->rsap,
	   "",
	   TN3270S_SHOW_STR_STATUS,
	   tn3270s_linksap_status_string(pShowLink->status),
	   "",
	   pShowLink->dynamic ? "DYNAMIC" : "");
    if (!detail) {
	return;
    }
    
    printf("\n"
	   "%8s%17s"
	   "%1s"	   
	   "%8s %-5d"
	   "%12s"
	   "%8s %-5d",
	   TN3270S_SHOW_STR_PARTNER,
	   pShowLink->partner_name,
	   "",
	   TN3270S_SHOW_STR_TGN, pShowLink->tgn,
	   "",
	   TN3270S_SHOW_STR_MAXDATA,pShowLink->maxdata);
    
    /*
     * Suppress display of irretrievable information:

    printf("\n"
	   "%8s %-5d"
	   "%12s"	   
	   "%8s %-5d"
	   "%12s"
	   "%8s %-5d",
	   TN3270S_SHOW_STR_SESSIONS, pShowLink->sessions,
	   "",
	   TN3270S_SHOW_STR_MAXIN, pShowLink->maxin,
	   "",
	   TN3270S_SHOW_STR_MAXOUT, pShowLink->maxout);
     */
}

static void
tn3270s_show_all_link_cfg (idbtype            *swidb,
			   cip_idb_infotype   *cip,
			   tn3270s_t          *pTn3270s,
			   tn3270s_dlur_sap_t *pSAP)
{
    tn3270s_link_t      *pLink;
    tn3270s_show_link_t *pShowLink;
    void                *pVlan;
    void                *pVadapt;
    boolean              success;
    ulong                lan_id;
    ulong                lan_type;
    boolean              found;
    
    pShowLink = malloc(sizeof(*pShowLink));
    if (!pShowLink) {
	printf(nomemory);
	return;
    }

/* NOTE: Links get dynamically established with DLUR;
 * must loop querying CIP for these after looping through
 * configured links
 */
    /* For all configured links on this SAP */
    pLink = tn3270s_link_entry_get_first(pTn3270s, pSAP,
					 TN3270S_LINK_CONTEXT_THIS_SAP);
    while (pLink) {
	if (automore_quit())
	    break;
	sstrncpy(pShowLink->name, pLink->name, MAX_NAME_SIZE+1);
	
	tn3270s_query_this_link(swidb, TN3270S_QUERY_STATUS,
				pSAP, pLink, pShowLink);
	tn3270s_show_this_link_cfg(swidb, cip, pTn3270s,
				   pShowLink, TN3270S_QUERY_STATUS);
	
	pLink = tn3270s_link_entry_get_next(pTn3270s, pSAP, pLink,
					    TN3270S_LINK_CONTEXT_THIS_SAP);
    }
    /*   
     * Now show all dynamic links that
     * are established on this SAP
     */
    bzero(pShowLink->name, sizeof(pShowLink->name));
    
    pShowLink->lsap = pSAP->lsap;
    /*
     * Get the first lan of this type
     * then get lmac of the matching adapter
     */
    found = FALSE;
    lan_id = 0;
    do {
	pVlan = cip_get_vlan(swidb->hwptr, TN3270S_FIND_NEXT,
			     pSAP->lan_type, lan_id);
	if (pVlan) {
	    lan_type = get_vlan_lan_type(pVlan);
	    if (lan_type != pSAP->lan_type) { 
		pVlan = NULL;
	    } else {
		lan_id = get_vlan_lan_id(pVlan);
		pVadapt = cip_get_vadapt_from_adapno(swidb->hwptr, pVlan,
						     TN3270S_FIND_MATCH,
						     pSAP->adapno);
		if (pVadapt) {
		    found = TRUE;
		    ieee_copy(get_vadapt_mac_address(pVadapt),
			      pShowLink->lmac);
		    do {
			if (automore_quit())
			    break;
			success = tn3270s_query_next_link(swidb,
							  TN3270S_QUERY_STATUS,
							  pSAP, pShowLink);
			if (success && pShowLink->dynamic) {
			    tn3270s_show_this_link_cfg(swidb, cip, pTn3270s,
						       pShowLink,
						       TN3270S_QUERY_STATUS);
			}
		    } while (success);
		}
		lan_id++;
	    }
	}
    } while (pVlan && !found);
    free(pShowLink);
}

/*
 * ---------------------------------------------------------------
 * tn3270s_show_this_sap_cfg
 * ---------------------------------------------------------------
 * show extended chan <slot>/<port> tn3270-server dlur
 * show extended chan <slot>/<port> tn3270-server dlurlink <name>
....5....1....5....2....5.v..3....5....4....5....5..v.5....6....5....7....5....8
sap  tokenring   0 C0     vrn <not configured>      status NONE             
 * ---------------------------------------------------------------
 */  

static void
tn3270s_show_this_sap_cfg (idbtype            *swidb,
			   cip_idb_infotype   *cip,
			   tn3270s_t          *pTn3270s,
			   tn3270s_dlur_sap_t *pSAP)
{
    tn3270s_show_dlur_sap_t *pShowSAP;
    
    if (!pSAP)
	return;
    
    pShowSAP = malloc(sizeof(*pShowSAP));
    if (!pShowSAP) {
	printf(nomemory);
	return;
    }
    tn3270s_query_this_sap(swidb, TN3270S_QUERY_STATUS,
			   pSAP, pShowSAP);
    
    printf("\n");
    printf("\n"
	   "%4s %10s %-2d %02X"
	   "%5s"	   
	   "%3s %17s"
	   "%5s"
	   "%s %8s"
	   "%1s",
	   TN3270S_SHOW_STR_DLUR_SAP,
	   tn3270s_lan_type_string(pSAP->lan_type),
	   pSAP->adapno,
	   pSAP->lsap,
	   "",
	   TN3270S_SHOW_STR_DLUR_SAP_VRN,
	   (pSAP->dlur_sap_vrn.use_default ?
	    TN3270S_SHOW_STR_NOT_CFGD : pSAP->dlur_sap_vrn.name),
	   "",
	   TN3270S_SHOW_STR_STATUS,
	   tn3270s_linksap_status_string(pShowSAP->status),
	   "");
    printf("%8s",
	   tn3270s_state_string(pSAP->state));

    free(pShowSAP);
}
/*
 * ---------------------------------------------------------------
 * tn3270s_show_all_sap_cfg
 * ---------------------------------------------------------------
 * show extended chan <slot>/<port> tn3270-server dlur
....5....1....5....2....5.v..3....5....4....5....5..v.5....6....5....7....5....8
sap  tokenring   0 C0     vrn <not configured>      status NONE             
link SATURN               remote 4000.0469.0077 04  status INACTIVE          
link JUPITER              remote 4000.0847.7477 04  status INACTIVE          
link P390                 remote 4000.7470.00e7 08  status INACTIVE         
sap  tokenring   1 40     vrn <not configured>      status NONE              
...
 * ---------------------------------------------------------------
 */  

static void
tn3270s_show_all_sap_cfg (idbtype          *swidb,
			  cip_idb_infotype *cip,
			  tn3270s_t        *pTn3270s)
{
    tn3270s_dlur_sap_t     *pSAP;


    pSAP = tn3270s_sap_entry_get_first(pTn3270s);
    while (pSAP) {
	if (automore_quit())
	    return;
	tn3270s_show_this_sap_cfg(swidb, cip, pTn3270s, pSAP);
	tn3270s_show_all_link_cfg(swidb, cip, pTn3270s, pSAP);
	pSAP = tn3270s_sap_entry_get_next(pTn3270s, pSAP);
    }
}

static const char* GetLuStatus (int status)
{
   static const char *const luStatus[] = {
      "UNKNOWN",
      "INACTIVE",
      "ACT/NA",
      "P-SDT",
      "ACT/SESS",
      "P-ACTLU",
      "P-NTF/AV",
      "P-NTF/UA",
      "P-RESET",
      "P-PSID",
      "P-BIND",
      "P-UNBIND",
      "WT-UNBND",
      "WT-SDT"
   };
   if (status > D_tn3270sLuState_unbindWt) {
      status = 0;
   }
   return luStatus[status];
  
}

static const char luBanner[] = 
"\nNote: if state is ACT/NA then the client is disconnected"
"\n\nlu    name   client-ip:tcp        state    model   frames in out   idle for";

/*
 * ---------------------------------------------------------------
 * category
 *   returns the lu state category
 * ---------------------------------------------------------------
 */ 
 
enum show_state_t_ {
    DISC_CLIENT = TN3270S_SHOW_CLIENT_THIS_DISCONNECTED, 
    PEND_CLIENT = TN3270S_SHOW_CLIENT_THIS_PENDING, 
    INSESS_CLIENT = TN3270S_SHOW_CLIENT_THIS_INSESSION,
    ALL_CLIENT = TN3270S_SHOW_CLIENT_THIS_ALL  
};
typedef enum show_state_t_ show_state_t;

static show_state_t
category (uchar lu_state)
{
  return ((lu_state == D_tn3270sLuState_actSession)? 
               INSESS_CLIENT:
               ((lu_state == D_tn3270sLuState_active ||
                 lu_state == D_tn3270sLuState_inactive)?
                     DISC_CLIENT:
                     PEND_CLIENT));
}


/*
 * ---------------------------------------------------------------
 * tn3270s_show_all_client_lu
 *   show all lu information for specific client ip address
 * ---------------------------------------------------------------
 */  
static void
tn3270s_show_all_client_lu (idbtype          *swidb,
			    cip_idb_infotype *cip,
			    tn3270s_t        *pTn3270s,
			    ipaddrtype        ipaddr,
                            uchar             show_state)
{
    tn3270s_pu_t      *pPU;
    tn3270s_show_lu_t *pShowLU;
    int                count;
    boolean            success;

    pShowLU = malloc(sizeof(*pShowLU));
    if (!pShowLU){
	printf(nomemory);
	return;
    }

    pShowLU->client_ipaddr = ipaddr;
    pShowLU->client_port = 0;
    count = 0;
    
    do {
	if (automore_quit())
	    break;
	
	success = tn3270s_query_next_client_lu(swidb, TN3270S_QUERY_STATUS,
					       pShowLU);
	if (success) {
	    pPU = tn3270s_pu_entry_locate_by_snmp_index(pTn3270s, 
							pShowLU->pu_index);
            if (show_state & category(pShowLU->lu_state)) {
                printf(luBanner);
	        sstrncpy(pShowLU->pu_name, pPU->name,
		   	         MAX_NAME_SIZE+1);
	        tn3270s_show_lu(swidb, cip, pTn3270s, pShowLU, TN3270S_LU_DETAIL);
	    }
	    count++;
            pShowLU->client_port++;       /* next client */
	}
    } while (success);
    free(pShowLU);
    
    /* Don't report "No clients" and "Total clients" if automore_quit */
    if (automore_quit())
	return;
    
    if (!count) {
	printf("\nNo clients found using %i",
	       ipaddr);
    } else {
        printf("\nTotal %d clients found using %i", count, ipaddr);
    }
}

/*
 * ---------------------------------------------------------------
 * tn3270s_show_lu
 * show extended chan <slot>/<port> tn3270-server pu <puname>
 * show extended chan <slot>/<port> tn3270-server pu <puname> lu <lu-number>
 *
....5....1....5....2v...5....3....5....4v...5....5....5....6v...5....7....5....8
lu    name     client ip:tcp     state     model   frames in out   last-used
12  pu02001  192.195.80.40:1023  act-sess 3270E4  1234567 9999999  12:23:0

 * ---------------------------------------------------------------
 * show extended chan <slot>/<port> tn3270-server pu <puname> lu <lu-number>
auburn#show ext c 2/2 tn pu pu2 lu 0
12  pu02001  192.195.80.40:1023  act-sess 3270E4  1234567/9999999 1234/9999
pu is CAPPU, lu is STATIC type 0, never connected by clients
PU is CAPPU, lu is DYNAMIC type 2, use TN3270, DLUR link %s, lfsid 1e45
bytes in/out 12345678/9999999, RuSize in/out 2048/1024, NegRsp in/out 0/2
inactvity period 5 seconds.
pacing window in/out 4/0, credits(in) 2, queue size in/out 0/0,
last activity: Client connect req, Reply Psid RSP, notify req, actlu req, 
   bind req, sdt req.
*/

 
static const char* GetEvent (uint event)
{
   static const char *const luEvents[] = {
           "unknown",
           "inactivity timer expired",
           "dynamic timer expired",
           "actlu req",
           "bind req",
           "clear req",
           "dactlu req",
           "warm actpu req",
           "sdt req",
           "unbind req",
           "notify resp",
           "Reply PSID neg rsp",
           "Reply PSID pos rsp",
           "Unbind rsp",
           "hierarchical reset",
           "Client connect req",
           "Client disconnect req",
           "Timing Mark received",
           "flow control timer expired",
           "Neg rsp to host",
           "Neg rsp from host",
           "Data contention",
           "no buffer to send sna data",
           "receive response while inbound"
       };

   if (event > sizeof(luEvents)/sizeof(char*)) {
        event = 0;
    }
   return luEvents[event];
}


/* ---------------------------------------------------------------
 */  
static void
tn3270s_show_lu (idbtype           *swidb,
			 cip_idb_infotype  *cip,
			 tn3270s_t         *pTn3270s, 
			 tn3270s_show_lu_t *pShowLU,
			 lu_detail           detail)
{
    char ip[28];
    char time[12];
    ushort min;
    
    min = pShowLU->idleTime % 3600;
    
    sprintf(time, "%d:%d:%d", pShowLU->idleTime/3600, min/60,min%60);

    if (pShowLU->client_ipaddr == 0) {
       strcpy(ip, "never connected");
    }
    else {
       sprintf(ip, "%i:%d", pShowLU->client_ipaddr, pShowLU->client_port);
    }
    printf("\n%3d %8s %21s %8s %8s %7d %7d %s",
        pShowLU->lu_index, 
        tn3270s_name_from_fqname(pShowLU->lu_name),
        ip,
        GetLuStatus(pShowLU->lu_state),
        pShowLU->display_mod_name,
        pShowLU->sentRus, pShowLU->receivedRus,
        time);

        
    if (detail != TN3270S_NO_LU_DETAIL) {
       int i;
       char luType[10];
       sprintf(luType, "type %d", pShowLU->luType);
       printf("\n\npu is %s, lu is %s %s, ",
           pShowLU->pu_name,
           pShowLU->dynamicType == D_tn3270sLuType_dynamic ? "DYNAMIC" : "STATIC",
           pShowLU->luType ? luType : "unbound");
       if (pShowLU->client_ipaddr == 0) {
           printf(" never connected by clients");
           return;
       }
       printf("negotiated %s", pShowLU->telnetType == D_tn3270sLuTelnetType_tn3270e ?
                              "TN3270E" : "TN3270");

       if (pShowLU->lfsid) {
           printf("\nDLUR link %s, lfsid %x",pShowLU->appnLinkName,pShowLU->lfsid);
       } 

       printf("\nbytes %u in, %u out; RuSize %d in, %d out; NegRsp %d in, %d out",
              pShowLU->sentBytes, pShowLU->receivedBytes,
              pShowLU->maxSndRuSize, pShowLU->maxRcvRuSize,
              pShowLU->sentNegativeResps, pShowLU->receivedNegativeResps);
              
       printf("\npacing window %d in, %d out; credits %d in, queue-size %d in, %d out",
               pShowLU->sndPacingSize,  pShowLU->rcvPacingSize,
               pShowLU->curInbPacing,
               pShowLU->curInbQsize, pShowLU->curOutQsize);

       if (detail == TN3270S_LU_DETAIL_HISTORY) {
          printf("\ntraces: ");
          for (i = 0; i < MaxEvents && pShowLU->luEvents[i]; i++) {
             printf("\n         %s", GetEvent(pShowLU->luEvents[i]));
          }
          for (i = 0; i < NumberOfTraceBuffer; i++) {
            /* if less than 11 bytes or telnet data, print all
             * else if in print 2cxxxxxxxx
             * else       print ddxxxxxxxx
             */
            
            uchar* s;
            int len;
            boolean in;
            boolean telnet;

            s = pShowLU->trace[i];
               
            len = ((s[0] & 0x1f) << 8) + s[1];
            if (len == 0) {
               break;
	    }
            in = (s[0] & InboundData);
            if ((telnet = (s[0] & TelnetTrace)) != 0) {
               printf("\nTELNET");
            }
            else {
               printf("\n");
	    }
            printf("%s len=%4d ", in? "IN " : "OUT", len);

            s += 2;
            if (len > 10) {
                if (!telnet){
                   len -= 5;     /* 5 bytes used */
                   if (in) {
                       printf("xxxxxxxxxx");
                   }
                   else {
                      printf("%02xxxxxxxxx", *s++);
                      if (len > 9) {
                        len = 9;
		     }
		   }
	        }
                if (len > 10) {
                   len = 10;
	        }
	    }
            while (len-- > 0) {
               printf("%02x", *s++);
            }
         }
      }
   }
}


static void
tn3270s_show_all_pu_lu (idbtype          *swidb,
			cip_idb_infotype *cip,
			tn3270s_t        *pTn3270s, 
			tn3270s_pu_t     *pPU)
{
    tn3270s_show_lu_t *pShowLU;
    boolean            success;

    pShowLU = malloc(sizeof(*pShowLU));
    if (!pShowLU){
	printf(nomemory);
	return;
    }

    pShowLU->lu_index = 0;
    pShowLU->pu_index = pPU->index;
    printf(luBanner);
    do {
	if (automore_quit())
	    break;
        success = tn3270s_cip_query(swidb->hwptr,TN3270S_FIND_NEXT, TN3270S_QUERY_STATUS,
				TN3270S_CIP_QUERY_LU,
				pShowLU);
	if (success){
	    tn3270s_show_lu(swidb, cip, pTn3270s, pShowLU, TN3270S_NO_LU_DETAIL);
            pShowLU->lu_index++;       /* scan next lu */
        }
    } while (success);
    
    free(pShowLU);
}
static void
apply_siftdown(tn3270s_t        *pTn3270s,
               tn3270s_pu_t     *pPU,
               tn3270s_show_pu_t *pShowPU)
{
    if (pPU->siftdown.tcp_port.use_default) {
      /* use sifted down value */
      pShowPU->siftdown.tcp_port.val = pTn3270s->svr.siftdown.tcp_port.val;
      
    }
      
    
    if (pPU->siftdown.idle_time.use_default) {
      /* use sifted down value */
      pShowPU->siftdown.idle_time.val = pTn3270s->svr.siftdown.idle_time.val;
    }
    
    if (pPU->siftdown.keepalive.use_default) {
      /* use sifted down value */
      pShowPU->siftdown.keepalive.val = pTn3270s->svr.siftdown.keepalive.val;
    }

    
    if (pPU->siftdown.unbind_act.use_default) {
      /* use sifted down value */
      pShowPU->siftdown.unbind_act.val = pTn3270s->svr.siftdown.unbind_act.val;
    }
    
    if (pPU->siftdown.generic_pool.use_default) {
      /* use sifted down value */
      pShowPU->siftdown.generic_pool.val = pTn3270s->svr.siftdown.generic_pool.val;
    }
}
/*
 * ---------------------------------------------------------------
 * tn3270s_show_pu
 * show extended chan <slot>/<port> tn3270-server pu <pu-name>
 *
....5....1....5....2v...5....3....5....4v...5....5....5....6v...5....7....5....8
 * ---------------------------------------------------------------

name(index)    ip:tcp               xid   state     link   destination   r-lsap
PUS11(3)     172.28.1.107:23     05D19011 ACTIVE    tok 0  4000.7470.00e7 08 28

idle-time    0      keepalive  240      unbind-act discon   generic-pool perm
bytes 353 in, 4757 out; frames 13 in, 17 out; NegRsp 0 in, 0 out
actlus 1, dactlus 0, binds 2

lu    name   client-ip:tcp        state    model   frames in out   idle for
1   PUS11001 192.195.80.40:2720   ACT/SESS 327804   13      14      0:5:14

 * ---------------------------------------------------------------
 */


static void
tn3270s_show_pu (idbtype          *swidb,
			  cip_idb_infotype *cip,
			  tn3270s_t        *pTn3270s, 
			  tn3270s_pu_t     *pPU,
			  boolean           detail)
{
    tn3270s_show_pu_t *pShowPU;
    char  name[16];
    char  ip[28];

    pShowPU = malloc(sizeof(*pShowPU));
    if (!pShowPU) {
	printf(nomemory);
	return;
    }
    if (!tn3270s_query_this_pu(swidb, detail, pPU, pShowPU)) {
      /* cip was shut or this PU was shut so we need to apply siftdown
         locally here */
      apply_siftdown(pTn3270s, pPU, pShowPU);
      
    }
    
    sprintf(name, "%s(%d)", pPU->name, pShowPU->index);
    sprintf(ip, "%i:%d", pShowPU->ipaddr, pShowPU->siftdown.tcp_port.val);
    printf("\n%12s %19s %08X %10s", 
           name, ip,
           pShowPU->xid,
	   tn3270s_pu_status_string(pShowPU->status, pShowPU->dlurStatus));

    if (pPU->type == TN3270S_PUTYPE_DIRECT){
        printf("%3s %2d %s %02X %02X",
	    tn3270s_lan_type_string(pPU->lan_type), pPU->adapno,
            tn3270s_rmac_string(pShowPU->rmac),
            pShowPU->rsap,
            pShowPU->lsap);
    }
    else {
       printf("dlur   %s", pShowPU->name_at_host);
    }
    if (detail) {
       printf("\n");
       if (pPU->type == TN3270S_PUTYPE_DIRECT && pShowPU->lu_seed[0]){
	    printf("\nluseed %s", pShowPU->lu_seed);
       }
       printf("\n"
	   "%s%-5d"
	   "%6s"
	   "%s%-5d"
	   "%6s"
	   "%10s %6s"
	   "%3s"
	   "%s %4s",
	   TN3270S_SHOW_STR_IDLE_TIME, pShowPU->siftdown.idle_time.val,
	   "",
	   TN3270S_SHOW_STR_KEEPALIVE, pShowPU->siftdown.keepalive.val,
	   "",
	   TN3270S_SHOW_STR_UNBIND_ACT, 
	   (pShowPU->siftdown.unbind_act.val ==
	    D_tn3270sPuUnbindAction_keep) ? TN3270S_SHOW_STR_UNBIND_KEEP :
	   TN3270S_SHOW_STR_UNBIND_DISC,
	   "",
	   TN3270S_SHOW_STR_GPOOL, 
	   (pShowPU->siftdown.generic_pool.val ==
	    D_tn3270sPuGenericPool_permit) ? TN3270S_SHOW_STR_GPOOL_PERMIT :
	   TN3270S_SHOW_STR_GPOOL_DENY);

      printf("\nbytes %llu in, %llu out; frames %u in, %u out; NegRsp %d in, %d out",
          pShowPU->sentBytes.l, pShowPU->receivedBytes.l,
          pShowPU->sentPius, pShowPU->receivedPius,
          pShowPU->sentNegativeResps, pShowPU->receivedNegativeResps);

      printf("\nactlus %d, dactlus %d, binds %d",
          pShowPU->actLus, pShowPU->dactLus, pShowPU->bindLus);

      if (tn3270s_query_pu(swidb, pPU)) {
          tn3270s_show_all_pu_lu(swidb, cip, pTn3270s, pPU);
      }
   }
   free(pShowPU);
}

/*
 * ---------------------------------------------------------------
 * tn3270s_show_all_pu_cfg
 * show extended chan <slot>/<port> tn3270-server
 *
 * ---------------------------------------------------------------
 */  

/* Show all configurations of specific PU type */
static void
tn3270s_show_all_pu_cfg (idbtype          *swidb,
			 cip_idb_infotype *cip,
			 tn3270s_t        *pTn3270s, 
			 tn3270s_putype_t  pu_type)
{
    tn3270s_pu_t     *pPU;
    
    pPU = tn3270s_pu_entry_get_first(pTn3270s);
    while (pPU) {
	if (pPU->type == pu_type) {
	    if (automore_quit())
		return;
	    tn3270s_show_pu(swidb, cip, pTn3270s, pPU, FALSE);
	}
	pPU = tn3270s_pu_entry_get_next(pTn3270s, pPU);
    }
}    
static void
tn3270s_show_all_server_stats (idbtype          *swidb,
			 cip_idb_infotype *cip,
			 tn3270s_t        *pTn3270s)
{
   int inUse;
   int totalSession = 0;
   int totalInuse = 0;
   int banner = FALSE;
   char  ip[28];
   tn3270s_show_stat_t *pShowStat;
   if (tn3270s_query_shut_svr(pTn3270s)){
        printf("\n");
	return ;
   }
   pShowStat = malloc(sizeof(*pShowStat));
   if (!pShowStat) {
	printf(nomemory);
	return;
   }
   pShowStat->ipAddr = 0;
   pShowStat->tcpPort = 0;
   while (tn3270s_cip_query_stats(swidb->hwptr, pShowStat)) {
      if (!banner) {
          printf("\n                 <current stats> < connection stats >  <response time(ms)>");
          printf("\nserver-ip:tcp        lu in-use   connect disconn fail   host     tcp");
          banner = TRUE;
      }
      sprintf(ip, "%i:%d", pShowStat->ipAddr,  pShowStat->tcpPort);
      inUse = pShowStat->maxSess - pShowStat->spareSess;
      printf("\n%18s %-4d  %-4d %-8d %-8d %-5d %-6d %-6d",
        ip,
        pShowStat->maxSess, inUse,
        pShowStat->connectsIn, pShowStat->disconnects,
        pShowStat->connectsFailed,
        pShowStat->siTransactions == 0 ? 0 : 
                    pShowStat->siTransactionsTime*10/pShowStat->siTransactions,
        pShowStat->soTransactions == 0 ? 0 : 
                    pShowStat->soTransactionsTime*10/pShowStat->soTransactions);
      totalSession += pShowStat->maxSess;
      totalInuse += inUse;
      pShowStat->tcpPort++;    /* search next stats */
      if (! pShowStat->tcpPort) break;   /* If gone past 0xFFFF break to avoid going thru again. */
   }
   printf("\ntotal             %-5d %-5d",totalSession,totalInuse);
   free(pShowStat);
}
           
     

/*
 * ---------------------------------------------------------------
 * tn3270s_show_all_dlur_cfg
 * show extended chan <slot>/<port> tn3270-server dlur
 *
 * ---------------------------------------------------------------
....5....1....5....2....5....3....5....4v...5....5....5....6....5....7....5....8
dlur MPX.REDBCP                                     
current dlus MPX.NGMVMPC                dlur-dlus status INACTIVE
current server MPX.abcde                cp-cp status NOTQRYD
preferred dlus MPX.NGMVMPC              backup dlus MPX.DLUSBACK
preferred server MPX.PREFNN  
....5....1....5....2....5.v..3....5....4....5....5..v.5....6....5....7....5....8

sap  tokenring   0 C0     vrn <not configured>      status NONE             
link SATURN               remote 4000.0469.0077 04  status INACTIVE          
link JUPITER              remote 4000.0847.7477 04  status INACTIVE          
link P390                 remote 4000.7470.00e7 08  status INACTIVE          

sap  tokenring   8 C0     vrn <not configured>      status NONE             
link BANANA               remote 0000.0000.0000 04  status NONE              
link BEST                 remote 4444.5555.6666 88  status NONE              
...
 * ---------------------------------------------------------------
 */  
static void
tn3270s_show_all_dlur_cfg (idbtype          *swidb,
			   cip_idb_infotype *cip,
			   tn3270s_t        *pTn3270s)
{
    tn3270s_dlur_t      *pDLUR;
    tn3270s_show_dlur_t *pShowDLUR;
    
    pDLUR = &pTn3270s->dlur;
    
    if (!(pDLUR->state & TN3270S_STATE_INUSE))
	return;

    pShowDLUR = malloc(sizeof(*pShowDLUR));
    if (!pShowDLUR) {
	printf(nomemory);
	return;
    }

    /* By virtue of our config implementation index=1 is primary DLUS
       while index >1 is secondary DLUS; currently only one
       secondary/bacup-dlus is possible*/
    tn3270s_query_this_dlur(swidb, TN3270S_QUERY_DETAIL, pShowDLUR,1);/* primary DLUS */
    
    printf("\n"
	   "%s %17s"
	   "%18s",
	   TN3270S_SHOW_STR_DLUR,
	   pShowDLUR->dlur_fq_cpname,
	   "");
    printf("%8s",
	   tn3270s_state_string(pDLUR->state));
    
    printf("\n"
	   "%s %s %17s"
	   "%7s"	   
	   "%s %s %8s",
	   TN3270S_SHOW_STR_PREFERRED, TN3270S_SHOW_STR_DLUS,
	   pShowDLUR->current_dlusname,
	   "",
	   TN3270S_SHOW_STR_DLUR_DLUS, TN3270S_SHOW_STR_STATUS,
	   tn3270s_dlur_status_string(pShowDLUR->dlur_dlus_status));

    /*
     * Suppress display of irretrievable information:
  
    printf("\n"
           "%s %s %17s"
           "%8s"	   
           "%s %s %s",
           TN3270S_SHOW_STR_CURRENT, TN3270S_SHOW_STR_SERVER,
           pShowDLUR->current_nn_svr,
           "",
           TN3270S_SHOW_STR_CP_CP, TN3270S_SHOW_STR_STATUS,
           tn3270s_cpcp_status_string(pShowDLUR->cp_cp_status));
     */

    printf("\n"
	   "%s %s %17s",
	   TN3270S_SHOW_STR_BACKUP, TN3270S_SHOW_STR_DLUS,
	   pShowDLUR->dlur_dlus_backup_name[0] ?
	   pShowDLUR->dlur_dlus_backup_name : TN3270S_SHOW_STR_NOT_CFGD);
    
    if(pShowDLUR->dlur_dlus_backup_name[0]) {

      tn3270s_query_this_dlur(swidb, TN3270S_QUERY_DETAIL, pShowDLUR,2);/* secondary DLUS */

      printf("%10s%s %s %8s",
             "",
             TN3270S_SHOW_STR_DLUR_DLUS, TN3270S_SHOW_STR_STATUS,
             tn3270s_dlur_status_string(pShowDLUR->dlur_dlus_status));
    }

    printf("\n"
	   "%s %s %17s",
	   TN3270S_SHOW_STR_PREFERRED, TN3270S_SHOW_STR_SERVER,
	   pShowDLUR->dlur_nn_svr[0] ?
	   pShowDLUR->dlur_nn_svr : TN3270S_SHOW_STR_NOT_CFGD);
    
    tn3270s_show_all_sap_cfg(swidb, cip, pTn3270s);
    free(pShowDLUR);
}


static const char puBanner[] = 
"\n\nname(index)    ip:tcp               xid   state     link   destination   r-lsap";

/*
 * ---------------------------------------------------------------
 * tn3270s_show_all_svr_cfg
 * show extended chan <slot>/<port> tn3270-server
 *
 * ---------------------------------------------------------------
....5....1....5....2....5v...3....5....4....5v...5....5....6....5....7....5....8

                 <current stats> < connection stats >  <response time(ms)>
server-ip:tcp        lu in-use   connect disconn fail   host     tcp
172.28.1.106:23     255     1        1        0     0      0      0
172.28.1.107:23     255     1        2        1     0      0      0
172.28.1.108:23     255     0        0        0     0      0      0
172.28.1.109:23     255     0        0        0     0      0      0
total              1020     2
configured max_lu 20000
idle-time    0           keepalive  240      unbind-action disconnect  
tcp-port   23            generic-pool permit
dlur MPX.GOANCP                              status ACTIVE           
dlus MPX.NGMVMPC      

name(index)    ip:tcp               xid   state     link   destination   r-lsap
EXT2(1)      172.28.1.109:23     05D18092 ACTIVE    tok 0  4000.7470.00e7 08 04
PUS10(2)     172.28.1.107:23     05D19010 SHUT      tok 0  4000.7470.00e7 08 2C
PUS11(3)     172.28.1.107:23     05D19011 ACTIVE    tok 0  4000.7470.00e7 08 28
PUS12(4)     172.28.1.108:23     05D19012 ACTIVE    tok 0  4000.7470.00e7 08 24
INT1(5)      172.28.1.106:23     05D18091 ACTIVE    dlur   MPX.GOAN1
 * ---------------------------------------------------------------
 */  
static void
tn3270s_show_all_svr_cfg (idbtype          *swidb,
			  cip_idb_infotype *cip,
			  tn3270s_t        *pTn3270s)
{    
    tn3270s_dlur_t      *pDLUR;
    tn3270s_show_svr_t  *pShowSvr;
    tn3270s_show_dlur_t *pShowDLUR;

    pShowSvr = malloc(sizeof(*pShowSvr));
    if (!pShowSvr) {
	printf(nomemory);
	return;
    }
    
    tn3270s_show_all_server_stats (swidb, cip, pTn3270s);
    tn3270s_query_this_svr(swidb, TN3270S_QUERY_STATUS,
			   pTn3270s, pShowSvr);
    
    printf("\nconfigured max_lu %5d", pShowSvr->max_lu);
    
    /* Omit lu-dns-domain if not configured */
    if (pShowSvr->lu_dns_fqdn[0]) {
	printf("\n"
	       "%13s %11s"
	       "%s",
	       TN3270S_SHOW_STR_SVR_LU_DNS, TN3270S_SHOW_STR_STEM,
	       pShowSvr->lu_dns_fqdn[0] ?
	       pShowSvr->lu_dns_fqdn : TN3270S_SHOW_STR_NOT_CFGD);
	printf("\n"
	       "%13s %11s"
	       "%s",
	       TN3270S_SHOW_STR_SVR_LU_DNS, TN3270S_SHOW_STR_REVERSE,
	       pShowSvr->lu_rev_dns_fqdn[0] ?
	       pShowSvr->lu_rev_dns_fqdn : TN3270S_SHOW_STR_NOT_CFGD);
    }
    printf("\n"
	   "%s%-5d"
	   "%11s"
	   "%s%-5d"
	   "%6s"
	   "%s %12s",
	   TN3270S_SHOW_STR_IDLE_TIME, pShowSvr->siftdown.idle_time.val,
	   "",
	   TN3270S_SHOW_STR_KEEPALIVE, pShowSvr->siftdown.keepalive.val,
	   "",
	   TN3270S_SHOW_STR_UNBIND_ACT, 
	   (pShowSvr->siftdown.unbind_act.val ==
	    D_tn3270sPuUnbindAction_keep) ? TN3270S_SHOW_STR_UNBIND_KEEP :
	   TN3270S_SHOW_STR_UNBIND_DISC);
    printf("\n"
	   "%s%-5d"
	   "%12s"
	   "%s %s",
	   TN3270S_SHOW_STR_TCP_PORT, pShowSvr->siftdown.tcp_port.val,
	   "",
	   TN3270S_SHOW_STR_GPOOL, 
	   (pShowSvr->siftdown.generic_pool.val ==
	    D_tn3270sPuGenericPool_permit) ? TN3270S_SHOW_STR_GPOOL_PERMIT :
	   TN3270S_SHOW_STR_GPOOL_DENY);

    printf(" %s timing-mark", pShowSvr->timingMarkSupported == 1 ? "use" : "no");
    
    pDLUR = &pTn3270s->dlur;
    
    if ((pDLUR->state & TN3270S_STATE_INUSE)) {
	pShowDLUR = malloc(sizeof(*pShowDLUR));
	if (!pShowDLUR) {
	    printf(nomemory);
	    free(pShowSvr);
	    return;
	}

        /* By virtue of our config implementation index=1 is primary
       DLUS while index >1 is secondary DLUS; currently only one
       secondary/bacup-dlus is possible*/

	tn3270s_query_this_dlur(swidb, TN3270S_QUERY_STATUS, pShowDLUR, 1);/* primary DLUS*/
	printf("\n"
	       "%s %17s" /* DLUS cpname */
	       "%23s"
	       "%s %8s"   /* STATUS status */
	       "%1s",
	       TN3270S_SHOW_STR_DLUS, pShowDLUR->dlur_fq_dlusname,
	       "",
	       TN3270S_SHOW_STR_STATUS,
	       tn3270s_dlur_status_string(pShowDLUR->dlur_dlus_status),
	       "");
	printf("%8s",
	       tn3270s_state_string(pDLUR->state));
        
        if(pShowDLUR->dlur_dlus_backup_name[0]) {

          /* only do this if backup is configured */
          tn3270s_query_this_dlur(swidb, TN3270S_QUERY_STATUS, pShowDLUR, 2);/* secondary DLUS */
          printf("\n"
                 "%s %17s" /* DLUS cpname */
                 "%23s"
                 "%s %8s"   /* STATUS status */
                 "%1s",
                 TN3270S_SHOW_STR_DLUS, pShowDLUR->current_dlusname,
                 "",
                 TN3270S_SHOW_STR_STATUS,
                 tn3270s_dlur_status_string(pShowDLUR->dlur_dlus_status),
                 "");
          printf("%8s",
                 tn3270s_state_string(pDLUR->state));
        }
	printf("\n"
	       "%s %17s",  /* DLUR name */
	       TN3270S_SHOW_STR_DLUR, pShowDLUR->dlur_fq_cpname);
	free(pShowDLUR);
    }
    printf(puBanner);
    tn3270s_show_all_pu_cfg(swidb, cip, pTn3270s, TN3270S_PUTYPE_DIRECT);
    tn3270s_show_all_pu_cfg(swidb, cip, pTn3270s, TN3270S_PUTYPE_DLUR);
    free(pShowSvr);
}
/*
 * ==============================================================
 * Show commands: parser entry point
 * ==============================================================
 */

/* Sample configuration for show output
 *
 * interface Channel2/2
 *  ip address 172.18.4.153 255.255.255.248
 *  no keepalive
 *  lan TokenRing 0
 *   source-bridge 103 1 102
 *   adapter 0 4000.0000.2815
 *  tn3270-server
 *   maximum-lus 1000
 *   lu-dns-domain-stem LU.DNS.DOMAIN.STEM LU.DNS.DOMAIN.REVERSE
 *   pu PU2      05D190B2 172.28.1.99     tokenring  0  4C rmac 4000.7470.00e7 rsap 08
 *   pu PU3      05D190B3 172.28.1.97     tokenring  0  48 rmac 4000.7470.00e7 rsap 08
 *   pu PUFOO1   05D12345 172.18.4.154    tokenring  0  04
 *   pu PUS1     05D19001 172.28.1.100    tokenring  0  20 rmac 4000.7470.00e7 rsap 08
 *   pu PUS3     05D19003 172.28.1.97     tokenring  0  C4 rmac 4000.7470.00e7 rsap 08
 *   dlur MPX.REDBCP MPX.NGMVMPC
 *    dlus-backup AQUANET.DLUSBACK 
 *    preferred-NNserver PERMANET.PREFNN 
 *    sap tokenring  0  C0
 *     link SATURN   rmac 4000.0469.0077
 *     link JUPITER  rmac 4000.0847.7477
 *     link P390     rmac 4000.7470.00e7 rsap 08
 *    sap tokenring  8  C0
 *     link BANANA  
 *     link BEST     rmac 4444.5555.6666 rsap 88
 *    pu DLURFOO1 05D98765 172.18.4.155   
 *    pu PU1      05D190B1 172.28.1.98    
 */
/*
 * ---------------------------------------------------------------
 * tn3270s_show_command
 *
 * show extended chan <slot>/<port>
 *  tn3270-server [pu <pu-name> [lu <lu-number>]]
 *                [client-ip-address <ip-address>]
 *                [dlur]
 *                [dlurlink <link-name>]
 *
 * OBJ(idb,1) = <interface>
 * OBJ(int,1) =  TN3270S_SHOW_SVR
 *               TN3270S_SHOW_PU_THIS
 *               TN3270S_SHOW_PU_THIS_LU_THIS
 *               TN3270S_SHOW_CLIENT_THIS
 *               TN3270S_SHOW_DLUR
 *               TN3270S_SHOW_DLURLINK_THIS
 * ---------------------------------------------------
 * TN3270S_SHOW_PU_THIS
 * OBJ(string,1) = <pu-name>  
 *
 * TN3270S_SHOW_PU_THIS_LU_THIS
 * OBJ(string,1) = <pu-name>  
 * OBJ(int,2)    = <lu-number>
 *
 * ---------------------------------------------------
 * TN3270S_SHOW_DLURLINK_THIS
 * OBJ(string,1) = <link-name>  
 *
 * ---------------------------------------------------
 * TN3270S_SHOW_CLIENT_THIS
 * OBJ(paddr,1)  = <ip-address>
 * ---------------------------------------------------
 * ---------------------------------------------------------------
 */  
static void
tn3270s_show_command (parseinfo *csb)
{
    idbtype             *swidb;
    cip_idb_infotype    *cip;
    tn3270s_t           *pTn3270s;
    tn3270s_pu_t        *pPU;
    tn3270s_link_t      *pLink;
    tn3270s_dlur_sap_t  *pSAP;
    char                 name[MAX_NAME_SIZE+1];
    ipaddrtype           ipaddr;
    tn3270s_show_lu_t   *pShowLU;
    tn3270s_show_link_t *pShowLink;
    boolean              success;
    boolean              detail;
    
    swidb = GETOBJ(idb,1);
    cip = channel_sb_get_cip_info(swidb->hwptr);
    if (!cip)
        return ;
    
    pTn3270s = cip->pTn3270sData;
    if (!pTn3270s)
        return;
    
    if (interface_admin_down(swidb)) {
	printf("\n%s is %s",
	       swidb->namestring,
	       print_hwidbstate(swidb->hwptr));
	return;
    }
    
    if (!(pTn3270s->svr.state & TN3270S_STATE_INUSE)) {
	printf("\ntn3270-server is not configured on %s",
	       swidb->namestring);
	return;
    }
    
    automore_enable(NULL);
    
    switch (GETOBJ(int,1)) {
    case TN3270S_SHOW_SVR       :
	tn3270s_show_all_svr_cfg(swidb, cip, pTn3270s);
	break;
    case TN3270S_SHOW_PU_THIS    :
    case TN3270S_SHOW_PU_THIS_LU_THIS:
	if (tn3270s_string_param(name, GETOBJ(string,1), MAX_NAME_SIZE,
			         STRING_PARAM_UPPERCASE |
				 STRING_PARAM_VERIFY_LEN)) {
	    automore_disable();
	    return;
	}
	pPU = tn3270s_pu_entry_locate_by_name(pTn3270s, name);
	if (!pPU) {
	    printf("\nPU %s is not configured.",
		   name);
	    automore_disable();
	    return;
	}
	switch (GETOBJ(int,1)) {
	case TN3270S_SHOW_PU_THIS    :
	    printf(puBanner);
	    tn3270s_show_pu(swidb, cip, pTn3270s, pPU,
				     TN3270S_QUERY_DETAIL);
	    break;
	case TN3270S_SHOW_PU_THIS_LU_THIS:
	    /* Find it from the CIP */
	    pShowLU = malloc(sizeof(*pShowLU));
	    
	    if (pShowLU) {
		pShowLU->pu_index = pPU->index;
		pShowLU->lu_index = GETOBJ(int,2);
		
                if (tn3270s_query_pu(swidb, pPU) &&
                     tn3270s_cip_query(swidb->hwptr,TN3270S_FIND_MATCH, 
                                TN3270S_QUERY_DETAIL,TN3270S_CIP_QUERY_LU,pShowLU)){
		    sstrncpy(pShowLU->pu_name, pPU->name,
			     MAX_NAME_SIZE+1);
                    printf(luBanner);
		    tn3270s_show_lu(swidb, cip, pTn3270s, pShowLU, 
                         GETOBJ(int,3) ? TN3270S_LU_DETAIL_HISTORY : TN3270S_LU_DETAIL);
		}
		free(pShowLU);
	    }
	    break;
	}
	break;
    case TN3270S_SHOW_DLUR       :
	tn3270s_show_all_dlur_cfg(swidb, cip, pTn3270s);
	break;
    case TN3270S_SHOW_DLURLINK_THIS :
	if (tn3270s_string_param(name, GETOBJ(string,1), MAX_NAME_SIZE,
			         STRING_PARAM_UPPERCASE |
				 STRING_PARAM_VERIFY_LEN)) {
	    automore_disable();
	    return;
	}
	
	pShowLink = malloc(sizeof(*pShowLink));
	if (!pShowLink) {
	    printf(nomemory);
	    automore_disable();
	    return;
	}
	sstrncpy(pShowLink->name, name, MAX_NAME_SIZE+1);
	detail = TN3270S_QUERY_DETAIL;
	
	pLink = tn3270s_link_entry_locate_by_name(pTn3270s, name);
	if (pLink) { 
	    /* Then it is a configured link */
	    success = tn3270s_query_this_link(swidb, TN3270S_QUERY_DETAIL,
					      pLink->pSAP, pLink, pShowLink);
	    /*
	     * The Link is configured but the query failed
	     * if the SAP, DLUR or SVR is shut,
	     * still display the link, not detail
	     */
	    if (!success) {
		if (tn3270s_query_shut_svr(pTn3270s) ||
		    tn3270s_query_shut_dlur(pTn3270s) ||
		    tn3270s_query_shut_dlur_sap(pLink->pSAP)) {
		    success = TRUE;
		    detail = TN3270S_QUERY_STATUS;
		}
	    }
	} else {
	    /* Then it could be a dynamic DLUR link */
	    success = tn3270s_query_this_link(swidb, TN3270S_QUERY_DETAIL,
					      NULL, NULL, pShowLink);
	    if (!success) {
		printf("\nLink %s does not exist",
		       name);
	    }
	}
	if (success) {
	    pSAP = tn3270s_sap_entry_locate_by_xsap_xmac(swidb, pTn3270s,
							 pShowLink->lmac,
							 pShowLink->lsap);
	    if (pLink && !pSAP) {
		/*
		 * Couldn't find lmac on a configured csna adapter; 
		 * it could be a configured link without an associated 
		 * csna adapter configured for it.  Or the lmac wasn't
		 * retrieved because lsap, dlur, or tn3270 is shut. 
		 */
		pSAP = pLink->pSAP;
	    }
	    tn3270s_show_this_sap_cfg(swidb, cip, pTn3270s, pSAP);
	    tn3270s_show_this_link_cfg(swidb, cip, pTn3270s,
				       pShowLink, detail);
	}
	
	free(pShowLink);
	break;
    case TN3270S_SHOW_CLIENT_THIS  :
	ipaddr = GETOBJ(paddr,1)->ip_addr;
	tn3270s_show_all_client_lu(swidb, cip, pTn3270s,
				   ipaddr, GETOBJ(int,2));
	break;
    }
    printf("\n");
    automore_disable();
}

/*
 * ---------------------------------------------------------------
 * tn3270s_show_this_ipvc
 * show extended chan <slot>/<port> vc
 *
 * ---------------------------------------------------------------
....5....1....5....2....5....3....5....4....5....5....5....6....5....7....5....8
VC 	IP address     	 Cfgd	Route
---	----------     	-----	-----
608	172.18.4.154   	    1	    1
609	172.28.1.99    	    1	    1
610	172.28.1.97    	    2	    2
611	172.28.1.100   	    1	    1
612	172.18.4.155   	    1	    1
613	172.28.1.98    	    1	    1
 */  
static void
tn3270s_show_this_ipvc (tn3270s_ipvc_t *pIPVC)
{
    printf("\n"
	   "%3d"
	   "\t%15i"
	   "\t%-5d"
	   "\t%-5d",
	   pIPVC->vc,
	   pIPVC->ipaddr,
	   pIPVC->vc_users,
	   pIPVC->sr_users);
}
/*
 * ---------------------------------------------------------------
 * tn3270s_show_all_ipvc
 * show extended chan <slot>/<port> vc
 *  - Display a list of all the VC IP address mappings
 * ---------------------------------------------------------------
 */  
static void
tn3270s_show_all_ipvc (tn3270s_t *pTn3270s)
{

    tn3270s_ipvc_t *pIPVC;
    int i;

    printf("\n"
	   "%3s"
	   "\t%15s"
	   "\t%-5s"
	   "\t%-5s",
	   TN3270S_SHOW_STR_VC_VCNUM,
	   TN3270S_SHOW_STR_VC_IPADDRESS,
	   TN3270S_SHOW_STR_VC_CFGD,
	   TN3270S_SHOW_STR_VC_ACTV);
    printf("\n"
	   "%3s"
	   "\t%15s"
	   "\t%-5s"
	   "\t%-5s",
	   TN3270S_SHOW_STR_DASHES,
	   TN3270S_SHOW_STR_DASHES,
	   TN3270S_SHOW_STR_DASHES,
	   TN3270S_SHOW_STR_DASHES);

    for (i = 0; i < TN3270S_NUM_VC; i++) {
	if (automore_quit())
	    return;
	pIPVC = &pTn3270s->vc_array[i];
	if (pIPVC->inuse) {
	    tn3270s_show_this_ipvc(pIPVC);
	}
    }
}

/*
 * ---------------------------------------------------------------
 * tn3270s_display_vc_list
 *  called here from if_cip.c by show ext c <slot>/<port> vc
 *
 * ---------------------------------------------------------------
 */  

static void
tn3270s_display_vc_list (idbtype *swidb)
{
    cip_idb_infotype   *cip;
    tn3270s_t          *pTn3270s;
    
    cip = channel_sb_get_cip_info(swidb->hwptr);
    if (!cip)
        return ;

    pTn3270s = cip->pTn3270sData;
    automore_enable(NULL);
    tn3270s_show_all_ipvc(pTn3270s);

    printf("\n");
    automore_disable();
}

/*
 * ---------------------------------------------------------------
 * tn3270s_add_idb_to_list
 *   Add this idb to the list of idbs that have
 *   TN3270 Server configurability.
 *   If the list has not yet been created, do so.
 * ---------------------------------------------------------------
 */
static void
tn3270s_add_idb_to_list (idbtype   *swidb,
			 tn3270s_t *pTn3270s)
{
    ulong list_id;
    
    if (tn3270s_idb_list == IDBLIST_MAX) {
	/* Create the list */
	if (!idb_create_list(IDBLIST_DYNAMIC, &list_id)) 
	    return;
	
	tn3270s_idb_list = list_id;
	tn3270s_snmp_pu_index = TN3270S_SNMP_INDEX_INIT;
    }
    add_tn3270s_entity(swidb->hwptr);     /* add this idb to the entity */

    /* Add this idb to the list */
    idb_add_swidb_to_list(swidb, tn3270s_idb_list);
}

/*
 * ==============================================================
 * Public entry points
 * ==============================================================
 */
/*
 * ---------------------------------------------------------------
 * tn3270s_hwidb_from_pu_index
 *   Determine on which interface this pu_index
 *   is configured and return its hwidb
 * ---------------------------------------------------------------
 */

hwidbtype *
tn3270s_hwidb_from_pu_index (ulong pu_index, boolean match, tn3270s_pu_t** ppPU)
{
    idbtype	     *idb;
    list_element     *l_elt;
    cip_idb_infotype *cip;
    tn3270s_t        *pTn3270s;
    tn3270s_pu_t     *pPU;
    tn3270s_pu_t*    pBestPU;
    hwidbtype*       bestIdb;

    
    if (tn3270s_idb_list == IDBLIST_MAX)
	return NULL;
    
    pBestPU = NULL;
    bestIdb = NULL;
    FOR_ALL_SWIDBS_IN_LIST(tn3270s_idb_list, l_elt, idb) {
        cip = channel_sb_get_cip_info(idb->hwptr);
        if (!cip)
           continue;

	pTn3270s = cip->pTn3270sData;
        if (pTn3270s == NULL) {
           continue;
        }
	pPU = tn3270s_pu_entry_locate_by_snmp_index(pTn3270s, 
	           				    pu_index);
        if (pPU) {
           if (pPU->index != pu_index) {
              if (match) {
                 continue;             /* search for next pu */
	      }
              else if (pBestPU == NULL || pBestPU->index > pPU->index){
                  pBestPU = pPU;
                  bestIdb = idb->hwptr;
	      }
	  }
          else {
             *ppPU = pPU;;
	     return idb->hwptr;
	  }
        }
    }
    *ppPU = pBestPU;
    return bestIdb;
}

tn3270s_pu_t* tn3270s_pu_from_index (hwidbtype* hwidb, boolean match, ulong*  pu_index)
{
   cip_idb_infotype *cip;
   tn3270s_t        *pTn3270s;
   tn3270s_pu_t     *pPU;

   cip = channel_sb_get_cip_info(hwidb);
   if (cip){
      pTn3270s = cip->pTn3270sData;
      if (pTn3270s) {
          pPU = tn3270s_pu_entry_locate_by_snmp_index(pTn3270s, *pu_index);
          if (pPU) {
	    if (match && pPU->index != *pu_index) {
                return NULL;
            }
            *pu_index = pPU->index;
	  }
          return pPU;
      }
   }
   return NULL;
}




/*
 * ----------------------------------------------------------------------------
 */

/* update pu parameters from configuration */

boolean IsPuShut(tn3270s_pu_t* pPU, ulong* puIndex)
{
    *puIndex = pPU->index;
    return tn3270s_query_shut_pu(pPU);
 }

boolean UpdateLocalPuConfig(tn3270s_pu_t* pPU, sysmgt_snatn3270s_table_t type,
    void* entry)
{
   switch(type) {
   
   case SYSMGT_TN3270S_PU:
   {
      Tn3270sPuEntry_t* tn3270sPuEntry = entry;
      tn3270sPuEntry->tn3270sPuIndex = pPU->index;
      tn3270sPuEntry->tn3270sPuIpAddr = pPU->ipaddr;
      tn3270sPuEntry->tn3270sPuTcpPort = pPU->siftdown.tcp_port.val;
      tn3270sPuEntry->tn3270sPuIdleTimeout  = pPU->siftdown.idle_time.val; 
      tn3270sPuEntry->tn3270sPuKeepAlive = pPU->siftdown.keepalive.val;
      tn3270sPuEntry->tn3270sPuUnbindAction =   (pPU->siftdown.unbind_act.val == TN3270S_UNBIND_ACT_KEEP) ?
          D_tn3270sPuUnbindAction_keep : D_tn3270sPuUnbindAction_disconnect;    
      tn3270sPuEntry->tn3270sPuGenericPool = (pPU->siftdown.generic_pool.val == TN3270S_GPOOL_PERMIT) ?
          D_tn3270sPuGenericPool_permit : D_tn3270sPuGenericPool_deny;

      tn3270sPuEntry->tn3270sPuType = pPU->type == TN3270S_PUTYPE_DIRECT ?
                 D_tn3270sPuType_direct :  D_tn3270sPuType_dlur ;
      tn3270sPuEntry->tn3270sPuState = D_tn3270sPuState_shut;
      tn3270sPuEntry->tn3270sLocalSapAddress = pPU->lsap;
      tn3270sPuEntry->tn3270sRemoteSapAddress = pPU->rsap;

      ieee_copy(tn3270sPuEntry->tn3270sRemoteMacAddress, pPU->rmac);
      sstrncpy(tn3270sPuEntry->tn3270sPuLuSeed, pPU->lu_seed, MAX_NAME_SIZE+1);
   }
   break;
   
   case SYSMGT_TN3270S_NODE_ADMIN:
   {
      SnaNodeAdminEntry_t* adminEntryData = entry;
      memset(adminEntryData, 0, sizeof(SnaNodeAdminEntry_t));
      memcpy(adminEntryData->snaNodeAdminName, pPU->name, sizeof(adminEntryData->snaNodeAdminName));
      adminEntryData->snaNodeAdminIndex = pPU->index;
      adminEntryData->snaNodeAdminXid = pPU->xid;
      adminEntryData->snaNodeAdminType = D_snaNodeAdminType_pu20;
      adminEntryData->snaNodeAdminXidFormat = D_snaNodeAdminXidFormat_format3;
      adminEntryData->snaNodeAdminEnablingMethod = D_snaNodeAdminEnablingMethod_startup;
      adminEntryData->snaNodeAdminLuTermDefault = D_snaNodeAdminLuTermDefault_unbind;
      adminEntryData->snaNodeAdminMaxLu = 255;
      adminEntryData->snaNodeAdminHostDescription[0] = 0;
      adminEntryData->snaNodeAdminStopMethod = D_snaNodeAdminStopMethod_other;
      adminEntryData->snaNodeAdminState = D_snaNodeAdminState_inactive;
      adminEntryData->snaNodeAdminRowStatus = D_snaNodeAdminRowStatus_active;
   }
   break;
   
   case SYSMGT_TN3270S_LINK_ADMIN:
   {
      SnaNodeLinkAdminEntry_t* entryData = entry;
      entryData->snaNodeAdminIndex = pPU->index;
      entryData->snaNodeLinkAdminMaxPiu = 0;
      entryData->snaNodeLinkAdminRowStatus = D_snaNodeLinkAdminRowStatus_active;
   }
   break;

   default:
      return FALSE;
   }

   return TRUE;
}


/*
 * ---------------------------------------------------------------
 * cip_tn3270s_create
 *
 * Initialize TN3270 server structure
 *   This is called at initial idb init time.
 *   Create the tn3270s data structure and initialize it to unused.
 *   Add this idb to the list of interfaces that can be 
 *     configured with the TN3270 Server feature.
 * ---------------------------------------------------------------
 */
void
cip_tn3270s_create (idbtype          *swidb,
		    cip_idb_infotype *cip)
{
    tn3270s_t   *pTn3270s;

    if (!cip_interface_is_virtual(swidb->hwptr)) {
	return;
    }
    
    cip->pTn3270sData = malloc(sizeof(tn3270s_t));
    if (!cip->pTn3270sData) {
	printf(nomemory);
	return;
    }
    
    pTn3270s = cip->pTn3270sData;
    bzero(pTn3270s, sizeof(tn3270s_t));

    /* Initialize */
    pTn3270s->svr.state = TN3270S_STATE_UNUSED;
    
    /* Add to list of TN3270 Server idbs */
    tn3270s_add_idb_to_list(swidb, pTn3270s);
}

/*
 * ---------------------------------------------------------------------
 * cip_tn3270s_reset_cfg
 *    Reset the current configuration states
 *
 *    This is called at process level.  
 *      - at cip_reset, after issuing a DISABLE to the CIP
 *      - in the event that cip_reset is called at interrupt
 *        level, this is called at process level
 *        (at deferred enable), prior to issuing the ENABLE
 *
 *    o 
 *
 *    o If the configuration is still PNDING, either because it
 *      got purged rather than processed, or because the CIP did
 *      not acknowledge it, then process it here and take it out of
 *      PNDING state
 *
 * ---------------------------------------------------------------------
 */
void
cip_tn3270s_reset_cfg (hwidbtype        *hwidb,
		       cip_idb_infotype *cip)
{
    tn3270s_t          *pTn3270s;

    if (!cip)
	return;
    
    if (!cip->pTn3270sData)
	return;
    
    if (channel_debug)
	buginf("\n%s: reset_cfg - tn3270 server feature",
	       hwidb->hw_namestring);
    
    pTn3270s = cip->pTn3270sData;

    /* Clear any asynchronous state change pending conditions */
    
    /* If a static route is defined on this vc, delete it */
    
    /* If any configurations are in PNDING state, reset them */

    /* Reset the cfg correlator
     *   not necessary to reset individual cfg correlators ?
     */

    /*    
     * Reset all configurations that are in CIP_SHUT state
     * This assumes that it is shut because of a nacked cfg
     */
    tn3270s_buginf_svr_state_flags(pTn3270s);
    
    if (pTn3270s->svr.state & TN3270S_STATE_DELETE) {
	tn3270s_svr_delete(pTn3270s);
    }
    
    if (!(pTn3270s->svr.state & TN3270S_STATE_INUSE))
	return;
    /*
     * Reset the configuration correlator 
     */
    pTn3270s->tn3270s_correlator = 0;

    tn3270s_reset_all_svr_cfg(hwidb->firstsw, cip, pTn3270s);
}

/* ---------------------------------------------------------------
 * Send all configuration commands.  This is called after the
 * interface has been reset.
 * ---------------------------------------------------------------
 */
void
cip_tn3270s_send_cfg (idbtype *swidb)
{
    cip_idb_infotype   *cip;
    tn3270s_t          *pTn3270s;

    cip = channel_sb_get_cip_info(swidb->hwptr);
    if (!cip)
        return;

    if (!cip->pTn3270sData)
	return;
    
    pTn3270s = cip->pTn3270sData;

    tn3270s_send_all_svr_cfg(swidb, cip, pTn3270s);
}
/*
 * ---------------------------------------------------------------
 * cip_tn3270s_cfg_str
 *
 * Construct an errbuf[32] debug string
 * ---------------------------------------------------------------
 */
void
cip_tn3270s_cfg_str (ushort  cfg_cmd,
		     uchar  *pMsg,
		     uchar  *pCfg,
		     ushort  len)
{
    sprintf(pMsg, " tn3270s cmd %d",
	    cfg_cmd);
}

/*
 * ---------------------------------------------------------------
 * cip_tn3270s_cfg_acknowledge
 *
 * A configuration has been acknowledged by the CIP 
 * --- this is called at process level ---
 *
 * ---------------------------------------------------------------
 */
void
cip_tn3270s_cfg_acknowledge (hwidbtype *hwidb,
			     ushort     ack_flags,
			     ulong      code,
			     uchar     *buf,
			     ulong      len)
{
    tn3270s_cfg_acknowledge(hwidb, ack_flags, code,
			    buf, len);
}
/*
 * ---------------------------------------------------------------
 * cip_tn3270s_display_vc_list
 * ---------------------------------------------------------------
 */
void
cip_tn3270s_display_vc_list (idbtype *swidb)
{
    tn3270s_display_vc_list(swidb);
}

void
tn3270s_cfg_command (parseinfo *csb)
{
    tn3270s_command(csb);
}

void
tn3270s_exit_command (parseinfo *csb)
{
    tn3270s_exit_subcommand(csb);
}

void
tn3270s_show_ext_channel_command (parseinfo *csb)
{
 tn3270s_show_command (csb);
}    

void *
tn3270s_svr_set_vars (parseinfo *csb)
{
    return(tn3270s_set_vars(csb, TN3270S_MODE_SVR));
}

void
tn3270s_svr_reset_vars (parseinfo *csb,
			void      *var)
{
    tn3270s_reset_vars(csb, var, TN3270S_MODE_SVR);
}

void *
tn3270s_pu_set_vars (parseinfo *csb)
{
    return(tn3270s_set_vars(csb, TN3270S_MODE_PU));
}

void
tn3270s_pu_reset_vars (parseinfo *csb,
		       void      *var)
{
    tn3270s_reset_vars(csb, var, TN3270S_MODE_PU);
}

void *
tn3270s_dlur_set_vars (parseinfo *csb)
{
    return(tn3270s_set_vars(csb, TN3270S_MODE_DLUR));
}

void
tn3270s_dlur_reset_vars (parseinfo *csb,
			 void      *var)
{
    tn3270s_reset_vars(csb, var, TN3270S_MODE_DLUR);
}

void *
tn3270s_dlur_pu_set_vars (parseinfo *csb)
{
    return(tn3270s_set_vars(csb, TN3270S_MODE_DLUR_PU));
}

void
tn3270s_dlur_pu_reset_vars (parseinfo *csb,
			    void      *var)
{
    tn3270s_reset_vars(csb, var, TN3270S_MODE_DLUR_PU);
}

void *
tn3270s_dlur_sap_set_vars (parseinfo *csb)
{
    return(tn3270s_set_vars(csb, TN3270S_MODE_DLUR_SAP));
}

void
tn3270s_dlur_sap_reset_vars (parseinfo *csb,
			     void      *var)
{
    tn3270s_reset_vars(csb, var, TN3270S_MODE_DLUR_SAP);
}

void
tn3270s_svr_subcommand (parseinfo  *csb,
			transition *mine,
			void       *null)
{
    tn3270s_subcommand(csb, mine, TN3270S_CFG_SVR);
}

void
tn3270s_pu_subcommand (parseinfo  *csb,
		       transition *mine,
		       void       *null)
{
    tn3270s_subcommand(csb, mine, TN3270S_CFG_PU);
}

void
tn3270s_dlur_subcommand (parseinfo  *csb,
			 transition *mine,
			 void       *null)
{
    tn3270s_subcommand(csb, mine, TN3270S_CFG_DLUR);
}

void
tn3270s_dlur_pu_subcommand (parseinfo  *csb,
			    transition *mine,
			    void       *null)
{
    tn3270s_subcommand(csb, mine, TN3270S_CFG_DLUR_PU);
}

void
tn3270s_dlur_sap_subcommand (parseinfo  *csb,
			     transition *mine,
			     void       *null)
{
    tn3270s_subcommand(csb, mine, TN3270S_CFG_DLUR_SAP);
}

ulong GetTn3270sUpTime (hwidbtype *hwidb)
{
    cip_idb_infotype *cip;
    tn3270s_t        *pTn3270s;

    if ((cip = channel_sb_get_cip_info(hwidb)) == NULL || 
        (pTn3270s = cip->pTn3270sData) == NULL) {
        return 0;
    }
    return pTn3270s->tn3270sUpTime;
}

      
