/* $Id: mci_err.c,v 3.1 1995/11/09 09:07:22 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/diag/mci/mci_err.c,v $
 *------------------------------------------------------------------
 * mci_err.c -- MCI diagnostic error routines
 *
 * 8-December-1989, Steve Grant (Access Associates)
 *
 * Copyright (c) 1989-1992 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mci_err.c,v $
 * Revision 3.1  1995/11/09  09:07:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:13:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1992/03/07 23:56:57  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../../defs.h"
#include "../../types.h"
#include "../../stdio.h"
#include "../h/if_mci.h"
#include "../h/if_fci.h"
#include "../h/mci_err.h"

#define BUFLEN 30

extern mcitype  *mcidata[];
extern dcitype  *dcidata[];
MCI_TEST        *diag_results[MAXCTRLLERS];

/*
 * function declarations
 */

extern void     printf(), bzero();
void            init_struct(), init_error_struct(), print_errors(), show_err_mask(),
                show_auto_results(), show_auto_interface_results(), get_num(), show_all_errors();
boolean         store_error();

/*
 *  init_struct initializes the MCI_TEST structure pointed to by diag
 */

void init_struct(diag)
    MCI_TEST *diag;
{
    int i;
    
    bzero((uchar *)diag, sizeof(MCI_TEST));
    diag->bus_test.stat = STAT_NOT_DONE;
    diag->bus_test.flag = NO_FLAG;
    diag->mema_test.stat = STAT_NOT_DONE;
    diag->mema_test.flag = NO_FLAG;
    diag->memd_test.stat = STAT_NOT_DONE;
    diag->memd_test.flag = NO_FLAG;
    diag->xmem_test.stat = STAT_NOT_DONE;
    diag->xmem_test.flag = NO_FLAG;
    for (i = 0; i < FCI_MAX_CTRLR_INTERFACES; ++i)  {

        diag->add_flt_mem[i].stat = STAT_NOT_DONE;
        diag->add_flt_mem[i].flag = NO_FLAG;
        diag->add_flt[i].stat = STAT_NOT_DONE;
        diag->add_flt[i].flag = AF_FLAG;
        diag->ether_ping_test[i].stat = STAT_NOT_DONE;
        diag->ether_ping_test[i].flag = PING_FLAG;
        diag->serial_ping_test[i].stat = STAT_NOT_DONE;
        diag->serial_ping_test[i].flag = PING_FLAG;
        
    }  /*  end for (i)  */
    
}  /*  end init_struct()  */

/*
 *  init_error_struct initializes all of the arrays of MCI_ERROR structures
 */

void init_error_struct(max_ctlrs)
    short   max_ctlrs;
{
    int i;
    MCI_TEST *diag;
    
    for (i = 0; i < max_ctlrs; ++i) 
        init_struct(diag_results[i]);
    
}  /*  end of init_error_struct()  */

/*
 *  print_errors is passed the address of an array of MCI_ERROR structures
 *  and a test name string.  It then prints to stdout a screen-full of data
 *  and prompts the user for --More--.
 */

