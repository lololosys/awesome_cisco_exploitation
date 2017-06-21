/* $Id: tn3270.h,v 3.4.20.1 1996/03/27 23:28:35 irfan Exp $
 * $Source: /release/111/cvs/Xsys/tn3270/tn3270.h,v $
 *------------------------------------------------------------------
 * T N 3 2 7 0 . H
 *
 * July, 1994, Bill Palter
 * 
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: tn3270.h,v $
 * Revision 3.4.20.1  1996/03/27  23:28:35  irfan
 * CSCdi51821:  tn3270 should allow typeahead
 * Branch: California_branch
 * Add typeahead buffer to hold keyboard input while keyboard appears
 * locked. Add a TN3270 UI knob to control this behaviour. Reduce the
 * amount
 * of whitespace painted
 *
 * Revision 3.4  1996/01/25  10:02:59  irfan
 * CSCdi47466:  tn3270 does not check malloc return code..may modify
 * low-mem
 * review comment: make the process sleep and retry on malloc failures
 *
 * Revision 3.3  1996/01/23  07:55:07  irfan
 * CSCdi47316:  possible memory corruption in tn3270
 * tn3270 copies data from telnet w/o bounds checking onto internal
 * bufferes. Force upper bounds.
 *
 * Revision 3.2  1995/11/17  18:51:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:37:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:08:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __DATA_LOADED
#define __DATA_LOADED	1
#include "platform.h"


#define TN3270_MOD2	(2)
#define TN3270_MOD3	(3)
#define TN3270_MOD4	(4)
#define TN3270_MOD5	(5)



#define TELNET_PROTOCOL
/* #define BOUNDS_CHECKING*/

#ifdef TELNET_PROTOCOL
#define IAC	255
#define EOR	239
#endif

#ifdef __TURBOC__
#include <winsock.h>
#endif

#ifdef _MSC_VER
#include <winsock.h>
#endif


#define MAX_SCREEN_SIZE 4096

#define SCREEN_INPUT_BUFF_SIZE 8096   /* Used in vt_interface.h */
#define TN3270_MEMWAIT_SLEEP_SECS 5   /* Wait this long on mem shortage */
#define TN3270_TYPEAHEAD_BUF_SIZE 50  /* typeahead buffer size */

/*
 * The following structure is used to pass the display attributes to
 * the lower levels to display
 */

struct interface_attribute_type {
    unsigned char   foreground_color;
    unsigned char   background_color;
    unsigned char   highlighting;
    unsigned char   character_set;
    unsigned char   field_outlining;
    unsigned char   transparency;
};


#define INTERFACE_HIGHLIGHT_BLINK	(0x01)
#define INTERFACE_HIGHLIGHT_REVERSE	(0x02)
#define INTERFACE_HIGHLIGHT_INTENSIFY	(0x04)
#define INTERFACE_HIGHLIGHT_UNDERSCORE	(0x08)
#define INTERFACE_HIGHLIGHT_INVISIBLE	(0x10)



struct character_attribute_type {
    unsigned char   foreground_color;
    unsigned char   character_set;
    unsigned char   extended_highlight;
    unsigned char   transparency;
    unsigned char   background_color;
};

#define COMP_CHAR_ATT(att1,att2)	\
		(((att1)->foreground_color     == (att2)->foreground_color) &&     \
		 ((att1)->character_set        == (att2)->character_set) &&        \
		 ((att1)->extended_highlight   == (att2)->extended_highlight) &&   \
		 ((att1)->transparency         == (att2)->transparency) &&         \
		 ((att1)->background_color     == (att2)->background_color))




struct field_attribute_type {
    unsigned char   foreground_color;
    unsigned char   character_set;
    unsigned char   extended_highlight;
    unsigned char   background_color;
    unsigned char   transparency;
    unsigned char   field_outlining;
    unsigned char   field_validation;
    unsigned char   field_attribute;
};

struct field_type {
    struct field_type *next_field;
    struct field_type *prev_field;
    unsigned int    start_address;
    unsigned char   flags;
    struct field_attribute_type attribute;
    struct field_attribute_type prev_att;
    unsigned char   new;
};

typedef struct field_type *FieldPtr;

#define FIELD_NEW	(0x01)
#define FIELD_OLD	(0x02)

#define FIELD_NORMAL	(0x00)
#define FIELD_FIRST	(0x01)
#define FIELD_LAST	(0x02)




#define FIELD_NEXT_FIELD(x)	(x->next_field)
#define FIELD_PREV_FIELD(x)	(x->prev_field)
#define FIELD_END_ADDRESS(x)	(x->next_field->start_address)
#define FIELD_START_ADDRESS(x)	(x->start_address)
#define FIELD_ATTRIBUTE(x)	(x->attribute.field_attribute)

