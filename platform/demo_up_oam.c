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

#ifdef VTSS_FEATURE_OAM


extern void print_voe_cfg(vtss_oam_voe_conf_t *voe_cfg);


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
static int mep_ingress_port = 0;
static int mep_egress_port = 1;

static u8  mel = 3;


// This function simply create a service VOE and return an
// ID for it to be used later on.
//
vtss_rc create_service_voe(vtss_oam_voe_idx_t  *rv_voe_idx)
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
  demo_voe_common_cfg(&voe_cfg, VTSS_OAM_UPMEP);

  // (2.2) --- Configure operation of VOE
  //
  demo_voe_generic_cfg(&voe_cfg.generic[0]);
  demo_voe_unknown_cfg(&voe_cfg.unknown);
  demo_voe_ccm_cfg(&voe_cfg.ccm);
  demo_voe_lm_cfg(&voe_cfg.ccm_lm, &voe_cfg.count);
  demo_voe_single_ended_lm(&voe_cfg.single_ended_lm);
  demo_voe_lb_cfg(&voe_cfg.lb);           // Configure LB
  demo_voe_tst_cfg(&voe_cfg.tst);
  demo_voe_dm_cfg(&voe_cfg.dm);
  demo_voe_lt_cfg(&voe_cfg.lt);

  demo_voe_proc_cfg(&voe_cfg.proc, mel);
 
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



// This function set the ISDX needed in order to have the
// VOE perform loop to LB and DM, i.e. LBM->LBR and DMM->DMR
//
vtss_rc config_service_voe_lb(vtss_oam_voe_idx_t voe_idx)
{
  vtss_rc rc;
  vtss_oam_voe_conf_t      voe_cfg;
  vtss_mce_port_info_t     mce_port_info;
  vtss_isdx_t isdx;
  vtss_mce_id_t  mce_id = 1;
 

  // Get the ISDX for this 'mce_id'
  //
  rc = vtss_mce_port_info_get(inst, mce_id, mep_ingress_port, &mce_port_info);
  if (rc) return rc;

  isdx = mce_port_info.isdx; // Get the MCE ingress ISDX
  demo_printf("MCE ingress ISDX=%u\n", isdx);

  // Configure the VOE with this ingress MCE ISDX, so that the VOE
  // will assign this ISDX for response frames
  //
  rc = vtss_oam_voe_conf_get(inst, voe_idx, &voe_cfg);
  if (rc) return rc;

#if defined(VTSS_ARCH_SERVAL)
  voe_cfg.loop_isdx_w = isdx;
#endif

  rc = vtss_oam_voe_conf_set(inst, voe_idx, &voe_cfg);
  return rc;
}



