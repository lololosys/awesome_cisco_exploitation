/* $Id: texti_keymaps.h,v 3.3.62.3 1996/09/11 23:07:04 snyder Exp $
 * $Source: /release/112/cvs/Xsys/os/texti_keymaps.h,v $
 *
 *------------------------------------------------------------------
 * texti_keymaps.h - keymaps for readline
 *
 * July 1991, Robert Widmer
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: texti_keymaps.h,v $
 * Revision 3.3.62.3  1996/09/11  23:07:04  snyder
 * CSCdi68880:  more const stuff
 *              120 out of image, 144 data
 * Branch: California_branch
 *
 * Revision 3.3.62.2  1996/08/07  09:01:59  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.3.62.1  1996/06/05  15:05:47  widmer
 * CSCdi57346:  Add printf style formatting to readline prompt
 * Branch: California_branch
 *
 * Revision 3.3  1995/11/17  18:52:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:19:06  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:48:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:59:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/* Readline prototypes */

static void	TI_texti_init(texti_struct*, const char *, va_list);

/*  Primitives  */
static int	TI_forward_space_char(register texti_struct*);
static int	TI_back_space_char(register texti_struct*);
static int	TI_insert_char(register texti_struct*);
static int	TI_delete_char(register texti_struct*);

/*  Insertions  */
static int	TI_insert_string(register texti_struct*, register char*, int);
static int	TI_tab(texti_struct*);
static int	TI_retype_line(texti_struct*);

/*  Movement  */
static int	TI_move_to_bol(register texti_struct*);
static int	TI_move_to_eol(register texti_struct*);
static int	TI_back_space_word(register texti_struct*);
static int	TI_forward_space_word(texti_struct*);

/*  Deletions  */
static int	TI_back_space_delete_char(texti_struct*);
static int	TI_kill_to_eol(register texti_struct*);
static int	TI_kill_to_bol(register texti_struct*);
static int	TI_back_word_delete(register texti_struct*);
static int	TI_delete_word(register texti_struct*);

/*  Misc. Editing  */
static int	TI_capitalize_word(register texti_struct*);
static int	TI_lowercase_word(register texti_struct*);
static int	TI_uppercase_word(register texti_struct*);
static int	TI_transpose_char(texti_struct*);
static int	TI_escape(texti_struct*);
static int	TI_quoted_insert(texti_struct*);
static int	TI_arrow(texti_struct*);
static int	TI_INVALID_CHAR(texti_struct*);

/*  Kill & Yank  */
static int	TI_yank(texti_struct*);
static int	TI_rotate_yank(register texti_struct*);
static void	TI_add_kill(texti_struct*, char*, int);
static char	*TI_rotate_ring(texti_struct*, int);
static void	TI_free_kill(tt_soc*);

/*  History Functions  */
static int	TI_next_history(texti_struct*);
static int	TI_prev_history(texti_struct*);
static int	TI_beginning_of_history(texti_struct *);
static int	TI_end_of_history(texti_struct *);
static int	TI_add_history(texti_struct *, register char *);
static char	*TI_get_next_history(texti_struct *);
static char	*TI_get_prev_history(texti_struct *);
static void	TI_free_history(texti_history_struct *);
static void	TI_history_size(texti_history_struct *, int);
static void	history_size(int);
static void	history_off(void);
static void     history_on(void);
static void	TI_free_history(texti_history_struct *);

/*  Display Functions  */
static void    TI_display(texti_struct*);
static void    TI_replace_line(char*, char*, int, int);
static void    TI_move_cursor(register int, register int*, char*);
static void    TI_scroll(register texti_struct*);
static int     TI_char_len(register char);

/*  texti free  */
static void TI_free(void **);