void print_errors(err_struct, test, linect)
    MCI_ERROR *err_struct;
    char *test;
    int *linect;
{
    int i;
    
    printf("\n  MCI %s errors", test);
    switch (err_struct->flag) {
      case NO_FLAG:
        if (err_struct->data_is[0] == 0 && err_struct->data_sb[0] == 0 && err_struct->data_addr[0] == 0) {
            printf("\n  Unknown error.  Run test in VERBOSE mode with REPORTING for more info");
            ++(*linect);
            return;
        }
        else {
            show_err_mask(err_struct);
            *linect += 3;
            for (i = 0; i < err_struct->count; ++i) {
                printf("\n  Wrote 0x%-04x, read 0x%-04x at address 0x%-04x", err_struct->data_sb[i],
                       err_struct->data_is[i], err_struct->data_addr[i]);
                if (quit_display(++(*linect), NULL))
                    break;
            }
        }
        break;
      case AF_FLAG:
        if ((err_struct->af_sent == 0) && (err_struct->af_enabled == 0)) {
            printf("\n  Unknown error.  Run test in VERBOSE mode with REPORTING for more info");
            ++(*linect);
            return;
        }
        else {
            printf("\n  Enabled 0x%x, sent 0x%x, fsm is %d, octet is %d", err_struct->af_enabled,
                   err_struct->af_sent, err_struct->data1[0], err_struct->data2[0]);
            if (quit_display(++(*linect), NULL))
                break;
        }
        break;
      case PING_FLAG:
        if ((err_struct->data1[0] == 0) && (err_struct->data2[0] == 0)) {
            printf("\n  Unknown error.  Run test in VERBOSE mode with REPORTING for more info");
            ++(*linect);
            return;
        }
        else {
            switch(err_struct->af_enabled) { /* Use unused field to determine
                                              * which error occurred.         */
              case 0:
                printf("\n  Unable to select interface.");
                break;
              case 1:
                printf("\n  Unable to allocate buffers.");
                break;
              case 2:
                printf("\n  Unable to setup address filter.");
                break;
              case 3:
                printf("\n  Applique type wrong or reporting incorrectly.");
                printf("\n     Should be RS232_DCE (0x%x), reports 0x%x",
                        err_struct->data_sb[err_struct->count],
                        err_struct->data_is[err_struct->count]);
              case 4:
                printf("\n  Unable to enable port.  Response = 0x%x",
                        err_struct->data_is[err_struct->count]);
                break;
              case 5:
                printf("\n  Unknown interface type.");
                break;
              case 6:
                printf("\n  packet length sent : %d, received %d",
                        err_struct->data1[err_struct->count],
                        err_struct->data2[err_struct->count]);
                break;
              case 7:
                printf("\n  Data mismatch on packet length %d",
                        err_struct->data1[err_struct->count]);
                printf("\n     byte %d should be 0x%x, was 0x%x",
                        err_struct->data_addr[err_struct->count],
                        err_struct->data_sb[err_struct->count],
                        err_struct->data_is[err_struct->count]);
                break;
              case 8:
                printf("\n  Did not receive packet length %d.",
                        err_struct->data1[err_struct->count]);
                break;
              case 9:
                printf("\n  Unable to transmit.");
                break;
              case 10:
                printf("\n  Unable to receive.");
                break;
              case 11:
                printf("\n  Unable to loopback.");
                break;
              case 12:
                printf("\n  Unable to send and receive at same time.");
                break;
              case 13:
                printf("\n  Enable interface failed!!.");
                break;

            }
        }
        break;
        
    }  /*  end switch(err_struct->flag)  */
    
}  /*  end print_errors()  */

/*
 *  show_err_mask displays the bad_bits member of the MCI_ERROR struct passed
 *  it and attempts to interpet the data if possible.
 */

void show_err_mask(err_struct)
    MCI_ERROR *err_struct;
{
    if (err_struct->bad_bits == 0xFFFF)
        printf("\n  Major data path problem.  All data bits are bad!");
    
    printf("\n  Error mask = 0x%-04x", err_struct->bad_bits);
    printf("\n  Any bits \"ON\" signifies an problem with that bit");
    
}  /*  end show_err_mask()  */

/*
 *  store_error adds the error information to the MCI_ERROR struct pointed to
 *  by err_struct.  This is done only if err_struct->bad_bits if different
 *  from good EOR bad.
 */

