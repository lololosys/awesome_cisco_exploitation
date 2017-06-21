/* $Id$
 * $Source$
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ N V R A M . H
 *
 * June, 1997  Edward Cheng
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log$
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
 
 
/******************************************************************
 * debug nvram
 *
 */
 
EOLS    (debug_nvram_eol, debug_command, DEBUG_NVRAM);
KEYWORD_DEBUG(debug_nvram, debug_nvram_eol, ALTERNATE,
              OBJ(pdb,1), os_debug_arr,
              "nvram", "Debug NVRAM behavior", PRIV_OPR);
 
#undef  ALTERNATE
#define ALTERNATE       debug_nvram

