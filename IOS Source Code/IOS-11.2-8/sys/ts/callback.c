/* $Id: callback.c,v 3.9.18.2 1996/03/27 08:33:39 billw Exp $
 * $Source: /release/111/cvs/Xsys/ts/callback.c,v $
 *------------------------------------------------------------------
 * Callback functions
 *
 * April 1995, Shoou Yiu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: callback.c,v $
 * Revision 3.9.18.2  1996/03/27  08:33:39  billw
 * CSCdi49184:  AAA accounting records do not contain IP addr if
 * autoselected
 * Branch: California_branch
 *
 * Revision 3.9.18.1  1996/03/18  22:19:39  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.16.2  1996/03/07  10:57:42  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.16.1  1996/02/20  21:23:18  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/02/08  18:44:55  syiu
 * CSCdi48434:  Async Callback Initiate should drop connection
 *
 * Revision 3.8  1996/02/07  16:16:17  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.7  1996/02/03  04:13:08  tkolar
 * CSCdi47986:  IOS modem integration isnt quite there yet
 * Add a way for the IOS to know if the modem switch has reserved the
 * modem.
 *
 * Revision 3.6  1996/02/01  06:10:55  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1995/12/09  00:43:53  tkolar
 * CSCdi45302:  Need a clean interface between modem hardware and
 * commserver
 * Make it easy to claim a tty for exclusive use by a process, and add a
 * modem_busyout registry.
 *
 * Revision 3.4  1995/11/19  07:13:22  syiu
 * CSCdi43665:  PPP Callback Authen-Ack sometimes wedged in modem
 *
 * Revision 3.3  1995/11/19  06:56:35  syiu
 * CSCdi41498:  Async PPP Callback with autoselect during-login fails
 *
 * Revision 3.2  1995/11/17  18:52:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:38:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.16  1995/10/25  22:56:43  syiu
 * CSCdi42876:  PPP callback via LCP negotiation broken (regress)
 *
 * Revision 2.15  1995/10/25  22:39:58  syiu
 * CSCdi42876:  PPP callback via LCP negotiation broken (regress)
 *
 * Revision 2.14  1995/10/16  22:56:10  snyder
 * CSCdi42258:  spellink errors
 *              lpd.c: printng -> printing
 *              callback.c: scipt -> script
 *
 * Revision 2.13  1995/09/18  19:38:41  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.12  1995/08/04  23:14:21  irfan
 * CSCdi38157:  memory leaks in PPP PAP, CHAP, Callback
 * fix memory leaks in pap_rrequest and chap_rresponse path towards
 * callback. change as many malloc's to malloc_named, as possible.
 *
 * Revision 2.11  1995/07/14  07:03:09  hampton
 * Convert Terminal Server to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi37093]
 *
 * Revision 2.10  1995/06/26 17:38:39  syiu
 * CSCdi36417:  Async callback debug message even when no callback
 * necessary
 *
 * Revision 2.9  1995/06/26  17:13:41  syiu
 * CSCdi36339:  memory leak in async callback
 *
 * Revision 2.8  1995/06/20  04:14:34  ahh
 * CSCdi36095:  Connection routines use msclock
 * Remove more msclock references.  (Should have been in initial commit,
 * but something got screwed up.)
 *
 * Revision 2.7  1995/06/15  22:58:12  syiu
 * CSCdi35686:  Async callback AAA authorization and accounting phase not
 * working
 *
 * Revision 2.6  1995/06/14  15:18:35  anke
 * CSCdi35807:  Yet more spellink errors
 *
 * cls/dlcqllc.c:         Unexepcted -> Unexpected
 * ipc/msg_ipc.c:         availble -> available
 * exec_debug_channel.h:  debuggeries -> debugging
 * filesys/fslib.c:       inconsistant -> inconsistent
 * atm/sscop.c:           initalize -> initialize
 * filesys/fslib_print.c: progresss -> progress
 * ts/callback.c:         unnexpected -> unexpecte
 *
 * Revision 2.5  1995/06/13  18:01:54  syiu
 * CSCdi35778:  Exec callback debug message misleading
 *
 * Revision 2.4  1995/06/13  17:35:35  syiu
 * CSCdi35772:  Async callback exec dialstring prompt missing :
 *
 * Revision 2.3  1995/06/13  16:37:25  syiu
 * CSCdi35685:  Async callback rotary should be in reverse oder
 *
 * Revision 2.2  1995/06/09  13:21:52  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:08:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "subsys.h"
#include "sys_registry.h"
#include "../h/cs_registry.regh"
#include "../h/auth_registry.regh"
#include "interface_private.h"
#include "login.h"
#include "ttysrv.h"
#include "address.h"
#include "packet.h"
#include "config.h"
#include "aaa.h"
#include "../os/async.h"
#include "../ts/if_async.h"
#include "../ui/command.h"
#include "../ts/chat.h"
#include "../ui/debug.h"
#include "../ts/callback.h"
#include "../ts/callback_debug.h"
#include "../wan/ppp.h"
#include "../ts/if_slip.h"
#include "parser.h"
#include "../parser/parser_defs_autoselect.h"
#include "../ts/parser_defs_callback.h"
#include "../ip/ip_addrpool.h"
#include "../ip/ip_registry.h"

/* 
 * Function prototypes in this module 
 */
void callback_init(void);
static void callback_on_same_line(tt_soc *, int, usernametype *);
static forktype callback_process(tt_soc *); 
static boolean callback_dsrBLOCK(tt_soc *);
static boolean callback_nodsrBLOCK(tt_soc *);
static boolean callback_forcewaitBLOCK(tt_soc *);
static boolean callback_interfaceBLOCK(tt_soc *);
static boolean callback_rotary_failover(tt_soc *, int, char *);
boolean callback_enabled(usernametype *, tt_soc *, int);
static void callback_set_tty(usernametype *, tt_soc *, int, int);
static boolean callback_generate_setup(tt_soc *);
static boolean callback_global_enabled(int, tt_soc *);
static boolean callback_line_enabled(int, tt_soc *);
static boolean callback_cleanup_protocol(int, tt_soc *);
static forktype callback_ppp_process(usernametype *);
boolean callback_expired_tty(tt_soc *, boolean);
boolean callback_ppp_start(tt_soc *, usernametype *, char *);
boolean callback_initiate(usernametype *, tt_soc *, int, boolean);
static boolean callback_newscript(tt_soc *, char **, regexp **, char *, char *, int);
static boolean callback_drivescript(tt_soc *, int);
static void callback_accounting(int, tt_soc *, usernametype *);
static char *callback_string(int);

/*
 * External references
 */
extern char *tcperror2string(int); 


/*
 * Callback Initialization 
 */
void callback_init (void)
{

   /*
    * Add some registries
    */
   reg_add_callback_initiate(callback_initiate, "callback_initiate");
   reg_add_callback_expired_tty(callback_expired_tty, "callback_expired_tty");
   reg_add_callback_ppp_start(callback_ppp_start, "callback_ppp_start");
   reg_add_callback_enabled(callback_enabled, "callback_enabled");

   callback_parser_init(); 
   callback_debug_init(); 
}



