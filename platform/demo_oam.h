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
#ifndef _DEMO_OAM_H_
#define _DEMO_OAM_H_

#include <vtss_api.h>
#include <icli_api.h>


#if defined(VTSS_FEATURE_OAM)

#define DEMO_OAM_UNI_PORT 0
#define DEMO_OAM_NNI_PORT 1
#define DEMO_OAM_MEG_LEVEL 3

void demo_voe_lb_cfg(vtss_oam_voe_lb_conf_t*,BOOL);
void demo_voe_generic_cfg(vtss_oam_voe_generic_conf_t* p);
void demo_voe_unknown_cfg(vtss_oam_voe_unknown_conf_t*);
void demo_voe_ccm_cfg(vtss_oam_voe_ccm_conf_t*, BOOL);
void demo_voe_lm_cfg(vtss_oam_voe_ccm_lm_conf_t* lm, vtss_oam_voe_lm_counter_conf_t* lm_count, BOOL);
void demo_voe_single_ended_lm(vtss_oam_voe_se_lm_conf_t*, BOOL);
void demo_voe_tst_cfg(vtss_oam_voe_tst_conf_t*, BOOL);
void demo_voe_dm_cfg(vtss_oam_voe_dm_conf_t*, BOOL, BOOL);
void demo_voe_lt_cfg(vtss_oam_voe_lt_conf_t*, BOOL);

void demo_voe_proc_cfg(vtss_oam_proc_conf_t*, u8 mel);
void demo_voe_common_cfg(vtss_oam_voe_conf_t *p, vtss_oam_mep_type_t updown);

void demo_voe_count_cfg(vtss_oam_voe_lm_counter_conf_t *p);


#ifdef VTSS_ARCH_SERVAL
void demo_voe_upmep_cfg(vtss_oam_voe_up_mep_conf_t *p);
#endif

struct frame_list {
  vtss_packet_oam_type_t type;
  const char*            name;
  u32                    pdu_offset;
  u8*                    frame;
  size_t                 size;
};

extern struct frame_list frame_list[2][13];

#endif
#endif
