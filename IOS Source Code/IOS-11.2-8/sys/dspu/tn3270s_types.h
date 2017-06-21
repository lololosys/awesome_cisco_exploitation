/* $Id: tn3270s_types.h,v 3.1.4.1 1996/05/17 10:49:56 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dspu/tn3270s_types.h,v $
 *------------------------------------------------------------------
 * tn3270 server alert type
 *
 * Feb 1996, Michael Boe
  *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: tn3270s_types.h,v $
 * Revision 3.1.4.1  1996/05/17  10:49:56  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.1  1996/03/21  03:31:20  bchan
 * CSCdi52147:  Add tn3270 server new files to IbuMod_Calif_branch
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1  1996/03/20  07:10:59  bchan
 * *** empty log message ***
 *
 * $Endlog$
 */

#ifndef __TN3270S_TYPE_H__
#define __TN3270S_TYPE_H__

/*
** This structure provides a decoupling of the CIP subsystem from the
** SNANM subsystem.  If you need new info to flow from the tn3270s to
** the alert generator, update the information here and maybe in
** "../hes/sysmgt_tn3270s.h"(sysmgt_tn3270s_tnsess_event).
**
** Add members as needed.  No particular order is needed; whereever
** minimizes total space (thru alignment) is just fine. 
** */
typedef struct tn3270s_data {
  hwidbtype*            idb;   /* pointer to CIP virtual idb */
  const char*           alertVector; /* alert data (in internal vector form) */
  unsigned short        alertLength; /* length of alert data                 */
} tn3270s_data;

#endif
