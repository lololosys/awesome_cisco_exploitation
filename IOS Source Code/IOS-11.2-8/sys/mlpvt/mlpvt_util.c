/* $Id: mlpvt_util.c,v 1.1.42.5 1996/07/25 22:33:30 perryl Exp $
 * $Source: /release/112/cvs/Xsys/mlpvt/mlpvt_util.c,v $
 *------------------------------------------------------------------
 * mlpvt_util.c -- MLPVT group setup and maintenance utilities
 *
 * January 1996, Andrew G. Harvey
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mlpvt_util.c,v $
 * Revision 1.1.42.5  1996/07/25  22:33:30  perryl
 * CSCdi62937:  Multichassis Multilink PPP crash in TCP
 * Branch: California_branch
 * SGBP may get 2 or more message on a closing tcb. Keep
 * track of free status of closing tcb in mlpvt_process_tcp_message()
 * Do not free an already freed tcb.
 *
 * Revision 1.1.42.4  1996/05/18  22:35:01  perryl
 * CSCdi57939:  MLPVT is a bad name for Stack Group Bidding Protocol
 * Branch: California_branch
 * Rename it to SGBP, Stack Group Bidding Protocol
 *
 * Revision 1.1.42.3  1996/05/06  21:47:15  perryl
 * CSCdi56528:  MLPVT doesnt allow stack member to use more than 1 IP src
 * address.
 * Branch: California_branch
 * Add a secondary address list to the MLPVT member structure.
 * When a hello msg's source IP address different than the
 * configured member IP address, record it in the secondary
 * address list. When TCP pipe open request, it will than
 * able to recognize the address as the member's
 *
 * Revision 1.1.42.2  1996/05/06  18:09:00  syiu
 * CSCdi56520:  Show ppp multilink not showing member links
 * Branch: California_branch
 *
 * Revision 1.1.42.1  1996/04/27  06:34:43  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 1.1.2.9  1996/04/22  00:31:20  perryl
 * Branch: Dial1_branch
 * Added CHAP style authenication to MLPVT.
 * Used AAA to obtain password, eliminate "mlpvt password"
 * command.
 * Fixed misc bugs and enhanced debug messages.
 *
 * Revision 1.1.2.8  1996/04/17  03:32:33  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 1.1.2.7  1996/03/09  07:18:08  syiu
 * Branch: Dial1_branch
 * Move virtual-template from mlpvt to mlp
 *
 * Revision 1.1.2.6  1996/03/05  02:46:59  agh
 * Branch: Dial1_branch
 * Much cleanup of unwanted code, also added comments.
 *
 * Revision 1.1.2.5  1996/03/02  02:57:27  abaindur
 * Branch: Dial1_branch
 *  Remove hooks that were added for multi-MLPVT groups per system.
 *
 * Revision 1.1.2.4  1996/03/01  22:58:09  abaindur
 * Branch: Dial1_branch
 *  Remove config knob for a tie-breaker metric used in case of equal
 *  MLPPP bundle mastership bids.
 *
 * Revision 1.1.2.3  1996/02/08  23:47:17  syiu
 * Branch: Dial1_branch
 * Support for CHAP challenge by stackname if locally defined
 *
 * Revision 1.1.2.2  1996/02/08  06:51:44  syiu
 * Branch: Dial1_branch
 * Tie together MLPVT/MLP/VPN registries, add vtemplate
 * support in MLPVT
 *
 * Revision 1.1.2.1  1996/01/26  20:28:47  agh
 * Branch: Dial1_branch
 * Initial Code Commit for MLPVT.
 *
 * Revision 1.1  1996/01/26  19:22:53  abaindur
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "../ip/ip.h"
#include "../tcp/tcp.h"
#include "../ip/ip_registry.h"
#include "logger.h"
#include "login.h"
#include "../os/list.h"

#include "mlpvt.h"
#include "mlpvt_util.h"

static list_header *mlpvt_groups;
extern ipaddrtype sgbp_source_ip;

/*
 * =================================================================
 * Name: mlpvt_find_stackname
 *
 * Description: Get the Stack name defined locally. 
 *              Since we only support one group number currently, 
 *              the first group is _the_ configured group
 * Parameters:
 *           stackname = char ptr to stackname
 *           len = length of char string passed in 
 *           source_ip = the source ip address of this member.
 * Returns:   
 *           TRUE & Copies Stack name to stackname ptr
 *           FALSE 
 *
 * =================================================================
 */