/*
 * Entry point to initiate callback. If the incoming line is stdio, 
 * user messages (printf) are displayed at appropriate times. 
 * Note that this routine does NOT return to its caller as long as the user
 * callback is required and the target line is deemed available.
 * 
 * Will return:
 * FALSE -callback is not required for this user - caller proceeds as 
 *        normal.
 *  
 * TRUE (logically returns but never gets there) 
 *      - callback cannot be initiated due to unavailable lines etc.
 *        The current process of the incoming tty is terminated anyway 
 *        (since the user was designated a callback user)
 *     OR
 *     -  callback is to be initiated. It does not mean end-to-end 
 *        connection is achieved). 
 *        Actually, when the intention to return is TRUE, the current process
 *        of the incoming tty is either killed (and callback process 
 *        spawned) or is tranformed to the callback process itself (when 
 *        incoming line is the target callback line). In the former case, 
 *        since the current process is killed in this routine, the return to 
 *        caller never actually happens. In the latter case, the callback 
 *        process is eventually killed (kfork or death) - thus, the return to 
 *        caller never actually happens also. 
 */
boolean callback_initiate (usernametype *user, tt_soc *incoming_tty, 
			   int callback_type, boolean discard_info)
{

  tt_soc *target_tty = 0;
  char dialstring[50];
  int line;
  int status = FALSE;

  /*
   * Do some housecleaning if this was the second iteration i.e. the 
   * callback code path comes in here again when callback is effect.
   * We want to clean up the noverify flag as a precautionary measure
   * then.
   */    
  if (incoming_tty->statbits & CALLBACK_IN_PROGRESS)
     incoming_tty->acflags &= ~ACF_CALLBACK_NOVERIFY;

  /*
   * If Callback not required/necessary/supported/enabled, 
   * return FALSE so that login can proceed without callback
   */
  if (!callback_enabled(user, incoming_tty, callback_type)) {
     if (discard_info && user)
        free_usernametype(user);
     /*
      * Display debug message only when it is an error - i.e. 
      * user can callback but there were some complications
      */ 
     if (callback_debug && (user->flags & USER_CALLBACK_DIALSTRING) &&
        !(incoming_tty->statbits & CALLBACK_IN_PROGRESS))
        buginf("\nTTY%t callback disabled or not supported on line for %s", 
               incoming_tty->ttynum, callback_string(callback_type));
     return(FALSE);
  }

  /*
   * Check if callback is enabled for incoming line. If not, the
   * incoming connection fails
   */
  if (!callback_line_enabled(callback_type, incoming_tty)) {
    if (discard_info && user)
        free_usernametype(user);
     PRINTF_IF_ALLOWED("\nCallback line unavailable for callback\n");
     if (callback_debug)
        buginf("\nTTY%t callback unavailable on line for %s", 
               incoming_tty->ttynum, callback_string(callback_type));
     death(incoming_tty);
     return(TRUE);
  }

 
  /*
   * If dialstring is NULL and current thread is not a protocol connection, 
   * prompt for dial string.Otherwise, disallow connection on NULL dial string.
   */
  if (user->dial_string == 0) {
     if (callback_type == ACF_CALLBACK_EXEC) {
        status = (askstring(incoming_tty, dialstring, sizeof dialstring, 
                               "Callback Dialstring: ",30 * ONESEC, 1, FALSE));
	if (status)
           setstring(&user->dial_string, (char *) &dialstring);
        else {
           if (callback_debug)
              buginf("\nTTY%t Callback user %s Dialstring prompt timeout\n", 
                     incoming_tty->ttynum, user->name);
           if (discard_info)
              free_usernametype(user);
           death(incoming_tty);
           return(TRUE);
        }
     } else {
        if (callback_debug)
           buginf("\nTTY%t Callback user %s dialstring NULL and protocol mode illegal\n", incoming_tty->ttynum, user->name); 
        if (discard_info)
              free_usernametype(user);
        death(incoming_tty);
        return(TRUE);
     }
  }

  /*
   * Check whether callback line and rotary are both defined. This may
   * be possible since the AAA server does not check for mutual exclusion.
   * In that case, neither selection is valid. Instead use incoming line 
   * for target.
   */
  if ((user->flags & USER_CALLBACK_LINE) && 
      (user->flags & USER_CALLBACK_ROTARY)) {
     if (callback_debug)
        buginf("\nTTY%t Callback line and rotary is invalid - using same line for outgoing callback", incoming_tty->ttynum);
     user->flags &= ~USER_CALLBACK_LINE;
     user->flags &= ~USER_CALLBACK_ROTARY;
  }

  /*
   * Get the target line - if neither callback line nor rotary is defined
   * the incoming line will be used as the target line.
   * Note that when callback is a rotary, the next line in the rotary is
   * selected in descending order of tty number. That is, the rotary search
   * is opposite of the usual telco rotary search for incoming calls.
   */
  if (user->flags & USER_CALLBACK_LINE)
     target_tty = MODEMS[user->callback_rotary_or_line];
  else if (user->flags & USER_CALLBACK_ROTARY) {
     line = select_rotary(user->callback_rotary_or_line, 
                          NULL, NULL, ROTARY_DESCENDING);
     if ((line > 0) && CALLBACK_SUPPORTED_LINES(MODEMS[line]) &&
         callback_line_enabled(callback_type, MODEMS[line])) {
         /*
          * Use the selected line as the target line
          */
         target_tty = MODEMS[line];
     } else if (incoming_tty->ttynum == (-1*line))
        /* 
         * The selected line is the incoming line itself, which would be 
         * busy. We should use this line
         */
        target_tty = incoming_tty;
     else {
        /*
         * No rotary line available or even defined
         */
        if (callback_debug)
           buginf("\nTTY%t Callback user %s Rotary %d Unavailable for %s\n", 
                   incoming_tty->ttynum, user->name, 
                   user->callback_rotary_or_line, 
                   callback_string(callback_type));
        PRINTF_IF_ALLOWED("\nCallback rotary unavailable for callback\n");
        if (discard_info)
           free_usernametype(user);
        death(incoming_tty);
        return(TRUE);
      }
  } else
     target_tty = incoming_tty;

  /*
   * Special case:
   * If target line == incoming line, the incoming line cannot be killed
   * since we then cannot guarantee the callback process will acquire 
   * the same line before another process does. Hence don't spawn the 
   * callback process. We'll instead transform the current process
   * to the callback process, all while still disconnecting the connection
   */
  if (incoming_tty == target_tty) {

    /* 
     * Reserve the target (incoming) line
     */ 
    incoming_tty->statbits |= DIALOUT;
    incoming_tty->statbits &= ~CARDROP;  

    /*
     * Set callback access flags in incoming tty
     */ 
    callback_set_tty(user, incoming_tty, status, callback_type);

    /*
     * Automatically generate callback script and tty script setup 
     * (if necessary). On system anomalies, exit.
     */
    if (!callback_generate_setup(incoming_tty)) {
       if (callback_debug)
          ttybug(incoming_tty,"Callback script generation unexpected failure");
       PRINTF_IF_ALLOWED("\nCallback script generation unexpected failure\n");
       if (discard_info)
          free_usernametype(user);     
       death(incoming_tty);
       return(TRUE); 
    }

    /*
     * Do callback accounting
     */
    callback_accounting(callback_type, incoming_tty, user);

    /*
     * Trigger the callback on the same line
     */
    callback_on_same_line(incoming_tty, callback_type, user);

    if (discard_info)
       free_usernametype(user);

    return(TRUE);
  } 

  /*
   * Outgoing line is not the same as incoming line.
   * Check to make sure outgoing line has this particular callback
   * supported. If not, punt.
   */
  if (!callback_enabled(user, target_tty, callback_type)) {
     if (discard_info)
        free_usernametype(user);
     if (callback_debug)
        buginf("\nTTY%t callback not supported on line for %s", 
               target_tty->ttynum, callback_string(callback_type));
     return(FALSE);
  }

  /*
   * If target line is already active, punt
   */
  if ((target_tty->statbits & HASPROC) 
       || reg_invoke_modem_hardware_in_use(target_tty)) {
    if (callback_debug)
       buginf("\nTTY%t Callback line unavailable for %s", 
              target_tty->ttynum, callback_string(callback_type));
    PRINTF_IF_ALLOWED("\nCallback line unavailable\n");
    if (discard_info)
       free_usernametype(user);    
    death(incoming_tty);
    return(TRUE);
  } 

  /*
   * Set callback access flags in incoming and target tty
   */ 
  callback_set_tty(NULL, incoming_tty, FALSE, callback_type);
  callback_set_tty(user, target_tty, status, callback_type);

  /*
   * Do callback accounting
   */
  callback_accounting(callback_type, incoming_tty, user);

  /*
   * Propagate the ustruct from incoming to target line
   */
  if (incoming_tty->ustruct) {
     target_tty->ustruct = incoming_tty->ustruct;
     incoming_tty->ustruct = NULL;
  }

  /*
   * Propagate the current IP address from incoming to target line 
   */
  reg_invoke_move_ip_peer_addr(incoming_tty, target_tty);

  /*
   * Automatically generate callback script and tty script setup 
   * (if necessary). On system anomalies, exit.
   */
  if (!callback_generate_setup(target_tty)) {
     if (callback_debug)
        ttybug(incoming_tty, "Callback script generation unexpected failure");
     PRINTF_IF_ALLOWED("\nCallback script generation unexpected failure\n");
     if (discard_info)
        free_usernametype(user);     
     death(incoming_tty);
     return(TRUE); 
  }

  /*
   * Transfer username from incoming tty to the target tty's 
   */
  if (incoming_tty->user) {
     setstring(&target_tty->user, incoming_tty->user);
     free(incoming_tty->user);
     incoming_tty->user = NULL;
  }

  /*
   * If this was ARAP callback, the target tty must know the 
   * ARAP version for incoming call
   */
  if (callback_type == ACF_CALLBACK_ARAP)
     target_tty->arap_version = incoming_tty->arap_version;

  /* 
   * Reserve the target line
   */ 
  CLAIM_TTY(target_tty);
  target_tty->statbits |=  DIALOUT;
  target_tty->statbits &= ~CARDROP;   

  /*
   * For PPP callback, the incoming async line is to be reset.
   * The flag DIALOUT is set so that the async reset will not 
   * kill the line and the process with it. When incoming line 
   * == outgoing line, the DIALOUT flag is already set.
   */
  incoming_tty->statbits |= DIALOUT;

  /*
   * Do a forced wait here that is 1/4 the configured forced wait
   * This allows the modem to send out any data it has before we 
   * drop the connection
   */
  TIMER_START(incoming_tty->callbacktimer, 
              (incoming_tty->callback_forcewaittime/4)*ONESEC);
  edisms((blockproc *)callback_forcewaitBLOCK,(long)incoming_tty);
  if (modemcheck(incoming_tty)) {
     if (callback_debug)
        buginf("\nTTY%t Callback process before disconnect- modemcheck "
               "status=%x", 
               incoming_tty->ttynum, incoming_tty->statbits);
     RELEASE_TTY(target_tty);
     target_tty->statbits &= ~DIALOUT;
     target_tty->statbits |= CARDROP;
     incoming_tty->statbits &= ~DIALOUT;
     death(incoming_tty);
     return(TRUE);
  }

  /*
   * Cleanup any protocol-specific context on incoming line 
   * Reset the target and incoming tty on abnormal return
   */
  if (!callback_cleanup_protocol(callback_type, incoming_tty)){
     RELEASE_TTY(target_tty);
     target_tty->statbits &= ~DIALOUT;
     target_tty->statbits |= CARDROP;
     incoming_tty->statbits &= ~DIALOUT;
     if (discard_info)
        free_usernametype(user);
     death(incoming_tty);
     return(TRUE);
  }

  /*
   * Clear DIALOUT flag on incoming line 
   */  
  incoming_tty->statbits &= ~DIALOUT;
        
  /*
   * Fork the callback process to the target line
   */
  target_tty->tiptop = cfork((forkproc *)callback_process,
			     (long)target_tty, 0, "Callback Process", 
                              target_tty->ttynum);
          
  /*
   * Release target line if process creation fails
   */
  if (target_tty->tiptop == -1) {
     RELEASE_TTY(target_tty);
     target_tty->statbits &= ~DIALOUT;
     target_tty->statbits |= CARDROP;
     if (callback_debug)
        buginf("\nTTY%t Callback process creation failed, user: %s", 
                target_tty->ttynum, target_tty->user);    
     PRINTF_IF_ALLOWED("\nCallback not initiated - line is disconnected\n");
  } else {
     if (callback_debug)
        buginf("\nTTY%t Callback process initiated, user: %s dialstring %s",
               target_tty->ttynum, target_tty->user, target_tty->dial_string);
     PRINTF_IF_ALLOWED("\nCallback initiated - line is disconnected\n");

     /*
      * Drop DTR to prevent modem from answering incoming calls
      */
     tty_dtrstate(target_tty, FALSE); 

  }

  if (discard_info)
     free_usernametype(user);
  death(incoming_tty);
  return(TRUE);
}    


