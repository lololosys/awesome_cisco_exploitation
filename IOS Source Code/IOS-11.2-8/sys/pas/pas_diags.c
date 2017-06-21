/* $Id: pas_diags.c,v 
 *------------------------------------------------------------------
 * pas_diags.c - PA diag menu
 *
 * June 1996, Paulina D. Tran
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 * $Endlog$
 */
/*
 * Include files
 */
#include "stdlib.h"
#include "master.h"
#include "ttysrv.h"
#include "stdarg.h"
#include "interface_private.h"
#include "interface.h"
#include "ctype.h"
#include "pas_diags.h"
#include "subsys.h"
#include "sched.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_priority.h"
#include "if_pas.h"
#include "../pas/pas_id.h"

#define ALTERNATE       NONE
#include "pas_diags_chain.h"
LINK_POINT(diag_pas_commands, ALTERNATE);
#undef  ALTERNATE
 
static parser_extension_request pa_diag_chain_init_table[] = {
     { PARSE_ADD_TEST_PAS_CMD, &pname(diag_pas_commands) },
     { PARSE_LIST_END, NULL }
 };

#define NUM_DIAGS        100
#define NUM_TESTLIST     25
#define NUM_FATAL        20

#define ACTION_SKIP      0
#define ACTION_ABORT     1

#define VERB_REPORT      0
#define VERB_DONT        1

struct test_dbtype_ {
    char name[40];
    char desc[40];
    int (*func)(int bay);
};

struct pa_db_type_ {
    char name[100];
    int (*menu)(int bay);
};

/*
 * Externs
 */
extern parser_extension_request epadtest_chain_init_table[];
extern hwidbtype *epad_gidb[];
extern int epa_frame_diag(int bay);
extern int epa_diag4(int bay);
extern int epa_diag5(int bay);
extern int epa_diag6(int bay);
extern int epa_diag7(int bay);
extern int epa_diag8(int bay);
extern int epa_diag9(int bay);
extern int epa_diag10(int bay);

/*
 * Statics
 */
static void show_fatal (void);
static void show_pas_diags_menu (void);
static void show_help (void);
static void show_verbosity_menu (void);
static int verbosity_menu (void);
static void show_execution_menu (void);
static int execution_menu (void);
static void show_bay_menu (void);
static int bay_menu (void);
static void show_testlist (void);
static void add_testlist (int test_num);
static void del_testlist (void);
static void run_testlist (void);
static void show_epa_menu (void);
static int get_epa_test_num(void);
static int epa_menu (int bay);

/*
 * Globals
 */
static struct test_dbtype_ test_db[NUM_DIAGS] = {
    {"Cisco encryption/decryption test", "", epa_frame_diag},
    {"Cylink Public Key Processor test", "", epa_diag4},
    {"Cylink Real Time Clock test", "", epa_diag5},
    {"Cylink DES CFB8  test", "", epa_diag6},
    {"Cylink DES CFB64  test", "", epa_diag7},
    {"Cylink DES CBC64 test", "", epa_diag8},
    {"Cylink DES ECB test", "", epa_diag9},

};

static struct pa_db_type_ pa_db[PA_NUM_HARDWARE_TYPES] = {
    {"Unsupported", NULL }, /* 0 */
    {"Unsupported", NULL }, /* 1 */
    {"Unsupported", NULL }, /* 2 */
    {"Unsupported", NULL }, /* 3 */
    {"Unsupported", NULL }, /* 4 */
    {"Unsupported", NULL }, /* 5 */
    {"Unsupported", NULL }, /* 6 */
    {"Unsupported", NULL }, /* 7 */
    {"EPA", epa_menu }, /* 8 */
    {"CPA", NULL }, /* 9 */
    {"Unsupported", NULL }, /* 10 */
    {"Unsupported", NULL }, /* 11 */
    {"Unsupported", NULL }, /* 12 */
    {"Unsupported", NULL }, /* 13 */
    {"Unsupported", NULL }, /* 14 */
    {"Unsupported", NULL }, /* 15 */
    {"Unsupported", NULL }, /* 16 */
    {"Unsupported", NULL }, /* 17 */
    {"Unsupported", NULL }, /* 18 */
    {"Unsupported", NULL }, /* 19 */
    {"Unsupported", NULL }, /* 20 */
    {"Unsupported", NULL }, /* 21 */
};

