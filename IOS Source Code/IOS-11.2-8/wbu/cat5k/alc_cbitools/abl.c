/* $Id: abl.c,v 1.1.4.2 1996/06/11 02:36:49 venkat Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_cbitools/abl.c,v $
 *-----------------------------------------------------------------
 * abl.c -- Synergy ATM line card binary linker tool.
 *		(adapted from NMP bl.c).
 *
 * July 13, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: abl.c,v $
 * Revision 1.1.4.2  1996/06/11  02:36:49  venkat
 * CSCdi60077:  Build tools under wbu independent of the Host OS
 * Branch: California_branch
 * Built a wrapper function for buginf for the tools abl.c
 * csumaout.c. Makefile changes to accomodate the whole tuple
 * of uname(1) to figure out the host OS.
 *
 * Revision 1.1.4.1  1996/05/09  14:54:25  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:13:49  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:31  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:09:51  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.2  1996/03/20  10:52:24  rlowe
 * Integrate with ios/wbu sub-tree and makefiles.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:40:50  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:21  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.8.2.1  1995/11/15  23:16:22  rbau
 * merge mainline code changes in 1.4 branch
 *
 * Revision 1.9  1995/11/15  21:23:47  rbau
 * enable support for off-line diags sar code
 *
 * Revision 1.8  1995/11/02  04:00:45  rlowe
 * Add (but disable) support for Tx/Rx SAR manufacturing loopback diag code.
 *
 * Revision 1.7  1995/10/13  06:07:01  rlowe
 * Enable and use BCS checksums in PROM and flash Boots.
 *
 * Revision 1.6  1995/09/06  03:59:07  rlowe
 * Fixes so that abl tool will compile/link/run on either SunOS or Solaris
 * machines.  Always generate the tool from scratch now when invoked.
 *
 * Revision 1.5  1995/08/31  12:27:36  rlowe
 * Changes to work with IOS 11.0 and higher.
 * SunOS and Solaris have different descriptions for free; and different
 * locations for the a.out header "exec" structure.
 *
 * Revision 1.4  1995/08/01  07:48:54  rlowe
 * Add basic support for BCS full-image version record.
 *
 * Revision 1.3  1995/07/31  23:23:37  rlowe
 * Fix to NOT append a newline at the end of date/time expansions.
 *
 * Revision 1.2  1995/07/31  21:55:18  rlowe
 * Cleanup run-time output a bit.
 *
 * Revision 1.1  1995/07/27  10:33:59  rlowe
 * Code for ATM line card binary linker (ABL), which runs on SUN as
 * part of the ALC image build procedure.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This file contains the source for the Combined Binary Linker.
 * The Combined Binary Linker (Build/Glue Utility) takes disassociated
 * executable binary load images and combines them into one integrated
 * download image.  Each executable binary receives a header describing
 * some attributes of the contents and owner of the binary.  The 
 * resulting integrated load image receives a header describing how
 * many individual modules were combined, among other things.
 *
 * rfa 950208-09-10-0328-0404
 */
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#ifdef	__svr4__
#include <sys/exechdr.h>		/* For Solaris */
#else
#include <sys/exec.h>			/* For SunOS */
#endif
#include <errno.h>
#include <malloc.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdarg.h>

#include "libcom.h"
#include "acp_hw_defs.h"
#include "acp_image_map.h"
#include "bcs_api.h"
#include "../alc_bcs/bcs_image_hdr.h"
#include "../alc_bcs/bcs_commonIF.h"
#include "acbl.h"


/*
 *	Forward definitions and other data structures.
 */

#define FORWARD	/**/

FORWARD void ABL_PrtUsage () ;
FORWARD void ABL_Set_TraceLevel () ;
FORWARD void ABL_Set_TL_Debug () ;
FORWARD void ABL_Set_TL_Dump () ;
FORWARD void ABL_Set_TL_Verb () ;
FORWARD u_long ABL_GetHexAddr () ;
FORWARD void ABL_SetBCS_SvrPt () ;
FORWARD void ABL_SetFlashBase () ;
FORWARD void ABL_Set_OFile () ;
FORWARD void ABL_Set_ImgVers () ;
FORWARD void ABL_FatalError () ;
FORWARD char * ABL_FindArgVal () ;
FORWARD u_long ABL_Parse_Args () ;
FORWARD u_long ABL_Parse_Images () ;
FORWARD void ABL_ImageQuals () ;
FORWARD void ABL_SetString () ;
FORWARD void ABL_Set_ID_BaseAddr () ;
FORWARD void ABL_Set_ID_FileName () ;
FORWARD void ABL_Set_ID_Version () ;
FORWARD void ABL_ReadInpF () ;
FORWARD void ABL_SeekFile () ;
FORWARD void ABL_WriteOutF () ;
FORWARD void ABL_Write_CBI () ;
FORWARD void ABL_WriteGap () ;
FORWARD void ABL_ExpandPath () ;
FORWARD void ABL_LoadFile () ;
FORWARD void ABL_LoadVers () ;
FORWARD void ABL_LoadExploder () ;
FORWARD void ABL_LoadImages () ;
FORWARD void ABL_WriteBCS () ;
FORWARD void ABL_WriteOCS () ;
FORWARD u_long ABL_ReadFileHdr () ;


#define	ABL_BLANKCHAR	' '
#define	ABL_BUFSIZ	(((BUFSIZ) * 4) * 16)	/* Ensure whole long-words. */
#define	ABL_LINEBUF_SZ	(80)
#define	ABL_NAME_SIZE	(BCS_IHL_IMAGE_NAME)
#define	ABL_NBR_IMAGES	(BCS_IH_TYPE_LAST + 1)	/* Include exploder. */

/*
 *	This table drives the ACP image combination link.  It is aware of
 *	each of the sub-images that must be included to effect a complete
 *	flash-ready image, including the embedded exploder.  BCS headers
 *	are generated as part of each sub-image descriptor (not relevant
 *	for the exploder) as input files are processed.
 *	Default values are provided where possible.
 */

typedef struct ABL_CmdLine	/* Per-image command-line data. */
{
	char *	CmdLineTag ;	/* Use on cmd-line to override default. */
	char *	CmdHelpStr ;	/* For help/usage output. */
	u_long	ImageBase ;	/* Image base address in target. */
	char	F_Name [ABL_NAME_SIZE] ;
	char	F_Vers [BCS_IHL_IMAGE_VERSION] ;
} tABL_CmdLine ;

#define	ABL_CMDLINE_INIT(t,h,b,n,v)	{ t, h, b, n, v, }

typedef struct ABL_RT_Data	/* Per-image data seen at run-time. */
{
	u_long	SpecFlags  ;	/* Special image/descriptor flags. */
	u_long	FileSize   ;	/* From actual image file. */
	u_long	EntryPt    ;	/* From actual image file. */
	u_long	CheckSum   ;	/* From image contents. */
	u_long	FlashOfst  ;	/* Offset to image in flash. */
	char	F_Date [BCS_IHL_DATE_TIME] ;	/* Sub-image creation date. */
	char	I_Date [BCS_IHL_DATE_TIME] ;	/* For installation date. */
} tABL_RT_Data ;

#define	ABL_SF_NONE	0	/* Bit values for ABL_RT_Data.SpecFlags field.*/
#define	ABL_SF_AOUT	0x1	/* Set if a.out instead of flat binary image. */
#define	ABL_SF_DUPLICAT	0x2	/* Set if duplicate (BCS) header. */
#define	ABL_SF_NOT_BCS	0x4	/* Set if not a BCS component image. */
#define	ABL_SF_INCLHDR	0x8	/* Set to include binary file header (a.out). */
#define	ABL_SF_RUNFLSH	0x10	/* Set to indicate run-from-flash image. */
#define	ABL_SF_FIRSTHD	0x20	/* Set for first BCS header. */
#define	ABL_SF_VERINFO	0x40	/* Set for Version info BCS header. */
#define	ABL_SF_DONE	0x80000000	/* Set once image written to output. */

