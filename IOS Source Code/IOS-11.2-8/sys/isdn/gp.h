/* $Id: gp.h,v 3.2.58.1 1996/04/30 23:07:20 dclare Exp $
 * $Source: /release/112/cvs/Xsys/isdn/gp.h,v $
 *------------------------------------------------------------------
 * gp.h  Global Parameters Module include file
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: gp.h,v $
 * Revision 3.2.58.1  1996/04/30  23:07:20  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:40:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:13:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/09  20:59:06  rbeach
 * CSCdi32791:  BRI/MBRI interfaces dont allow more than 64 isdn caller
 * entries. Make the isdn caller entries dynamically allocated. Dialer
 * and ISDN call screening now share the same mechanism and code.
 *
 * Revision 2.1  1995/06/07  21:15:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE:       gp.h                                                    
DESCRIPTION:  Global Parameters Module include file                   
VERSION:      3.21                                                    

*************************************************************************
****copyright_c**********************************************************

                         COPYRIGHT STATEMENT

                        Copyright (c) 1991,92
                          by Telenetworks
                        Petaluma, CA   94952
                           (707) 778-8737

All rights reserved. Copying, compilation, modification, distribution
or any other use whatsoever of this material is strictly prohibited
except in accordance with a Software License Agreement with
Telenetworks.

****copyright_c**********************************************************
****history**************************************************************
****history**************************************************************
*************************************************************************

GENERAL NOTES

*************************************************************************
*************************************************************************
*/


/************************************************************************
*                           C O N S T A N T S                           *
*************************************************************************
*/

/* misc constants
*/
#define LEN_LDN      (16+1)                           /* includes null */
/* #define LEN_SPID     (10+1)                           includes null ===*/
#define LEN_SPID     (20+1)                           /* includes null ===*/
#define MAXCALLER_ENTRIES 64 /*===*/
#define MAXCALLER_LEN 25 /*===*/

#define MAX_ANSWER_LEN 50 /*=== Called Party Number and Subaddress*/
/************************************************************************
*               D A T A   T Y P E   D E F I N I T I O N S               *
*************************************************************************
*/
typedef struct
   {
   BOOLEAN  valid;         /* TRUE when the following values are valid */
   BOOLEAN  epsf;                             /* endpoint service flag */
   int      usid;                                    /* user select id */
   int      tid;                                        /* terminal id */
   }
EPID, *EP_ID;

typedef struct
   {
   char     ldn [LEN_LDN];           /* local directory no, null term. */
   char     spid [LEN_SPID];         /* service profile id, null term. */
   }
NUMBER_PARAM;

/* === following structure added */
typedef struct
   {
   char     ldn [LEN_LDN];           /* local directory no, null term. */
   char     spid [LEN_SPID];         /* service profile id, null term. */
   BOOLEAN  DN_Routing;                        /* Route to Host via DN */
   int      Bearer_Type;              /* Voice - 1; Circuit Data - 2;  */
                                                    /* Packet Data -3  */
   BOOLEAN  Bearer_Routing;    /* Incoming calls routed by Bearer Type */
   INT16    Host_ID;                        /* ID of destingation host */
   int      Ces;                                 /* Endpoint Specifier */
   BOOLEAN  Initializing_Terminal;        /* Requires an EID from Net  */
   BOOLEAN  spid_sent;           /* need to know if sent and validated */
   }
NUMBER_PARAM_FIT;

/* === struct for storing called party number and subaddress #s */
typedef struct
   {
   char     answer1 [MAX_ANSWER_LEN];  
   char     answer2 [MAX_ANSWER_LEN];  
   }
ANSWER_NUMBER;

typedef struct
   {
   unsigned long T200;  
   unsigned long N200;  
   BOOLEAN nend_to_end; /* Flag indicating if we act on isdn not end to end */
   int     nend_speed;  /* Speed we use when isdn not end to end received */
   BOOLEAN voice_rate;  /* Flag indicating if we over speech incoming rate */
   int     voice_speed; /* Speed we use when we get an incoming speech call */
   BOOLEAN sending_complete;              /* send sending complete in Setup */
   BOOLEAN twait_disabled;   /* NI1 startup timer, default is enabled */
   }
ISDN_SYS_CONST;


/* store the Calling Party number, this is
 * for Australia only, basic-ts013 switchtype
 */
typedef struct
   {
      char number [LEN_LDN];
   }
CALLING_ID;

/************************************************************************
*                  D A T A   D E C L A R A T I O N S                    *
*************************************************************************
*/

extern NUMBER_PARAM np;                         /* phone number params */
extern EPID epid;                                       /* endpoint id */
extern NUMBER_PARAM_FIT np_fit [MAX_DSL] [ MAX_HOSTS ];	/*===*/


/*
 *  Create the isdn caller array here, statically defined
 *  for now.
 */

CALLING_ID isdn_calling [MAX_DSL];    /* calling party number */

ANSWER_NUMBER isdn_answer [MAX_DSL]; /*===*/
ISDN_SYS_CONST isdn_sys_constants[MAX_DSL];
/************************************************************************
*               F U N C T I O N    D E C L R A T I O N S                *
*************************************************************************
*/
#ifdef 0
PUBLIC BYTE * gpGetLdn    (void);
PUBLIC BOOLEAN gpGetEpid  (int *, int *, int *);
#endif
PUBLIC BYTE * gpGetLdn    (void);
PUBLIC BOOLEAN gpGetEpid  (int * usid, int * tid , int * espf);
PUBLIC void gpBuildHostCb (char *, char *, BOOLEAN * , int *,
                           BOOLEAN *,  INT16 *, int *, BOOLEAN *, int, int);	