#define FIELD_LAST_FIELD(x)	(x->flags & FIELD_LAST)
#define FIELD_FIRST_FIELD(x)	(x->flags & FIELD_FIRST)


#define FIELD_ATT_TO_COLOR(x)	\
		(x & FA_PROT_MASK) 		\
		? (((x & FA_DISPLAY_MASK) == FA_DISPLAY_INTENSIFIED) \
			? EA_COLOR_WHITE : EA_COLOR_BLUE)		  \
		: (((x & FA_DISPLAY_MASK) == FA_DISPLAY_INTENSIFIED)  \
			? EA_COLOR_RED : EA_COLOR_GREEN)


#define FIELD_ATT_DATA_INVISIBLE(x) \
		((x & FA_DISPLAY_MASK) == FA_DISPLAY_NODISPLAY)

#define FIELD_ATT_DATA_INTENSIFIED(x) \
		((x & FA_DISPLAY_MASK) == FA_DISPLAY_INTENSIFIED)


/*
 * Constants for compare attributes
 *
 * EQUAL 	= attributes are the same
 * DISPLAY_SAME = not equal, but display the same on the screen
 * DISPLAY_TEXT = not equal, change affects only printable characters
 * DISPLAY_ALL  = not equal, changes affect all locations
 */

#define COMPARE_EQUAL 		(0x00)
#define COMPARE_DISPLAY_SAME    (0x01)
#define COMPARE_DISPLAY_TEXT    (0x02)
#define COMPARE_DISPLAY_ALL     (0x03)




#define DYN_SEGMENT		(0x01)
#define SEGMENT_INVISIBLE_MASK    (0x8000)
#define SEGMENT_HIGHLIGHT_MASK    (0x0800)
#define SEGMENT_UNCHAINED_MASK    (0x0080)
#define SEGMENT_PROLOGUE_MASK     (0x0010)
#define SEGMENT_NEWSEGMENT_MASK   (0x0006)
#define SEGMENT_NS_NEW	  	  (0x0000)
#define SEGMENT_NS_APPEND 	  (0x0006)
#define SEGMENT_DATA_MASK   	  (0x0001)


struct segment_type {
    struct segment_type *flink;
    struct segment_type *blink;
    unsigned char   type;
    unsigned char   name[4];
    unsigned short  flags;
    unsigned long   length;
    unsigned char   ps_name[4];
    unsigned char  *segment_data;
    unsigned char  *exec;
};


struct point_type {
    short           x;
    short           y;
};

struct graphics_contex_type {
    struct point_type current_point;
};


struct screen_type_ {
    unsigned char   screen[MAX_SCREEN_SIZE];
    unsigned char   tags[MAX_SCREEN_SIZE]; 
    /* 0 = unchanged, 1 = changed, 2= changed, but
       does not effect what is displaed*/
    struct character_attribute_type attribute[MAX_SCREEN_SIZE];
#ifdef CISCO
    void           *socket;
#else
    unsigned int    socket;
#endif
    unsigned char  *IBM_Buf;
    unsigned char  *IBM_Buf_Ptr;
    unsigned long   IBM_Buf_Size;
    unsigned short *SF_Len_Ptr;
    unsigned long   SF_expand;
    unsigned char  *SF_SDP_Len_Ptr;
    unsigned long   SF_SDP_Expand_Base;
    unsigned char   pt_contex;
    unsigned short  current_address;
    unsigned short  cursor_location;
    unsigned char   addressing_mode;
    unsigned short  current_size;
    unsigned short  current_height;
    unsigned short  current_width;
    unsigned char   current_cell_x;
    unsigned char   current_cell_y;
    unsigned short  primary_width;
    unsigned short  primary_height;
    unsigned char   primary_cell_x;
    unsigned char   primary_cell_y;
    unsigned short  alternate_width;
    unsigned short  alternate_height;
    unsigned char   alternate_cell_x;
    unsigned char   alternate_cell_y;
    unsigned char   partition_mode;
    unsigned char   color_mode;
    unsigned char   screen_clear;
    struct field_type *field_cache;
    struct field_type *last_field;
    struct field_type *field_first;
    struct character_attribute_type default_attribute;
    unsigned int    emulation_mask;
    struct graphics_contex_type *graphics_contex;
    unsigned char   AID;
    void           *log;
    void           *emulation_data;
    void           *interface_data;
    unsigned long   debug;
    unsigned char   yale_improved_null;
    unsigned char   error_state;
    unsigned char   keyboard_state;
    unsigned char   insert_mode;
    unsigned char   column_tabs[132];
    unsigned short  home;
    unsigned short  left_margin;
    unsigned char   transparent_mode;
    unsigned char   pref_reset;
    /*
     * Function Pointers
     */
    void            (*change_size) (void *interface_data,
				                    unsigned short width,
				                    unsigned short height,
				                    unsigned short cell_x,
				                    unsigned short cell_y);
    void            (*put_chars) (void *interface_data,
				                  unsigned char *string,
				                  int len,
				                  unsigned short address,
		                 struct interface_attribute_type * attribute
    );
    void            (*clear_screen) (void *interface_data);
    void            (*set_cursor_location) (void *interface_data,
				                   unsigned short location);
    void            (*delete_char) (void *interface_data,
				                    unsigned short location);
    void            (*ring_bell) (void *interface_data);
    void            (*draw_lines) (void *interface_data, struct point_type * points, short count);
    void            (*status_msg) (void *interface_data,
				                   unsigned long msg_type,
				                   unsigned long msg_code);

