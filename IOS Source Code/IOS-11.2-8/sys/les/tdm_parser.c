/* $Id: tdm_parser.c,v 3.1.40.2 1996/07/02 23:55:23 jturner Exp $
 * $Source: /release/112/cvs/Xsys/les/tdm_parser.c,v $
 *------------------------------------------------------------------
 * tdm_parser.c -- TDM parser routines
 *
 * February 1996, Joe Turner
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tdm_parser.c,v $
 * Revision 3.1.40.2  1996/07/02  23:55:23  jturner
 * CSCdi61176:  post-brasil-commit code cleanup in tdm, asm, src and
 * makefile
 * Branch: California_branch
 *
 * Revision 3.1.40.1  1996/06/16  21:17:00  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1996/02/02  22:50:17  jturner
 * Placeholder for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "ttysrv.h"
#include "config.h"
#include "interface_private.h"
#include "packet.h"
#include "parser.h"
#include "ieee.h"
#include "if_as5200_tdm.h"
#include "if_les.h"
#include "parser_defs_tdm.h"
#include "tdm_parser.h"
#include "../ui/command.h"

/*
 * void show_tdm_connection_memory(int slot)
 *	- show the slot specific tdm connection information.
 */
static void
show_tdm_connection_memory (int slot)
{
    ch_info_t *tdm;
    leveltype SR;
    int stream, channel;
    tdm_regs tdm_device[TDM_MAX_STREAMS], *tdm_ptr;

    tdm = malloc(sizeof(ch_info_t));
    if(!tdm) {
        return;
    }
    tdm->slot = slot;

    /*
     * Get pointer to physical tdm device registers.
     */
    tdm_ptr = tdm_getregs(tdm);
 
    /*
     * Disable interrupts while we do our dirty work.
     */
    SR = raise_interrupt_level(NETS_DISABLE);
 
    /*
     * Store control reg
     */
    tdm_device[0].control_reg = tdm_ptr->control_reg;
 
    /*
     * Copy out channel connection information for all streams.
     * Note you're trashing the control reg here.
     */
    for (stream = 0; stream < TDM_MAX_STREAMS; stream++) {
        tdm_ptr->control_reg = (TDM_CONNECTION_MEMORY_LOW | stream);
        for (channel = 0; channel < TDM_MAX_CHANNELS; channel++) {
            tdm_device[stream].channel[channel] = tdm_ptr->channel[channel];
        }
    }
    tdm_device[0].ode_reg = tdm_ptr->ode_reg;
 
    /*
     * Restore control register.
     */
    tdm_ptr->control_reg = tdm_device[0].control_reg;
 
    /*
     * Restore interrupt level.
     */
    reset_interrupt_level(SR);
 
    /*
     * Print out info.
     */

    /* do the automore thing */
    automore_enable(NULL);
    if (tdm->slot == TDM_MOTHERBOARD_SLOT) {
        printf("Motherboard MT8980 TDM unit 0, ");
    } else {
        printf("Slot %d MT8980 TDM unit 0, ", tdm->slot);
    }
    printf("Control Register = 0x%02x, ", (tdm_device[0].control_reg & TDM_MASK));
    printf("ODE Register = 0x%02x\n", (tdm_device[0].ode_reg & TDM_MASK));
    for (stream = 0; stream < TDM_MAX_STREAMS; stream++) {
        printf("Connection Memory for ST%d:\n", stream);
        for (channel = 0; channel < TDM_MAX_CHANNELS; channel++) {
            printf("Ch%d:", channel);
	    if (channel<10)
		printf(" ");
            printf(" 0x%02x", (tdm_device[stream].channel[channel] & TDM_MASK));
            /*
             * Print 4 channel's worth of info before a newline.
             */
            if ((channel+1)%4) {
                printf(", ");
            } else {
                printf("\n");
            }
        }
    }
    /* do the automore thing */
    automore_disable();

    /*
     * Exit stage left...
     */
    free(tdm);
    return;
}

/*
 * void show_tdm_data_memory(int slot)
 *	- show the slot specific tdm connection information.
 */
