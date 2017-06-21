/* $Id: svip_dbus_addr.S,v 1.1.6.1 1996/07/08 22:43:03 mwu Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-4k-svip/svip_dbus_addr.S,v $
 *------------------------------------------------------------------
 * svip_dbus_addr.S
 *
 * Mar, 96. Paul Greenfield
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: svip_dbus_addr.S,v $
| Revision 1.1.6.1  1996/07/08  22:43:03  mwu
| CSCdi60922:  11.2 Core Features Commit
| Branch: California_branch
|
| Revision 1.1  1996/03/23  01:22:57  pgreenfi
| Placeholder files for src-4k-svip.
|
 *------------------------------------------------------------------
 * $Endlog$
 */
/* $Id: svip_dbus_addr.S,v 1.1.6.1 1996/07/08 22:43:03 mwu Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-4k-svip/svip_dbus_addr.S,v $
 *------------------------------------------------------------------
 * svip_dbus_addr.S
 *
 * Mar, 96. Paul Greenfield
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: svip_dbus_addr.S,v $
| Revision 1.1.6.1  1996/07/08  22:43:03  mwu
| CSCdi60922:  11.2 Core Features Commit
| Branch: California_branch
|
| Revision 1.1.2.1  1996/03/23  02:11:48  pgreenfi
| CSCdi52430:  VIP ROM initial checkin
| Branch: ELC_branch
|
| Revision 1.1  1996/03/23  01:22:57  pgreenfi
| Placeholder files for src-4k-svip.
|
 *------------------------------------------------------------------
 * $Endlog$
 */


/* this is placed at the beginning of the dbus code and */
/* used by the loadbin routine to load the dbus code */

	j	main
	.word	link_start
	.word	link_end
