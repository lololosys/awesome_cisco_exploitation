/* $Id: modemcap_public.h,v 3.3.32.1 1996/08/08 17:56:38 sjackson Exp $
 * $Source: /release/112/cvs/Xsys/ts/modemcap_public.h,v $
 *------------------------------------------------------------------
 * modemcap_public.h -- public information for other parts of the system
 *			wishing to use the modem capabilities database.
 *
 * June 1995, Tim Kolar
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: modemcap_public.h,v $
 * Revision 3.3.32.1  1996/08/08  17:56:38  sjackson
 * CSCdi65288:  Modem autoconfig should always redetect and attempt to
 * lock speed
 * Branch: California_branch
 * Modem locking speed is now configureable which prevents changing of the
 * desired line speed. Configuration is also attempted after every hangup.
 *
 * Revision 3.3  1996/01/12  20:03:25  sjackson
 * CSCdi44704:  modem busyout command is needed
 * Command added as an extension of the line modem autoconfigure
 *
 * Revision 3.2  1995/11/17  18:55:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:39:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:33:24  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/26  01:08:01  tkolar
 * *** empty log message ***
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


extern boolean confmodem_debug;

#define MODEMCAP_ERR            -1
#define PARSER_MODEMCAP_ENTRY   1
#define PARSER_MODEMCAP_EDIT    2
#define MODEMCAP_FACDEFAULTS    3
#define MODEMCAP_AUTOANSWER     4
#define MODEMCAP_CARDETECT      5
#define MODEMCAP_DTR            6
#define MODEMCAP_HARDFLOW       7
#define MODEMCAP_SPEED          8
#define MODEMCAP_BESTERR        10
#define MODEMCAP_BESTCOMP       11
#define MODEMCAP_NOERR          12
#define MODEMCAP_NOCOMP         13
#define MODEMCAP_NOECHO         14
#define MODEMCAP_NORESULT       15
#define MODEMCAP_SOFTFLOW       16
#define MODEMCAP_CALLID         17
#define MODEMCAP_MISC           18
#define MODEMCAP_TEMPLATE       19
#define MODEMCAP_DEFAULT       	20
#define MODEMCAP_ONHOOK         21
#define MODEMCAP_OFFHOOK        22
#define MODEMCAP_DTESPEED       23
#define PARSER_MODEMCAP_SPD     24

#define MODEMCAP_MAXLEN		60

#define DEFAULT_HAYES_MODEM "default"
#define MAX_MODEM_STRING 128

/*
 *  Special stuff for data train
 */
#define DATA_TRAIN_NAME 1
#define DATA_TRAIN_END 2

#define DATA_TRAIN_BASESIZE 256


typedef struct datatrain_ {
    int dt_size;                /* total space available in data_train */
    int dt_curlen;              /* space used in data_train */
    char *data_train;
} dt_type;

dt_type *dt_init(void);
void dt_free(dt_type *);
void dt_dump_train(dt_type *);
void dt_add_record(dt_type *, char *);
void dt_delete_record(dt_type *, char *);
void dt_change_attr(dt_type *, char *, int, char *);
char *dt_find_attr(dt_type *, char *, int);
char *dt_get_first_record(dt_type *);
char *dt_get_next_record(dt_type *, char *);
boolean dt_record_exists(dt_type *, char *);


/*
 *  End of special stuff for data train
 */


void modemcap_command(parseinfo *);
void modemcap_init(void);
void show_modemcap(parseinfo *);
char *modemcap_get_string(char *, char *, int);

extern int modem_talk(tt_soc *, char *, char *, int);
extern modemcap_record_exists (char *modemname);