static const texti_function editing_functions[256]={
	TI_INVALID_CHAR,		/* ^@, NULL  */
	TI_move_to_bol,			/* ^A  */
	TI_back_space_char,		/* ^B  */
	TI_INVALID_CHAR,		/* ^C  */
	TI_delete_char,			/* ^D  */
	TI_move_to_eol,			/* ^E  */
	TI_forward_space_char,		/* ^F  */
	TI_INVALID_CHAR,		/* ^G  */
	TI_back_space_delete_char,	/* ^H  */
	TI_tab,				/* ^I, TAB  */
	TI_INVALID_CHAR,		/* ^J  */
	TI_kill_to_eol,			/* ^K  */
	TI_retype_line,			/* ^L  */
	TI_INVALID_CHAR,		/* ^M  */
	TI_next_history,		/* ^N  */
	TI_INVALID_CHAR,		/* ^O  */
	TI_prev_history,		/* ^P  */
	TI_INVALID_CHAR,		/* ^Q  */
	TI_retype_line,			/* ^R  */
	TI_INVALID_CHAR,		/* ^S  */
	TI_transpose_char,		/* ^T  */
	TI_kill_to_bol,			/* ^U  */
	TI_quoted_insert,		/* ^V  */
	TI_back_word_delete,		/* ^W  */
	TI_kill_to_bol,			/* ^X  */
	TI_yank,			/* ^Y  */
	TI_INVALID_CHAR,		/* ^Z  */
	TI_escape,			/* Esc */
	TI_INVALID_CHAR,		/* 28  */
	TI_INVALID_CHAR,		/* 29  */
	TI_INVALID_CHAR,		/* 30  */
	TI_INVALID_CHAR,		/* 31  */
	TI_insert_char,			/*Space*/
	TI_insert_char,			/*  !  */
	TI_insert_char,			/*  "  */
	TI_insert_char,			/*  #  */
	TI_insert_char,			/*  $  */
	TI_insert_char,			/*  %  */
	TI_insert_char,			/*  &  */
	TI_insert_char,			/*  '  */
	TI_insert_char,			/*  (  */
	TI_insert_char,			/*  )  */
	TI_insert_char,			/*  *  */
	TI_insert_char,			/*  +  */
	TI_insert_char,			/*  ,  */
	TI_insert_char,			/*  -  */
	TI_insert_char,			/*  .  */
	TI_insert_char,			/*  /  */
	TI_insert_char,			/*  0  */
	TI_insert_char,			/*  1  */
	TI_insert_char,			/*  2  */
	TI_insert_char,			/*  3  */
	TI_insert_char,			/*  4  */
	TI_insert_char,			/*  5  */
	TI_insert_char,			/*  6  */
	TI_insert_char,			/*  7  */
	TI_insert_char,			/*  8  */
	TI_insert_char,			/*  9  */
	TI_insert_char,			/*  :  */
	TI_insert_char,			/*  ;  */
	TI_insert_char,			/*  <  */
	TI_insert_char,			/*  =  */
	TI_insert_char,			/*  >  */
	TI_insert_char,			/*  ?  */
	TI_insert_char,			/*  @  */
	TI_insert_char,			/*  A  */
	TI_insert_char,			/*  B  */
	TI_insert_char,			/*  C  */
	TI_insert_char,			/*  D  */
	TI_insert_char,			/*  E  */
	TI_insert_char,			/*  F  */
	TI_insert_char,			/*  G  */
	TI_insert_char,			/*  H  */
	TI_insert_char,			/*  I  */
	TI_insert_char,			/*  J  */
	TI_insert_char,			/*  K  */
	TI_insert_char,			/*  L  */
	TI_insert_char,			/*  M  */
	TI_insert_char,			/*  N  */
	TI_insert_char,			/*  O  */
	TI_insert_char,			/*  P  */
	TI_insert_char,			/*  Q  */
	TI_insert_char,			/*  R  */
	TI_insert_char,			/*  S  */
	TI_insert_char,			/*  T  */
	TI_insert_char,			/*  U  */
	TI_insert_char,			/*  V  */
	TI_insert_char,			/*  W  */
	TI_insert_char,			/*  X  */
	TI_insert_char,			/*  Y  */
	TI_insert_char,			/*  Z  */
	TI_insert_char,			/*  [  */
	TI_insert_char,			/*  \  */
	TI_insert_char,			/*  ]  */
	TI_insert_char,			/*  ^  */
	TI_insert_char,			/*  _  */
	TI_insert_char,			/*  `  */
	TI_insert_char,			/*  a  */
	TI_insert_char,			/*  b  */
	TI_insert_char,			/*  c  */
	TI_insert_char,			/*  d  */
	TI_insert_char,			/*  e  */
	TI_insert_char,			/*  f  */
	TI_insert_char,			/*  g  */
	TI_insert_char,			/*  h  */
	TI_insert_char,			/*  i  */
	TI_insert_char,			/*  j  */
	TI_insert_char,			/*  k  */
	TI_insert_char,			/*  l  */
	TI_insert_char,			/*  m  */
	TI_insert_char,			/*  n  */
	TI_insert_char,			/*  o  */
	TI_insert_char,			/*  p  */
	TI_insert_char,			/*  q  */
	TI_insert_char,			/*  r  */
	TI_insert_char,			/*  s  */
	TI_insert_char,			/*  t  */
	TI_insert_char,			/*  u  */
	TI_insert_char,			/*  v  */
	TI_insert_char,			/*  w  */
	TI_insert_char,			/*  x  */
	TI_insert_char,			/*  y  */
	TI_insert_char,			/*  z  */
	TI_insert_char,			/*  {  */
	TI_insert_char,			/*  |  */
	TI_insert_char,			/*  }  */
	TI_insert_char,			/*  ~  */
	TI_back_space_delete_char,	/* DEL */

	TI_insert_char,			/*  \128 */
	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,

	TI_insert_char,			/*  \144 */
	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,

	TI_insert_char,			/*  \160 */
	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,

	TI_insert_char,			/*  \176 */
	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,

	TI_insert_char,			/*  \192 */
	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,

	TI_insert_char,			/*  \208 */
	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,

	TI_insert_char,			/*  \224 */
	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,

	TI_insert_char,			/*  \240 */
	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,
};

