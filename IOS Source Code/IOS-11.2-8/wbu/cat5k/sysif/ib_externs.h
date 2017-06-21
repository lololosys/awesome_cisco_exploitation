/* $Id: ib_externs.h,v 1.1.4.1 1996/05/09 15:01:45 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/sysif/ib_externs.h,v $
 *------------------------------------------------------------------
 * ib_externs.h
 *
 * May 1995, jjchen
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ib_externs.h,v $
 * Revision 1.1.4.1  1996/05/09  15:01:45  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:41  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:43:10  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:51  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:42:35  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:49  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.5  1995/10/04  01:07:40  kchristi
 * Fix CVS Endlog directives so that cvs-fix-merge script will work.
 *
 * Revision 1.4  1995/06/27  02:49:55  rahuja
 * Added void * to formal argument declarations.
 *
 * Revision 1.3  1995/05/05  16:26:22  jjchen
 * Added function declaration for ALC boot.
 *
 * Revision 1.2  1995/05/02  00:03:44  jjchen
 * Added CVS header.
 *
*
 *------------------------------------------------------------------
 * $Endlog$
 */

extern
int Ipc_AppRegLink (uInt16,		/* source ID */
		    uInt16,		/* destination ID */
		    uChar,		/* destination card slot */
		    void (*)(void *),	/* Tx ACK callback */
		    void (*)(void *),	/* Tx NAK callback */
		    void (*)(void *),	/* Rx callback */
		    void (*)(void *),   /* pass-through callback */
		    uChar,		/* indicates (fast/slow) MAC to use */
		    uInt32  *,		/* ptr return registered handle */
		    uInt32  *);		/* ptr return MTU size */

extern
int Ipc_AppDeRegister (uInt32);		/* IPC previously registered handle */

extern
int Ipc_AppSend (uInt32,		/* registered handle */
		 uInt16,		/* destination ID */
		 uChar,			/* destination slot */
		 uChar,			/* type of service */
		 uInt32,		/* message length after application header */
		 uChar   *);		/* message buffer (ie. application hdr) */

extern
int Ipc_AppRxDone (uInt32,		/* source ID */
		   uChar   *);		/* ptr message buffer (ie. application hdr) */

extern
int Ipc_rx (uChar *);			/* ptr to the packet */

extern
int Ipc_AppRequest (uChar *);		/* ptr message buffer (ie. DL header) */

extern
int Ipc_AppGetStats (uInt32,		/* registered handle of interest, 0xffffffff for all */
		     IBIPC_Table_T **);	/* ptr stats record array */
extern
uChar * Ipc_AppMsgMalloc (uInt32);	/* number of bytes excluding app/DL headers */

extern
int Ipc_AppMsgFree (uChar *);		/* ptr message buffer */

#ifdef IOS_BOOT

extern
int ipc_boot_dl();

extern
void ipc_dl_init(uChar *);

#endif
