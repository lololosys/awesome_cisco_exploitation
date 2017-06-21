/* $Id: ospf_support.c,v 3.8.18.19 1996/09/10 03:08:08 snyder Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/ospf_support.c,v $
 *------------------------------------------------------------------
 * ospf.c -- Open Shortest Path First Interior Gateway Routing Protocol
 *
 * October 4 1990, Phanindra Jujjavarapu
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ospf_support.c,v $
 * Revision 3.8.18.19  1996/09/10  03:08:08  snyder
 * CSCdi68568:  more constant opportuniites
 *              160 out of data
 *               72 out of image
 * Branch: California_branch
 *
 * Revision 3.8.18.18  1996/09/08  08:12:16  myeung
 * CSCdi64631:  Router crashes on sh ip os int after fiddling with ip os
 * priority
 * Branch: California_branch
 *
 * Revision 3.8.18.17  1996/08/23  21:36:47  myeung
 * CSCdi66427:  LSA refresh is sent across OSPF demand circuit
 * Branch: California_branch
 *
 * Revision 3.8.18.16  1996/08/17  05:22:27  myeung
 * CSCdi65930:  sh ip ospf database summary-database only shows one
 * process as dflt
 * Branch: California_branch
 *
 * Revision 3.8.18.15  1996/07/19  07:27:22  myeung
 * CSCdi63444:  OSPF router should not be ABR if it dont see neighbor from
 * backbone
 * Branch: California_branch
 *
 * Revision 3.8.18.14  1996/07/09  19:13:01  myeung
 * CSCdi61982:  OSPF network LSA is missing
 * Branch: California_branch
 * - Refine the fix for CSCdi47402
 *
 * Revision 3.8.18.13  1996/06/28  23:19:30  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.8.18.12  1996/06/28  06:04:25  myeung
 * CSCdi61229:  Trying to turn on OSPF on IRB BVI interfaces crashes router
 * Branch: California_branch
 * - Make OSPF recognize BVI
 * - Improve OSPF interface detection so that it will not crash again
 *   when new type of virtual interface is added
 *
 * Revision 3.8.18.11  1996/06/25  21:16:06  myeung
 * CSCdi45110:  OSPF does not load balance over External routes
 * Branch: California_branch
 * - Modify OSPF router route data structure and external route calculation
 *
 * Revision 3.8.18.10  1996/06/18  01:48:05  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.8.18.9  1996/06/05  07:02:24  myeung
 * CSCdi58092:  ospf crash in get_interval when unconfiguring ospf
 * Branch: California_branch
 *
 * Revision 3.8.18.8  1996/05/23  07:37:56  myeung
 * CSCdi58029:  OSPF crash in ospf_interface_up() when unconfiguring OSPF
 * Branch: California_branch
 * - Add boolean parameter to ip_address_change registry to indicate if
 *   the change is for secondary or primary address.
 *
 * Revision 3.8.18.7  1996/05/08  04:27:22  myeung
 * CSCdi56577:  Type-7 summary is not removed from database when summary
 * is disabled
 * Branch: California_branch
 *
 * Revision 3.8.18.6  1996/05/01  08:06:57  myeung
 * CSCdi55367:  Summary from NSSA area does not get outside NSSA
 * Branch: California_branch
 * - Set forwarding address for summary type 7 LSA
 *
 * Revision 3.8.18.5  1996/04/30  23:14:26  myeung
 * CSCdi56225:  OSPF type 7 LSA is not handled correctly
 * Branch: California_branch
 * - Fix request list for type 7 LSA
 * - Remove translated type 5 LSA when type 7 LSA is not reachable
 *
 * Revision 3.8.18.4  1996/04/20  08:03:46  myeung
 * CSCdi54385:  Add show ip ospf maxage for debugging MAXAGE LSA problem
 * Branch: California_branch
 *
 * Revision 3.8.18.3  1996/04/16  19:02:08  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.8.18.2  1996/04/15  21:18:19  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.8.18.1  1996/03/18  20:28:54  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.2  1996/03/07  09:50:08  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.2.1  1996/02/20  01:04:02  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/07  16:13:21  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.7  1996/02/04  06:07:45  myeung
 * CSCdi48132:  Extract additional OSPF specific information out of pdbtype
 *
 * Revision 3.6  1996/02/01  06:05:57  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/22  06:14:46  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.4  1996/01/05  10:17:53  hampton
 * Move/rename the files containing the deprecated timer callback
 * routines.  [CSCdi46482]
 *
 * Revision 3.3  1995/11/17  17:36:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:48:45  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:10:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.10  1995/10/12  11:51:10  myeung
 * CSCdi42047:  Clear up OSPF db flags
 * - Replace bit mask with bit field variable.
 *
 * Revision 2.9  1995/10/08  23:44:41  myeung
 * CSCdi41790:  Boolean db_free in db structure should be replaced by bit
 * field
 * - Backout the patch
 * - Decide to use bit variable instead of bit mask to replace boolean
 *
 * Revision 2.8  1995/10/08  22:49:53  myeung
 * CSCdi41790:  Boolean db_free in db structure should be replaced by bit
 * field
 *
 * Revision 2.7  1995/09/11  23:50:40  hampton
 * Convert IP host and IP routing to fully use the passive timers macros
 * instead of referencing the system clock directly.  [CSCdi40085]
 *
 * Revision 2.6  1995/08/31 00:51:46  myeung
 * CSCdi39540:  Crash in display_ospf_vidb()
 *
 * Revision 2.5  1995/08/01  15:16:03  myeung
 * CSCdi36150:  OSPF database synchronization problem after route recovers
 * - Always take care of the elasped time when calculating LSA age
 * - The maximum age used for comparsion between LSA should be 3600
 *
 * Revision 2.4  1995/07/07  05:26:33  hampton
 * Hide the definitions of watched data structures.  These data structures
 * should never be referenced directly.  [CSCdi36799]
 *
 * Revision 2.3  1995/06/28 03:21:45  hampton
 * Use the passive timer macros instead of direct references to the system
 * clock.  [CSCdi36502]
 *
 * Revision 2.2  1995/06/15 00:18:24  myeung
 * CSCdi35275:  Router crashes in _print_delete_list_db
 *
 * Revision 2.1  1995/06/07  21:10:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "exception.h"
#include "route.h"
#include "../ip/ip.h"
#include "route_inlines.h"
#include "../if/network.h"
#include "config.h"
#include "parser.h"
#include "parser_defs_ospf.h"
#include "name.h"
#include "../util/tree.h"
#include "ospf_pak.h"
#include "ospf_dbase.h"
#include "ospf.h"
#include "../ip/ip_registry.h"
#include "mgd_timers.h"
#include "ospf_inlines.h"


extern void compute_iso_checksum(uchar *, int, uchar *);

static spfruntimetype spf_time[SPF_TIME_MAX]; 
static ushort run_spf_first;
static ushort run_spf_last;
static spfstattimetype spf_proc_time, spf_sum_proc_time, spf_add_route_time;

static queuetype ospf_bad_checksum_queue;

void ospf_add_bad_checksum_queue (paktype *pak)
{
    ospf_bad_cksum *pkt;
    iphdrtype      *ip;
    ospftype       *ospf;
    short	   length;

    ip = (iphdrtype *) ipheadstart(pak);
    ospf = (ospftype *) ipdatastart(pak);
    length = ospf->ospf_length + sizeof(iphdrtype);

    pkt = malloc(sizeof(ospf_bad_cksum) + length);

    if (pkt) {
	pkt->length = length;
	pkt->received_checksum = ospf->ospf_checksum;

	/*
	 * Compute the actual checksum.
	 */
	ospf->ospf_checksum = 0;
	pkt->computed_checksum = ipcrc((ushort *)ospf, ospf->ospf_length);

	/*
	 * Restore the received checksum into the ospf packet.
	 */
	ospf->ospf_checksum = pkt->received_checksum;
	bcopy(ip, pkt->ip_data, length);

	enqueue(&ospf_bad_checksum_queue, pkt);

	if (ospf_bad_checksum_queue.count > MAX_BAD_CKSUM_QUEUE_SIZE) {
	    pkt = dequeue(&ospf_bad_checksum_queue);
	    pkt->next = NULL;
	    free(pkt);
	}
    }
    return;
}

/*
 * ospf_show_bad_checksum_queue
 *
 * Display the packets that are in the bad packets queue.
 * Currently the packet is displayed in hex. Should interpret the OSPF
 * packet eventually. Any volunteers?
 */
static void ospf_show_bad_checksum_queue (void)
{
    ospf_bad_cksum *pkt;
    short	    length;
    short	    i;

    if (QUEUEEMPTY(&ospf_bad_checksum_queue))
	return;

    printf("\nOSPF packets received with bad checksum");
    
    for (pkt = ospf_bad_checksum_queue.qhead; pkt; pkt = pkt->next) {
	length = pkt->length;
	mem_lock(pkt);
	printf("\n  Checksum received - %04x, computed - %04x",
	       pkt->received_checksum, pkt->computed_checksum);
	printf("\n  Raw data");

	for (i = 0; i < length; i += 2) {
	    if (!(i % 28))
		printf("\n    ");
	    printf("%02x%02x ", pkt->ip_data[i], pkt->ip_data[i+1]);
	}
	free(pkt);
	printf("\n");
    }
}

static void show_ospf_summary_address (void)
{
    addrsumtype *range;
    pdbtype *pdb;

    for (pdb = ospf_pdbs_ptr; pdb ; pdb = pdb->ospf_next) {
	if (automore_quit())
	    break;
	mem_lock(pdb);
	printf("\n\nOSPF Process %d, Summary-address\n", pdb->asystem);
	if (QUEUEEMPTY(&pdb->ip_address_sumq)) {
	    printf("\n    Not configured");
	    continue;
	}
	for (range = (addrsumtype *) pdb->ip_address_sumq.qhead; range; 
	     range = range->next) {
	    if (automore_quit())
		break;
	    mem_lock(range);
	    printf("\n%i/%i Metric %d, Type %d, Tag %d", 
		   range->network, range->mask,
		   range->metric, range->metric_type,
		   range->tag);
	    free(range);
	}
	free(pdb);
    }
    automore_disable();
}

/* add_to_array
 * add one SPF run time to keep track of the last ten SPF run
 */
void add_to_array (spfruntimetype time)
{   int i;

    if (run_spf_last >= run_spf_first) {
 	if (run_spf_last == SPF_TIME_MAX-1) {
 	    run_spf_last = 0;
 	    run_spf_first = 1;
 	} else {
	    if ( run_spf_last == 0 && run_spf_first == 0 )
		run_spf_first++;
	    run_spf_last++;
	} 
    } else {
 	/* case of of run_spf_last is < run_spf_first */
 	run_spf_last++;
 	if (run_spf_first == SPF_TIME_MAX-1)
 	    run_spf_first = 0;
 	else	
 	    run_spf_first++;
    }
    for (i=0; i<7; i++)
	spf_time[run_spf_last].mtime[i] = time.mtime[i];
    spf_time[run_spf_last].change = time.change;
    COPY_TIMESTAMP(time.timestamp, spf_time[run_spf_last].timestamp);
}

void write_circular (ushort selector, ulong *wtime, ushort step)
{
    int i;
    spfstattimetype *ptr;
    processtimetype *time;  /* only one instance */
    ushort first, last;

    switch (selector) {
      case ADD_EX_TIME:
	ptr = &spf_add_route_time;
	break;
      case PROCESS_SUM_TIME:
	ptr = &spf_sum_proc_time;
	break;
      case PROCESS_ASE_TIME:
	ptr = &spf_proc_time;
	break;
      default:
	return;
    }
    
    first = ptr->first;
    last  = ptr->last;
    time  = ptr->proc_time;
    if (last >= first) {
 	if (last == SPF_PROC_TIME_MAX-1) {
 	    last = 0;
 	    first = 1;
	} else {
	    if ( first == last && first == 0 )
		first++;
	    last++;
	} 
    } else {
 	/* case of of last is < first */
 	last++;
 	if (first == SPF_PROC_TIME_MAX-1)
 	    first = 0;
 	else
 	    first++;
    }
    for (i=0; i < step; i++, wtime++)
	time[last].ptime[i] = *wtime;
}