boolean
mlpvt_find_stackname (char *stackname, int len, ipaddrtype *source_ip)
{
    mlpvtgroup_t *grp;

    if (!stackname)
       return(FALSE);

    /*
     * Find the first group
     */
    grp = mlpvt_getgroup();
    if (!grp)
        return(FALSE);

    if (strlen(grp->name) >= len)
       return(FALSE);

    strcpy(stackname, grp->name);

    *source_ip = sgbp_source_ip;
    return(TRUE);
    
}

/*
 * =================================================================
 * Name: mlpvt_find_membername
 *
 * Description: Get the member name defined locally. 
 * Parameters:
 *           membername = char ptr to membername
 *           len = length of char string passed in 
 *           ip_address = IP address 
 * Returns:   
 *           TRUE & Copies member name to membername ptr
 *           FALSE 
 *
 * =================================================================
 */

boolean
mlpvt_find_membername (char *membername, int len, ipaddrtype ip_address)
{
    mlpvtmember_t *member;

    if (!membername)
       return(FALSE);

    member = mlpvt_getmemberbyaddr(NULL, ip_address);
    if (!member)
       return(FALSE);

    if (strlen(member->name) >= len)
       return(FALSE);

    strcpy(membername, member->name);
    return(TRUE);
    
}
/*
 * =================================================================
 * Name: mlpvt_getgroup
 *
 * Description:	Return the first group from the list.  For the first
 *		revision of mlpvt there will only be one group.
 *
 * Parameters:	void - None.
 *
 * Returns:	mlpvtgroup_t * - pointer to the first group.
 *
 * =================================================================
 */

mlpvtgroup_t *
mlpvt_getgroup (void)
{
    mlpvtgroup_t *grp;
    
    if ((mlpvt_groups == NULL) || !LIST_VALID(mlpvt_groups))
        return(NULL);

    grp = LIST_HEAD_DATA(mlpvt_groups);
    return(grp);
}

/*
 * =================================================================
 * Name: mlpvt_getgroupbyname
 *
 * Description:	Find a group using its name.
 *
 * Parameters:	char * - name of group.
 *
 * Returns:	mlpvtgroup_t * - pointer to group.
 *		NULL - group not found.
 *
 * =================================================================
 */

mlpvtgroup_t *
mlpvt_getgroupbyname (char *name)
{
    mlpvtgroup_t *grp;

    grp = mlpvt_getgroup();

    if (grp != NULL) {
        if ((strcmp(name, grp->name)) == 0)
            return(grp);
    }
    return(NULL);
}

/*
 * =================================================================
 * Name: mlpvt_getfirst_member
 *
 * Description:	Find the first member of this group.  If group is
 *		NULL find the first member of any group.
 *
 * Parameters:	mlpvtgroup_t * - pointer to group.
 *
 * Returns:	mlpvtmember_t * - pointer to first member of group.
 *		NULL - member not found.
 *
 * =================================================================
 */

mlpvtmember_t *
mlpvt_getfirst_member (mlpvtgroup_t *grp)
{
    mlpvtmember_t *memb = NULL;
    
    if (grp == NULL)
        grp = mlpvt_getgroup();

    if ((grp != NULL) && (grp->members != NULL)) {
        memb = LIST_HEAD_DATA(grp->members);
        return(memb);
    }
    return(NULL);
}

/*
 * =================================================================
 * Name: mlpvt_getnext_member
 *
 * Description: Find next member in this group.
 *
 * Parameters:	mlpvtgroup_t * - group to search for member.
 *		mlpvtmember_t * - pointer to present member.
 *
 * Returns:	mlpvtmember_t * - pointer to next member.
 *
 * =================================================================
 */

mlpvtmember_t *
mlpvt_getnext_member (mlpvtgroup_t *grp, mlpvtmember_t *thismember)
{
    mlpvtmember_t *memb;
    list_element *el;

    if (thismember == NULL)
        return(NULL);
    
    if (grp == NULL)
        grp = mlpvt_getgroup();

    if ((grp != NULL) && (grp->members != NULL)) {
        FOR_ALL_DATA_IN_LIST(grp->members, el, memb) {
            if (memb == thismember) {
                memb = LIST_NEXT_DATA(el);
                return(memb);
            }
        }
    }
    return(NULL);
}