    void            (*trans_write) (void *interface_data, unsigned char *buffer, int length);
    void            (*trans_mode) (void *interface_data, unsigned char flag);
    void            (*erase_line) (void *interface_data, unsigned
				   short location);
    unsigned char insert_type;
    int typeahead_buffer[TN3270_TYPEAHEAD_BUF_SIZE];
    int *typeahead_pointer;
    char use_clear_to_end_of_line;
    char display_fields;
};


#define ON		(1)
#define OFF		(2)
#define INSERT_TYPE_3277 (0)
#define INSERT_TYPE_3278 (1)

typedef struct screen_type_ *ScreenPtr;

#define TRANS_OFF	(0)
#define TRANS_READ	(1)
#define TRANS_WRITE	(1)

#define MSG_CURSOR	(1)
#define MSG_STATUS	(2)

#define CURSOR_NORMAL	(1)
#define CURSOR_INSERT 	(2)

#define STATUS_NORMAL		(0)
#define STATUS_PROT_FIELD	(1)
#define STATUS_INSERT_OVERFLOW	(2)
#define STATUS_SLOCK		(3)    /* System Lock */

#define INSERT_OFF		(0)
#define INSERT_ON		(1)

#define KEYBOARD_UNLOCKED	(0)
#define KEYBOARD_LOCKED		(1)

#define YALE_ON	 (1)
#define YALE_OFF (0)

#define ERROR_NONE		(0)
#define ERROR_INSERT_OVERFLOW   (1)
#define ERROR_PROT_FIELD	(2)
#define ERROR_DATASTREAM	(3)
#define ERROR_EMULATOR		(4)





#define DEBUG_BUGS			(0x00000001)
#define DEBUG_INFO			(0x00000002)
#define DEBUG_DATASTREAM		(0x00000004)
#define DEBUG_TELNET			(0x00000005)


#define PARTITION_MODE_IMPLICIT		(0x01)
#define PARTITION_MODE_EXPLICIT		(0x02)


#define EM_BASE		(0x00000001)   /* Base emulation type (bit mask ) */
#define EM_GRAPHICS	(0x00000002)   /* API Graphics */
#define	EM_16_COLORS	(0x00000004)   /* 16 Graphics Colors */


/*
 * Color mode, base or extended
 * dirty means that we have just switched modes and need to
 * update what is on the screen.
 */

#define COLOR_MODE_BASE			(0x01)
#define COLOR_MODE_EXTENDED		(0x02)
#define COLOR_MODE_EXTENDED_DIRTY	(0x03)


#define SCREEN_CLEAR		(0x01)
#define SCREEN_NOT_CLEAR	(0x02)


#define PT_CONTEX_LAST_DATA	(0x01)
#define PT_CONTEX_LAST_ORDER	(0x02)
#define PT_CONTEX_NULL_INSERT	(0x03)

/*
 * Definitions and macro to handle converting buffer addresses in the
 * datastream into standard values
 *
 * We first check the addressing mode we are in, next we look
 * at the high two bits to determine if we have a 12 or a 14 bit address
 */

#define ADDRESS_MODE_1214	(0x01)
#define ADDRESS_MODE_16		(0x02)
#define ADDRESS_14_BIT_MASK	(0xc0)

#define BUFFER_TO_ADDRESS(x,y)		\
	(screen->addressing_mode == ADDRESS_MODE_16) ? ((x *256) + y):	\
	((x & ADDRESS_14_BIT_MASK) == 0) ? ( ((x & 0x3f) << 8) + y) :	\
	(((x & 0x3f) << 6)+ (y &0x3f))