void change2lsatype (ushort change, char *type)
{
    if ( change & SPF_SCHD_RTR )
 	type += sprintf(type, "R, ");
    if ( change & SPF_SCHD_NET )
 	type += sprintf(type, "N, ");
    if ( change & SPF_SCHD_SNET )
 	type += sprintf(type, "SN, ");
    if ( change & SPF_SCHD_ASB )
 	type += sprintf(type, "SA, ");
    if ( change & SPF_SCHD_ASE )
 	type += sprintf(type, "X");
}

static void print_spf_array (void)
{
    int   current, ki;
    char  str[20];

    if ((run_spf_first == 0) && (run_spf_last == 0)) {
 	printf("None \n");
 	return;
    }
    printf("\n\n  SPF calculation time\n");
    printf("Delta T   "
	   "Intra\tD-Intra\tSumm\tD-Summ\tExt\tD-Ext\tTotal\tReason\n");

    current = run_spf_first-1;
    do {
	current++;
	if (current == SPF_TIME_MAX)
	    current = 0;
	print_ticks_in_dhms(ELAPSED_TIME(spf_time[current].timestamp));
	printf("   ");
	for (ki=0; ki<7; ki++)
	    printf("%d\t",spf_time[current].mtime[ki]);
	change2lsatype(spf_time[current].change, str);
	printf("%s\n", str);
    } while (current != run_spf_last);
    printf("\n");
}

 
static void print_avg_process_ase (void)
{
    int i,j;
    processtimetype *time, sum;  /* only one instance */

    time = spf_proc_time.proc_time;
 
    for (j=0; j<5; j++)
	sum.ptime[j] = 0;

    for (i=0; i < SPF_PROC_TIME_MAX; i++) 
	for (j=0; j<5; j++)
	    sum.ptime[j] += time[i].ptime[j];

    printf("\n  Avg. and Accumulated time of the last 250 process_ase()\n");
    printf("\n                      Avg.\tAccumulated");
    printf("\n    ASBR-lookup       %d,\t%d\n", 
		  sum.ptime[0] / SPF_PROC_TIME_MAX, sum.ptime[0]);
    printf("\n    Forw-Addr-lookup  %d,\t%d\n", 
		  sum.ptime[1] / SPF_PROC_TIME_MAX, sum.ptime[1]);
    printf("\n    compare metric    %d,\t%d\n", 
		  sum.ptime[2] / SPF_PROC_TIME_MAX, sum.ptime[2]);
    printf("\n    add_ex-route      %d,\t%d\n", 
		  sum.ptime[3] / SPF_PROC_TIME_MAX, sum.ptime[3]);
    printf("\n    route_delete      %d,\t%d\n", 
		  sum.ptime[4] / SPF_PROC_TIME_MAX, sum.ptime[4]);

    printf("\n");
}

static void print_avg_process_sum (void)
{
    int i,j;
    processtimetype *time, sum;  /* only one instance */

    time = spf_sum_proc_time.proc_time;
    for (j=0; j<5; j++)
	sum.ptime[j] = 0;

    for (i=0; i < SPF_PROC_TIME_MAX; i++) 
	for (j=0; j<4; j++)
	    sum.ptime[j] += time[i].ptime[j];

    printf("\n  Avg. and Accumulated time of the last 250 summary LSA process\n");
    printf("\n                                           Avg.\tAccumulated");
    printf("\n    ABR-lookup                             %d,\t%d\n", sum.ptime[0] / SPF_PROC_TIME_MAX, sum.ptime[0]);
    printf("\n    destination-lookup                     %d,\t%d\n", sum.ptime[1] / SPF_PROC_TIME_MAX, sum.ptime[1]);
    printf("\n    add summary route                      %d,\t%d\n", sum.ptime[2] / SPF_PROC_TIME_MAX, sum.ptime[2]);
    printf("\n    route_delete & build_inter_route_all   %d,\t%d\n", sum.ptime[3] / SPF_PROC_TIME_MAX, sum.ptime[3]);
    
    printf("\n");
}

static void print_avg_add_ex_route (void)
{
    int i,j;
    processtimetype *time, sum;  /* only one instance */

    time = spf_add_route_time.proc_time;

    for (j=0; j<5; j++)
	sum.ptime[j] = 0;

    for (i=0; i < SPF_PROC_TIME_MAX; i++) 
	for (j=0; j<3; j++)
	    sum.ptime[j] += time[i].ptime[j];

    printf("\n  Avg. and Accumulated time of the last 250 add_ex_route\n");
    printf("\n                                 Avg.\tAccumulated");
    printf("\n    ex_delete_route_list         %d,\t%d\n", sum.ptime[0] / SPF_PROC_TIME_MAX, sum.ptime[0]);
    printf("\n    network_update               %d,\t%d\n", sum.ptime[1] / SPF_PROC_TIME_MAX, sum.ptime[1]);
    printf("\n    ex_insert_route_list         %d,\t%d\n", sum.ptime[2] / SPF_PROC_TIME_MAX, sum.ptime[2]);
    printf("\n");
} 

static void print_ospf_spf (void)
{   
    areatype *area;
    pdbtype  *pdb;

    for (pdb = ospf_pdbs_ptr; pdb; pdb = pdb->ospf_next) {
 	for (area = pdb->ospf_area_list; area; area = area->ar_next) 
	    printf("\n  Area %s: SPF algorithm executed %d times", 
		   area->ar_id_str, area->ar_ospfSpfRuns);
	print_spf_array();
	print_avg_process_ase();
	print_avg_add_ex_route();
	print_avg_process_sum();
    }
}

static void print_delete_list_db(lsdbtype *db)
{
    pathtype *path, *next_path = NULL;
    
    printf("\n\n      Dest: %i, Type: %d, Metric: %d, ADV RTR: %i", 
	   db->db_dest, LSA_TYPE(db), db->db_dist, ADV_RTR(db));
    printf("\n      Path:");
    
    for ( path = db->db_path_list; path; path = next_path )  {
	mem_lock(path);
	printf("\n        gateway %i, interface %s", path->pb_gateway, 
	       path->pb_firsthop_idb->namestring);
	next_path = path->pb_next;
	free(path);
    }	
}

static boolean show_delete_list(lsdbtype *db)
{
    lsdbtype *ptr1;
    lsatype *ptr2;

    for (; db; db = db->db_rnext) { 
	if (automore_quit())
	   goto punt;
	if (!validmem(db))
	   goto punt;
 	if (db->db_lsa_ptr == NULL)
 	    continue;
 	ptr1 = db;
 	ptr2 = db->db_lsa_ptr;
 	mem_lock(ptr1);
 	mem_lock(ptr2);
 
 	print_delete_list_db(db);
 
 	free(ptr1);
 	free(ptr2);
    }
    return (TRUE);

  punt:
    automore_disable();
    return (FALSE);
}

static void show_delete (void)
{
    pdbtype *pdb;
    areatype *area;
     
    for (pdb = ospf_pdbs_ptr; pdb ; pdb = pdb->ospf_next) {
 	printf("\n  AS System  %d", pdb->asystem);
 
	for( area = pdb->ospf_area_list; area;area = area->ar_next) {
 	    if (area->ar_id == BACKBONE_AREA_ID)
 		printf("\n\n    Area BACKBONE(%s)\n",area->ar_id_str);
 	    else
 		printf("\n\n    Area %s\n",area->ar_id_str);
 	    printf("\n    ROUTER and NETWORK LSDB delete list");
 	    if (!show_delete_list( area->ar_delete_list))
	       return;
 	    printf("\n\n    SUMMARY NET and ASBR LSDB delete list");
	    if (!show_delete_list( area->ar_sum_delete_list))
	       return;
	    printf("\n\n    TYPE-7 EXTERNAL LSDB delete list");
	    if (!show_delete_list(area->ar_ex_delete_list))
		return;
 	}
 	printf("\n\n    EXTERNAL LSDB delete list");
 	if (!show_delete_list(pdb->ospf_ex_delete))
	   return;
    }
}

/*
 * get_area_ptr
 * given a pdb and area id,
 * gets pointer to the area
 * structure.
 */

areatype * get_area_ptr (pdbtype *pdb, ulong area_id)
{
    areatype *area;

    for (area = pdb->ospf_area_list; area ; area = area->ar_next) {
	if (area->ar_id == area_id)
	    return(area);
    }
    return(area);
} /* end of get_area_ptr */

/*
 * find_area_range
 * checks if a given address
 * falls into an area range.
 */

addr_range *find_area_range (areatype *area, ipaddrtype dest)
{
    addr_range *range;

    for (range = area->ar_range_list; range ; range = range->net_next)
	if (range->net_address == (dest & range->net_mask))
	    return(range);
    return(NULL);
} /* end of find_area_range */

/*
 * lsa_checksum
 * calculate the checksum
 * for a given LSA.
 */

void lsa_checksum (lsdbtype *db)
{
    ushort	age;

    age = db->db_lsa_ptr->lsh_age;
    db->db_lsa_ptr->lsh_age = 0;
    compute_iso_checksum((uchar *)db->db_lsa_ptr,
			 db->db_lsa_ptr->lsh_length,
			 (uchar *)&(db->db_lsa_ptr->lsh_chksum));
    db->db_lsa_ptr->lsh_age = age;
} /* end of lsa_checksum */

/*
 * print_ip_address_name
 */

static int print_ip_address_name (ipaddrtype address, boolean format)
{
    nametype *cache;
    int i;

    if (address == 0) {
        if (format)
	    printf("%15s", "N/A");
	else
	    printf("N/A");
	return(0);
    }
    if (!ospf_name_lookup) {
	if (format)
	    i = printf("%15i",address);
	else
	    i = printf("%i",address);
	return(i);
    }
    cache = reg_invoke_ip_address_lookup(address);
    if (cache == NULL) {
	if (format)
	    i = printf("%15i",address);
	else
	    i = printf("%i",address);
    } else {
	lowercase(cache->name);
	if (format)
	    i = printf("%15s",cache->name);
	else
	    i = printf("%s",cache->name);
    }
    return(i);

} /* end of print_ip_address_name */

/*
 * ip_address_same_host
 * checks if the two given Ip 
 * addresses are on the same host.
 */

static boolean ip_address_same_host (ipaddrtype address1, ipaddrtype address2)
{
    nametype *cache1;
    nametype *cache2;

    if (address1 == address2)
	return(TRUE);
    if (!ospf_name_lookup)
	return(FALSE);
    cache1 = reg_invoke_ip_address_lookup(address1);
    if (cache1 == NULL)
	return(FALSE);
    cache2 = reg_invoke_ip_address_lookup(address2);
    if (cache2 == NULL)
	return(FALSE);
    return(!strcasecmp(cache1->name, cache2->name));
} /* end of print_ip_address_name */

/*
 * ospf_if_state_string
 * Return pointer to a string 
 * describing the interface state
 */
static char *ospf_if_st_descr[] =
{
    "DOWN",                     /* IFS_DOWN             */
    "LOOPBACK",                 /* IFS_LOOPBACK         */
    "WAITING",                  /* IFS_WAITING          */
    "POINT_TO_MULTIPOINT",      /* IFS_POINT_TO_M_POINT */
    "POINT_TO_POINT",           /* IFS_POINT_TO_POINT   */
    "DR",                       /* IFS_DR               */
    "BDR",                      /* IFS_BACKUP           */
    "DROTHER"                   /* IFS_OTHER            */
};
static char *ospf_if_state_string (int state)
{
    if ((state >= 0) && (state < IF_STATES))
	return ospf_if_st_descr[state];
    else
	return "????";
} /* ospf_if_state_string */

