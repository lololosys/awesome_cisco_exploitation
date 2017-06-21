/* $Id: nm_runcmd.c,v 3.5.18.5 1996/09/03 02:11:40 bchan Exp $
 * $Source: /release/112/cvs/Xsys/dspu/nm_runcmd.c,v $
 *------------------------------------------------------------------
 *
 * This module contains entrypoints which create and destroy the network
 * management run command execution process, an entrypoint which
 * is the run command executor, and an entrypoint which is registered
 * with the major vector tx/rx software layer - to receive specific
 * vectors with a specific application id (in this case "CONSOLE").
 *
 * The registered run command receiver routine runs as part of the DSPU
 * process, pulling vectors off the focalpoint queue, and enqueues
 * a run command entry onto the run commnd queue.
 *
 * The run command executor dequeues entries from the run command queue,
 * breaks up run commands that potentially contain multiple commands and
 * submits each one to the parser.
 *
 * The output generated from printf is captured by overwriting the default
 * vputc() function with our own copy. The captured output is written into
 * a major vector and transmitted back across the sscp-pu session, via
 * the major vector tx/rx layer.
 *
 * Note carefully who allocates and de-allocates resources on behalf of
 * RunCmdExecProc; ie CreateRunCmdExecProcess & DestroyRunCmdExecProcess.
 *
 * Extra note: if the RunCommandReceiver must start the run command
 * execution process, and this fails, the sense code will be set
 * appropriately. It will be percolated back to the sscp-pu level,
 * where a negative response will be shipped back to the host.
 *
 * SNA Network Management Product Major Vector API
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nm_runcmd.c,v $
 * Revision 3.5.18.5  1996/09/03  02:11:40  bchan
 * CSCdi67280:  Telnet command from Netview hanging up session
 * Branch: California_branch
 *
 * Revision 3.5.18.4  1996/06/28  23:05:56  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.5.18.3  1996/05/21  09:46:08  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5.18.2  1996/05/17  10:48:54  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.4.3  1996/04/12  03:36:55  bchan
 * CSCdi51685:  Netview Service Point doesnt free VTY lines
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.4.2  1996/04/03  14:03:40  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.5.4.1  1996/03/30  08:10:35  doner
 * CSCdi52657:  IBU modularity - DSPU conversion to use new IOS scheduler
 * Branch: IbuMod_Calif_branch
 * Update to new scheduler interface
 *
 * Revision 3.5.18.1  1996/03/18  19:34:03  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.2  1996/03/07  08:46:02  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.2.1  1996/02/20  00:47:26  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/08  03:19:36  anke
 * CSCdi48405:  Grammar are bad in output
 *
 * Revision 3.4  1996/01/18  22:32:40  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1995/12/17  18:26:20  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:05:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/11/08  20:56:39  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.8  1995/10/16  22:56:48  snyder
 * CSCdi42258:  spellink errors
 *              telent -> telnet
 *
 * Revision 2.7  1995/09/29  07:29:18  bchan
 * CSCdi41275:  Service Point allows for new terminator and misc fixed
 *
 * Revision 2.6  1995/07/04  01:53:09  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.5  1995/06/22  07:57:29  bchan
 * CSCdi35751:  service point runcmd long data handling
 *
 * Revision 2.4  1995/06/13  08:01:23  bchan
 * CSCdi35751:  service point runcmd long data handling
 *
 * Revision 2.3  1995/06/09  13:03:55  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.2  1995/06/09 00:23:59  hampton
 * Remove include of deleted file.  [CSCdi35520]
 *
 * Revision 2.1  1995/06/07 20:30:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */



#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "ttysrv.h"
#include "../os/async.h"
#include "stacks.h"    /* EXEC_STACK_SIZE */
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../ui/exec.h"

#include "mv_datah.h"
#include "mv_parse.h"
#include "nm_runcmd.h"
#include "nm_tty.h"
#include "snautil.h"



#define SelfDefineMessage 0x31
#define TextData 0x1300
#define RUNCMD_BREAKSET   ";?%"
#define NM_EXEC_PROC_NAME "SNA-NM Exec"
#define APPL_ID           "CONSOLE"

# define RUNCMD_BATCHQ_NAME  "runcmd batchQ"
# define RUNCMD_BATCHQ_EVENT 0

static RuncmdReceiver* gRuncmdP;

static void SendReplyRuncmd(tt_soc* tty);
static void reset_privilege(parseinfo* bsc, tt_soc* tty);

