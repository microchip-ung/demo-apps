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


/*
 * deb sym write qsys:hsch[216]:cir_cfg 0x41
 * deb sym write qsys:hsch[214]:cir_cfg 0x41
 */

#include "demo_api.h"

mesa_rc demo_shaper(u32 r)
{
  mesa_rc  rc;
  mesa_packet_rx_conf_t  conf;

  rc = mesa_packet_rx_conf_get(NULL, &conf);
  if (rc) return rc;

  // CPU port shaper bitrate in kbps
#ifdef VTSS_FEATURE_QOS_CPU_PORT_SHAPER
  conf.shaper_rate = r;
#endif

  rc = mesa_packet_rx_conf_set(NULL, &conf);
  return rc;
}