/*
 * ospf_if_type_string
 * returns an ascii string for
 * an interface type.
 */
static char *ospf_if_type_descr[] =
{
    "",                         /* n/a                  */
    "BROADCAST",                /* IF_BROADCAST         */
    "NON_BROADCAST",            /* IF_NONBROADCAST      */
    "POINT_TO_POINT",           /* IF_POINT_TO_POINT    */
    "POINT_TO_MULTIPOINT",      /* IF_POINT_TO_M_POINT  */
    "VIRTUAL_LINK",             /* IF_VIRTUAL_LINK      */
    "LOOPBACK",                 /* IF_LOOPBACK          */
};
static char *ospf_if_type_string (int type)
{
    if ((type > 0) && (type <= IF_LOOPBACK))
	return ospf_if_type_descr[type];
    else
	return "UNKNOWN";
} /* end of ospf_if_type_string */

/*
 * ospf_nbr_state_string
 * returns an ascii string for
 * a neighbor state.
 */
static char *ospf_nbr_st_descr[] = 
{
    "DOWN",                     /* NBR_DOWN             */
    "ATTEMPT",                  /* NBR_ATTEMPT          */
    "INIT",                     /* NBR_INIT             */
    "2WAY",                     /* NBR_2WAY             */
    "EXSTART",                  /* NBR_EXSTART          */
    "EXCHANGE",                 /* NBR_EXCHANGE         */
    "LOADING",                  /* NBR_LOADING          */
    "FULL",                     /* NBR_FULL             */
    "SCVIRTUAL"                 /* NBR_SCVIRTUAL        */
};
char *ospf_nbr_state_string (int state)
{
    if ((state >= 0) && (state < NBR_STATES))
	return ospf_nbr_st_descr[state];
    else
	return "UNKNOWN";
} /* ospf_nbr_state_string */

/*
 * ospf_link_type_string
 * returns an ascii string for
 * a router link type
 */
static char* ospf_link_type_descr[] = 
{
    "",                                 /* n/a                  */
    "another Router (point-to-point)",  /* LINK_IF_RTR          */
    "a Transit Network",                /* LINK_IF_TRANS_NET    */
    "a Stub Network",                   /* LINK_IF_STUB_NET     */
    "a Virtual Link"                    /* LINK_IF_VIRTUAL_LINK */
};
static char *ospf_link_type_string (int type)
{
    if ((type > 0) && (type <= LINK_IF_VIRTUAL_LINK))
	return ospf_link_type_descr[type];
    else
	return "UNKNOWN";
} /* end of ospf_link_type_string */

static const uchar lsa_summary_header[] = 
	"\nLink ID         ADV Router      Age         Seq#       Checksum";

static inline void display_lsa_summary_common (lsdbtype *db, lsatype *lsa, 
					       ulong time)
{
    printf("\n%15i ", lsa->lsh_id);
    print_ip_address_name(lsa->lsh_adv_rtr, TRUE);
    printf(" %6d", time);
    if (db && db->db_do_not_age)
	printf("(DNA) ");
    else
	printf("      ");
    printf("%#10x %#08x", lsa->lsh_seq, lsa->lsh_chksum);
}

/*
 * display_lsa_common
 */
static inline void display_lsa_common (lsdbtype *db, lsatype *lsa, 
				       ulong time, boolean internal)
{
    if (db->db_freeme) {
	printf("\n  Delete flag is set for this LSA");
	if (internal) {
	    printf("\n  Last delete attempt : ");
	    if (TIMER_RUNNING(db->db_free_time)) {
		printf(" %TE ago", db->db_free_time);
		printf("\n  Failure reason :");
		if (db->db_nf_ack_cnt)
		    printf("\n    Ack count not zero");
		if (db->db_nf_dbase_lock)
		    printf("\n    Database locked");
		if (db->db_nf_retrans)
		    printf("\n    Wait for retransmission");
		if (db->db_nf_nbr_ex_load)
		    printf("\n    Neighbor in exchange state");
		if (db->db_nf_rt_table)
		    printf("\n    Contained in routing table");
		if (db->db_nf_flood_dly)
		    printf("\n    Wait for delayed flood");
		if (db->db_nf_in_maxageq)
		    printf("\n    Contained in Maxage queue");
		if (db->db_nf_freeme)
		    printf("\n    Delete flag not set");
		if (db->db_nf_partial)
		    printf("\n    Contained in partial SPF queue");
	    } else
	        printf("Never");
	}
    }
    if (db->db_rt_table) {
	printf("\n  Routing Bit Set on this LSA");
    }
    if (((LSA_TYPE(db) == LSA_TYPE_RTR) || 
	 (LSA_TYPE(db) == LSA_TYPE_SUM_ASB)) && !db->db_rtr_reachable) {
	printf("\n  Adv Router is not-reachable");
    }
    if (time >= MAXAGE)
	printf("\n  LS age: MAXAGE(%d)", time);
    else
	printf("\n  LS age: %d", time);
    if (db->db_do_not_age)
	printf(" (DoNotAge)");
    printf("\n  Options: (");
    if (GET_T_BIT(lsa->lsh_options))
	printf("TOS-capable");
    else
	printf("No TOS-capability");
    if (LSA_TYPE(db) == LSA_TYPE_7_ASE) {
	if (!GET_N_P_BIT(lsa->lsh_options))
	    printf(", No");
	else
	    printf(",");
	printf(" Type 7/5 translation");
    }
    if (!GET_DC_BIT(lsa->lsh_options))
	printf(", No");
    else
	printf(",");
    printf(" DC");
    printf(")");
}


/*
 * display_rtr_lsa
 * Display Router LSA
 */

static void display_rtr_lsa (
    areatype  *area,
    lsatype   *lsa,
    lsdbtype  *db,
    boolean   sum_flag,
    boolean   flag,
    boolean   internal)
{
    ulong	time;
    rtr_lsatype	*rtr_lsa;
    link_info	*link;
    ushort	count, tos_count, tos_index;
    
    if (lsa == NULL)
	return;
    if (db && !db->db_do_not_age)
	time = lsa->lsh_age + DB_ELAPSE(db);
    else
	time = lsa->lsh_age;
    if (flag) {
	printf("\n\n		Router Link States (Area %s)\n",
	       area->ar_id_str);
	if (sum_flag) {
	    printf(lsa_summary_header);
	    printf(" Link count");
	}
    }
    rtr_lsa = (rtr_lsatype *)lsa;
    if (sum_flag) {
	display_lsa_summary_common(db, lsa, time);
	printf(" %d", rtr_lsa->la_lnk_cnt);
	return;
    }
    display_lsa_common(db, lsa, time, internal);
    printf("\n  LS Type: Router Links");
    printf("\n  Link State ID: %i", lsa->lsh_id);
    printf("\n  Advertising Router: ");
    print_ip_address_name(lsa->lsh_adv_rtr, FALSE);
    printf("\n  LS Seq Number: %x", lsa->lsh_seq);
    printf("\n  Checksum: %#x", lsa->lsh_chksum);
    printf("\n  Length: %d", lsa->lsh_length);
    if (AREA_BORDER_B_BIT & rtr_lsa->la_E_B) {
	printf("\n  Area Border Router");
    }
    if (AS_BORDER_E_BIT & rtr_lsa->la_E_B) {
	printf("\n  AS Boundary Router");
    }

    if (rtr_lsa->la_zero != 0) {
	printf("\n  Invalid value in zero field");
    }
    printf("\n   Number of Links: %d", rtr_lsa->la_lnk_cnt);
    count = rtr_lsa->la_lnk_cnt;
    link = &(rtr_lsa->la_link);
    printf("\n");
    while (count != 0) {
	printf("\n    Link connected to: %s", 
	       ospf_link_type_string(link->lnk_con_type));
	if ((link->lnk_con_type == LINK_IF_RTR) 
	    || (link->lnk_con_type == LINK_IF_VIRTUAL_LINK)) {
	    printf("\n     (Link ID) Neighboring Router ID: %i", link->lnk_id);
	} else { 
	    if (link->lnk_con_type == LINK_IF_TRANS_NET) {
		printf("\n     (Link ID) Designated Router address: %i",
		       link->lnk_id);
	    } else {
		printf("\n     (Link ID) Network/subnet number: %i",
		       link->lnk_id);
	    }
	}
	if (link->lnk_con_type == LINK_IF_STUB_NET) {
	    printf("\n     (Link Data) Network Mask: %i", link->lnk_data);
	} else {
	    printf("\n     (Link Data) Router Interface address: %i",
		   link->lnk_data);
	}
	printf("\n      Number of TOS metrics: %d", link->lnk_tos_count);
	printf("\n       TOS 0 Metrics: %d",link->lnk_tos0_metric);
	tos_count = link->lnk_tos_count;
	tos_index = 0;
	while (tos_count) {
	    printf("\n       TOS %d Metrics: %d",
		   link->lnk_tos_metric[tos_index].tos >> 8,
		   link->lnk_tos_metric[tos_index].metric);
	    tos_count--;
	    tos_index++;
	    /*
	     * Include the space for TOS metrics
	     */
	    link = (link_info *) ((uchar *) link + LINK_METRIC_SIZE);
	}
	count--;
	printf("\n");
	link++;
    }
    printf("\n");
} /* end of display_rtr_lsa */

/*
 * display_net_lsa
 * Display Net LSA
 */

static void display_net_lsa (
    areatype *area,
    lsatype  *lsa,
    lsdbtype *db,
    boolean  sum_flag,
    boolean  flag,
    boolean  internal)
{
    ulong time;
    net_lsatype	*net_lsa;
    ulong *att_router;
    ushort count;
    
    if (lsa == NULL)
	return;
    if (db && !db->db_do_not_age)
	time = lsa->lsh_age + DB_ELAPSE(db);
    else
	time = lsa->lsh_age;
    if (flag) {
	printf("\n\n		Net Link States (Area %s)\n",
	       area->ar_id_str);
	if (sum_flag) {
	    printf(lsa_summary_header);
	}
    }
    net_lsa = (net_lsatype *)lsa;
    if (sum_flag) {
	display_lsa_summary_common(db, lsa, time);
	return;
    }
    display_lsa_common(db, lsa, time, internal);
    printf("\n  LS Type: Network Links");
    printf("\n  Link State ID: %i (address of Designated Router)",lsa->lsh_id);
    printf("\n  Advertising Router: ");
    print_ip_address_name(lsa->lsh_adv_rtr, FALSE);
    printf("\n  LS Seq Number: %x", lsa->lsh_seq);
    printf("\n  Checksum: %#x", lsa->lsh_chksum);
    printf("\n  Length: %d", lsa->lsh_length);
    printf("\n  Network Mask:%#m", net_lsa->la_mask);
    att_router = &(net_lsa->la_nbr);
    count = (lsa->lsh_length - NET_LSA_HDR_SIZE);
    while (count > 0) {
	printf("\n	Attached Router: %i", *att_router);
	count -= 4;
	att_router++;
    }
    printf("\n");
} /* end of display_net_lsa */

/*
 * display_sum_lsa
 * Display a summary LSA
 */

