/* $Id: vt_interface.h,v 3.4 1996/03/07 00:37:41 irfan Exp $
 * $Source: /release/111/cvs/Xsys/tn3270/vt_interface.h,v $
 *------------------------------------------------------------------
 * V T _ I N T E R F A C E . H
 *
 * July, 1994, Bill Palter
 * 
 * Copyright (c) 1994,1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: vt_interface.h,v $
 * Revision 3.4  1996/03/07  00:37:41  irfan
 * CSCdi48513:  TN3270 sends excessive cursor-move strings
 * Add "[no] tn3270 optimize-cursor-move" & "[no] tn3270 status-message".
 *
 * Revision 3.3  1996/01/23  07:55:12  irfan
 * CSCdi47316:  possible memory corruption in tn3270
 * tn3270 copies data from telnet w/o bounds checking onto internal
 * bufferes. Force upper bounds.
 *
 * Revision 3.2  1995/11/17  18:52:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:37:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:08:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define	STATE_NULL	-1	       /* Falls off edge */
#define	STATE_GOTO	-2	       /* GOTO internal state */


struct state_type {
    int             match;	       /* character to match */
    int             result;	       /* 3270 control code */
    struct state_type *next;	       /* next entry in this same state */
    struct state_type *address;	       /* if goto, where is next state */
};

#define NEXTSTATE(x)	(x->next)

typedef struct {
    int             type;	       /* LEX_* - type of character */
    int             value;	       /* character this was */
}               lexicon;


typedef struct {
    int             length;	       /* length of character string */
    char            array[50];	       /* character string */
}               stringWithLength;





struct vtinfo_type {
    unsigned short  width;
    unsigned short  height;
    unsigned char   screen_clear;
    unsigned char   text[MAX_SCREEN_SIZE];
    struct interface_attribute_type attribute[MAX_SCREEN_SIZE];
    unsigned short  cursor_location; /* protocol: cursor-location */
                                     /* expected next byte output location */
    unsigned short  current_cursor_location; /* .... used for optimization */
    unsigned char   screen_reversed;
    int            *terminal;
    unsigned short  input_channel;
    unsigned char   keyboard_state;
    struct conntype_ *connection;
    unsigned char   input_buffer[SCREEN_INPUT_BUFF_SIZE];
    unsigned char  *input_buffer_ptr;


    struct state_type *head;
    struct state_type *state;
    char          **whichkey;
    char           *environPointer;
    int             Empty;
    int             Full;
    lexicon         lifo[20];
    int             rp;
    int             wp;
    char 	    doPaste;

    char            termcap_buffer[1024];
    char            tc_strings[1024];
    char           *x0;
    char           *x1;
    char           *x2;
    char           *x3;
    char           *x4;
    char           *x5;
    char           *x6;
    char           *x7;
    char           *mr;		       /* Reverse Video */
    char           *mb;		       /* Bold */
    char           *us;		       /* underscore */
    char           *md;		       /* highlight */
    char           *me;		       /* Turn off attributes */
    char           *cm;		       /* cursor move */
    char           *cl;		       /* Clear screen */
    char           *is;		       /* init string */
    char           *rs;		       /* reset string */
    char           *ti;		       /* termcap init string */
    char           *vs;		       /* Visual cursor */
    char           *te;		       /* termcap end string */
    char           *ve;		       /* End Visual cursor */
    char           *ce;                /* Erase to end of line */
    char    	    transparent_mode;
    boolean         optimize_position; /* Optimize cm strings */
    boolean         do_status_msg;     /* do status messages */
    int             cache_highlight;
    int             cache_foreground_color;
};


#define KS_BASE 	(0x00)
#define KS_ESCAPE	(0x01)
#define KS_ESC_BRACKET  (0x02)
#define KS_ESC_O  	(0x03)

#define VT_SCREEN_CLEAR	(0x01)
#define VT_SCREEN_DIRTY	(0x00)


#define IADDRESS_TO_ROW(x) 	(x % vtinfo->width)
#define IADDRESS_TO_COL(x) 	(x / vtinfo->width)

#define BAD_SCREEN_COORDINATES (-1)

typedef struct vtinfo_type *VtinfoPtr;
