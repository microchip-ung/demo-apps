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
#include "demo_oam.h"

void demo_voe_lb_cfg(vtss_oam_voe_lb_conf_t *lb)
{
  lb->enable = TRUE;

  lb->copy_lbm_to_cpu = FALSE;
  lb->copy_lbr_to_cpu = FALSE;

  lb->forward_lbm = FALSE;
  lb->forward_lbr = FALSE;

  lb->count_as_selected  = FALSE;
  lb->count_as_data      = FALSE;

  lb->tx_update_transaction_id  = FALSE;

  lb->tx_transaction_id  = TRUE;
  lb->rx_transaction_id = 0x11223344;
}

void demo_voe_generic_cfg(vtss_oam_voe_generic_conf_t* p)
{}

void demo_voe_unknown_cfg(vtss_oam_voe_unknown_conf_t* p)
{}

void demo_voe_ccm_cfg(vtss_oam_voe_ccm_conf_t* p)
{}

void demo_voe_lm_cfg(vtss_oam_voe_ccm_lm_conf_t* p)
{}

void demo_voe_single_ended_lm(vtss_oam_voe_se_lm_conf_t* p)
{}

void demo_voe_tst_cfg(vtss_oam_voe_tst_conf_t* p)
{}

void demo_voe_dm_cfg(vtss_oam_voe_dm_conf_t* p)
{}

void demo_voe_lt_cfg(vtss_oam_voe_lt_conf_t *p)
{}


void demo_voe_proc_cfg(vtss_oam_proc_conf_t* p, u8 mel)
{
  p->meg_level = (u32)mel;
  p->dmac_check_type = VTSS_OAM_DMAC_CHECK_UNICAST;
  p->ccm_check_only = TRUE;

  // --- This flag is a modifier of the following 'copy_on_*' so that only the next occurence
  //     that match a certain 'copy_on_*' will go to the CPU
  p->copy_next_only = FALSE; // FALSE: No modification to 'copy_on_*'
                             // TRUE: hit once for each 'copy_on_*'


  p->copy_on_cmm_err               = FALSE; 
  p->copy_on_mel_too_low_err       = FALSE;
  p->copy_on_ccm_more_than_one_tlv = FALSE; // Only if copy_next_only = TRUE in Serval-1
  p->copy_on_dmac_err              = FALSE; // Do not honour copy_next_only in Serval-1
}
