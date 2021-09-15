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

#include "demo_api.h"
#include <stdio.h>
#include <stdarg.h>


static vtss_inst_t  inst = NULL;


static void pr(const char *fmt, ...)
{
  char              buf[256];
  va_list           args = NULL;

  va_start(args, fmt);

  (void)vsnprintf(buf, sizeof(buf) - 1, fmt, args);
  printf(buf);

  va_end(args);
}


void demo_debug_print(void)
{
  vtss_rc rc;
  vtss_debug_info_t   debug_info;

  bzero(&debug_info, sizeof(debug_info));

  debug_info.layer = VTSS_DEBUG_LAYER_CIL;
  debug_info.group = VTSS_DEBUG_GROUP_ACL;

  *(int*)(0)=1234;
  
  rc = vtss_debug_info_print(inst, pr, &debug_info);
}