static void display_sum_lsa (
    areatype *area,
    lsatype  *lsa,
    lsdbtype *db,
    boolean  sum_flag,
    boolean  flag,
    boolean  internal)
{
    ulong time;
    sum_lsatype	*sum_lsa;
    ulong *tos_metric;
    ushort count;
    
    if (lsa == NULL)
	return;
    if (db && !db->db_do_not_age)
	time = lsa->lsh_age + DB_ELAPSE(db);
    else
	time = lsa->lsh_age;
    if (flag) {
	if (lsa->lsh_type == LSA_TYPE_SUM_NET)
	   printf("\n\n		Summary Net Link States (Area %s)\n",
		  area->ar_id_str);
	else
	   printf("\n\n		Summary ASB Link States (Area %s)\n",
		  area->ar_id_str);
	if (sum_flag) {
	    printf(lsa_summary_header);
	}
    }
    sum_lsa = (sum_lsatype *)lsa;
    if (sum_flag) {
	display_lsa_summary_common(db, lsa, time);
	return;
    }
    display_lsa_common(db, lsa, time, internal);
    if (lsa->lsh_type == LSA_TYPE_SUM_NET) {
	printf("\n  LS Type: Summary Links(Network)");
	printf("\n  Link State ID: %i (summary Network Number)",lsa->lsh_id);
    } else {
	printf("\n  LS Type: Summary Links(AS Boundary Router)");
	printf("\n  Link State ID: %i (AS Boundary Router address)",
	       lsa->lsh_id);
    }

    printf("\n  Advertising Router: ");
    print_ip_address_name(lsa->lsh_adv_rtr, FALSE);
    printf("\n  LS Seq Number: %x", lsa->lsh_seq);
    printf("\n  Checksum: %#x", lsa->lsh_chksum);
    printf("\n  Length: %d", lsa->lsh_length);

    printf("\n  Network Mask:%#m", sum_lsa->la_mask);
    tos_metric = &(sum_lsa->la_tos_metric);
    count = (lsa->lsh_length - SUM_LSA_HDR_SIZE);
    while (count > 0) {
	printf("\n	TOS: %d ", SUM_TOS(*tos_metric));
	printf("	Metric: %d ", SUM_METRIC(*tos_metric));
	count -= 4;
	tos_metric++;
    }
    
    printf("\n");
} /* end of display_sum_lsa */

/*
 * display_as_ex_lsa
 * Display an Type-5 and Type-7 external LSA
 */

static void display_as_ex_lsa (
    areatype	*area,
    lsatype	*lsa,
    lsdbtype	*db,
    boolean	sum_flag,
    boolean	flag,
    boolean     internal)
{
    ulong	time;
    ex_lsatype	*ex_lsa;
    as_metric	*metric;
    ushort	count;
    
    if (lsa == NULL)
	return;
    if (db && !db->db_do_not_age)
	time = lsa->lsh_age + DB_ELAPSE(db);
    else
	time = lsa->lsh_age;
    if (flag) {
	if (lsa->lsh_type == LSA_TYPE_ASE)
	    printf("\n\n		Type-5 AS External Link States\n");
	else
	    printf("\n\n		Type-7 AS External Link States "
		   "(Area %s)\n", area->ar_id_str);
	if (sum_flag) {
	    printf(lsa_summary_header);
	    printf(" Tag");
	}
    }
    ex_lsa = (ex_lsatype *)lsa;
    metric = &(ex_lsa->la_metric);
    if (sum_flag) {
	display_lsa_summary_common(db, lsa, time);
	printf(" %u", metric->as_ex_tag);
	return;
    }
    display_lsa_common(db, lsa, time, internal);
    printf("\n  LS Type: AS External Link");
    printf("\n  Link State ID: %i (External Network Number )",lsa->lsh_id);
    printf("\n  Advertising Router: ");
    print_ip_address_name(lsa->lsh_adv_rtr, FALSE);
    printf("\n  LS Seq Number: %x", lsa->lsh_seq);
    printf("\n  Checksum: %#x", lsa->lsh_chksum);
    printf("\n  Length: %d", lsa->lsh_length);
    printf("\n  Network Mask:%#m", ex_lsa->la_mask);
    count = (lsa->lsh_length - ASE_LSA_HDR_SIZE);
    while (count > 0) {
	if (ASE_METRIC_TYPE(metric->as_e_tos_metric))
	    printf("\n	Metric Type: 2 (Larger than any link state path)");
	else {
	    printf("\n	Metric Type: 1 (Comparable");
	    printf(" directly to link state metric)");
	}
	printf("\n	TOS: %d ", ASE_TOS(metric->as_e_tos_metric));
	printf("\n	Metric: %d ", ASE_METRIC(metric->as_e_tos_metric));

	printf("\n	Forward Address: %i", metric->as_forw_addr);
	printf("\n	External Route Tag: %d", metric->as_ex_tag);
	count -= AS_METRIC_SZ;
	metric++;
    }
    
    printf("\n");
} /* end of display_as_ex_lsa */

static boolean print_one_type (
    areatype 	*area,
    ushort 	type,
    boolean 	flag, 
    boolean 	first_flag,
    ipaddrtype 	target,
    boolean	address_passed,
    boolean	internal)
{
    lsdbtype    *db;
    lsatype     *lsa;
    boolean     valid_db;

    for (db = db_get_first_db(area->ar_lsdbase[type]); db ;
	 db = db_get_next_db(db)) {
	if (automore_quit())
	    goto punt;
	if (!validmem(db))
	    goto punt;
	if (db->db_lsa_ptr == NULL)
	    continue;
	if (address_passed)
	    if (db->db_lsa_ptr->lsh_id != target)
		continue;
	lsa = db->db_lsa_ptr;
	mem_lock(db);
	mem_lock(lsa);
	/* if we are showing detail, check after every
	 * LSA except the first one 
	 */
	if ((flag == FALSE) && (first_flag == FALSE)) {
	    automore_conditional(0);
	}
	switch (type) {
	  case LSA_TYPE_RTR:
	    display_rtr_lsa(area, db->db_lsa_ptr, db,
			    flag, first_flag, internal);
	    break;
	  case LSA_TYPE_NET:
	    display_net_lsa(area, db->db_lsa_ptr, db,
	    		flag, first_flag, internal);
	    break;
	  case LSA_TYPE_SUM_NET:
	    display_sum_lsa(area, db->db_lsa_ptr, db,
			    flag, first_flag, internal);
	    break;
	  case LSA_TYPE_SUM_ASB:
	    display_sum_lsa(area, db->db_lsa_ptr, db,
			    flag, first_flag, internal);
	    break;
	  case LSA_TYPE_ASE:
	  case LSA_TYPE_7_ASE:
	    display_as_ex_lsa(area, db->db_lsa_ptr, db,
			      flag, first_flag, internal);
	    break;
	}
	if (first_flag)
	    first_flag = FALSE;
        valid_db = db_valid_db(db);
	free(db);
	free(lsa);
        if (!valid_db)
	    goto punt;
    }
    return (TRUE);

  punt:
    automore_disable();
    return (FALSE);
} /* end of print_one_type */

/*
 * display_links
 * display LSA in an
 * area.
 */

static boolean display_links (
    areatype 	*area,
    ushort	type,
    boolean	flag,		/* summary (TRUE) or detail (FALSE) */
    ipaddrtype  target,
    boolean address_passed,
    boolean internal)
{
    boolean	first_flag;

    if (area == NULL) {
	return(TRUE);
    }
    if (type == 0) {
	for (type = LSA_TYPE_RTR; type <=  LSA_TYPE_7_ASE; type++) {
	    if ((type == LSA_TYPE_ASE) || (type == LSA_TYPE_IGNORE))
		continue;
	    first_flag = TRUE;
	    if (!print_one_type(area, type, flag, first_flag, 0, FALSE, 
				internal))
		return(FALSE);
	}
    } else {
	first_flag = TRUE;
	if (!print_one_type(area, type, flag, first_flag, target, 
		            address_passed, internal))
	    return(FALSE);
    }
    return(TRUE);
} /* end of display_links */

/*
 * display_area_database
 * display database elements in an
 * area.
 */

static boolean display_area_database (
    pdbtype	*pdb,
    areatype 	*area,
    int         type,
    ipaddrtype  target,
    boolean address_passed,
    boolean internal)
{
    ospf_pdb_info *ospf_pdb = pdb->ospf_pdb;

    if (area == NULL) {
	for (area = pdb->ospf_area_list; area; area = area->ar_next) {
	    if (type == 0) {
		if (!display_links(area, 0, TRUE, 0, FALSE, internal))
		    return(FALSE);
	    } else {
		if (type != LSA_TYPE_ASE) {
		    if (!display_links(area, type, FALSE, target, 
		                       address_passed, internal))
			return(FALSE);
		}
	    }
	}
    } else {
	if (type == 0) {
	    if (!display_links(area, 0, TRUE, 0, FALSE, internal))
		return(FALSE);
	} else {
	    if (type != LSA_TYPE_ASE) {
		if (!display_links(area, type, FALSE, target, address_passed,
				   internal))
		    return(FALSE);
	    }
	}
    }
    if (type  == 0) {
	if (!display_links(ospf_pdb->ospf_dummy_area, LSA_TYPE_ASE, TRUE, 
			   0, FALSE, internal))
	    return(FALSE);
    } else {
	if (type == LSA_TYPE_ASE) {
	    if (!display_links(ospf_pdb->ospf_dummy_area, LSA_TYPE_ASE, FALSE, 
			       target, address_passed, internal))
		return(FALSE);
	}
    }

    return(TRUE);
}/* end of display_area_database */

/*
 * show_database
 * display database elements.
 */

static void show_database (
    pdbtype *pdb,
    areatype *area,
    int flag,
    ipaddrtype target,
    boolean address_passed,
    boolean internal)
{
    if (pdb == NULL) {
        for (pdb = ospf_pdbs_ptr; pdb; pdb = pdb->ospf_next) {
	    printf("\n\n       OSPF Router with ID (%i) "
		   "(Process ID %d)\n",
                   pdb->ospf_rtr_id, pdb->asystem);
            if (!display_area_database(pdb, area, flag, target, 
				       address_passed, internal))
		return;
	}
    } else {
        printf("\n\n       OSPF Router with ID (%i) (Process ID %d)\n",
               pdb->ospf_rtr_id, pdb->asystem);
        display_area_database(pdb, area, flag, target, address_passed, 
			      internal);
    }
} /* end of show_database */

/*
 * count_one_type
 */
static boolean count_one_type(areatype *area, ushort type, int *count, 
		       int *delete_count, int *maxage_count) 
{
    lsdbtype *db;

    for (db = db_get_first_db(area->ar_lsdbase[type]); db;
	 db = db_get_next_db(db)) {
        if (!validmem(db))
            return (FALSE);
        if (!db->db_lsa_ptr)
            continue;
	if (db->db_freeme)
	    (*delete_count)++;
	if (ADV_AGE(db) >= MAXAGE)
	    (*maxage_count)++;
	(*count)++;
    }
    return (TRUE);
}

/*
 * show_one_database_summary
 */
static boolean show_one_database_summary (pdbtype *pdb, areatype *area) {
    areatype *ar;
    ushort type;
    int count[LSA_TYPE_7_ASE+1], type_total[LSA_TYPE_7_ASE+1];
    int total, area_total, delete_count, maxage_count, external_count;

    total = external_count = 0;
    for (type = LSA_TYPE_RTR; type <= LSA_TYPE_7_ASE; type++)
        type_total[type] = 0;
    printf("\n\n             OSPF Router with ID (%i) (Process ID %d)\n",
	   pdb->ospf_rtr_id, pdb->asystem);
    printf("\n%16s%8s%8s%8s%8s%8s%9s%7s%8s", "Area ID", "Router", "Network",
	   "S-Net", "S-ASBR", "Type-7", "Subtotal", "Delete", "Maxage");
    for (ar = pdb->ospf_area_list; ar; ar = ar->ar_next) {
        area_total = delete_count = maxage_count = 0;
        if (area && (area != ar))
	    continue;
        for (type = LSA_TYPE_RTR; type <= LSA_TYPE_7_ASE; type++) {
	    if ((type == LSA_TYPE_ASE) || (type == LSA_TYPE_IGNORE))
		continue;
	    count[type] = 0;
	    if (!count_one_type(ar, type, &(count[type]), &delete_count, 
				&maxage_count))
	        goto punt;
	    type_total[type] += count[type];
	    area_total += count[type];
        }
	printf("\n%16s", ar->ar_id_str);
	for (type = LSA_TYPE_RTR; type <= LSA_TYPE_7_ASE; type++) {
	    if ((type == LSA_TYPE_ASE) || (type == LSA_TYPE_IGNORE))
		continue;
	    if ((type == LSA_TYPE_7_ASE) && !ar->ar_nssa)
		printf("%8s", "N/A");
	    else 
		printf("%8d", count[type]);
	}
	printf("%9d%7d%8d", area_total, delete_count, maxage_count);
    }
    if (!area) {
        external_count = delete_count = maxage_count = 0;
	if (!count_one_type(pdb->ospf_pdb->ospf_dummy_area, LSA_TYPE_ASE, 
			    &external_count, &delete_count, 
			    &maxage_count))
	    goto punt;
        printf("\n%56s%9d%7d%8d", "AS External", external_count, 
	       delete_count, maxage_count); 
        printf("\n%16s", "Total");
        for (type = LSA_TYPE_RTR; type <= LSA_TYPE_7_ASE; type++) {
	    if ((type == LSA_TYPE_ASE) || (type == LSA_TYPE_IGNORE))
		continue;
            printf("%8d", type_total[type]);
	    total += type_total[type];
        }
	total += external_count;
        printf("%9d", total);
    }
    return (TRUE);

  punt:
    return (FALSE);
} /* end of show_one_database_summary */