/*
 * =================================================================
 * Name: mlpvt_getfirst_activemember
 *
 * Description:	Find the first active member of this group.  If group is
 *		NULL find the first member of any group.
 *
 * Parameters:	mlpvtgroup_t * - pointer to group.
 *
 * Returns:	mlpvtmember_t * - pointer to first member of group.
 *		NULL - member not found.
 *
 * =================================================================
 */

mlpvtmember_t *
mlpvt_getfirst_activemember (mlpvtgroup_t *grp)
{
    mlpvtmember_t *memb = NULL;
    list_element *el;
    
    if (grp == NULL)
        grp = mlpvt_getgroup();

    if ((grp != NULL) && (grp->members != NULL)) {
	FOR_ALL_DATA_IN_LIST(grp->members, el, memb) {
	    if (memb->status == MLPVTGROUP_MEMBER_ACTIVE) 
                return(memb);
	}
    }
    return(NULL);
}


/*
 * =================================================================
 * Name: mlpvt_getnext_memberbyref
 *
 * Description: Find next member using its reference number.
 *
 * Parameters:	mlpvtgroup_t * - group to search.
 *		ulong - next member reference number.
 *
 * Returns:	mlpvtmember_t * - pointer to next member or NULL if
 *		member is not found.
 *
 * =================================================================
 */

mlpvtmember_t *
mlpvt_getnext_memberbyref (mlpvtgroup_t *grp, ulong ref)
{
    mlpvtmember_t *memb;
    list_element *el;

    if (grp == NULL)
        grp = mlpvt_getgroup();

    if ((grp != NULL) && (grp->members != NULL)) {
        FOR_ALL_DATA_IN_LIST(grp->members, el, memb) {
            if (memb->reference == ref) {
                memb = LIST_NEXT_DATA(el);
                return(memb);
            }
        }
    }
    return(NULL);
}

/*
 * =================================================================
 * Name: mlpvt_getnext_activememberbyref
 *
 * Description: Find next active member using its reference number.
 *
 * Parameters:	mlpvtgroup_t * - group to search.
 *		ulong - next member reference number.
 *
 * Returns:	mlpvtmember_t * - pointer to next member or NULL if
 *		member is not found.
 *
 * =================================================================
 */

mlpvtmember_t *
mlpvt_getnext_activememberbyref (mlpvtgroup_t *grp, ulong ref)
{
    mlpvtmember_t *memb=NULL;
    list_element *el=NULL;

    if (grp == NULL)
        grp = mlpvt_getgroup();

    if ((grp != NULL) && (grp->members != NULL)) {
        FOR_ALL_DATA_IN_LIST(grp->members, el, memb) {
            if (memb->reference == ref) {
                el = LIST_NEXT_ELEMENT(el);
                break;
            }
        }
    }

    while (NULL != (memb = LIST_GET_DATA(el))) {
	if (memb->status == MLPVTGROUP_MEMBER_ACTIVE)
	    return(memb);
	el = LIST_NEXT_ELEMENT(el);
    }
    return(NULL);
}


static boolean 
mlpvt_chk_secaddr(mlpvtmember_t *memb, ipaddrtype addr)
{
    ipaddrtype *ipaddr;
    list_element *el=NULL;
    if (!memb)
	return FALSE;

    FOR_ALL_DATA_IN_LIST(memb->secaddr, el, ipaddr) {
	if (*ipaddr == addr) 
	    return TRUE;
    }
    return FALSE;
}

boolean
mlpvt_add_secaddr(mlpvtmember_t *memb, ipaddrtype addr)
{
    ipaddrtype *ipaddr;
    if (!memb)
	return FALSE;

    if (mlpvt_chk_secaddr(memb, addr))
	return TRUE;
    
    if (!(ipaddr = malloc_named(sizeof(ipaddrtype), "MLPVT sec addr")))
	return FALSE;

    *ipaddr = addr;
    return(list_enqueue(memb->secaddr, NULL, ipaddr));
}

