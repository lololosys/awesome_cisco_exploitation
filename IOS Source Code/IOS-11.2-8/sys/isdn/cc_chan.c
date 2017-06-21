/* $Id: cc_chan.c,v 3.2.58.2 1996/05/01 14:51:07 hrobison Exp $
 * $Source: /release/112/cvs/Xsys/isdn/cc_chan.c,v $
 *------------------------------------------------------------------
 * cc_chan.c. ISDN Layer 3 Call Controller Channel Management Module
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: cc_chan.c,v $
 * Revision 3.2.58.2  1996/05/01  14:51:07  hrobison
 * CSCdi46413:  MIP when used as PRI must be in slot 0-4
 * Branch: California_branch
 * Change PRI initialization: for the 7xxx platform the
 * dsl id is now assigned independently of the MIP card
 * slot/subunit location.  The number of PRI interfaces
 * per router is still limited to 10.  The dsl assignment
 * for the 4xxx platform is based on the slot location
 * and corrected to allow only 1 dsl id per slot.
 *
 * Read bug release notes for details on how the ISDN
 * debug output and ISDN show commands have changed.
 * The debug output now uses the hw_short_namestring
 * rather than PRI<dsl #> or BRI<dsl #>.  The show commands
 * now take the interface name or the dsl id as input.
 *
 * Revision 3.2.58.1  1996/04/30  23:06:32  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:38:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:11:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/16  17:54:38  dclare
 * CSCdi40762:  PRI connected to DMS100 incorrectly puts B channels out of
 * service. A Restart and a shutdown of the interface will put channels
 * back into in-service.
 *
 * Revision 2.2  1995/06/23  15:14:06  dclare
 * CSCdi36325:  Australian ISDN PRI implementation for the 7000. Also get
 * rid of some homologation defines that became too awkward to use. These
 * are the final changes found during homologation.
 *
 * Revision 2.1  1995/06/07  21:12:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE:       cc_chan.c                                               
DESCRIPTION:  ISDN Layer 3 Call Controller Channel Management Module  
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


This module is intended as an "off-the-shelf" B-Channel management
utility package for use by and incorporation into a Call Control task.

ORGANIZATION OF FUNCTIONS

The functions in this module are arranged alphabetically.

*************************************************************************
*************************************************************************
*/


#define     CC_CHAN_C                                   /* module name */

/************************************************************************
*                 E X T E R N A L   R E F E R E N C E S                 *
*************************************************************************
*/
#include    "master.h"
#include    "../ui/debug.h"
#include    "../wan/isdn_debug.h"
#include    "../wan/isdn.h"

#include    "flags.h"
#include    "gendef.h"
#include    "tune.h"
#include    "lif.h"
#include    "cc_chan.h"
#include    "ccie.h"
/* """
#include    <stdio.h>
""" */

/************************************************************************
*                           C O N S T A N T S                           *
*************************************************************************
*/

/* Error reporting definitions.
*/
#define CC_CHAN_NAME                   "cc_chan"   /* The module name. */

#ifdef DISPLAY_LOCAL_ERR        /* Burning a little memory for strings */
/* The names of the functions calling LIF_Error.
*/
#define GET_IDLE_CHAN             "CC_CHAN_GetIdleChan"
#define GET_IDLE_DMS_CHAN         "CC_CHAN_GetIdleDMSChan"
#define GET_PROTO                 "CC_CHAN_GetProtocol"
#define GET_SERV_STATE            "CC_CHAN_Get_ServState"
#define GET_STATE                 "CC_CHAN_GetState"
#define GET_TIMER_ID              "CC_CHAN_GetTimerID"
#define GET_USER                  "CC_CHAN_GetUser"
#define INIT_TABLE                "CC_CHAN_InitTable"
#define RELEASE_CHAN              "CC_CHAN_ReleaseChan"
#define SET_PROTO                 "CC_CHAN_SetProtocol"
#define SET_SERV_STATE            "CC_CHAN_SetServState"
#define SET_STATE                 "CC_CHAN_SetState"
#define SET_TIMER_ID              "CC_CHAN_SetTimerID"
#define SET_USER                  "CC_CHAN_SetUser"

/* The errors reported.
*/
#define BAD_CHAN_ERR              "Bad channel number"
#define BAD_TYPE_ERR              "Bad channel type"

#else
/* The names of the functions calling LIF_Error
*  (NULL strings to save memory).
*/
#define GET_IDLE_CHAN             ""
#define GET_PROTO                 ""
#define GET_SERV_STATE            ""
#define GET_STATE                 ""
#define GET_USER                  ""
#define SET_PROTO                 ""
#define SET_SERV_STATE            ""
#define SET_STATE                 ""
#define SET_USER                  ""
#define RELEASE_CHAN              ""
#define INIT_TABLE                ""

/* The errors reported.
*/
#define BAD_CHAN_ERR              ""
#define BAD_TYPE_ERR              ""
#endif

/************************************************************************
*               D A T A   T Y P E   D E F I N I T I O N S               *
*************************************************************************
*/
struct ChanData              /* The data retained for a single channel */
    {
    BYTE Channel_State;                                 /* Usage state */
    BYTE Protocol;                   /* Protocol in use on the channel */
    int Using_Call_ID;        /* Call_ID of the call using the channel */
    BYTE Service_State;                  /* Service (maintenance) state */
    int TimerID;                  /* Service message and restart timer */
    };

/************************************************************************
*                              M A C R O S                              *
*************************************************************************
*/
#ifdef DISPLAY_LOCAL_ERR
#define LIF_ERROR(m,f,e)    LIF_Error( (m), (f), (e) )
#else
#define LIF_ERROR(m,f,e)                                        /* NOP */
#endif

/************************************************************************
*                     D A T A   A L L O C A T I O N                     *
*************************************************************************
*/

/* Reserve the Channel Management Table.  The 0th channel element is
*  not used (for simplicity of access), thus the table is sized to
*  MAX_CHANNEL + 1.  Interface ID is a dimension of the table, but it 
*  always defaults to zero for BRI.
*/
PRIVATE struct ChanData ChanMgmtTable[MAX_DSL][MAX_INT_IDP+1][MAX_CHANNELP+1]; 

/************************************************************************
*                F U N C T I O N   D E F I N I T I O N S                *
*************************************************************************
*/
/* All the functions are PUBLIC access routines and are declared in
*  cc_chan.h.
*/

/************************************************************************
*                          F U N C T I O N S                            *
*************************************************************************
*/