#define SaveRuncmdCB(tty,runcmd) (tty->outpak = (paktype*)runcmd)
#define GetRuncmdCB(tty)     ((RuncmdCB*)tty->outpak)


/* Telnet will not be supported in this version until it is examined
* further. The following checked for telnet string
*/

static boolean IsTelnet (const char* s)
{
   char c;
   const char* telnet = "TELNET";
   int i;

   for (i = 0; i < 6 && (c = *s++) != 0 ; i++){
     if (c >= 'a') {
         c = c - 'a' + 'A';   /* convert to upper case */
     }
     if (c != telnet[i])
        return FALSE;      /* didn't  match */
   }
   return i >= 3;      /* at least 3 chars match */
}

/*=====================================================================*
* RunCmdExecProc
*======================================================================*
*
* Description:
*    This process is created when a runcmd is received from the host. 
* It handles block
* data from the runcmdP->batchQ which gets data from the Netview console. 
* It calls batch_input
* to handle all the input line and calls parse_cmd to handle token.
*/



static void
RunCmdExecProc (void)

{

    RuncmdReceiver *runcmdP;  /*Parameter to the process*/
    parseinfo* csb;
    tt_soc* tty;
    int error;
    RuncmdCB* cmdBlk;
    uint savedTimeout;


   if (!process_get_arg_ptr((void**)&runcmdP)) {
       buginf("\nnm_runcmd No arg for RunCmdRxecProc");
       process_kill(THIS_PROCESS);
   }
   process_watch_queue(runcmdP->batchQ, ENABLE, RECURRING); 
   for(;;){
         /*---- Wait until queue non-empty. Then drain queue. */
        process_wait_for_event();  /*Wait for entry to be queued*/
        while ((cmdBlk = (RuncmdCB *) process_dequeue(runcmdP->batchQ)) != 
	       NULL) {

            char* s;
            /*---- Setup csb. */
            tty = runcmdP->tty;         /* For speed only. */
            csb = tty->csb;             /* Ditto. */
            SaveRuncmdCB(tty,cmdBlk);     /* save run command control block */

            csb->batch_buf = csb->line_buffer;
            csb->batch_offset = csb->line_buffer;
            csb->flags = 0;

            s = csb->line_buffer;
            while (cmdBlk->readIndex != cmdBlk->writeIndex)
               *s++ = runcmdP->buffer[cmdBlk->readIndex++];
            if (strchr(RUNCMD_BREAKSET, s[-1]) == NULL)
               *s++ = ';';
            *s = 0;              /* null termination */
            csb->line = csb->line_buffer;
            setup(tty);                 /* Setup output buffer chain. */

            savedTimeout = tty->tty_exectimeout; /* execTimeout can be changed at more */
         
            /*---- Parse all commands within run command. */
            while (batch_input(csb, csb->line, PARSEBUF, csb->break_chars,
				  &csb->lastchar)) {
                if ((*csb->line  || csb->lastchar == '?') && (*csb->line != '!')) {
                    csb->in_help = (csb->lastchar == '?') ? PARSER_HELP :
		                                            PARSER_NO_HELP;
                    if (IsTelnet(csb->line)){       /* telnet command not supported */
                        printf("\ntelnet not supported at service point");
                    }
                    else {
                       error = parse_cmd(csb);
                       if (error != 0) 
                          report_error(csb, error, TRUE);
                       else {
                          /*---- Config command changes these. Urgh! */
                          csb->batch_buf = csb->line_buffer;
		          csb->break_chars = RUNCMD_BREAKSET;
		        }
                        /*---- If "exit" command executed, reset privilege. */
                        if (csb->exit_all) {
                            reset_privilege(csb, tty);
                            csb->exit_all = FALSE;
			}
		    }
		}
                BuildPrompt(csb, tty);
                printf("\n%s\n", csb->prompt);
                runcmdP->interactive_mode = FALSE;
	    }
            /* The more is turned on when 5000 bytes of messages had sent to
             * the host. This will take too long to read and the timeout is
             * disabled.
             */
            tty->tty_exectimeout = savedTimeout;

            SendReplyRuncmd(tty);
	 }
     }
}  /* RunCmdExecProc. */
               


static void
reset_privilege (parseinfo* csb,
		 tt_soc* tty)

{

   csb->priv = PRIV_USER;
   csb->priv_set = 0;
   tty->def_capabilities = 0L;
   tty->capabilities = 0L;
   SET_DEFPRIV(tty->privilege, DEFAULT_PRIV);
   SET_PRIV(tty->privilege, CUR_DEFPRIV(tty->privilege));

}  /* reset_privilege. */