// This function will create a "MEP Control Entry" or MCE for short.
//
// 
vtss_rc create_mce(vtss_oam_voe_idx_t voe_idx, vtss_evc_id_t evc_id)
{

  vtss_rc        rc;

  vtss_mce_id_t  mce_id;
  vtss_mce_t     mce;

  rc = vtss_mce_init(inst, &mce);
  if (rc) return rc;

  // (1) --- Configure the MEP Control Entry

  // (1.1) --- Give the MCE an id.
  mce.id = 1;

  // (1.2) --- Set the ports from which an ingress frame must come from in order
  //           to be able to match this entry.
  //           In this case we only set one port, namely 'mep_ingress_port'.
  //           If the frames from the CPU shall be potentially matched, then
  //           set port_cpu to TRUE. The CPU port is just another port,
  //           but it has its own member in this structure instead of putting 
  //           it into port_list.
  //
  mce.key.port_list[mep_ingress_port] = TRUE;

  // (1.3) --- Set the MEG Level (or MEL for short) which is a number 0,...,7.
  //           For this example we choose MEL = 3.
  //
  mce.key.mel.value = (1<<mel) - 1; // So the 'mel' less significant bits are set
  mce.key.mel.mask  = 0x7f;         // Exact match.

  // (1.4) --- If frames inject by the CPU to the ingress port shall be subject
  //           to this rule, then set 'port_cpu' to TRUE.
  //           Since vtss_mce_init() has st this value to FALSE, we do not explitcitly
  //           set it to that value, if it is supposed to be FALSE.
  //
  // mce.port_cpu = FALSE;

  // (1.5) --- Configure outer tag to exist. Dont care if it is S- or C-tag, 
  //           set the VID=0x123, pcp=dont care, dei=dont care
  //
#if defined(VTSS_ARCH_SERVAL)
  mce.key.tag.tagged = VTSS_VCAP_BIT_1;     // Frame must be tagged
  mce.key.tag.s_tagged = VTSS_VCAP_BIT_ANY; // but dont care if it is S- or C-tagged.

  mce.key.tag.vid.value = 0x4d2;            // The VID must be 0x123
  mce.key.tag.vid.mask =  0xfff;            // excatly, i.e. no wildcard bits.

  mce.key.tag.pcp.value = 0;                // This value coes not matter since
  mce.key.tag.pcp.mask = 0;                 // the mask is 0, that is dont care.

  mce.key.tag.dei = VTSS_VCAP_BIT_ANY;      // Dont care.
#endif


  // (1.6) --- Similar to (1.5) we can configure inner_tag. In this example we will let it
  //           be, so that it is dontcare wrt esistence and value.

  // (1.7) --- CPU/AFI injection
  // mce.injected = 

  // (1.8) --- For IS1, 3 lookup can be performed.
  //           The MCE lookup can be configured to happen in the 1st or 2nd lookup.
  //           Default is in the 1st lookup, i.e. the value 0 of 'lookup'.
  //
  // mce.lookup = 0;

  // (1.9) --- The 'dmac' and 'dmac_mc' are not used for anything.
  //           So just forget that they exist.


  // (2) --- Configure the action

  mce.action.port_list[mep_ingress_port] = TRUE;
  mce.action.voe_idx = voe_idx;
  mce.action.isdx = VTSS_MCE_ISDX_NEW; 
  mce.action.oam_detect = VTSS_MCE_OAM_DETECT_SINGLE_TAGGED;
  mce.action.evc_id = evc_id;
  mce.action.evc_counting = FALSE;
#if defined(VTSS_ARCH_SERVAL)

  mce.action.evc_etree = FALSE;

  //  mce.action.outer_tag = 0;
  mce.action.rule = VTSS_MCE_RULE_RX;
  mce.action.tx_lookup = VTSS_MCE_TX_LOOKUP_ISDX;
  mce.action.policer_id = 0;
  mce.action.policy_no = 0;
#endif
  mce.action.prio_enable = FALSE;
  mce.action.prio = 6; // 0,...,7
  mce.action.vid = 1234;
  mce.action.pop_cnt = 1;

#if 0
    BOOL                  port_list[VTSS_PORT_ARRAY_SIZE]; /**< Egress port list */
    vtss_oam_voe_idx_t    voe_idx;                         /**< VOE index or VTSS_OAM_VOE_IDX_NONE */
    vtss_isdx_t           isdx;                            /**< ISDX or VTSS_MCE_ISDX_NONE or VTSS_MCE_ISDX_NEW or VTSS_MCE_ISDX_EVC */
    vtss_mce_oam_detect_t oam_detect;                      /**< OAM detection */
    vtss_evc_id_t         evc_id;                          /**< EVC ID or VTSS_EVC_ID_NONE */
    BOOL                  evc_counting;                    /**< OAM frames hitting this rule must be counting as EVC frames */
    BOOL                  evc_etree;   /**< OAM frames hitting this rule must be forwarded as in an E-TREE. ISDX mask must be 
                                          used restricting forwarding and no ES0 created */
    vtss_mce_outer_tag_t  outer_tag;   /**< Egress outer tag */
    vtss_mce_rule_t       rule;        /**< Rule type */
    vtss_mce_tx_lookup_t  tx_lookup;   /**< Egress lookup type */
    vtss_evc_policer_id_t policer_id;  /**< Policer ID */
    vtss_acl_policy_no_t  policy_no;   /**< ACL policy number */
    BOOL                  prio_enable; /**< Enable priority classification */
    vtss_prio_t           prio;        /**< Priority (QoS class) */
    vtss_vid_t            vid;         /**< Classified internal VID */
    u8                    pop_cnt;     /**< Pop count or VTSS_MCE_POP_NONE */
#endif

  // (3) --- Add this MCE rule to the API
  //
  mce_id = VTSS_MCE_ID_LAST;

  rc = vtss_mce_add(inst, mce_id, &mce);
  if (rc) return rc;

  return rc;
}