boolean store_error(err_struct, good, bad, address, data1, data2, af_sent, af_enabled)
    MCI_ERROR *err_struct;
    ushort good, bad, address, af_sent, af_enabled;
    int data1, data2;
{
    ushort mask1, mask2;
    
    err_struct->stat = STAT_FAIL;                       /*  indicate test failed  */
    switch (err_struct->flag) {
      case AF_FLAG:
        err_struct->data1[err_struct->count] = data1;   /*  fsm  */
        err_struct->data1[err_struct->count] = data2;   /*  octet  */
        err_struct->af_sent = af_sent;
        err_struct->af_enabled = af_enabled;
        break;
      case PING_FLAG:
        err_struct->data_sb[err_struct->count] = good;
        err_struct->data_is[err_struct->count] = bad;
        err_struct->data_addr[err_struct->count] = address;
        err_struct->data1[err_struct->count] = data1;           /*  tx pkt length  */
        err_struct->data2[err_struct->count] = data2;           /*  rx pkt length  */
        err_struct->af_sent = af_sent;
        err_struct->af_enabled = af_enabled;
        break;
      case NO_FLAG:
        if (err_struct->bad_bits == 0xFFFF)
            return(FALSE);
        else  {
            mask1 = good ^ bad;                         /*  find the bad bits  */
            if ((mask2 = err_struct->bad_bits ^ mask1) != 0)  {
                err_struct->bad_bits = mask2;
                err_struct->data_sb[err_struct->count] = good;
                err_struct->data_is[err_struct->count] = bad;
                err_struct->data_addr[err_struct->count++] = address;
            }
        }
        break;
        
    }  /*  end switch(err_struct->flag)  */
    
    return(TRUE);
    
}  /*  end store_error()  */

/*
 *  show_auto displays the results of running autotest on all the
 *  controllers.  The results are shown by board.  Only pass/fail 
 *  is indicated.
 */

void show_auto_results()
{
    int i;
    mcitype *mci;
    
    printf(RESULT_HDR);
    for (i = 0; i < MAXCTRLLERS; ++i)  {
        mci = mcidata[i];
        if (mci->unit < 0)
            continue;
        else  {
            printf("\n%-2d  ", mci->unit);
            printf(" %c  ", diag_results[i]->bus_test.stat);
            printf("  %c  ", diag_results[i]->mema_test.stat);
            printf("  %c  ", diag_results[i]->memd_test.stat);
            printf("  %c  ", diag_results[i]->xmem_test.stat);
            show_auto_interface_results(mci, i, MCI_HARDWARE);
            printf("\n");
            
        }
        
    }  /*  end for(i)  */
    
}  /*  end show_auto_results()  */

void show_dci_auto_results(cbus)
    mcitype *cbus;
{
    int i;
    
    printf(DCI_RESULT_HDR);
    for (i = 0; i < FCI_MAX_DCI_CTRLRS; ++i)  {
        if (dcidata[i]->ctrlr_unit >= FCI_MAX_DCI_CTRLRS)
            continue;
        else  {
            printf("\n%-2d  ", dcidata[i]->ctrlr_unit);
            printf("  %c  ", diag_results[i]->mema_test.stat);
            printf("  %c  ", diag_results[i]->memd_test.stat);
            show_auto_interface_results(cbus, i, FCI_HARDWARE);
            printf("\n");
            
        }
        
    }  /*  end for(i)  */
    
}  /*  end show_dci_auto_results()  */

/*
 *  show_auto_interface_results displays the results of running autotest
 *  on up to MCI_MAXUNITS interfaces per controller.  Only pass/fail is
 *  indicated.
 */

void show_auto_interface_results(mci, index, type)
    mcitype *mci;
    int     index;
    short   type;
{
    short   max_interfaces = 4, interface_type = 0;
    int i;
    
    switch (type) {
      case MCI_HARDWARE:
      case SCI_HARDWARE:
        max_interfaces = MCI_MAXUNITS;
        break;
      case FCI_HARDWARE:
        max_interfaces = FCI_MAX_CTRLR_INTERFACES;
        break;
    }
    for (i = 0; i < max_interfaces; ++i)  {
        switch (type) {
          case MCI_HARDWARE:
          case SCI_HARDWARE:
            interface_type = mci->interfacetype[i];
            break;
          case FCI_HARDWARE:
            interface_type = mci->interfacetype[(index * 6) + i];
            break;
        }
        switch (interface_type)  {
          case MCI_TYPE_ETHER:
          case FCI_TYPE_FDDI:
            if (diag_results[index]->add_flt_mem[i].stat == STAT_FAIL ||
                diag_results[index]->add_flt[i].stat == STAT_FAIL ||
                diag_results[index]->ether_ping_test[i].stat == STAT_FAIL)
                printf("%c ", STAT_FAIL);
            else if (diag_results[index]->add_flt_mem[i].stat == STAT_NOT_DONE ||
                     diag_results[index]->add_flt[i].stat == STAT_NOT_DONE ||
                     diag_results[index]->ether_ping_test[i].stat == STAT_NOT_DONE)
                printf("%c ", STAT_NOT_DONE);
            else if (diag_results[index]->add_flt_mem[i].stat == STAT_NO_PORT ||
                     diag_results[index]->add_flt[i].stat == STAT_NO_PORT ||
                     diag_results[index]->ether_ping_test[i].stat == STAT_NO_PORT)
                printf("%c ", STAT_NO_PORT);
            else
                printf("%c ", STAT_PASS);
            break;
          case MCI_TYPE_SERIAL:
          case FCI_TYPE_ULTRA:
          case FCI_TYPE_DS3:
            if (diag_results[index]->serial_ping_test[i].stat == STAT_FAIL)
                printf("%c ", STAT_FAIL);
            else if (diag_results[index]->serial_ping_test[i].stat == STAT_NOT_DONE)
                printf("%c ",STAT_NOT_DONE);
            else if (diag_results[index]->serial_ping_test[i].stat == STAT_NO_PORT)
                printf("%c ", STAT_NO_PORT);
            else
                printf("%c ", STAT_PASS);
            break;
          default:
            printf("%c ", STAT_NO_PORT);
            break;
            
        }  /*  end switch(c)  */
        
    }  /*  end for(i)  */
    
}  /*  end show_auto_interface_results()  */

