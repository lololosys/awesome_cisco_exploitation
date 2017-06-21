/* $Id: pas_diags.h,v 
 *------------------------------------------------------------------
 * pas_diags.h - PA diagnostic menu
 *
 * June 1996, Paulina D. Tran
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 * $Endlog$
 */

#define MSG_HARDWARE     0 /* reserved */
#define MSG_SYSTEM       1
#define MSG_ERROR        2
#define MSG_WARNING      3
#define MSG_STATUS       4
#define MSG_BOARD        5
#define MSG_PROGRAM      6
#define MSG_QUIET        7

#define NUM_MSGTYPES  8
#define MAXLEN_MSG       80

typedef struct bay_info_ {
    char desc[100];
    int (*func)(void);
} bay_info_t;
extern int pas_diag_errmsg (int msgtype, const char *fmt, ...);