static uint testlist[NUM_TESTLIST];
static uint num_testlist = 0;
static uint verbosity[NUM_MSGTYPES] = {0, 0, 0, 0, 0};
static uchar *verbstr[2] = { "Report", "Don't Report" };
static uchar *msgstr[NUM_MSGTYPES] = { "", "SYSTEM: ", "ERROR: ", 
    "WARNING: ", "STATUS: ", "BOARD: ", "PROGRAM: ", "QUIET: "};
static uint num_loops = 1;
static uint num_loops_err = 1;
static uint err_action = 1;
static uchar *err_action_str[2] = {"Skip to next test", "Abort"};
static uint current_bay = 0;
static uchar fatal_errors[NUM_FATAL][MAXLEN_MSG];
static uint num_fatal = 0;

static char buf[100];
static char buf2[100];

void pas_diags (parseinfo *csb)
{
    boolean quit = FALSE;

    while (!quit) {
        show_pas_diags_menu();
        rdtty(buf,sizeof(buf));

        if (isalpha(buf[0]))
            buf[0] = toupper(buf[0]);

        switch (buf[0]) {
            case '0':
                bay_menu();
                break;
            case '1':
                printf("not impletmented\n");
                break;
            case '2':
                verbosity_menu();
                break;
            case '3':
                execution_menu();
                break;
            case 'Q': 
                quit = TRUE;
                break;
            case 'F':
                show_fatal();
                break;
            case 'T':
            case 'U':
                printf("Already at top level\n");
                break;
            case 'H':
                show_help();
                break;
            default:
                printf("Not an option\n");
                break;
        }
    }
}

void show_fatal (void)
{
    int i;

    for (i=0;i<num_fatal;i++) {
        printf("%s",msgstr[MSG_ERROR]); 
        printf(fatal_errors[i]);
    }
}

void show_pas_diags_menu (void)
{
    printf("\n");
    printf("0 Bay\n");
    printf("1 Board database\n");
    printf("2 Verbosity\n");
    printf("3 Execution Control\n");
    printf("h help\n");
    printf("> ");
}

void show_help (void)
{
    printf("\n");
    printf("*  Q - Quit\n");
    printf("*  T - Top level menu\n");
    printf("*  U - Up one level\n");
    printf("*  C - Clear test list\n");
    printf("*  R - Run tests\n");
    printf("*  A - Add to test list\n");
    printf("*  D - Add default tests to current list\n");
    printf("*  S - Show test list\n");
    printf("*  K - Kill (delete) a test from the testlist\n");
    printf("*  H - Help\n");
    printf("*  F - First Fatal Errors\n");
}

void show_verbosity_menu (void)
{
    printf("\n");
    printf("0 Hardware designator prefix Setting: %s\n",verbstr[verbosity[0]]);
    printf("1 System Verbosity Level Setting: %s\n",verbstr[verbosity[1]]);
    printf("2 Error Verbosity Level Setting: %s\n",verbstr[verbosity[2]]);
    printf("3 Warning Verbosity Level Setting: %s\n",verbstr[verbosity[3]]);
    printf("4 Status Verbosity Level Setting: %s\n",verbstr[verbosity[4]]);
    printf("5 Board Debug Verbosity Level Setting: %s\n",verbstr[verbosity[5]]);
    printf("6 Program Debug Verbosity Level Setting: %s\n",verbstr[verbosity[6]]);
    printf("7 Program Debug Quiet Verbosity Level Setting: %s\n",verbstr[verbosity[7]]);
    printf("h help\n");
    printf("> ");
}

int verbosity_menu (void)
{
    boolean quit = FALSE;
    int top = 0;

    while (!quit && !top) {
        show_verbosity_menu();
        rdtty(buf,sizeof(buf));

        if (isalpha(buf[0]))
            buf[0] = toupper(buf[0]);

        switch (buf[0]) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
                printf("**** 0:Report\n");
                printf("**** 1:Don't Report\n");
                printf("***** Enter value [0,1]: ");
                rdtty(buf2,sizeof(buf2));
                if (buf2[0] == '0' || buf2[0] == '1')
                    verbosity[buf[0] - '0'] = buf2[0] - '0';
                else
                    printf("Not an option\n");
                break;
            case 'F':
                show_fatal();
                break;
            case 'U':
                quit = TRUE;
                break;
            case 'T':
                top = TRUE;
                break;
            case 'H':
                show_help();
                break;
            default:
                printf("Not an option\n");
                break;
        }
    } 
    return(top);
}

