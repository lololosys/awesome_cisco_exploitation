/* $Id: stty.c,v 1.1.4.1 1996/05/09 15:00:33 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sarpmon/stty.c,v $
 *------------------------------------------------------------------
 * stty.c -- PMON source.
 *
 * 
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: stty.c,v $
 * Revision 1.1.4.1  1996/05/09  15:00:33  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:30  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:47  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:42  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:38:16  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:11  rlowe
 * Placeholders for California.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <stdio.h>
#include <termio.h>
#include <string.h>
#include "pmon.h"

struct brate {
	char *str;
	int value;
	};

struct brate btable[] = {
	{"50",B50},
	{"75",B75},
	{"110",B110},
	{"134",B134},
	{"200",B200},
	{"150",B150},
	{"300",B300},
	{"600",B600},
	{"1200",B1200},
	{"1800",B1800},
	{"2400",B2400},
	{"4800",B4800},
	{"9600",B9600},
	{"19200",B19200},
	{"38400",B38400},
	{0}};

Optdesc stty_opts[] = {
	{"-v","list possible baud rates and terminal types"},
	{"-a","list all settings"},
	{"<baud>","set baud rate"},
	{"<term>","set terminal type"},
	{"sane","set sane settings"},
	{"ixany","allow any char to restart output"},
	{"-ixany","allow only <start> to restart output"},
	{"ixoff","enable tandem mode"},
	{"-ixoff","disable tandem mode"},
	{0}};

/*************************************************************
*  stty(ac,av)
*	the 'stty' command 
*/
stty(ac,av)
int ac;
char *av[];
{
int fd,i,flag,aflag,vflag,j;
struct termio tbuf;
char buf[16];

fd = STDIN;
flag = aflag = vflag = 0;
for (i=1;i<ac;i++) {
	if (av[i][0] == '-') { /* option */
		if (strequ(av[i],"-ixoff")) {
			flag = 1;
			ioctl(fd,TCGETA,&tbuf);
			tbuf.c_iflag &= ~IXOFF;
			ioctl(fd,TCSETAF,&tbuf);
			}
		else if (strequ(av[i],"-ixany")) {
			flag = 1;
			ioctl(fd,TCGETA,&tbuf);
			tbuf.c_iflag &= ~IXANY;
			ioctl(fd,TCSETAF,&tbuf);
			}
		else for (j=1;av[i][j] != 0;j++) {
			if (av[i][j] == 'a') aflag = 1;
			else if (av[i][j] == 'v') vflag = 1;
			}
		}
	else if (isdigit(av[i][0])) { /* set baud rate */
		flag = 1;
		ioctl(fd,TCGETA,&tbuf);
		for (j=0;btable[j].str != 0;j++) {
			if (strequ(av[i],btable[j].str)) break;
			}
		if (btable[j].str == 0) {
			printf("%s: unrecognized baud rate\n",av[i]);
			continue;
			}
		tbuf.c_cflag &= ~CBAUD;
		tbuf.c_cflag |= btable[j].value;
		ioctl(fd,TCSETAF,&tbuf);
		}
	else if (strequ(av[i],"ixoff")) {
		flag = 1;
		ioctl(fd,TCGETA,&tbuf);
		tbuf.c_iflag |= IXOFF;
		ioctl(fd,TCSETAF,&tbuf);
		}
	else if (strequ(av[i],"ixany")) {
		flag = 1;
		ioctl(fd,TCGETA,&tbuf);
		tbuf.c_iflag |= IXANY;
		ioctl(fd,TCSETAF,&tbuf);
		}
	else if (strequ(av[i],"sane")) {
		flag = 1;
		ioctl(fd,SETSANE);
		}
	else if (strpat(av[i],"tty?")) { /* select device */
		fd = open(av[i],0);
		if (fd == -1) {
			printf("can't open %s\n",av[i]);
			return;
			}
		}
	else { /* set term type */
		flag = 1;
		if (ioctl(fd,SETTERM,av[i]) == -1) {
			printf("term type %s not found\n",av[i]);
			continue;
			}
		}
	}

if (vflag) {
	printf("Baud rates:\n\t");
	for (i=0;btable[i].str != 0;i++) {
		printf("%5s ",btable[i].str);
		if (i == 8) printf("\n\t");
		}
	printf("\n");
	printf("Terminal types:\n");
	for (i=0;(i = ioctl(i,TERMTYPE,buf)) != -1;) {
		if ((i%6) == 0) printf("\n");
		printf("%10s ",buf);
		}
	printf("\n");
	}
	
if (flag) return;

flag = 0;
if (ioctl(fd,GETTERM,buf) != -1) {
	printf("term=%s ",buf);
	flag = 1;
	}
ioctl(fd,TCGETA,&tbuf);
for (i=0;btable[i].str != 0;i++) {
	if (tbuf.c_cflag == btable[i].value) break;
	}
if (btable[i].str != 0) {
	printf("baud=%s",btable[i].str);
	flag = 1;
	}
if (flag) printf("\n");

if (!aflag) return;

printf("%sistrip ",((tbuf.c_iflag&ISTRIP)?"":"-")); 
printf("%sixon ",((tbuf.c_iflag&IXON)?"":"-")); 
printf("%sixany ",((tbuf.c_iflag&IXANY)?"":"-"));
printf("%sixoff ",((tbuf.c_iflag&IXOFF)?"":"-"));
printf("%sicanon ",((tbuf.c_lflag&ICANON)?"":"-"));
printf("%secho ",((tbuf.c_lflag&ECHO)?"":"-"));
printf("%sechoe ",((tbuf.c_lflag&ECHOE)?"":"-"));
printf("%sicrnl ",((tbuf.c_iflag&ICRNL)?"":"-")); 
printf("%sonlcr ",((tbuf.c_oflag&ONLCR)?"":"-"));
printf("\n");

printf("erase=%s ",cc2str(buf,tbuf.c_cc[VERASE]));
printf("stop=%s ",cc2str(buf,tbuf.c_cc[V_STOP]));
printf("start=%s ",cc2str(buf,tbuf.c_cc[V_START]));
printf("eol=%s ",cc2str(buf,tbuf.c_cc[VEOL]));
printf("eol2=%s ",cc2str(buf,tbuf.c_cc[VEOL2]));
printf("vintr=%s ",cc2str(buf,tbuf.c_cc[VINTR]));
printf("\n");
}

/*************************************************************
*  getbaudrate(p)
*	convert string to baudrate
*/
getbaudrate(p)
char *p;
{
int j;

for (j=0;btable[j].str != 0;j++) {
	if (strequ(p,btable[j].str)) return(btable[j].value);
	}
return(0);
}

/*************************************************************
*  getstty(dst,dev)
*	get stty settings for specified device.
*/
getstty(dst,dev)
char *dst;
int dev;
{
int fd,i;
char tmp[20];
struct termio tbuf;

sprintf(tmp,"/dev/tty%d",dev);
fd = open(tmp,0);
if (fd == EOF) return(1);
ioctl(fd,TCGETA,&tbuf);
ioctl(fd,GETTERM,tmp);
close(fd);
for (i=0;btable[i].str;i++) {
	if (tbuf.c_cflag == btable[i].value) break;
	}
if (btable[i].str == 0) return(1);
sprintf(dst,"stty tty%d %s %s %sixany %sixoff\n",dev,btable[i].str,tmp,
	((tbuf.c_iflag&IXANY)?"":"-"), ((tbuf.c_iflag&IXOFF)?"":"-"));
return(0);
}
