/* $Id: tn3270s_alert.c,v 3.1.4.3 1996/06/12 19:09:03 pst Exp $
 * $Source: /release/112/cvs/Xsys/dspu/tn3270s_alert.c,v $
 *------------------------------------------------------------------
 * tn3270 server alert handling
 *
 * Feb 1996, Michael Boe
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: tn3270s_alert.c,v $
 * Revision 3.1.4.3  1996/06/12  19:09:03  pst
 * CSCdi60173:  Eliminate cisco definition of ntohl in favor of
 * swap_xx_bits (partial commit part 1)
 * Branch: California_branch
 *
 * Revision 3.1.4.2  1996/05/22  12:31:24  bchan
 * CSCdi56082:  TN3270: remove test scaffolding from IPC SNA alert gen path
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/05/17  10:49:53  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.2  1996/04/25  23:57:17  bchan
 * CSCdi55817:  gs7-s-mz broken
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.2.1  1996/03/21  03:31:19  bchan
 * CSCdi52147:  Add tn3270 server new files to IbuMod_Calif_branch
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1  1996/03/20  07:11:07  bchan
 * *** empty log message ***
 *
 * $Endlog$
 */


#include "master.h"
#include "types.h"
#include "string.h"
#include "interface_private.h"
#include "tn3270s_types.h"

#include "snautil.h"
#include "registry.h"
#include "../dspu/mv_sr.h"
#include "../dspu/snanm_registry.regh"
#include "../hes/tnlogscat.h"          /* log-entry scatter functios    */
#include "subsys.h"

/*
** ***************************************** ALERT ANCILLARY MACROS
** - For each defined alert, include the alert file & define some
** ancillary macros we'll use later in the file.  Yucky but true
** stories.
** */
#include "../snanm/tn3270s_alert1.alth"
#include "../snanm/tn3270s_alert2.alth"

/*
** ***************************************** end alert ancillary macros 
** */

/*
** - table of alerts to generate.  the table is indexed by
** Tn3270s_AlertType: */
struct alert_profile {
  const void* alertTemplatePtr; /* pointer to NMVT template to copy for alert */
  unsigned alertTemplateSize;   /* size of NMVT template                      */
  char* alertDescr;             /* description of alert for function registry */
  service_Tn3270s_Alert_type alertFunc; /* alert handler function             */
};

static void TnAlert1_Generator (Tn3270s_AlertType alertType,
                         struct tn3270s_data*     Tn3270s_data); 
static void TnAlert2_Generator (Tn3270s_AlertType alertType,
                         struct tn3270s_data*     Tn3270s_data); 

/*
** ***************************************** ALERT TABLE & UNION -
** tables and unions: each time you add a POSSIBLE alert, add the
** POSSIBLE table entry & union member here.  The index is the
** alertType (type=Tn3270s_AlertType). See <snnm_registry.regh> for
** details on this.
** */
static struct alert_profile tn3270s_alert_table[] = {
  {&TN3270S1, sizeof(TN3270S1), "TN3270S Alert#1",      TnAlert1_Generator    },
  {&TN3270S2, sizeof(TN3270S2), "TN3270S Alert#2",      TnAlert2_Generator    }
};

/*
** ***************************************** end ALERT TABLE & UNION 
** */

/*
** ***************************************** alert functions
** */



/*======================================================================*
 * set_cip_tn3270s_fail_cause
 *======================================================================*
 *
 * Description:
 *   Set fail cause at detail data (0x82) NMVT
 */

#if 0
void set_cip_tn3270s_fail_cause (byte *sv, const struct tn3270s_data* alert_data)
{
  short sfLen;
  byte *sf;
  char buffer[21+1];

  ForAllSfInSV(sv,sf,sfLen) {
    if (sf[1] == 0x82) {
      switch (sf[3]) {
      case 0x3d:
        sprintf(buffer, "%i %d", alert_data->serverIpAddr, alert_data->serverTcpPortNum);
        memset(&sf[5], 0x40, 21);
        ConvertAsciiToEbcdic(buffer, &sf[5], strlen(buffer));
        break;
      case 0x39:
        sprintf(buffer, "%i %d", alert_data->clientIpAddr, alert_data->clientTcpPortNum);
        memset(&sf[5], 0x40, 21);
        ConvertAsciiToEbcdic(buffer, &sf[5], strlen(buffer));
        break;
      }
    }
  }
}