static void 
mlpvt_del_secaddr(mlpvtmember_t *memb)
{
    ipaddrtype *ipaddr;
    list_element *el;

    if (!memb)
	return;

    FOR_ALL_DATA_IN_LIST(memb->secaddr, el, ipaddr) {
	free(ipaddr);
    }

    list_destroy(memb->secaddr);
}

/*
 * =================================================================
 * Name: mlpvt_getmemberbyaddr
 *
 * Description: Find a member in this group using its ip address.
 *		If group is NULL then find in first group.
 *
 * Parameters:	mlpvtgroup_t * - pointer to group.
 *		ipaddrtype - ip address of member.
 *
 * Returns:	mlpvtmember_t * - pointer to member or NULL if
 *		member is not found.
 *
 * =================================================================
 */

mlpvtmember_t *
mlpvt_getmemberbyaddr (mlpvtgroup_t *grp, ipaddrtype addr)
{
    mlpvtmember_t *memb;

    if (grp == NULL)
        grp = mlpvt_getgroup();

    if (grp != NULL) {
        memb = mlpvt_getfirst_member(grp);
        while (memb != NULL) {
            if (addr == memb->addr)
                return(memb);
	    else
		if (mlpvt_chk_secaddr(memb, addr))
		    return(memb);

            memb = mlpvt_getnext_member(grp, memb);
        }
    }
    return(NULL);
}

/*
 * =================================================================
 * Name: mlpvt_getmemberbyref
 *
 * Description: Find a member in this group using its reference.
 *		If group is NULL then find in first group.
 *
 * Parameters:	mlpvtgroup_t * - pointer to group.
 *		ulong - reference number for member.
 *
 * Returns:	mlpvtmember_t * - pointer to member or NULL if
 *		member is not found.
 *
 * =================================================================
 */

mlpvtmember_t *
mlpvt_getmemberbyref (mlpvtgroup_t *grp, ulong reference)
{
    mlpvtmember_t *memb;

    if (grp == NULL)
        grp = mlpvt_getgroup();

    if (grp != NULL) {
        memb = mlpvt_getfirst_member(grp);
        while (memb != NULL) {
            if (reference == memb->reference)
                return(memb);
            memb = mlpvt_getnext_member(grp, memb);
        }
    }
    return(NULL);
}

/*
 * =================================================================
 * Name: mlpvt_getmemberbyname
 *
 * Description: Find a member in this group using its name.
 *		If group is NULL then find in first group.
 *
 * Parameters:	mlpvtgroup_t * - pointer to group.
 *		char * - name of member.
 *
 * Returns:	mlpvtmember_t * - pointer to member or NULL if
 *		member is not found.
 *
 * =================================================================
 */

mlpvtmember_t *
mlpvt_getmemberbyname (mlpvtgroup_t *grp, char *name)
{
    mlpvtmember_t *memb;

    if (grp == NULL)
        grp = mlpvt_getgroup();

    if (grp != NULL) {
        memb = mlpvt_getfirst_member(grp);
        while (memb != NULL) {
            if ((strcmp(name, memb->name)) == 0)
                return(memb);
            memb = mlpvt_getnext_member(grp, memb);
        }
    }
    return(NULL);
}

/*
 * =================================================================
 * Name: mlpvt_get_connectedpeer_count
 *
 * Description:	Count the number of member for this group.
 *
 * Parameters:	mlpvtgroup_t * - group to search.
 *
 * Returns:	int - number of members in this group.
 *
 * =================================================================
 */

ulong
mlpvt_get_connectedpeer_count(mlpvtgroup_t *grp)
{
    mlpvtmember_t *memb;
    ulong count = 0;

    memb = mlpvt_getfirst_member(grp);
    while (memb != NULL) {
        if (memb->status == MLPVTGROUP_MEMBER_ACTIVE)
            count++;
        memb = mlpvt_getnext_member(grp, memb);
    }
    return(count);
}

/*
 * =================================================================
 * Name: mlpvt_addmember
 *
 * Description:	Enqueue a new member to this group.
 *
 * Parameters:	mlpvtgroup_t * - group to enqueue member.
 *		mlpvtmember-t * - member to enqueue.
 *
 * Returns:	boolean - TRUE if OK else FALSE.
 *
 * =================================================================
 */

