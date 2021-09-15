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

/* #include <cyg/infra/diag.h> */
#include "demo_api.h"
#include "demo_oam.h"
#include "demo_util.h"


#ifdef VTSS_FEATURE_OAM

vtss_mce_id_t demo_allocate_mce_id(void)
{
  static vtss_mce_id_t mce_id=0;
  return ++mce_id;
}


/*
 *  Serval-1 support:
 *
 *  ETH-CC
 *  ETH-LB
 *  ETH-Test
 *  ETH-LM
 *    - Dual-ended; use CCM frames
 *    - Single-ended; use LM frames. NE: CPU inject LMM. FE: Generate LMR. NE: sent LMR to CPU.
 *  DownMEP: Counters: RX (RX_LM)  OAM_MEP:RX_VOE_PM:MEP_RX_FRM_CNT
 *                     TX (TX_LM)  OAM_MEP:TX_VOE_PM:MEP_TX_FRM_CNT
 *  UpMEP:   Counters: RX (RX_LM)  OAM_MEP:TX_VOE_PM:MEP_TX_FRM_CNT
 *                     TX (TX_LM)  ANA:OAM_UPM_LM_CNT:OAM_UPM_LM_CNT
 *
 *  ETH-LM (single-ended)
 *  ETH-LM (dual-ended)
 *  One-way DM (1DM)
 *  Two-way DM (DMM)
 *
 */

static const vtss_inst_t inst=NULL;

static int nni_port = DEMO_OAM_NNI_PORT;
static u8  mel = DEMO_OAM_MEG_LEVEL;


// This function simply create a service VOE and return an
// ID for it to be used later on.
//
  vtss_rc create_service_voe(vtss_oam_voe_idx_t  *rv_voe_idx, vtss_oam_mep_type_t updown, int m)
{
  vtss_rc rc;
  vtss_oam_voe_type_t      voe_type;
  vtss_oam_voe_alloc_cfg_t voe_alloc_cfg;
  vtss_oam_voe_conf_t      voe_cfg;
  vtss_oam_voe_idx_t       voe_idx;


  // (1) --- Allocate a VOE resource
         
  voe_type = VTSS_OAM_VOE_SERVICE; // The type can be VTSS_OAM_VOE_PORT or VTSS_OAM_VOE_SERVICE.
                                   //  It is necessary at this point to specify which it is, since
                                   //  the resources are comming from diferent pools in the chip.

  voe_alloc_cfg.phys_port = 0;     // Only needed if voe_type = VTSS_OAM_VOE_PORT.
                                   // So in our case, where we are creating a service VOE
                                   // it is not needed.
 
  // (1.1) --- The 3 first parameters are input parameters. The 4th parameter
  //           is the return value that we need later on in order to address 
  //           the VOE.
  // 
  rc = vtss_oam_voe_alloc(inst, voe_type, &voe_alloc_cfg, &voe_idx);
  if (rc) return rc;


  // (2) --- Now configure the voe to do what you want it to do.

  rc = vtss_oam_voe_conf_get(inst, voe_idx, &voe_cfg);
  if (rc) goto create_service_voe_error;


  // (2.1) --- Configure common things like:
  //            - MAC address of the MEP/VOE
  //            - Up- or Down-MEP
  //            etc. 
  //
  demo_voe_common_cfg(&voe_cfg, updown);

  // (2.2) --- Configure operation of VOE
  //
  demo_voe_generic_cfg(&voe_cfg.generic[0]);
  demo_voe_unknown_cfg(&voe_cfg.unknown);

  
  // (2.2.1) --- OAM performance group
  
  //             ETH-LM dual ended
  //
  demo_voe_lm_cfg(&voe_cfg.ccm_lm, &voe_cfg.count, DEMO_ENABLE_CCM_LM(m));

  //             ETH-LM single ended
  //
  demo_voe_single_ended_lm(&voe_cfg.single_ended_lm, DEMO_ENABLE_LM(m));

  //             ETH-DM dual and single ended
  //
  demo_voe_dm_cfg(&voe_cfg.dm, DEMO_ENABLE_DM_DE(m), DEMO_ENABLE_DM_DE(m));

  //             ETH-SLM dual and single ended
  //
  // ???

  // (2.2.2) --- OAM fault management group

  
  //             ETH-CC
  //
  demo_voe_ccm_cfg(&voe_cfg.ccm, DEMO_ENABLE_CCM(m));


  //             ETH-LB 
  //
  demo_voe_lb_cfg(&voe_cfg.lb, DEMO_ENABLE_LB(m));

  //             ETH-LB 
  //
  demo_voe_lt_cfg(&voe_cfg.lt, DEMO_ENABLE_LT(m));

  
  //             ETH-TST
  //
  demo_voe_tst_cfg(&voe_cfg.tst, DEMO_ENABLE_TST(m));


  // (2.2.3) ---

  demo_voe_proc_cfg(&voe_cfg.proc, mel);
 
  demo_voe_count_cfg(&voe_cfg.count);

  rc = vtss_oam_voe_conf_set(inst, voe_idx, &voe_cfg);
  if (rc) goto create_service_voe_error;

  // Return the VOE index. This is used for future reference.
  *rv_voe_idx = voe_idx;
  
  return rc;


  // --- If things go wrong, we'll delete the voe resource again.
  //     Of course this should not happen, but anyway.

 create_service_voe_error:
  vtss_oam_voe_free(inst, voe_type);
  return rc;
}