/*=====================================================================*
* SendReplyRuncmd
*======================================================================*
*
* Description:
*    This passed buffers that saved all output character via printf or putc
* to netview console.
*/


void SendReplyRuncmd (tt_soc* tty)
{
static const char* replyMessage[] = {
  "Router has no message to reply to this command",
  "Router has no buffers left, check router buffer usage",
  "Message lost due to buffer overflow",
  "Message lost due to buffer shortage or temporary outage"
};

 
    NmOutbuffer* outBuffer;
    DataHeader* head;
    DataHeader* data;
    byte  replyRuncmd[8];
    MV_SV_RC rc;

    hword messageLen;
    byte  errMessage[80];    /* one of the above message */
    RuncmdCB*  runcmdCB = GetRuncmdCB(tty);

    if (runcmdCB == 0) { 
        /* no correlator or no data */
        outputpending(tty);
        while ((outBuffer = dequeue(&tty->txpakq)) != 0)
            free(outBuffer);
        return;
    }

    tty->tty_length = 0;           /* disable automore */
    head = DataHeader_new();
    head->dataLength = 8;
    head->dataBytes = replyRuncmd;
    head->next = 0;

    if (!outputpending(tty) || tty->overflow){   /* cannot get buffer */

        /* there is no buffer or the replay message is too long. We free up
         * all the buffer and send a reply to the runcmd that there is no
         * buffer.
         */
        const char* s;
        while ((outBuffer = dequeue(&tty->txpakq)) != 0)
            free(outBuffer);
 
        data = DataHeader_new();
        head->next = data;

        s = replyMessage[(tty->overflow)];
        messageLen = strlen(s) + 2;
        errMessage[0] = messageLen;   /* vector length */
        errMessage[1] = 0x31;
        ConvertAsciiToEbcdic(s, &errMessage[2], messageLen - 2);
        data->dataBytes = errMessage;
        data->dataLength = messageLen;
    }
    else{
       data = head;
       outBuffer = tty->txpakq.qhead;
       for (messageLen = 0; outBuffer != 0; outBuffer = outBuffer->next){
          data->next = DataHeader_new();
          data = data->next;
          data->dataBytes = outBuffer->buffer;
          data->dataLength = outBuffer->length;
          messageLen += outBuffer->length;
       }
    }
    replyRuncmd[0] =0;
    replyRuncmd[1] =4;      /* 4 bytes major vector */
    replyRuncmd[2] =0;
    replyRuncmd[3] =0x61;   /* reply runcmd */

    messageLen += 4;        /* 4 bytes 0x1300 major vector */
    replyRuncmd[4] = messageLen >> 8;
    replyRuncmd[5] = messageLen;

    replyRuncmd[6] =0x13;   /* 1300 = test message */
    replyRuncmd[7] =0;
    rc = MV_SendVector(head, runcmdCB->correlatorP,runcmdCB->correlatorLength) ;

    while ((outBuffer = dequeue(&tty->txpakq)) != 0)
        free(outBuffer);
    
    DataHeader_deleteChain(head);       /* remove all data chain */

    if (rc !=  MV_SV_OK){
       buginf("\nSNA: MV_SendVector rc = %x", rc);
       if (tty->overflow == 0 && rc == MV_SV_Overflow){
           tty->overflow = MessageSentFailure;           /* send a short message up if we can */
           SendReplyRuncmd(tty);        /* send a message if we can          */
           return;
       }
    }
    runcmdCB->next = 0;
    enqueue(&gRuncmdP->emptyQueue, runcmdCB);
    SaveRuncmdCB(tty,0);              /* clear runcmdCB */
}
/*=====================================================================*
* Nm_startoutput
*======================================================================*
*
* Description:
*    This is called to start output. It then changed the system to interactive mode.
*/

int Nm_startoutput (tt_soc* tty)
{
   if (!gRuncmdP->interactive_mode){
      SendReplyRuncmd(tty);
      gRuncmdP->interactive_mode = TRUE;      /* change to conversation mode */
   }
   return 1;
}

/*=====================================================================*
* CreateRunCmdExecProcess
*======================================================================*
*
* Description:
*    This subroutine is called when a runcmd is received from the host.
* It creates a process to simulate a vty.
*/

static boolean
CreateRunCmdExecProcess (RuncmdReceiver* runcmdP)