#define	ABL_RUNTIME_INIT(flg)	{ flg, 0, BCS_IH_NO_ACP_ENTRY, ABL_CKSUM_SEED, 0, "", "", }

typedef struct ABL_ImageDesc	/* Per-image descriptor. */
{
	tABL_CmdLine	CL ;	/* Cmd-line/default values. */
	tABL_RT_Data	RT ;	/* Run-time data. */
} tABL_ImageDesc ;

#define	ABL_IDIT_EXPLODER	0	/* Tbl index of exploder entry. */

tABL_ImageDesc	ABL_ImageTbl [ABL_NBR_IMAGES] =
				/* Image descriptors in order used by BCS. */
{
	{	ABL_CMDLINE_INIT ("-expl", "embedded exploder",
				  ARAM_EXPLODER_BUFR, "expl_acp.aout", "1"),
		ABL_RUNTIME_INIT ((ABL_SF_AOUT | ABL_SF_NOT_BCS)), },

	{	ABL_CMDLINE_INIT ("-btwo", "acp boot-2",
				  BCS_IH_BOOT2_FBASE, "boot2.aout", "1"),
		ABL_RUNTIME_INIT ((ABL_SF_AOUT | ABL_SF_RUNFLSH | ABL_SF_FIRSTHD)), },

	{	ABL_CMDLINE_INIT ("-iosb", "acp IOS boot",
				  BCS_IH_IOSBT_FBASE, "alcbt.aout", "1"),
		ABL_RUNTIME_INIT ((ABL_SF_AOUT | ABL_SF_RUNFLSH)), },

	{	ABL_CMDLINE_INIT ("-pmon", "SAR monitor",
				  BCS_IH_TXSAR_RBASE, "pmon.bin", "1"),
		ABL_RUNTIME_INIT (ABL_SF_NONE), },

	{	ABL_CMDLINE_INIT (NULL, "SAR monitor",
				  BCS_IH_RXSAR_RBASE, "", ""),
		ABL_RUNTIME_INIT ((ABL_SF_NONE | ABL_SF_DUPLICAT)), },

	{	ABL_CMDLINE_INIT ("-spud", "SAR diags",
				  BCS_IH_TXSAR_RBASE, "sarpud.bin", "1"),
		ABL_RUNTIME_INIT (ABL_SF_NONE), },

	{	ABL_CMDLINE_INIT (NULL, "SAR diags",
				  BCS_IH_RXSAR_RBASE, "", ""),
		ABL_RUNTIME_INIT ((ABL_SF_NONE | ABL_SF_DUPLICAT)), },

	{	ABL_CMDLINE_INIT ("-tsar", "Tx SAR run-time",
				  BCS_IH_TXSAR_RBASE, "txnopmon.bin", "1"),
		ABL_RUNTIME_INIT (ABL_SF_NONE), },

	{	ABL_CMDLINE_INIT ("-rsar", "Rx SAR run-time",
				  BCS_IH_RXSAR_RBASE, "rxnopmon.bin", "1"),
		ABL_RUNTIME_INIT (ABL_SF_NONE), },

	{	ABL_CMDLINE_INIT ("-kern", "acp IOS kernel",
				  BCS_IH_IOSKR_RBASE, "alcios.aout", "1"),
		ABL_RUNTIME_INIT ((ABL_SF_AOUT | ABL_SF_INCLHDR)), },

	{	ABL_CMDLINE_INIT (NULL, "version info record",
				  BCS_IH_NO_RAM_BASE, "", "1"),
		ABL_RUNTIME_INIT (ABL_SF_VERINFO), },

	{	ABL_CMDLINE_INIT ("-todg", "Tx SAR mfg diag",
				  BCS_IH_TXSAR_RBASE, "txoffdiag.bin", "1"),
		ABL_RUNTIME_INIT (ABL_SF_NONE), },

	{	ABL_CMDLINE_INIT ("-rodg", "Rx SAR mfg diag",
				  BCS_IH_RXSAR_RBASE, "rxoffdiag.bin", "1"),
		ABL_RUNTIME_INIT (ABL_SF_NONE), },
} ;

/*
 *	Table for parsing image keyword (arg) qualifiers.
 */

typedef struct ABL_Quals
{
	char	Qual ;		/* Qualifier letter suffix to cmd arg keyword.*/
	char *	Help ;		/* Description of qualifier for usage help. */
	void	(*pFcn) () ;	/* Routine to handle qualifier. */
} tABL_Quals ;

#define	ABL_QUAL_INIT(q,h,f)	{ q, h, f, }

tABL_Quals	ABL_KeyQual [] =
	{ 	ABL_QUAL_INIT ('a', "image base addr", ABL_Set_ID_BaseAddr),
		ABL_QUAL_INIT ('n', "image file name", ABL_Set_ID_FileName),
		ABL_QUAL_INIT ('v', "image version", ABL_Set_ID_Version), } ;

#define ABL_NBR_KEYQUAL_TBL	(sizeof (ABL_KeyQual) / sizeof (ABL_KeyQual[0]))

/*
 *	This is the global data for this program.
 */

typedef struct ABL_Globals
{
	char *	ProgName ;	/* From command-line. */
	int	argc ;		/* From command-line. */
	char **	argv ;		/* From command-line. */
	char **	envp ;		/* From command-line. */
	char *	CurArgHelp ;	/* For indicating cmd-line arg errors. */
	u_long	TraceLvl ;	/* Output trace level. */
	u_long	FlashBase ;	/* Flash base address of target. */
	u_long	BCS_SvcPt ;	/* BCS service entry point. */
	char *	I_Vers ;	/* Image version.    */
	char *	O_Name ;	/* Output file name. */
	int	O_File ;	/* Output file desc. */
	int	I_File ;	/* Input file desc. */
	u_long	CurFilePt ; 	/* Offset into file. */
	u_long	CurFlashPt ;	/* Offset into flash. */
	u_long	BCS_AreaSz ;	/* Size of BCS area. */
	char *	buffer ;	/* File I/O buffer. */
	ocs_t	ABL_ocs ;	/* OCS header. */
	char	ErrBuf [ABL_LINEBUF_SZ] ;	/* For holding error output. */
} tABL_Globals ;

#define	ABL_TL_NONE	0	/* Values for output trace level. */
#define	ABL_TL_VERBOSE	1	/* Each level is superset of previous level. */
#define	ABL_TL_DUMP	2
#define	ABL_TL_DEBUG	3

#define	ABL_FILLCHAR	0xff
#define	ABL_DFLT_FLBASE	ACP_HW_FLASH_BASE
#define	ABL_DFLT_ONAME	"acpflash.bbi"
#define	ABL_DFLT_IVERS	"1"
#define	ABL_DFLT_BCSSVC	BCS_IH_NO_ACP_ENTRY

tABL_Globals	ABL_Gbl ;	/* Allocate for program global data.  */
				/* Program code must initialize this. */


/*
 *	These are the general command-line args (parse table) for us.
 */

typedef struct ABL_CL_Args
{
	char *	ArgTag ;	/* Command-line arg. */
	char *	ArgHlp ;	/* Help info for usage statement. */
	void	(*pFcn) () ;	/* Handler routine for arg. */
} tABL_CL_Args ;

#define	ABL_ARGTBL_INIT(t,h,f)	{ t, h, f, }

tABL_CL_Args	ABL_CL_ArgTbl [] =
	{
		ABL_ARGTBL_INIT ("-bcss", "set BCS server addr to hex <value>", ABL_SetBCS_SvrPt),
		ABL_ARGTBL_INIT ("-debug", "debug output", ABL_Set_TL_Debug),
		ABL_ARGTBL_INIT ("-dump", "verbose output", ABL_Set_TL_Dump),
		ABL_ARGTBL_INIT ("-flad", "set flash base addr to hex <value>", ABL_SetFlashBase),
		ABL_ARGTBL_INIT ("-help", "help/usage info", ABL_PrtUsage),
		ABL_ARGTBL_INIT ("-loud", "verbose output", ABL_Set_TL_Verb),
		ABL_ARGTBL_INIT ("-out", "set image output file to string <value>", ABL_Set_OFile),
		ABL_ARGTBL_INIT ("-vers", "image version", ABL_Set_ImgVers), };

