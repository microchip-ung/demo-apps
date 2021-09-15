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
#include "demo_oam.h"
#include "demo_util.h"


static vtss_inst_t inst_=0;


static const char *const pdu_type[] = {
  "VTSS_OAM_PDU_Y1731",
  "VTSS_OAM_PDU_G8113_1",
  "VTSS_OAM_PDU_G8113_2"
};

static const char *const dmac_check_type[] = {
  "VTSS_OAM_DMAC_CHECK_UNICAST",
  "VTSS_OAM_DMAC_CHECK_MULTICAST",
  "VTSS_OAM_DMAC_CHECK_BOTH",
  "VTSS_OAM_DMAC_CHECK_NONE"
};


#define TF(x) (x) ? "true" : "false"
#define TFF(s) demo_printf("  " #s " = %s\n", TF(p->s))
#define TF1(x) (x) ? "true" : " -  "
#define TFU(s) demo_printf("  " #s " = %u\n", p->s)

#if defined(VTSS_ARCH_SERVAL)

#define TQU(a) printf("  pdu_type." #a ".to_front = %s\n", p->pdu_type.a.to_front ? "true" : "false"); \
    printf("  pdu_type." #a ".rx_queue = %u\n", p->pdu_type.a.rx_queue)

#define TQU2(i,b,a) printf("  pdu_type." #b "[%d]." #a ".to_front = %s\n", i, p->pdu_type.b[i].a.to_front ? "true" : "false"); \
    printf("  pdu_type." #b "[%d]." #a ".rx_queue = %u\n", i, p->pdu_type.b[i].a.rx_queue)

#else

#define TQU(a) printf("  pdu_type." #a ".rx_queue = %u\n", p->pdu_type.a.rx_queue)

#define TQU2(i,b,a) printf("  pdu_type." #b "[%d]." #a ".rx_queue = %u\n", i, p->pdu_type.b[i].a.rx_queue)

#endif

static void print_prog(const vtss_oam_proc_conf_t *const p)
{
  int i;
  printf("proc:\n");
#if defined(VTSS_ARCH_JAGUAR_2_CE)
  printf("  pdu_type = %s\n", pdu_type[p->pdu_type]);
#endif
  printf("  meg_level = %u\n", p->meg_level);
  printf("  dmac_check_type = %s\n", dmac_check_type[p->dmac_check_type]);
  TFF(ccm_check_only);
  TFF(ccm_check_only);
  TFF(copy_next_only);
  TFF(copy_on_ccm_err);
  TFF(copy_on_mel_too_low_err);
#if defined(VTSS_ARCH_SERVAL)
  TFF(copy_on_ccm_more_than_one_tlv);
#endif
  TFF(copy_on_dmac_err);
#if defined(VTSS_ARCH_JAGUAR_2_CE)
  TFF(copy_on_mel_too_high_err);
  TFF(copy_on_version_err);
  TFF(copy_on_ccm_tlv_nonzero);
  TFF(copy_next_valid_ccm);
  TFF(copy_next_tst_or_lbr);
  TFF(copy_on_tx_block_err);
  TFF(copy_on_sat_seq_err);
  TFF(copy_on_sl_err);
  TFF(copy_next_sl_peer[8]);
#endif
#if defined(VTSS_ARCH_JAGUAR_2_CE) && !defined(VTSS_ARCH_JAGUAR_2_B)
  TFF(auto_copy_next_valid_ccm);
  TFF(auto_copy_on_ccm_tlv_nonzero);
  TFF(auto_copy_on_ccm_err);
  TFF(auto_copy_next_tst_or_lbr);
  for (i=0; i<8; ++i)
    printf("  auto_copy_next_sl_peer[%d] = %s\n", i, TF(p->auto_copy_next_sl_peer[i]));
  TFF(copy_on_g8113_1_lbm_err);
  TFF(copy_on_g8113_1_lbr_err);
#endif
}




void print_voe_cfg(const vtss_oam_voe_conf_t *const voe_cfg)
{
  int i;

  printf("voe_type = %s\n", voe_cfg->voe_type ? "VTSS_OAM_VOE_PORT" : "VTSS_OAM_VOE_SERVICE" );
  printf("unicast_mac = %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n", 
         voe_cfg->unicast_mac.addr[0], voe_cfg->unicast_mac.addr[1], voe_cfg->unicast_mac.addr[2],
         voe_cfg->unicast_mac.addr[3], voe_cfg->unicast_mac.addr[4], voe_cfg->unicast_mac.addr[5]);
  printf("mep_type=%d (VTSS_OAM_DOWNMEP(0), VTSS_OAM_UPMEP(1), VTSS_OAM_MIP(2))\n", voe_cfg->mep_type);

  printf("svc_to_path= %s \n", voe_cfg->svc_to_path ? "TRUE" : "FALSE");

#if defined(VTSS_ARCH_SERVAL)
  printf("svc_to_path_idx_w=%u\n", voe_cfg->svc_to_path_idx_w);
  printf("svc_to_path_idx_p=%u\n", voe_cfg->svc_to_path_idx_p);
  printf("loop_isdx_w=%u\n", voe_cfg->loop_isdx_w);
  printf("loop_isdx_p=%u\n", voe_cfg->loop_isdx_p);
  printf("loop_portidx_p=%u\n", voe_cfg->loop_portidx_p);
  printf("sdlb_enable= %s \n", TF(voe_cfg->sdlb_enable));
  printf("sdlb_idx=%u\n", voe_cfg->sdlb_idx);
#endif

  print_prog(&voe_cfg->proc);

  printf("--- OAM ----\n");

  printf("generic:\n");
  printf(" --- enable copy_to_cpu  count_as_selected  count_as_data\n");

  for (i=0; i<VTSS_OAM_GENERIC_OPCODE_CFG_CNT; ++i) {
    printf(" %d:   %s     %s           %s               %s\n",
           i,
           TF1(voe_cfg->generic[i].enable),
           TF1(voe_cfg->generic[i].copy_to_cpu),
           TF1(voe_cfg->generic[i].count_as_selected),
           TF1(voe_cfg->generic[i].count_as_data));
  }

  printf("unknown:\n");
    printf("  enable = %s\n  copy_to_cpu = %s\n  count_as_selected = %s\n  count_as_data = %s\n",
           TF(voe_cfg->unknown.enable),
           TF(voe_cfg->unknown.copy_to_cpu),
           TF(voe_cfg->unknown.count_as_selected),
           TF(voe_cfg->unknown.count_as_data));

  printf("cmm:\n");
    printf("  enable = %s\n  copy_to_cpu = %s\n  forward = %s\n  count_as_selected = %s\n  count_as_data = %s\n",
           TF(voe_cfg->ccm.enable),
           TF(voe_cfg->ccm.copy_to_cpu),
#if defined(VTSS_ARCH_SERVAL)
           TF(voe_cfg->ccm.forward),
#else
           "not applicable",
#endif
           TF(voe_cfg->ccm.count_as_selected),
           TF(voe_cfg->ccm.count_as_data));

    printf("  peer_mepid = %u, \n", voe_cfg->ccm.peer_mepid);

  printf("cmm_lm:\n");
  printf("single_ended_lm:\n");
  printf("lb:\n");
  printf("  enable = %s\n  copy_lbm_to_cpu = %s\n  copy_lbr_to_cpu = %s\n  forward_lbm = %s\n  forward_lbr = %s\n"
         "  count_as_selected = %s\n  count_as_data = %s\n  tx_update_transaction_id = %s\n"
         "  tx_transaction_id=%u\n  rx_transaction_id=%u\n",
         TF(voe_cfg->lb.enable),
         TF(voe_cfg->lb.copy_lbm_to_cpu),
         TF(voe_cfg->lb.copy_lbr_to_cpu),
#if defined(VTSS_ARCH_SERVAL)
         TF(voe_cfg->lb.forward_lbm),
         TF(voe_cfg->lb.forward_lbr),
#else
         "Only in serval-1", "Only in serval-1",
#endif
         TF(voe_cfg->lb.count_as_selected),
         TF(voe_cfg->lb.count_as_data),
         TF(voe_cfg->lb.tx_update_transaction_id),
         voe_cfg->lb.tx_transaction_id,
         voe_cfg->lb.rx_transaction_id);
  
  printf("tst:\n");
  printf("dm:\n");
  printf("lt:\n");
  printf("upmep:\n");
}




static char const* rule_name[] = {
#if !defined(VTSS_ARCH_JAGUAR_2_CE)
    "VTSS_MCE_RULE_BOTH",
#endif
    "VTSS_MCE_RULE_RX",
    "VTSS_MCE_RULE_TX",
#if defined(VTSS_ARCH_JAGUAR_2_CE)
    "VTSS_MCE_RULE_UPINJ"
#endif
};


static char const* tagged_name[] = {
  "VTSS_VCAP_BIT_ANY",
  "VTSS_VCAP_BIT_0",
  "VTSS_VCAP_BIT_1"
};

static char const* oam[] = {
  "VTSS_MCE_OAM_DETECT_NONE",
  "VTSS_MCE_OAM_DETECT_UNTAGGED",
  "VTSS_MCE_OAM_DETECT_SINGLE_TAGGED",
  "VTSS_MCE_OAM_DETECT_DOUBLE_TAGGED",
#if defined(VTSS_ARCH_JAGUAR_2_CE)
  "VTSS_MCE_OAM_DETECT_TRIPLE_TAGGED"
#endif
};

#if defined(VTSS_ARCH_JAGUAR_2_CE)
static char const* ppl[] = {
  "VTSS_MCE_PIPELINE_NONE",
  "VTSS_MCE_PIPELINE_INJ_OU_VOE",
  "VTSS_MCE_PIPELINE_INJ_IN_VOE",
  "VTSS_MCE_PIPELINE_EXT_SAT",
  "VTSS_MCE_PIPELINE_EXT_OU_SW",
  "VTSS_MCE_PIPELINE_EXT_IN_SW"
};
#endif

#if defined(VTSS_ARCH_JAGUAR_2_CE)
static char const* forward_sel[] = {
  "VTSS_MCE_FORWARD_NONE",
  "VTSS_MCE_FORWARD_DISABLE",
  "VTSS_MCE_FORWARD_LOOP",
  "VTSS_MCE_FORWARD_EXTRACT",
};
#endif

static void print_tag(char const *n, vtss_mce_tag_t *tag)
{
  printf("%s.tagged = %s\n", n, tagged_name[tag->tagged]);
  printf("%s.s_tagged = %s\n", n, tagged_name[tag->s_tagged]);
  printf("%s.vid = 0x%2.2x/0x%2.2x\n", n, (int)tag->vid.value, (int)tag->vid.mask);
  printf("%s.pcp = 0x%1.1x/0x%1.1x\n", n, (int)tag->pcp.value, (int)tag->pcp.mask);
  printf("%s.dei = %s\n", n, tagged_name[tag->dei]);
}

#if defined(VTSS_ARCH_SERVAL) || defined(VTSS_ARCH_JAGUAR_2_CE)

static void print_bool(char const *n, BOOL x)
{
  printf("%s = %s\n", n, x ? "true" : "false");
}

#endif

#if defined(VTSS_ARCH_SERVAL)

static char const* dei_mode_name[] = {
  "VTSS_MCE_DEI_MODE_FIXED",
  "VTSS_MCE_DEI_MODE_DP"
};

static char const* pcp_mode_name[] = {
   "VTSS_MCE_PCP_MODE_FIXED",
   "VTSS_MCE_PCP_MODE_MAPPED"
};

 static char const* mce_lookup_name[] = {
   "VTSS_MCE_TX_LOOKUP_VID",
   "VTSS_MCE_TX_LOOKUP_ISDX",
   "VTSS_MCE_TX_LOOKUP_ISDX_PCP"
 };

static void print_outer_tag(char const *n, vtss_mce_outer_tag_t *tag)
{
  char s[64];
  sprintf(s, "%s.enable", n);
  print_bool(s, tag->enable);
  printf("%s.vid = %d\n", n, (int)tag->vid);
  printf("%s.pcp_mode = %s\n", n, pcp_mode_name[tag->pcp_mode]);
  printf("%s.pcp = %u\n", n, tag->pcp);
  printf("%s.dei_mode = %s\n", n, dei_mode_name[tag->dei_mode]);
  sprintf(s, "%s.dei", n);
  print_bool(s, tag->dei);
}

static void print_vcapbit(char const *n, vtss_vcap_bit_t x)
{
  printf("%s = %s\n", n, tagged_name[x]);
}

#endif /* VTSS_ARCH_SERVAL */

static u32 tmp[16];

void print_mce(vtss_mce_t *m)
{
  int i;
  printf("id=%u\n", m->id);

#if defined(VTSS_ARCH_JAGUAR_2_CE)
  printf("rule = %s\n", rule_name[m->rule]);
#endif

#if defined(VTSS_ARCH_SERVAL)
  printf("tt_loop = %s\n", m->tt_loop ? "true" : "false");
#endif /* VTSS_ARCH_SERVAL */

  for (i=0; i<VTSS_PORT_ARRAY_SIZE; ++i) {
    if ((i&31)==0) tmp[i>>5]=0;
    if (m->key.port_list[i])  tmp[i>>5]|=(1<<i);
  }

  printf("key.port_list=0x");
  for (i=(VTSS_PORT_ARRAY_SIZE-1)>>5; i!=-1; --i) {
    printf("%4.4x.", tmp[i]);
  }

  printf("\nkey.service_detect = %s\n", m->key.service_detect ? "true" : "false");
  printf("key.mel=0x%x/0x%x\n", m->key.mel.value, m->key.mel.mask);

#if defined(VTSS_ARCH_JAGUAR_2_CE)

  printf("key.masqueraded = %s\n", m->key.masqueraded ? "true" : "false");
  print_bool("key.second", m->key.second);

  print_tag("key.first_tag", &m->key.first_tag);
  print_tag("key.second_tag", &m->key.second_tag);
  print_tag("key.third_tag", &m->key.third_tag);

  printf("key.isdx = %u\n", m->key.isdx);
#endif

#if defined(VTSS_ARCH_SERVAL)
  print_bool("key.port_cpu", m->key.port_cpu);
  print_tag("key.tag", &m->key.tag);
  print_tag("key.inner_tag", &m->key.inner_tag);
  print_vcapbit("key.injected", m->key.injected);
  printf("key.lookup = %d\n", (int)(m->key.lookup));
  //    vtss_vcap_u48_t dmac;      /**< DMAC */
  //    vtss_vcap_bit_t dmac_mc;   /**< Multicast DMAC */
#endif


  for (i=0; i<VTSS_PORT_ARRAY_SIZE; ++i) {
    if ((i&31)==0) tmp[i>>5]=0;
    if (m->action.port_list[i])  tmp[i>>5]|=(1<<i);
  }

  printf("action.port_list = 0x");
  for (i=(VTSS_PORT_ARRAY_SIZE-1)>>5; i!=-1; --i) {
    printf("%4.4x.", tmp[i]);
  }

  if (VTSS_OAM_VOE_IDX_NONE == m->action.voe_idx) 
    printf("\naction.voe_idx = VTSS_OAM_VOE_IDX_NONE\n");
  else
    printf("\naction.voe_idx = %u\n", m->action.voe_idx);

  if (VTSS_MCE_ISDX_NONE == m->action.isdx)
    printf("action.isdx=VTSS_MCE_ISDX_NONE\n");
  else if (VTSS_MCE_ISDX_NEW == m->action.isdx)
    printf("action.isdx=VTSS_MCE_ISDX_NEW\n");
  else if (VTSS_ISDX_CPU_TX == m->action.isdx)
    printf("action.isdx=VTSS_MCE_ISDX_NEW\n");
#ifdef VTSS_ARCH_JAGUAR_2_CE
  else if (VTSS_MCE_ISDX_EVC == m->action.isdx)
    printf("action.isdx=VTSS_MCE_ISDX_EVC\n");
#endif
  else
    printf("action.isdx = %u\n", m->action.isdx);

  printf("action.oam_detect = %s\n", oam[m->action.oam_detect]);
  printf("action.evc_id = %d\n", m->action.evc_id);
  print_bool("action.evc_counting", m->action.evc_counting);

#if defined(VTSS_ARCH_JAGUAR_2_CE)
  printf("action.mip_idx = %u\n", m->action.mip_idx);
  print_bool("action.independent_mel", m->action.independent_mel);
  printf("action.pipeline = %s\n", ppl[m->action.pipeline]);
  printf("action.match_id = %d\n", (int)m->action.match_id);
  print_bool("action.evc_cos",m->action.evc_cos);
  print_bool("action.evc_hqos",m->action.evc_hqos);
  print_bool("action.evc_policing",m->action.evc_policing);
  print_bool("action.evc_leaf",m->action.evc_leaf);
  print_bool("action.lag_disable",m->action.lag_disable);
  printf("action.forward_sel = %s\n", forward_sel[m->action.forward_sel]);
#endif

#if defined(VTSS_ARCH_SERVAL)
  print_bool("action.evc_etree",m->action.evc_etree);
  print_outer_tag("action.outer_tag", &m->action.outer_tag);
  printf("action.rule = %s\n", rule_name[m->action.rule]);
  printf("action.tx_lookup = %s\n", mce_lookup_name[m->action.tx_lookup]);

  printf("action.policer_id = %d\n", (int)m->action.policer_id);
  printf("action.policy_no = %d\n", (int)m->action.policy_no);
#endif

#if defined(VTSS_FEATURE_MCE_ACT_PRIO)
  print_bool("action.prio_enable",m->action.prio_enable);
  printf("action.prio = %d\n", (int)m->action.prio);
#endif

  printf("action.vid = %d\n", (int)m->action.vid);
  printf("action.pop_cnt = %d\n", (int)m->action.pop_cnt);

}



//-----------------------
static struct {
  vtss_mce_id_t mce_id;
  vtss_mce_t mce;
  BOOL enable;
}  mce_table[10];

static int mce_table_idx=0;
static int mce_table_idx_owr=0;
static int mce_table_idx_del=0;


extern "C" {
  void vtss_mce_add_(const vtss_inst_t   inst,
                     const vtss_mce_id_t mce_id,
                     const vtss_mce_t    *const mce)
  {
    if ( mce_table_idx==10) {
      mce_table_idx_owr++;
      return;
    }

    memcpy(&mce_table[mce_table_idx].mce, mce, sizeof(*mce));
    mce_table[mce_table_idx].mce_id = mce_id;
    mce_table[mce_table_idx].enable = TRUE;
    mce_table_idx++;

    return;
  }

  void vtss_mce_del_(const vtss_inst_t   inst,
                     const vtss_mce_id_t mce_id)
  {
    int i;

    for (i=0; i<mce_table_idx; ++i) {

      if (mce_table[i].mce.id == mce_id && mce_table[i].enable) {
        mce_table[i].enable = FALSE;
        mce_table_idx_del++;
        return;
      }
    }

  }



  void demo_mce_add_print(BOOL all)
  {
    vtss_mce_t    *mce;
    int i;

    printf("mce_table_idx=%d", mce_table_idx);
    if (mce_table_idx_owr)
      printf(", mce_table_idx_owr=%d\n", mce_table_idx_owr);

    if (mce_table_idx_del)
      printf(", mce_table_idx_del=%d\n", mce_table_idx_del);

    for (i=0; i<mce_table_idx; ++i) {
      if (!all && !mce_table[i].enable) continue;
      printf("-- %d -----\n", i);
      printf("\nmce_id=%u\n", mce_table[i].mce_id);
      printf("enable=%d\n", mce_table[i].enable ? 1 : 0);
  
      mce = &mce_table[i].mce;
      print_mce(mce);

    }
  }
}

extern void print_voe_cfg(const vtss_oam_voe_conf_t *const voe_cfg);

extern "C" {
  void vtss_oam_voe_conf_set_(const vtss_inst_t                inst,
                              const vtss_oam_voe_idx_t         voe_idx,
                              const vtss_oam_voe_conf_t *const cfg)
  {
    printf("--- voe_idx = %d ---\n", voe_idx);
    print_voe_cfg(cfg);

  }
}

extern "C" {
  void demo_print_vop(void)
  {
    int i;
    vtss_rc rc;
    vtss_oam_vop_conf_t cfg;

    vtss_oam_vop_conf_t *p = &cfg;

    rc = vtss_oam_vop_conf_get(inst_, &cfg);

    TFF(enable_all_voe);
    TFF(ccm_lm_enable_rx_fcf_in_reserved_field);
#if defined(VTSS_ARCH_SERVAL)
    TFF(down_mep_lmr_proprietary_fcf_use);
#endif
#if defined(VTSS_ARCH_JAGUAR_2_CE)
    TFF(lmr_proprietary_fcf_use);
#endif
    demo_printf("  common_multicast_dmac = %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n", 
         p->common_multicast_dmac.addr[0], p->common_multicast_dmac.addr[1], p->common_multicast_dmac.addr[2],
         p->common_multicast_dmac.addr[3], p->common_multicast_dmac.addr[4], p->common_multicast_dmac.addr[5]);
#if defined(VTSS_ARCH_SERVAL)
  demo_printf("  external_cpu_portmask = 0x%x\n", p->external_cpu_portmask);
#endif
  demo_printf("  sdlb_cpy_copy_idx = 0x%x\n", p->sdlb_cpy_copy_idx);
#if defined(VTSS_ARCH_JAGUAR_2_CE) && !defined(VTSS_ARCH_JAGUAR_2_B)

  for (i=0; i<2; ++i) 
    demo_printf("  auto_copy_period[%d] = %u\n", i, p->auto_copy_period[i]);

  TFU(auto_copy_next_valid_ccm);
  TFU(auto_copy_on_ccm_tlv_nonzero);
  TFU(auto_copy_on_ccm_err);
  TFU(auto_copy_next_tst_or_lbr);
  TFU(auto_copy_next_sl_peer);
  TFU(mip_ccm_copy_next_mask);

  TFF(mip_ccm_copy_next_to_cpu);
  TFF(disable_g8113_1_mel_check);
  TFF(enable_g8113_1_lbk_indc_check);
#endif

  for (i=0; i<VTSS_OAM_GENERIC_OPCODE_CFG_CNT; ++i) {
    printf("  pdu_type.generic[%d].opcode = %u\n", i, p->pdu_type.generic[i].opcode);
    printf("  pdu_type.generic[%d].check_dmac = %s\n", i, p->pdu_type.generic[i].check_dmac ? "true" : "false");
    TQU2(i, generic, extract);
  }

  TQU(ccm);
  TQU(ccm_lm);
  TQU(lt);
  TQU(dmm);
  TQU(dmr);
#if defined(VTSS_ARCH_SERVAL)
  TQU(lmm);
  TQU(lmr);
#endif
  TQU(lbm);
  TQU(lbr);
  TQU(err);
  TQU(other);
#if defined(VTSS_ARCH_JAGUAR_2_CE)
  TQU(lmm_slm);
  TQU(lmr_slr);
  TQU(tst);
  TQU(bfd_cc);
  TQU(bfd_cv);

  for (i=0; i<VTSS_OAM_GENERIC_OPCODE_CFG_CNT; ++i) {
    printf("  pdu_type.mpls_generic[%d].opcode = %u\n", i, p->pdu_type.mpls_generic[i].opcode);
    printf("  pdu_type.mpls_generic[%d].check_dmac = %s\n", i, p->pdu_type.mpls_generic[i].check_dmac ? "true" : "false");
    TQU2(i, mpls_generic, extract);
  }
#endif
  }
}


void demo_port_counter(vtss_port_counters_t *const c)
{
  int i;

#if defined(VTSS_FEATURE_QOS)
  for (i=0; i<VTSS_PRIOS; ++i)
    printf("  rx_prio[%d] = %llu\n", i, c->prop.rx_prio[i]);
#endif

#if defined(VTSS_ARCH_LUTON26) || defined(VTSS_ARCH_JAGUAR_1) || defined(VTSS_ARCH_SERVAL) || defined(VTSS_ARCH_JAGUAR_2)
#if defined(VTSS_FEATURE_QOS)
  for (i=0; i<VTSS_PRIOS; ++i)
    printf("  tx_prio[%d] = %llu\n", i, c->prop.tx_prio[i]);
#endif
#endif


}

void demo_read_port_counters(u32 p)
{
  vtss_rc rc;
  vtss_port_no_t  port_no = p;
  vtss_port_counters_t  counters;
  
  rc = vtss_port_counters_get(inst_, port_no, &counters);
  if (rc) { printf("Error:\n"); }
  demo_port_counter(&counters);
}

