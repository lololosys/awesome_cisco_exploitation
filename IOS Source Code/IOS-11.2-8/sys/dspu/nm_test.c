/* $Id: nm_test.c,v 3.6.10.2 1996/05/21 09:46:09 thille Exp $
 * $Source: /release/112/cvs/Xsys/dspu/nm_test.c,v $
 *------------------------------------------------------------------
 * SNA Network management Alert tester.
 *   To be removed later
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nm_test.c,v $
 * Revision 3.6.10.2  1996/05/21  09:46:09  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.10.1  1996/03/18  19:34:05  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.14.3  1996/03/16  06:40:54  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.14.2  1996/03/13  01:20:18  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.14.1  1996/02/20  00:47:28  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/22  14:32:51  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/01  06:02:02  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1995/12/17  18:26:21  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/17  09:05:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:22:54  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:29:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:30:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "subsys.h"
#include "interface_private.h"

#include "xid_reg.h"
#include "sys_registry.h"
#include "../llc2/llc2.h"
#include "../x25/x25.h"
#include "../x25/x25_event.h"
#include "../x25/x25_address.h"
#include "../dspu/snanm_registry.regh"
#include "config.h"
#include "parser.h"
#include "packet.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"

static void TrAlertTest(byte alertNum);
static void SdlcAlertTest(byte alertNum);
static void CsmaAlertTest(byte alertNum);
static void BridgeAlertTest(byte alertNum);
static void LlcAlertTest(byte alertNum);
static void X25AlertTest(byte alertNum);
static void X25lapbAlertTest(byte alertNum);
static void QllcAlertTest(byte alertNum);


static hwidbtype* FindTrIdb(void)
{
    hwidbtype* idb;
    list_element *l_elt;

    FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_TOKEN, l_elt, idb) {
	return(idb);
    }
    return(NULL);
}
static hwidbtype* FindEthIdb(void)
{
    hwidbtype* idb;
    list_element *l_elt;

    FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_ETHER, l_elt, idb) {
	    return(idb);
    }
    return(NULL);
}

extern llctype		*llc_dte_hash_table[LLC_DTE_HASH_SIZE];

static llctype* FindLlc(void)
{
   int i;
   for (i = 0; i < LLC_DTE_HASH_SIZE; i++){
      if (llc_dte_hash_table[i])
          return (llc_dte_hash_table[i]);
   }
   return 0;
}
static qllctype* GetQllc(void)
{
   hwidbtype        *hwidb;
   lcitype          *lci;
   int vc = 0;
   FOR_ALL_HWIDBS(hwidb) {
      if (is_x25(hwidb)){
           while ((lci = x25_lci_lookup_next(hwidb, vc, 0)) != 0){
              if (lci->qllc)
                  return lci->qllc;
              else
                  vc = lci->lci_lci;  /* continue search */
	   }
        }
   }
   return 0;
}
static lcitype* GetLci(void)
{
   hwidbtype        *hwidb;
   lcitype          *lci;
   FOR_ALL_HWIDBS(hwidb) {
       if (is_x25(hwidb) && (lci = x25_lci_lookup_next(hwidb, 0, 0)) != 0)
           return lci;
   }
   return 0;
}
static hwidbtype* GetIdb(void)
{
   hwidbtype        *hwidb;

   FOR_ALL_HWIDBS(hwidb) {
       if (is_x25(hwidb))
           return hwidb;
   }
   return 0;
}

sdlc_data_block  *nmtestsdb;

static sdlc_data_block* FindSdlcSdb(void)
{
   hwidbtype* idb;
   sdlc_data_block  *sdb;
   int i;
   FOR_ALL_HWIDBS(idb){
       if (is_sdlc(idb)){
            for (i = 1; (sdb = idb->sdlc_data[i]) == 0 && i < 256; i++)
                  ;
            return nmtestsdb = sdb;
	 }
    }
    return 0;
}

void  nm_test(byte test)
{
   int i;
   if (test < 51)
       printf("Registration alert not implemented\n");

   else if (test < 70)
      TrAlertTest(test - 51);
   else if (test == 70){
       for (i = 0; i < 14; i++)
            TrAlertTest(i);
   }
   else if (test < 90)
      SdlcAlertTest(test - 71);
   else if (test == 90){
      for (i = 0; i < 16; i++)
         SdlcAlertTest(i);
   }
   else if (test < 110)
      CsmaAlertTest(test - 91);
   else if (test == 110){
      for (i = 0; i < 14; i++)
         CsmaAlertTest(i);
   }
   else if (test < 120)
      BridgeAlertTest(test - 111);
   else if (test == 120){
      for (i = 0; i < 14; i++)
         BridgeAlertTest(i);
         
   }
   else if (test < 140)
      LlcAlertTest(test - 121);

   else if (test < 160)
      X25AlertTest(test - 141);
   else if (test == 160){
      for (i = 0; i < 14; i++)
         X25AlertTest(i);
   }      
   else if (test < 180)
      X25lapbAlertTest(test - 161);
   else if (test == 180){
      for (i = 0; i < 14; i++)
         X25lapbAlertTest(i);
   }
   else if (test < 200)
      QllcAlertTest(test - 181);
   else if (test == 200){
      for (i = 0; i < 10; i++)
          QllcAlertTest(i);
   }
   else {
       printf("51 - 69: Token ring alert 1 to 19\n");
       printf("70:      All token ring alerts\n");
       printf("71 - 89: Sdlc alert 1 to 19\n");
       printf("90:      all sdlc alerts\n");
       printf("91 - 110: CSMA/CD lan, 111-120: bridge lan\n");
       printf("121-140:llc\n");
       printf("141:160: x.25 plc\n");
       printf("156: Send Clear, 7:Reset, 8:Restart,9:diag\nt");
       printf("171:180: x.25 lapb\n");
       printf("181-200:qllc\n");
   }
         
}