#define	ABL_NBR_CL_ARGS	(sizeof (ABL_CL_ArgTbl) / sizeof (ABL_CL_ArgTbl [0]))


/*
 *	Combined binary image file format (organization of cbi output file):
 *
 *	+-----+------+----------+-----+
 *	| OCS | EXPL | <images> | BCS |
 *	+-----+------+----------+-----+
 *	^     ^      ^          ^     ^
 *	1     2      3          4     5
 *
 *	OCS	- object control set header	(discarded after download)
 *	EXPL	- embedded exploder		(discarded after download)
 *	BCS	- boot code service headers	(burned to flash)
 *	<images>- various sub-images		(burned to flash)
 *
 *	full cbi checksum			covers	2-5
 *	exact contents of flash				4-5,3-4
 *	flash offset					3-5
 *	file offset					1-5
 *
 *	Some of the sub-images are run-from-flash, so must be positioned
 *	in flash (and the cbi image file) at particular locations.  Where
 *	possible, use a.out headers to construct BCS (per-image) headers.
 *	Some sub-images are flat binary; their BCS headers are initialized
 *	in this file (above).  Gaps must be written to the output file to
 *	reflect flash holes (typically with 0xff values).
 *
 *	Headers and checksums are built as sub-images are copied.  They
 *	(BCS) are written after the images are done.  The OCS header is
 *	computed and written after the BCS headers are done.  This
 *	organization allows the full-file cbi checksum to be built
 *	concurrently with sub-image checksums and copying.  However,
 *	the exploder will re-arrange things for the burn to flash as
 *	follows (this is the organization in flash):
 *
 *	+-----+----------+
 *	| BCS | <images> |
 *	+-----+----------+
 *      ^     ^
 *      a     b
 *
 *	Any gap between the end of BCS and position "b" in flash must be
 *	handled by the exploder.  It will not be represented in the cbi file.
 */


/*
 *	Main entry point.
 */

void main (int argc, char ** argv, char ** envp)
{
	register tABL_Globals   * pG = & ABL_Gbl ;
	register tABL_ImageDesc * pI = & ABL_ImageTbl [0] ;
	register tABL_CL_Args   * pA = & ABL_CL_ArgTbl [0] ;
	register tABL_Quals     * pQ = & ABL_KeyQual [0] ;


		/* Init program global data structure.  Process all */
		/* command-line args.  Give usage statement if user */
		/* is confused or needs help.                       */
	memset ((char *) pG, 0, sizeof (* pG)) ;
	pG->ProgName	= *argv ;
	pG->argc	= --argc ;
	pG->argv	= ++argv ;
	pG->envp	= envp ;
	pG->TraceLvl	= ABL_TL_NONE ;
	pG->FlashBase	= ABL_DFLT_FLBASE ;
	pG->BCS_SvcPt	= ABL_DFLT_BCSSVC ;
	pG->I_Vers	= ABL_DFLT_IVERS ;
	pG->O_Name	= ABL_DFLT_ONAME ;

		/* Determine the size of the area for BCS headers */
		/* and data strings.  Don't forget to pad out for */
		/* long-word alignment if needed (for checksum).  */
		/* Each BCS header has strings associated with it,*/
		/* ensure that those ARE long-aligned.            */
		/* The size calculation MUST match the allocation */
		/* algorithm in ABL_WriteBCS.                     */
	pG->BCS_AreaSz = (BCS_IHL_IMAGE_NAME) + (BCS_IHL_IMAGE_VERSION) +
			 (BCS_IHL_DATE_TIME)  + (BCS_IHL_DATE_TIME) ;

	pG->BCS_AreaSz = ((pG->BCS_AreaSz + 3) & ~ 0x03L) ;

	pG->BCS_AreaSz += (BCS_IHI_END_NBR) * sizeof (u_long) ;

	pG->BCS_AreaSz *= (BCS_IH_TYPE_LAST) ;


		/* Setup OCS header tag string and version.  Note   */
		/* that checksum seed values always start at ZERO.  */
	ABL_SetString (pG->ABL_ocs.tag, ABL_MAGIC, sizeof (pG->ABL_ocs.tag)) ;
	pG->ABL_ocs.Version   = OCS_CUR_VERSION ;
	pG->ABL_ocs.cbi_cksum = ABL_CKSUM_SEED ;
	pG->ABL_ocs.ocs_cksum = ABL_CKSUM_SEED ;

		/* Ensure that the general-purpose I/O buffer is sized    */
		/* with whole long-words (32-bits) to allow for alignment */
		/* during checksums.  Init the file I/O buffer with       */
		/* non-zero char to avoid any run-length encoding by      */
		/* the file system (ie. giving number of zero bytes       */
		/* instead of actually writing them to disk).             */
	pG->buffer = malloc (ABL_BUFSIZ) ;
	if (pG->buffer == NULL)
	{
		ABL_FatalError (pG, "malloc failure", "pG->buffer", errno) ;
	}
	memset (pG->buffer, ABL_FILLCHAR, ABL_BUFSIZ) ;


		/* Process all command-line args.  Give usage statement */
		/* if user is confused or needs help.                   */
	for (; pG->argc > 0 ; -- pG->argc, ++ pG->argv)
	{
		if (ABL_Parse_Args (pG, pA))
			continue ;

		else  if (ABL_Parse_Images (pG, pI, pQ))
			continue ;

			/* Unknown arg keyword, User must be confused. */
			/* Give them a usage statement and quit.       */
		else	ABL_PrtUsage (pG) ;
	}


		/* -------------------------------------------- */
		/* Open and truncate or create the output file. */
		/* -------------------------------------------- */
	pG->O_File = open (pG->O_Name, O_RDWR | O_CREAT | O_TRUNC, 0644) ;
	if (pG->O_File < 0)
	{
		ABL_FatalError (pG, "open failure", pG->O_Name, errno) ;
	}


	/*
	 * Output file is started with the Object Control Set (OCS)
	 * structure header.  Pad this for now, the header will be
	 * filled in after the combined image file is created.
	 * Re-init the full image file checksum seed value, as the
	 * OCS header has its own checksum and the OCS will overwrite
	 * this pad.
	 * Leave the output image length alone, since the OCS header
	 * (or gap) must be counted as part of the full output file.
	 */
	ABL_WriteGap (pG, sizeof (pG->ABL_ocs), FALSE) ;
	pG->ABL_ocs.cbi_cksum = ABL_CKSUM_SEED ;


	/*
	 *	Next in the output image file is the embedded exploder.
	 */
	ABL_LoadExploder (pG, pI) ;


	/*
	 *	Load/copy the various sub-images.  This denotes the part
	 *	of the output file/target download that will be resident
	 *	in flash.  Run-time image data are acquired here.
	 */
	ABL_LoadImages (pG, pI) ;


	/*
	 *	Generate BCS headers to the output image file AFTER
	 *	the sub-images.  The exploder must reposition the BCS
	 *	headers in flash to PRECEDE the sub-images.
	 */
	ABL_WriteBCS (pG, pI) ;


	/*
	 *	Complete (checksum) and write the OCS header to the
	 *	output image file.  This is actually written into the
	 *	OCS-sized pad area written at the start of the output
	 *	file above.
	 */
	ABL_WriteOCS (pG, TRUE) ;


	/*
	 *	All done !  Indicate normal exit status.
	 */
	printf ("\n") ;
	exit (0) ;

}	/* main */


/*
 *	Give user a help statement.
 */