/*
 * Routine to change the current process to the callback process
 * and drive the callback. Used only if the incoming line and the 
 * target line are the same
 */
static void callback_on_same_line (tt_soc *incoming_tty, int callback_type, 
			    usernametype *user)
{

  /*
   * Set Process name
   */ 
  process_set_name("Callback Process"); 

  if (callback_debug)
     buginf("\nTTY%t Callback process initiated, user: %s dialstring %s",
            incoming_tty->ttynum, incoming_tty->user,
	    incoming_tty->dial_string);

  PRINTF_IF_ALLOWED("\nCallback initiated - line is disconnected");

  /*
   * Do a forced wait here that is 1/4 the configured forced wait
   * This allows the modem to send out any data it has before we 
   * drop the connection
   */
  TIMER_START(incoming_tty->callbacktimer, 
              (incoming_tty->callback_forcewaittime/4)*ONESEC);
  edisms((blockproc *)callback_forcewaitBLOCK,(long)incoming_tty);
  if (modemcheck(incoming_tty)) {
     if (callback_debug)
        buginf("\nTTY%t Callback process before disconnect- modemcheck "
               "status=%x", 
               incoming_tty->ttynum, incoming_tty->statbits);
     incoming_tty->statbits &= ~DIALOUT;
     death(incoming_tty);
     return;
  }

  /*
   * Cleanup any protocol-specific context. 
   * On abnormal return, clean up line, and kill the line
   */
  if (!callback_cleanup_protocol(callback_type, incoming_tty)){
     incoming_tty->statbits &= ~DIALOUT;
     death(incoming_tty);
     return; 
  }

  /*
   * Drop DTR to disconnect and prevent modem from answering incoming calls
   */
  tty_dtrstate(incoming_tty, FALSE);   


  /*
   * Since we are reusing the same line for callback, any previous
   * autocommand loaded here should be cleared
   */
  incoming_tty->tty_autocommand = NULL;
  
  /*
   * Clean up user stuff, including accounting, unless it is an exec callback
   * and noverify is required
   */
  if (incoming_tty->ustruct && !((callback_type == ACF_CALLBACK_EXEC) &&
     (user->flags & USER_NOCALLBACK_VERIFY))) {
	aaa_free_user(&incoming_tty->ustruct);
  }

  /*
   * Drive the callback 
   */
  callback_process(incoming_tty);
}  