/*************************
* CC_CHAN_Set_Chan_State *
**************************
*
* This routine will set the state of the channels to what is defined by
* the user. the bitfield is a 32-bit int, showing if the channels are
* to be enabled. A 0xFFFFFE00 will indicate that channels 1-23 are available,
* but channels 24 - 32 are disabled. The dchan flag passed in describes if
* the D-channel on the interface should be reserved for signalling.
* Australia will actually use B1-B30, NET5 uses B1-B15, B17-B31.
*/
PUBLIC void CC_CHAN_Set_Chan_State (bitfield, dsl, dchan, dchan_val)
int     bitfield;
int     dsl;
boolean dchan;
int     dchan_val;
{
    int i;
    ulong lower16, upper16;

    /*
     * Australia E1 is different from NET5 E1. Australia uses B1-B30, but
     * NET5 uses B1-B15, B17-B31. So have to shift the upper 16B's left one.
     * This will make sure the correct number of B-channels are set in service
     * and prevent Australia from trying to use B31. Also make sure that the
     * dchan flag is cleared if you have more than 16 B-channels configured.
     */
    if(SwitchType == PRI_TS014_STYPE) {
        upper16 = bitfield & 0x0000fffe;
        lower16 = bitfield & 0xffff0000;
        if (upper16) {
            upper16 = upper16 << 1;
            bitfield = lower16 | upper16;
            dchan = FALSE; /* at least 16 B-channels defined */
        }
    }

    for(i=1; i <= MAX_CHANNELP; i++) {
        if(bitfield & (1 << (32 - i))) {
            CC_CHAN_SetStatepri( MIN_INT_IDP, i, B_CHANNEL_UNITS,
                CHANNEL_IDLE, dsl);
        } else {
            /*
             * The channel is unavailable for usage.
             */
            CC_CHAN_SetStatepri( MIN_INT_IDP, i, B_CHANNEL_UNITS,
                CHANNEL_RESERVED, dsl);
        }
    }

    /*
     * Mark the D-channel reserved for signalling.
     */
    if(dchan)
        CC_CHAN_SetStatepri( MIN_INT_IDP, dchan_val, B_CHANNEL_UNITS,
            CHANNEL_RESERVED, dsl);
    /*
     * Mark the 0th channel as Reserved, it is not used for a channel,
     * but to show that a channel has been defined in the interface.
     */
    if (bitfield)
        ChanMgmtTable[dsl][MIN_INT_IDP][0].Channel_State = CHANNEL_RESERVED;
}

/**********************
* CC_CHAN_Show_Status *
***********************
*
* Used to show the state and service value for the ISDN PRI
* channels. If a -1 is passed then all the defined interfaces 
* are shown, otherwise only the specified interface is shown.
*/
PUBLIC void CC_CHAN_Show_Status (dsl )
int dsl;
{
    int dsl_id;
    int intfc;
    int bchan;
    int start;
    int range;

    /*
     * Let's look at the channel states for pri
     */
    if((dsl < 0) || (dsl >= MAX_DSL)) {
        start = 0;
        range = MAX_DSL; 
    } else {
        start = dsl;
        range = dsl + 1;
    }
    automore_enable(NULL);
    printf("\nPRI Channel Statistics:");
    for(dsl_id = start; dsl_id < range; dsl_id++) {
	for(intfc = MIN_INT_IDP; intfc <= MAX_INT_IDP; intfc++) {
	   /*
	    * Only dump out the interfaces that have been defined. This is known
	    * by a value of 3 kept in element 0 of the array, normally not used.
	    */
           if (isdn_idblist[dsl_id] != NULL) {
		if((ChanMgmtTable[dsl_id][intfc][0].Channel_State == CHANNEL_RESERVED) || (dsl > 0)) { 
	    	printf("\nISDN %s, Channel (1-31)", DSL_TO_NAME(dsl_id));
	    	printf("\n  Activated dsl %d", dsl_id);
	    	printf("\n  State (0=Idle 1=Propose 2=Busy 3=Reserved 4=Restart 5=Maint)\n  ");
	    	for(bchan = ISDN_MIN_CHAN; bchan <= ISDN_MAX_CHAN; bchan++) {
	    	   printf("%x ", ChanMgmtTable[dsl_id][intfc][bchan].Channel_State);
	    	}
	    	printf("\n  Channel (1-31) Service (0=Inservice 1=Maint 2=Outofservice)\n  ");
	    	for(bchan = ISDN_MIN_CHAN; bchan <= ISDN_MAX_CHAN; bchan++) {
	    	   printf("%x ", ChanMgmtTable[dsl_id][intfc][bchan].Service_State);
	    	}
	        automore_conditional(0);
		}
	    }
	}
    }
}

/****************************
* CC_CHAN_set_status_inserv *
*****************************
*
* Used to set PRI channels into an in-service
* state for an interface.
*/

PUBLIC void CC_CHAN_set_status_inserv (dsl)
int dsl;
{
    int intfc;
    int bchan;

    /*
     * check if interface is in range, ignore if not.
     */
    if((dsl < 0) || (dsl > MAX_DSL)) {
        return;
    }
    if(detail_debug)
        buginf("\nISDN %s: PRI Channel being set to in-service for dsl %d ",
            DSL_TO_NAME(dsl), dsl);
    for (intfc = MIN_INT_IDP; intfc <= MAX_INT_IDP; intfc++) {
        for (bchan = ISDN_MIN_CHAN; bchan <= ISDN_MAX_CHAN; bchan++)
            ChanMgmtTable[dsl][intfc][bchan].Service_State = IN_SERVICE;
    }
}

/*************************
* CC_CHAN_GetIdleChanbri *
**************************
*
* Access routine to locate a B-channel or a range of contiguous B-channels
* which are both idle and in service within a user-specified search range.
* This function returns b_chan = 0 if no available channel is found.  If
* the search is satisfied, the function returns the first B-channel number
* in the located range.  It is assumed that the calling function understands
* the B-channel dimension of the channel type specified.
*
* The function is complicated by its support for H0 and H11 channels
* wherein each channel in a group must be checked and, for H0, multiple
* channel groups may be contained within the range provided for search and
* the boundries of the valid H0 channels within the range may not align
* with either the lo_chan or hi_chan specified in the call.
*
* The valid channel types (defined in CCIE.H) are:
*
*   B_CHANNEL_UNITS
*
*/
PUBLIC int CC_CHAN_GetIdleChanbri( lo_chan, hi_chan, type, dsl_id )

int lo_chan;                       /* Low B-Channel in range to search */
int hi_chan;                      /* High B-Channel in range to search */
int type;          /* Type of channel requested - B-channel, H0 or H11 */
int dsl_id;                              /* Digital subscriber loop ID */
{
   BYTE int_id = 0;                                  /* Always default */
   int span = 0;            /* Number of channels in a B-channel range */
   int ranges = 0;               /* Number of channel ranges to search */
   int tmp_ranges;               /* Number of channel ranges to search */
   int init_chan = 0;                    /* Initial channel in a range */
   int chan = 0;            /* Channel under inspection within a range */
   BYTE intfc, min_int, max_int;                      /* Interface IDs */


   switch ( type )
      {
      case B_CHANNEL_UNITS:
         if (( VALID_CHANNELB( lo_chan )) &&
             ( VALID_CHANNELB( hi_chan )) &&
             ( lo_chan <= hi_chan ))
            {
            init_chan = lo_chan;
            span = 1;
            ranges = hi_chan - lo_chan + 1;
            }
         break;
      default:
         chan = 0;
         LIF_ERROR( CC_CHAN_NAME, GET_IDLE_CHAN, BAD_TYPE_ERR );
         goto bailout;
      }
   min_int = ( int_id == ANY_INTERFACE ) ? MIN_INT_IDB : int_id;
   max_int = ( int_id == ANY_INTERFACE ) ? MAX_INT_IDB : int_id;

   if ( ranges )
      {
      /* Search the Channel Management Table for an available channel.
      */
      for ( intfc = min_int; intfc <= max_int; intfc++ )
         {
         tmp_ranges = ranges;   /* Reset tmp_ranges for each interface */
         do
            {
            for ( chan = init_chan; chan < init_chan + span; chan++ )
               {
               if ( ChanMgmtTable[dsl_id][intfc][chan].Channel_State != CHANNEL_IDLE)
                  {
                  chan = 0;              /* A busy B-channel was found */
                  init_chan += span;                     /* Next range */
                  break;
                  }
               }
            if ( chan )
               {
               chan = init_chan;  /* Success - set chan = 1st in range */
               break;
               }
            } while ( --tmp_ranges );
         }
      }
   else
      {
      chan = 0;                                             /* Failure */
      LIF_ERROR( CC_CHAN_NAME, GET_IDLE_CHAN, BAD_CHAN_ERR );
      }

bailout:

   return ( chan );
}

