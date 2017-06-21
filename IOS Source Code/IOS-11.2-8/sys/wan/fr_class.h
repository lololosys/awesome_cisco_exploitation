/* $Id: fr_class.h,v 3.1.2.1 1996/05/15 05:54:37 fowler Exp $
 * $Source: /release/112/cvs/Xsys/wan/fr_class.h,v $
 *------------------------------------------------------------------
 * Frame Relay Map Class 
 *
 * March 1996, Charles Poe
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fr_class.h,v $
 * Revision 3.1.2.1  1996/05/15  05:54:37  fowler
 * Frame relay VC queuing and rate enforcement
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/26  05:46:58  ccpoe
 * placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#define ADD_CLASS 0x0
#define CHANGE_CLASS 0x1
#define REMOVE_CLASS 0x2

#define FR_NO_CLASS 0x0
#define FR_VC_CLASS 0x1
#define FR_SUBSW_CLASS 0x2
#define FR_FIRSTSW_CLASS 0x3


extern struct fr_idb_class *fr_find_idb_class(idbtype *);
extern void fr_mapclass_nvgen_func(mapclass_type *);
extern void fr_class_zap (mapclass_type *);
extern void fr_idb_class_alloc(idbtype *);
extern void fr_firstidb_class_add (idbtype *, mapclass_type *);
extern void fr_subidb_class_add (idbtype *, mapclass_type *);
extern void fr_int_class (idbtype *, mapclass_type *, boolean);
extern void fr_dlci_class_inht(idbtype *, ushort, mapclass_type *, int, uchar);
extern void fr_class_init(void);