vtss_mce_id_t demo_allocate_mce_id(void)
{
  static vtss_mce_id_t mce_id=0;
  return ++mce_id;
}



// Create a MCE TX (MEP Control Entry wrt TX direction of the MEP/VOE).
//
// Here we create a ES0 entry that point at the VOE 'voe_id' in the
// EVC 'evc_id'.
//
vtss_rc create_mce_tx(vtss_oam_voe_idx_t voe_idx, vtss_evc_id_t evc_id, vtss_isdx_t *tx_isdx)
{

  vtss_rc  rc;

  vtss_mce_id_t         mce_id;
  vtss_mce_t            mce;
  vtss_mce_port_info_t  mce_info;

  // (1) --- Prepear the mce structure
  //
  rc = vtss_mce_init(inst, &mce);
  demo_printf("E1 rc=%d\n", rc);
  if (rc) return rc;


  // (2) --- Configure the MEP Control Entry
  //
  // (2.1) --- Give the MCE an id. The function demo_allocate_mce_id()
  //           is supposed to get us an ID that has not already been used.
  mce.id = demo_allocate_mce_id();
#ifdef VTSS_ARCH_JAGUAR_2_CE
  mce.rule = VTSS_MCE_RULE_TX;
  mce.key.isdx = VTSS_MCE_ISDX_NEW; // We need an new ISDX in order to hit this rule.
#endif

#ifdef VTSS_ARCH_SERVAL
  mce.tt_loop = FALSE;
#endif


  // (3) --- Configure the action
  //
  mce.action.port_list[mep_egress_port]  =  TRUE;
  mce.action.voe_idx      =  voe_idx;
  //  mce.action.isdx         =  VTSS_MCE_ISDX_NEW; 
  //  mce.action.oam_detect   =  VTSS_MCE_OAM_DETECT_SINGLE_TAGGED;
  mce.action.evc_id       =  evc_id;
  //  mce.action.evc_counting =  FALSE;
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
                              mep_egress_port, 
                              &mce_info);
  if (rc) return rc;

  // (6) --- Return the ISDX needed in order to hit this rule.
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

