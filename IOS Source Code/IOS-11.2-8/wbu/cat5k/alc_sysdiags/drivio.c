/* $Id: drivio.c,v 1.1.4.1 1996/05/09 15:01:01 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/drivio.c,v $
 *------------------------------------------------------------------
 * drivio.c -- I/O Utility Functions for diagnostics
 *
 * December 1994, rlb (adapted to ATM line card)
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: drivio.c,v $
 * Revision 1.1.4.1  1996/05/09  15:01:01  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:58  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:21  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:09  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:53:53  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:27  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.1  1995/02/17  23:34:21  rbau
 * initial check-in
 *
*
 *------------------------------------------------------------------
 * $Endlog$
 */
/* 12/21/94  rlb
 *    adapted to Synergy's ATM card
 */

#include	"ctype.h"
#include	"types.h"
#include	"constant.h"

extern	int		sys_clock_display;
extern	int		sys_more_prompt;
extern	int		sys_term_vt100;
extern	int		sys_help_sort;
extern	char		cli_print_buf[];
extern	int		diagExecution;

void put_line(
	char		*line)
{
#ifdef CALYPSO
#ifndef RISC
	if (!diagExecution) {
	   printf(line);
	}
	else {
#endif /* RISC */
#endif /* CALYPSO */
	   while (*line != '\0') {
	      if (*line == '\n')
	         putchar_p('\r');
	      putchar_p(*line++);
	   }
#ifdef CALYPSO
#ifndef RISC
	}
#endif /* RISC */
#endif /* CALYPSO */
}

void get_line(
	char		*line,
	int		length)
{
	register int	i, j;
	char		c;

	i = 0;
	while(1) {

#ifdef CALYPSO
#ifdef RISC
	   c = (char) getchar_p1();
#else /* !RISC */
	   c = (char) getchar_p();
#endif /* RISC */
#else /* CALYPSO */
	   c = (char) getchar_p();
#endif /* CALYPSO */

	   if (c == '\n' || c == '\r' || c == '\0') {
	      break;
	   }

	   switch (c) {
	      case '\b': case DEL:
	         if (i > 0) {
	            putchar_p('\b');
	            putchar_p(' ');
	            putchar_p('\b');
	            i--;
	         }
	         else {
	            if (c == CTL_H)
	               sys_help_sort = ~sys_help_sort & 0x1;
	         }
	         break;
	      case CTL_R:
	         if (sys_term_vt100)
	            set_cli_screen();
	         break;
	      case CTL_P:
	         sys_more_prompt = ~sys_more_prompt & 0x1;
	         break;
	      case CTL_T:
	         if (sys_term_vt100)
	            sys_clock_display = ~sys_clock_display & 0x1;
	         break;
	      case CTL_V:
	         sys_term_vt100 = ~sys_term_vt100 & 0x1;
	         if (sys_term_vt100)
	            set_cli_screen();
	         else
	            reset_cli_screen();
	         break;
	      default:
	         if ((i < (length-1)) && isprint(c)) {
	            putchar_p(c);
	            line[i++] = c;
	         }
	         break;
	   }
	   if (c == CTL_R || c == CTL_V) {
	      i = 0;
	      break;
	   }
	}

	putchar_p('\r');
	putchar_p('\n');

	/* chop off trailing blanks */
	for (i--; i >= 0 && line[i] == ' '; i--);
	line[++i] = '\0';

	/* chop off leading blanks */
	for (i = 0; line[i] == ' '; i++);
	strcpy(&line[0], &line[i]);

	/* chop off intermediate blanks, if more than one */
	for (i = j = 0; line[i] != '\0'; ) {
	   line[j++] = line[i++];
	   if (line[i] == ' ') {
	      line[j++] = ' ';
	      while (line[i] == ' ')
	         i++;
	   }
	}
	line[j] = '\0';
}

#ifdef CALYPSO
#ifndef RISC
void get_pline(
	char		*line,
	int		length)
{
	register int	i, j;
	char		c;

	i = 0;
	while(1) {

	   c = (char) getchar_p();

	   if (c == '\n' || c == '\r' || c == '\0') {
	      break;
	   }

	   switch (c) {
	      case '\b': case DEL:
	         if (i > 0) {
	            putchar_p('\b');
	            putchar_p(' ');
	            putchar_p('\b');
	            i--;
	         }
	         break;
	      default:
	         if (i < (length-1)) {
	            putchar_p(' ');
	            line[i++] = c;
	         }
	         break;
	   }
	}

	putchar_p('\r');
	putchar_p('\n');

	line[i] = '\0';
}
#endif /* RISC */
#endif /* CALYPSO */

void print_buf(
	char		*buf_ptr[],
	int		buf_index)
{
	char		c;
	register int	line, index;
	register int	len, max_len, scr_lines;

	if (sys_term_vt100)
	   scr_lines = 19;
	else
	   scr_lines = 24;

	for (index = max_len = 0; index < buf_index; index++)
	   max_len = (max_len > (len = strlen(buf_ptr[index]))) ? max_len : len;

	for (index = 0, line = 1; index < buf_index; index++, line++) {

	   if (sys_more_prompt) {
	      if (line%scr_lines == 0 && index != (buf_index-1)) {
	         sprintf(cli_print_buf, "--More--(%u%%)",(index*100)/buf_index);
	         put_line(cli_print_buf);
	         while (1) {
	            c = (char) getchar_p();
	            if (isalpha(c) && isupper(c))
	               c = tolower(c);
	            if (c == ' ' || c == '\n' || c == '\r' || 
	                c == 'q' || c == '\b' || c == DEL  || c == ESC || 
	                c == 'b') {

	               sprintf(cli_print_buf,"%c               %c", '\r', '\r');
	               put_line(cli_print_buf);

	               if (c == ' ') {
		          line = 1;
	                  break;
	               }

	               if (c == '\n' || c == '\r') {
	                  line--;
	                  break;
	               }

	               if (c == '\b' || c == DEL || c == 'b') {
	                  line = 1;
	                  if (index > ((2*scr_lines)-2))
	                     index -= ((2*scr_lines)-2);
	                  else
	                     index = 0;
	                  break;
	               }

	               if (c == 'q' || c == ESC) {
	                  break;
	               }

	            }
	         }
	         if (c == 'q' || c == ESC)
	            break;
	      }
	   }

	   if (!strcmp(buf_ptr[index], "-") || !strcmp(buf_ptr[index], "=")) {
	      if (index == buf_index - 1)
		 break;
	      c = buf_ptr[index][0];
	      for (len = 1; len < max_len; len++)
		 buf_ptr[index][len] = c;
	      buf_ptr[index][len-1] = '\n';
	      buf_ptr[index][len] = '\0';
	   }
	   put_line(buf_ptr[index]);
	}
}