// This function set the isdx of the VOE that shall be used when the VOE
// is generating OAM responses as a consequense of OAM messages.
// The slang seems to be, that we say that the VOE loops.
//
vtss_rc demo_oam_voe_loop(const vtss_oam_voe_idx_t voe_idx, const u32 loop_isdx)
{
  vtss_rc  rc;
  vtss_oam_voe_conf_t  voe_cfg;

  rc = vtss_oam_voe_conf_get(inst, voe_idx, &voe_cfg);
  if (rc) return rc;


#if defined(VTSS_ARCH_SERVAL)
  voe_cfg.loop_isdx_w = loop_isdx;
  voe_cfg.loop_isdx_p = 0;
#endif
    
#if defined(VTSS_ARCH_JAGUAR_2_CE)
  voe_cfg.loop_isdx = loop_isdx;
#endif

  rc = vtss_oam_voe_conf_set(inst, voe_idx, &voe_cfg);

  return rc;
}


// Create a MCE TX (MEP Control Entry wrt TX direction of the MEP/VOE).
//
// Here we create a ES0 entry that point at the VOE 'voe_id' in the
// EVC 'evc_id'.
//
static vtss_rc create_mce_tx(vtss_oam_voe_idx_t voe_idx, vtss_evc_id_t evc_id, vtss_isdx_t *tx_isdx)
{

  vtss_rc  rc;

  vtss_mce_id_t         mce_id;
  vtss_mce_t            mce;
  vtss_mce_port_info_t  mce_info;

  // (1) --- Prepear the mce structure
  //
  rc = vtss_mce_init(inst, &mce);
  if (rc) return rc;


  // (2) --- Configure the MCE (MEP Control Entry)
  //
  // (2.1) --- Give the MCE an id. The function demo_allocate_mce_id()
  //           is supposed to give us an ID that has not already been used.
  //
  mce.id = demo_allocate_mce_id();
#ifdef VTSS_ARCH_JAGUAR_2_CE
  mce.rule     = VTSS_MCE_RULE_TX;
  mce.key.isdx = VTSS_MCE_ISDX_NEW; // We need an new ISDX in order to hit this rule.
#endif

  //           In Serval-1 things are organized differently, so just do the following
  //           instead:
  //
#ifdef VTSS_ARCH_SERVAL
  //  The setting of the following two attributes effectivily means, that an TX rule
  //  similar to the Jaguar2 case is made, i.e. an ES0 entry.
  //
  mce.key.port_cpu = TRUE;
  mce.action.rule = VTSS_MCE_RULE_BOTH;

  mce.action.isdx = VTSS_MCE_ISDX_NEW; // We need an new ISDX in order to hit this rule.
  mce.action.tx_lookup = VTSS_MCE_TX_LOOKUP_ISDX;
#endif


#ifdef VTSS_ARCH_SERVAL
  mce.tt_loop = FALSE;
#endif


  // (3) --- Configure the action
  //
  mce.action.port_list[nni_port]  =  TRUE;
  mce.action.voe_idx      =  voe_idx;
  mce.action.oam_detect   =  VTSS_MCE_OAM_DETECT_UNTAGGED; // See note (4) below
  mce.action.evc_id       =  evc_id;                       // See note (4) below
  //  mce.action.evc_counting =  FALSE;
#ifdef VTSS_ARCH_SERVAL
  mce.action.evc_etree    =  FALSE;
#endif


  //  (4) --- In Serval-1 a CPU/AFI injected OAM frame can be added an outer tag
  //          by the egress rule. For Services, which we are doing here, the outer
  //          tag should be that of the service.
  //          In Jaguar-2 this connection is made by the 'mce.action.evc_id' so that
  //          OAM frame get the same outer tag as the EVC. Note, that if the
  //          outer tag of the EVC change, then the MCE rule for OAM is not updated
  //          and need to be deleted an created again for this to take effect.
  //
#ifdef VTSS_ARCH_SERVAL
  mce.action.outer_tag.enable = TRUE;
  if (mce.action.outer_tag.enable) {
    mce.action.outer_tag.vid = 1235;
    mce.action.outer_tag.pcp_mode = VTSS_MCE_PCP_MODE_FIXED;
    mce.action.outer_tag.pcp = 0UL;
    mce.action.outer_tag.dei_mode = VTSS_MCE_DEI_MODE_FIXED;
    mce.action.outer_tag.dei = FALSE;
  }
#endif


  // (5) --- Add this MCE rule to the API
  //
  mce_id = VTSS_MCE_ID_LAST;

  rc = vtss_mce_add(inst, mce_id, &mce);
  if (rc) return rc;

  // (6) --- We need to know the value of the new ISDX is.
  //         Otherwise we can not hit the rule
  //
  rc = vtss_mce_port_info_get(inst, 
                              mce.id, 
                              nni_port, 
                              &mce_info);
  if (rc) return rc;

  // (7) --- Return the ISDX needed in order to hit this rule.
  //
  *tx_isdx = mce_info.isdx;

  return rc;
}