/*
 * show_database_summary
 */
static void show_database_summary (pdbtype *pdb, areatype *area) {
    if (pdb == NULL) {
        for (pdb = ospf_pdbs_ptr; pdb; pdb = pdb->ospf_next) {
            if (!show_one_database_summary(pdb, area))
		return;
	}
    } else {
        show_one_database_summary(pdb, area);
    }
}

static boolean print_nbr_for_idb(idbtype *idb, ipaddrtype target, boolean header,
			  boolean detail)
{
    nbrtype *nbr;
    int i, j;
    long due_time;
    ospf_idb_info *ospf_info = idb->ospf_info;

    for (j = 0; (nbr = queuepick(&ospf_info->oi_nbr, j)); j++) {
	if (automore_quit())
	    goto punt;
	if (!validmem(nbr)) {
	    printf("\n A Neighbor was deleted, Please try again\n");
	    goto punt;
	}
	mem_lock(nbr);
	if ((target == 0) && (!detail)) {
	    if (header) {
		printf("\n\nNeighbor ID     Pri   State           "
		       "Dead Time   Address         Interface");
		header = FALSE;
	    }
	    /*
	     * print ID and priority
	     */
	    printf("\n");
	    print_ip_address_name(nbr->nbr_id, TRUE);
	    printf(" %-3d",nbr->nbr_pri);
	    /*
	     * print state
	     */
	    i = printf("   %s", ospf_nbr_state_string(nbr->nbr_state));
	    if (nbr == ospf_info->dr)
		i += printf("/DR");
	    else
		if (nbr == ospf_info->bdr)
		    i += printf("/BDR");
		else
		    if (ospf_info->if_type <= IF_NONBROADCAST)
			i += printf("/DROTHER");
		    else
			i += printf("/  -  ");
	    for (; i < 19; i++)
		printf(" ");
	    if (mgd_timer_running(&nbr->nbr_dead_timer)) {
		due_time = mgd_timer_left_sleeping(&nbr->nbr_dead_timer);
		print_ticks_in_dhms(due_time);
	    } else {
		printf("   -    ");
	    }
	    printf("    %15i %-s",nbr->nbr_ipaddr, idb->namestring);
	    free(nbr);
	    continue;
	}
	if (target && !ip_address_same_host(target,nbr->nbr_id)) {
	    free(nbr);
	    continue;
	}
	/*
	 * print detail of neighboor
	 */
	printf("\n Neighbor ");
	print_ip_address_name(nbr->nbr_id, FALSE);
	printf(", interface address %i",nbr->nbr_ipaddr);
	printf("\n    In the area %s via interface %s ",
	       ospf_info->area_ptr->ar_id_str, idb->namestring);
	printf("\n    Neighbor priority is %d, State is %s",
	       nbr->nbr_pri, ospf_nbr_state_string(nbr->nbr_state));
	if (ospf_info->if_type == IF_NONBROADCAST)
	    printf("\n    Poll interval %d",nbr->nbr_poll_interval);

	printf("\n    Options %d  ",nbr->nbr_options);
	if (mgd_timer_running(&nbr->nbr_dead_timer)) {
	    printf("\n    Dead timer due in ");
	    due_time = mgd_timer_left_sleeping(&nbr->nbr_dead_timer);
	    print_ticks_in_dhms(due_time);
	}
	if (mgd_timer_running(&nbr->nbr_lsa_retrs_timer)) {
	    printf("\n    Link State retransmission due in ");
	    due_time = mgd_timer_left_sleeping(&nbr->nbr_lsa_retrs_timer);
	    print_ticks_in_dhms(due_time);
	    printf("\n         LSA in retransmission queue %u", 
		   nbr->nbr_retrans_queue.count);  
	}
	if (mgd_timer_running(&nbr->nbr_poll_timer)) {
	    printf("\n    Poll due in ");
	    due_time = mgd_timer_left_sleeping(&nbr->nbr_poll_timer);
	    print_ticks_in_dhms(due_time);
	}
	free(nbr);
    }
    return(TRUE);

  punt:
    automore_disable();
    return(FALSE);
} /* end of print_nbr_for_idb */

/*
 * display_ospf_nbr
 * Display neighbors with an interface
 */

static void display_ospf_nbr (idbtype *idb, ipaddrtype target, boolean detail)
{
    boolean header = TRUE;
    ospf_idb_info *ospf_info;

    if (idb) {
	ospf_info = idb->ospf_info;
	if ((ospf_info && ospf_info->enabled) && 
	    (ospf_info->if_state != IFS_DOWN))
	    (void) print_nbr_for_idb(idb, target, header, FALSE);
    } else {
	FOR_ALL_SWIDBS(idb) {
	    ospf_info = idb->ospf_info;
	    if ((ospf_info && ospf_info->enabled) && 
		(ospf_info->if_state != IFS_DOWN) &&
		ospf_info->oi_nbr) {
		if (!print_nbr_for_idb(idb, target, header, detail))
		    return;
		header = FALSE;
	    }
	}
    }
} /* end of display_ospf_nbr */


/*
 * display_ospf_message_digest
 * Display message digest information
 */
static void display_ospf_message_digest (ospf_idb_info *ospf_info)
{
    ospf_md_key *md_key;
    printf("\n  Message digest authentication enabled");
    if (ospf_info->youngest_md_key)
        printf("\n    Youngest key id is %u", 
	       ospf_info->youngest_md_key->id);
    else
        printf("\n      No key configured, using default key id 0");
    if (ospf_info->old_md_nbr_count) {
        printf("\n    Rollover in progress, "
	       "%d neighbor(s) using the old key(s):", 
	       ospf_info->old_md_nbr_count);
	md_key = (ospf_md_key *) (ospf_info ? ospf_info->md_key_q.head : NULL);
	while (md_key) {
	    if (md_key != ospf_info->youngest_md_key)
	        printf("\n      key id %u", md_key->id);
	    md_key = (ospf_md_key *) md_key->link.next;	    
	}
    }
}

/*
 * display_ospf_idb
 * Display link level statistics for one network interface
 */

static void display_ospf_idb (idbtype *idb)
{
    nbrtype *nbr, *next_nbr;
    pdbtype *pdb;
    areatype *area_ptr;
    long due_time;
    ospf_idb_info *ospf_info = idb->ospf_info;

    printf("\n%s is %s, line protocol is %s %s", idb->namestring,
	   print_idbstate(idb),
	   (interface_up(idb) ? "up" : "down"),
	   print_idbflags(idb->hwptr));
    if (!ospf_info || !ospf_info->enabled) {
	printf("\n   OSPF not enabled on this interface");
	return;
    }
    area_ptr = ospf_info->area_ptr;
    pdb = (pdbtype *) area_ptr->ar_pdb_ptr;
    printf("\n  Internet Address %i%m, Area %s ",
	   idb->ip_address, idb->ip_nets_mask, area_ptr->ar_id_str);
    printf("\n  Process ID %d, Router ID %i, Network Type %s, Cost: %d",
	   pdb->asystem, pdb->ospf_rtr_id,
	   ospf_if_type_string(ospf_info->if_type), 
	   ospf_info->if_cost[0]);
    if (ospf_info->oi_cfg_demand_circuit)
	printf("\n  Configured as demand circuit.");
    if (ospf_info->oi_demand_circuit) {
	printf("\n  Run as demand circuit.");
	printf("\n  DoNotAge LSA ");
	if (area_ptr->ar_dcbitless_lsa_count) {
	    printf("not allowed (Number of DCbitless LSA is %d).", 
		   area_ptr->ar_dcbitless_lsa_count);
	} else {
	    printf("allowed.");
	}
    }
    if (ospf_info->if_type == IF_LOOPBACK) {
	printf("\n  Loopback interface is treated as a stub Host");
	return;
    }
    printf("\n  Transmit Delay is %d sec, State %s,",
	   ospf_info->if_transdly, 
	   ospf_if_state_string(ospf_info->if_state));
    if (ospf_info->if_type < IF_POINT_TO_POINT) {
	printf(" Priority %d ", ospf_info->rtr_pri);
        if (ospf_info->dr) {
	    printf("\n  Designated Router (ID) ");
	    print_ip_address_name(ospf_info->dr->nbr_id, FALSE);
	    printf(", Interface address %i",
		   ospf_info->dr->nbr_ipaddr);
	} else 
	    printf("\n  No designated router on this network");

	/* backup designated router on this interface */
	if (ospf_info->bdr) {
	    printf("\n  Backup Designated router (ID) ");
	    print_ip_address_name(ospf_info->bdr->nbr_id, FALSE); 
	    printf(", Interface address %i",
		   ospf_info->bdr->nbr_ipaddr);
	} else
	    printf("\n  No backup designated router on this network");
	if (ospf_info->oi_old_dr) {
	    printf("\n  Old designated Router (ID) ");
	    print_ip_address_name(ospf_info->oi_old_dr->nbr_id, FALSE);
	    printf(", Interface address %i", 
		   ospf_info->oi_old_dr->nbr_ipaddr);
	}
    }  
    if (mgd_timer_running(&ospf_info->oi_flush_net_timer)) {
	due_time = mgd_timer_left_sleeping(&ospf_info->oi_flush_net_timer);
	printf("\n  Flush timer for old DR LSA due in ");
	print_ticks_in_dhms(due_time);
    }
    printf("\n  Timer intervals configured, Hello %d, Dead %d, Wait %d,",
	   ospf_info->hello_interval, ospf_info->dead_interval,
	   ospf_info->wait_interval);
    printf(" Retransmit %d", ospf_info->retr_interval);
    if (ospf_info->if_state == IFS_DOWN)
	return;
    if (!idb->static_routing &&
	((idb->ip_passive_router & pdb->mask) == 0)) {
	printf("\n    Hello due in ");
	due_time = mgd_timer_left_sleeping(&ospf_info->oi_hello_timer); 
	print_ticks_in_dhms(due_time);
    } else
	printf("\n    No Hellos (Passive interface) ");
    if (mgd_timer_running(&ospf_info->oi_wait_timer)) {
	printf("\n    Wait time before Designated router selection ");
	due_time = mgd_timer_left_sleeping(&ospf_info->oi_wait_timer); 
	print_ticks_in_dhms(due_time);
    }
    printf("\n  Neighbor Count is %d", ospf_info->nbrcnt);
    printf(", Adjacent neighbor count is %d ", ospf_info->nbr_fcnt);
    for (nbr = ospf_info->oi_nbr; nbr; nbr = next_nbr) {
	mem_lock(nbr);
	if (nbr->nbr_state == NBR_FULL) {
	    printf("\n    Adjacent with neighbor ");
	    print_ip_address_name(nbr->nbr_id, FALSE);
	    if (ospf_info->dr == nbr) 
		printf("  (Designated Router)");
	    else if (ospf_info->bdr == nbr)
		printf("  (Backup Designated Router)");
	    if (nbr->nbr_suppress_hello)
		printf("  (Hello suppressed)");
	}
	next_nbr = nbr->nbr_next;
	free(nbr);
    }
    printf("\n  Suppress hello for %d neighbor(s)", 
	   ospf_info->oi_suppress_hello_count);
    if (area_ptr->ar_autype == OSPF_AUTH_MD)
        display_ospf_message_digest(ospf_info);
} /* end of display_ospf_idb */