void ABL_PrtUsage (tABL_Globals * pG)
{
	register u_long           count ;
	register tABL_CL_Args   * pA = & ABL_CL_ArgTbl [0] ;
	register tABL_ImageDesc * pI = & ABL_ImageTbl [0] ;
	register tABL_Quals     * pQ = & ABL_KeyQual [0] ;


	printf ("\nUsage: %s [<switches>] [<images>]\n", pG->ProgName) ;

	printf ("\n\t<switches>:  <key> | <key> <value>") ;
	printf ("\n\t<key>:\n") ;
	for (count = 0 ; count < ABL_NBR_CL_ARGS ; ++ count, ++ pA)
	{
		printf ("\t\t%s\t %s\n", pA->ArgTag, pA->ArgHlp) ;
	}

	printf ("\n\t<images>:  <i-type><i-qualifier> <value>") ;
	printf ("\n\t<i-type>:\n") ;
	for (count = 0 ; count < ABL_NBR_IMAGES ; ++ count, ++ pI)
	{
		if ((pI->CL.CmdLineTag != NULL)  &&
		    ! (pI->RT.SpecFlags & (ABL_SF_VERINFO)))
			if (pI->CL.CmdLineTag [0] != '\0')
				printf ("\t\t%s\t %s\n",
					pI->CL.CmdLineTag, pI->CL.CmdHelpStr) ;
	}

	printf ("\n\t<i-qualifier>:\n") ;
	for (count = 0 ; count < ABL_NBR_KEYQUAL_TBL ; ++ count, ++ pQ)
	{
		printf ("\t\t%c\t %s\n", pQ->Qual, pQ->Help) ;
	}

	printf ("\n") ;

	errno = EINVAL ;
	exit (errno) ;

}	/* ABL_PrtUsage */


/*
 *	Set various trace levels.  Always keep the highest level given.
 */

void ABL_Set_TraceLevel (tABL_Globals * pG, u_long ReqLevel)
{
	if (pG->TraceLvl < ReqLevel)
		pG->TraceLvl = ReqLevel ;

}	/* ABL_Set_TraceLevel */

void ABL_Set_TL_Debug (tABL_Globals * pG)
{
	ABL_Set_TraceLevel (pG, ABL_TL_DEBUG) ;

}	/* ABL_Set_TL_Debug */

void ABL_Set_TL_Dump (tABL_Globals * pG)
{
	ABL_Set_TraceLevel (pG, ABL_TL_DUMP) ;

}	/* ABL_Set_TL_Dump */

void ABL_Set_TL_Verb (tABL_Globals * pG)
{
	ABL_Set_TraceLevel (pG, ABL_TL_VERBOSE) ;

}	/* ABL_Set_TL_Verb */


/*
 *	Set target flash base address, ie. override default value.
 */

u_long ABL_GetHexAddr (tABL_Globals * pG, char * ValueStr)
{
	u_long	Result = 0 ;


	if (sscanf (ValueStr, " %lx", & Result) != 1)
		ABL_FatalError (pG, "invalid hex address", ValueStr, EFAULT) ;

	return (Result) ;

}	/* ABL_GetHexAddr */

void ABL_SetBCS_SvrPt (tABL_Globals * pG)
{
	char *	ValueStr ;


	ValueStr = ABL_FindArgVal (pG) ;

	pG->BCS_SvcPt = ABL_GetHexAddr (pG, ValueStr) ;

}	/* ABL_SetBCS_SvrPt */

void ABL_SetFlashBase (tABL_Globals * pG)
{
	char *	ValueStr ;


	ValueStr = ABL_FindArgVal (pG) ;

	pG->FlashBase = ABL_GetHexAddr (pG, ValueStr) ;

}	/* ABL_SetFlashBase */


/*
 *	Set image output file, ie. override default image output file name.
 */

void ABL_Set_OFile (tABL_Globals * pG)
{
	pG->O_Name = ABL_FindArgVal (pG) ;

}	/* ABL_Set_OFile */


/*
 *	Set image version, ie. override default image version.
 */

void ABL_Set_ImgVers (tABL_Globals * pG)
{
	pG->I_Vers = ABL_FindArgVal (pG) ;

}	/* ABL_Set_ImgVers */


/*
 *	Check command-line args for value string once a keyword is matched.
 *	Command-line pointers are updated in any case, as a value is expected.
 */

void ABL_FatalError (tABL_Globals * pG, char * Why, char * What, int NewErr)
{
	if (Why == NULL)
		sprintf (pG->ErrBuf, "\n%s (%s)", pG->ProgName, What) ;
	else
		sprintf (pG->ErrBuf, "\n%s %s (%s)", pG->ProgName, Why, What) ;


	errno = NewErr ;
	perror (pG->ErrBuf) ;

	if (pG->O_File > 0)		/* Not likely to use stdin */
	{
		close  (pG->O_File) ;	/* Purge output file if opened. */
		unlink (pG->O_Name) ;
	}

	exit (errno) ;

}	/* ABL_FatalError */


/*
 *	Check command-line args for value string once a keyword is matched.
 *	Command-line pointers are updated in any case, as a value is expected.
 */

char * ABL_FindArgVal (tABL_Globals * pG)
{
		/* Fatal if no value given with keyword. */
	if (pG->argc <= 1)
		ABL_FatalError (pG, pG->CurArgHelp, * pG->argv, EINVAL) ;


		/* Consume keyword that was matched. */
	-- pG->argc ;
	++ pG->argv ;

		/* Return value string.  Caller consumes this from arglist. */
	return (* pG->argv) ;

}	/* ABL_FindArgVal */


/*
 *	Scan the command-line arg table for a keyword match.  Handle
 *	any match found (set global flags or get output file name),
 *	and return TRUE.  Return FALSE if no match.
 */

u_long ABL_Parse_Args (tABL_Globals * pG, tABL_CL_Args * pA)
{
	u_long	count ;


	for (count = 0 ; count < ABL_NBR_CL_ARGS ; ++ count, ++ pA)
	{
		if (! strncasecmp (* pG->argv, pA->ArgTag, strlen (pA->ArgTag)))
		{
			pG->CurArgHelp = pA->ArgHlp ;
			(* pA->pFcn) (pG) ;
			return (TRUE) ;
		}
	}

	return (FALSE) ;

}	/* ABL_Parse_Args */


/*
 *	Scan the command-line arg table for a keyword match.  Handle
 *	any match found (look for keyword qualifier), and return TRUE.
 *	Return FALSE if no match.
 */

u_long ABL_Parse_Images (tABL_Globals * pG, tABL_ImageDesc * pI,
							tABL_Quals * pQ)
{
	u_long	count ;
	u_long	qual ;
	long	taglen ;


	for (count = 0 ; count < ABL_NBR_IMAGES ; ++ count, ++ pI)
 	{
			/* Duplicate entries have NULL tag strings. */
		if ((pI->CL.CmdLineTag != NULL)  &&
		    ! (pI->RT.SpecFlags & (ABL_SF_VERINFO)))
			taglen = strlen (pI->CL.CmdLineTag) ;
		else
			taglen = 0 ;


		if ((taglen > 0) &&
		    (! strncasecmp (* pG->argv, pI->CL.CmdLineTag, taglen)))
		{
			pG->CurArgHelp = pI->CL.CmdHelpStr ;
			qual = (u_long) ((* pG->argv) [taglen]) ;
			ABL_ImageQuals (pG, qual, pI, pQ);
			return (TRUE) ;
		}
	}

	return (FALSE) ;

}	/* ABL_Parse_Images */


/*
 *	Scan the keyword qualifier table for a match.  Handle any match
 *	found (look for value string), and give usage statement if User
 *	is confused.
 */