// Create a MCE RX (MEP Control Entry wrt RX direction of the MEP/VOE).
//
// Here we create a CLM entry that point at the VOE 'voe_id' in the
// EVC 'evc_id'.
//
// This function is similar to create_mce_tx(), except that instead of
// creating a ES0 rule that can be hit with a tx_isdx, we make a rule
// in the CLM that can be hit by an apropiate OAM frame. And if a given
// frame hit this rule, it will be assigned the ISDX 'rx_isdx'.
// This rx_idsx is used as an index into the IPT table like IPT[rx_isdx]
// and that element in the IPT table will point to the VOE 'voe_idx'.
// In this way frames that match the CLM rule will be processed by
// the VOE 'voe_idx'.
//
static vtss_rc create_mce_rx(vtss_oam_voe_idx_t voe_idx, vtss_evc_id_t evc_id, vtss_isdx_t *rx_isdx)
{

  vtss_rc  rc;

  vtss_mce_id_t         mce_id;
  vtss_mce_t            mce;
  vtss_mce_port_info_t  mce_info;

  // (1) --- Prepear the mce structure
  //
  rc = vtss_mce_init(inst, &mce);
  if (rc) return rc;


  // (2) --- Configure the MEP Control Entry


  // (2.1) --- Give the MCE an id. The function demo_allocate_mce_id()
  //           is supposed to get us an ID that has not already been used.
  mce.id = demo_allocate_mce_id();

  // (2.2) --- The rule is located in different places in Jaguar2 and Serval1.
  //           Serval1: The rule does not realy make any difference here,
  //                    since action.port_list is empty anyway, so only a RX
  //                    will be created. So it would be ok to set it to VTSS_MCE_RULE_RX also.
  //
#ifdef VTSS_ARCH_JAGUAR_2_CE
  mce.rule = VTSS_MCE_RULE_RX;
#endif
#ifdef VTSS_ARCH_SERVAL
  mce.action.rule = VTSS_MCE_RULE_BOTH;
#endif


#if 0
  // This field is only used for MCE TX for Jagaur-2. For MCE RX the mce.action.isdx is used instead. See below.
  // I list it here to make you aware of the difference since it is not easy to guess.
  mce.key.isdx = VTSS_MCE_ISDX_NEW;
#endif

  // (2.2) --- This rule shall only match frames from port 'nni_port'.
  //
  mce.key.port_list[nni_port] = TRUE;

  // (2.3) --- The frame shall have a outer tag.
  //           We do not care if the tag is s- or c-tag.
  //           The VID shall be 1234
  //
#ifdef VTSS_ARCH_JAGUAR_2_CE
  mce.key.first_tag.tagged = VTSS_VCAP_BIT_1;     // Frame must have outer tag
  mce.key.first_tag.s_tagged = VTSS_VCAP_BIT_ANY; // Dont care is the outer tag is S- or C-tag
  mce.key.first_tag.vid.value = 1234;             
  mce.key.first_tag.vid.mask  = 0xffff;
#endif

#ifdef VTSS_ARCH_SERVAL
  // outer_tag
#endif



  // (2.4) --- Config the MEG level
  //
  mce.key.mel.value = (1<<mel) - 1; // So the 'mel' less significant bits are set
  mce.key.mel.mask  = 0x7f;         // Exact match.

  // (3) --- Configure the action

  mce.action.voe_idx         =  voe_idx;
  mce.action.isdx            =  VTSS_MCE_ISDX_NEW;
  mce.action.oam_detect      =  VTSS_MCE_OAM_DETECT_SINGLE_TAGGED;
  mce.action.evc_id          =  evc_id;


#ifdef VTSS_ARCH_JAGUAR_2_CE
  mce.action.independent_mel =  FALSE;
  mce.action.evc_cos         =  TRUE;
#endif



#ifdef VTSS_ARCH_SERVAL
  mce.action.evc_etree    =  FALSE;
#endif

  // (4) --- Add this MCE rule to the API
  //
  mce_id = VTSS_MCE_ID_LAST;

  rc = vtss_mce_add(inst, mce_id, &mce);
  if (rc) return rc;

  // (5) --- We need to know the value of the new ISDX is.
  //         Otherwise we can not hit the rule
  //
  rc = vtss_mce_port_info_get(inst, 
                              mce.id, 
                              nni_port, 
                              &mce_info);
  if (rc) return rc;

  // (6) --- Return the ISDX needed in order to hit this rule.
  //
  *rx_isdx = mce_info.isdx;

  return rc;
}



