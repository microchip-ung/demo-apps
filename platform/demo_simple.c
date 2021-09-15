/*

  Vitesse Switch API software.

 Copyright (c) 2002-2013 Vitesse Semiconductor Corporation "Vitesse". All
 Rights Reserved.

 Unpublished rights reserved under the copyright laws of the United States of
 America, other countries and international treaties. Permission to use, copy,
 store and modify, the software and its source code is granted. Permission to
 integrate into other products, disclose, transmit and distribute the software
 in an absolute machine readable format (e.g. HEX file) is also granted.  The
 source code of the software may not be disclosed, transmitted or distributed
 without the written permission of Vitesse. The software and its source code
 may only be used in products utilizing the Vitesse switch products.

 This copyright notice must appear in any copy, modification, disclosure,
 transmission or distribution of the software. Vitesse retains all ownership,
 copyright, trade secret and proprietary rights in the software.

 THIS SOFTWARE HAS BEEN PROVIDED "AS IS," WITHOUT EXPRESS OR IMPLIED WARRANTY
 INCLUDING, WITHOUT LIMITATION, IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 FOR A PARTICULAR USE AND NON-INFRINGEMENT.

*/

//#include <vtss_types.h>
#include <vtss/appl/types.h>
#include <misc_api.h>
#include <conf_api.h>


#ifdef VTSS_SW_OPTION_ICFG
#include <icfg_api.h>
#endif

#include "demo_api.h"
#include "demo.h"
#include "demo_trace.h"


/*
 *  We'll call this software option for: VTSS_SW_OPTION_DEMO
 *  Changes to make, in order to include this option:
 *
 *  1): In vtss_appl/main/main.cxx search for VTSS_SW_OPTION_OPTIONAL_MODULES
 *      and add the demo module in a similar fashion (two places).
 *
 *  2):
 *
 */
mesa_rc demo_init(vtss_init_data_t *data)
{

    switch (data->cmd) {

    case INIT_CMD_INIT:
        printf("DEMO: INIT_CMD_INIT\n");
        // Do not put a printf here since the scheduler is not activated
        break;

    case INIT_CMD_START:
        printf("DEMO: INIT_CMD_START\n");
        break;

    case INIT_CMD_CONF_DEF:
        printf("DEMO: INIT_CMD_CONF_DEF\n");
        break;

    case INIT_CMD_MASTER_UP:
        printf("DEMO: INIT_CMD_MASTER_UP\n");
        break;

    case INIT_CMD_MASTER_DOWN:
        printf("DEMO: INIT_CMD_DOWN\n");
        break;

    case INIT_CMD_SWITCH_ADD:
        printf("DEMO: INIT_CMD_ADD\n");
        break;

    case INIT_CMD_SWITCH_DEL:
        printf("DEMO: INIT_CMD_DEL\n");
        break;

    case INIT_CMD_SUSPEND_RESUME:
        printf("DEMO: INIT_CMD_SUSPEND_RESUME\n");
        break;

    case INIT_CMD_WARMSTART_QUERY:
        printf("DEMO: INIT_CMD_WARMSTART_QUERY\n");
        break;

    default:
      printf("DEMO: Unknow command %d\n", data->cmd);
        break;
    }

    return VTSS_RC_OK;
}


const char *demo_error_txt(vtss_rc error)
{
  switch (error) {
  case DEMO_ERROR_GEN: return "DEMO_ERROR_GEN";
  case DEMO_ERROR_SOMETHING: return "DEMO_ERROR_SOMETHING";
  case DEMO_ERROR_SOMETHING_ELSE: return "DEMO_ERROR_SOMETHING_ELSE";
  }

  return "DEMO: Unknown error code";
}
