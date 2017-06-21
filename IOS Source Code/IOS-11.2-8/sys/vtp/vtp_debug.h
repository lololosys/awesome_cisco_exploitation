/* $Id: vtp_debug.h,v 1.1.44.4 1996/09/13 21:47:45 thille Exp $
 * $Source: /release/112/cvs/Xsys/vtp/vtp_debug.h,v $
 *------------------------------------------------------------------
 * vtp_debug.h: DEBUG command specific vars. & constants
 *
 * January, 1996,  Percy P. Khabardar
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Longer description here.
 *------------------------------------------------------------------
 * $Log: vtp_debug.h,v $
 * Revision 1.1.44.4  1996/09/13  21:47:45  thille
 * CSCdi69139:  IOS includes in platform independent VTP code.
 * Branch: California_branch
 * Remove the inclusion of IOS include files from source files shared with
 * non-IOS platforms.  Add comments so someone else won't make the same
 * mistake later.  Move the inclusions to platform specific include file
 * vtp_debug.h.
 *
 * Revision 1.1.44.3  1996/08/28  13:20:18  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 1.1.44.2  1996/06/18  00:51:02  cyoung
 * CSCdi60069:  router crashes when writing vtp info to tftp file
 * CSCdi59372:  wrong behavior when conflicting vlan info is received
 * CSCdi59933:  RSP crashes for creating-deleting-recreating vtp-domains
 * CSCdi60044:  7000 image crash
 * CSCdi60394:  [sh vtp trunk] crashes 7000
 * CSCdi60528:  rsp can't change vtp database until someone else does
 * CSCdi60541:  Attempt to delete a vlan crashes image
 * CSCdi60632:  cannot set password for vtp management-domain
 * CSCdi60384:  configuration timestamp is not displayed correctly
 * Plus some Cat5k bug fixes rolled in as well as dependant code clean up.
 * Branch: California_branch
 *
 * Revision 1.1.44.1  1996/05/09  14:48:30  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 1.1.74.1  1996/04/27  07:38:32  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.62.2  1996/04/17  21:56:58  cyoung
 * CSCdi53840:  error message should pop up once the error occurs
 * Use errmsg to provide timely VTP error logging.
 *
 * CORE - retrieve domain/vlan functions modify input parameters instead
 *        of affecting global structure.
 *        Convert macros to functions (change_to_client,
 *        change_to_transparent).
 *        domain/vlan notifications now pass before and after data so that
 *        transistion actions can be done.
 *        rip out duplicate add/changes from the verify/validate
 *        routines.
 *        vtp_vlan_change_notification is called without checking for
 *        changes.
 *        do not change to client as a result of failed vlan change
 *        notification of domain revision number change notification.
 *  DEP - support new function declarations, fix bug where trying to
 *        configure swidb when no more can be created.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.1.62.1  1996/04/08  02:42:22  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.50.1  1996/03/22  09:45:00  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.34.1  1996/03/05  05:37:31  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1.14.1  1996/02/27  21:20:25  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.1.2.5  1996/02/22  04:42:52  percyk
 * remove basic typedefs from core code for portability reasons.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.1.2.4  1996/02/15  02:30:27  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * move platform dependent stuff into interface files.
 *
 * Revision 1.1.2.3  1996/02/14  18:33:12  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * do not allow any changes to default vlans.
 *
 * Revision 1.1.2.2  1996/02/09  21:27:14  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * vtp debug support - part 1.
 *
 * Revision 1.1.2.1  1996/01/23  00:47:16  percyk
 * Branch: LE_Cal_V111_0_16_branch
 * add default vlans at startup time.  add meat to debug files.
 *
 * Revision 1.1  1996/01/23  00:18:32  percyk
 * placeholder files for vtp debugging
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../vtp/msg_vtp.c"		/* Not a typo, see logger.h */

void vtp_log_packet(unsigned char*, unsigned long, char*);
boolean is_vtp_debug_enabled(void);

/* ----- syslog convention ----- 
 * LOG macro common for others 
 * "fmt" must have the following format:
 *      "%s...msg here..." (%s will point to proper prefix in this macro) 
 * "level" must be EMERG,ALERT,CRIT,ERR,WARNING,NOTICE,INFO,DEBUG 
 */

#define VTP_PRUNING_SYSLOG(level, msg, fmt, args...) { \
        errmsg(&msgsym(msg, PRUNING), ## args); }

/*
 * hexdump of sent/rcvd packet
 * x = start of buffer
 * y = byte count
 * z = string denoting input/output e.g. VTP(I) or VTP(O)
 */
#define VTP_LOG_PACKET(x, y, z)    vtp_log_packet((x), (y), (z))

#define VTP_LOG_RUNTIME            if (is_vtp_debug_enabled()) buginf
#define VTP_LOG_CONFIRMATION       if (is_vtp_debug_enabled()) buginf 
#define VTP_LOG_ERROR(x)           errmsg(&msgsym(ERROR, VTP),(x));
#define VTP_LOG_CONFIG_ERROR       if (is_vtp_debug_enabled()) buginf

#define VTP_PRUNING_LOG_WARNING(msg, fmt, args...) \
                VTP_PRUNING_SYSLOG(WARNING, msg, fmt, ## args)
#define VTP_PRUNING_LOG_NOTICE(msg, fmt, args...) \
                VTP_PRUNING_SYSLOG(NOTICE, msg, fmt, ## args)
#define VTP_PRUNING_LOG_ALERT(msg, fmt, args...) \
                VTP_PRUNING_SYSLOG(ALERT, msg, fmt, ## args)
#define VTP_PRUNING_LOG_EMERG(msg, fmt, args...) \
                VTP_PRUNING_SYSLOG(EMERG, msg, fmt, ## args)
#define VTP_PRUNING_LOG_ERR(msg, fmt, args...) \
                VTP_PRUNING_SYSLOG(ERR, msg, fmt, ## args)
#define VTP_PRUNING_LOG_INFO(msg, fmt, args...) \
                if (is_vtp_debug_enabled()) \
                VTP_PRUNING_SYSLOG(INFO, msg, fmt, ## args)

/* LOG macro additional for trace ----- 
 * "fmt" must have the following format:
 *      "%s...msg here..." (%s will point to proper prefix in this macro) 
 * "level" must be EMERG,ALERT,CRIT,ERR,WARNING,NOTICE,INFO,DEBUG,... 
 */
#define VTP_PRUNING_LOG_TRACE(level, fmt, args...) \
                if (is_vtp_debug_enabled()) \
                  buginf(fmt, ## args);