// This function configure the CLM- and ES0 TCAM so that frames that belong to
// the EVC 'evc_id', that ingress or egress on NNI 'nni_port' is sent to the VOE,
// so that it can count them. This is needed for ETH-CC and ETH-LM.
// 
static vtss_rc config_downmep_counting(vtss_oam_voe_idx_t voe_idx, vtss_evc_id_t evc_id)
{
  vtss_evc_oam_port_conf_t pc;
  vtss_rc rc;

  rc = vtss_evc_oam_port_conf_get(inst, evc_id, nni_port, &pc);
  if (rc) return rc;

  pc.voe_idx = voe_idx;

  rc = vtss_evc_oam_port_conf_set(inst, evc_id, nni_port, &pc);

  return rc;
}



// Create a DownMEP that assume the existence of an EVC with id 'evc_id' equal to 1
// This is the main entrance point for creating a DownMEP/VOE
//
static vtss_rc create_downmep(int m)
{
  vtss_rc rc;
  vtss_oam_voe_idx_t  voe_idx;
  vtss_evc_id_t       evc_id=1;
  vtss_isdx_t         tx_isdx=0, rx_isdx=0;


  // (1) --- First the VOE is allocated and configured
  //         The index 'voe_idx' is the only reference we have to
  //         this resource and it is used to connect it later on.
  //
  demo_printf("Allocate a VOE\n");
  rc = create_service_voe(&voe_idx, VTSS_OAM_DOWNMEP, m);
  if (rc) return rc;


  // (2) --- Next the MCE TX rule is created which create a rule in ES0.
  //         This rule is hit when a frames has the ISDX 'tx_isdx'. 
  //         A VOE can not initiate frames. So when an MEP shall transmit an OAM message
  //         this frame must be injected by the CPU or AFI.
  //         But the CPU can not insert the counters that is needed for ETH-CC, ETH-LM
  //         or ETH-DM in the OAM messages. But the VOE can. So the idea is
  //         that the CPU/AFI inject a frames where these fields are set to zero and
  //         use the ISDX 'tx_isdx' which will hit the ES0 rule, which point at the VOE
  //         and the VOE will fill in these fields.
  //         This is the reason for the MCE TX rule.
  //
  demo_printf("Create an MCE TX: voe_idx=%d, evc_id=%d\n", voe_idx, evc_id);
  rc = create_mce_tx(voe_idx, evc_id, &tx_isdx);
  if (rc) return rc;

  //         We need the value of tx_isdx later on when we inject OAM frames from the CPU,
  //         so we print it out here.
  //
  demo_printf("TX ISDX=%d\n", tx_isdx);


  // (3) --- If the VOE is going to generate OAM responses, then it must know the TX ISDX,
  //         so here we tell it what it is.
  //
  rc = demo_oam_voe_loop(voe_idx, tx_isdx);


  // (4) --- The VOE must know what egress frames to count. So here
  //         we tell the VOE 'voe_idx' that it shall count egress frames
  //         for EVC 'evc_id' on the egress port. The port is not
  //         provided in this call, but the function use the global variable
  //         'nni_port'.
  //
  rc = config_downmep_counting(voe_idx, evc_id);
  if (rc) return rc;


  // (5) --- In order to be able to handle OAM frames received on
  //         the NNI port a rule in CLM must be setup that point to the
  //         VOE.
  //         Actualy a CLM rule is setup that asign a ISDX to the OAM frame received.
  //         A new ISDX value is allocated. This new ISDX value point to the
  //         element IPT[ISDX] in the IPT table, that eventually point to the VOE.
  //         This is a technical detail, but nice to know since that is the reason
  //         for the need of a new ISDX value. 
  //         If the OAM frame shall be forward to the CPU, for example for OAM Responses,
  //         then this ISDX value can be used to identify that a given frames sent to
  //         the CPU is an OAM frame. 
  //
  rc = create_mce_rx(voe_idx, evc_id, &rx_isdx);
  if (rc) return rc;

  // In this example we do not actually need to know this value,
  // but we print it anyway.
  // (I have put it in (...), so you do not
  //  confuse it with TX ISDX, which ia actually needed when you
  //  eventually inject OAM messages from the CPU).
  //
  demo_printf("(RX ISDX=%u)\n", rx_isdx);


  return rc;
}



void demo_oam_down(int m)
{
  vtss_rc rc;

  demo_printf("Trying to create a DownMEP/DownVOE on NNI port %d\n", nni_port+1);
  rc = create_downmep(m);

  if (rc) {
    demo_printf("Error rc=%d\n", rc);
  }

}

#else

// The VTSS_FEATURE_OAM is not defined

void demo_oam_down(int m)
{
  demo_printf("VTSS_FEATURE_OAM is not defined\n");
}
#endif

