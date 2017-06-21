/* $Id: snmp_flashmib_cmnds.h,v 3.3.60.1 1996/04/22 18:57:52 ssangiah Exp $
 * $Source: /release/112/cvs/Xsys/dev/snmp_flashmib_cmnds.h,v $
 *------------------------------------------------------------------
 * snmp_flashmib_cmnds.h - High level flash routines to support the
 *			   Dual Flash Bank (DFB) flash MIB
 *
 * April 1995, Shalabh Agarwal
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: snmp_flashmib_cmnds.h,v $
 * Revision 3.3.60.1  1996/04/22  18:57:52  ssangiah
 * CSCdi54920:  Need to add support for the flash MIB on c7200.
 * Branch: California_branch
 *
 * Revision 3.3  1995/11/17  09:01:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:15:46  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:20:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:54:29  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/09/13  04:06:56  hampton
 * The Fall of the House of Msclock.  [CSCdi40182]
 *
 * Revision 2.1  1995/06/07 20:25:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SNMP_FLASHMIB_CMNDS_H__
#define __SNMP_FLASHMIB_CMNDS_H__

#define	TO_FLASH	0
#define FROM_FLASH	1

/*
 * Prototypes
 */
/*
 * Prototypes for the Device Information Group functions.
 */
extern ulong snmp_flash_dev_size(char *);
extern ulong snmp_flash_dev_min_partn_size(char *);
extern int snmp_flash_dev_max_partns(char *);
extern int snmp_flash_dev_num_partns(char *);
extern char *snmp_flash_dev_desc(char *);
extern char *snmp_flash_dev_cntlr(char *);
extern OID *snmp_flash_dev_card_index(char* devnm);
extern int snmp_flash_dev_vpp(char *);
extern sys_timestamp snmp_flash_dev_init_t(char *);
extern boolean snmp_flash_dev_removable(char *);
extern boolean snmp_flash_dev_removed(char *);
extern int snmp_flash_device_count(void);
extern int snmp_flash_dev_num_partns(char *devnm);
extern int snmp_flash_device_chipcnt(char *devnm);

/*
 * Prototypes for the Chip Group functionse.
 */
extern void snmp_flash_chipcode(char *, int, uchar *);
extern uchar *snmp_flash_chipdesc(char *, int);
extern int snmp_flash_chip_wretries(char *, int);
extern int snmp_flash_chip_eretries(char *, int);
extern int snmp_flash_chip_max_wretries(char *);
extern int snmp_flash_chip_max_eretries(char *);

/*
 * Prototypes for the Partition Information Group functions.
 */
extern long snmp_flash_partition_number(char *devnm);
extern void snmp_flash_partition_chip_bounds(char *devnm, long *startchip,
                                                        long *endchip);
extern long snmp_flash_partition_size(char *devnm);
extern long snmp_flash_partition_free_space(char *devnm);
extern long snmp_flash_partition_filecount(char *devnm);
extern long snmp_flash_partition_chksum_algo(char *devnm);
extern long snmp_flash_partition_status(char *devnm);
extern long snmp_flash_partition_upgrade_method(char *devnm);
extern char *snmp_flash_partition_name(char *devnm);
extern long snmp_flash_partition_need_erasure(char *devnm);
extern long snmp_flash_partition_filename_length(char *devnm);

/*
 * Prototypes for the File Group functions.
 */
extern int snmp_flash_fileindx(char *, int);
extern int snmp_flash_file_size(char *, int);
extern void snmp_flash_filechksum(char *, int, uchar *);
extern int snmp_flash_filestatus(char *, int);
extern uchar *snmp_flash_filename(char *, int);

/*
 * Prototypes for the Copy Operation functions.
 */
extern int snmp_tftp_to_flash(addrtype *address, char *sfname, char *dfname, 
			      long *time_elapsed, boolean erase);
extern int snmp_flash_to_tftp(addrtype *address, char *sfname, char *dfname,
			      long *time_elapsed);
extern int snmp_rcp_to_flash(addrtype *address, char *sfname, char *dfname, 
                             char* user, long *time_elapsed, boolean erase);
extern int snmp_flash_to_rcp (addrtype *address, char *sfname, char *dfname,
			      char *user, long *time_elapsed);
extern int snmp_flhlog_to_server(addrtype *address, char *dfname,
                                 long *time_elapsed);

/*
 * Prototypes for the Partitioning Operation functions.
 */
extern boolean partition_flash_snmp_wrapper(ciscoFlashPartitioningEntry_t 
                                                                       *entry);
/*
 * Prototypes for the Miscellaneous Operation functions.
 */
extern int snmp_copy_misc_op_status(int flash_status);
extern boolean snmp_flash_erase(ciscoFlashMiscOpEntry_t *entry,
                                                    flash_copy_parms_t *fparms);
extern boolean snmp_flash_verify(ciscoFlashMiscOpEntry_t *entry,
                                                    flash_copy_parms_t *fparms);
extern boolean snmp_flash_delete(ciscoFlashMiscOpEntry_t *entry,
                                                    flash_copy_parms_t *fparms);
extern boolean snmp_flash_undelete(ciscoFlashMiscOpEntry_t *entry,
                                                    flash_copy_parms_t *fparms);
extern boolean snmp_flash_squeeze(ciscoFlashMiscOpEntry_t *entry,
                                                    flash_copy_parms_t *fparms);
extern boolean snmp_flash_format(ciscoFlashMiscOpEntry_t *entry,
                                                    flash_copy_parms_t *fparms);

/*
 * Prototypes for misc. functions.
 */
extern int snmp_init_dev_database(void);
extern void snmp_make_devnm(int devnum, int partnum, char *devnm);
extern boolean snmp_process_name(char *dest_name, flash_copy_parms_t *fparms);
extern boolean partition_flash_snmp(ciscoFlashPartitioningEntry_t *part_entry);

#endif /* __SNMP_FLASHMIB_CMNDS_H__ */