void show_execution_menu (void)
{
    printf("\n");
    printf("0 Number of times to loop on Testlist [0=infinite loop]:%d\n",
		num_loops);
    printf("1 Action on Error Setting: %s\n",err_action_str[err_action]);
    printf("2 Number of times to loop after Error [0=infinite loop]:%d\n",
		num_loops_err);
    printf("h help\n");
    printf("> ");
}

int execution_menu (void)
{
    boolean quit = FALSE;
    int top = 0;

    while (!quit && !top) {
        show_execution_menu();
        rdtty(buf,sizeof(buf));

        if (isalpha(buf[0]))
            buf[0] = toupper(buf[0]);

        switch (buf[0]) {
            case '0':
                printf("***** Enter value [0,4M]: ");
                rdtty(buf2,sizeof(buf2));
                num_loops = atoi(buf2);
                break;
            case '1':
                printf("***** Enter value [0,1]: ");
                rdtty(buf2,sizeof(buf2));
                if (buf2[0] == '0' || buf2[0] == '1')
                    err_action = buf2[0] - '0';
                else
                    printf("Not an option\n");
                break;
            case '2':
                printf("***** Enter value [0,4M]: ");
                rdtty(buf2,sizeof(buf2));
                num_loops_err = atoi(buf2);
                break;
            case 'F':
                show_fatal();
                break;
            case 'U':
                quit = TRUE;
                break;
            case 'T':
                top = TRUE;
                break;
            case 'H':
                show_help();
                break;
            default:
                printf("Not an option\n");
                break;
        }
    } 
    return(top);
}

void show_bay_menu (void)
{
    uint pa_type;

    printf("\n");
    pa_type = pas_get_pa_type(0);
    if (pa_type < PA_NUM_HARDWARE_TYPES)
        printf("0 %s\n",pa_db[pa_type].name);
    else
        printf("0 ERROR\n");

    pa_type = pas_get_pa_type(1);
    if (pa_type < PA_NUM_HARDWARE_TYPES)
        printf("1 %s\n",pa_db[pa_type].name);
    else
        printf("1 ERROR\n");

    printf("h help\n");
    printf("> ");
}

int bay_menu (void)
{
    boolean quit = FALSE;
    int top = 0;
    uint pa_type;

    while (!quit && !top) {
        show_bay_menu();
        rdtty(buf,sizeof(buf));

        if (isalpha(buf[0]))
            buf[0] = toupper(buf[0]);

        switch (buf[0]) {
            case '0':
            case '1':
                current_bay = buf[0] - '0';
                pa_type = pas_get_pa_type(current_bay);
                if (pa_type < PA_NUM_HARDWARE_TYPES && pa_db[pa_type].menu)
                    top = pa_db[pa_type].menu(current_bay);
                break;
            case 'F':
                show_fatal();
                break;
            case 'U':
                quit = TRUE;
                break;
            case 'T':
                top = TRUE;
                break;
            case 'H':
                show_help();
                break;
            default:
                printf("Not an option\n");
                break;
        }
    } 
    return(top);
}

void show_testlist (void)
{
    int i;

    for (i=0;i<num_testlist;i++) {
        printf("%d %s - %s\n",i,
		test_db[testlist[i]].name, test_db[testlist[i]].desc);
    }
}

void add_testlist (int test_num)
{
    if (num_testlist < NUM_TESTLIST) {
        testlist[num_testlist] = test_num;
        num_testlist++;
        printf("%s added to test list\n",test_db[test_num].name);
    } else
        printf("Test list full\n");
}

void del_testlist (void)
{
    if (num_testlist) {
        num_testlist--;
        printf("Last test list entry deleted\n");
    } else
        printf("Test list empty\n");
}

void run_testlist (void)
{
    int t,c;

    num_fatal = 0;
    for (c=0;!num_loops || c<num_loops;c++) {
        for (t=0;t<num_testlist;t++) {
            if (!test_db[testlist[t]].func(current_bay)) {
                if (err_action == ACTION_ABORT)
                    goto DONE;
            }
        }
    }
DONE:
    printf("Test list run complete\n");
    return;
}

int pas_diag_errmsg (int msgtype, const char *fmt, ...)
{
    va_list ap;
    int ret=0; 

    va_start(ap, fmt);
    if (msgtype && msgtype<NUM_MSGTYPES) {
        if (verbosity[msgtype] == VERB_REPORT) {
            ttyprintf(CONTTY,"%s",msgstr[msgtype]); 
            ret = _doprnt(NULL, CONTTY, fmt, ap, FALSE);
        }
        if (msgtype == MSG_ERROR && num_fatal < NUM_FATAL) {
            vsprintf(fatal_errors[num_fatal],fmt,ap);
            num_fatal++;
        }
    }
    return(ret);
    va_end(ap);
}