/*************************
* CC_CHAN_GetIdleChanpri *
**************************
*
* Access routine to locate a B-channel or a range of contiguous B-channels
* which are both idle and in service within a user-specified search range.
* This function returns b_chan = 0 if no available channel is found.  If
* the search is satisfied, the function returns the first B-channel number
* in the located range.  It is assumed that the calling function understands
* the B-channel dimension of the channel type specified.
*
* The function is complicated by its support for H0 and H11 channels
* wherein each channel in a group must be checked and, for H0, multiple
* channel groups may be contained within the range provided for search and
* the boundries of the valid H0 channels within the range may not align
* with either the lo_chan or hi_chan specified in the call.
*
* The valid channel types (defined in CCIE.H) are:
*
*   B_CHANNEL_UNITS
*   H0_CHANNEL_UNITS
*   H11_CHANNEL_UNITS
*
*/

PUBLIC int CC_CHAN_GetIdleChanpri( int_id, lo_chan, hi_chan, type, dsl_id )

BYTE int_id;                                           /* Interface ID */
int lo_chan;                       /* Low B-Channel in range to search */
int hi_chan;                      /* High B-Channel in range to search */
int type;          /* Type of channel requested - B-channel, H0 or H11 */
int dsl_id;                              /* Digital subscriber loop ID */
{
   int span;                /* Number of channels in a B-channel range */
   int ranges;                   /* Number of channel ranges to search */
   int tmp_ranges;               /* Number of channel ranges to search */
   int init_chan;                        /* Initial channel in a range */
   int chan;                /* Channel under inspection within a range */
   BYTE intfc, min_int, max_int;                      /* Interface IDs */

   span = 0;
   ranges = 0;
   init_chan = 0;
   chan = 0;

   if(detail_debug)
      buginf("\n entering CC_CHAN_GetIdleChanpri");
 
   switch ( type )
      {
      case B_CHANNEL_UNITS:
         if (( VALID_CHANNELP( lo_chan )) &&
             ( VALID_CHANNELP( hi_chan )) &&
             ( lo_chan <= hi_chan ))
            {
            init_chan = lo_chan;
            span = 1;
            ranges = hi_chan - lo_chan + 1;
            }
         break;
      case H0_CHANNEL_UNITS:
         /* It is necessary to set init_chan = the lowest H0 boundry within
         *  the specified search range.  The boundries are channel 1,
         *  channel 7 and channel 13 and 19 for interfaces without the
         *  D-channel.  Thus if lo_chan is specified as 3
         *  and hi_chan is specified as 21, two H0 ranges are available to
         *  be searched (channel 6 - 12 and channels 13 - 19).
         */
         if (( VALID_CHANNELP( lo_chan )) &&
             ( VALID_CHANNELP( hi_chan )) &&
             ( lo_chan < hi_chan ))
            {
            for ( init_chan = 1; init_chan < 14; init_chan += CHANS_PER_H0 )
               {
               if ( lo_chan <= init_chan )
                  {
                  /* Calculate how many 6-channel ranges exist between
                  *  init_chan and hi_chan.
                  */
                  chan = hi_chan - init_chan + 1;
                  while ( chan >= CHANS_PER_H0 )
                     {
                     ranges++;
                     chan -= CHANS_PER_H0;
                     }
                  span = CHANS_PER_H0;
                  break;
                  }
               }
            }
         break;
      case H11_CHANNEL_UNITS:
         if (( lo_chan == MIN_CHANNEL ) &&
             ( hi_chan == MAX_CHANNELP ) &&
             ( lo_chan == 1 )           &&
             ( hi_chan == CHANS_PER_H11 ))
            {
            init_chan = 1;
            span = CHANS_PER_H11;
            ranges = 1;
            }
         break;
      default:
         chan = 0;
         LIF_ERROR( CC_CHAN_NAME, GET_IDLE_CHAN, BAD_TYPE_ERR );
         goto bailout;
      }
   min_int = ( int_id == ANY_INTERFACE ) ? MIN_INT_IDP : int_id;
   max_int = ( int_id == ANY_INTERFACE ) ? MAX_INT_IDP : int_id;

   if ( ranges )
      {
      /* Search the Channel Management Table for an available channel.
      */
      for ( intfc = min_int; intfc <= max_int; intfc++ )
         {
         tmp_ranges = ranges;   /* Reset tmp_ranges for each interface */
         do
            {
            for ( chan = init_chan; chan < init_chan + span; chan++ )
               {
               if ((ChanMgmtTable[dsl_id][intfc][chan].Channel_State != 
						CHANNEL_IDLE) ||
                   (ChanMgmtTable[dsl_id][intfc][chan].Service_State != 
						IN_SERVICE ))
                  {
                  chan = 0;              /* A busy B-channel was found */
                  init_chan += span;                     /* Next range */
                  break;
                  }
               }
            if ( chan )
               {
               chan = init_chan;  /* Success - set chan = 1st in range */
               break;
               }
            } while ( --tmp_ranges );
         }
      }
   else
      {
      chan = 0;                                             /* Failure */
      LIF_ERROR( CC_CHAN_NAME, GET_IDLE_CHAN, BAD_CHAN_ERR );
      }

bailout:

   return ( chan );
}

/*************************
* CC_CHAN_GetIdleDMSChan *
**************************
*
* Access routine to locate a B-channel or a range of contiguous B-channels
* which are both idle and in service within a user-specified search range.
* This function returns b_chan = 0 if no available channel is found.  If
* the search is satisfied, the function returns the highest B-channel number
* in the located range.  This function does no support H0 or H11 channels
* currently.
*
* This function is provided specifically for PRI_DMS100 where the "slave"
* in channnel allocation is expected to allocate channels in DESCENDING
* order (see NIS A211-1, Ver. 2, Dec., 1989, Sec 4.2.1.3).
*/
PUBLIC int CC_CHAN_GetIdleDMSChan( int_id, lo_chan, hi_chan, dsl_id )

