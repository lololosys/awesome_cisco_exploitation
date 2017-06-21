/* $Id: cpai.h,v 3.2.62.2 1996/06/12 19:56:58 jng Exp $
 * $Source: /release/112/cvs/Xsys/src-36-c1000/cpai.h,v $
 *------------------------------------------------------------------
 * CPAI ASIC 5:1 interface routines shared include file.
 *
 * April 1995, Henniger
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cpai.h,v $
 * Revision 3.2.62.2  1996/06/12  19:56:58  jng
 * CSCdi60260:  Async catwoman commit
 * Branch: California_branch
 * Async Catwoman commit into California
 *
 * Revision 3.2.62.1  1996/03/18  21:57:23  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/01/24  22:50:09  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  18:40:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:19:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:26:37  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:52:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __CPAI_H__
#define __CPAI_H__


/*----------------------------------------------------------------------*/
/* External CPAI Interface Definitions                                  */
/*----------------------------------------------------------------------*/
/* The CPAI interface routines abstract a software based register       */
/* that is platform independent.  It is 16 bits and has this format:    */
/*   [ 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00 ]                */
/*     [ RSVD ] L  I  N  S  D  [[[[ID REGISTER]]]]  D                   */
/*              O  N  O  Y  C                       C                   */ 
/*              O  V  T  N  C                       E                   */
/*              P  R  X  C                                              */
/*              B  T  C                                                 */
/*----------------------------------------------------------------------*/

#define CPAI_LOOP             0x1000  /* loopback mode enable           */
#define CPAI_INVERT           0x0800  /* invert incoming clock          */
#define CPAI_NO_TXCE          0x0400  /* if we want not TXCE pres/ignore*/
#define CPAI_SYNC             0x0200  /* turns off Rx data synchronizer */
#define CPAI_DCD_INVERT       0x0100  /* invert DCD if DTR=1            */
#define CPAI_MODES_MASK       CPAI_LOOP | CPAI_INVERT | CPAI_NO_TXCE | \
                              CPAI_SYNC | CPAI_DCD_INVERT
#define CPAI_IDREG_MASK       0x00fe  /* ID Register (cable type)       */
#define CPAI_DCE_MODE         0x0001  /* Cable is a DCE cable (/DTE')   */


#define CPAI_ID_NONE          0x0000  /* if no cable then 0x0001        */
#define CPAI_ID_232           0x0002  /* EIA232      DCE or DTE cable   */
#define CPAI_ID_V35           0x0004  /* V.35        DCE or DTE cable   */
#define CPAI_ID_449           0x0006  /* EIA 449     DCE or DTE cable   */
#define CPAI_ID_X21           0x0008  /* X.21        DCE or DTE cable   */
#define CPAI_ID_530           0x000e  /* EIA530      DCE or DTE cable   */
#define CPAI_ID_UNKNOWN       0x00fe  /* if not one of the above        */

typedef enum cable_type_t {
            NO_CABLE,
            DTE_RS232,
            DTE_RS449,
            DTE_RS530,
            DTE_V35,
            DTE_X21,

            DCE_RS232,
            DCE_RS449,
            DCE_RS530,
            DCE_V35,
            DCE_X21,
            UNKNOWN_CABLE
} cable_type_t;


inline boolean cable_present(void);
cable_type_t cable_type(void);
boolean      cable_dte(void);
boolean      dcd_on(void);
boolean      cts_on(void);
boolean      dsr_on(void);
boolean      rts_on(void);
boolean      dtr_on(void);


void         dtr_assert(boolean);
void         rts_assert(boolean);
void         c1000_show_cable_state(hwidbtype *idb);
void         c1000_show_serial_state(hwidbtype* idb);


inline void cpai_set_loop(boolean value);   
inline void cpai_set_txc_invert(boolean value);   
inline void cpai_set_no_txce(boolean value);   
inline void cpai_set_sync(boolean value);   
inline void cpai_set_dcd_invert(boolean value);   

       void    cpai_init(void);
inline boolean cpai_get_loop(void);            
inline boolean cpai_get_txc_invert(void);            
inline boolean cpai_get_no_txce(void);            
inline boolean cpai_get_sync(void);            
inline boolean cpai_get_dcd_invert(void);            
#endif