/* 
 * Process to do callback on the target line.
 * When the callback connection is successful (end-to-end connection seen), 
 * this process dies silently, leaving for linewatch or modem_scanner
 * to pick up where it left off.
 */ 
static forktype callback_process (tt_soc *tty)
{

  int status, line;
  modem_status current;
  char last_expect[CHAT_MAXEXPECT];
  tt_soc *save_tty;
  pid_t pid;
  int count = 0;

  /*
   * Iterate once for non-rotary line
   * For rotary lines, it will iterate to the next line in the same 
   * rotary when the current one fails due to certain modem conditions
   */
  process_get_pid(&pid);
  while (TRUE){
     /*
      * Some sanity checking
      */
     if ((tty->tiptop != pid) || modemcheck(tty)) {
         if (callback_debug)
            buginf("\nTTY%t Callback process conflict pid=%x, modem state=%x",
                   tty->ttynum, tty->tiptop, tty->statbits);
         death(tty); 
         process_kill(THIS_PROCESS);
     }

     /*
      * Give the modems on both ends some breather to drop the connection
      * Block waiting for DSR to go down 
      */
     TIMER_START(tty->callbacktimer, tty->callback_wait_nodsr); 
     edisms((blockproc *)callback_nodsrBLOCK,(long)tty); 
 
     /* 
      * Allright, who caused me to unblock ?
      */
     if (modemcheck(tty)) {
         if (callback_debug)
            buginf("TTY%t Callback process fail - modemcheck status=%x", 
                    tty->ttynum, tty->statbits);
         death(tty);
     } else if (!SLEEPING(tty->callbacktimer)) { 
         if (callback_debug){
            ttybug(tty,"Callback process fail - timeout with DSR up");
	 }
         death(tty);
     } 

     /* 
      * Forced wait here. 
      */
     if (callback_debug)
        buginf("\nTTY%t Callback forced wait = %d seconds", 
                tty->ttynum, tty->callback_forcewaittime);
     TIMER_START(tty->callbacktimer, tty->callback_forcewaittime*ONESEC);
     edisms((blockproc *)callback_forcewaitBLOCK,(long)tty);

     /*
      * Double check line status after wait.
      */
     if (modemcheck(tty)) {
         if (callback_debug)
            buginf("\nTTY%t Callback process fail - modemcheck status=%x", 
                   tty->ttynum, tty->statbits);
         death(tty);
     }

     /*
      * Clean up tty I/O and callback timer
      */
     TIMER_STOP(tty->callbacktimer);
     clearinput(tty);
     clearoutput(tty);
     
     /*
      * Raise DTR 
      */
     tty_dtrstate(tty, TRUE);

     /*
      * Some modems require a pause after DTR high to accept commands
      * To get the modem's attention, use the offhook command. 
      * Thus when the modem does respond, the line is "busied out"
      * Retry a number of times. On failure, carry on (so that rotary 
      * failover can happen)
      */
     while ((callback_drivescript(tty, SCRIPT_OFFHOOK) != OK) &&
            (count++ < CALLBACK_RETRY_ON_DTR)) {
       if (callback_debug)
         ttybug(tty,"Callback process offhook script retry");
     }

     /*
      * Call appropriate chat script. 
      * If callback chat-script for line has problems
      *    Kill the line and process
      * Else
      *    Use that chat-script to do callback 
      */
     if (tty->acflags & ACF_CALLBACK_ARAP)
       status = do_chat_script(tty, tty->scr_arapcallback, 
                      tty->scr_arapcallback_comp,SCRIPT_CALLBACK, last_expect);
     else
       status = do_chat_script(tty, tty->scr_callback, 
                      tty->scr_callback_comp, SCRIPT_CALLBACK, last_expect);
     if (status != OK) {
        if (callback_debug)
           buginf("\nTTY%t Callback process chat script fail status=%s", 
                  tty->ttynum, tcperror2string(status));

        /*
         * For target lines that is a rotary, explore whether callback should
         * skip to the next available one, and attempt callback again
         * Rotary is search bottom up
         */
        if (callback_rotary_failover(tty, status, last_expect) &&
            ((line = select_rotary(tty->rotary, NULL, tty, 
                                   ROTARY_DESCENDING)) > 0) && 
            CALLBACK_SUPPORTED_LINES(MODEMS[line]) && 
            callback_global_enabled((tty->acflags & ACF_CALLBACK_MASK),tty) &&
            callback_line_enabled((tty->acflags & ACF_CALLBACK_MASK),tty)){

           if (callback_debug)
              buginf("\nTTY%t Callback user: %s failover to TTY%t rotary %d", 
                      tty->ttynum, tty->user, line, tty->rotary);

           /*
            * Switch to new rotary line. Let new line assume this 
            * callback info of the previous line, reserve the line 
            * and kill the old line
            */
           save_tty = tty; 
           tty = MODEMS[line];
           tty->acflags = save_tty->acflags;
           reg_invoke_move_ip_peer_addr(save_tty, tty);
           tty->tiptop = save_tty->tiptop;
           save_tty->tiptop = 0;
           setstring(&tty->dial_string, save_tty->dial_string);
           setstring(&tty->user, save_tty->user);
           free(save_tty->user);
           save_tty->user = NULL;
	   /*
	    * Propagate the ustruct from incoming to target line
	    */
	   if (save_tty->ustruct) {
	       tty->ustruct = save_tty->ustruct;
	       save_tty->ustruct = NULL;
	   }

           /*
            * If this was ARAP callback, the new rotary tty must inherit the 
            * ARAP version from previous line
            */
           if (tty->acflags & ACF_CALLBACK_ARAP)
              tty->arap_version = save_tty->arap_version;

           /*
            * Automatically generate callback script and tty script setup 
            * (if necessary). On system anomalies, kill both lines
            */
           if (!callback_generate_setup(tty)) {
              if (callback_debug)
                 ttybug(tty,"Callback script generation unexpected failure");
              death(save_tty);
              death(tty);
	    }
          
           /* 
            * Reserve the new target line
            */ 
	   CLAIM_TTY(tty);
           tty->statbits |= DIALOUT;
           tty->statbits &= ~CARDROP;   

           /*
            * Drop DTR to prevent modem from answering incoming calls
            */
           tty_dtrstate(tty, FALSE); 

           /*
            * Kill the previous rotary line
            */
           death(save_tty);
           continue;  
       }
        
       /*
        * It's a regular target line that failed or if it is a rotary, no 
        * other line is in the same rotary. Kill the line and this process
        * with it
        */
       death(tty); 
     } else {
    
       /*
        * The chat script went thru.
        * Don't trust that the script's CONNECT went through (since this 
        * string is not enforced). Instead, wait for DSR to come up, timeout, 
        * carrier drop etc
        */
       TIMER_START(tty->callbacktimer, CALLBACK_WAIT_DSR);
       edisms((blockproc *)callback_dsrBLOCK,(long)tty); 

       /* 
        * Ahem, who caused me to unblock this time ?
        */
       if (modemcheck(tty)) {
          if (callback_debug)
            buginf("\nTTY%t Callback process fail - modemcheck status=%x", 
                   tty->ttynum,tty->statbits);
            death(tty);
       } else if (!SLEEPING(tty->callbacktimer)) {
            if (callback_debug)
               ttybug(tty,"Callback process fail - timeout with DSR down");
            death(tty);
       } else {
        
            /*
             * Check the modem signals again 
             */
            current = tty_modemsignal(tty);
       
            /*
             * If DSR is up, silently kill this process without hanging up the
             * modem. This allows the standard exec/autoselect to pick up.
             * Always passes if line is not modem-controlled
             */
            if (current.ring || ((tty->capabilities & MODEMMASK) == 0)) {

              /*
               * Prepare the line for succesful exec/autoselect pick up
               */
	      RELEASE_TTY(tty);
	      tty->statbits &= ~DIALOUT;
          
              if (callback_debug) {
	        buginf("\nTTY%t %s Successful - await exec/autoselect pickup",
                      tty->ttynum,
                      callback_string(tty->acflags & ACF_CALLBACK_MASK));
              }

              /*
               * If line is modem-controlled, move the modem state to IDLE
               * (required initial state)
               */
              if (tty->capabilities & MODEMMASK) {
                 tty->statbits &= ~(IDLE + CARDROP + READY + HANGUP);
                 tty->statbits |= IDLE;
              }

              /*
               * Set flag to indicate TTY is now in Callback
               * Set a timer to make it ephemeral
               */ 
              TIMER_START(tty->callbacktimer, CALLBACK_WAIT_PROCESS);
              tty->statbits |= CALLBACK_IN_PROGRESS;

              /*
               * Silently kill process without hanging up the line
               */
              kfork(tty);
            } else { 
              if (callback_debug)
                ttybug(tty,"Callback process fail - DSR down");
              death(tty);
            }
       }   
     }
   } /* end WHILE */
}