BYTE int_id;                                           /* Interface ID */
int lo_chan;                       /* Low B-Channel in range to search */
int hi_chan;                      /* High B-Channel in range to search */
int dsl_id;                              /* Digital subscriber loop ID */
{
   int chan;                /* Channel under inspection within a range */
   BYTE intfc, min_int, max_int;                      /* Interface IDs */

   chan = 0;

   if(detail_debug)
      buginf("\n entering CC_CHAN_GetIdleDMSChan");

   min_int = ( int_id == ANY_INTERFACE ) ? MIN_INT_IDP : int_id;
   max_int = ( int_id == ANY_INTERFACE ) ? MAX_INT_IDP : int_id;

   if (( VALID_CHANNELP( lo_chan )) && ( VALID_CHANNELP( hi_chan )))
      {
      for ( intfc = min_int; intfc <= max_int; intfc++ )
         {
         for ( chan = hi_chan; chan >= lo_chan; chan-- )
            {
            if ((ChanMgmtTable[dsl_id][intfc][chan].Channel_State == CHANNEL_IDLE) &&
                (ChanMgmtTable[dsl_id][intfc][chan].Service_State == IN_SERVICE ))
               break;
            }
         }
      }
   else
      {
      chan = 0;
      LIF_ERROR( CC_CHAN_NAME, GET_IDLE_DMS_CHAN, BAD_CHAN_ERR );
      }

   return ( chan );
}

/*************************
* CC_CHAN_GetProtocolbri *
**************************
*
* Access routine to read the protocol assigned to a channel(s).
*
* The valid channel types (defined in CCIE.H) are:
*
*   B_CHANNEL_UNITS
*
*/
PUBLIC int CC_CHAN_GetProtocolbri( lo_chan, type, dsl_id )

int lo_chan;               /* Lowest B-chan in a range (range may = 1) */
int type;                    /* Type of channel - B-channel, H0 or H11 */
int dsl_id;                              /* Digital subscriber loop ID */

{
   BYTE int_id = 0;                                  /* Always default */
   int proto;                                      /* Channel protocol */
   int hi_chan;           /* Highest B-chan in a range (range may = 1) */


   if ( LEGAL_CHAN_TYPEB( type ) )
      {
      hi_chan = lo_chan + GET_CHAN_SPANB( type );
      if (( VALID_INT_IDB( int_id ))   &&
          ( VALID_CHANNELB( lo_chan )) &&
          ( VALID_CHANNELB( hi_chan )))
         {
         proto = ChanMgmtTable[dsl_id][int_id][lo_chan].Protocol;
         }
      else
         {
         proto = 0;
         LIF_ERROR( CC_CHAN_NAME, GET_PROTO, BAD_CHAN_ERR );
         }
      }
   else
      {
      proto = 0;
      LIF_ERROR( CC_CHAN_NAME, GET_PROTO, BAD_TYPE_ERR );
      }

   return ( proto );
}
/*************************
* CC_CHAN_GetProtocolpri *
**************************
*
* Access routine to read the protocol assigned to a channel(s).
*
* The valid channel types (defined in CCIE.H) are:
*
*   B_CHANNEL_UNITS
*   H0_CHANNEL_UNITS
*   H11_CHANNEL_UNITS
*
*/
PUBLIC int CC_CHAN_GetProtocolpri( int_id, lo_chan, type, dsl_id )

BYTE int_id;                                           /* Interface ID */
int lo_chan;               /* Lowest B-chan in a range (range may = 1) */
int type;                    /* Type of channel - B-channel, H0 or H11 */
int dsl_id;                              /* Digital subscriber loop ID */

{
   int proto;                                      /* Channel protocol */
   int hi_chan;           /* Highest B-chan in a range (range may = 1) */
   int b_chan;                           /* Current B-Channel in range */


   if ( LEGAL_CHAN_TYPEP( type ) ) 
      {
      hi_chan = lo_chan + GET_CHAN_SPANP( type );
      if (( VALID_INT_IDP( int_id ))   && 
          ( VALID_CHANNELP( lo_chan )) && 
          ( VALID_CHANNELP( hi_chan )))
         {
         proto = ChanMgmtTable[dsl_id][int_id][lo_chan].Protocol;
         for ( b_chan = lo_chan + 1; b_chan <= hi_chan; b_chan++ )
            {
            /* Check for consistent assignment for a hyperchannel
            */
            if ( ChanMgmtTable[ dsl_id ][int_id][ b_chan ].Protocol != proto )
               {
               proto = NOT_A_HYPERCHANNEL;
               break;
               }
            }
         }
      else
         {
         proto = 0;
         LIF_ERROR( CC_CHAN_NAME, GET_PROTO, BAD_CHAN_ERR );
         }
      }
   else
      {
      proto = 0;
      LIF_ERROR( CC_CHAN_NAME, GET_PROTO, BAD_TYPE_ERR );
      }

   return ( proto );
}


/***********************
* CC_CHAN_GetServState *
************************
*
* Access routine to read the service state of a channel(s).
*
* The valid channel types (defined in CCIE.H) are:
*
*   B_CHANNEL_UNITS
*   H0_CHANNEL_UNITS
*   H11_CHANNEL_UNITS
*
*/
PUBLIC int CC_CHAN_GetServState (int_id, lo_chan, type, dsl_id)

BYTE int_id;                                           /* Interface_ID */
int lo_chan;               /* Lowest B-chan in a range (range may = 1) */
int type;                    /* Type of channel - B-channel, H0 or H11 */
int dsl_id;                              /* Digital subscriber loop ID */

{
   int serv_state;                            /* Channel service state */
   int hi_chan;           /* Highest B-chan in a range (range may = 1) */
   int b_chan;                           /* Current B-Channel in range */


   if (LEGAL_CHAN_TYPEP(type)) {
      hi_chan = lo_chan + GET_CHAN_SPANP(type);
      if ((VALID_INT_IDP(int_id))   && 
          (VALID_CHANNELP(lo_chan)) && 
          (VALID_CHANNELP(hi_chan - 1))) {
         serv_state = ChanMgmtTable[dsl_id][int_id][lo_chan].Service_State;
         for (b_chan = lo_chan + 1; b_chan < hi_chan; b_chan++) {
            /*
             * Check for consistent assignment for a hyperchannel
             */
            if (ChanMgmtTable[dsl_id][int_id][b_chan].Service_State != serv_state) {
               serv_state = NOT_A_HYPERCHANNEL;
               break;
            }
         }
      } else {
         serv_state = OUT_OF_SERVICE;
         LIF_ERROR(CC_CHAN_NAME, GET_SERV_STATE, BAD_CHAN_ERR);
      }
   } else {
      serv_state = OUT_OF_SERVICE;
      LIF_ERROR(CC_CHAN_NAME, GET_SERV_STATE, BAD_TYPE_ERR);
   }

   return (serv_state);
}

/**********************
* CC_CHAN_GetStatebri *
***********************
*
* Access routine to read the call-related state of a B-channel(s).
*
* The valid channel types (defined in CCIE.H) are:
*
*   B_CHANNEL_UNITS
*
*/
PUBLIC int CC_CHAN_GetStatebri( lo_chan, type, dsl_id )

int lo_chan;               /* Lowest B-chan in a range (range may = 1) */
int type;                    /* Type of channel - B-channel, H0 or H11 */
int dsl_id;                              /* Digital subscriber loop ID */