// Create a MEP RX configuration
//
// The (A.*) part of the function is almost identical to the MEP RX code
// for the DownMEP case. The only differetnce is that 'mce.action.evc_counting'
// is set to TRUE. But otherwize the idea and structure is the same.
//
// So part (A.*) create a CLM rule on the NNI, so that when OAM frames are received
// on this port, it will match to CLM entry created, and will be given the ISDX 'rx_isdx'
// So in this way, we have classified the relecant OAM frames.
//
// The UpMEP or (Up)VOE is resident on the UNI port, so when the OAM frame come through
// the switch and is on its way towards the UNI, then we want it to be sent to the VOE.
// Therefore in part (B.*) we create a MCE TX rule which will create an entry in the ES0
// and use only the 'rx_isdx' as a key. This ES0 rule shall point at the VOE in question.
//
//
vtss_rc create_mce_rx(vtss_oam_voe_idx_t voe_idx, vtss_evc_id_t evc_id, vtss_isdx_t *rx_isdx)
{

  vtss_rc  rc;

  vtss_mce_id_t         mce_id;
  vtss_mce_t            mce;
  vtss_mce_port_info_t  mce_info;

  // (1) --- Prepear the mce structure
  //
  rc = vtss_mce_init(inst, &mce);
  demo_printf("E1 rc=%d\n", rc);
  if (rc) return rc;


  // (2) --- Configure the MEP Control Entry


  // (2.1) --- Give the MCE an id. The function demo_allocate_mce_id()
  //           is supposed to get us an ID that has not already been used.
  mce.id = demo_allocate_mce_id();

#ifdef VTSS_ARCH_JAGUAR_2_CE
  mce.rule = VTSS_MCE_RULE_RX;
#endif

#if 0
  // This field is only used for MCE TX. For MCE RX the mce.action.isdx is used instead. See below.
  // I list it here to make you aware of the difference since it is not easy to guess.
  mce.key.isdx = VTSS_MCE_ISDX_NEW;
#endif

  // (2.2) --- This rule shall only match frames from port 'mep_egress_port'.
  //
  mce.key.port_list[mep_egress_port] = TRUE;

  // (2.3) --- The frame shall have a outer tag.
  //           We do not care if the tag is s- or c-tag.
  //           The VID shall be 1234
  //
#ifdef TSS_ARCH_JAGUAR_2_CE
  mce.key.first_tag.tagged = VTSS_VCAP_BIT_1;
  mce.key.first_tag.s_tagged = VTSS_VCAP_BIT_ANY;
  mce.key.first_tag.vid.value = 1234;
  mce.key.first_tag.vid.mask  = 0xffff;
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


#ifdef TSS_ARCH_JAGUAR_2_CE
  mce.action.independent_mel =  FALSE;
  mce.action.evc_cos         =  TRUE;
  mce.action.evc_counting    =  TRUE; //tf? does what
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
                              mep_egress_port, 
                              &mce_info);
  if (rc) return rc;

  // (6) --- Return the ISDX needed in order to hit this rule.
  //
  *rx_isdx = mce_info.isdx;




  //
  // --- Uptil this point the creating of an MEP RX has been identical to
  //     the DownMEP case, except for the mce.action.evc_counting attribute.
  //
  //     For UpMEP, the MEP is on the "other side" of the switch, so we need 
  //     to create a MCE TX rule on the UNI also.

  // So here we go creating one more MCE but TX this time, because it has to be
  // in ES0




  // (B.1) --- Prepear the mce structure
  //
  rc = vtss_mce_init(inst, &mce);
  if (rc) return rc;


  // (B.2) --- Configure the MEP Control Entry


  // (B.2.1) --- Give the MCE an id. The function demo_allocate_mce_id()
  //           is supposed to get us an ID that has not already been used.
  mce.id = demo_allocate_mce_id();

#ifdef VTSS_ARCH_JAGUAR_2_CE
  mce.rule = VTSS_MCE_RULE_TX;
#endif

  // (B.2.2) --- The 'rx_isdx' for the MCE RX rule above is part of the key here.
  //             So that means that is an OAM frame hit the CLM rule created in 
  //             section (A.*) above and are switched to the UNI port, then it will
  //             hit this ES0 rule we are about to create.
  mce.key.isdx = rx_isdx;

  // (B.3) --- Configure the action

  mce.action.voe_idx         =  voe_idx;
  mce.action.evc_id          =  evc_id;

  mce.action.port_list[mep_ingress_port] = TRUE;

#ifdef TSS_ARCH_JAGUAR_2_CE
  mce.action.independent_mel =  FALSE;
#endif

#ifdef VTSS_ARCH_SERVAL
  mce.action.evc_etree    =  FALSE;
#endif

  // (B.4) --- Add this MCE rule to the API
  //
  mce_id = VTSS_MCE_ID_LAST;

  rc = vtss_mce_add(inst, mce_id, &mce);
  if (rc) return rc;

  //
  // Finally we should also make a rule, that make sure, that 
  // OAM frames ingress on the UNI is blocked.
  //
  // TBD

  return rc;
}