/*
 * show_rtr_routes
 * Display Routes to ASBR/ABR
 */

static void show_rtr_routes (pdbtype *pdb)
{
    rtr_route *route, *hash;
    ospf_rtr_path *path, *next_path;
    int i, j;

    for (i = 0; i < HTBLSIZE; i++) {
	hash = (pdb->ospf_routes)[i];
 	for (j = 0; (route = queuepick(&hash, j)); j++) {
 	    if (!validmem(route)) {
 		printf("\nOSPF: Router route is being deleted, please try"
 		       " again later");
 		return;
 	    }
 	    mem_lock(route);
	    for (path = route->rt_path_list; path; path = next_path) {
		mem_lock(path);
		if (path->orp_route_type == RTYPE_INTRA)
		    printf("\ni ");
		else
		    printf("\nI ");

		printf("%i [%d] via %i, %s, ", route->rt_dest, path->orp_cost,
		       path->orp_forw_addr, path->orp_idb->namestring);
		if (path->orp_dest_type == (DTYPE_ASBR | DTYPE_ABR))
		    printf("ABR/ASBR");
		else
		    if (path->orp_dest_type == DTYPE_ASBR)
			printf("ASBR");
		    else
			printf("ABR");
  		
		printf(", Area %s, SPF %d", path->orp_area->ar_id_str,
		       path->orp_spf_inst);
		next_path = path->orp_next;
		free(path);
	    }
 	    free(route);
 	}
    }
} /* end of show_rtr_routes */

/*
 * show_router_routes
 * Display Routes to ASBR/ABR to
 * all processes.
 */

static void show_router_routes (void)
{
    pdbtype	*pdb;
    
    automore_enable(NULL);
    for (pdb = ospf_pdbs_ptr; pdb ; pdb = pdb->ospf_next) {
	if (automore_quit()) 
	    break;
	printf("\n\nOSPF Process %d internal Routing Table\n", pdb->asystem);
	printf("\nCodes: i - Intra-area route, I - Inter-area route\n");
	show_rtr_routes(pdb);
    }
    automore_disable();
} /* end of show_router_routes */

/*
 * display_pdb
 * Display OSPF process
 * information
 */

static void display_pdb (pdbtype *pdb)
{
    pdbtype *proto, *next_proto;
    boolean headerflag;
    areatype *area, *next_area;
    addr_range *range, *next_range;
    boolean first;
    long due_time;
    ospf_pdb_info *ospf_pdb;

    ospf_pdb = pdb->ospf_pdb;
    mem_lock(ospf_pdb);
    headerflag = FALSE;
    printf("\n Routing Process \"ospf %d\"", pdb->asystem);
    printf(" with ID %i", pdb->ospf_rtr_id);
    if (pdb->ospf_tos_option)
	printf("\n Supports more than one TOS routes");
    else
	printf("\n Supports only single TOS(TOS0) routes");
    if ((pdb->ospf_area_brtr) && (pdb->ospf_as_brtr))
     printf("\n It is an area border and autonomous system boundary router");
    else {
	if (pdb->ospf_area_brtr)
	    printf("\n It is an area border router");
	else if (pdb->ospf_as_brtr)
	    printf("\n It is an autonomous system boundary router");
    }
    if (mgd_timer_running(&ospf_pdb->ospf_sumupdate_timer)) {
	printf("\n Summary Link update interval is ");
	print_ticks_in_dhms(LS_REFRESH_TIME*ONESEC);
	printf(" and the update due in ");
	due_time = mgd_timer_left_sleeping(&ospf_pdb->ospf_sumupdate_timer);
	print_ticks_in_dhms(due_time);
    }
    if (mgd_timer_running(&ospf_pdb->ospf_exupdate_timer)) {
	printf("\n External Link update interval is ");
	print_ticks_in_dhms(LS_REFRESH_TIME*ONESEC);
	printf(" and the update due in ");
	due_time = mgd_timer_left_sleeping(&ospf_pdb->ospf_exupdate_timer);
	print_ticks_in_dhms(due_time);
    }
    if (pdb->ospf_as_brtr) {
	printf("\n Redistributing External Routes from,");
	for (proto = (pdbtype *) protoQ.qhead; proto; proto = next_proto) {
	    mem_lock(proto);
	    if ((proto->mask & pdb->distflag) && (pdb != proto)) {
		printf("\n    %s", proto->name);
		if (proto->asystem)
		    printf(" %d", proto->asystem);
		if (ip_def_metric_configured(pdb, proto->index))
		    printf(" with metric mapped to %d",
			   ip_get_def_metric_scalar(pdb, proto->index));
		if (pdb->metric[proto->index].subnets)
		    printf(", includes subnets in redistribution"); 
	    }
	    next_proto = proto->next;
	    free(proto);
	}
    }
    printf("\n SPF schedule delay %d secs, "
	   "Hold time between two SPFs %d secs",
	   pdb->ospf_spf_schd_delay, pdb->ospf_spf_hold_interval);
    printf("\n Number of DCbitless external LSA %u", 
	   ospf_pdb->ospf_dcbitless_lsa_count); 
    printf("\n Number of DoNotAge external LSA %u", 
	   ospf_pdb->ospf_do_not_age_lsa_count); 
    printf("\n Number of areas in this router is %d. %d normal %d stub "
	   "%d nssa", pdb->area_count, ospf_pdb->ospf_normal_area_count,
	   ospf_pdb->ospf_stub_area_count, ospf_pdb->ospf_nssa_area_count);
    for (area = pdb->ospf_area_list; area; area = next_area) {
	mem_lock(area);
        if (area->ar_id == BACKBONE_AREA_ID) {
	    printf("\n    Area BACKBONE(%s)",area->ar_id_str);
	    if (!ospf_backbone_active(area, TRUE))
		printf(" (Inactive)");
	} else {
	    printf("\n    Area %s",area->ar_id_str);
	}
 	printf("\n	Number of interfaces in this area is %d"
	       ,area->ar_ifcnt);
	if (area->ar_stub) {
	    printf("\n	It is a stub area");
	    if (area->ar_stub_nosum)
		printf(", no summary LSA in this area");
	    if (pdb->ospf_area_brtr)
		printf("\n	  generates stub default route with cost %d", 
		       area->ar_stub_default_cost);
	}
	if (area->ar_nssa) {
	    printf("\n	It is a NSSA area");
	    if (area->ar_nssa_no_redist)
		printf(", no redistribution into this area");
	    if (pdb->ospf_area_brtr) {
		if (area->ar_nssa_translate_type7)
		    printf("\n        Perform type-7/type-5 LSA translation");
		if (area->ar_nssa_default)
		    printf("\n        generates NSSA default route with "
			   "cost %d", area->ar_stub_default_cost);
	    }
	}
	switch (area->ar_autype) {
	    case OSPF_AUTH_CT:
	        printf("\n	Area has simple password authentication");
		break;
	    case OSPF_AUTH_MD:
	        printf("\n	Area has message digest authentication");
		break;
	    default:
	        printf("\n	Area has no authentication");
	}
	printf("\n	SPF algorithm executed %d times",
	       area->ar_ospfSpfRuns);
	printf("\n	Area ranges are");
	first = TRUE;
	for (range = area->ar_range_list; range; range = next_range) {
	    mem_lock(range);
	    printf("\n	   %i%m", range->net_address, range->net_mask);
	    if (range->net_cost == SUM_LS_INFINITY)
		printf(" Passive ");
	    else
		printf(" Active(%d) ", range->net_cost);
	    next_range = range->net_next;
	    free(range);
	}
	printf("\n	Link State Update Interval is ");
	print_ticks_in_dhms(LS_REFRESH_TIME * ONESEC);
	printf(" and due in ");
	due_time = mgd_timer_left_sleeping(&area->ar_update_timer);
	print_ticks_in_dhms(due_time);
	printf("\n	Link State Age Interval is ");
	print_ticks_in_dhms(AGE_TIME * ONESEC);
	printf(" and due in ");
	due_time = mgd_timer_left_sleeping(&area->ar_age_timer);
	print_ticks_in_dhms(due_time);
	if (mgd_timer_running(&area->ar_rebuild_rtr_timer)) {
	    due_time = mgd_timer_left_sleeping(&area->ar_rebuild_rtr_timer);
	    printf("\n	Router LSA rebuild timer due in ");
	    print_ticks_in_dhms(due_time);
	}
	printf("\n	Number of DCbitless LSA %u", 
	       area->ar_dcbitless_lsa_count);
	printf("\n	Number of indication LSA %u", 
	       area->ar_indication_lsa_count);
	printf("\n	Number of DoNotAge LSA %u", 
	       area->ar_do_not_age_lsa_count); 
	next_area = area->ar_next;
	free(area);
    }
    printf("\n");
    free(ospf_pdb);
} /* display_pdb */

/*
 * show_ospf_pdb
 * Display OSPF process
 * information
 */
static void show_ospf_pdb (pdbtype *pdb)
{
    pdbtype *next_pdb;

    automore_enable(NULL);
    if (pdb == NULL) {
	for (pdb = ospf_pdbs_ptr; pdb ; pdb = next_pdb) {
	    if (automore_quit())
		goto punt;
	    mem_lock(pdb);
	    display_pdb(pdb);
	    next_pdb = pdb->ospf_next;
	    free(pdb);
	}
    } else {
	mem_lock(pdb);
	display_pdb(pdb);
	free(pdb);
    }
  punt:
    automore_disable();
} /* end of display_ospf_pdb */

/*
 * display_ospf_vidb
 * Display link level statistics for one network interface
 */

static void display_ospf_vidb (idbtype *idb)
{
    pdbtype *pdb;
    nbrtype *nbr;
    ospf_idb_info *ospf_info = idb->ospf_info;
    ulong due_time;

    pdb = (pdbtype *)ospf_info->area_ptr->ar_pdb_ptr;
    printf("\nVirtual Link %s to router %i", idb->namestring, 
	   ospf_info->vnbr_id);
    if (ospf_info->if_state == IFS_POINT_TO_POINT)
	printf(" is up");
    else
	printf(" is down");
    if (ospf_info->oi_demand_circuit) {
	printf("\n  Run as demand circuit");
	printf("\n  DoNotAge LSA ");
	if (pdb->ospf_backbone->ar_dcbitless_lsa_count) {
	    printf("not allowed (Number of DCbitless LSA is %d).", 
		   pdb->ospf_backbone->ar_dcbitless_lsa_count);
	} else {
	    printf("allowed.");
	}
    }
    printf("\n  Transit area %s", ospf_info->vlink_trans->ar_id_str);
    if (idb->output_if)
	printf(", via interface %s", idb->output_if->namestring);

    printf(", Cost of using %d",ospf_info->if_cost[0]);
    printf("\n  Transmit Delay is %d sec,", ospf_info->if_transdly);
    printf(" State %s,", ospf_if_state_string(ospf_info->if_state));
    printf("\n  Timer intervals configured, Hello %d, Dead %d, Wait %d,"
	   ,ospf_info->hello_interval, ospf_info->dead_interval,
	   ospf_info->wait_interval);
    printf(" Retransmit %d", ospf_info->retr_interval);
    if (ospf_info->if_state == IFS_DOWN)
	return;
    if (!idb->static_routing &&
	((idb->ip_passive_router & pdb->mask) == 0)) {
	printf("\n    Hello due in ");
	due_time = mgd_timer_left_sleeping(&ospf_info->oi_hello_timer); 
	print_ticks_in_dhms(due_time);
    } else 
	printf("\n    No Hellos (Passive interface) ");
    if (ospf_info->oi_nbr) {
	nbr = ospf_info->oi_nbr; 
	printf("\n    Adjacency State %s",
	       ospf_nbr_state_string(nbr->nbr_state));
	if (nbr->nbr_suppress_hello)
	    printf(" (Hello suppressed)");
	if (mgd_timer_running(&nbr->nbr_lsa_retrs_timer)) {
	    printf("\n    Link State retransmission due in ");
	    due_time = mgd_timer_left_sleeping(&nbr->nbr_lsa_retrs_timer); 
	    print_ticks_in_dhms(due_time);
	    printf("\n         LSA in retransmission queue %u", 
		   nbr->nbr_retrans_queue.count);
	}		
    }
    if (ospf_info->area_ptr->ar_autype == OSPF_AUTH_MD)
        display_ospf_message_digest(ospf_info);
} /* end of display_ospf_vidb */

