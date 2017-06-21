# $Id: makefile,v 1.1.4.7 1996/06/11 18:14:40 hampton Exp $
# $Source: /release/112/cvs/Xwbu/cat5k/obj-68-alc/makefile,v $
#------------------------------------------------------------------
# makefile -- for Synergy ATM line card platform code.
#
# March, 1996, Rick Lowe
#
# Copyright (c) 1996 by cisco Systems, Inc.
# All rights reserved.
#
# Assumes GNU make and GNU cc
#------------------------------------------------------------------
# $Log: makefile,v $
# Revision 1.1.4.7  1996/06/11  18:14:40  hampton
# Quiet the compilation output when the environment variable QUIET is
# set.  [CSCdi60124]
# Branch: California_branch
#
# Revision 1.1.4.6  1996/06/11  02:36:57  venkat
# CSCdi60077:  Build tools under wbu independent of the Host OS
# Branch: California_branch
# Built a wrapper function for buginf for the tools abl.c
# csumaout.c. Makefile changes to accomodate the whole tuple
# of uname(1) to figure out the host OS.
#
# Revision 1.1.4.5  1996/06/06  22:53:28  venkat
# CSCdi59790:  PATH to GNU toolset hardcoded in WBU makefiles
# Branch: California_branch
# Defined three sets of (CC, LD, AR etc) defines for host
# (sunos/solaris), cross-compiler for 68k, cross-compiler for mips.
# All these defines are moved to makefile.c5k.alc and the other
# makefiles (cat5k/obj-3k-sar/makefile cat5k/obj-68-alc/makefile
# cat5k/alc_sartools/Makefile) include this file.
#
# Revision 1.1.4.4  1996/06/05  02:05:57  venkat
# CSCdi59532:  Objects under wbu/cat5k built using 930630 tool chain
# linker
# Branch: California_branch
# Fixed target $(PROM_STEM).srec to use 96q1 toolchain linker
#
# Revision 1.1.4.3  1996/06/04  05:20:33  rlowe
# Fixed wbu/cat5k/obj-68-alc and boot/obj-68-alc makefiles.
# CSCdi59410:  Cant drive C5K ATM card ios boot build without preexisting
# WBU libs
# Branch: California_branch
#
# Revision 1.1.4.2  1996/05/31  01:03:27  venkat
# CSCdi59149:  Tactical fix of ATM image breakages from 96q1 tools.
# Branch: California_branch
# Assembly file and pmcc (cygnus.c) fixes to get builds to work with 96q1
# tool
# chain
#
# Revision 1.1.4.1  1996/05/09  15:01:29  rbadri
# Branch: California_branch
# Synergy features to the mainline
#
# Revision 1.1.40.5  1996/05/04  01:29:11  rlowe
# PROM boot image checksum is based on stripped binary image, not
# an image containing a symbol table.
# Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
#
# Revision 1.1.40.4  1996/05/02  01:45:23  rlowe
# When dispatching kernel submake from wbu-side, suppress submakes
# dispatched from sys-side.
# Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
#
# Revision 1.1.40.3  1996/05/01  05:59:41  rlowe
# Change names of targets of sub-makes to conform with appropriate
# kernel and boot sub-makefiles.
# Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
#
# Revision 1.1.40.2  1996/05/01  05:43:27  rlowe
# Add TAGS and ID generation to depend(ancies) rule.
# Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
#
# Revision 1.1.40.1  1996/04/27  06:17:25  cakyol
# non sync sync from V112_0_2 to ....
# Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
#
# Revision 1.1.28.4  1996/04/26  05:51:35  rlowe
# Treat compiler warnings as errors.  Eliminate such warnings from
# current ACP source.  Revise LCP handshake pulse width to be closer
# to 1.0 usec.
# Branch: LE_Cal_V112_0_2_branch
#
# Revision 1.1.28.3  1996/04/25  05:46:23  rlowe
# Clean out obsolete files and code.
# Branch: LE_Cal_V112_0_2_branch
#
# Revision 1.1.28.2  1996/04/10  02:33:29  rlowe
# Add SYNALC to platforms for sys dependancies.  Also fix-up
# integration of wbu makefile with sys makefile for obj-68-alc.
# Branch: LE_Cal_V112_0_2_branch
#
# Revision 1.1.28.1  1996/04/08  01:42:53  bbenson
# Branch: LE_Cal_V112_0_2_branch
# Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
#
# Revision 1.1.16.5  1996/04/06  03:34:15  rlowe
# Allow single location (sys or wbu) to drive entire cat5k ATM image
# make and dependencies.
# Branch: LE_Syn_Cal_V111_1_3_branch
#
# Revision 1.1.16.4  1996/04/02  05:08:20  rlowe
# Fix up include file paths.  Cleanup main ALC makefile regarding
# sar images and tools.
# Branch: LE_Syn_Cal_V111_1_3_branch
#
# Revision 1.1.16.3  1996/03/23  04:46:15  rlowe
# Fix include file usage and some makefile rules.
# Branch: LE_Syn_Cal_V111_1_3_branch
#
# Revision 1.1.16.2  1996/03/22  23:21:30  rlowe
# Allow IOS kernel portion to be made with -j8 parallel switch.
# Branch: LE_Syn_Cal_V111_1_3_branch
#
# Revision 1.1.16.1  1996/03/22  09:13:36  rlowe
# Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
# yielding LE_Syn_Cal_V111_1_3_branch.
# Branch: LE_Syn_Cal_V111_1_3_branch
#
# Revision 1.1.2.3  1996/03/21  06:47:37  rlowe
# Position pmon.static.srec as the checked-in S-record file for PMON.
# Change sarpud to sar_pod to be consistent.
# Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
#
# Revision 1.1.2.2  1996/03/20  10:52:50  rlowe
# Integrate with ios/wbu sub-tree and makefiles.
# Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
#
# Revision 1.1.2.1  1996/03/13  02:38:29  rlowe
# Add file content from Synergy repository.
# Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
#
# Revision 1.1  1996/03/11  07:55:41  rlowe
# Placeholders for California.
#
#
#------------------------------------------------------------------
# $Endlog$
#