{
   BYTE int_id = 0;                                  /* Always default */
   int state;                                         /* Channel state */
   int hi_chan;           /* Highest B-chan in a range (range may = 1) */


   if ( LEGAL_CHAN_TYPEB( type ))
      {
      hi_chan = lo_chan + GET_CHAN_SPANB( type );
      if (( VALID_INT_IDB( int_id ))   &&
          ( VALID_CHANNELB( lo_chan )) &&
          ( VALID_CHANNELB( hi_chan )))
         {
         state = ChanMgmtTable[dsl_id][int_id][lo_chan].Channel_State;
         }
      else
         {
         state = 0;
         LIF_ERROR( CC_CHAN_NAME, GET_STATE, BAD_CHAN_ERR );
         }
      }
   else
      {
      state = 0;
      LIF_ERROR( CC_CHAN_NAME, GET_STATE, BAD_TYPE_ERR );
      }

   return ( state );
}

/**********************
* CC_CHAN_GetStatepri *
***********************
*
* Access routine to read the call-related state of a B-channel(s).
*
* The valid channel types (defined in CCIE.H) are:
*
*   B_CHANNEL_UNITS
*   H0_CHANNEL_UNITS
*   H11_CHANNEL_UNITS
*
*/
PUBLIC int CC_CHAN_GetStatepri( int_id, lo_chan, type, dsl_id )

BYTE int_id;                                           /* Interface ID */
int lo_chan;               /* Lowest B-chan in a range (range may = 1) */
int type;                    /* Type of channel - B-channel, H0 or H11 */
int dsl_id;                              /* Digital subscriber loop ID */

{
   int state;                                         /* Channel state */
   int hi_chan;           /* Highest B-chan in a range (range may = 1) */
   int b_chan;                           /* Current B-Channel in range */


   if ( LEGAL_CHAN_TYPEP( type ))
      {
      hi_chan = lo_chan + GET_CHAN_SPANP( type );
      if (( VALID_INT_IDP( int_id ))   && 
          ( VALID_CHANNELP( lo_chan )) && 
          ( VALID_CHANNELP( hi_chan - 1 ))) /* ===, boundary problems */
         {
         state = ChanMgmtTable[dsl_id][int_id][lo_chan].Channel_State;
         for ( b_chan = lo_chan + 1; b_chan < hi_chan; b_chan++ )
            {
            /* Check for consistent assignment for a hyperchannel
            */
            if ( ChanMgmtTable[dsl_id][int_id][b_chan].Channel_State != state )
               {
               state = NOT_A_HYPERCHANNEL;
               break;
               }
            }
         }
      else
         {
         state = 0;
         LIF_ERROR( CC_CHAN_NAME, GET_STATE, BAD_CHAN_ERR );
         }
      }
   else
      {
      state = 0;
      LIF_ERROR( CC_CHAN_NAME, GET_STATE, BAD_TYPE_ERR );
      }
   return ( state );
}

/*********************
* CC_CHAN_GetTimerID *
**********************
*
* Access routine to read the timer ID of a channel.
*
* The valid channel types (defined in CCIE.H) are:
*
*   B_CHANNEL_UNITS
*
* Only the b-channel is supported since this timer is presumed to be
* associated with the SERVICE message function, and SERVICE messages
* are only expected on a B-channel basis.
*
*/
PUBLIC int CC_CHAN_GetTimerID( int_id, chan, dsl_id )

BYTE int_id;                                           /* Interface ID */
int chan;                  /* Lowest B-chan in a range (range may = 1) */
int dsl_id;                              /* Digital subscriber loop ID */

{
   int timer_id;                               /* Channel timer handle */


   if (( VALID_CHANNELP( chan )) && ( VALID_INT_IDB( int_id )))
      {
      timer_id = ChanMgmtTable[dsl_id][int_id][chan].TimerID;
      }
   else
      {
      timer_id = NOT_TIMING;
      LIF_ERROR( CC_CHAN_NAME, GET_TIMER_ID, BAD_CHAN_ERR );
      }

   return ( timer_id );
}

/*********************
* CC_CHAN_GetUserbri *
**********************
*
* Access routine to recover the Call_ID of a call given the B-Channel(s)
* it uses.
*
* The valid channel types (defined in CCIE.H) are:
*
*   B_CHANNEL_UNITS
*
*/
PUBLIC int CC_CHAN_GetUserbri( lo_chan, type, dsl_id )

int lo_chan;               /* Lowest B-chan in a range (range may = 1) */
int type;                    /* Type of channel - B-channel, H0 or H11 */
int dsl_id;                              /* Digital subscriber loop ID */

{
   BYTE int_id = 0;                                  /* Always default */
   int user;                                           /* Channel user */
   int hi_chan;           /* Highest B-chan in a range (range may = 1) */


   if ( LEGAL_CHAN_TYPEB( type ))
      {
      hi_chan = lo_chan + GET_CHAN_SPANB( type );
      if (( VALID_INT_IDB( int_id ))   &&
          ( VALID_CHANNELB( lo_chan )) &&
          ( VALID_CHANNELB( hi_chan )))
         {
         user = ChanMgmtTable[dsl_id][int_id][lo_chan].Using_Call_ID;
         }
      else
         {
         user = 0;
         LIF_ERROR( CC_CHAN_NAME, GET_USER, BAD_CHAN_ERR );
         }
      }
   else
      {
      user = 0;
      LIF_ERROR( CC_CHAN_NAME, GET_USER, BAD_TYPE_ERR );
      }

   return ( user );
}

/*********************
* CC_CHAN_GetUserpri *
**********************
*
* Access routine to recover the Call_ID of a call given the B-Channel(s)
* it uses.
*
* The valid channel types (defined in CCIE.H) are:
*
*   B_CHANNEL_UNITS
*   H0_CHANNEL_UNITS
*   H11_CHANNEL_UNITS
*
*/
PUBLIC int CC_CHAN_GetUserpri( int_id, lo_chan, type, dsl_id )

BYTE int_id;                                           /* Interface ID */
int lo_chan;               /* Lowest B-chan in a range (range may = 1) */
int type;                    /* Type of channel - B-channel, H0 or H11 */
int dsl_id;                              /* Digital subscriber loop ID */

{
   int user;                                           /* Channel user */
   int hi_chan;           /* Highest B-chan in a range (range may = 1) */


   if ( LEGAL_CHAN_TYPEP( type ))
      {
      hi_chan = lo_chan + GET_CHAN_SPANP( type );
      if (( VALID_INT_IDP( int_id ))   && 
          ( VALID_CHANNELP( lo_chan )) && 
          ( VALID_CHANNELP( hi_chan )))
         {
         user = ChanMgmtTable[dsl_id][int_id][lo_chan].Using_Call_ID;
#ifdef 0
         /*
          * We don't do Hyperchannel stuff yet, but keep this around for a
          * little while longer. The logic may be valid for using multiple
          * channels. ===
          */
         for ( b_chan = lo_chan + 1; b_chan <= hi_chan; b_chan++ )
            {
            /* Check for consistent assignment for a hyperchannel
            */
            if ( ChanMgmtTable[dsl_id][int_id][b_chan].Using_Call_ID != user )
               {
               user = NOT_A_HYPERCHANNEL;
               break;
               }
            }
#endif
         }
      else
         {
         user = 0;
         if(detail_debug)
            buginf("\n CC_CHAN_GetUserpri, BAD_CHAN_ERR");
         LIF_ERROR( CC_CHAN_NAME, GET_USER, BAD_CHAN_ERR );
         }
      }
   else
      {
      user = 0;
      if(detail_debug)
         buginf("\n CC_CHAN_GetUserpri, BAD_Type_ERR");
      LIF_ERROR( CC_CHAN_NAME, GET_USER, BAD_TYPE_ERR );
      }

   if(detail_debug)
      buginf("\n CC_CHAN_GetUserpri, returning user = %x", user);
   return ( user );
}