// This function connect the VOE to the counters in EVC 'evc_id' on port 'mep_egress_port'
// 
static vtss_rc config_downmep_counting(vtss_oam_voe_idx_t voe_idx, vtss_evc_id_t evc_id)
{
  vtss_evc_oam_port_conf_t pc;
  vtss_rc rc;

  rc = vtss_evc_oam_port_conf_get(inst, evc_id, mep_egress_port, &pc);
  if (rc) return rc;

  pc.voe_idx = voe_idx;

  rc = vtss_evc_oam_port_conf_set(inst, evc_id, mep_egress_port, &pc);

  return rc;
}



// Create a DownMEP that assume the existence of an EVC with id 'evc_id' equal to 1
// The EVC is assumed to have a UNI on port 'ingress_port_This is the main entrance point for creating a DownMEP/VOE
//
static vtss_rc create_downmep(void)
{
  vtss_rc rc;
  vtss_oam_voe_idx_t  voe_idx;
  vtss_evc_id_t       evc_id=1;
  vtss_isdx_t         tx_isdx, rx_isdx;


  // (1) --- First the VOE is allocated and configured
  //         The index 'voe_idx' is the only reference we have to
  //         this resource and it is used to connect it later on.
  //
  demo_printf("Allocate a VOE\n");
  rc = create_service_voe(&voe_idx);
  if (rc) return rc;


  // (2) --- Next the MCE TX rule is created which create a rule in ES0.
  //         This rule is hit when a frames has the ISDX 'tx_isdx'. 
  //         A VOE can not initial frames, which is necessary when a
  //         OAM message shall be transmitted. This frame must be injected by
  //         the CPU or AFI. But the CPU can not inster the counters that
  //         might be needed in the OAM messages. But the VOE can. So the idea is
  //         that the CPU/AFI inject a frames where these fields are set to zero and
  //         use the ISDX 'tx_isdx' The ES0 rule will trigger, that the frame is
  //         subject to VOE processing, and the VOE will fill in these fields.
  //         This is the reason for the MCE TX rule!
  //
  demo_printf("Create an MCE TX: voe_idx=%d, evc_id=%d\n", voe_idx, evc_id);
  rc = create_mce_tx(voe_idx, evc_id, &tx_isdx);
  if (rc) return rc;

  //         We need the value of tx_isdx later on when we inject OAM frames from the CPU. 
  //
  demo_printf("TX ISDX=%d\n", tx_isdx);


  // (3) --- The VOE must know what egress frames to count. So here
  //         we tell the VOE 'voe_idx' that it shall count egress frames
  //         for EVC 'evc_id' on the egress port. The port is not
  //         provided in this call, but the function use the global variable
  //         'mep_egress_port'.
  //
  rc = config_downmep_counting(voe_idx, evc_id);
  if (rc) return rc;


  // (4) --- In order to be able to handle OAM frames received on
  //         the NNI port a rule in CLM must be setup that point to the
  //         VOE.
  //         Actualy a CLM rule is setup that asign a ISDX to the OAM frame received.
  //         A new ISDX value is allocated. This mew ISDX value point to the
  //         element IPT[ISDX] in the IPT table, that eventually point to the VOE.
  //         This is a technical detail, but nice to know since that is the reason
  //         for the need of a new ISDX value. 
  //         If the OAM frame shall be forward to the CPU, for example for OAM Responses,
  //         then this ISDX value can be used to identify that a given frames sent to
  //         the CPU is an OAM frame. 
  //
rc = create_mce_rx(voe_idx, evc_id, &rx_isdx);
  if (rc) return rc;

  demo_printf("RX ISDX=%d\n", rx_isdx);
  

  return rc;
}



void demo_oam(void)
{
  vtss_rc rc;

  rc = create_downmep();

  if (rc) {
    demo_printf("Error rc=%d\n", rc);
  }

}

#else

// The VTSS_FEATURE_OAM is not defined

void demo_oam(void)
{
  demo_printf("VTSS_FEATURE_OAM is not defined\n");
}
#endif