include ../../makefile.c5k.alc

default: flash


ifeq ($(origin QUIET), undefined)
QUIET_COM=@\#
QUIET_CMD=@
QUIET_CC=
else
QUIET_COM=@
QUIET_CMD=@
QUIET_CC=@
endif


#
#	Make automagically generates the dependencies.  Arrange matters
#	so that dependencies are NOT generated twice if the User requests
#	them (and they do not already exist).
#

ifeq ($(wildcard dependancies),dependancies)
ALT_DEPEND_TARGETS := depend dependancies dependencies
else
ALT_DEPEND_TARGETS :=
endif


# Command-line or environment imports (by user/script):
#	LIST     - define this (non-empty) to generate listings.
#	FVERSION - flash (normal) image version to give to abl tool.
#	PVERSION - prom (boot) image version to give to abl tool.
#	IOSBOOT  - defined non-empty to indicate make chain called from ios/boot.
#	IOSKERN  - defined non-empty to indicate make chain called from ios/sys.
#	CPPDEFS  - defs and flags for cpp and gcc.
#	OPTFLGS  - optimization-only defs and flags for cpp and gcc.
#	IOSDEFS  - definitions used by IOS sys files.
#
# Defined locally, can be overriden/augmented:
#	MKSTUFF	- makefiles used in building code.

#
#	Override on command-line if different than this default.
#	Assign default if this is otherwise undefined.
#

ifeq ($(strip $(LIST)),)		# Command-line to enable listings.
LSTFLGS	= -pipe
else
LSTFLGS	= -Wa,-al > $(@:.o=.lst) 2>&1
endif

ifeq ($(strip $(PVERSION)),)	# for boot prom code -- user must override.
override PVERSION = 0.000
endif
export PVERSION

ifeq ($(strip $(FVERSION)),)	# for flash image code -- user must override.
override FVERSION = 0.000
endif
export FVERSION

ifeq ($(strip $(IOSDEFS)),)
override IOSDEFS = -DSYNALC -DOBJECT_68 -DTARGET_CISCO
endif


#
#	Static lists of files to use (ACP stuff) or exclude (SAR stuff).
#	Note that ALL standard .s assembly files are handled through ACP_sFILE.
#

SAR_DIRS  := src-3k-sar alc_sarinc alc_sarpmon alc_sartools
SAR_HDRS  := $(wildcard $(SAR_DIRS:%=../%/*.h))
SAR_FILES := $(wildcard $(SAR_DIRS:%=../%/*.[cCSs]))
TOOLFILES := $(wildcard ../alc_cbitools/*.[cCSs])

SRC_IPC_KERNEL	:= $(addprefix ../alc_ibipc/, ipcios.c ibipc.c ibdld.c)
SRC_IPC_ROM	:= $(addprefix ../alc_ibipc/, ipcdlboot.c ibipc.c ibdld.c)
SRC_FLASH_CSUM	:= $(wildcard ../alc_cbiflash/*.[cCS])
SRC_DIAGS	:= $(wildcard ../alc_sysdiags/*.[cCS])
SRC_IOS_DVRS	:= $(addprefix ../alc_drivers/, atmdriver.c led_tx_rx.c)
OBJ_IOS_DVRS	:= $(addsuffix .o, $(basename $(notdir $(SRC_IOS_DVRS))))

ACP_FILES := $(filter-out $(SAR_FILES) $(TOOLFILES) $(SRC_IPC_KERNEL) \
			$(SRC_IPC_ROM) $(SRC_FLASH_CSUM) $(SRC_DIAGS) \
			$(SRC_IOS_DVRS), $(wildcard ../*/*.[cCSs]))

ACP_sFILE := $(filter %.s, $(ACP_FILES))
ACP_SFILE := $(filter %.S, $(ACP_FILES))
ACP_CFILE := $(filter-out %.s %.S, $(ACP_FILES))
ACP_SsOBJ := $(addsuffix .o, $(basename $(notdir $(ACP_sFILE) $(ACP_SFILE))))
ACP_CcOBJ := $(addsuffix .o, $(basename $(notdir $(ACP_CFILE))))