/*
 *  get_number
 *  prompts user to enter requested data.  checks number entered
 *  against max and min values passed to it.
 */

void get_number(data, msg, min, max)
    ushort *data, min, max;
    char *msg;
{
    char buff[BUFLEN];
    boolean parsed = FALSE;
    
    while(!parsed) {
        printf("%s", msg);
        gets(buff);
        if ((*data = parse_unsigned(buff)) != -1) {
            if (*data < min || *data > max) {
                printf("\nEntry out of range!");
                printf("\nEntry must be >= 0x%x or <= 0x%x", min, max);
                continue;
            }
            else
                parsed = TRUE;
        }
        
    }  /*  end while(!parsed)  */
    
    
}  /*  end get_number()  */

/*
 *  show_all_errors
 *  displays failure data for all diagnostic tests that have been run
 *  for the mci passed to it.
 */

void show_all_errors(mci, type, board)
    mcitype *mci;
    short   type, board;
{
    int i, port, linect = 0, max_interfaces = 0;
    MCI_TEST *err_struct;
    short   interface_type = 0;
    
    /*
     *  print the results of the basic tests first
     */

    switch (type) {
      case MCI_HARDWARE:
      case SCI_HARDWARE:
        board = mci->unit;
        printf("\n********  MCI %d diagnostic results ********", board);
        max_interfaces = MCI_MAXUNITS;
        break;
      case FCI_HARDWARE:
        max_interfaces = FCI_MAX_CTRLR_INTERFACES;
        printf("\n********  FCI %d diagnostic results ********",
            mci->dci[board]->ctrlr_unit);
        break;
    }
    ++linect;
    err_struct = diag_results[board];
    if (board >= 0)
        {
            if (err_struct->bus_test.stat == STAT_FAIL)
                print_errors(&err_struct->bus_test, "BUS TEST", &linect);
            else if (err_struct->bus_test.stat == STAT_NOT_DONE) {
                printf("\n                                     BUS TEST not done");
                ++linect;
            }
            else {
                printf("\n                                     BUS TEST passed");
                ++linect;
            }
            
            if (err_struct->mema_test.stat == STAT_FAIL)
                print_errors(&err_struct->mema_test, "MEMA TEST", &linect);
            else if (err_struct->mema_test.stat == STAT_NOT_DONE) {
                printf("\n                                    MEMA TEST not done");
                ++linect;
            }
            else {
                printf("\n                                    MEMA TEST passed");
                ++linect;
            }
            
            if (err_struct->memd_test.stat == STAT_FAIL)
                print_errors(&err_struct->memd_test, "MEMD TEST", &linect);
            else if (err_struct->memd_test.stat == STAT_NOT_DONE) {
                printf("\n                                    MEMD TEST not done");
                ++linect;
            }
            else {
                printf("\n                                    MEMD TEST passed");
                ++linect;
            }
            
            if (type == MCI_HARDWARE) {

                if (err_struct->xmem_test.stat == STAT_FAIL)
                    print_errors(&err_struct->xmem_test, "XMEM TEST", &linect);
                else if (err_struct->xmem_test.stat == STAT_NOT_DONE) {
                    printf("\n                                    XMEM TEST not done");
                    ++linect;
                }
                else {
                    printf("\n                                    XMEM TEST passed");
                    ++linect;
                }
            }
            
            /*
             *  print the results of the various port tests
             */
            
            for (i = 0; i < max_interfaces; ++i) {
                printf("\n---Port %d ", i);
                ++linect;
                switch (type) {
                  case MCI_HARDWARE:
                  case SCI_HARDWARE:
                    interface_type = mci->interfacetype[i];
                    break;
                  case FCI_HARDWARE:
                    interface_type = mci->interfacetype[(board * 6) + i];
                    break;
                }
                switch (interface_type) {
                  case MCI_TYPE_ETHER:
                  case FCI_TYPE_FDDI:
                    printf(interface_type ? "is an FDDI Port---" : "is an Ethernet Port---");
                    if (err_struct->add_flt_mem[i].stat == STAT_FAIL)
                        print_errors(&err_struct->add_flt_mem[i], "AF MEM TEST", &linect);
                    else if (err_struct->add_flt_mem[i].stat == STAT_NOT_DONE) {
                        printf("  AF MEM TEST not done");
                        ++linect;
                    }
                    else {
                        printf("  AF MEM TEST passed");
                        ++linect;
                    }
                    
                    if (err_struct->add_flt[i].stat == STAT_FAIL)
                        print_errors(&err_struct->add_flt[i], "AF TEST", &linect);
                    else if (err_struct->add_flt[i].stat == STAT_NOT_DONE) {
                        printf("\n                                      AF TEST not done");
                        ++linect;
                    }
                    else {
                        printf("\n                                      AF TEST passed");
                        ++linect;
                    }
                    
                    if (err_struct->ether_ping_test[i].stat == STAT_FAIL)
                        print_errors(&err_struct->ether_ping_test[i],
                            interface_type ? "FDDI PING TEST" :
                            "ETHER PING TEST", &linect);
                    else if (err_struct->ether_ping_test[i].stat == STAT_NOT_DONE) {
                        printf("\n                              ", interface_type ? "FDDI" : "ETHER", " PING TEST not done");
                        ++linect;
                    }
                    else {
                        printf("\n                              ETHER PING TEST passed");
                        ++linect;
                    }
                    break;
                  case MCI_TYPE_SERIAL:
                    printf("is a Serial Port-");
                    if (err_struct->serial_ping_test[i].stat == STAT_FAIL)
                        print_errors(&err_struct->serial_ping_test[i],
                                "SERIAL PING TEST", &linect);
                    else if (err_struct->serial_ping_test[i].stat == STAT_NOT_DONE) {
                        printf(" SERIAL PING TEST not done");
                        ++linect;
                    }
                    else {
                        printf(" SERIAL PING TEST passed");
                        ++linect;
                    }
                    break;
                  case FCI_TYPE_ULTRA:
                  case FCI_TYPE_DS3:
                    printf((interface_type == FCI_TYPE_ULTRA) ? "is an Ultra Port  " : "is a DS3 Port---  ");
                    if (err_struct->serial_ping_test[i].stat == STAT_FAIL)
                        print_errors(&err_struct->serial_ping_test[i],
                                "PING TEST       ", &linect);
                    else if (err_struct->serial_ping_test[i].stat == STAT_NOT_DONE) {
                        printf("        PING TEST not done");
                        ++linect;
                    }
                    else {
                        printf("        PING TEST passed");
                        ++linect;
                    }
                    break;
                  default:
                    printf("is not available---");
                    break;
                    
                }  /*  end switch(mci->interfacetype[i])  */
                
            } /*  end for(i)  */
        }
    else
        printf("\n\n\tBoard not responding...\n\tLoop Bus Test to diagnose.\n");
}  /*  end show_all_errors()  */

/*  end mci_err.c  */

