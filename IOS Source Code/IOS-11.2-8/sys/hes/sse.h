/* $Id: sse.h,v 3.2.60.4 1996/09/11 17:38:52 snyder Exp $
 * $Source: /release/112/cvs/Xsys/hes/sse.h,v $
 *------------------------------------------------------------------
 * sse.h  -- business declarations for sse.c
 *
 * March 1993 Tony Li
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sse.h,v $
 * Revision 3.2.60.4  1996/09/11  17:38:52  snyder
 * CSCdi68797:  char pointers to char arrays and make const
 *              120 data, 152 image
 * Branch: California_branch
 *
 * Revision 3.2.60.3  1996/08/28  12:46:08  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.60.2  1996/07/30  14:41:42  michellw
 * CSCdi64226:  Support larger max_frame_size (>1516) for LANE
 * Branch: California_branch
 * - The max_frame_size will be set to 1516 for ethernet and 4544 for TR
 *   if mtu is not configured on the sub-interface; otherwise, it will be
 *   set to based on the configured mtu.
 * - The lane module will not come up if the configured mtu is not valid.
 * - Add one more parameter (swidb) to the reg_invoke_media_change_mtu
 *   registry function.
 * - 1577 will come up with the default AIP mtu size (4470) if mtu is not
 *   configured on the sub-interface.
 *
 * Revision 3.2.60.1  1996/03/18  19:43:49  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/01/24  21:48:19  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  09:20:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:42:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/28  21:57:57  gchristy
 * CSCdi41169:  SSE: always issue an errmsg before hanging
 *
 * Revision 2.1  1995/06/07  20:44:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__SSE_H__
#define	__SSE_H__

struct sse_protoinfo_ {
    struct ssetype_ *sseptr;
    sse_tbinfo *tbinfo;			/* Transparent bridging info */
    sse_ipinfo *ipinfo;			/* IP information */
    sse_ipaccessinfo *ipaccessinfo;	/* IP access list information */
    sse_ipxinfo *ipxinfo;		/* IPX information */
    sse_srbinfo *srbinfo;		/* SRB information */
    sse_clnsinfo *clnsinfo;		/* CLNS information */
    sse_atalkinfo *atalkinfo;		/* AppleTalk information */
};

#define BASE_DECIMAL 10
#define BASE_HEX     16

/*
 * Flags to be passed to sse_add()
 */

#define SSE_ADD_INCREMENTAL 	0x1	/* This is an incremental add to an */
					/* existing tree */ 
#define SSE_ADD_COOKIE		0x2	/* There's a cookie to load */

/*
 * Memory sizes (used only by the parser -- everything else is dynamic)
 */
#define SSE_MAX_ADDRESS		65535

extern boolean sse_debug;
extern boolean sse_manager_blocked;
extern const char show_sse_internal_header[];

extern void sse_init_tree(ssetype *sse, int protocol);
extern boolean sse_add(ssetype *sse, int protocol, uint bytecount,
		       void *start, uchar high, idbtype *idb,
		       void *encaps, uint encapslen, uint usage,
		       int enctype, uchar cookie, int flags);  
extern void sse_compile(ssetype *sse, int protocol);
extern boolean sse_delete(ssetype *sse, int protocol, uint bytecount,
			  void *start, uchar high, boolean debug);
extern void sse_purge(ssetype *sse, int protocol);
extern void sse_set_mtu(hwidbtype *hwidb, idbtype *swidb, int delta);

#endif	__SSE_H__