{
   int i;
   parseinfo* csb;

   if (!runcmdP->runcmdRunning){
       runcmdP->batchQ = create_watched_queue(RUNCMD_BATCHQ_NAME,
					      0,
					      RUNCMD_BATCHQ_EVENT);
       queue_init(&runcmdP->interactiveQ, 0);
       queue_init(&runcmdP->emptyQueue, 0);

       for (i = 0; i < MaximumRuncmd; i++){
            runcmdP->runcmdCB[i].next = 0;
            enqueue(&runcmdP->emptyQueue, &runcmdP->runcmdCB[i]); 
	 }
       runcmdP->interactive_mode = FALSE;
       runcmdP->writeIndex = 0;
       runcmdP->correlatorIndex = 0;
       runcmdP->tty = tty_allocate (0, TRUE);  /* Get permanent VTY. */
       if (runcmdP->tty) {
           nm_tty_init(runcmdP->tty);     /* Set tty to appropriate state. */
         
         /*---- Grab a parse buffer & initialize it; before creating process */
         runcmdP->tty->csb = get_csb("RUNCMD");
         csb = runcmdP->tty->csb;
         if (csb) {
            reset_privilege(csb, runcmdP->tty);
            csb->break_chars = RUNCMD_BREAKSET;
            runcmdP->tty->tiptop = process_create(RunCmdExecProc,
						  NM_EXEC_PROC_NAME,
						  LARGE_STACK,
						  PRIO_NORMAL);
	    process_set_arg_ptr(runcmdP->tty->tiptop, runcmdP);
	    process_set_ttysoc(runcmdP->tty->tiptop, runcmdP->tty);
         }
         else{
            tty_reset(runcmdP->tty);             /* Reset run command process tty. */
            tty_deallocate(runcmdP->tty);       /* And take it away. */
	    return FALSE;
         }
          
      }
      else {
         return  FALSE;
      }
   }
   runcmdP->runcmdRunning = TRUE;
   return (TRUE);

}  /* CreateRunCmdExecProcess. */


/*=====================================================================*
* PHReceiveCmd
*======================================================================*
*
* Description:
*    This is the API interface to receive a runcmd from the host
*/

static word
PHReceiveCmd (RuncmdReceiver* this, MV* vector, hword vecLen,
              char* correlator, hword correlatorLength)

{
   MV*   mv = vector;
   RuncmdCB* runcmdCB;
   byte* message = this->buffer;
   
   /* iterate major vector, the first major vector is 0x8061. Then check 
    * major vector 0x1300. We assume there can be multiple 0x31 subvector
    */
   if (!CreateRunCmdExecProcess(this))
       return InsufficientResource;    /* we can not create a process */

   if ((runcmdCB = dequeue(&this->emptyQueue)) == 0)
       return InsufficientResource;    /* we can not create a process */

   runcmdCB->readIndex = this->writeIndex;
   runcmdCB->writeIndex = this->writeIndex;       /* start data */
   
   if (this->correlatorIndex + correlatorLength >= MAX_CORRELATOR_LN)
       this->correlatorIndex = 0;

   runcmdCB->correlatorP = &this->correlator[this->correlatorIndex];
   this->correlatorIndex += correlatorLength;

   memcpy(runcmdCB->correlatorP,  correlator,  correlatorLength);
   runcmdCB->correlatorLength = correlatorLength;
   FOR_ALL_MV {

      if (MV_Key(mv) == TextData || MV_Key(mv) == MV_Runcmd){

         byte*  sv;
         short lenLeft;

         ForAllSvInMV(mv, sv, lenLeft){
          
            if (sv[1] == SelfDefineMessage){

               /* the message can be separated by ';', this is converted into nl */

                byte* s = &sv[2];              /* point to the test message */
                byte len = sv[0] - 2;          /* length of the data */
                sys_ebcdic_to_ascii(s, len);       /* convert the text into ascii */
                while (len-- > 0)
                   message[this->writeIndex++] = *s++;
	     }
         }
      }
   }

    runcmdCB->next = 0;

    /* Pass data to the runcmd process. There are two queues. We handle the 
     * batchQ while the
     * interactive Q is handled by getc which is called by the parse_cmd 
     * after the token is parsed.
     */

    if (this->interactive_mode){
        if (message[this->writeIndex - 1] == ';')       /* check if null line */
            this->writeIndex--;                 /* remove the null */
        /* now add \r as a terminator. In the automore mode, it will treat
         * \r as separate input, so we will remove it here except if ';' is
         * a single character
         */
        if (!this->tty || this->tty->automore_count < (MaxAutomoreLine - 1) ||
              this->writeIndex == runcmdCB->readIndex){  /* must have at least one char */
           message[this->writeIndex++] = '\r';     /* terminated by new line */
        }
        enqueue(&this->interactiveQ, runcmdCB);
    }
    else 
       process_enqueue(this->batchQ, runcmdCB);

    runcmdCB->writeIndex = this->writeIndex;

   return 0;                            /* Ok status. */

}  /* PHReceiveCmd. */

