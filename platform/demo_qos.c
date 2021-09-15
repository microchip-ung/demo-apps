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
#include <vtss_api.h>


const vtss_inst_t  inst = NULL;

#if defined(VTSS_FEATURE_QOS_WRED_V2) || defined(VTSS_FEATURE_QOS_WRED_V3)
void demo_qos_dump()
{
  vtss_rc          rc;
  vtss_qos_conf_t  conf;
  int q, d;
  vtss_red_v2_t *p=0;
  
  rc = vtss_qos_conf_get(inst, &conf);
  if (rc) { printf("error=%d", rc); return;}

  for (q=0; q<VTSS_QUEUE_ARRAY_SIZE; ++q) {
    for (d=0; d<2; ++d) {

#ifdef VTSS_FEATURE_QOS_WRED_V2
      p = &conf.red_v2[q][d];
#endif
      
      printf("(%d,%d enable=%d, min_fl=%d, max=%d, max_unit=%d\n", q,d, (int)p->enable, (int)p->min_fl, (int)p->max, (int)p->max_unit);
    }
  }

}

#else
void demo_qos_dump()
{
  printf("demo_qos_dump() not supported since neither VTSS_FEATURE_QOS_WRED_V2 or VTSS_FEATURE_QOS_WRED_V3 is defined\n");
}
#endif