/*
 * show_virtual_links
 * Display Virtual-link
 * information
 */

static void show_virtual_links (void)
{
    pdbtype *pdb;
    areatype *backbone;
    idbtype *idb;
    
    automore_enable(NULL);
    for (pdb = ospf_pdbs_ptr; pdb ; pdb = pdb->ospf_next) {
	backbone = pdb->ospf_backbone;
	if (backbone == NULL)
	    continue;
	for (idb = backbone->ar_if_list; idb ; idb = 
	     idb->ospf_info->next) {
	    if (automore_quit())
		goto punt;
	    if (idb->ospf_info->if_type == IF_VIRTUAL_LINK)
		display_ospf_vidb(idb);
	}
    }
  punt:
    automore_disable();
} /* end of show virtual links */

/*
 * show_request_list
 *
 * show the list of link state request that we are asking from a neighbor.
 */
static void show_request_list (nbrtype *nbr, idbtype *idb)
{
    int type;
    ls_req_list *req;

    mem_lock(nbr);
    printf("\n\n Neighbor %i, interface %s address %i",
	   nbr->nbr_id, idb->namestring, nbr->nbr_ipaddr);

    printf("\n\n Type  LS ID           ADV RTR         Seq NO      Age    Checksum");
    for (type = LSA_TYPE_RTR; type <= LSA_TYPE_7_ASE; type++) {
	if (type == LSA_TYPE_IGNORE)
	    continue;
	if (nbr->nbr_ls_req[type] != NULL) {
	    for (req = nbr->nbr_ls_req[type]; req; req = req->req_next) {
		if (automore_quit()) 
		    goto punt;
		if (!validmem(req)) {
		    printf("\nOSPF: Request list is being changed, "
			   "please try again later");
		    goto punt;
		}

		mem_lock(req);
		printf("\n    %1d  %15i %15i %#10x  %6d %#08x", req->req_type, 
		       req->req_ls_id, req->req_adv_rtr, req->req_seq, 
		       req->req_age, req->req_chksum);
		free(req);
	    }
	}
    }

 punt:
    automore_disable();
    free(nbr);
}

/*
 * print_db_simple
 *
 * Print a LSA header.
 */
static void print_db_simple (lsdbtype *db)
{
    ulong time;
    lsatype *lsa;

    lsa = db->db_lsa_ptr;

    if (db)
	time = lsa->lsh_age + DB_ELAPSE(db);
    else
	time = lsa->lsh_age;
    printf("\n    %1d  %15i %15i %#10x  %6d %#08x", lsa->lsh_type,
	   lsa->lsh_id, lsa->lsh_adv_rtr, lsa->lsh_seq, time,
	   lsa->lsh_chksum);
}
/*
 * show_retransmission_list
 *
 * Show LSAs in a neighbor retransmission queue.
 */
static void show_retransmission_list (nbrtype *nbr, idbtype *idb)
{
    lsdb_list	*list, *next_list;
    long	due_time;

    mem_lock(nbr);
    printf("\n\n Neighbor %i, interface %s address %i",
	   nbr->nbr_id, idb->namestring, nbr->nbr_ipaddr);

    if (mgd_timer_running(&nbr->nbr_lsa_retrs_timer)) {

	printf("\n Link state retransmission due in ");
	due_time = mgd_timer_left_sleeping(&nbr->nbr_lsa_retrs_timer);
	print_ticks_in_dhms(due_time);
	printf(", Queue length %u", nbr->nbr_retrans_queue.count);  
	
	printf("\n\n Type  LS ID           ADV RTR         Seq NO      Age    Checksum");
	for (list = nbr->nbr_retrans_queue.q_first; list; 
	     list = next_list) {
	    if (automore_quit()) 
		goto punt;
	    ospf_lsdb_list_lock(list);
	    if (list->list_db_ptr)
		print_db_simple(list->list_db_ptr);
	    next_list = list->list_next;
	    ospf_lsdb_list_free(list);
	}
    }

 punt:
    automore_disable();
    free(nbr);
}

/*
 * show_ospf_internal_list
 *
 * Show either the link state request list or retransmission list
 * of a neighbor.
 */
void show_ospf_internal_list (parseinfo *csb)
{
    ipaddrtype address;
    idbtype *idb;
    nbrtype *nbr;
    ospf_idb_info *ospf_info;

    idb = GETOBJ(idb,1);
    address = GETOBJ(paddr,1)->ip_addr;
    ospf_info = idb->ospf_info;
    if (!ospf_info)
        return;

    /* Search thru all neighbors for the one that matches address
     */
    for (nbr = ospf_info->oi_nbr; nbr && nbr->nbr_id != address; 
	 nbr = nbr->nbr_next);

    if (nbr) {
	switch (csb->which) {
	case OSPF_RETX_LIST:
	    automore_enable(NULL);
	    show_retransmission_list(nbr, idb);
	    automore_disable();
	    break;
	case OSPF_REQ_LIST:
	    automore_enable(NULL);
	    show_request_list(nbr, idb);
	    automore_disable();
	    break;
	default:
	    return;
	}
    } else
	printf("\nCannot find neighbor %i", address);
}

/*
 * ospf_event
 *
 * Log events so we can detect what's going on.
 */

#define OSPF_NUM_EVENTS 1000
static ospf_event_type *events;
static ospf_event_type *ev_curptr;

void ospf_event (ospf_event_code code, ulong data1, ulong data2)
{
    /* If the system has crashed, forget it. */

    if (process_get_boolean(system_crashed))
	return;

    /* If no memory, forget it. */

    if (events == NULL)
	return;

    /* Record it. */

    ev_curptr->code = code;
    ev_curptr->data1 = data1;
    ev_curptr->data2 = data2;
    GET_TIMESTAMP(ev_curptr->timestamp);

    /* Bump and circle. */

    ev_curptr++;

    if (ev_curptr >= (events + OSPF_NUM_EVENTS))
	ev_curptr = events;
}

/*
 * init_ospf_events
 *
 * Grab some memory and set things up.
 */

void ospf_init_events (void)
{

    /* If it's already there, so be it. */

    if (events != NULL)
	return;

    /* If malloc returns NULL, so be it. */

    ev_curptr = events = malloc(sizeof(ospf_event_type) *
				OSPF_NUM_EVENTS);
}
/*
 * ospf_log_copy
 *
 * Copy the event log to a newly allocated structure.
 * Used for displaying the log and, when needed, as a debugging tool
 * to capture events leading to a certain event by copy the event to
 * another permanent log.
 */

static void ospf_log_copy (
    ospf_event_type *src,
    ospf_event_type *src_ptr,
    ospf_event_type **buf,
    ospf_event_type **bufptr)
{
    if (src == NULL)
	return;
    if (*buf == NULL) {
	*buf = malloc(sizeof(ospf_event_type) * OSPF_NUM_EVENTS);
	if (*buf == NULL)
	    return;
    }
    bcopy(src, *buf, (sizeof(ospf_event_type) * OSPF_NUM_EVENTS));
    *bufptr = (src_ptr - src) + *buf;
}


/*
 * show_ospf_events
 *
 * Display the event list.
 */

typedef enum {NUMBER, HEX, IPADDR, STRING, DB_THING, DB_THING2} data_type_enum;

