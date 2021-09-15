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

#ifdef VTSS_FEATURE_OAM


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

static int uni_port = DEMO_OAM_UNI_PORT;
static int nni_port = DEMO_OAM_NNI_PORT;
static u8  mel = DEMO_OAM_MEG_LEVEL;


// In this file we re-use the function in demo_oam_downmep.c to allocate and
// configure the VOE, since there is no real difference beside the value
// of the 'updown' which let the VOE know whether it is being used as
// a DownMEP or a UpMEP.
//
extern vtss_rc create_service_voe(vtss_oam_voe_idx_t  *rv_voe_idx, vtss_oam_mep_type_t updown, int m);

extern vtss_mce_id_t demo_allocate_mce_id(void);

extern vtss_rc demo_oam_voe_loop(const vtss_oam_voe_idx_t voe_idx, const u32 loop_isdx);


// Create a MCE TX (MEP Control Entry wrt TX direction of the MEP/VOE).
//
// Here we create a CLM entry that point at the VOE 'voe_id' in the
// EVC 'evc_id'.
//
static vtss_rc create_mce_tx_up(vtss_oam_voe_idx_t voe_idx, vtss_evc_id_t evc_id, vtss_isdx_t *tx_isdx)
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
  //
  // (2.1) --- Give the MCE an id. The function demo_allocate_mce_id()
  //           is supposed to get us an ID that has not already been used.
  //
  mce.id = demo_allocate_mce_id();
#ifdef VTSS_ARCH_JAGUAR_2_CE
  mce.rule = VTSS_MCE_RULE_UPINJ;   // This is an Up Injection rule
  mce.key.isdx = VTSS_MCE_ISDX_NEW; // We need an new ISDX in order to hit this rule.
#endif

  mce.key.port_list[uni_port]  =  TRUE; // The residence port

#ifdef VTSS_ARCH_SERVAL
  mce.tt_loop = FALSE;
#endif

  // (3) --- Configure the action
  //
  mce.action.voe_idx      =  voe_idx;

#ifdef VTSS_ARCH_SERVAL
  mce.action.isdx         =  VTSS_MCE_ISDX_NEW; 
  mce.action.oam_detect   =  VTSS_MCE_OAM_DETECT_SINGLE_TAGGED;
#endif

  mce.action.evc_id       =  evc_id;

#ifdef VTSS_ARCH_JAGUAR_2_CE
  mce.action.independent_mel =  FALSE;
  mce.action.evc_cos         =  TRUE;
  mce.action.evc_counting    =  TRUE; //tf? does what?
#endif


#ifdef VTSS_ARCH_SERVAL
  mce.action.evc_etree    =  FALSE;
#endif

  //  mce.action.port_list[nni_port]  =  TRUE;

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
  *tx_isdx = mce_info.isdx;

  return rc;
}


// Create a MEP RX configuration
//
// The (A.*) part of the function is almost identical to the MEP RX code
// for the DownMEP case. The only difference is that 'mce.action.evc_counting'
// is set to TRUE. But otherwise the idea and structure is the same.
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
static vtss_rc create_mce_rx_up(vtss_oam_voe_idx_t voe_idx, vtss_evc_id_t evc_id, vtss_isdx_t *rx_isdx)
{

  vtss_rc  rc;

  vtss_mce_id_t         mce_id;
  vtss_mce_t            mce;
  vtss_mce_port_info_t  mce_info;

  // (1) --- Prepear the mce structure
  //
  rc = vtss_mce_init(inst, &mce);
  if (rc) return rc;


  // (2) --- Configure the MEP Control Entry RX


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

  // (2.2) --- This rule shall only match frames from port 'nni_port'.
  //
  mce.key.port_list[nni_port] = TRUE;

  // (2.3) --- The frame shall have a outer tag.
  //           We do not care if the tag is s- or c-tag.
  //           The VID shall be 1234
  //
#ifdef VTSS_ARCH_JAGUAR_2_CE
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

  mce.action.isdx            =  VTSS_MCE_ISDX_NEW;
  mce.action.oam_detect      =  VTSS_MCE_OAM_DETECT_SINGLE_TAGGED;
  mce.action.evc_id          =  evc_id;


#ifdef VTSS_ARCH_JAGUAR_2_CE
  mce.action.independent_mel =  FALSE;
  mce.action.evc_cos         =  TRUE;
  mce.action.evc_counting    =  TRUE;
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
#ifdef VTSS_ARCH_JAGUAR_2_CE
  mce.key.isdx = *rx_isdx;
#endif

  // (B.3) --- Configure the action

  mce.action.voe_idx         =  voe_idx;
  mce.action.evc_id          =  evc_id;

  mce.action.port_list[uni_port] = TRUE;

#ifdef VTSS_ARCH_JAGUAR_2_CE
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


// This function configure the CLM- and ES0 TCAM so that frames that belong to
// the EVC 'evc_id', that ingress or egress on UNI 'uni_port' is sent to the VOE,
// so that it can count them. This is needed for ETH-CC and ETH-LM.
// 
static vtss_rc config_upmep_counting(vtss_oam_voe_idx_t voe_idx, vtss_evc_id_t evc_id)
{
  vtss_evc_oam_port_conf_t pc;
  vtss_rc rc;

  rc = vtss_evc_oam_port_conf_get(inst, evc_id, uni_port, &pc);
  if (rc) return rc;

  pc.voe_idx = voe_idx;

  rc = vtss_evc_oam_port_conf_set(inst, evc_id, uni_port, &pc);

  return rc;
}


// Create a DownMEP that assume the existence of an EVC with id 'evc_id' equal to 1
// The EVC is assumed to have a UNI on port 'ingress_port_This is the main entrance point for creating a DownMEP/VOE
//
static vtss_rc create_upmep(int m)
{
  vtss_rc rc;
  vtss_oam_voe_idx_t  voe_idx;
  vtss_evc_id_t       evc_id=1;
  vtss_isdx_t         tx_isdx=0, rx_isdx=0;


  // (1) --- First the VOE is allocated and configured
  //         The index 'voe_idx' is the only reference we have to
  //         this resource and it is used to connect it later on.
  //
  demo_printf("Allocate a VOE and configure it as an UpMEP/UpVOE\n");
  rc = create_service_voe(&voe_idx, VTSS_OAM_UPMEP, m);
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
  rc = create_mce_tx_up(voe_idx, evc_id, &tx_isdx);
  if (rc) return rc;

  //         We need the value of tx_isdx later on when we inject OAM frames from the CPU. 
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
  rc = config_upmep_counting(voe_idx, evc_id);
  if (rc) return rc;


  // (5) --- In order to be able to handle OAM frames received on
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
  rc = create_mce_rx_up(voe_idx, evc_id, &rx_isdx);
  if (rc) return rc;

  demo_printf("RX ISDX=%d\n", rx_isdx);
  

  return rc;
}



void demo_oam_up(int m)
{
  vtss_rc rc;

  demo_printf("Trying to create a UpMEP/UpVOE on UNI port %d\n", uni_port+1);
  rc = create_upmep(m);

  if (rc) {
    demo_printf("Error rc=%d\n", rc);
  }

}

#else

// The VTSS_FEATURE_OAM is not defined

void demo_oam_up(int m)
{
  demo_printf("VTSS_FEATURE_OAM is not defined\n");
}
#endif