#define ADDRESS_TO_3270_0(y)	((unsigned char)        \
	((screen->addressing_mode == ADDRESS_MODE_16)   \
	 ? ((y &  0xff00) >> 8)	          /* 16 Bit */	\
	 : (screen->current_size >= 0x1000)             \
	   ? (((y & 0xff00) >> 8) & 0x3f) /* 14 Bit */	\
	   : code_table[(y >> 6) & 0x3f]  /* 12 bit */ 	\
	))

#define ADDRESS_TO_3270_1(y)	((unsigned char)       \
	((screen->addressing_mode == ADDRESS_MODE_16)  \
	 ? (y & 0xff)				/* 16 Bit */ 		\
	 : (screen->current_size >= 0x1000)                  		\
	   ? (y & 0xff)       			/* 14 Bit */	 	\
	   : code_table[y&0x3f] 		/* 12 bit */ 		\
	))



#define RESET_CHAR_ATTRIBUTE(x) 	\
	{                                               \
	(x)->character_set = 0;                         \
	(x)->extended_highlight = 0;                    \
	(x)->foreground_color  = 0;                     \
	(x)->background_color  = 0;                     \
	(x)->transparency = 0;				\
}


/*
#define INCREMENT_ADDRESS(x)	x = (x + 1) % screen->current_size
*/
#define INCREMENT_ADDRESS(x)	x = (++x == screen->current_size)? 0 : x
#define DECREMENT_ADDRESS(x)	x = ((x==0) ? (screen->current_size - 1) : x-1)

#define ADDRESS_TO_ROW(x) 	(x / screen->current_width)
#define ADDRESS_TO_COL(x) 	(x % screen->current_width)


extern unsigned const char code_table[64];

struct packet_type {
    unsigned char  *data;
    unsigned char  *exec;
    unsigned int    len;
    unsigned char  *field_data;
    unsigned int    field_len;

};
typedef struct packet_type *PacketPtr;

/*
 * Data Queue MACRO
 *
 * 	These macros manage the queue of bytes going to the IBM
 *	system. They make the ASSUMTION that there is a variable
 *	in the current scope called 'screen' which is a pointer
 *	to the current screen structure
 *
 * 	The QUEUE_LONG and QUEUE_SHORT macro also handle differences in
 *	the IBM and local host byte ordering
 *
 *	These macro probably should be worked on to make them as efficent as
 *	possible.
 */



#ifdef TELNET_PROTOCOL
#ifdef BOUNDS_CHECKING
#define QUEUE_BYTE(x) {			\
	if(screen->IBM_Buf_Ptr == (screen->IBM_Buf+screen->IBM_Buf_Size-1))  \
		QUEUE_FLUSH;                              		     \
		if (x == IAC){						\
			*screen->IBM_Buf_Ptr++ = IAC;			\
			screen->SF_expand ++;
} \

*screen->IBM_Buf_Ptr++ = x;
\
}

#else
#define QUEUE_BYTE(x) {	int ___temp_braindead;		\
		{ ___temp_braindead =					\
		(x == IAC) ?	(*screen->IBM_Buf_Ptr++ = IAC, 		\
			         screen->SF_expand ++) :0 ;}		\
		*screen->IBM_Buf_Ptr++ = x;                             \
	}
#endif
#endif

#define QUEUE_SHORT(x) {		\
	QUEUE_BYTE ((unsigned char) ((x >> 8 ) & 0xff) );\
	QUEUE_BYTE ((unsigned char) ((x      ) & 0xff) );\
}


#define QUEUE_LONG(x) {		\
	QUEUE_BYTE ((unsigned char) ((x >> 24) & 0xff) );\
	QUEUE_BYTE ((unsigned char) ((x >> 16) & 0xff) );\
	QUEUE_BYTE ((unsigned char) ((x >>  8) & 0xff) );\
	QUEUE_BYTE ((unsigned char) ((x      ) & 0xff) );\
}

#define QUEUE_END_OF_TRANS 			\
	{					\
	*screen->IBM_Buf_Ptr++ = IAC;		\
	*screen->IBM_Buf_Ptr++ = EOR;           \
	QUEUE_FLUSH;				\
	}




void queue_start_sf  (ScreenPtr screen,unsigned char x);
void queue_start_sdp (ScreenPtr screen,unsigned char x);
void queue_end_sf (ScreenPtr screen);
void queue_end_sdp (ScreenPtr screen);
void queue_flush (ScreenPtr screen);

#define QUEUE_FLUSH queue_flush (screen);
#define QUEUE_START_SF(x)	queue_start_sf(screen,x);
#define QUEUE_START_SDP(x)	queue_start_sdp(screen,x);
#define QUEUE_END_SF		queue_end_sf(screen);
#define QUEUE_END_SDP		queue_end_sdp(screen);

#endif