/***********************
* CC_CHAN_InitTablebri *
************************
*
* Access routine to initialize all channels within a range in the Channel
* Management Table to a set of application-defined values.
*
*/
PUBLIC BOOLEAN CC_CHAN_InitTablebri( low_chan, hi_chan, state, proto, call_id )

int low_chan;             /* The lowest numbered B-Channel of interest */
int hi_chan;             /* The highest numbered B-Channel of interest */
int state;                             /* The state for the B-Channels */
int proto;                          /* The protocol for the B-Channels */
int call_id;                                 /* The call_id B-Channels */

{
   int dsl_id;                           /* Digital subscriber loop ID */
   int intfc;                                         /* Interface ID */
   int b_chan;                                /* The current B-Channel */
   BOOLEAN no_error;                          /* Initialization result */


   if (( VALID_CHANNELB( low_chan )) &&
       ( VALID_CHANNELB( hi_chan ))  &&
       ( low_chan <= hi_chan ))
      {
      for ( dsl_id = 0; dsl_id < MAX_DSL; dsl_id++ )
         {
         for ( intfc = MIN_INT_IDB; intfc <= MAX_INT_IDB; intfc++ )
            {
            for ( b_chan = low_chan; b_chan <= hi_chan; b_chan++ )
               {
               ChanMgmtTable[dsl_id][intfc][b_chan].Channel_State = state;
               ChanMgmtTable[dsl_id][intfc][b_chan].Protocol      = proto;
               ChanMgmtTable[dsl_id][intfc][b_chan].Using_Call_ID = call_id;
               }
            }
         }
      no_error = TRUE;
      }
   else
      {
      no_error = FALSE;
      LIF_ERROR( CC_CHAN_NAME, INIT_TABLE, BAD_CHAN_ERR );
      }

   return( no_error );
}



/***********************
* CC_CHAN_InitTablepri *
************************
*
* Access routine to initialize all channels within a range in the Channel
* Management Table to a set of application-defined values.
*
*/
PUBLIC BOOLEAN CC_CHAN_InitTablepri( low_chan, hi_chan, state, proto, call_id,
                                  serv_state )

int low_chan;             /* The lowest numbered B-Channel of interest */
int hi_chan;             /* The highest numbered B-Channel of interest */
int state;                             /* The state for the B-Channels */
int proto;                          /* The protocol for the B-Channels */
int call_id;                                 /* The call_id B-Channels */
int serv_state;                /* The service state for the B-Channels */

{
   int dsl_id;                           /* Digital subscriber loop ID */
   int intfc;                                         /* Interface ID */
   int b_chan;                                /* The current B-Channel */
   BOOLEAN no_error;                          /* Initialization result */


   if (( VALID_CHANNELP( low_chan )) &&
       ( VALID_CHANNELP( hi_chan ))  &&
       ( low_chan <= hi_chan ))
      {
      for ( dsl_id = 0; dsl_id < MAX_DSL; dsl_id++ )
         {
         for ( intfc = MIN_INT_IDP; intfc <= MAX_INT_IDP; intfc++ )
            {
            for ( b_chan = low_chan; b_chan <= hi_chan; b_chan++ )
               {
               ChanMgmtTable[dsl_id][intfc][b_chan].Channel_State = state;
               ChanMgmtTable[dsl_id][intfc][b_chan].Protocol      = proto;
               ChanMgmtTable[dsl_id][intfc][b_chan].Using_Call_ID = call_id;
               ChanMgmtTable[dsl_id][intfc][b_chan].Service_State = serv_state;
               ChanMgmtTable[dsl_id][intfc][b_chan].TimerID       = -1;
               }
            }
         }
      no_error = TRUE;
      }
   else
      {
      no_error = FALSE;
      if(detail_debug)
         buginf("\n error initializing PRI table");
      LIF_ERROR( CC_CHAN_NAME, INIT_TABLE, BAD_CHAN_ERR );
      }

   return( no_error );
}


/*************************
* CC_CHAN_ReleaseChanbri *
**************************
*
* Access routine to clear the call usage fields of the channel structure
* for a channel(s), specifically Channel_State, Protocol and Using_Call_ID.
*
* The valid channel types (defined in CCIE.H) are:
*
*   B_CHANNEL_UNITS
*
*/
PUBLIC void CC_CHAN_ReleaseChanbri( lo_chan, type, dsl_id )

int lo_chan;               /* Lowest B-chan in a range (range may = 1) */
int type;                    /* Type of channel - B-channel, H0 or H11 */
int dsl_id;                              /* Digital subscriber loop ID */

{
   BYTE int_id = 0;                                  /* Always default */
   int hi_chan;           /* Highest B-chan in a range (range may = 1) */
   int b_chan;                           /* Current B-Channel in range */


   if ( LEGAL_CHAN_TYPEB( type ))
      {
      hi_chan = lo_chan + GET_CHAN_SPANB( type );
      if (( VALID_INT_IDB( int_id ))   &&
          ( VALID_CHANNELB( lo_chan )) &&
          ( VALID_CHANNELB( hi_chan )))
         {
         for ( b_chan = lo_chan; b_chan <= hi_chan; b_chan++ )
            {
            ChanMgmtTable[dsl_id][int_id][b_chan].Channel_State = CHANNEL_IDLE;            ChanMgmtTable[dsl_id][int_id][b_chan].Protocol      = 0;
            ChanMgmtTable[dsl_id][int_id][b_chan].Using_Call_ID = 0;
            }
         }
      else
         {
         LIF_ERROR( CC_CHAN_NAME, RELEASE_CHAN, BAD_CHAN_ERR );
         }
      }
   else
      {
      LIF_ERROR( CC_CHAN_NAME, RELEASE_CHAN, BAD_TYPE_ERR );
      }
}


/*************************
* CC_CHAN_ReleaseChanpri *
**************************
*
* Access routine to clear the call usage fields of the channel structure
* for a channel(s), specifically Channel_State, Protocol and Using_Call_ID.
*
* The valid channel types (defined in CCIE.H) are:
*
*   B_CHANNEL_UNITS
*   H0_CHANNEL_UNITS
*   H11_CHANNEL_UNITS
*
*/
PUBLIC void CC_CHAN_ReleaseChanpri( int_id, lo_chan, type, dsl_id )