/*
 * Block waiting for DSR to come up, timeout, or carrier drops
 * Always unblock if line is not modem-controlled
 */
static boolean callback_dsrBLOCK (tt_soc *tty)
{
  modem_status current;

  if (modemcheck(tty) || AWAKE(tty->callbacktimer) || 
      ((tty->capabilities & MODEMMASK) == 0)) 
    return(FALSE);

  /* 
   * Get the modem signals
   */
  current = tty_modemsignal(tty);
  
  /*
   * Unblock if DSR up
   */
  if (current.ring)
    return(FALSE);
  
  return(TRUE);
}


/*
 * Block waiting for DSR to go down, timeout or carrier drops
 * Always unblock if line is not modem-controlled
 */
static boolean callback_nodsrBLOCK (tt_soc *tty)
{
  modem_status current;

  if (modemcheck(tty) || AWAKE(tty->callbacktimer) || 
      ((tty->capabilities & MODEMMASK) == 0))  
    return(FALSE);

  /* 
   * Get the modem signals
   */
  current = tty_modemsignal(tty);
  
  /*
   * Unblock if DSR down
   */
  if (!(current.ring)) {
    return(FALSE);
  }
  
  return(TRUE);
}


/*
 * Block waiting for forcewait timeout or carrier comes up
 */
static boolean callback_forcewaitBLOCK (tt_soc *tty)
{
  if (modemcheck(tty) || AWAKE(tty->callbacktimer)) 
    return(FALSE);

  return(TRUE);
}


/*
 * Block waiting for interface to go down, timeout or carrier drops
 */
static boolean callback_interfaceBLOCK (tt_soc *tty)
{

  if (AWAKE(tty->callbacktimer))  
    return(FALSE);

  /* 
   * If not in SLIPMODE, awake
   */
  if ((tty->tty_idb) && !(tty->statbits & SLIPMODE)) {
    return(FALSE);
  }

  return(TRUE);
}


/*
 * Check if callback timer has expired. 
 * The reset option allows the callback timer to be reset to 0.
 * It should not used if the caller only intends to poll for 
 * callback expiration.
 */
boolean callback_expired_tty (tt_soc *tty, boolean reset)
{
  /*
   * If Callback allowable timer window has expired, kill the line.
   * That causes the flag to be cleared and the line to be disconnected.
   */
  if (TIMER_RUNNING(tty->callbacktimer) &&
      (tty->statbits & CALLBACK_IN_PROGRESS)) {

    if (!SLEEPING(tty->callbacktimer)) {
       if (callback_debug)
          ttybug(tty, "Callback on line reneged");
       death(tty);
       return(TRUE);
    }

    /*
     * So the timer has not expired yet. Check to see if reset is 
     * requested. If it is, the caller signifies that it has accepted
     * the callback connection and that the timer is to be cleared 
     * Note that tty->statbits still retains CALLBACK_IN_PROGRESS until 
     * the callback session terminates.
     */
    if (reset) {
       if (callback_debug)
          ttybug(tty, "Callback in effect");
       TIMER_STOP(tty->callbacktimer);
    }
  } 
  return(FALSE);
}


/*
 * Decide whether to failover to the next rotary line.
 * The line must be a non-zero rotary to begin with.
 * Note that only certain conditions are designated as worthwhile to 
 * failover to the next rotary line. 
 */  
static boolean callback_rotary_failover (tt_soc *tty, int status,
				  char *last_expect)
{
  /*
   * Must be rotary callback to failover 
   */
  if (!(tty->acflags & ACF_CALLBACK_ROTARY))
    return(FALSE);

  /*
   * If modem status aborts, per chat-script designated e.g. ERROR, BUSY 
   * etc, or if the modem timeout out and the last expected string was
   * OK, the callback must have failed before it even tried a successful
   * connect (well, usually). In these cases, we will consider trying a next 
   * line in the same rotary
   */
  if ((status == ABORTED) ||
      ((status == TIMEDOUT) && (strcmp(last_expect, "OK") == 0)))
    return(TRUE);

  return(FALSE);

}