static void show_ospf_events (void)
{
    ospf_event_type *show_ptr, *end_ptr;
    ospf_event_type *copy, *copy_curptr;
    int trip_count = 0;
    sys_timestamp curtime;

    /* If no memory, forget it. */

    if (events == NULL)
	return;

    automore_enable(NULL);

    copy = NULL;
    ospf_log_copy(events, ev_curptr, &copy, &copy_curptr);
    if (copy == NULL) {
	printf("\n  Insufficient free memory to display event log.");
	return;
    }

    show_ptr = copy_curptr;		/* We decrement first. */
    end_ptr = copy_curptr;		/* The last one */

    GET_TIMESTAMP(curtime);

    /*
     * Go backwards from the current position.
     */
    do {
	char *type = "";
	data_type_enum data1_type = HEX;
	data_type_enum data2_type = HEX;
	char *data1_string;
	char *data2_string;

	/* Bump the pointer.  Wrap as necessary. */

	trip_count++;
	show_ptr--;
	if (show_ptr < copy)
	    show_ptr = copy + OSPF_NUM_EVENTS - 1;

	data1_string = (char *) show_ptr->data1;
	data2_string = (char *) show_ptr->data2;

	/* Format the entry. */

	if (show_ptr->code != 0) {
	    switch (show_ptr->code) {
	    case OC_GENERIC:
		type = "Generic";
		data1_type = STRING;
		break;
	    case OC_TIMER_EXPIRED:
		type = "Timer Exp";
		data1_type = STRING;
		break;
	    case OC_SUSPEND:
		type = "Suspend";
		break;
	    case OC_UNSUSPEND:
		type = "End Suspend";
		break;
	    case OC_BAD_PKT_RCVD:
		type = "Bad pkt rcvd";
		data1_type = IPADDR;
		data2_type = NUMBER;
		break;
	    case OC_CLEAN_AREA:
		type = "Clean area";
		data1_type = IPADDR;
		break;
	    case OC_CLEANUP:
		type = "Cleanup";
		break;
	    case OC_DB_ADD:
		type = "DB add";
		data1_type = IPADDR;
		data2_type = DB_THING;
		break;
	    case OC_DB_FREE:
		type = "DB free";
		data1_type = IPADDR;
		data2_type = DB_THING;
		break;
	    case OC_ELECT_BDR:
		type = "Elect BDR";
		data1_type = STRING;
		data1_string = ((idbtype *)show_ptr->data1)->namestring;
		data2_type = IPADDR;
		break;
	    case OC_ELECT_DR:
		type = "Elect DR";
		data1_type = STRING;
		data1_string = ((idbtype *)show_ptr->data1)->namestring;
		data2_type = IPADDR;
		break;
	    case OC_CLEAN_IF:
		type = "Clean i/f";
		data1_type = STRING;
		data1_string = ((idbtype *)show_ptr->data1)->namestring;
		break;
	    case OC_ROUTE_ADJUST:
		type = "Route adjust";
		data1_type = STRING;
		data1_string = ((idbtype *)show_ptr->data1)->namestring;
		break;
	    case OC_SPF:
		type = "Schedule SPF";
		data1_type = IPADDR;
		break;
	    case OC_I_ERROR:
		type = "i/f state error";
		data1_type = STRING;
		data1_string = ((idbtype *)show_ptr->data1)->namestring;
		break;
	    case OC_I_UP:
		type = "i/f state up";
		data1_type = STRING;
		data1_string = ((idbtype *)show_ptr->data1)->namestring;
		break;
	    case OC_I_DOWN:
		type = "i/f state down";
		data1_type = STRING;
		data1_string = ((idbtype *)show_ptr->data1)->namestring;
		break;
	    case OC_I_LOOP:
		type = "i/f state loopback";
		data1_type = STRING;
		data1_string = ((idbtype *)show_ptr->data1)->namestring;
		break;
	    case OC_I_UNLOOP:
		type = "i/f state unloop";
		data1_type = STRING;
		data1_string = ((idbtype *)show_ptr->data1)->namestring;
		break;
	    case OC_I_WAITTM:
		type = "i/f state waittm";
		data1_type = STRING;
		data1_string = ((idbtype *)show_ptr->data1)->namestring;
		break;
	    case OC_I_NBRCH:
		type = "i/f state nbr chg";
		data1_type = STRING;
		data1_string = ((idbtype *)show_ptr->data1)->namestring;
		break;
	    case OC_I_BACKUP:
		type = "i/f state backup";
		data1_type = STRING;
		data1_string = ((idbtype *)show_ptr->data1)->namestring;
		break;
	    case OC_N_ERROR:
		type = "nbr state error";
		data1_type = IPADDR;
		break;
	    case OC_N_START:
		type = "nbr state start";
		data1_type = IPADDR;
		break;
	    case OC_N_2WAY:
		type = "nbr state 2way";
		data1_type = IPADDR;
		break;
	    case OC_N_ADJOK:
		type = "nbr state adjok";
		data1_type = IPADDR;
		break;
	    case OC_N_NEGDONE:
		type = "nbr state negdone";
		data1_type = IPADDR;
		break;
	    case OC_N_EXDONE:
		type = "nbr state exdone";
		data1_type = IPADDR;
		break;
	    case OC_N_BADREQ:
		type = "nbr state badreq";
		data1_type = IPADDR;
		break;
	    case OC_N_BADSEQ:
		type = "nbr state badseq";
		data1_type = IPADDR;
		break;
	    case OC_N_LOADDONE:
		type = "nbr state loaddone";
		data1_type = IPADDR;
		break;
	    case OC_N_1WAY:
		type = "nbr state 1way";
		data1_type = IPADDR;
		break;
	    case OC_N_RSTAD:
		type = "nbr state reset";
		data1_type = IPADDR;
		break;
	    case OC_N_DOWN:
		type = "nbr state down";
		data1_type = IPADDR;
		break;
	    case OC_SERVICE_MAXAGE:
		type = "delete MAXAGE lsa";
		break;
	    case OC_INSERT_MAXAGE:
		type = "Insert MAXAGE lsa";
		data2_type = IPADDR;
		break;
	    case OC_DEL_1_RT_LST:
		type = "Delete 1 route list";
		break;
	    case OC_DEL_RT_LST:
		type = "Delete route list";
		data1_type = IPADDR;
		data2_type = DB_THING;
		break;
	    case OC_DEL_RT_LST_INFO:
		type = "Delete route list INFO";
		data1_type = IPADDR;
		break;
	    case OC_DEL_RT_LST_2:
		type = "Delete route list 2";
		data1_type = IPADDR;
		data2_type = DB_THING;
		break;
	    case OC_INS_RT_LST:
		type = "Insert route list";
		data1_type = IPADDR;
		data2_type = DB_THING;
		break;
	    case OC_INS_RT_LST_INFO:
		type = "Insert route list INFO";
		data2_type = DB_THING2;
		break;
	    case OC_REDIST_SUM_CHECK:
		type = "Redist summary-address check";
		data1_type = IPADDR;
		data2_type = DB_THING;
		break;
	    case OC_SERVICE_REDIST_SUM:
		type = "Service Redist scan";
		break;
	    case OC_MASK_CHANGED:
                type = "Mask changed";
                data1_type = IPADDR;
                data2_type = DB_THING;
		break;
	    default:
		type = "Unknown";
	    }

	    printf("\n%4d %7d %s:  ", trip_count, 
		   CLOCK_DIFF_SIGNED(show_ptr->timestamp, curtime), type);
	    switch (data1_type) {
	    case NUMBER:
		printf("%d", show_ptr->data1);
		break;
	    case IPADDR:
		printf("%i", show_ptr->data1);
		break;
	    case STRING:
		printf("%s", data1_string);
		break;
	    case HEX:
		printf("0x%x", show_ptr->data1);
		break;
	    case DB_THING:
		printf("0x%x %d", show_ptr->data1 & 0xffffff,
		       (show_ptr->data1 >> 24) & 0xff);
		break;
	    case DB_THING2:
		printf("  0x%x %d", show_ptr->data1 & 0x7fffff,
		       (show_ptr->data1 >> 23) & 0x1ff);
		break;
	    }
	    switch (data2_type) {
	    case NUMBER:
		printf("  %d", show_ptr->data2);
		break;
	    case IPADDR:
		printf("  %i", show_ptr->data2);
		break;
	    case STRING:
		printf("  %s", data2_string);
		break;
	    case HEX:
		printf("  0x%x", show_ptr->data2);
		break;
	    case DB_THING:
		printf("  0x%x %d", show_ptr->data2 & 0xffffff,
		       (show_ptr->data2 >> 24) & 0xff);
		break;
	    case DB_THING2:
		printf("  0x%x %d", show_ptr->data2 & 0x7fffff,
		       (show_ptr->data2 >> 23) & 0x1ff);
		break;
	    }
	}
    } while (show_ptr != end_ptr && !automore_quit());
    free(copy);
    automore_disable();
}

/*
 * ospf_show_pdb_maxage_list
 */
static void ospf_show_pdb_maxage_list (pdbtype *pdb)
{
    lsdbtype *db, *ptr1, *temp;
    lsatype *ptr2;

    printf("\n  AS System %d", pdb->asystem);
    printf("\n  Maxage delete timer due in ");
    if (pdb->ospf_flag & OSPF_MAXAGEQ) {
	print_ticks_in_dhms((MAXAGEQ_SCAN_INTERVAL*ONESEC - 
			    ELAPSED_TIME(pdb->ospf_maxage_delete_time)));
    } else {
	printf("NEVER");
    }
    for (db = pdb->ospf_max_headptr; db; db = temp) {
	temp = db->db_mnext;
	if (automore_quit())
	   goto punt;
	if (!validmem(db))
	   goto punt;
 	if (db->db_lsa_ptr == NULL)
 	    continue;
 	ptr1 = db;
 	ptr2 = db->db_lsa_ptr;
 	mem_lock(ptr1);
 	mem_lock(ptr2);
       	print_db_simple(db);
	/*
	 * The next db pointer may change
	 */
	temp = db->db_mnext;
	free(ptr1);
	free(ptr2);
    }
 punt:
    automore_disable();
}

/*
 * ospf_show_maxage_list
 * Print dbs in the maxage queue
 * Use for debugging if LSA is stuck
 */
static void ospf_show_maxage_list (pdbtype *pdb)
{
    if (!pdb) {
	for (pdb = ospf_pdbs_ptr; pdb; pdb = pdb->ospf_next)
	    ospf_show_pdb_maxage_list(pdb);
    } else {
	ospf_show_pdb_maxage_list(pdb);
    }
}

/*
 * ospf_show_command
 * Display OSPF Protocol information
 */
void ospf_show_command (parseinfo *csb)
{
    areatype	*area;
    pdbtype	*pdb;
    ulong 	area_id;
    
    pdb = NULL;
    if (GETOBJ(int,1) > 0) {
	for (pdb = ospf_pdbs_ptr; pdb; pdb = pdb->ospf_next) {
	    if (pdb->asystem == GETOBJ(int,1)) {
		break;
	    }
	}
	if (pdb == NULL) {
	    printf("\nInvalid OSPF router process: %d", GETOBJ(int,1));
	    return;
	}
    }

    area = NULL;
    if (GETOBJ(int,5)) {
        if (GETOBJ(paddr,2)->type == ADDR_IP){
	    area_id = (ulong) GETOBJ(paddr,2)->ip_addr;
        } else {
	    area_id = (ulong) GETOBJ(int,2);
        }
	for (area = pdb->ospf_area_list;
	     area; area=area->ar_next) {
	    if (area->ar_id == area_id) {
		break;
	    }
	}	
	if (area == NULL) {
	    printf("\nInvalid OSPF area: %i", area_id);
	    return;
	}
    }

    switch (csb->which) {
      case OSPF_INTERFACE:
	automore_enable(NULL);
	show_swidb_chain(GETOBJ(idb,1), display_ospf_idb, 0);
	automore_disable();
	break;
      case OSPF_NEIGHBOR:
	automore_enable(NULL);
	display_ospf_nbr(GETOBJ(idb,1), GETOBJ(paddr,1)->ip_addr, 
			 GETOBJ(int,4));
	automore_disable();
	break;
      case OSPF_VLINKS:
	show_virtual_links();
	break;
      case OSPF_EVENTS:
	show_ospf_events();
	break;
      case OSPF_BORDER_ROUTER:
	show_router_routes();
	break;
      case OSPF_DATABASE:
	automore_enable(NULL);
	if (GETOBJ(paddr,1)->type == ADDR_IP) 
	   show_database(pdb, area, GETOBJ(int,3), GETOBJ(paddr,1)->ip_addr,
			 TRUE, GETOBJ(int,4));
	else
	   show_database(pdb, area, GETOBJ(int,3), 0, FALSE, GETOBJ(int,4));
	automore_disable();
	break;
      case OSPF_DEFAULT:
	show_ospf_pdb(pdb);
	break;
      case OSPF_SPF_STAT:
        automore_enable(NULL);
	print_ospf_spf();
	automore_disable();
	break;
      case OSPF_DELETE_LIST:
        automore_enable(NULL);
        show_delete();
        automore_disable();
	break;
      case OSPF_SUMMARY_ADDRESS:
	automore_enable(NULL);
	show_ospf_summary_address();
	automore_disable();
	break;
      case OSPF_DATABASE_SUMMARY:
	automore_enable(NULL);
	show_database_summary(pdb, area);
	automore_disable();
	break;
      case OSPF_MAXAGE_LIST:
	automore_enable(NULL);
	ospf_show_maxage_list(pdb);
	automore_disable();
	break;
    case OSPF_BAD_CHECKSUM_QUEUE:
	automore_enable(NULL);
	ospf_show_bad_checksum_queue();
	automore_disable();
	break;
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
} /* end of show_ospf */

/*
 * ospf_show_traffic
 * display ospf traffic.
 */

void ospf_show_traffic (void)
{
    printf("\n\nOSPF statistics:");
    printf("\n  Rcvd: %lu total, %lu checksum errors",
 		ospf_traffic.inputs, ospf_traffic.checksumerr);
    printf("\n  	%lu hello, %lu database desc, %lu link state req"
	   "\n  	%lu link state updates, %lu link state acks\n",
	   ospf_traffic.input_hello, ospf_traffic.input_dbdes,
	   ospf_traffic.input_req, ospf_traffic.input_lsupd,
	   ospf_traffic.input_ack);

    printf("\n  Sent: %lu total", ospf_traffic.outputs);
} /* end of ospf_show_traffic */


/*
 * ospf_stub_and_asbr_check
 * Gives warning message if we currently have one area and it is a stub
 * area while we are ASBR
 */
void ospf_stub_and_asbr_check (pdbtype *pdb)
{
    areatype *area;

    if ((pdb->running) && (!router_init_running) && (pdb->ospf_as_brtr) 
	&& (pdb->area_count == 1)) {
	area = pdb->ospf_area_list;
	if (area && area->ar_stub)
	    printf("\nWarning: Router is currently an ASBR while having"
		   " only one area which is a stub area");
    }
} /* end of ospf_stub_and_asbr_check */