static const texti_function non_editing_functions[256]={
	TI_INVALID_CHAR,		/* ^@, NULL  */
	TI_insert_char,			/* ^A  */
	TI_insert_char,			/* ^B  */
	TI_insert_char,			/* ^C  */
	TI_insert_char,			/* ^D  */
	TI_insert_char,			/* ^E  */
	TI_insert_char,			/* ^F  */
	TI_INVALID_CHAR,		/* ^G  */
	TI_back_space_delete_char,	/* ^H  */
	TI_tab,				/* ^I, TAB  */
	TI_insert_char,			/* ^J  */
	TI_insert_char,			/* ^K  */
	TI_insert_char,			/* ^L  */
	TI_insert_char,			/* ^M  */
	TI_next_history,		/* ^N  */
	TI_insert_char,			/* ^O  */
	TI_prev_history,		/* ^P  */
	TI_insert_char,			/* ^Q  */
	TI_retype_line,			/* ^R  */
	TI_insert_char,			/* ^S  */
	TI_insert_char,			/* ^T  */
	TI_kill_to_bol,			/* ^U  */
	TI_quoted_insert,		/* ^V  */
	TI_back_word_delete,		/* ^W  */
	TI_kill_to_bol,			/* ^X  */
	TI_insert_char,			/* ^Y  */
	TI_insert_char,			/* ^Z  */
	TI_escape,			/* Esc */
	TI_insert_char,			/* 28  */
	TI_insert_char,			/* 29  */
	TI_insert_char,			/* 30  */
	TI_insert_char,			/* 31  */
	TI_insert_char,			/*Space*/
	TI_insert_char,			/*  !  */
	TI_insert_char,			/*  "  */
	TI_insert_char,			/*  #  */
	TI_insert_char,			/*  $  */
	TI_insert_char,			/*  %  */
	TI_insert_char,			/*  &  */
	TI_insert_char,			/*  '  */
	TI_insert_char,			/*  (  */
	TI_insert_char,			/*  )  */
	TI_insert_char,			/*  *  */
	TI_insert_char,			/*  +  */
	TI_insert_char,			/*  ,  */
	TI_insert_char,			/*  -  */
	TI_insert_char,			/*  .  */
	TI_insert_char,			/*  /  */
	TI_insert_char,			/*  0  */
	TI_insert_char,			/*  1  */
	TI_insert_char,			/*  2  */
	TI_insert_char,			/*  3  */
	TI_insert_char,			/*  4  */
	TI_insert_char,			/*  5  */
	TI_insert_char,			/*  6  */
	TI_insert_char,			/*  7  */
	TI_insert_char,			/*  8  */
	TI_insert_char,			/*  9  */
	TI_insert_char,			/*  :  */
	TI_insert_char,			/*  ;  */
	TI_insert_char,			/*  <  */
	TI_insert_char,			/*  =  */
	TI_insert_char,			/*  >  */
	TI_insert_char,			/*  ?  */
	TI_insert_char,			/*  @  */
	TI_insert_char,			/*  A  */
	TI_insert_char,			/*  B  */
	TI_insert_char,			/*  C  */
	TI_insert_char,			/*  D  */
	TI_insert_char,			/*  E  */
	TI_insert_char,			/*  F  */
	TI_insert_char,			/*  G  */
	TI_insert_char,			/*  H  */
	TI_insert_char,			/*  I  */
	TI_insert_char,			/*  J  */
	TI_insert_char,			/*  K  */
	TI_insert_char,			/*  L  */
	TI_insert_char,			/*  M  */
	TI_insert_char,			/*  N  */
	TI_insert_char,			/*  O  */
	TI_insert_char,			/*  P  */
	TI_insert_char,			/*  Q  */
	TI_insert_char,			/*  R  */
	TI_insert_char,			/*  S  */
	TI_insert_char,			/*  T  */
	TI_insert_char,			/*  U  */
	TI_insert_char,			/*  V  */
	TI_insert_char,			/*  W  */
	TI_insert_char,			/*  X  */
	TI_insert_char,			/*  Y  */
	TI_insert_char,			/*  Z  */
	TI_insert_char,			/*  [  */
	TI_insert_char,			/*  \  */
	TI_insert_char,			/*  ]  */
	TI_insert_char,			/*  ^  */
	TI_insert_char,			/*  _  */
	TI_insert_char,			/*  `  */
	TI_insert_char,			/*  a  */
	TI_insert_char,			/*  b  */
	TI_insert_char,			/*  c  */
	TI_insert_char,			/*  d  */
	TI_insert_char,			/*  e  */
	TI_insert_char,			/*  f  */
	TI_insert_char,			/*  g  */
	TI_insert_char,			/*  h  */
	TI_insert_char,			/*  i  */
	TI_insert_char,			/*  j  */
	TI_insert_char,			/*  k  */
	TI_insert_char,			/*  l  */
	TI_insert_char,			/*  m  */
	TI_insert_char,			/*  n  */
	TI_insert_char,			/*  o  */
	TI_insert_char,			/*  p  */
	TI_insert_char,			/*  q  */
	TI_insert_char,			/*  r  */
	TI_insert_char,			/*  s  */
	TI_insert_char,			/*  t  */
	TI_insert_char,			/*  u  */
	TI_insert_char,			/*  v  */
	TI_insert_char,			/*  w  */
	TI_insert_char,			/*  x  */
	TI_insert_char,			/*  y  */
	TI_insert_char,			/*  z  */
	TI_insert_char,			/*  {  */
	TI_insert_char,			/*  |  */
	TI_insert_char,			/*  }  */
	TI_insert_char,			/*  ~  */
	TI_back_space_delete_char,	/* DEL */

	TI_insert_char,			/*  \128 */
	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,

	TI_insert_char,			/*  \144 */
	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,

	TI_insert_char,			/*  \160 */
	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,

	TI_insert_char,			/*  \176 */
	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,

	TI_insert_char,			/*  \192 */
	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,

	TI_insert_char,			/*  \208 */
	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,

	TI_insert_char,			/*  \224 */
	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,

	TI_insert_char,			/*  \240 */
	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,
	TI_insert_char,	TI_insert_char,	TI_insert_char,	TI_insert_char,
};