BYTE int_id;                                           /* Interface ID */
int lo_chan;               /* Lowest B-chan in a range (range may = 1) */
int type;                    /* Type of channel - B-channel, H0 or H11 */
int dsl_id;                              /* Digital subscriber loop ID */

{
   int hi_chan;           /* Highest B-chan in a range (range may = 1) */
   int b_chan = lo_chan;                 /* Current B-Channel in range */

   if(detail_debug)
     buginf("\nISDN %s: releasing channel = %x", DSL_TO_NAME(dsl_id),
         lo_chan); 

   if ( LEGAL_CHAN_TYPEP( type )) {
      hi_chan = lo_chan + GET_CHAN_SPANP( type ) - 1;
      if (( VALID_INT_IDP( int_id ))   && 
          ( VALID_CHANNELP( lo_chan )) && 
          ( VALID_CHANNELP( hi_chan ))) {
         for ( b_chan = lo_chan; b_chan <= hi_chan; b_chan++ ) {
            /*
             * Cannot idle a channel that is reserved. This
             * may be a D-channel or one that is unassigned. Check to see
             * if it is reserved.
             */
            if (ChanMgmtTable[dsl_id][int_id][b_chan].Channel_State != CHANNEL_RESERVED) {
                ChanMgmtTable[dsl_id][int_id][b_chan].Channel_State = CHANNEL_IDLE;
                ChanMgmtTable[dsl_id][int_id][b_chan].Protocol      = 0;
                ChanMgmtTable[dsl_id][int_id][b_chan].Using_Call_ID = 0;
            }
         }
      } else {
         if(detail_debug)
            buginf("\n channel = %d", b_chan);
         LIF_ERROR( CC_CHAN_NAME, RELEASE_CHAN, BAD_CHAN_ERR );
      }
   } else
      LIF_ERROR( CC_CHAN_NAME, RELEASE_CHAN, BAD_TYPE_ERR );
}

/*************************
* CC_CHAN_SetProtocolbri *
**************************
*
* Access routine to write a new protocol for a B_Channel(s).
*
* The valid channel types (defined in CCIE.H) are:
*
*   B_CHANNEL_UNITS
*
*/
PUBLIC void CC_CHAN_SetProtocolbri( lo_chan, type, proto, dsl_id )

int lo_chan;               /* Lowest B-chan in a range (range may = 1) */
int type;                    /* Type of channel - B-channel, H0 or H11 */
int proto;                                         /* The new protocol */
int dsl_id;                              /* Digital subscriber loop ID */

{
   BYTE int_id = 0;                                  /* Always default */
   int hi_chan;           /* Highest B-chan in a range (range may = 1) */
   int b_chan;                           /* Current B-Channel in range */


   if ( LEGAL_CHAN_TYPEB( type ))
      {
      hi_chan = lo_chan + GET_CHAN_SPANB( type );
      if (( VALID_INT_IDB( int_id ))   &&
          ( VALID_CHANNELB( lo_chan )) &&
          ( VALID_CHANNELB( hi_chan )))
         {
         for ( b_chan = lo_chan; b_chan <= hi_chan; b_chan++ )
            ChanMgmtTable[dsl_id][int_id][b_chan].Protocol = proto;
         }
      else
         {
         LIF_ERROR( CC_CHAN_NAME, SET_PROTO, BAD_CHAN_ERR );
         }
      }
   else
      {
      LIF_ERROR( CC_CHAN_NAME, SET_PROTO, BAD_TYPE_ERR );
      }
}


/*************************
* CC_CHAN_SetProtocolpri *
**************************
*
* Access routine to write a new protocol for a B_Channel(s).
*
* The valid channel types (defined in CCIE.H) are:
*
*   B_CHANNEL_UNITS
*   H0_CHANNEL_UNITS
*   H11_CHANNEL_UNITS
*
*/
PUBLIC void CC_CHAN_SetProtocolpri( int_id, lo_chan, type, proto, dsl_id )

BYTE int_id;
int lo_chan;               /* Lowest B-chan in a range (range may = 1) */
int type;                    /* Type of channel - B-channel, H0 or H11 */
int proto;                                         /* The new protocol */
int dsl_id;                              /* Digital subscriber loop ID */

{
   int hi_chan;           /* Highest B-chan in a range (range may = 1) */
   int b_chan;                           /* Current B-Channel in range */


   if ( LEGAL_CHAN_TYPEP( type ))
      {
      hi_chan = lo_chan + GET_CHAN_SPANP( type );
      if (( VALID_INT_IDP( int_id ))   && 
          ( VALID_CHANNELP( lo_chan )) && 
          ( VALID_CHANNELP( hi_chan )))
         {
         for ( b_chan = lo_chan; b_chan <= hi_chan; b_chan++ )
            ChanMgmtTable[dsl_id][int_id][b_chan].Protocol = proto;
         }
      else
         {
         LIF_ERROR( CC_CHAN_NAME, SET_PROTO, BAD_CHAN_ERR );
         }
      }
   else
      {
      LIF_ERROR( CC_CHAN_NAME, SET_PROTO, BAD_TYPE_ERR );
      }
}


/***********************
* CC_CHAN_SetServState *
************************
*
* Access routine to write a new service state for a B-Channel(s).
*
* The valid channel types (defined in CCIE.H) are:
*
*   B_CHANNEL_UNITS
*   H0_CHANNEL_UNITS
*   H11_CHANNEL_UNITS
*
*/
PUBLIC void CC_CHAN_SetServState (int_id, lo_chan, type, serv_state, dsl_id)

BYTE int_id;                                           /* Interface ID */
int lo_chan;               /* Lowest B-chan in a range (range may = 1) */
int type;                    /* Type of channel - B-channel, H0 or H11 */
int serv_state;                               /* The new service state */
int dsl_id;                              /* Digital subscriber loop ID */

{
   int hi_chan;           /* Highest B-chan in a range (range may = 1) */
   int b_chan;                           /* Current B-Channel in range */


   if (LEGAL_CHAN_TYPEP(type)) {
      hi_chan = lo_chan + GET_CHAN_SPANP(type);
      if ((VALID_INT_IDP(int_id))   && 
          (VALID_CHANNELP(lo_chan)) && 
          (VALID_CHANNELP(hi_chan - 1))) {
         for (b_chan = lo_chan; b_chan < hi_chan; b_chan++)
            ChanMgmtTable[dsl_id][int_id][b_chan].Service_State = serv_state;
      } else {
         LIF_ERROR(CC_CHAN_NAME, SET_SERV_STATE, BAD_CHAN_ERR);
      }
   } else {
      LIF_ERROR(CC_CHAN_NAME, SET_SERV_STATE, BAD_TYPE_ERR);
   }
}


/**********************
* CC_CHAN_SetStatebri *
***********************
*
* Access routine to write a new channel state for a B-Channel(s).
*
* The valid channel types (defined in CCIE.H) are:
*
*   B_CHANNEL_UNITS
*
*/
PUBLIC void CC_CHAN_SetStatebri( lo_chan, type, new_state, dsl_id )

int lo_chan;               /* Lowest B-chan in a range (range may = 1) */
int type;                    /* Type of channel - B-channel, H0 or H11 */
int new_state;                   /* The new call state for the channel */
int dsl_id;                              /* Digital subscriber loop ID */