void ABL_ImageQuals (tABL_Globals *pG, u_long Qualifier, tABL_ImageDesc *pI,
							tABL_Quals * pQ)
{
	char *	ValueStr ;
	u_long	count ;


	for (count = 0 ; count < ABL_NBR_KEYQUAL_TBL ; ++ count, ++ pQ)
 	{
		if (Qualifier == (u_long) (pQ->Qual))
		{
			pG->CurArgHelp = pQ->Help ;
			ValueStr = ABL_FindArgVal (pG) ;
			(* pQ->pFcn) (pG, pI, ValueStr) ;
			return ;
		}
	}

		/* User is confused, qualifier is illegal.  Give usage. */
	ABL_PrtUsage (pG) ;

}	/* ABL_ImageQuals */


/*
 *	Set various image descriptor fields.  Always null-out string
 *	buffers before re-use.
 */

void ABL_SetString (char * Dest, char * Value, long MaxLen)
{
		/* Ensure that the copied string will have a null byte. */
	memset (Dest, 0, MaxLen) ;
	strncpy (Dest, Value, MaxLen - 1) ;

}	/* ABL_SetString */

void ABL_Set_ID_BaseAddr (tABL_Globals *pG, tABL_ImageDesc *pI, char * ValueStr)
{
	pI->CL.ImageBase = ABL_GetHexAddr (pG, ValueStr) ;

}	/* ABL_Set_ID_BaseAddr */

void ABL_Set_ID_FileName (tABL_Globals *pG, tABL_ImageDesc *pI, char * ValueStr)
{
	ABL_SetString (pI->CL.F_Name, ValueStr, sizeof (pI->CL.F_Name)) ;

}	/* ABL_Set_ID_FileName */

void ABL_Set_ID_Version (tABL_Globals *pG, tABL_ImageDesc *pI, char * ValueStr)
{
	ABL_SetString (pI->CL.F_Vers, ValueStr, sizeof (pI->CL.F_Vers)) ;

}	/* ABL_Set_ID_Version */


/*
 *	Various input and seek routines.  We basically do the low-level
 *	operation and then check for error/failure return codes.
 */

void ABL_ReadInpF (tABL_Globals * pG, int RdReq, char * F_Name)
{
	if (read (pG->I_File, pG->buffer, RdReq) != RdReq)
		ABL_FatalError (pG, "image read error", F_Name, errno) ;

}	/* ABL_ReadInpF */


void ABL_SeekFile (tABL_Globals * pG, int F_Desc, off_t Where, char * F_Name)
{
	if (lseek (F_Desc, Where, SEEK_SET) < 0)
		ABL_FatalError (pG, "seek error", F_Name, errno) ;

}	/* ABL_SeekFile */


/*
 *	Various output routines.  The same low-level file output routine
 *	is used for all, but various counters are incremented for each.
 */

void ABL_WriteOutF (tABL_Globals * pG, char * Buf, int WrLen)
{
	if (WrLen > 0)
		if (write (pG->O_File, Buf, WrLen) != WrLen)
			ABL_FatalError (pG, "write failure", pG->O_Name, errno);

}	/* ABL_WriteOutF */

void ABL_Write_CBI (tABL_Globals * pG, char * Buf, int WrLen, u_long fFlash)
{
	ABL_WriteOutF (pG, Buf, WrLen) ;
	pG->CurFilePt += WrLen ;

	if (fFlash)
		pG->CurFlashPt += WrLen ;

}	/* ABL_Write_CBI */

void ABL_WriteGap (tABL_Globals * pG, int GapLen, u_long fFlash)
{
	memset (pG->buffer, ABL_FILLCHAR, ABL_BUFSIZ) ;

		/* Ensure that file gap is long-word aligned. */
	GapLen = ((GapLen + 3) & ~ 0x03L) ;

	while (GapLen > ABL_BUFSIZ)
	{
		pG->ABL_ocs.cbi_cksum = checksum ((u_long *)(pG->buffer),
						  ((ABL_BUFSIZ) >> 2),
						  pG->ABL_ocs.cbi_cksum) ;
		ABL_Write_CBI (pG, pG->buffer, ABL_BUFSIZ, fFlash) ;
		GapLen -= ABL_BUFSIZ ;
	}

	if (GapLen > 0)
	{
		pG->ABL_ocs.cbi_cksum = checksum ((u_long *)(pG->buffer),
						  (GapLen >> 2),
						  pG->ABL_ocs.cbi_cksum) ;
		ABL_Write_CBI (pG, pG->buffer, GapLen, fFlash) ;
	}

}	/* ABL_WriteGap */


/*
 *	Expand a file name using the environment variable $HOME.
 *	Use the pre-existing buffer "pG->buffer" for scratch.
 *	(Assume ABL_NAME_SIZE < ABL_BUFSIZ.)
 */

void ABL_ExpandPath (tABL_Globals * pG, tABL_ImageDesc * pI)
{
	char *	Home = NULL ;


		/* Cannot expand ~<username> right now. */
		/* ~/<filename> is just fine though.    */
	Home = getenv ("HOME") ;
	if ((Home == NULL) || (pI->CL.F_Name [1] != '/'))
	{
		printf ("%s: $HOME not located, cannot expand `%s'\n",
			pG->ProgName, pI->CL.F_Name) ;
		exit (-1) ;
	}

		/* Assume that full path name will fit in ABL_BUFSIZ bytes. */
		/* Don't forget to step past the original '~' character.    */
	sprintf (pG->buffer, "%s%s", Home, & pI->CL.F_Name [1]) ;

	if (strlen (pG->buffer) >= sizeof (pI->CL.F_Name))
	{
		errno = ENAMETOOLONG ;
		perror (pG->buffer) ;
		exit (errno) ;
	}
	else	strcpy (pI->CL.F_Name, pG->buffer) ;

}	/* ABL_ExpandPath */


/*
 *	Load and process a sub-image.  If it is in a.out format, record
 *	some data in the BCS header, or else just use canned data.  Pad
 *	the back end of the image to be long-word aligned, so that checksums
 *	can be done on whole long-words.  Gaps may be emitted in the output
 *	file if necessary, to ensure that the sub-image is positioned
 *	correctly in the output file (and thus in flash memory).
 */