/*****************************************************************************
 *
 * From here on down are port adapter specific menus   
 *
 *****************************************************************************/

/*****************************************************************************
 * EPA tests 
 *****************************************************************************/
static void show_epa_menu (void)
{
    printf("\n");
    printf("0 Cisco Encryption/Decryption test\n");
    printf("1 Cylink Public Key Processor test\n");
    printf("2 Cylink Real Time Clock test\n");
    printf("3 Cylink DES CFB8 test\n");
    printf("4 Cylink DES CFB64 test\n");
    printf("5 Cylink DES CBC64 test\n");
    printf("6 Cylink DES ECB test\n");
    printf("h help\n");
    printf("> ");
}

static int get_epa_test_num(void)
{
    int test_num = -1;

    printf("***** Which test [0-6]: ");
    rdtty(buf2,sizeof(buf2));
    if (atoi(buf2)<0 || atoi(buf2)>6)
        return(-1);
    switch (atoi(buf2)) {
      case 0:
	test_num = 0;
	break;
      case 1:
	test_num = 1;
	break;
      case 2:
	test_num = 2;
	break;
      case 3:
	test_num = 3;
	break;
      case 4:
	test_num = 4;
	break;
      case 5:
	test_num = 5;
	break;
      case 6:
	test_num = 6;
	break;
    }
    return(test_num);
}

static int epa_menu (int bay)
{
    boolean quit = FALSE;
    int test_num = 0;
    int top = 0;

    while (!quit && !top) {
        show_epa_menu();
        rdtty(buf,sizeof(buf));

        if (isalpha(buf[0]))
            buf[0] = toupper(buf[0]);

        switch (buf[0]) {
            case '0':
                epa_frame_diag(bay);
                printf("Test run complete\n");
                break;
            case '1':
		epa_diag4(bay);
                printf("Test run complete\n");
		break;
	    case '2':
		epa_diag5(bay);
                printf("Test run complete\n");
		break;
	    case '3':
		epa_diag6(bay);
                printf("Test run complete\n");
		break;
	    case '4':
		epa_diag7(bay);
                printf("Test run complete\n");
		break;
	    case '5':
		epa_diag8(bay);
                printf("Test run complete\n");
		break;
	    case '6':
                epa_diag9(bay);
                printf("Test run complete\n");
		break;
            case 'D':
                add_testlist(0);
                add_testlist(1);
                add_testlist(2);
                add_testlist(3);
                add_testlist(4);
                add_testlist(5);
                add_testlist(6);
                break; 
            case 'C':
                num_testlist = 0;
                printf("Test list cleared\n");
                break;
            case 'R':
                run_testlist();
                break;
            case 'A':
                test_num = get_epa_test_num();
                if (test_num == -1)
                    break;
                add_testlist(test_num);
                break;
            case 'S':
                show_testlist();
                break;
            case 'K':
                del_testlist();
                break;
            case 'F':
                show_fatal();
                break;
            case 'U':
                quit = TRUE;
                break;
            case 'T':
                top = TRUE;
                break;
            case 'H':
                show_help();
                break;
            default:
                printf("Not an option\n");
                break;
        }
    } 
    return(top);
}

/*
 * diag_init
 * Initialize functions specific to a diag image
 */
void pas_diag_subsys_init (subsystype *subsys)
{
    parser_add_command_list(pa_diag_chain_init_table, "diag pas");
    if (epad_gidb[0] != NULL && epad_gidb[1] != NULL) {
        parser_add_command_list(epadtest_chain_init_table, "epadtest");
    }
}
 
/*
 * PAS DIAG subsystem header
 */
 
#define PAS_DIAG_MAJVERSION 1
#define PAS_DIAG_MINVERSION 0
#define PAS_DIAG_EDITVERSION  1
 
SUBSYS_HEADER(pas_diag, PAS_DIAG_MAJVERSION, PAS_DIAG_MINVERSION, PAS_DIAG_EDITVERSION,
              /*pas_diag_subsys_init, SUBSYS_CLASS_PROTOCOL,*/
              pas_diag_subsys_init, SUBSYS_CLASS_PROTOCOL,
              NULL,
              NULL);
 
/* end of file */