{
   BYTE int_id = 0;                                  /* Always default */
   int hi_chan;           /* Highest B-chan in a range (range may = 1) */
   int b_chan;                           /* Current B-Channel in range */


   if ( LEGAL_CHAN_TYPEB( type ))
      {
     hi_chan = lo_chan + GET_CHAN_SPANB( type );
      if (( VALID_INT_IDB( int_id ))   &&
          ( VALID_CHANNELB( lo_chan )) &&
          ( VALID_CHANNELB( hi_chan )))
         {
         for ( b_chan = lo_chan; b_chan <= hi_chan; b_chan++ )
            ChanMgmtTable[dsl_id][int_id][b_chan].Channel_State = new_state;
         }
      else
         {
         LIF_ERROR( CC_CHAN_NAME, SET_STATE, BAD_CHAN_ERR );
         }
      }
   else
      {
      LIF_ERROR( CC_CHAN_NAME, SET_STATE, BAD_TYPE_ERR );
      }
}

/**********************
* CC_CHAN_SetStatepri *
***********************
*
* Access routine to write a new channel state for a B-Channel(s).
*
* The valid channel types (defined in CCIE.H) are:
*
*   B_CHANNEL_UNITS
*   H0_CHANNEL_UNITS
*   H11_CHANNEL_UNITS
*
*/
PUBLIC void CC_CHAN_SetStatepri( int_id, lo_chan, type, new_state, dsl_id )

BYTE int_id;                                           /* Interface ID */
int lo_chan;               /* Lowest B-chan in a range (range may = 1) */
int type;                    /* Type of channel - B-channel, H0 or H11 */
int new_state;                   /* The new call state for the channel */
int dsl_id;                              /* Digital subscriber loop ID */

{
   int hi_chan;           /* Highest B-chan in a range (range may = 1) */
   int b_chan;                           /* Current B-Channel in range */

   if ( LEGAL_CHAN_TYPEP( type ))
      {
      hi_chan = lo_chan + GET_CHAN_SPANP( type );
      if (( VALID_INT_IDP( int_id ))   && 
          ( VALID_CHANNELP( lo_chan )) && 
          ( VALID_CHANNELP( hi_chan - 1)))
         {
         for ( b_chan = lo_chan; b_chan < hi_chan; b_chan++ )
            ChanMgmtTable[dsl_id][int_id][b_chan].Channel_State = new_state;
         }
      else
         {
         LIF_ERROR( CC_CHAN_NAME, SET_STATE, BAD_CHAN_ERR );
         }
      }
   else
      {
      LIF_ERROR( CC_CHAN_NAME, SET_STATE, BAD_TYPE_ERR );
      }
}


/*********************
* CC_CHAN_SetTimerID *
**********************
*
* Access routine to write the timer ID of a channel(s).
*
* The valid channel types (defined in CCIE.H) are:
*
*   B_CHANNEL_UNITS
*
* Only the b-channel is supported since this timer is presumed to be
* associated with the SERVICE message function, and SERVICE messages
* are only expected on a B-channel basis.
*
*/
PUBLIC void CC_CHAN_SetTimerID( int_id, chan, timer_id, dsl_id )

BYTE int_id;                                           /* Interface ID */
int chan;                  /* Lowest B-chan in a range (range may = 1) */
int timer_id;                              /* Timer ID for the channel */
int dsl_id;                              /* Digital subscriber loop ID */

{
   if (( VALID_INT_IDP( int_id )) && ( VALID_CHANNELP( chan )))
      {
      ChanMgmtTable[dsl_id][int_id][chan].TimerID = timer_id;
      }
   else
      {
      LIF_ERROR( CC_CHAN_NAME, SET_TIMER_ID, BAD_CHAN_ERR );
      }
}


/*********************
* CC_CHAN_SetUserbri *
**********************
*
* Access routine to write the new call_id to be associated as the
* user of a B-channel(s).
*
* The valid channel types (defined in CCIE.H) are:
*
*   B_CHANNEL_UNITS
*
*/
PUBLIC void CC_CHAN_SetUserbri( lo_chan, type, call_id, dsl_id )

int lo_chan;               /* Lowest B-chan in a range (range may = 1) */
int type;                    /* Type of channel - B-channel, H0 or H11 */
int call_id;             /* Call Control Call_ID now using the channel */
int dsl_id;                              /* Digital subscriber loop ID */

{
   BYTE int_id = 0;                                  /* Always default */
   int hi_chan;           /* Highest B-chan in a range (range may = 1) */
   int b_chan;                           /* Current B-Channel in range */


   if ( LEGAL_CHAN_TYPEB( type ))
      {
      hi_chan = lo_chan + GET_CHAN_SPANB( type );
      if (( VALID_INT_IDB( int_id ))   &&
          ( VALID_CHANNELB( lo_chan )) &&
          ( VALID_CHANNELB( hi_chan )))
         {
         for ( b_chan = lo_chan; b_chan <= hi_chan; b_chan++ )
            ChanMgmtTable[dsl_id][int_id][b_chan].Using_Call_ID = call_id;
         }
      else
         {
         LIF_ERROR( CC_CHAN_NAME, SET_USER, BAD_CHAN_ERR );
         }
      }
   else
      {
      LIF_ERROR( CC_CHAN_NAME, SET_USER, BAD_TYPE_ERR );
      }
}

/*********************
* CC_CHAN_SetUserpri *
**********************
*
* Access routine to write the new call_id to be associated as the
* user of a B-channel(s).
*
* The valid channel types (defined in CCIE.H) are:
*
*   B_CHANNEL_UNITS
*   H0_CHANNEL_UNITS
*   H11_CHANNEL_UNITS
*
*/
PUBLIC void CC_CHAN_SetUserpri( int_id, lo_chan, type, call_id, dsl_id )

BYTE int_id;                                           /* Interface ID */
int lo_chan;               /* Lowest B-chan in a range (range may = 1) */
int type;                    /* Type of channel - B-channel, H0 or H11 */
int call_id;             /* Call Control Call_ID now using the channel */
int dsl_id;                              /* Digital subscriber loop ID */

{
   int hi_chan;           /* Highest B-chan in a range (range may = 1) */
   int b_chan;                           /* Current B-Channel in range */


   if ( LEGAL_CHAN_TYPEP( type ))
      {
      hi_chan = lo_chan + GET_CHAN_SPANP( type );
      if (( VALID_INT_IDP( int_id ))   && 
          ( VALID_CHANNELP( lo_chan )) && 
          ( VALID_CHANNELP( hi_chan - 1)))
         {
         for ( b_chan = lo_chan; b_chan < hi_chan; b_chan++ )
            ChanMgmtTable[dsl_id][int_id][b_chan].Using_Call_ID = call_id;
         }
      else
         {
         LIF_ERROR( CC_CHAN_NAME, SET_USER, BAD_CHAN_ERR );
         }
      }
   else
      {
      LIF_ERROR( CC_CHAN_NAME, SET_USER, BAD_TYPE_ERR );
      }
}

/* End of module.
*/