static void
show_tdm_data_memory (int slot)
{
    ch_info_t *tdm;
    leveltype SR;
    int stream, channel;
    tdm_regs tdm_device[TDM_MAX_STREAMS], *tdm_ptr;

    tdm = malloc(sizeof(ch_info_t));
    if(!tdm) {
        return;
    }
    tdm->slot = slot;

    /*
     * Get pointer to physical tdm device registers.
     */
    tdm_ptr = tdm_getregs(tdm);
 
    /*
     * Disable interrupts while we do our dirty work.
     */
    SR = raise_interrupt_level(NETS_DISABLE);
 
    /*
     * Store control reg
     */
    tdm_device[0].control_reg = tdm_ptr->control_reg;
 
    /*
     * Copy out channel connection information for all streams.
     * Note you're trashing the control reg here.
     */
    for (stream = 0; stream < TDM_MAX_STREAMS; stream++) {
        tdm_ptr->control_reg = (TDM_DATA_MEMORY | stream);
        for (channel = 0; channel < TDM_MAX_CHANNELS; channel++) {
            tdm_device[stream].channel[channel] = tdm_ptr->channel[channel];
        }
    }
    tdm_device[0].ode_reg = tdm_ptr->ode_reg;
 
    /*
     * Restore control register.
     */
    tdm_ptr->control_reg = tdm_device[0].control_reg;
 
    /*
     * Restore interrupt level.
     */
    reset_interrupt_level(SR);
 
    /*
     * Print out info.
     */

    /* do the automore thing */
    automore_enable(NULL);
    if (tdm->slot == TDM_MOTHERBOARD_SLOT) {
        printf("Motherboard MT8980 TDM unit 0, ");
    } else {
        printf("Slot %d MT8980 TDM unit 0, ", tdm->slot);
    }
    printf("Control Register = 0x%02x, ", (tdm_device[0].control_reg & TDM_MASK));
    printf("ODE Register = 0x%02x\n", (tdm_device[0].ode_reg & TDM_MASK));
    for (stream = 0; stream < TDM_MAX_STREAMS; stream++) {
        printf("Data Memory for ST%d:\n", stream);
        for (channel = 0; channel < TDM_MAX_CHANNELS; channel++) {
            printf("Ch%d:", channel);
            if (channel<10)
                printf(" ");
            printf(" 0x%02x", (tdm_device[stream].channel[channel] & TDM_MASK));
            /*
             * Print 4 channel's worth of info before a newline.
             */
            if ((channel+1)%4) {
                printf(", ");
            } else {
                printf("\n");
            }
        }
    }
    /* do the automore thing */
    automore_disable();

    /*
     * Exit stage left...
     */
    free(tdm);
    return;

}

/*
 * show_tdm_command()
 *
 * Description:
 * Parse the "show tdm" command.
 *
 * show tdm [<connections>|<data>] [{slot-num|motherboard|cr}]
 *
 * Parameters:
 * csb - pointer to parseinfo
 *
 * OBJ(int,1) = <connections> | <data>
 * OBJ(int,2) = <motherboard> | <slot> | <cr==all>
 * OBJ(int,3) = <slot-num>
 *
 * Returns:
 * None
 */
void show_tdm_command (parseinfo *csb)
{
    int keyword1, keyword2, slot, i;

    keyword1 = GETOBJ(int,1);
    keyword2 = GETOBJ(int,2);
    switch(keyword1) {
	case SHOW_TDM_CONNECTIONS:
	    switch(keyword2) {
		case SHOW_TDM_ALL:
		    show_tdm_connection_memory(TDM_MOTHERBOARD_SLOT);
		    for (i=0; i < BRASIL_MAX_SLOTS; i++) {
			if (!BRASIL_CARD_PRESENT(i)) {
			    continue;
			} else {
			    show_tdm_connection_memory(i);
			}
		    }
		    break;
		case SHOW_TDM_MOTHERBOARD:
		    show_tdm_connection_memory(TDM_MOTHERBOARD_SLOT);
		    break;
		case SHOW_TDM_SLOT:
		    slot = GETOBJ(int,3);
                    if (!BRASIL_CARD_PRESENT(slot)) {
                        printf("\nNo TDM capable device in slot %d!", slot);
                    } else {
		        show_tdm_connection_memory(slot);
                    }
		    break;
		default:
		    bad_parser_subcommand(csb, csb->which);
		    break;
	    }
	    break;
	case SHOW_TDM_DATA:
	    switch(keyword2) {
		case SHOW_TDM_ALL:
		    show_tdm_data_memory(TDM_MOTHERBOARD_SLOT);
                    for (i=0; i < BRASIL_MAX_SLOTS; i++) {
                        if (!BRASIL_CARD_PRESENT(i)) {
                            continue;
                        } else {
                            show_tdm_data_memory(i);
                        }
                    }
		    break;
		case SHOW_TDM_MOTHERBOARD:
		    show_tdm_data_memory(TDM_MOTHERBOARD_SLOT);
		    break;
		case SHOW_TDM_SLOT:
		    slot = GETOBJ(int,3);
		    if (!BRASIL_CARD_PRESENT(slot)) {
			printf("\nNo TDM capable device in slot %d!", slot);
		    } else {
			show_tdm_data_memory(slot);
		    }
		    break;
		default:
		    bad_parser_subcommand(csb, csb->which);
		    break;
	    }
	    break;
	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
    }
}