/*=====================================================================*
* DestroyRunCmdExecProcess
*======================================================================*
*
* Description:
*    Destroy run command execution process. This is called when the focalpoint
* PU is inactive.
*/

static void
DestroyRunCmdExecProcess (RuncmdReceiver* this)

{
   if (this->runcmdRunning){
      tt_soc* tty = this->tty;
 
      if (outputpending(tty)){
         char* outBuffer;
         while ((outBuffer = dequeue(&tty->txpakq)) != 0)
            free(outBuffer);
      }

      exit_config_modes(tty->csb); /* Clear configuration mode to prevent
                                      lockout */
      free_csb(&tty->csb);       /* Destroy csb parse structure(s). */
      tty_reset(tty);             /* Reset run command process tty. */
      tty_deallocate (tty);       /* And take it away. */
      process_kill(tty->tiptop);  /* Kill run command process. */
      tty->tiptop = -1;           /* set process kill flag */
      this->runcmdRunning = FALSE;
      delete_watched_queue(&this->batchQ); /*process is no longer there*/
   }

}  /* DestroyRunCmdExecProcess. */


/*----------------------------------------------------------------------------*/
/* ParserHookInit: Called at system startup; via nm_init. */

void
PH_Init (void)

{

   MV_OpRcv_RC rc = MV_SV_OK;

   /*---- Register our PHReceiveCmd function with the major vector tx/rx
          s/ware; to receive and process all vectors w/ application id of
          "CONSOLE". */
   gRuncmdP = MVR_MVReceiver(MV_Runcmd, APPL_ID, PHReceiveCmd, 
                         sizeof(RuncmdReceiver), &rc);   /* read to receive runcmd */

   if (rc != MV_SV_OK)
      buginf("SNA: MV registration failure = rc = %x\n", rc);
   else{
      gRuncmdP->runcmdRunning = FALSE;
      gRuncmdP->fpInactive = DestroyRunCmdExecProcess;
   }
}

/*=====================================================================*
* NmInputPending
*======================================================================*
*
* Description:
*   return true if there is input data.
* This subroutine is called at the time when interative mode begins. This is
* called by the block routine to check if input should be blocked.
*   Note that when current parser command is completed, the interactive_mode
* will change to FALSE. If we changed to interactive, that means we are
* waiting for input, so we pass data up to inform the runcmd that data is
* required. But if we are already in interactive mode and there is no input
* data, then we also need to send the output to ask for more data.
*   
*/

int NmInputPending (tt_soc* tty)
{
   RuncmdCB* runcmdCB;
   if (!gRuncmdP->interactive_mode){
      SendReplyRuncmd(tty);
      gRuncmdP->interactive_mode = TRUE;      /* change to conversation mode */
      return FALSE;                            /* wait for input */
   }
   else if ((runcmdCB = GetRuncmdCB(tty)) != NULL){
      /* we had a handle to reply */
      if (runcmdCB->readIndex == runcmdCB->writeIndex && outputpending(tty)){
          SendReplyRuncmd(tty);
          return FALSE;     /* wait for reply */
      }
      /* check if any more characters */
      return runcmdCB->readIndex != runcmdCB->writeIndex;
   }
   /* now wait for runcmd to send data to us */
   return gRuncmdP->interactiveQ.qhead != 0; 
}


/* Get a character from interactive buffer
*
*/
int GetInteractiveChar (tt_soc* tty)
{
   RuncmdCB* runcmdCB = GetRuncmdCB(tty);

   if (!gRuncmdP->interactive_mode)
      return -1;               /* we are not in interactive_mode */

   if (runcmdCB == 0){
      runcmdCB = dequeue(&gRuncmdP->interactiveQ);
      if (runcmdCB == 0)
          return -1;         /* no characters */
      SaveRuncmdCB(tty,runcmdCB);
   }
   if (runcmdCB->readIndex != runcmdCB->writeIndex)
      return  gRuncmdP->buffer[runcmdCB->readIndex++];
   else
      return -1;     /* no char, terminate the input */
}
 
