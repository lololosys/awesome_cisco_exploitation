/* $Id: tn3270_private.h,v 3.2.62.1 1996/03/27 23:29:08 irfan Exp $
 * $Source: /release/111/cvs/Xsys/tn3270/tn3270_private.h,v $
 *------------------------------------------------------------------
 * T N 3 2 7 0 _ P R I V A T E . H - Prototypes for the tn3270 routines.
 *
 * June 1991, Robert Snyder
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tn3270_private.h,v $
 * Revision 3.2.62.1  1996/03/27  23:29:08  irfan
 * CSCdi51821:  tn3270 should allow typeahead
 * Branch: California_branch
 * Add typeahead buffer to hold keyboard input while keyboard appears
 * locked. Add a TN3270 UI knob to control this behaviour. Reduce the
 * amount
 * of whitespace painted
 *
 * Revision 3.2  1995/11/17  18:51:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:37:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:08:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef TN3270_PRIVATE_
#define TN3270_PRIVATE_

/*
 * Forward struct reference
 */
typedef struct vtinfo_type vtinfo_type_;
typedef struct state_type state_type_;

/*
 * TN3270 character mapping cruft
 */

#define MIN_ASCII		0x00
#define MAX_ASCII		0xff
#define MIN_EBCDIC		0x40
#define MAX_EBCDIC		0xff

/*
 * Function codes for tn3270_show_command
 */

#define TN3270_SHOW_CHARMAP	0x00
#define TN3270_SHOW_ASCIIVAL	0x01

/*
 * Global variables (in tn3270_glue.c)...
 */

extern boolean tn3270_use_8bit_display;          /* Use 8bit display */
extern boolean tn3270_use_8bit_transparent_mode; /* " 8bit transparent mode? */
extern boolean tn3270_use_extended_term;         /* " extended terminal type */
extern boolean tn3270_use_3270_null_processing;  /* " 3270 style null proc? */
extern boolean tn3270_require_reset_key;         /* Require reset on error? */
extern boolean tn3270_typeahead;                 /* Typeahead? */

/*
 * In tn3270_glue.c
 */ 

extern void tt_keystroke(ScreenPtr screen, unsigned char input);
extern void vt_info_area(struct vtinfo_type *vtinfo, unsigned long msg_code);
extern void vt_cursor_msg(struct vtinfo_type *vtinfo, unsigned long msg_code);
extern void vt_status_msg(struct vtinfo_type *vtinfo, unsigned long msg_type,
			  unsigned long msg_code);
extern void vt_trans_write(struct vtinfo_type *vtinfo, unsigned char
			   *buffer, int len); 
extern void vt_trans_mode(struct vtinfo_type *vtinfo, unsigned char flag);
extern char *vt_3270_color_to_vt_color(int color, struct vtinfo_type * vtinfo);
extern void vt_clear_screen(struct vtinfo_type * vtinfo);
extern void vt_ring_bell(struct vtinfo_type * vtinfo);
extern void vt_change_size(struct vtinfo_type * vtinfo,
			   unsigned short width,
			   unsigned short height,
			   unsigned short cell_x,
			   unsigned short cell_y);
extern void vt_set_cursor_location(struct vtinfo_type * vtinfo,
				   unsigned short location);
extern void vt_put_chars(struct vtinfo_type * vtinfo, unsigned char *string,
			 int len, unsigned short address,
			 struct interface_attribute_type * attribute);
extern void tn3270_keystroke(ScreenPtr screen,int key);
extern void vt_erase_line(struct vtinfo_type * vtinfo, unsigned short
		   address);

/*
 * In globals.c
 */ 

void tn3270_init (subsystype *subsys);

/*
 * In map3270.c
 */

extern char *keysgeneric[];
extern tty_3270def *keymap_list; /* pointer to 3270 key maps tn3270 uses */
void init_keymap (struct vtinfo_type *vtinfo);
void tn3270_keystroke (ScreenPtr screen, int key);
void FreeNode (struct state_type *state);

/*
 * In tn3270config.c
 */

extern void nv_3270save(tty_3270def *list, char *s);

/*
 * tn3270show.c
 */ 
extern void show_all_ttycap(void);
extern void show_ttycap_specific(char *ttycapname);
extern void show_ttycap_in_use(void);
extern void show_all_keymaps(void);
extern void show_keymap_specific(char *keymapname);
extern void show_keymap_in_use(void);

/*
 * tn3270_chain.c
 */
extern void tn3270_parser_init(void);
#endif /* TN3270_PRIVATE_ */