SRC_ROM_BTINF := $(filter %bt_buildinfo.c, $(ACP_FILES))
OBJ_ROM_BTINF := $(addsuffix .o, $(basename $(notdir $(SRC_ROM_BTINF))))

SRC_FLS_BTINF := $(filter %bt2_buildinfo.c, $(ACP_FILES))
OBJ_FLS_BTINF := $(addsuffix .o, $(basename $(notdir $(SRC_FLS_BTINF))))

SRC_TOOL_SUPT := $(filter %libcom.c %libcom2.c, $(SRC_FLASH_CSUM)) \
		 $(filter %bcs_util.c, $(ACP_FILES))
OBJ_TOOL_SUPT := $(addsuffix .o, $(basename $(notdir $(SRC_TOOL_SUPT))))

TOOL_NAMES	  := $(basename $(notdir $(TOOLFILES)))

TAGS_HDRS  := $(filter-out $(SAR_HDRS), $(wildcard ../*/*.h))
TAGS_SRCS  := $(ACP_FILES) $(TOOLFILES) $(SRC_IPC_KERNEL) $(SRC_IPC_ROM) \
			$(SRC_FLASH_CSUM) $(SRC_DIAGS $(SRC_IOS_DVRS)
TAGS_FILES := $(TAGS_HDRS) $(filter-out %.s, $(TAGS_SRCS))

MKSTUFF := makefile


#
#	Static lists of various target files, including our special tools.
#

PROM_STEM	:= acpprom
PROM_LD		:= prom.ld			# linker script.
FLASH_IMAGE	:= acpflash.bbi

ROMBOOT_STEM	:= acppowbt
ROMBOOT_LD	:= rombt.ld			# linker script.

FLASHBOOT_STEM	:= boot2
FLASHBOOT_ACP	:= $(FLASHBOOT_STEM).aout
FLASHBOOT_LD	:= flashbt.ld			# linker script.

EXPLODER_ACP	:= expl_acp.aout

SAR_OBJDIR	:= ../obj-3k-sar
SAR_PMON	:= pmon.static.bin
SAR_PU_DIAG	:= sar_pod.bin
SAR_RX_RT	:= rxsar_no_pmon.bin
SAR_TX_RT	:= txnopmon.bin
SAR_RX_OL_DIAG	:= rxoffdiag.bin
SAR_TX_OL_DIAG	:= txoffdiag.bin
FLASH_SARIMAGES := $(SAR_RX_RT) $(SAR_TX_RT) $(SAR_RX_OL_DIAG) \
			$(SAR_TX_OL_DIAG) $(SAR_PU_DIAG) $(SAR_PMON)

EMANATE_OBJDIR	:= ../../../snmp-em/obj-68-alc
FLASH_ACPIMAGES	:= $(FLASHBOOT_ACP) $(EXPLODER_ACP)
FLASH_IOSBOOT	:= ../../../boot/obj-68-alc/alc.aout
FLASH_IOSKERNEL	:= ../../../sys/obj-68-alc/alc-a-m.ios

FLASH_SUBIMAGES	:= $(FLASH_ACPIMAGES) sars iosboot
ifeq ($(strip $(IOSKERN)),)
FLASH_SUBIMAGES := $(FLASH_SUBIMAGES) ioskernel
endif

ifeq ($(strip $(IOSBOOT)),)
ALC_MKIBT_DEP = @echo "Checking IOS BOOT dependancies ..." ; \
		$(MAKE) -C $(dir $(FLASH_IOSBOOT)) depend
else
ALC_MKIBT_DEP = @echo "Suppressing IOS BOOT dependancies check"
endif


#
#	Static lists of libraries and their component files.
#
#	We give explicit archive rules to force serialization in case
#	parallel jobs are being run by make (ie. -j).
#
#	Certain sub-lists of object files may exist because they need
#	special compiler flags.
#
#	For in-band IPC, note that the SAME source files are compiled
#	with DIFFERENT compiler flags for boot and kernel use !  To
#	handle this we prefix the object files generated for the ROMBOOT
#	case to get unique object filenames.
#
#	bt_buildinfo.o, bt2_buildinfo.o are NOT shown in these lists.
#	They are re-built whenever the appropriate (rom, flash) boot
#	image is re-built.  Thus they are handled by the rules for
#	those specific boot images.
#

LIB_ROM_BOOT	:= lib_romboot.a
OBJ_ROM_BOOT	:= bt_vector.o bt_main.o bt_powdiag.o bt_globvar.o rom_traps.o \
			boot.o bt_c_powdiag.o

LIB_EXPLODER	:= lib_exploder.a
OBJ_EXPLODER	:= explode_acp.o

LIB_FLASH_BOOT	:= lib_flashboot.a
OBJ_FLASH_BOOT	:= bt2_start.o bt2_main.o

LIB_FLASH_DIAGS	:= lib_diags_flash.a
OBJ_C_FLSH_DIAG	:= $(addsuffix .o, $(basename $(notdir $(SRC_DIAGS))))
OBJ_A_FLSH_DIAG	:= MEMDIAG.o

LIB_BTGLUE_ROM	:= lib_btglue_rboot.a
OBJ_BTGLUE_ROM	:= btdl_glue.o

LIB_BTGLUE_FLSH	:= lib_btglue_flash.a
OBJ_BTGLUE_FLSH	:= btdl_sars.o

LIB_BTGLUE_COMN	:= lib_btglue_common.a
OBJ_BTGLUE_COMN	:= btdl_dvr.o btdl_dnld.o

LIB_BCS_SERVER	:= libbcssrvr.a
OBJ_BCS_SERVER	:= bcs_glue.o bcs_ServIf.o

LIB_BCS_CLIENT	:= libbcsclnt.a
OBJ_BCS_CLIENT	:= bcs_BindIf.o bcs_PromIf.o bcs_common.o

LIB_BCS_UTIL	:= libbcsutil.a
OBJ_BCS_UTIL	:= bcs_util.o

LIB_FLASH_DVR	:= lib_flashdvr.a
OBJ_FLASH_DVR	:= flash.o flash2.o intel.o amd.o flash_acp.o

LIB_FL_CHECKSUM	:= lib_flashcsum.a
OBJ_FL_CHECKSUM	:= libcom.o libcom2.o

LIB_IPC_KERNEL	:= lib_ipc_kernel.a
OBJ_IPC_KERNEL	:= $(addsuffix .o, $(basename $(notdir $(SRC_IPC_KERNEL))))

LIB_IPC_ROM	:= lib_ipc_rboot.a
OBJ_IPC_ROM	:= $(patsubst ib%, bt_ib%, \
			$(addsuffix .o, $(basename $(notdir $(SRC_IPC_ROM)))))

LIB_ATMDVR_COMN	:= lib_atmdvr_common.a
OBJ_ATMDVR_COMN	:= atmdriver.o txsarif.o cam_if.o atmdvr_gdata.o camtest.o

LIB_ATMDVR_FLSH	:= lib_atmdvr_flash.a
OBJ_ATMDVR_FLSH	:= atmdvr_poll.o

LIB_ACPDVR_COMN	:= lib_acpdvrs_common.a
OBJ_ACPDVR_COMN	:= bigainit.o bufmgr.o ntc_if.o ntc_dmabuf.o \
			syn_atm_pmc_oc3.o pmd_eeprom.o

LIB_LEDDVR_KERN	:= lib_leddvr_kernel.a
OBJ_LEDDVR_KERN	:= led_tx_rx.o

LIB_LCPLED_DVRS	:= lib_lcp_led_dvrs.a
OBJ_LCPLED_DVRS	:= lcp_low.o lcp_dvr.o lcp_pkts.o lcp_lane.o led_dvr.o

LIB_BIGADVR_ROM	:= lib_biga_rboot.a
OBJ_BIGADVR_ROM	:= bt_biga.o

LIB_MFPDVR_BOOT	:= libmfp_boot.a
OBJ_MFPDVR_BOOT	:= io_boot.o

LIB_MFPDVR_COMN	:= libmfp.a
OBJ_MFPDVR_COMN	:= bt_mfp.o mfp.o mfp_hwt.o sccmfp.o mfp_pollt.o

LIB_LIBC_BOOTS	:= libc_acp_boots.a
OBJ_LIBC_BOOTS	:= longmath.o memset.o strcpy.o strncpy.o strcmp.o strncmp.o \
			strpbrk.o strcat.o strcspn.o strncat.o strlen.o \
			strspn.o strtok.o doprnt.o sprintf.o vsscanf.o  \
			memchr.o index.o bitswap.o random.o rand.o bcopy.o

LIB_LIBC_SUBSET	:= libc_acp_kernel.a
OBJ_LIBC_SUBSET	:= memcpy.o sscanf.o vfscanf.o strtol.o strtoul.o \
			strchr.o strrchr.o

LIB_LOW_UTILS	:= liblow_util.a
OBJ_LOW_UTILS	:= sys_splx.o sys_cache.o lo_longjmp.o lo_memblks.o \
			lo_memdiag.o lo_code.o lo_error.o lo_wdog.o

#
#	Lists of libraries used to create distinct images.
#	These also tell the linker the library linkage order.
#

LIBLIST_ALL_FLASH := \
	$(LIB_FLASH_BOOT) $(LIB_FLASH_DIAGS) $(LIB_BTGLUE_FLSH) \
	$(LIB_BTGLUE_COMN) $(LIB_BCS_SERVER) $(LIB_BCS_CLIENT)	\
	$(LIB_BCS_UTIL) $(LIB_FLASH_DVR) $(LIB_FL_CHECKSUM)	\
	$(LIB_IPC_KERNEL) $(LIB_ATMDVR_COMN) $(LIB_ATMDVR_FLSH)	\
	$(LIB_ACPDVR_COMN) $(LIB_LEDDVR_KERN) $(LIB_LCPLED_DVRS) \
	$(LIB_MFPDVR_BOOT) $(LIB_MFPDVR_COMN) $(LIB_LIBC_BOOTS)	\
	$(LIB_LIBC_SUBSET) $(LIB_LOW_UTILS) $(LIB_EXPLODER)

LIBLIST_IOS_BOOT := \
	$(LIB_BCS_CLIENT) $(LIB_BCS_UTIL) $(LIB_FL_CHECKSUM)	\
	$(LIB_MFPDVR_COMN) $(LIB_LIBC_BOOTS) $(LIB_LIBC_SUBSET)	\
	$(LIB_LOW_UTILS)

LIBLIST_FLASH_BOOT := \
	$(LIB_FLASH_BOOT) $(LIB_BCS_SERVER) $(LIB_FLASH_DIAGS)	\
	$(LIB_BTGLUE_FLSH) $(LIB_BTGLUE_COMN) $(LIB_BCS_CLIENT)	\
	$(LIB_BCS_UTIL) $(LIB_FLASH_DVR) $(LIB_FL_CHECKSUM)	\
	$(LIB_ATMDVR_COMN) $(LIB_ATMDVR_FLSH) $(LIB_ACPDVR_COMN) \
	$(LIB_LCPLED_DVRS) $(LIB_MFPDVR_BOOT) $(LIB_MFPDVR_COMN) \
	$(LIB_LIBC_BOOTS) $(LIB_LIBC_SUBSET) $(LIB_LOW_UTILS)

LIBLIST_EXPLODER := \
	$(LIB_EXPLODER) $(LIB_FLASH_DVR) $(LIB_MFPDVR_COMN) $(LIB_LOW_UTILS)

LIBLIST_ROMBOOT := \
	$(LIB_ROM_BOOT) $(LIB_BTGLUE_ROM) $(LIB_BTGLUE_COMN)	\
	$(LIB_BCS_CLIENT) $(LIB_BCS_UTIL) $(LIB_FLASH_DVR)	\
	$(LIB_FL_CHECKSUM) $(LIB_IPC_ROM) $(LIB_LCPLED_DVRS)	\
	$(LIB_BIGADVR_ROM) $(LIB_MFPDVR_BOOT) $(LIB_MFPDVR_COMN) \
	$(LIB_LIBC_BOOTS) $(LIB_LIBC_SUBSET) $(LIB_LOW_UTILS)

#
#	Compiler switches and flags.
#

IBIPCDEFS	= -DNO_VLAN $(IOSDEFS)
BT_IBIPCDEFS	= $(IBIPCDEFS) -DIOS_BOOT
FLASHDVRDEFS	:= -DSYNALC
FLASHDVRFLAGS	:= $(FLASHDVRDEFS) -O

DIAGS_DEFS	:= -DACP -DDEBUGFUNC -DDIAGTEST -DCALYPSO -DGNU
DIAGS_FLAGS	:= $(DIAGS_DEFS) -O0

INCDIRS := . ../alc_include ../alc_include/catalyst ../alc_cominc ../sysif \
		../../../sys/h ../../../sys/machine ../../../sys/lib/cisco/include
INCPATH := $(INCDIRS:%=-I%)

DBGFLGS	:= -g -DDEBUG -DPROTO
CPPFLGS	:= $(INCPATH) $(CPPDEFS) $(DBGFLGS) -nostdinc -DGCC68K
CCFLAGS	:= -m68030 -mbitfield $(OPTFLGS) $(CPPFLGS) -Werror
C_ONLYF	:= -DLANGUAGE_C

INCDIRS.tools := ../alc_bcs ../alc_cbiflash /usr/include $(INCDIRS)
INCPATH.tools := $(INCDIRS.tools:%=-I%)
CPPFLGS.tools := -DSYNALC -DUTILITY -DDEBUG -DGCC68K $(INCPATH.tools) $(CPPDEFS)
CCFLAGS.tools := $(CPPFLGS.tools) -O -Werror


#
#	Make rules for the world.  The "all" rule must be first, of course.
#	General high-level targets are "all", "prom", "flash", and "depend".
#
#	Note use of "WBU_DRIVE" when dispatching a submake into ios/sys.
#	This prevents the sys/obj-68-alc/makefile from dispatching a submake
#	back into here !!
#

all : prom flash

libs : $(LIBLIST_ALL_FLASH) emanate

sars : FRC
	$(MAKE) -C $(SAR_OBJDIR) all

emanate : FRC
#	$(MAKE) -C $(EMANATE_OBJDIR) all

for_iosboot : $(MKSTUFF) $(LIBLIST_IOS_BOOT)

iosboot : for_iosboot
	$(MAKE) -C $(dir $(FLASH_IOSBOOT)) "WBU_DRIVE=yes" default

ioskernel : libs
	$(MAKE) $(MAKEFLAG_J) -C $(dir $(FLASH_IOSKERNEL)) \
		"WBU_DRIVE=yes" default


#
#	ACP Boot PROM image containing Primary Boot.
#	Downstream we may have special manufacturing diags in PROM too.
#	A BCS-style header is included to provide PROM image checksums.
#
#	It seems that only the older GNU "ld" allows us to concatenate
#	multiple s-record images.  (930630)
#

prom : $(PROM_STEM).srec

$(PROM_STEM).srec : $(ROMBOOT_STEM).srec $(MKSTUFF) $(PROM_LD) $(PRMTOOL)
	@rm -f $(PROM_STEM).hdr
	$(PRMTOOL) -aout -debug -out $(PROM_STEM).hdr \
		-vers $(PVERSION) -bonen $(ROMBOOT_STEM).aout
	$(LD) -o $@ -Map $(PROM_STEM).map -T $(PROM_LD)

$(ROMBOOT_STEM).srec : $(ROMBOOT_STEM).aout $(MKSTUFF)
	$(OC) -I a.out-zero-big -O srec -S -v --gap-fill=0 $< $@

$(ROMBOOT_STEM).aout : $(MKSTUFF) $(ROMBOOT_LD) \
			$(LIBLIST_ROMBOOT) $(OBJ_ROM_BTINF)
	$(LD) -o $@ -Map $(ROMBOOT_STEM).map -oformat a.out-zero-big \
		-u __bt_VectorTbl -e __Boot_main -N \
		-Ttext 0x00000000 -Tdata 0x40ff0000 \
		-defsym _buginf=_printf \
		-T $(ROMBOOT_LD) $(LIBLIST_ROMBOOT) $(OBJ_ROM_BTINF)
	$(NM) -n $@ > $(ROMBOOT_STEM).nmap
	$(STRIP) $@

$(OBJ_ROM_BTINF) : $(SRC_ROM_BTINF) $(MKSTUFF) $(ROMBOOT_LD) $(LIBLIST_ROMBOOT)


#
#	ACP full flash image, containing everything but the Primary Boot.
#	This is the current production mechanism.  Note that we drive
#	the libraries first, then the sub-images just link these libs.
#

#
#	The ordering of the next few (3) rules is CRITICAL because of the
#	use of FLASH_IMAGE.  Do NOT play with this unless you know make !!
#

flash : $(FLASH_IMAGE)

for_kernel $(FLASH_IMAGE) : $(MKSTUFF) $(LIBLIST_ALL_FLASH) $(FLASH_SUBIMAGES)

fast $(FLASH_IMAGE) : $(ABLTOOL)
	$(ABLTOOL) -debug -out $(FLASH_IMAGE) -vers $(FVERSION) \
		-bcss `awk '/_BCS_Service/{print $$1}' $(FLASHBOOT_STEM).nmap` \
		-expln $(EXPLODER_ACP) \
		-btwon $(FLASHBOOT_ACP) \
		-iosbn $(FLASH_IOSBOOT) \
		-pmonn $(SAR_OBJDIR)/$(SAR_PMON) \
		-spudn $(SAR_OBJDIR)/$(SAR_PU_DIAG) \
		-tsarn $(SAR_OBJDIR)/$(SAR_TX_RT) \
		-rsarn $(SAR_OBJDIR)/$(SAR_RX_RT) \
		-kernn $(FLASH_IOSKERNEL) \
		-todgn $(SAR_OBJDIR)/$(SAR_TX_OL_DIAG) \
		-rodgn $(SAR_OBJDIR)/$(SAR_RX_OL_DIAG)
	rm -f $(dir $(FLASH_IOSKERNEL))$(FLASH_IMAGE)
	cp -p $(FLASH_IMAGE) $(dir $(FLASH_IOSKERNEL))$(FLASH_IMAGE)

flash_acp : $(FLASH_ACPIMAGES)

$(FLASHBOOT_ACP) : $(MKSTUFF) $(FLASHBOOT_LD) \
			$(LIBLIST_FLASH_BOOT) $(OBJ_FLS_BTINF)
	$(LD) -o $@ -Map $(FLASHBOOT_STEM).map -oformat a.out-zero-big \
		-u __Bt2_Start -e __Bt2_Start -u _BCS_Service \
		-N -Ttext 0x90010000 -Tdata 0x40fa0000 \
		-defsym _buginf=_printf \
		-T $(FLASHBOOT_LD) $(LIBLIST_FLASH_BOOT) $(OBJ_FLS_BTINF)
	$(NM) -n $@ > $(FLASHBOOT_STEM).nmap
	$(STRIP) -s -v $@

$(OBJ_FLS_BTINF) : $(SRC_FLS_BTINF) $(MKSTUFF) \
			$(FLASHBOOT_LD) $(LIBLIST_FLASH_BOOT)

$(EXPLODER_ACP) : $(MKSTUFF) $(LIBLIST_EXPLODER)
	$(LD) -o $@ -Map $(@:.aout=.map) -oformat a.out-zero-big \
		-u _CBI_Explode -e _CBI_Explode -N -Ttext 0x40f80000 \
		$(LIBLIST_EXPLODER)
	$(NM) -n $@ > $(@:.aout=.nmap)
	$(STRIP) -s -v $@


#
#	Post-process the SAR S-record images into flat binary.
#	Most SAR images are built by their own makefile, but some
#	(ie. PMON) are checked in as S-record files.
#

$(FLASH_SARIMAGES) : %.bin : $(SAR_OBJDIR)/%.srec
	-rm -f $(@).tmp $@
	sed -e "/^S4/d" -e "/^S./!d" $< > $(@).tmp
	$(OC) -I srec -O binary -S -v --gap-fill=0 $(@).tmp $@
	-rm -f $(@).tmp


#
#	Dependencies and other rules.  Use "depend" pseudo-targets if
#	want to force dependency generation.  NB. Trick of defining
#	dependency pseudo-targets depending on existence of dependency
#	file itself.
#

depend dependancies dependencies :

../obj-68-alc/dependancies $(ALT_DEPEND_TARGETS) :
	-@rm -f dependancies
	@touch dependancies
	@echo Assembly files...
	@for LOOP in $(ACP_sFILE) ; do \
		echo `basename $$LOOP .s`.o": "$$LOOP >> dependancies ; done
	-@$(CC) -M -c $(CCFLAGS) $(A_ONLYF) $(ACP_SFILE) >> dependancies
	@echo Most C files...
	-@$(CC) -M -c $(CCFLAGS) $(C_ONLYF) $(ACP_CFILE) >> dependancies
	@echo IPC C files...
	-@$(CC) -M -c $(CCFLAGS) $(C_ONLYF) $(IBIPCDEFS) \
			$(SRC_IPC_KERNEL) >> dependancies
	-@$(CC) -M -c $(CCFLAGS) $(C_ONLYF) $(BT_IBIPCDEFS) $(SRC_IPC_ROM) | \
			sed -e "s/ibdld\.o/bt_ibdld.o/" \
			    -e "s/ibipc\.o/bt_ibipc.o/" >> dependancies
	@echo Flash C files...
	-@$(CC) -M -c $(CCFLAGS) $(C_ONLYF) $(FLASHDVRDEFS) \
					$(SRC_FLASH_CSUM) >> dependancies
	@echo Diags C files...
	-@$(CC) -M -c $(CCFLAGS) $(C_ONLYF) $(DIAGS_DEFS) \
					$(SRC_DIAGS) >> dependancies
	@echo IOS-centric C files...
	-@$(CC) -M -c $(CCFLAGS) $(C_ONLYF) $(IOSDEFS) \
					$(SRC_IOS_DVRS) >> dependancies
	@echo Tools support C files...
	-@$(CC_HOST) -MM -c $(CCFLAGS.tools) $(C_ONLYF) $(SRC_TOOL_SUPT) \
			$(TOOLFILES) | sed -e "s/\.o/_solaris.o/" >>dependancies
	-@$(CC_HOST) -MM -c $(CCFLAGS.tools) $(C_ONLYF) $(SRC_TOOL_SUPT) \
			$(TOOLFILES) | sed -e "s/\.o/_sunos.o/" >> dependancies
	$(ALC_MKIBT_DEP)
	@echo "Computing TAGS file"
	@etags -t $(TAGS_FILES)
	@echo "Computing ID file"
	@mkid '-S.S=asm' '-Sasm-c|' '-Sasm+u' '-Sasm+p' $(TAGS_FILES)

null:					# Just an empty target.

clean:		
	-rm -f *.[ao] *map *.lst *.srec made*

nodepend:
	-rm -f dependancies

#
#	Allow auto dependency generation if the file does NOT already exist.
#

-include ../obj-68-alc/dependancies

FRC:

.PHONY: FRC depend clean all nodepend prom flash fast libs sars emanate \
	iosboot ioskernel tools TOOLS_SUNOS TOOLS_SOLARIS dependencies \
	flash_acp for_kernel for_iosboot dependancies null


#
#	Dependencies and rules for libraries and object files.
#

$(ACP_SsOBJ) :
	$(QUIET_COM)echo "Compiling $< to $@"
	$(QUIET_CC)$(CC) -c -o $@ $(CCFLAGS) $(A_ONLYF) $< $(LSTFLGS) \
			|| (rm -f $@ ; exit 1)

$(ACP_CcOBJ) :
	$(QUIET_COM)echo "Compiling $< to $@"
	$(QUIET_CC)$(CC) -c -o $@ $(CCFLAGS) $(C_ONLYF) $< $(LSTFLGS) \
			|| (rm -f $@ ; exit 1)

$(OBJ_IPC_KERNEL) :
	$(QUIET_COM)echo "Compiling $< to $@"
	$(QUIET_CC)$(CC) -c -o $@ $(CCFLAGS) $(C_ONLYF) $(IBIPCDEFS) $< $(LSTFLGS) \
			|| (rm -f $@ ; exit 1)

$(OBJ_IPC_ROM) :
	$(QUIET_COM)echo "Compiling $< to $@"
	$(QUIET_CC)$(CC) -c -o $@ $(CCFLAGS) $(C_ONLYF) $(BT_IBIPCDEFS) $< $(LSTFLGS) \
			|| (rm -f $@ ; exit 1)

$(OBJ_FLASH_DVR) $(OBJ_FL_CHECKSUM) $(OBJ_EXPLODER) :
	$(QUIET_COM)echo "Compiling $< to $@"
	$(QUIET_CC)$(CC) -c -o $@ $(CCFLAGS) $(C_ONLYF) $(FLASHDVRFLAGS) $< $(LSTFLGS) \
			|| (rm -f $@ ; exit 1)

$(OBJ_C_FLSH_DIAG) :
	$(QUIET_COM)echo "Compiling $< to $@"
	$(QUIET_CC)$(CC) -c -o $@ $(CCFLAGS) $(C_ONLYF) $(DIAGS_FLAGS) $< $(LSTFLGS) \
			|| (rm -f $@ ; exit 1)

$(OBJ_IOS_DVRS) :
	$(QUIET_COM)echo "Compiling $< to $@"
	$(QUIET_CC)$(CC) -c -o $@ $(CCFLAGS) $(C_ONLYF) $(IOSDEFS) $< $(LSTFLGS) \
			|| (rm -f $@ ; exit 1)

%.a :
	$(AR) -rcvs $@ $?

$(LIB_ROM_BOOT)    : $(OBJ_ROM_BOOT)
$(LIB_EXPLODER)    : $(OBJ_EXPLODER)
$(LIB_FLASH_BOOT)  : $(OBJ_FLASH_BOOT)
$(LIB_FLASH_DIAGS) : $(OBJ_C_FLSH_DIAG) $(OBJ_A_FLSH_DIAG)
$(LIB_BTGLUE_ROM)  : $(OBJ_BTGLUE_ROM)
$(LIB_BTGLUE_FLSH) : $(OBJ_BTGLUE_FLSH)
$(LIB_BTGLUE_COMN) : $(OBJ_BTGLUE_COMN)
$(LIB_BCS_SERVER)  : $(OBJ_BCS_SERVER)
$(LIB_BCS_CLIENT)  : $(OBJ_BCS_CLIENT)
$(LIB_BCS_UTIL)    : $(OBJ_BCS_UTIL)
$(LIB_FLASH_DVR)   : $(OBJ_FLASH_DVR)
$(LIB_FL_CHECKSUM) : $(OBJ_FL_CHECKSUM)
$(LIB_IPC_KERNEL)  : $(OBJ_IPC_KERNEL)
$(LIB_IPC_ROM)     : $(OBJ_IPC_ROM)
$(LIB_ATMDVR_COMN) : $(OBJ_ATMDVR_COMN)
$(LIB_ATMDVR_FLSH) : $(OBJ_ATMDVR_FLSH)
$(LIB_ACPDVR_COMN) : $(OBJ_ACPDVR_COMN)
$(LIB_LEDDVR_KERN) : $(OBJ_LEDDVR_KERN)
$(LIB_LCPLED_DVRS) : $(OBJ_LCPLED_DVRS)
$(LIB_BIGADVR_ROM) : $(OBJ_BIGADVR_ROM)
$(LIB_MFPDVR_BOOT) : $(OBJ_MFPDVR_BOOT)
$(LIB_MFPDVR_COMN) : $(OBJ_MFPDVR_COMN)
$(LIB_LIBC_BOOTS)  : $(OBJ_LIBC_BOOTS)
$(LIB_LIBC_SUBSET) : $(OBJ_LIBC_SUBSET)
$(LIB_LOW_UTILS)   : $(OBJ_LOW_UTILS)


#
#	ACP image tools.
#
#	Since we do not have a dependency file for each of the main
#	tool files, force their compilation on each relevant make run.
#

tools : $(ACP_TOOLS)

%-$(NATIVE_OSREV) : %-$(NATIVE_OSREV).o $(OBJ_TOOLS)
	$(CC_HOST) $(CCFLAGS.tools) $(C_ONLYF) \
				-o $@ $^

libcom-$(NATIVE_OSREV).o : ../alc_cbiflash/libcom.c
	$(CC_HOST) -c -o $@ $(CCFLAGS.tools) $(C_ONLYF) $< \
			|| (rm -f $@ ; exit 1)

libcom2-$(NATIVE_OSREV).o : ../alc_cbiflash/libcom2.c
	$(CC_HOST) -c -o $@ $(CCFLAGS.tools) $(C_ONLYF) $< \
			|| (rm -f $@ ; exit 1)

bcs_util-$(NATIVE_OSREV).o : ../alc_bcs/bcs_util.c
	$(CC_HOST) -c -o $@ $(CCFLAGS.tools) $(C_ONLYF) $< \
			|| (rm -f $@ ; exit 1)

abl-$(NATIVE_OSREV).o : ../alc_cbitools/abl.c
	$(CC_HOST) -c -o $@ $(CCFLAGS.tools) $(C_ONLYF) $< \
			|| (rm -f $@ ; exit 1)

csumaout-$(NATIVE_OSREV).o : ../alc_cbitools/csumaout.c
	$(CC_HOST) -c -o $@ $(CCFLAGS.tools) $(C_ONLYF) $< \
			|| (rm -f $@ ; exit 1)

#	Leave an empty line here.