/*
** - set_cip_tn3270s_param: sets the NMVT alert data appropriately
** from alert data provided in `alert_data' or by this function
** internally via netmgt-rpc calls: */
void set_cip_tn3270s_param (char* buf, const struct tn3270s_data* alert_data)
{
  byte* sv;
  short svLen;

  ForAllSvInMV(((MV*)buf), sv, svLen) {
    switch (sv[1]) {
    case 0x96:                  /* failure cause */
      set_cip_tn3270s_fail_cause(sv, alert_data);
      break;
    }
  }
}
#endif

static void tn3270s_alert_figure_slotport (unsigned char* ebcdic_string, const hwidbtype* idb)
{
  char* ascii_string = ebcdic_string;
  int len = sprintf(ascii_string, "chan%d/2", idb->slot);
  ConvertAsciiToEbcdic(ascii_string, ebcdic_string, len);
  ebcdic_string[len] = 0;
}

static void tn3270s_alert_figure_ip_addrport (unsigned char* ebcdic_string,
                                       unsigned long addr, unsigned short tcp_port_lcl)
{
  char* ascii_string = ebcdic_string;
  const unsigned char* ip_bytes = (unsigned char*)&addr;
  int len = sprintf(ascii_string, "%d.%d.%d.%d:%d",
                    ip_bytes[0], ip_bytes[0], ip_bytes[0], ip_bytes[0],
                    ntohs(tcp_port_lcl));

  ConvertAsciiToEbcdic(ascii_string, ebcdic_string, len);
  ebcdic_string[len] = 0;
}


static void TnAlert1_Generator (Tn3270s_AlertType alert_num, struct tn3270s_data* alert_data)
{
  struct Tn3270S1 alert;
  unsigned long ip_addr_lcl;
  unsigned short tcp_port_lcl;

  memcpy(&alert, &TN3270S1, sizeof(TN3270S1));
  if (NoLuSessionsScatter(&ip_addr_lcl, &tcp_port_lcl, 
                          alert_data->alertVector, alert_data->alertLength) < (unsigned long)-1) {
    buginf("bad data interpretation on alert #%d", alert_num);
    return;
  }
  
  tn3270s_alert_figure_slotport(alert.cardSlotAndPort, alert_data->idb);
  tn3270s_alert_figure_ip_addrport(alert.ipAddrAndPortLcl, ip_addr_lcl, tcp_port_lcl);
  reg_invoke_SendAlertToHost((byte*)&alert, NULL);
}

static void TnAlert2_Generator (Tn3270s_AlertType alert_num, struct tn3270s_data* alert_data)
{
  struct Tn3270S2 alert;

  memcpy(&alert, &TN3270S2, sizeof(TN3270S2));
  
  tn3270s_alert_figure_slotport(alert.cardSlotAndPort, alert_data->idb);
  reg_invoke_SendAlertToHost((byte *)&alert, NULL);
}


static void Tn3270s_Alert_Default (Tn3270s_AlertType alert_num, struct tn3270s_data* alert_data)
{
  buginf("\nTN3270Server TN3270S ALERT Alert%d not supported", alert_num + 1);
}

static void tn3270s_alert_init (subsystype* subsys)
{
  uint i;
  reg_add_default_Tn3270s_Alert(Tn3270s_Alert_Default, "default alert handler");
  for (i = 0; i <  (int)( sizeof(tn3270s_alert_table)/sizeof(tn3270s_alert_table[0])); ++i) {
    reg_add_Tn3270s_Alert(i, tn3270s_alert_table[i].alertFunc,
                          tn3270s_alert_table[i].alertDescr);
  }
}


#define TN3270S_ALERT_MAJVERSION 1
#define TN3270S_ALERT_MINVERSION 0
#define TN3270S_ALERT_EDITVERSION 1
 
SUBSYS_HEADER(tn3270s_alert_hdr,
              TN3270S_ALERT_MAJVERSION, TN3270S_ALERT_MINVERSION, TN3270S_ALERT_EDITVERSION,
              tn3270s_alert_init, SUBSYS_CLASS_MANAGEMENT,
              "req: sna_pu", NULL);