static const texti_function escape_editing_functions[256]={
	TI_INVALID_CHAR,		/* ^@, NULL  */
	TI_INVALID_CHAR,		/* ^A  */
	TI_INVALID_CHAR,		/* ^B  */
	TI_INVALID_CHAR,		/* ^C  */
	TI_INVALID_CHAR,		/* ^D  */
	TI_INVALID_CHAR,		/* ^E  */
	TI_INVALID_CHAR,		/* ^F  */
	TI_INVALID_CHAR,		/* ^G, BELL  */
	TI_INVALID_CHAR,		/* ^H, BS  */
	TI_INVALID_CHAR,		/* ^I, TAB  */
	TI_INVALID_CHAR,		/* ^J  */
	TI_INVALID_CHAR,		/* ^K  */
	TI_INVALID_CHAR,		/* ^L  */
	TI_INVALID_CHAR,		/* ^M  */
	TI_INVALID_CHAR,		/* ^N  */
	TI_INVALID_CHAR,		/* ^O  */
	TI_INVALID_CHAR,		/* ^P  */
	TI_INVALID_CHAR,		/* ^Q  */
	TI_INVALID_CHAR,		/* ^R  */
	TI_INVALID_CHAR,		/* ^S  */
	TI_INVALID_CHAR,		/* ^T  */
	TI_INVALID_CHAR,		/* ^U  */
	TI_INVALID_CHAR,		/* ^V  */
	TI_INVALID_CHAR,		/* ^W  */
	TI_INVALID_CHAR,		/* ^X  */
	TI_INVALID_CHAR,		/* ^Y  */
	TI_INVALID_CHAR,		/* ^Z  */
	TI_INVALID_CHAR,		/* Esc */
	TI_INVALID_CHAR,		/* 28  */
	TI_INVALID_CHAR,		/* 29  */
	TI_INVALID_CHAR,		/* 30  */
	TI_INVALID_CHAR,		/* 31  */
	TI_INVALID_CHAR,		/*Space*/
	TI_INVALID_CHAR,		/*  !  */
	TI_INVALID_CHAR,		/*  "  */
	TI_INVALID_CHAR,		/*  #  */
	TI_INVALID_CHAR,		/*  $  */
	TI_INVALID_CHAR,		/*  %  */
	TI_INVALID_CHAR,		/*  &  */
	TI_INVALID_CHAR,		/*  '  */
	TI_INVALID_CHAR,		/*  (  */
	TI_INVALID_CHAR,		/*  )  */
	TI_INVALID_CHAR,		/*  *  */
	TI_INVALID_CHAR,		/*  +  */
	TI_INVALID_CHAR,		/*  ,  */
	TI_INVALID_CHAR,		/*  -  */
	TI_INVALID_CHAR,		/*  .  */
	TI_INVALID_CHAR,		/*  /  */
	TI_INVALID_CHAR,		/*  0  */
	TI_INVALID_CHAR,		/*  1  */
	TI_INVALID_CHAR,		/*  2  */
	TI_INVALID_CHAR,		/*  3  */
	TI_INVALID_CHAR,		/*  4  */
	TI_INVALID_CHAR,		/*  5  */
	TI_INVALID_CHAR,		/*  6  */
	TI_INVALID_CHAR,		/*  7  */
	TI_INVALID_CHAR,		/*  8  */
	TI_INVALID_CHAR,		/*  9  */
	TI_INVALID_CHAR,		/*  :  */
	TI_INVALID_CHAR,		/*  ;  */
	TI_beginning_of_history,	/*  <  */
	TI_INVALID_CHAR,		/*  =  */
	TI_end_of_history,		/*  >  */
	TI_INVALID_CHAR,		/*  ?  */
	TI_INVALID_CHAR,		/*  @  */
	TI_INVALID_CHAR,		/*  A  */
	TI_INVALID_CHAR,		/*  B  */
	TI_INVALID_CHAR,		/*  C  */
	TI_INVALID_CHAR,		/*  D  */
	TI_INVALID_CHAR,		/*  E  */
	TI_INVALID_CHAR,		/*  F  */
	TI_INVALID_CHAR,		/*  G  */
	TI_INVALID_CHAR,		/*  H  */
	TI_INVALID_CHAR,		/*  I  */
	TI_INVALID_CHAR,		/*  J */
	TI_INVALID_CHAR,		/*  K  */
	TI_INVALID_CHAR,		/*  L  */
	TI_INVALID_CHAR,		/*  M  */
	TI_INVALID_CHAR,		/*  N  */
	TI_arrow,			/*  O  */
	TI_INVALID_CHAR,		/*  P  */
	TI_quoted_insert,		/*  Q  */
	TI_INVALID_CHAR,		/*  R  */
	TI_INVALID_CHAR,		/*  S  */
	TI_INVALID_CHAR,		/*  T  */
	TI_INVALID_CHAR,		/*  U  */
	TI_INVALID_CHAR,		/*  V  */
	TI_INVALID_CHAR,		/*  W  */
	TI_INVALID_CHAR,		/*  X  */
	TI_INVALID_CHAR,		/*  Y  */
	TI_INVALID_CHAR,		/*  Z  */
	TI_arrow,			/*  [  */
	TI_INVALID_CHAR,		/*  \  */
	TI_INVALID_CHAR,		/*  ]  */
	TI_INVALID_CHAR,		/*  ^  */
	TI_INVALID_CHAR,		/*  _  */
	TI_INVALID_CHAR,		/*  `  */
	TI_INVALID_CHAR,		/*  a  */
	TI_back_space_word,		/*  b  */
	TI_capitalize_word,		/*  c  */
	TI_delete_word,			/*  d  */
	TI_INVALID_CHAR,		/*  e  */
	TI_forward_space_word,		/*  f  */
	TI_INVALID_CHAR,		/*  g  */
	TI_INVALID_CHAR,		/*  h  */
	TI_tab,				/*  i  */
	TI_INVALID_CHAR,		/*  j  */
	TI_INVALID_CHAR,		/*  k  */
	TI_lowercase_word,		/*  l  */
	TI_INVALID_CHAR,		/*  m  */
	TI_INVALID_CHAR,		/*  n  */
	TI_INVALID_CHAR,		/*  o  */
	TI_INVALID_CHAR,		/*  p  */
	TI_quoted_insert,		/*  q  */
	TI_INVALID_CHAR,		/*  r  */
	TI_INVALID_CHAR,		/*  s  */
	TI_INVALID_CHAR,		/*  t  */
	TI_uppercase_word,		/*  u  */
	TI_INVALID_CHAR,		/*  v  */
	TI_INVALID_CHAR,		/*  w  */
	TI_INVALID_CHAR,		/*  x  */
	TI_rotate_yank,			/*  y  */
	TI_INVALID_CHAR,		/*  z  */
	TI_INVALID_CHAR,		/*  {  */
	TI_INVALID_CHAR,		/*  |  */
	TI_INVALID_CHAR,		/*  }  */
	TI_INVALID_CHAR,		/*  ~  */
	TI_back_word_delete,		/* DEL */

	TI_INVALID_CHAR,   /*  \128 */
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	
	TI_INVALID_CHAR,   /*  \144 */
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	
	TI_INVALID_CHAR,   /*  \160 */
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	
	TI_INVALID_CHAR,   /*  \176 */
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	
	TI_INVALID_CHAR,   /*  \192 */
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	
	TI_INVALID_CHAR,   /*  \208 */
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	
	TI_INVALID_CHAR,   /*  \224 */
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	
	TI_INVALID_CHAR,   /*  \240 */
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR
};