boolean
mlpvt_addmember (mlpvtgroup_t *grp, mlpvtmember_t *memb)
{
    if (grp->members == NULL)
        return(FALSE);
    
    return(list_enqueue(grp->members, NULL, memb));
}

/*
 * =================================================================
 * Name: mlpvt_removemember
 *
 * Description: Dequeue a member from this group.
 *
 * Parameters:	mlpvtgroup_t * - group to dequeue member.
 *		mlpvtmember-t * - member to dequeue.
 *
 * Returns:	void - None.
 *
 * =================================================================
 */

void
mlpvt_removemember (mlpvtgroup_t *grp, mlpvtmember_t *memb)
{
    if (grp->members == NULL)
        return;
    list_remove(grp->members, NULL, memb);
}

/*
 * =================================================================
 * Name: mlpvt_addgroup
 *
 * Description:	Add group to list of groups.
 *
 * Parameters:	mlpvtgroup_t * - group to add into list
 *
 * Returns:	boolean - TRUE if OK else FALSE.
 *
 * =================================================================
 */

boolean
mlpvt_addgroup (mlpvtgroup_t *grp)
{
    return(list_enqueue(mlpvt_groups, NULL, grp));
}

/*
 * =================================================================
 * Name: mlpvt_removegroup
 *
 * Description: Remove a group from the group list.
 *
 * Parameters:	mlpvtgroup_t * - group to remove from list
 *
 * Returns:
 *
 * =================================================================
 */

void
mlpvt_removegroup (mlpvtgroup_t *grp)
{
    list_remove(mlpvt_groups, NULL, grp);
}

/*
 * =================================================================
 * Name: mlpvt_init_groups
 *
 * Description:	Initialize the group list data structure.
 *
 * Parameters:	void - None.
 *
 * Returns:	void - None.
 *
 * =================================================================
 */

void
mlpvt_init_groups (void)
{
    mlpvt_groups = list_create(NULL, 0, "MLPVT groups",
                                  LIST_FLAGS_AUTOMATIC);
    if (mlpvt_groups == NULL) {
        /*
         * XXX - what to do?
         */
        return;
    }
}

/*
 * =================================================================
 * Name: mlpvtgroup_member_delete
 *
 * Description:	Delete a member from a group.
 *
 * Parameters:	mlpvtgroup_t * - group to delete the member from.
 *		mlpvtmember_t * - member to delete from list.
 *
 * Returns:	void - None.
 *
 * =================================================================
 */

void
mlpvtgroup_member_delete (mlpvtgroup_t *grp, mlpvtmember_t *memb)
{
    /*
     * Remove member from list.
     */
    mlpvt_removemember(grp, memb);
    
    if (memb->name != NULL)
        free(memb->name);

    mlpvt_del_secaddr(memb);

    free(memb);
}

/*
 * =================================================================
 * Name: mlpvtgroup_delete
 *
 * Description:	Remove a group from the group list.  If any members
 *		are present remove the members.  If this is the last
 *		group remove the group data structure.
 *
 * Parameters:	mlpvtgroup_t * - group to delte
 *
 * Returns:	void - None.
 *
 * =================================================================
 */

void
mlpvtgroup_delete (mlpvtgroup_t *grp)
{
    mlpvtgroup_t *last;
    mlpvtmember_t *memb;
    mlpvtmember_t *nextmemb;

    mlpvt_removegroup(grp);

    memb = mlpvt_getfirst_member(grp);
    while (memb != NULL) {
        nextmemb = mlpvt_getnext_member(grp, memb);
        mlpvtgroup_member_delete(grp, memb);
        memb = nextmemb;
    }

    if (grp->name)
        free(grp->name);
    free(grp);

    /*
     * Check if this is the last group and if so stop the
     * processes.
     */
    last = mlpvt_getgroup();
    if (last == NULL) {
        mlpvt_process_shutdown();
        list_destroy(mlpvt_groups);
        mlpvt_groups = NULL;
    }
}

uchar
mlpvt_get_bidtype (mlpvtgroup_t *grp)
{
    return((grp = mlpvt_getgroup()) ?
	   grp->seedbidtype: MLPVT_SEEDBID_DEFAULT);
}