void ABL_LoadFile (tABL_Globals * pG, tABL_ImageDesc * pI, u_long fFlash)
{
	struct stat	F_Info ;
	char *		BaseName ;
	u_long		SzCur  ;
	u_long		SzLeft ;
	u_long		SzPad  ;


		/* Was image previously written, or is this desc a dup ? */
	if (pI->RT.SpecFlags & (ABL_SF_DONE | ABL_SF_DUPLICAT | ABL_SF_VERINFO))
	{
		pI->RT.SpecFlags |= (ABL_SF_DONE) ;
		return ;
	}

	/*
	 *	Expand tilda (~) into environment variable $HOME.
	 */
	if (pI->CL.F_Name [0] == '~')
		ABL_ExpandPath (pG, pI) ;

	/*
	 *	Locate the base filename without the path.
	 */
	BaseName = strrchr (pI->CL.F_Name, '/') ;
	if (BaseName == NULL)
		BaseName = pI->CL.F_Name ;
	else
		++ BaseName ;		/* Step past '/'. */


	if (pG->TraceLvl >= ABL_TL_VERBOSE)
		printf ("%s\t%s", pI->CL.CmdLineTag, BaseName) ;


	/*
	 *	Open the input image file and get some info (size) about it.
	 */
	pG->I_File = open (pI->CL.F_Name, O_RDONLY) ;
	if (pG->I_File < 0)
		ABL_FatalError (pG, NULL, BaseName, errno) ;

	if (fstat (pG->I_File, & F_Info) < 0)
		ABL_FatalError (pG, NULL, BaseName, errno) ;

	pI->RT.FileSize = F_Info.st_size ;
	pI->RT.CheckSum = ABL_CKSUM_SEED ;

		/* Produce a Unix-format date/time/timezone string. */
	memset (pI->RT.F_Date, 0, sizeof (pI->RT.F_Date)) ;
	if (! strftime (pI->RT.F_Date, sizeof (pI->RT.F_Date),
			"%a %b %d %X %Z %Y", localtime (& F_Info.st_mtime)))
		ABL_FatalError (pG, "date expansion", pI->RT.F_Date, ENOMEM) ;

		/* Just format zeros for installation date/time string. */
	memset (pI->RT.I_Date, 0, sizeof (pI->RT.I_Date)) ;


	/*
	 *	Extract any a.out header and process it into current
	 *	image descriptor run-time data.
	 */
	if (pI->RT.SpecFlags & (ABL_SF_AOUT))
		pI->RT.FileSize -= ABL_ReadFileHdr (pG, pI, BaseName) ;


	/*
	 *	Pad out to a given offset/address if this sub-image must be
	 *	run-from-flash.  Otherwise just note where we are adding it
	 *	in.  Note an error if the desired flash location is already
	 *	in use.
	 */
	if (pI->RT.SpecFlags & (ABL_SF_RUNFLSH))
	{
		SzCur = (pG->FlashBase + pG->BCS_AreaSz + pG->CurFlashPt) ;

		if (pI->CL.ImageBase > SzCur)
			ABL_WriteGap (pG, (pI->CL.ImageBase - SzCur), fFlash) ;

		else  if (pI->CL.ImageBase < SzCur)
			ABL_FatalError (pG, "images overlap", BaseName, EFAULT);
	}
	pI->RT.FlashOfst = pG->CurFlashPt ;	/* Offset to image in flash. */


	/*
	 *	Read, checksum, and write the image file.  Note that
	 *	all checksums start with initial seed value of ZERO.
	 *	Also, pad output image with ZERO bytes to ensure long-word
	 *	(32-bit) alignment in output file and for checksum calcs.
	 */
	SzCur = ABL_BUFSIZ ;
	SzPad = 0 ;
	for (SzLeft = pI->RT.FileSize ; SzLeft > 0 ; SzLeft -= SzCur)
	{
		SzCur = ((SzLeft >= ABL_BUFSIZ) ? ABL_BUFSIZ : SzLeft) ;

		ABL_ReadInpF (pG, SzCur, BaseName) ;

			/* Is a pad to longword needed ?  Not so for */
			/* exact multiples of ABL_BUFSIZ.            */
		SzPad = (SzCur & 0x03L) ;
		if (SzPad != 0)
		{
			SzPad = 4 - SzPad ;		/* Nbr pad bytes. */
			memset (& pG->buffer [SzCur], 0, SzPad) ;
			SzCur  += SzPad ;
			SzLeft += SzPad ;
		}

		pI->RT.CheckSum = checksum ((u_long *)(pG->buffer),
					    (SzCur >> 2), pI->RT.CheckSum) ;

		pG->ABL_ocs.cbi_cksum = checksum ((u_long *)(pG->buffer),
						  (SzCur >> 2),
						  pG->ABL_ocs.cbi_cksum) ;

		ABL_Write_CBI (pG, pG->buffer, SzCur, fFlash) ;
	}

	/*
	 *	Close the input image file and mark the input file descriptor.
	 */
	if (close (pG->I_File) < 0)
		ABL_FatalError (pG, "image close error", BaseName, errno) ;

	pG->I_File = 0 ;


	/*
	 *	Bump OCS module count and mark this image descriptor as done.
	 *	OCS full-file and sub-image checksums were done above.
	 */
	++ pG->ABL_ocs.cbi_mods ;		/* Incr module count. */
	pI->RT.SpecFlags |= (ABL_SF_DONE) ;	/* Mark image as written. */


	if (pG->TraceLvl >= ABL_TL_DUMP)
	{
		printf ("\ttb-%8x sz-%8x ep-%8x cs-%8x\n",
			pI->CL.ImageBase, pI->RT.FileSize, pI->RT.EntryPt,
			pI->RT.CheckSum) ;

		printf ("\tfp-%8x\t%s\tvr-%s\n",
			pG->FlashBase + pG->BCS_AreaSz + pI->RT.FlashOfst,
			pI->RT.F_Date, pI->CL.F_Vers) ;
	}

	if (pG->TraceLvl >= ABL_TL_VERBOSE)
		printf ("\n") ;

}	/* ABL_LoadFile */


/*
 *	Create raw data for a BCS version info record.
 */

void ABL_LoadVers (tABL_Globals * pG, tABL_ImageDesc * pI, u_long fFlash)
{
	char *	BaseName ;
	time_t	MyTime ;


		/* Was image previously written, or is this desc real ? */
	if ((pI->RT.SpecFlags & (ABL_SF_DONE))  ||
	    ! (pI->RT.SpecFlags & (ABL_SF_VERINFO)))
	{
		pI->RT.SpecFlags |= (ABL_SF_DONE) ;
		return ;
	}

	/*
	 *	Copy output filename as our filename.
	 */
	memset (pI->CL.F_Name, 0, sizeof (pI->CL.F_Name)) ;

	BaseName = strrchr (pG->O_Name, '/') ;
	if (BaseName == NULL)
		BaseName = pG->O_Name ;
	else
		++ BaseName ;		/* Step past '/'. */

	strncpy (pI->CL.F_Name, BaseName, sizeof (pI->CL.F_Name) - 1) ;


	/*
	 *	Copy full image version info.
	 */
	memset (pI->CL.F_Vers, 0, sizeof (pI->CL.F_Vers)) ;
	strncpy (pI->CL.F_Vers, pG->I_Vers, sizeof (pI->CL.F_Vers) - 1) ;


	/*
	 *	Open the input image file and get some info (size) about it.
	 */

	pI->RT.FileSize  = pG->CurFlashPt + pG->BCS_AreaSz ;
	pI->RT.EntryPt   = BCS_IH_NO_ACP_ENTRY ;
	pI->RT.CheckSum  = ABL_CKSUM_SEED ;
	pI->RT.FlashOfst = pG->BCS_AreaSz ;

		/* Produce a Unix-format date/time/timezone string. */
	memset (pI->RT.F_Date, 0, sizeof (pI->RT.F_Date)) ;

	MyTime = time (NULL) ;
	if (! strftime (pI->RT.F_Date, sizeof (pI->RT.F_Date),
			"%a %b %d %X %Z %Y", localtime (& MyTime)))
		ABL_FatalError (pG, "date expansion", pI->RT.F_Date, ENOMEM) ;

		/* Just format zeros for installation date/time string. */
	memset (pI->RT.I_Date, 0, sizeof (pI->RT.I_Date)) ;


	/*
	 *	Mark this image descriptor as done.
	 */
	pI->RT.SpecFlags |= (ABL_SF_DONE) ;	/* Mark image as written. */


	if (pG->TraceLvl >= ABL_TL_DUMP)
	{
		printf ("\ttb-%8x sz-%8x ep-%8x cs-%8x\n",
			pI->CL.ImageBase, pI->RT.FileSize, pI->RT.EntryPt,
			pI->RT.CheckSum) ;

		printf ("\tfp-%8x\t%s\tvr-%s\n",
			pG->FlashBase + pG->BCS_AreaSz + pI->RT.FlashOfst,
			pI->RT.F_Date, pI->CL.F_Vers) ;
	}

	if (pG->TraceLvl >= ABL_TL_VERBOSE)
		printf ("\n") ;

}	/* ABL_LoadVers */


/*
 *	Load and process the embedded exploder sub-image.  Copy some
 *	image-specific (a.out header) data to the OCS header.  (Done
 *	using a fake BCS header that is NOT written to the output file.)
 */

void ABL_LoadExploder (tABL_Globals * pG, tABL_ImageDesc * pI)
{
		/* Note file offset to the exploder. */
	pG->ABL_ocs.ExplOfst = pG->CurFilePt ;

	pI = & pI [ABL_IDIT_EXPLODER] ;
	ABL_LoadFile (pG, pI, FALSE) ;

	pG->ABL_ocs.ExplBase	= pI->CL.ImageBase ;
	pG->ABL_ocs.ExplSize	= pI->RT.FileSize  ;
	pG->ABL_ocs.ExplEnAddr	= pI->RT.EntryPt   ;

}	/* ABL_LoadExploder */