static const texti_function escape_non_editing_functions[256]={
	TI_INVALID_CHAR,		/* ^@, NULL  */
	TI_INVALID_CHAR,		/* ^A  */
	TI_INVALID_CHAR,		/* ^B  */
	TI_INVALID_CHAR,		/* ^C  */
	TI_INVALID_CHAR,		/* ^D  */
	TI_INVALID_CHAR,		/* ^E  */
	TI_INVALID_CHAR,		/* ^F  */
	TI_INVALID_CHAR,		/* ^G, BELL  */
	TI_INVALID_CHAR,		/* ^H, BS  */
	TI_INVALID_CHAR,		/* ^I, TAB  */
	TI_INVALID_CHAR,		/* ^J  */
	TI_INVALID_CHAR,		/* ^K  */
	TI_INVALID_CHAR,		/* ^L  */
	TI_INVALID_CHAR,		/* ^M  */
	TI_INVALID_CHAR,		/* ^N  */
	TI_INVALID_CHAR,		/* ^O  */
	TI_INVALID_CHAR,		/* ^P  */
	TI_INVALID_CHAR,		/* ^Q  */
	TI_INVALID_CHAR,		/* ^R  */
	TI_INVALID_CHAR,		/* ^S  */
	TI_INVALID_CHAR,		/* ^T  */
	TI_INVALID_CHAR,		/* ^U  */
	TI_INVALID_CHAR,		/* ^V  */
	TI_INVALID_CHAR,		/* ^W  */
	TI_INVALID_CHAR,		/* ^X  */
	TI_INVALID_CHAR,		/* ^Y  */
	TI_INVALID_CHAR,		/* ^Z  */
	TI_INVALID_CHAR,		/* Esc */
	TI_INVALID_CHAR,		/* 28  */
	TI_INVALID_CHAR,		/* 29  */
	TI_INVALID_CHAR,		/* 30  */
	TI_INVALID_CHAR,		/* 31  */
	TI_INVALID_CHAR,		/*Space*/
	TI_INVALID_CHAR,		/*  !  */
	TI_INVALID_CHAR,		/*  "  */
	TI_INVALID_CHAR,		/*  #  */
	TI_INVALID_CHAR,		/*  $  */
	TI_INVALID_CHAR,		/*  %  */
	TI_INVALID_CHAR,		/*  &  */
	TI_INVALID_CHAR,		/*  '  */
	TI_INVALID_CHAR,		/*  (  */
	TI_INVALID_CHAR,		/*  )  */
	TI_INVALID_CHAR,		/*  *  */
	TI_INVALID_CHAR,		/*  +  */
	TI_INVALID_CHAR,		/*  ,  */
	TI_INVALID_CHAR,		/*  -  */
	TI_INVALID_CHAR,		/*  .  */
	TI_INVALID_CHAR,		/*  /  */
	TI_INVALID_CHAR,		/*  0  */
	TI_INVALID_CHAR,		/*  1  */
	TI_INVALID_CHAR,		/*  2  */
	TI_INVALID_CHAR,		/*  3  */
	TI_INVALID_CHAR,		/*  4  */
	TI_INVALID_CHAR,		/*  5  */
	TI_INVALID_CHAR,		/*  6  */
	TI_INVALID_CHAR,		/*  7  */
	TI_INVALID_CHAR,		/*  8  */
	TI_INVALID_CHAR,		/*  9  */
	TI_INVALID_CHAR,		/*  :  */
	TI_INVALID_CHAR,		/*  ;  */
	TI_beginning_of_history,	/*  <  */
	TI_INVALID_CHAR,		/*  =  */
	TI_end_of_history,		/*  >  */
	TI_INVALID_CHAR,		/*  ?  */
	TI_INVALID_CHAR,		/*  @  */
	TI_INVALID_CHAR,		/*  A  */
	TI_INVALID_CHAR,		/*  B  */
	TI_INVALID_CHAR,		/*  C  */
	TI_INVALID_CHAR,		/*  D  */
	TI_INVALID_CHAR,		/*  E  */
	TI_INVALID_CHAR,		/*  F  */
	TI_INVALID_CHAR,		/*  G  */
	TI_INVALID_CHAR,		/*  H  */
	TI_INVALID_CHAR,		/*  I  */
	TI_INVALID_CHAR,		/*  J */
	TI_INVALID_CHAR,		/*  K  */
	TI_INVALID_CHAR,		/*  L  */
	TI_INVALID_CHAR,		/*  M  */
	TI_INVALID_CHAR,		/*  N  */
	TI_arrow,			/*  O  */
	TI_INVALID_CHAR,		/*  P  */
	TI_quoted_insert,		/*  Q  */
	TI_INVALID_CHAR,		/*  R  */
	TI_INVALID_CHAR,		/*  S  */
	TI_INVALID_CHAR,		/*  T  */
	TI_INVALID_CHAR,		/*  U  */
	TI_INVALID_CHAR,		/*  V  */
	TI_INVALID_CHAR,		/*  W  */
	TI_INVALID_CHAR,		/*  X  */
	TI_INVALID_CHAR,		/*  Y  */
	TI_INVALID_CHAR,		/*  Z  */
	TI_arrow,			/*  [  */
	TI_INVALID_CHAR,		/*  \  */
	TI_INVALID_CHAR,		/*  ]  */
	TI_INVALID_CHAR,		/*  ^  */
	TI_INVALID_CHAR,		/*  _  */
	TI_INVALID_CHAR,		/*  `  */
	TI_INVALID_CHAR,		/*  a  */
	TI_INVALID_CHAR,		/*  b  */
	TI_INVALID_CHAR,		/*  c  */
	TI_INVALID_CHAR,		/*  d  */
	TI_INVALID_CHAR,		/*  e  */
	TI_INVALID_CHAR,		/*  f  */
	TI_INVALID_CHAR,		/*  g  */
	TI_INVALID_CHAR,		/*  h  */
	TI_tab,				/*  i  */
	TI_INVALID_CHAR,		/*  j  */
	TI_INVALID_CHAR,		/*  k  */
	TI_INVALID_CHAR,		/*  l  */
	TI_INVALID_CHAR,		/*  m  */
	TI_INVALID_CHAR,		/*  n  */
	TI_INVALID_CHAR,		/*  o  */
	TI_INVALID_CHAR,		/*  p  */
	TI_quoted_insert,		/*  q  */
	TI_INVALID_CHAR,		/*  r  */
	TI_INVALID_CHAR,		/*  s  */
	TI_INVALID_CHAR,		/*  t  */
	TI_INVALID_CHAR,		/*  u  */
	TI_INVALID_CHAR,		/*  v  */
	TI_INVALID_CHAR,		/*  w  */
	TI_INVALID_CHAR,		/*  x  */
	TI_INVALID_CHAR,		/*  y  */
	TI_INVALID_CHAR,		/*  z  */
	TI_INVALID_CHAR,		/*  {  */
	TI_INVALID_CHAR,		/*  |  */
	TI_INVALID_CHAR,		/*  }  */
	TI_INVALID_CHAR,		/*  ~  */
	TI_INVALID_CHAR,		/* DEL */
	TI_INVALID_CHAR,   /*  \128 */
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	
	TI_INVALID_CHAR,   /*  \144 */
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	
	TI_INVALID_CHAR,   /*  \160 */
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	
	TI_INVALID_CHAR,   /*  \176 */
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	
	TI_INVALID_CHAR,   /*  \192 */
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	
	TI_INVALID_CHAR,   /*  \208 */
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	
	TI_INVALID_CHAR,   /*  \224 */
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	
	TI_INVALID_CHAR,   /*  \240 */
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR,
	TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR, TI_INVALID_CHAR
};

static const struct arrow_struct {
	texti_function	up_arrow;
	texti_function	down_arrow;
	texti_function	right_arrow;
	texti_function	left_arrow;
} arrow_functions = {
	TI_prev_history,		/*  Up   */
	TI_next_history,		/* Down  */
	TI_forward_space_char,		/* Right */
	TI_back_space_char,		/* Left  */
};
