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


static vtss_inst_t  inst = NULL,

void demo_afi(void)
{
  vtss_rc rc;

  vtss_afi_slow_inj_alloc_cfg_t  alloc_cfg;
  vtss_afi_slow_inj_start_cfg_t  start_cfg;

  vtss_afi_slowid_t              afi_id;


  // Set port_no to VTSS_PORT_NO_NONE for UpMEP. Then Virtual Device 1 (VD1)
  // will be used.
  allow_cfg.port_no = 0;
  allow_cfg.prio = 0;


  rc = vtss_afi_slow_inj_alloc(inst, &alloc_cfg, &afi_id);
  if (rc) return rc;


  // Now transmit a frame with ifh.afi_inj = 1.



  // Take the frame just transmitted and configure AFI 'afi_id' with it.
  rc = vtss_afi_slow_inj_frm_hijack(inst, afi_id);
  if (rc) return rc;

  start_cfg.fpm = 60; 
  start_cfg.jitter_mode = 0;

  rc = vtss_afi_slow_inj_start(inst, afi_id, &start_cfg);
  if (rc) return rc;


  rc = vtss_afi_slow_inj_stop(inst, afi_id);
  if (rc) return rc;


  return rc;
}