/*
 *	Walk the image descriptor table and load/copy each of the given
 *	sub-images.  These sub-images will all be flash-resident.  Where
 *	possible use existing (a.out) file headers to extract/build BCS
 *	headers (else just use canned BCS header data).
 *
 *	The back end of each sub-image is padded to be long-word aligned,
 *	so that copying operations are more efficient and checksums can be
 *	done on whole long-words.
 *
 *	Gaps may be emitted in the output file to ensure that each sub-image
 *	is positioned correctly.  This part of the output image file is
 *	written to flash exactly as given, so run-from-flash sub-images
 *	must be positioned exactly at their link addresses.
 */

void ABL_LoadImages (tABL_Globals * pG, tABL_ImageDesc * pT)
{
	tABL_ImageDesc *	pI ;			/* Current image desc.*/
	long			Count ;			/* Loop/type index. */


		/* Note file offset to the start of the sub-images. */
	pG->ABL_ocs.ImageOfst = pG->CurFilePt ;


	for (pI = pT, Count = 0 ; Count < ABL_NBR_IMAGES ; ++ pI, ++ Count)
	{
		if (pI->RT.SpecFlags & (ABL_SF_NOT_BCS))
			continue ;

		else  if (pI->RT.SpecFlags & (ABL_SF_DONE))
			ABL_FatalError (pG, "already opened",
					pI->CL.F_Name, EIO) ;

		else  if (pI->RT.SpecFlags & (ABL_SF_VERINFO))
			ABL_LoadVers (pG, pI, TRUE) ;

		else	ABL_LoadFile (pG, pI, TRUE) ;
	}

}	/* ABL_LoadImages */


/*
 *	Complete (checksum) and write the individual BCS headers to the
 *	output image file.  While they FOLLOW the sub-images in the file,
 *	the exploder must reposition the BCS headers in flash to PRECEDE
 *	the sub-images.  Note that the BCS header checksum also covers
 *	those fields referred to in the string table that are actually
 *	part of that BCS header.
 */

void ABL_WriteBCS (tABL_Globals * pG, tABL_ImageDesc * pT)
{
	tABL_ImageDesc *	pI ;			/* Current image desc.*/
	tABL_ImageDesc *	pL ;			/* Last image desc. */
	volatile u_long *	pH ;			/* Current BCS hdr.  */
	volatile u_long *	pP ;			/* Previous BCS hdr. */
	char *			BCS_buf ;		/* BCS hdr work buf. */
	char *			BCS_StrPtr ;		/* String tbl of BCS. */
	u_long			BCS_StrAddr ;		/* Addr of BCS strs. */
	long			Count ;			/* Loop/type index. */
	u_long			PadSz ;			/* Pad/Size value. */
	char *			BCS_StrPtr0 ;		/* Previous str ptr. */


		/* Note file offset to the start of the BCS headers. */
	pG->ABL_ocs.BCS_Ofst = pG->CurFilePt ;

		/* Acquire a work buffer for all BCS header and string data. */
		/* Ensure it is init with zero, so any padding will yield    */
		/* zero bytes.                                               */
	BCS_buf = malloc (pG->BCS_AreaSz) ;
	if (BCS_buf == NULL)
	{
		ABL_FatalError (pG, "malloc failure", "BCS_buf", errno) ;
	}
	memset (BCS_buf, 0, pG->BCS_AreaSz) ;

		/* Current BCS header is always generated from previous one. */
		/* Initialize some BCS header fields that don't change much. */
	pH = (volatile u_long *)(BCS_buf) ;
	pP = pH ;				/* Initial case. */
	pL = NULL ;

	pP [BCS_IHI_IMG_PRESENT_ ] = BCS_IH_PRESENT_ ;
	pP [BCS_IHI_IMG_INVALID_ ] = BCS_IH_VALID_IMAGE ;
	pP [BCS_IHI_HDR_VERSION  ] = BCS_IH_BASE_VERSION ;
	pP [BCS_IHI_IMAGE_FLAGS  ] = BCS_IH_FG_NONE ;
	pP [BCS_IHI_IMAGE_PROP   ] = BCS_NULL_PROPERTY ;
	pP [BCS_IHI_SPARE_1      ] = BCS_NULL_PAD ;


		/* The BCS headers occur at the start of target flash area.  */
		/* They are fully contiguous and long-word aligned.  The BCS */
		/* string data area (string table) follows immediately, and  */
		/* each set of string data (per BCS header) is long aligned. */
	BCS_StrAddr  = (BCS_IH_TYPE_LAST) * (BCS_IHI_END_NBR) * sizeof (u_long);
	BCS_StrPtr   = & BCS_buf [BCS_StrAddr] ;
	BCS_StrAddr += pG->FlashBase ;


		/* Walk and process the image descriptor table.  */
		/* Assume that duplicate image descriptors can   */
		/* be loaded from the immediately preceding one. */
	PadSz = 0 ;
	BCS_StrPtr0 = NULL ;
	for (pI = pT, Count = 0 ; Count < ABL_NBR_IMAGES ; ++ pI, ++ Count)
	{
		if (pI->RT.SpecFlags & (ABL_SF_NOT_BCS))
			continue ;

		else  if (! (pI->RT.SpecFlags & (ABL_SF_DONE)))
			ABL_FatalError(pG, "not processed", pI->CL.F_Name, EIO);


			/* Always init current BCS header from previous one. */
		if (pH != pP)
			memcpy ((char *) pH, (char *) pP,
				(BCS_IHI_END_NBR) * sizeof (u_long)) ;

			/* First BCS header has a different flag than others. */
			/* It also has the BCS server and its entry point.    */
		if (pI->RT.SpecFlags & (ABL_SF_FIRSTHD))
		{
			pH [BCS_IHI_FLAG_PATTERN ] = BCS_IH_BOOT_FPAT ;
			pH [BCS_IHI_IMAGE_SVC    ] = pG->BCS_SvcPt ;
		}
		else
		{
			pH [BCS_IHI_FLAG_PATTERN ] = BCS_IH_IMAGE_FPAT ;
			pH [BCS_IHI_IMAGE_SVC    ] = BCS_IH_NO_ACP_ENTRY ;
		}

			/* Assume that image descriptors are arranged */
			/* in ascending BCS order and that the loop   */
			/* count can be used for image type index.    */
			/* BCS image type index starts at ONE.        */
		pH [BCS_IHI_IMAGE_TYPE   ] = Count ;
		pH [BCS_IHI_RAM_BASE     ] = pI->CL.ImageBase ;


			/* Assume that duplicate images apply to images */
			/* that are common to both TxSAR and RxSAR.     */
		if (pI->RT.SpecFlags & (ABL_SF_DUPLICAT))
		{
			if (pL == NULL)
				ABL_FatalError (pG, "1st BCS hdr is duplicate",
						pI->CL.F_Name, EBADF) ;

			memcpy (pI->CL.F_Name, pL->CL.F_Name,
						sizeof (pI->CL.F_Name)) ;
			memcpy (pI->CL.F_Vers, pL->CL.F_Vers,
						sizeof (pI->CL.F_Vers)) ;
			memcpy (pI->RT.F_Date, pL->RT.F_Date,
						sizeof (pI->RT.F_Date)) ;
			memcpy (pI->RT.I_Date, pL->RT.I_Date,
						sizeof (pI->RT.I_Date)) ;
		}
			/* Special handling for version info BCS record. */
		else  if (pI->RT.SpecFlags & (ABL_SF_VERINFO))
		{
			pH [BCS_IHI_FLASH_BASE   ] = pG->FlashBase ;
			pH [BCS_IHI_FSH_IMAGE_LEN] = pI->RT.FileSize ;
			pH [BCS_IHI_RAM_BASE     ] = pI->CL.ImageBase ;
			pH [BCS_IHI_RAM_IMAGE_LEN] = pI->RT.FileSize ;
			pH [BCS_IHI_IMAGE_ENTRY  ] = pI->RT.EntryPt ;
			pH [BCS_IHI_IMAGE_CHK    ] = pI->RT.CheckSum ;
		}
		else	/* Non-duplicate image descriptor. */
		{
			/*
			 *	If running from flash, ensure that computed
			 *	and requested flash base addresses match !
			 */
			pH [BCS_IHI_FLASH_BASE   ] = pG->FlashBase
						     + pG->BCS_AreaSz
						     + pI->RT.FlashOfst ;

			if (pI->RT.SpecFlags & (ABL_SF_RUNFLSH))
			{
				if (pH [BCS_IHI_FLASH_BASE] != pI->CL.ImageBase)
					ABL_FatalError (pG, "image location",
							pI->CL.F_Name, EFAULT) ;

				pH [BCS_IHI_RAM_BASE] = BCS_IH_NO_RAM_BASE ;
			}

			pH [BCS_IHI_FSH_IMAGE_LEN] = pI->RT.FileSize ;
			pH [BCS_IHI_RAM_IMAGE_LEN] = pI->RT.FileSize ;
			pH [BCS_IHI_IMAGE_ENTRY  ] = pI->RT.EntryPt  ;
			pH [BCS_IHI_IMAGE_CHK    ] = pI->RT.CheckSum ;
		}

		BCS_StrPtr0 = BCS_StrPtr ;

		pH [BCS_IHI_IMAGE_NAME   ] = BCS_StrAddr ;
		memcpy (BCS_StrPtr, pI->CL.F_Name, sizeof (pI->CL.F_Name)) ;
		BCS_StrAddr += (BCS_IHL_IMAGE_NAME) ;
		BCS_StrPtr  += (BCS_IHL_IMAGE_NAME) ;

		pH [BCS_IHI_IMAGE_VERSION] = BCS_StrAddr ;
		memcpy (BCS_StrPtr, pI->CL.F_Vers, sizeof (pI->CL.F_Vers)) ;
		BCS_StrAddr += (BCS_IHL_IMAGE_VERSION) ;
		BCS_StrPtr  += (BCS_IHL_IMAGE_VERSION) ;

		pH [BCS_IHI_IMAGE_DATE   ] = BCS_StrAddr ;
		memcpy (BCS_StrPtr, pI->RT.F_Date, sizeof (pI->RT.F_Date)) ;
		BCS_StrAddr += (BCS_IHL_DATE_TIME) ;
		BCS_StrPtr  += (BCS_IHL_DATE_TIME) ;

		pH [BCS_IHI_INSTALL_DATE ] = BCS_StrAddr ;
		memcpy (BCS_StrPtr, pI->RT.I_Date, sizeof (pI->RT.I_Date)) ;
		BCS_StrAddr += (BCS_IHL_DATE_TIME) ;
		BCS_StrPtr  += (BCS_IHL_DATE_TIME) ;

		PadSz = (BCS_StrAddr & 0x03L) ;
		if (PadSz)
			PadSz = 4 - PadSz ;
		BCS_StrAddr += PadSz ;
		BCS_StrPtr  += PadSz ;

			/* Determine the BCS header image property value. */
		pH [BCS_IHI_IMAGE_PROP   ] = BCS_ImageProperty ((u_int32 *) pH);

			/* BCS header checksum only covers BCS data. */
			/* Don't forget each of the related BCS data strings. */
			/* Assume that all related strings are contiguous. */
		pH [BCS_IHI_HDR_CHK      ] = ABL_CKSUM_SEED ;
		pH [BCS_IHI_HDR_CHK      ] = 
			checksum ((u_long *) pH, BCS_IHI_HDR_CHK,
				  pH [BCS_IHI_HDR_CHK]) ;
		pH [BCS_IHI_HDR_CHK      ] = 
			checksum ((u_long *) BCS_StrPtr0,
				((BCS_StrAddr - pH [BCS_IHI_IMAGE_NAME]) >> 2),
				pH [BCS_IHI_HDR_CHK]) ;

			/* Remember current structures as old/previous ones. */
		pP  = pH ;
		pH += (BCS_IHI_END_NBR) ;
		pL  = pI ;
	}


		/* Full-file checksum covers entire BCS area, */
		/* including BCS headers and data strings.    */
	pG->ABL_ocs.cbi_cksum = checksum ((u_long *) BCS_buf,
					  (pG->BCS_AreaSz >> 2),
					  pG->ABL_ocs.cbi_cksum) ;

	ABL_Write_CBI (pG, BCS_buf, pG->BCS_AreaSz, TRUE) ;

	free (BCS_buf) ;
	BCS_buf = NULL ;

}	/* ABL_WriteBCS */