/*
 * Perform some common callback chores:
 * Transfer the dial-string to the tty, freeing the dialstring 
 * in the usernametype if it was generated for dialstring prompting.
 * Set the necessary callback specific access flags in the tty
 */
static void callback_set_tty (usernametype *user, tt_soc *tty, int status, 
		       int callback_type)
{

  /* 
   * Set up access flags depending on callback type
   */
   tty->acflags |= callback_type;

  /*
   * If user is NULL, the caller does not want usernametype 
   * information to be transfrred to the tty. Return immediately
   */
  if (!user)
     return;

  /*
   * Transfer the dialstring to the target tty 
   */
  setstring(&tty->dial_string, user->dial_string);

  /*
   * Clean up vestigial dial-string if it was prompted 
   */
  if (status) {
	free (user->dial_string);
	user->dial_string = NULL;
  }

  /*
   * Flag the target line as a rotary callback (for callback failover 
   * if necessary)
   */
  if (user->flags & USER_CALLBACK_ROTARY)
     tty->acflags |= ACF_CALLBACK_ROTARY;
     
  /*
   * If callback is noverify, set up access in TTY accordingly.
   * Otherwise, be absolutely certain flag is disabled. 
   */
  if (user->flags & USER_NOCALLBACK_VERIFY)
    tty->acflags |= ACF_CALLBACK_NOVERIFY;
  else
    tty->acflags &= ~ACF_CALLBACK_NOVERIFY;
}


/*
 * Automatically set up the line config to do callback, if necessarry
 * In that case, also set up the callback script, if necessarry
 */
static boolean callback_generate_setup (tt_soc *tty)
{
   char callback_scriptname[] = "callback";
   char arapcallback_scriptname[] = "arap-callback";
   char offhook_scriptname[] = "offhook";
   char callback_script[] = "ABORT ERROR ABORT BUSY \"\" \"ATZ\" OK \"ATDT \\T\" TIMEOUT 30 CONNECT \\c"; 
   char arapcallback_script[] = "ABORT ERROR ABORT BUSY \"\" \"ATZ\" OK \"ATDT \\T\" TIMEOUT 60 CONNECT \\c"; 
   char offhook_script[] = "\"\" \"ATH1\" OK";

   /*
    * Create a off-hook chat script 
    */
   callback_newscript(tty, &tty->scr_offhook,
                      &tty->scr_offhook_comp,
                      offhook_scriptname, offhook_script, SCRIPT_CALLBACK);

   /* 
    * For Exec and PPP callbacks, automatically generate the standard 
    * callback script. 
    * For ARAP, automatically generate an arap callback script. 
    * Currently, this script is the same as the standard callback script. 
    * The script would have to be manually edited with the specific
    * MNP4 ECC disable message since this is modem-specific
    */
   if ((tty->acflags & ACF_CALLBACK_EXEC) || 
       (tty->acflags & ACF_CALLBACK_PPP)){
      if (!callback_newscript(tty, &tty->scr_callback, &tty->scr_callback_comp,
                              callback_scriptname, callback_script,
                              SCRIPT_CALLBACK))    
         return(FALSE);
   } else if (tty->acflags & ACF_CALLBACK_ARAP) {
      if (!callback_newscript(tty, &tty->scr_arapcallback,
                              &tty->scr_arapcallback_comp,
                              arapcallback_scriptname, arapcallback_script,
                              SCRIPT_ARAPCALLBACK))
         return(FALSE);
   } 
   return(TRUE);
}



/*
 * Automatically generate the appropriate callback script, if necessary
 */
static boolean callback_newscript (tt_soc *tty,
			    char **line_scr_callback, 
			    regexp **line_scr_callback_comp, 
			    char *callback_scriptname , 
			    char *callback_script, 
			    int script_type) 
{
   char *script_exp = NULL;
   regexp *script_exp_comp = NULL;
   int status;

   /*
    * If line has callback setup and line's callback script is the 
    * default script, create default callback script, if one does not 
    * exist already. 
    */
   if (*line_scr_callback) {
      if (strcmp(callback_scriptname, *line_scr_callback) == 0) {
          status = chat_script_add_or_delete(TRUE, FALSE, callback_scriptname,
                                             callback_script); 
          if (status == OK) {
             if (callback_debug)
                buginf("\nTTY%t Callback script %s automatically created",
                       tty->ttynum, callback_scriptname);
             /*
              * Special warning message if arap script was generated
              */
             if (script_type == SCRIPT_ARAPCALLBACK)
               ttybug(tty,"ARAP chat-script requires MNP4 disable commands"); 
                
          }
      }
      return(TRUE);
    }


   /*
    * Line does not have callback setup. Set up the line for default
    * callback. Start by getting the script's regular expressions for 
    * that callback name
    */
   if (!get_script(TRUE, callback_scriptname, &script_exp, &script_exp_comp)) {
      return(FALSE);
   }

   /*
    * Normal conditions, this should not fail
    */
   if (!script_exp) 
      return(FALSE);

   /*
    * With the regular expressions of the default scriptname in hand, 
    * set up the line for default callback
    */
   mem_lock(script_exp);
   mem_lock(script_exp_comp);
   *line_scr_callback = script_exp;
   *line_scr_callback_comp = script_exp_comp;
   free(script_exp);
   free(script_exp_comp);
   
   if (callback_debug)
      buginf("\nTTY%t script callback %s automatically added to this line",
              tty->ttynum, callback_scriptname);

   /*
    * Now create the default chat script, if one already does not exist
    */
   status = chat_script_add_or_delete(TRUE, FALSE, callback_scriptname, callback_script);

   /* 
    * Clean up if the script addition somehow failed (other than the script
    * already exist)
    */
   if (status != OK) {
     if  (status == INUSE) {
         if (callback_debug)
          ttybug(tty,"Callback script exists - no script creation necessary"); 
         return(TRUE);
     } else {
         if (callback_debug)
           ttybug(tty, "Callback script creation failure");
         free(*line_scr_callback);
         free(*line_scr_callback_comp);
         *line_scr_callback = NULL;
         *line_scr_callback_comp = NULL;
         return(FALSE);
      }
   }
    
   if (callback_debug)
      buginf("\nTTY%t Callback script %s automatically created", 
             tty->ttynum, callback_scriptname);
             

   /*
    * Special warning message if arap script was generated
    */
   if (script_type == SCRIPT_ARAPCALLBACK)
          ttybug(tty,"ARAP chat-script requires MNP4 disable commands"); 
                
   return(TRUE);   
}


/*
 * Check if callback is enabled - it is disabled due to various reasons
 */
boolean callback_enabled (usernametype *user, tt_soc *tty,
			  int callback_type)
{
  if (!user || !(user->flags & USER_CALLBACK_DIALSTRING) ||
      (tty->statbits & CALLBACK_IN_PROGRESS) ||
      !CALLBACK_SUPPORTED_LINES(tty) ||
      !callback_global_enabled(callback_type, tty)) {
     return(FALSE);
   }
  return(TRUE);
}


