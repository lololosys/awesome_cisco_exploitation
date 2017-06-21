/* $Id: fr_class.c,v 3.1.2.5 1996/09/11 16:49:36 liqin Exp $
 * $Source: /release/112/cvs/Xsys/wan/fr_class.c,v $
 *------------------------------------------------------------------
 * Frame Relay Map Class Inheritance
 *
 * March 1996, Charles Poe
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fr_class.c,v $
 * Revision 3.1.2.5  1996/09/11  16:49:36  liqin
 * CSCdi68280:  replace malloc with malloc_named in frame relay code
 * Branch: California_branch
 *
 * Revision 3.1.2.4  1996/07/31  22:11:08  achopra
 * CSCdi64288:  VC queueing commands cannot be removed from under
 * map-class command
 * Branch: California_branch
 *
 * Revision 3.1.2.3  1996/05/24  21:21:29  ccpoe
 * CSCdi58460:  map-class frame-relay traffic-shaping features dont work
 * on FR SVC
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/05/17  21:13:40  ccpoe
 * CSCdi57877:  Frame relay traffic shaping breaks modular images
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/05/15  05:54:37  fowler
 * Frame relay VC queuing and rate enforcement
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/26  05:47:57  ccpoe
 * placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "config.h"
#include "../ui/debug.h"
#include "serial.h"
#include "../if/static_map.h"
#include "frame_relay.h"
#include "fr_class.h"
#include "fr_debug.h"
#include "fr_traffic_shape.h"
#include "serial_debug.h"
#include "logger.h"
#include "parser.h"
#include "parser_defs_frame.h"
#include "../wan/fr_registry.h"


/*
 * fr_find_vc_class
 *     find the map class associated with a vc
 */
static uchar fr_find_vc_class(fr_pvc_info_type *pvc,
		       mapclass_type **classp)
{
    struct fr_idb_class *idb_classp;
    
    if (pvc->pvc_class) {
	*classp = pvc->pvc_class;
	return (pvc->pvc_inht);
    }

    idb_classp = fr_find_idb_class(pvc->idb);
    if (idb_classp->classp) {
	*classp = idb_classp->classp;
	return (idb_classp->idb_inht);
    }

    idb_classp = fr_find_idb_class(pvc->idb->hwptr->firstsw);
    if (idb_classp->classp) {
	*classp = idb_classp->classp;
	return (idb_classp->idb_inht);
    }
    
    *classp = NULL;
    return ((uchar) FR_FIRSTSW_CLASS);
}
	
/*
 * fr_find_idb_class
 * 	find the map class associated with a swidb
 */
struct fr_idb_class *fr_find_idb_class (idbtype *swidb)
{
    fr_idb_struct_type *fr_idb;
    struct fr_idb_class *idb_classp;
    hwidbtype *hwidb = swidb->hwptr;
    
    fr_idb = hwidb->frame_relay_stuff;
    idb_classp = fr_idb->fr_idb_classQ;
    while (idb_classp) {
	if (idb_classp->swidb == swidb) {
	    break;
	}
	idb_classp = idb_classp->next;
    }
    return idb_classp;
}
    
/*
 * fr_firstidb_class_add
 *     assign a map class to a main interface.
 */
void fr_firstidb_class_add (idbtype *swidb, mapclass_type *classp)
{
    hwidbtype *hwidb;
    struct fr_idb_class *idb_classp;
    idbtype *idb;

    hwidb = swidb->hwptr;
    idb_classp = fr_find_idb_class(swidb);
    idb_classp->classp = classp;
    fr_dlci_class_inht(swidb, 0, classp, CHANGE_CLASS, FR_FIRSTSW_CLASS);
    
    FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
	idb_classp = fr_find_idb_class(idb);
	if (idb_classp->idb_inht == FR_FIRSTSW_CLASS) {
	    idb_classp->classp = classp;
	}
    }
}

/*
 * fr_subidb_class_add
 *	assign a map class to a subinterface
 */
void fr_subidb_class_add (idbtype *swidb, mapclass_type *classp)
{
    struct fr_idb_class *idb_classp;

    idb_classp = fr_find_idb_class(swidb);
    idb_classp->idb_inht = FR_SUBSW_CLASS;
    idb_classp->classp = classp;
    fr_dlci_class_inht(swidb, 0, classp, ADD_CLASS, FR_SUBSW_CLASS);
}
    

/*
 * fr_firstidb_class_remove
 *	Remove the idb class if it's associated with first sw and pvcs
 *      inherited from the first sw.
 */
static void fr_firstidb_class_remove (idbtype *swidb, mapclass_type *classp)
			       
{
    hwidbtype *hwidb;
    struct fr_idb_class *idb_classp;
    idbtype *idb;

    hwidb = swidb->hwptr;
    idb_classp = fr_find_idb_class(swidb);
    if (idb_classp->swidb == swidb && idb_classp->classp == classp) {
	idb_classp->classp = NULL;
	
	FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
	    idb_classp = fr_find_idb_class(idb);
	    if (idb_classp->idb_inht == FR_FIRSTSW_CLASS) {
		idb_classp->classp = NULL;
	    }
	}
	fr_dlci_class_inht(swidb, 0, classp, REMOVE_CLASS, FR_FIRSTSW_CLASS);
    }
}