/*
 *	Complete (checksum) and write the OCS header to the
 *	output image file.  This is actually written into the
 *	OCS-sized pad area written at the start of the output
 *	file when it was opened.  Note that the full-file checksum
 *	was updated as each sub-image, sub-header or gap was written out.
 */

void ABL_WriteOCS (tABL_Globals * pG, u_long fClose)
{
	u_long	OCS_hdr_size  = 0 ;


		/* Note the final output image file size. */
	pG->ABL_ocs.cbi_size = pG->CurFilePt ;

		/* Copy the OCS header into the work buffer and ensure  */
		/* that it is long-word aligned.  Pad the back end with */
		/* zero bytes if needed.  The OCS pad area at the start */
		/* of the output image file was also padded out to get  */
		/* long-alignment.                                      */
	OCS_hdr_size = ((sizeof (pG->ABL_ocs) + 3) & ~ 0x03L) ;

	memset (pG->buffer, 0, ABL_BUFSIZ) ;
	memcpy (pG->buffer, & pG->ABL_ocs, sizeof (pG->ABL_ocs)) ;

		/* Get the OCS header checksum.  The OCS image (file)  */
		/* checksum was determined as each element was written */
		/* to the output image file.                           */
	pG->ABL_ocs.ocs_cksum = checksum ((u_long *)(pG->buffer),
					  (OCS_hdr_size >> 2) - 1,
				 	  ABL_CKSUM_SEED) ;

	((ocs_t *)(pG->buffer))->ocs_cksum = pG->ABL_ocs.ocs_cksum ;

	/*
	 *	Write the OCS header at the beginning of the output image file.
	 */
	ABL_SeekFile (pG, pG->O_File, 0L, pG->O_Name) ;

	ABL_WriteOutF (pG, pG->buffer, OCS_hdr_size) ;


	/*
	 *	Close the output image file if desired.  Mark the output file
	 *	descriptor as a defensive measure.
	 */

	if (fClose)
	{
		if (close (pG->O_File) < 0)
			ABL_FatalError (pG, "output close error",
					pG->O_Name, errno) ;

		pG->O_File = 0 ;
	}

}	/* ABL_WriteOCS */


/*
 *	Read image file a.out header and incorporate into current BCS header.
 */

u_long ABL_ReadFileHdr (tABL_Globals * pG, tABL_ImageDesc * pI, char * F_Name)
{
	struct exec *	pH ;


	pH = (struct exec *)(pG->buffer) ;

	ABL_ReadInpF (pG, sizeof (*pH), F_Name) ;

	pI->RT.EntryPt = pH->a_entry ;


		/* Rewind the input file to the beginning if the a.out */
		/* header is to be copied, or else decrease the copy   */
		/* size for the a.out header.                          */
	if (pI->RT.SpecFlags & (ABL_SF_INCLHDR))
	{
		ABL_SeekFile (pG, pG->I_File, 0L, F_Name) ;
		return (0) ;
	}
	else	return (sizeof (*pH)) ;

}	/* ABL_ReadFileHdr */

/* 
 * Wrapper function for the IOS function buginf - just for the tools
 */
int 
buginf (const char *cp, va_list ap)
{
	int count;
	count = printf(cp, ap);
	return (count);
}