/*
 * Check for whether the particular callback type is enabled
 */
static boolean callback_global_enabled (int callback_type, tt_soc *tty)
{
  switch(callback_type) {
    case ACF_CALLBACK_EXEC:
      return(exec_callback_enable);
    case ACF_CALLBACK_PPP:
      /* 
       * PPP callback is set per interface. Hence there are no global
       * flags. However, we'll restrict dedicated interfaces here
       */ 
      return(!(tty->statbits & MUSTSLIP));
      break;
    case ACF_CALLBACK_ARAP:
      return(reg_invoke_arap_callback_enabled() & !(tty->statbits & MUSTARAP));
    default:
     break;
  }
  return(FALSE);
}  



/*
 * Check for whether the particular callback type is enabled for the line
 * and interface.
 */
static boolean callback_line_enabled (int callback_type, tt_soc *tty)
{

  /*
   * If the old access control method
   *   All callback outgoing lines must have login local or tacacs enabled
   *   This prevents lines which go into exec directly from being used for 
   *   outgoing callbacks. Lines with password enabled are also rejected.
   */
  if ( old_access_control && (tty->actype != AC_LOCAL) && 
       (tty->actype != AC_TACACS))
     return(FALSE);

  switch(callback_type) {
    case ACF_CALLBACK_EXEC:
      return(!(tty->capabilities & NOEXEC));
    case ACF_CALLBACK_PPP:
      /* 
       * PPP callback is set per interface. By the time this is called,
       * the interface obviously allowed callback. So we only check that
       * it is not a dedicated async interface, that PPP is autoselected 
       * and the interface is not administratively shutdown
       */ 
      return(tty->tty_idb && !(tty->statbits & MUSTSLIP) && 
             (tty->autoselectmask & PARSER_AUTOSELECT_PPP) &&
             (tty->tty_idb->state != IDBS_ADMINDOWN));
      break;
    case ACF_CALLBACK_ARAP:
      return((tty->autoselectmask & PARSER_AUTOSELECT_ARAP) && 
             !(tty->statbits & MUSTARAP));
    default:
     break;
  }
  return(FALSE);
}  
/*
 * Clean up any protocol specific stuff, if necessary
 */
static boolean callback_cleanup_protocol (int callback_type, tt_soc *tty)
{
  switch(callback_type) {
    case ACF_CALLBACK_EXEC:
        break;
    case ACF_CALLBACK_PPP:
        /* 
         * Reset the interface and wait until the interface is completely
         * gone down. Wait twice the serial pulse timer default
         */
        if (tty->tty_idb) {
          (*(tty->tty_idb->reset))(tty->tty_idb); 

          TIMER_START(tty->callbacktimer, 2*ASYNC_DEFAULT_PULSEINTERVAL);
          edisms((blockproc *)callback_interfaceBLOCK,(long)tty); 
    
          /*
           * On timeout, quit callback since something is wrong
           */
          if (!SLEEPING(tty->callbacktimer)) {
            if (callback_debug) {
              ttybug(tty,"Callback process fail - timeout on interface reset");
              buginf("\nTTY%t Callback process fail statbits=%x", 
                     tty->ttynum, tty->statbits);
	    }
            return(FALSE);
          }
        }
        break;
    case ACF_CALLBACK_ARAP:
        /*
         * Call arap_cleanup via registry. The line is actually not killed.
         * This is crucial when the incoming line is the  same as the outgoing
         * line doing the callback
         */
        if (tty->arapptr)
          reg_invoke_death(tty);          
        break;
    default:
        break;
  }
  return(TRUE);
}


/*
 * Return the message string for the callback type
 */
static char *callback_string (int callback_type)
{
  char *message;

  switch (callback_type) {
  case ACF_CALLBACK_EXEC:
       message = "Exec Callback";
       break;
  case ACF_CALLBACK_PPP:
       message = "PPP Callback";
       break;       
  case ACF_CALLBACK_ARAP:
       message = "ARAP Callback";
       break;       
  default:
       message = "Unknown Callback";
  }
  return(message);
}


/*
 * Will spawn a callback PPP process since there is no process context 
 * for async PPP links 
 */
boolean callback_ppp_start (tt_soc *tty, usernametype *info,
			    char *dialstring)
{

    usernametype *temp_info;
    /*
     * If Callback not required/necessary/supported/enabled, return
     */
    if (!callback_enabled(info, tty, ACF_CALLBACK_PPP)) {
	return(FALSE);
    }

    /*
     * Copy incoming username struct for use in Cback process
     * Callback will free this later.
     */
    temp_info = duplicate_usernametype(info);
    if (!temp_info) {
	return FALSE;
    }

    /*
     * If dialstring specified in PPP negotiation
     *    If user also has dialstring from database (local or TACACS)
     *      use the user-specified one
     *    Else
     *      use dialstring in PPP negotiation
     * Else
     *    If user has dialstring from database (local or TACACS
     *      use the user-specified one
     *    Else
     *      The user had specified a TACACS+/local callback-dialstring 
     *      NULL string. CALLBACK_DIALSTRING was set in the temp_info 
     *      flag field (that's why we got here) and callback-dialstring 
     *      was set to 0. Effectively, all this means is that the user had 
     *      specified a (non-0) NULL callback-dialstring, the PPP client 
     *      did not provide a dialstring _but_ we were enabled for callback 
     *      (ho->neg_callback is TRUE). Callback is illegal. 
     */
    if (dialstring) {
	if (!temp_info->dial_string) {
	    if (callback_debug)
		buginf("\nTTY%t Callback user %s dialstring %s from PPP"
		       " negotiation", tty->ttynum, temp_info->name, dialstring); 
	    setstring(&temp_info->dial_string, dialstring);
	}
    } else if (!temp_info->dial_string) {
	if (callback_debug)
	   buginf("\nTTY%t Callback no dialstring for user %s - disconnect",
                  tty->ttynum, temp_info->name);
        free_usernametype(temp_info);
        death(tty);
	return(FALSE);
    }
    
    /*
     * Set up info to contain the incoming link's idb
     */
    temp_info->callback_incoming_idb = tty->tty_idb;
    
    tty->tiptop = cfork ((forkproc *)callback_ppp_process,
			 (long) temp_info, 0, "Callback PPP", 0);
    if (tty->tiptop == -1) {
	quick_puts(tty,"\r\n%% Unable to start up callback process"
		   "- no memory or too many processes\r\n");
	free_usernametype(temp_info);
	return(FALSE);
    } else {
	return(TRUE);
    }
}


/*
 * Process to drive the PPP callback for async PPP links 
 */
static forktype callback_ppp_process (usernametype *info)
{
   hwidbtype *idb;

   idb = info->callback_incoming_idb;

   if (callback_debug)
      buginf("\nTTY%t Callback PPP process creation", idb->idb_tty->ttynum);

   /*
    * If async interface and link in AUTHENTICATING
    */
   callback_initiate(info, idb->idb_tty, ACF_CALLBACK_PPP, TRUE);
   process_kill(THIS_PROCESS); 
}

