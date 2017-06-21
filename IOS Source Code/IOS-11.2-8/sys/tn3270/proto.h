/* $Id: proto.h,v 3.2.62.1 1996/06/12 19:23:12 pst Exp $
 * $Source: /release/112/cvs/Xsys/tn3270/proto.h,v $
 *------------------------------------------------------------------
 * P R O T O . H
 *
 * July, 1994, Bill Palter
 * 
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: proto.h,v $
 * Revision 3.2.62.1  1996/06/12  19:23:12  pst
 * CSCdi60173:  Eliminate cisco definition of ntohl in favor of
 * swap_xxbit_word (final part)
 *
 * All good things must come to an end.  ntohl() and friends finally have
 * UNIX semantics in California releases and later.
 *
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  18:51:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:37:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:07:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef DEBUG_MODULE
int tn3270_debug (ScreenPtr screen, 
		  unsigned long type,
		  char *format_spec,
		  ...);
#endif

#ifdef CISCO
#ifndef GLUE
int send (void *socket,void *string,int length,unsigned int flags);
#endif
#else
int send (unsigned short socket,void *string,int length,unsigned int flags);
#endif

void fatal (int type);

int keyname_to_int (char *string);

char *int_to_keyname (int key);


struct field_type *field_find (ScreenPtr screen,
				unsigned short address);


struct field_type *field_add (	ScreenPtr screen,
				unsigned int address,
			     	struct field_attribute_type *attribute);

int field_modify (	ScreenPtr screen,
				unsigned short address,
			     	struct field_attribute_type *attribute);


int compare_attributes (struct field_attribute_type *att1,
			struct field_attribute_type *att2);

void field_delete_all (ScreenPtr screen);


void field_delete (ScreenPtr screen,
		unsigned short address);


ScreenPtr allocate_screen (int mode);
void deallocate_screen (ScreenPtr screen);

void tn3270_redraw (ScreenPtr screen,unsigned char arg1);
void field_reset_mdt_all (ScreenPtr screen);
void field_mark_all_new (ScreenPtr screen);

void display_fields (ScreenPtr screen);

void sf_read_partition (ScreenPtr screen,
		   struct packet_type *packet,
		   unsigned long sf_length);
                  
void sf_erase_reset    (ScreenPtr screen,
		   struct packet_type *packet,
		   unsigned long sf_length);
                  

void sf_outbound_3270ds (ScreenPtr screen,
		   struct packet_type *packet,
		   unsigned long sf_length);

void sf_object_picture (ScreenPtr screen,
		   struct packet_type *packet,
		   unsigned long sf_length);
                  
int sf_send_qr (	ScreenPtr screen,
		unsigned char qr_code);





int qr_summary 		(ScreenPtr screen);
int qr_character_sets 	(ScreenPtr screen);
int qr_color 		(ScreenPtr screen);
int qr_highlighting 	(ScreenPtr screen);
int qr_implicit_partition 	(ScreenPtr screen);
int qr_usable_area 		(ScreenPtr screen);
int qr_reply_modes 		(ScreenPtr screen);
int qr_field_outlining 	(ScreenPtr screen);
int qr_null 		(ScreenPtr screen);
int qr_storage_pools 	(ScreenPtr screen);
int qr_transparency 	(ScreenPtr screen);
int qr_segment 		(ScreenPtr screen);
int qr_procedure 		(ScreenPtr screen);
int qr_line_type 		(ScreenPtr screen);
int qr_graphics_color 	(ScreenPtr screen);
int qr_graphics_symbol_sets (ScreenPtr screen);




void write_data (ScreenPtr screen, struct packet_type *packet);
void update_screen (ScreenPtr screen);
void transparent_mode_end (ScreenPtr screen);
void transparent_write (ScreenPtr screen,PacketPtr packet);
void transparent_read_write (ScreenPtr screen,PacketPtr packet);


void update_tags (struct screen_type_ *screen);
void merge_attributes ( struct screen_type_ *screen,
			struct field_type  *field,
			struct character_attribute_type *char_att,
			struct interface_attribute_type *attribute);




void send_field (ScreenPtr screen, struct field_type *field);
void read_short(ScreenPtr screen);
void read_modified(ScreenPtr screen);


void clear_screen(ScreenPtr screen);



void order_sba (ScreenPtr screen,struct packet_type *packet);
void order_sf  (ScreenPtr screen,struct packet_type *packet);
void order_sfe (ScreenPtr screen,struct packet_type *packet);
void order_sa  (ScreenPtr screen,struct packet_type *packet);
void order_mf  (ScreenPtr screen,struct packet_type *packet);
void order_ic  (ScreenPtr screen,struct packet_type *packet);
void order_pt  (ScreenPtr screen,struct packet_type *packet);
void order_ra  (ScreenPtr screen,struct packet_type *packet);
void order_eua (ScreenPtr screen,struct packet_type *packet);
void order_ge  (ScreenPtr screen,struct packet_type *packet);
void order_yale (ScreenPtr screen,struct packet_type *packet);


void tn3270_packet (ScreenPtr screen, struct packet_type *packet);
void cmd_wsf (ScreenPtr screen, struct packet_type *packet);
void cmd_write (ScreenPtr screen, struct packet_type *packet);
void cmd_erase_write (ScreenPtr screen, struct packet_type *packet);
void cmd_erase_write_alt (ScreenPtr screen, struct packet_type *packet);
void cmd_read_modified (ScreenPtr screen);
void cmd_read_buffer (ScreenPtr screen);

void send_byte (unsigned char byte);
void send_string (char *string);

unsigned char *iac_do (unsigned char *cp);
unsigned char *iac_dont (unsigned char *cp);
unsigned char *iac_will (unsigned char *cp);
unsigned char *iac_wont (unsigned char *cp);

void data_from_host (unsigned char *buffer,unsigned short length);



void tn3270_enter_key (ScreenPtr screen,unsigned char arg1);
void tn3270_field_tab (ScreenPtr screen,unsigned char arg1);
void tn3270_clear_key (ScreenPtr screen,unsigned char arg1);
void tn3270_pf_key (ScreenPtr screen, unsigned char key);
void tn3270_pa_key (ScreenPtr screen, unsigned char key);
void tn3270_right_arrow (ScreenPtr screen,unsigned char arg1);
void tn3270_left_arrow (ScreenPtr screen,unsigned char arg1);
void tn3270_up_arrow (ScreenPtr screen,unsigned char arg1);
void tn3270_down_arrow (ScreenPtr screen,unsigned char arg1);
void tn3270_add_character (ScreenPtr screen,unsigned char character);
void tn3270_trans_keystroke (ScreenPtr screen, int key);

void graphics_segment (struct screen_type_ *screen,struct segment_type *segment);