/*
 * fr_subidb_class_remove
 *	remove the class from the subinterface and pvcs inherited from
 *	the subinterface.
 */
static void fr_subidb_class_remove (idbtype *swidb, mapclass_type *classp)

{
    struct fr_idb_class *idb_classp, *first_idb_classp;
    
    idb_classp = fr_find_idb_class(swidb);
    if (idb_classp->classp == classp) {
	idb_classp->idb_inht = FR_FIRSTSW_CLASS;
	first_idb_classp = fr_find_idb_class(swidb->hwptr->firstsw);
	idb_classp->classp = first_idb_classp->classp;
	fr_dlci_class_inht(swidb, 0, classp, REMOVE_CLASS, FR_SUBSW_CLASS);
    }
}
	
/*
 * fr_dlci_class_inht
 *	Associate or disassociate all the dlci under a swidb, from the classp
 *	and reassociate to a right class.
 */
void fr_dlci_class_inht (idbtype *swidb,
			 ushort dlci,
			 mapclass_type *classp,
			 int class_cmd,
			 uchar vc_inht)
			 
{
    mapclass_type *new_inht_classp = NULL;
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *pvc;
    hwidbtype *hwidb;
    boolean find_one_pvc, bind_pvc;
    int i;
    
    hwidb = swidb->hwptr;
    find_one_pvc = TRUE;
    fr_idb = hwidb->frame_relay_stuff;
    
    i = MIN_DLCI;
    while (i < MAX_DLCI && find_one_pvc) {
	bind_pvc = FALSE;
	if (dlci > 0) {
	    find_one_pvc = FALSE;
	    i = dlci;
	}
	if ((pvc = fr_idb->fr_dlci_tab[i])) {
	    switch (class_cmd) {
	    case ADD_CLASS:
		new_inht_classp = classp;
		if (!vc_inht) {
		    pvc->pvc_inht = fr_find_vc_class(pvc, &new_inht_classp);
		    bind_pvc = TRUE;
		} else {
		    if (vc_inht == FR_SUBSW_CLASS && pvc->idb == swidb &&
		    (pvc->pvc_inht != FR_VC_CLASS)) {
			if (pvc->pvc_inht == FR_FIRSTSW_CLASS) {
			    pvc->pvc_inht = vc_inht;
			}
			bind_pvc = TRUE;
		    }
		    if (vc_inht == FR_VC_CLASS) {
			pvc->pvc_inht = vc_inht;
			bind_pvc = TRUE;
		    }
		}
		break;
		
	    case CHANGE_CLASS:
		new_inht_classp = classp;
		if (pvc->pvc_inht == vc_inht || vc_inht == 0) {
		    if (!new_inht_classp) {
			pvc->pvc_inht = fr_find_vc_class(pvc,
							 &new_inht_classp);
		    }
		    bind_pvc = TRUE;
		}
		break;
		
	    case REMOVE_CLASS:
		if (pvc->pvc_inht == vc_inht && pvc->pvc_class == classp) {
		    pvc->pvc_class = NULL;
		    pvc->pvc_inht = 0;
		    pvc->pvc_inht = fr_find_vc_class(pvc, &new_inht_classp);
		    bind_pvc = TRUE;
		}
		break;
		
	    default:
	    }
	    if (bind_pvc) {
		reg_invoke_fr_dlci_class(new_inht_classp, pvc);
	    }
	    if (fr_tr_on_idb(hwidb))
		fr_shape_setup(pvc->idb, pvc, SHAPE_UPDATE);
	}
	i++;
    }
}

/*
 * fr_idb_class_dealloc
 * Free all structures on deencaps
*/
static void fr_idb_class_dealloc (hwidbtype *hwidb)
{
    fr_idb_struct_type *fr_idb;
    int level;
    struct fr_idb_class *idb_classp, *tmp_classp;

    fr_idb = hwidb->frame_relay_stuff;
    level = raise_interrupt_level(NETS_DISABLE);
    idb_classp = fr_idb->fr_idb_classQ;
    while (idb_classp) {
	tmp_classp = idb_classp->next;
	free(idb_classp);
	idb_classp = tmp_classp;
    }
    fr_idb->fr_idb_classQ = NULL;
    reset_interrupt_level(level);
}

/*	
 * fr_idb_class_alloc
 *	Allocated and initialize struct fr_idb_class per swidb
 */