hwidbtype* nmtestidb;
static void TrAlertTest(byte alertNum)
{
   hwidbtype* idb = FindTrIdb();
   if (idb == 0){
      printf("no token ring defined\n");
   }
   else {
        nmtestidb = idb;
        idb->lastFailCause = alertNum + 1;    /* force an error */  
       reg_invoke_TR_Alert(alertNum, idb);
   }
}

static void SdlcAlertTest(byte alertNum)
{
   sdlc_data_block* sdb = FindSdlcSdb();
   if (sdb == 0){
      printf("no sdlc defined\n");
      return;
    }
   reg_invoke_Sdlc_Alert(alertNum, sdb);
}
static void CsmaAlertTest(byte alertNum)
{
   hwidbtype* idb = FindEthIdb();
   if (idb == 0){
      printf("no ethernet defined\n");
   }
   else {
        nmtestidb = idb;
        idb->lastFailCause = alertNum + 1;    /* force an error */  
        reg_invoke_Ether_Alert(alertNum, idb);
   }
}

static void BridgeAlertTest(byte alertNum)
{
    printf("bridge lan test not implemented\n");
}

static void LlcAlertTest(byte alertNum)
{
    llctype* llc = FindLlc();
    if (llc == 0)
        printf("no llc defined\n");
    else{
       reg_invoke_Lanllc_Alert(alertNum, llc);
     }
}
static void X25lapbAlertTest(byte alertNum)
{
    lcitype* lci;
    if ((lci = GetLci()) == 0)
        printf("no lci defined\n");
    else{
        reg_invoke_Lapb_Alert(alertNum, lci->lci_idb->hwptr);
     }
}

static void QllcAlertTest(byte alertNum)
{
    qllctype* qllc = GetQllc();
    if (qllc == 0)
        printf("no qllc defined\n");
    else{
       reg_invoke_Qllc_Alert(alertNum, qllc);
     }
}

static void X25AlertTest(byte alertNum)
{
    lcitype* lci = GetLci();
    hwidbtype* idb;

    if (lci == 0) {
        idb = GetIdb();
        reg_invoke_X25_Alert(alertNum, 0, idb, 0);
         
        printf("no lci defined\n");
    }
    else{

       X25_EventData eventData;
       idb = lci->lci_idb->hwptr;
       eventData.diag = 0x11;
       eventData.cause = 0x22;
       eventData.alertType = AlertRestart;
       eventData.gfiLcg = 0;
       eventData.lcn = lci->lci_lci;
       eventData.pti = X25_RESTART;
       if (alertNum < 15)
          reg_invoke_X25_Alert(alertNum, lci, lci->lci_idb->hwptr, (uchar*) &eventData );
       else {
/*
           switch (alertNum){
	    case 15:
                x25_sendcause(idb, lci, X25_CLEAR, 0x15, 0x25, 0);
                break;
	    case 16:
                x25_sendcause(idb, lci, X25_RESET, 0x16, 0x26, 0);
                break;
	    case 17:
                x25_sendcause(idb, lci, X25_RESTART, 0x17, 0x27, 0);
                break;
	    case 18:
                x25_senddiag(idb, 0, 0x18, lci);
                break;
	     }
*/
	}
     }
}


void nmtest_debug_command (parseinfo *csb)
{
   nm_test(GETOBJ(int, 1));
}


/*
 * Parse chains for SNA debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_nmtest.h"
LINK_POINT(nmtest_debug_command, ALTERNATE);
#undef	ALTERNATE

static parser_extension_request nmtest_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(nmtest_debug_command) },
    { PARSE_LIST_END, NULL }
};

static void nmtest_subsys_init (subsystype *subsys)
{
    parser_add_command_list(nmtest_chain_init_table, "nmtest");
}


/*
 * NMTEST subsystem header
 */

#define NMTEST_MAJVERSION 1
#define NMTEST_MINVERSION 0
#define NMTEST_EDITVERSION  1

SUBSYS_HEADER(nmtest, NMTEST_MAJVERSION, NMTEST_MINVERSION, NMTEST_EDITVERSION,
	      nmtest_subsys_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: snanm",
	      "req: snanm");