/*
 * Callback line commands
 */
void callback_line_command (parseinfo *csb)
{
   tt_soc *tty;
   int i;

   if (system_loading) {
	return;
   }

   /*
    * Take care of NV generation chores
    */
   if (csb->nvgen) {
	switch (csb->which) {
          case PARSER_CALLBACK_FORCEDWAIT:
            tty = MODEMS[csb->line1];
	    nv_write(tty->callback_forcewaittime != CALLBACK_DEFAULT_FORCEDWAIT_SEC,
		     "%s %d", csb->nv_command,
		     tty->callback_forcewaittime);
	    break;
          case PARSER_CALLBACK_NODSRWAIT:
	      tty = MODEMS[csb->line1];
	      nv_write(tty->callback_wait_nodsr != CALLBACK_DEFAULT_WAIT_NODSR,
		       "%s %d", csb->nv_command,tty->callback_wait_nodsr);
	      break;
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
        return;
   }

   	
   switch (csb->which) {
      case PARSER_CALLBACK_FORCEDWAIT:
        for (i = csb->line1; i < csb->line2; i++) {
	    tty = MODEMS[i];
            if (csb->sense) 
              tty->callback_forcewaittime = GETOBJ(int,1);
            else
              tty->callback_forcewaittime = CALLBACK_DEFAULT_FORCEDWAIT_SEC;
	}
        break;     
      case PARSER_CALLBACK_NODSRWAIT:
	  for (i = csb->line1; i < csb->line2; i++) {
	      tty = MODEMS[i];
	      if (csb->sense) 
		  tty->callback_wait_nodsr = GETOBJ(int,1);
	      else
		  tty->callback_wait_nodsr = CALLBACK_DEFAULT_WAIT_NODSR;
	  }
	  break;     
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
   }
}

/*
 * Send accounting callback info 
 */
static void callback_accounting (int callback_type, tt_soc *tty,
			  usernametype *info)
{
   int val;
   acctinfo *acct;

   /*
    * Ignore if old access method or tty ustruct is empty 
    * (should not happen)
    */
   if (old_access_control || !tty->ustruct) 
      return;
 
   switch(callback_type) {
     case ACF_CALLBACK_EXEC:
        /* 
         * Find the accounting queue entry for this user, creating 
         * one if not found
         */
        acct = reg_invoke_aaa_acct_search_create_type(tty->ustruct, 
                                                      ACCT_TYPE_EXEC, TRUE);
        
        /* 
         * If somehow an accounting record is not found or created, return
         */
        if (!acct) 
           return;

        /*
         * Incrementally add accounting AV pairs and send it 
         */
        reg_invoke_aaa_acct_attr_add(acct, AAA_ATTR_SERVICE, AAA_VAL_SVC_shell,
                                     AAA_TYPE_STR, strlen(AAA_VAL_SVC_shell));

        if (info->flags & USER_NOCALLBACK_VERIFY){
           val = 1;
           reg_invoke_aaa_acct_attr_add(acct, AAA_ATTR_CALLBACK_NOVERIFY,
                                        &val, AAA_TYPE_INT, 1);
	}

        if (info->flags & USER_CALLBACK_LINE)
           reg_invoke_aaa_acct_attr_add(acct, AAA_ATTR_CALLBACK_LINE,
                                        &info->callback_rotary_or_line, 
                                        AAA_TYPE_INT, sizeof(int));
        else if (info->flags & USER_CALLBACK_ROTARY)
           reg_invoke_aaa_acct_attr_add(acct, AAA_ATTR_CALLBACK_ROTARY,
                                     &info->callback_rotary_or_line, 
                                     AAA_TYPE_INT, sizeof(int));

        /*
         * For Exec, report as exec accounting
         */
        reg_invoke_aaa_acct_execinfo(tty->ustruct, AAA_ATTR_CALLBACK,
                                     info->dial_string, AAA_TYPE_STR,
                                     strlen(info->dial_string));
        return;

      case ACF_CALLBACK_PPP:
        /* 
         * Find the accounting queue entry for this user, creating
         * one if not found
         */
        acct = reg_invoke_aaa_acct_search_create_type(tty->ustruct,
                                                     ACCT_TYPE_NET, TRUE);
        /* 
         * If somehow an accounting record is not found or created, return
         */
        if (!acct) 
           return;

        reg_invoke_aaa_acct_attr_add(acct, AAA_ATTR_SERVICE,
                                     AAA_VAL_SVC_ppp, AAA_TYPE_STR, 
                                     strlen(AAA_VAL_SVC_ppp));
        break;

      case ACF_CALLBACK_ARAP:
        /* 
         * Find the accounting queue entry for this user, creating
         * one if not found
         */
        acct = reg_invoke_aaa_acct_search_create_type(tty->ustruct, 
                                                      ACCT_TYPE_NET, TRUE);
        /* 
         * If somehow an accounting record is not found or created, return
         */
        if (!acct) 
           return;

        reg_invoke_aaa_acct_attr_add(acct, AAA_ATTR_SERVICE,
                                     AAA_VAL_SVC_arap, AAA_TYPE_STR, 
                                     strlen(AAA_VAL_SVC_arap));
        break;

     default:
        return;
    } 
 
    if (info->flags & USER_CALLBACK_LINE)
        reg_invoke_aaa_acct_attr_add(acct, AAA_ATTR_CALLBACK_LINE,
                                    &info->callback_rotary_or_line, 
                                    AAA_TYPE_INT, sizeof(int));
    else if (info->flags & USER_CALLBACK_ROTARY) 
        reg_invoke_aaa_acct_attr_add(acct, AAA_ATTR_CALLBACK_ROTARY,
                                    &info->callback_rotary_or_line, 
                                    AAA_TYPE_INT, sizeof(int));
    /*
     * For ARAP and PPP, report as network accounting
     */    
    reg_invoke_aaa_acct_netinfo(tty->ustruct, AAA_ATTR_CALLBACK,
                                info->dial_string, AAA_TYPE_STR,
                                strlen(info->dial_string), TRUE);

}


/*
 * Routine to drive specified chat-script
 * Returns status from chat script routine. 
 */
static boolean callback_drivescript (tt_soc *tty, int script_type)
{
  int status = ABORTED;
  char last_expect[CHAT_MAXEXPECT];

  /*
   * Drive the specified chat script type. 
   * Note that SCRIPT_CALLBACK is used to actually call the chat script 
   * routine. This lets the routine avoid any extracurricular activities
   * that callback frowns on
   */
  switch (script_type) {
    case SCRIPT_OFFHOOK:
      status = do_chat_script(tty, tty->scr_offhook, 
                      tty->scr_offhook_comp, SCRIPT_CALLBACK, last_expect);
      break;
  }

  if (status != OK) {
        if (callback_debug)
           buginf("\nTTY%t callback process chat script fail status=%s", 
                  tty->ttynum, tcperror2string(status));

  }
  return(status);
}