void fr_idb_class_alloc (idbtype *swidb)
{
    hwidbtype *hwidb;
    struct fr_idb_class *idb_classp,
                        *first_idb_classp,
                        **tmp_classp;
    fr_idb_struct_type *fr_idb;
    int level;

    hwidb = swidb->hwptr;
    fr_idb = hwidb->frame_relay_stuff;
    idb_classp = malloc_named(sizeof (struct fr_idb_class), "FR IDB Class");
    
    idb_classp->swidb = swidb;
    idb_classp->next = NULL;
    idb_classp->classp = NULL;
    idb_classp->idb_inht = FR_FIRSTSW_CLASS;
    
    level = raise_interrupt_level(NETS_DISABLE);
    if (!is_first_swidb(swidb)) {
	if ((first_idb_classp = fr_find_idb_class(hwidb->firstsw))) {
	    idb_classp->classp = first_idb_classp->classp;
	} 
    }
	
    tmp_classp = &fr_idb->fr_idb_classQ;
    while (*tmp_classp) {
	tmp_classp = &(*tmp_classp)->next;
    }
    *tmp_classp = idb_classp;
    reset_interrupt_level(level);
}

/*
 * fr_class_zap
 *	zap all the link to a class
 */
void fr_class_zap (mapclass_type *classp)
{
    hwidbtype *hwidb;
    idbtype *swidb;
    list_element *element;

    FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_FR, element, hwidb) {
	if (hwidb->enctype == ET_FRAME_RELAY) {
	    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
		fr_int_class(swidb, classp, FALSE);
	    }
	}
    }
    if (classp->params) {
	reg_invoke_fr_mapclass_remove(classp);
    }
    free(classp->params);
    classp->params = NULL;
}

    
/*
 * For inheritance to work correctly,
 * there are three levels of hierarchies.
 * To remove a class, the order needs to be maintained:
 * level 1: First swidb. (main interface)
 * level 2: subif.
 * level 3. dlci
 */

void fr_int_class (idbtype *swidb,
		   mapclass_type *classp,
		   boolean ADD)
{
    fr_idb_struct_type *fr_idb;
    hwidbtype *hwidb;

    hwidb = swidb->hwptr;
    fr_idb = hwidb->frame_relay_stuff;
    if (is_first_swidb(swidb)) {
	(ADD ?
	 fr_firstidb_class_add(swidb, classp)
	 :
	 fr_firstidb_class_remove(swidb, classp));
    } else {
	(ADD ?
	 fr_subidb_class_add(swidb, classp)
	 :
	 fr_subidb_class_remove(swidb, classp));
    }	
}

/*
 * fr_class_command
 */
void fr_class_command(parseinfo *csb)
{
    fr_idb_struct_type *fr_idb;
    mapclass_type *class_ptr;
    struct fr_idb_class *idb_classp;
    idbtype *swidb;
    hwidbtype *hwidb;
    char *name;
    ushort dlci;
    uint encap;
    
    class_ptr = NULL;
    swidb = csb->interface;

    if (csb->nvgen) {
	switch (csb->which) {
        case FR_INT_CLASS:
	    if ((idb_classp = fr_find_idb_class(swidb))) {
		if (is_first_swidb(swidb) ||
		    (idb_classp->idb_inht == FR_SUBSW_CLASS)) {
		    class_ptr = idb_classp->classp;
		    if (class_ptr) {
			nv_write(TRUE, "%s %s", csb->nv_command,
				 class_ptr->name);
		    }
		}
	    }
	break;
	    
	default:
	}	    
	return;
    }
    
    hwidb = swidb->hwptr;
    GET_FR_IDB(hwidb, fr_idb);
    encap = ET_FRAME_RELAY;
    dlci = csb->line1;
    name = GETOBJ(string, 1);
    if (name && (*name != '\0')) {
	class_ptr = mapclass_lookup(name, encap);
    } else {
	return;
    }

    if (csb->sense && !class_ptr) {
	class_ptr = mapclass_create(name, encap, TRUE);
	
	/*
	 * if map class successfully created, enqueue it
	 * else notify user and return
	 */
	
	if (class_ptr) {
	    enqueue(mapclassQ, class_ptr);
	} else {
	    printf("\n%% Can't create map class");
	    return;
	}
    }
    
    switch (csb->which) {
    case FR_INT_CLASS:
	if (csb->sense) {
	    fr_int_class(swidb, class_ptr, TRUE);
	} else {
	    fr_int_class(swidb, class_ptr, FALSE);
	}
	break;
    case FR_DLCI_CLASS:
	if (csb->sense) {
	    fr_dlci_class_inht(swidb, dlci, class_ptr, ADD_CLASS, FR_VC_CLASS);
	} else {
	    fr_dlci_class_inht(swidb, dlci, class_ptr, REMOVE_CLASS,
			       FR_VC_CLASS);
	}
	break;
    }
}

void fr_class_init (void)
{
    reg_add_mapclass_remove(fr_class_zap, "fr_class_zap");
    reg_add_fr_dlci_class_inht(fr_dlci_class_inht, "fr_dlci_class_inht");
    reg_add_fr_idb_class_alloc(fr_idb_class_alloc, "fr_idb_class_alloc");
    reg_add_fr_idb_uninit(fr_idb_class_dealloc, "fr_idb_class_dealloc");
    reg_add_fr_find_mapclass(fr_find_vc_class, "fr_find_vc_class");
}
