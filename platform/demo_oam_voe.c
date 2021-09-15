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
#include <string.h>
#include <vtss/api/options.h>

#if defined(VTSS_FEATURE_OAM)


//ok
void demo_voe_lb_cfg(vtss_oam_voe_lb_conf_t *lb, BOOL enable)
{
  lb->enable = enable;

  lb->copy_lbm_to_cpu = FALSE;
  lb->copy_lbr_to_cpu = FALSE;

#ifdef VTSS_ARCH_SERVAL
  lb->forward_lbm = FALSE;
  lb->forward_lbr = FALSE;
#endif

  lb->count_as_selected  = FALSE;
  lb->count_as_data      = FALSE;

  lb->tx_update_transaction_id  = FALSE;

  lb->tx_transaction_id  = TRUE;
  lb->rx_transaction_id = 0x11223344;
}

//ok
void demo_voe_generic_cfg(vtss_oam_voe_generic_conf_t* p)
{
#if 0
  p->enable = FALSE;             /**< TRUE => process opcode in hardware */
  p->copy_to_cpu = FALSE;        /**< TRUE => send all PDUs with the specified opcode to CPU */
  p->forward = FALSE;            /**< TRUE => forward all OAM frames of this type to the extraction destination */
  p->count_as_selected = FALSE;  /**< TRUE => count frames as selected */
  p->count_as_data = FALSE;      /**< TRUE => count frames as data for LM */
#endif
}

//ok
void demo_voe_unknown_cfg(vtss_oam_voe_unknown_conf_t* p)
{
#if 0
  p->enable = FALSE;             /**< TRUE => process opcode in hardware */
  p->copy_to_cpu = FALSE;        /**< TRUE => send all PDUs with unknown opcodes to CPU */
  p->count_as_selected = FALSE;  /**< TRUE => count frames as selected */
  p->count_as_data = FALSE;      /**< TRUE => count frames as data for LM */
#endif
}

//ok
void demo_voe_ccm_cfg(vtss_oam_voe_ccm_conf_t* p, BOOL enable)
{
  BOOL tx_ccm_sequence_number_update_and_increment;

  u8 megid[48]={'a','a','a','a','a','a','a','a',
                'a','a','a','a','a','a','a','a',
                'a','a','a','a','a','a','a','a',
                'a','a','a','a','a','a','a','a',
                'a','a','a','a','a','a','a','a',
                'a','a','a','a','a','a','a','a'};

  memcpy(&p->megid.data[0], &megid[0], 48);

  p->enable            = enable;  // Enable CCM
  p->copy_to_cpu       = FALSE; // Do not copy to CPU

  p->count_as_selected = TRUE;
  p->count_as_data     = TRUE;

  p->peer_mepid        = 0x12345678;

  p->tx_seq_no         = 0x100;

  p->rx_seq_no_check   = FALSE;
  p->rx_seq_no         = 0x1000;
  p->rx_priority       = 0;
  p->rx_period         = VTSS_OAM_PERIOD_100_MS;

  // --- Different way of doing the same thing in Serval-1 and Serval-2
  tx_ccm_sequence_number_update_and_increment = TRUE;

#if defined(VTSS_ARCH_SERVAL)
    p->tx_seq_no_auto_upd_op = tx_ccm_sequence_number_update_and_increment ? VTSS_OAM_AUTOSEQ_INCREMENT_AND_UPDATE : VTSS_OAM_AUTOSEQ_DONT;
#endif

#if defined(VTSS_ARCH_JAGUAR_2_CE)
    p->tx_seq_no_auto_update = tx_ccm_sequence_number_update_and_increment;
#endif


#if defined(VTSS_ARCH_SERVAL)
  // This attribute has most likely never been set to TRUE in Serval-1.
  // So in porting to Serval-2, this should be safe to ignore.
  p->forward = FALSE;
#endif

}



void demo_voe_count_cfg(vtss_oam_voe_lm_counter_conf_t *p)
{
  int i,j;


#ifdef VTSS_ARCH_SERVAL
  // --- This mask has one bit for each priority. Priority 0 is bit (1<<0),..., priority 7 is bit (1<<7).
  //     If a given priority bit is set in this mask, e.g. (1<<3) then priority 3 is counted on it own.
  //     If this bit was not set, then priority 3 frames would be counted in the priority 7 counter.
  //     So if each priority shall be counted seperately, then priority_mask shall be 0xff. If this
  //     value is set to 0, then all frames are counted in one counter namely the counter for priority 7.
  //
  p->priority_mask = 0;   /**< Per-priority counter enable. Zero-bit means count in prio. 7 */
#endif
#ifdef VTSS_ARCH_JAGUAR_2_CE
  // --- This attribute can have 4 different values:
  //
  //     VTSS_OAM_COSID_SRC_COSID      'ifh.cosid'  /**< Use COS ID as selected by QoS */
  //     VTSS_OAM_COSID_SRC_TC         'ifh.tc'     /**< Use selected MPLS traffic class */
  //     VTSS_OAM_COSID_SRC_CL_QOS     'ifh.iprio'  /**< Use classified QoS value (internal priority) */
  //     VTSS_OAM_COSID_SRC_PATH_COSID         = 3  /**< Use OAM MEP path COS ID value selected by QoS */
  //
  //     So 'cosid_src' specify what metadata of the frame, that shall
  //     determine the COSID.
  //
  //     VTSS_OAM_COSID_SRC_COSID: Look at create_ingress_map() in file demo_evc_j2.c.
  //     In this function that mapping of e.g. cosid is defined. So in this way you specify
  //     the cosid that a frame shall have. If cosid_src is set to VTSS_OAM_COSID_SRC_COSID,
  //     then this value is used to as the COSID in the LM context that we are talking about there
  //
#if 1
  p->cosid_src = VTSS_OAM_COSID_SRC_COSID;
#endif

  //     VTSS_OAM_COSID_SRC_CL_QOS: In this case the classified QoS value is used. This value can
  //     change when the frame flow through the.
  //     1) When the frame is received at the port, it is assigned a QoS value depending on the
  //        setting at the port which is done with vtss_qos_api.h::vtss_qos_port_conf_set().
  //     2) If the frame hit a CLM rule, then the action for that rule can change the QoS value.
  //     3) For an EVC, the associated CLM rule most likely do not change the QoS as such, but
  //        it may have an ingress map (see demo_evc_j2.c and search e.g. for ingress_map_id)
  //        associated with it. If this mapping specify a mapping for cos (which is the qos - yes
  //        some name confusion exist) then this is what take effect.
  //
#if 0
  p->cosid_src = VTSS_OAM_COSID_SRC_CL_QOS;
#endif

  // --- The source of the COSID is selected above. The value of the COSID is a number 0,...,7
  //     This number is mapped by the cosid_tbl[] below. So you can map the COSID around as you please
  //     and you can group counters in any way you like.
  //     So in Serval-1 you could count a frame at the priority it had from the beginning or
  //     you could group some priorities in the priority 7 counter. So if you had priority_mask=0xe0, 
  //     i.e. priority 5,6 and 7 is group and is counted in priority 7s counter, and 0,...,4 are counter at 
  //     each their counters, then you would, in Serval-2, set cosid_tbl[5|6|7]=7 and cosid_tbl[i]=i for i=0,..,4.
  //     Below we choose the identical mapping, i.e., the COSID does not effectivily change.
  //
  for (i=0; i<8; ++i) {
    p->cosid_tbl[i] = i;  /**< COS ID mapping table (Service/Path VOE only) */
  }

  // frames with priority 6 and 7 shall be counted in the counter for priority 7,
  // just you can see the effect.
  p->cosid_tbl[6] = 7;  /**< COS ID mapping table (Service/Path VOE only) */


  // The port_cosid_tbl[] below is similar the cosid_tbl[].
  //
  for (i=0; i<2; ++i) {
    for (j=0; j<8; ++j) {
      p->port_cosid_tbl[i][j] = j; /**< COS ID mapping table (Port VOE only). First index is DEI. */
    }
  }

  // --- Similar to cosid_src, the colour of the frame can be selected from different sources when counted.
  //     Normally yellow frames should not be counted. This selection, just like cosid_src affect only LM counting of course.
  //
  p->color_src = VTSS_OAM_COLOR_SRC_CL_DP; /**< Select source of colour for VOE LM (Service/Path VOE only) */

#endif

  p->yellow = FALSE;      /**< TRUE => include yellow frames in count */
}


//ok
void demo_voe_lm_cfg(vtss_oam_voe_ccm_lm_conf_t* lm, vtss_oam_voe_lm_counter_conf_t* lm_count, BOOL enable)
{
  int cosid, dei;

  lm->enable = enable;
  lm->copy_to_cpu = FALSE;

#if defined(VTSS_ARCH_SERVAL)
  // This attribute has most likely never been set to TRUE in Serval-1.
  // So in porting to Serval-2, this should be safe to ignore.
  lm->forward = FALSE;
#endif


  lm->count_as_selected = FALSE;


  demo_voe_count_cfg(lm_count);


  // --- Put into the frame, that we are transmitting once every sec.
  //     E.g.: If you inject 1 CCM frame every 100ms, i.e. 10 CCM frames
  //     per second, then only every 10th will contain the LM counters -
  //     meaning that these fields are zero in 9 out of 10 frames.
  //
  lm->period = VTSS_OAM_PERIOD_1_SEC;
}

//ok
void demo_voe_single_ended_lm(vtss_oam_voe_se_lm_conf_t* p, BOOL enable)
{
  p->enable = enable;
  p->copy_lmm_to_cpu = FALSE;            /**< TRUE => send all LMM PDUs to CPU */
  p->copy_lmr_to_cpu = FALSE;            /**< TRUE => send all LMR PDUs to CPU */

#if defined(VTSS_ARCH_SERVAL)
  p->forward_lmm = FALSE;                /**< TRUE => forward all OAM frames of this type to the extraction destination */
  p->forward_lmr = FALSE;                /**< TRUE => forward all OAM frames of this type to the extraction destination */
#endif

  p->count_as_selected = FALSE;          /**< TRUE => count frames as selected */
  p->count_as_data = FALSE;              /**< TRUE => count frames as data for LM */

}

//ok
void demo_voe_tst_cfg(vtss_oam_voe_tst_conf_t* p, BOOL enable)
{
#if 0
  p->enable = enable;       /**< TRUE => process TST in hardware */
  p->copy_to_cpu = FALSE;  /**< TRUE => send all TST PDUs to CPU */

  p->count_as_selected = FALSE;     /**< TRUE => count frames as selected */
  p->count_as_data = FALSE;         /**< TRUE => count frames as data for LM */
  p->tx_seq_no_auto_update = FALSE; /**< TRUE => Automatically update TX TST sequence number -- unless TST is already active */
  p->tx_seq_no = 0;                 /**< Sequence number to use for first TST TX -- unless TST is already active */
  p->rx_seq_no = 0;                 /**< Sequence number - 1 to expect -- unless TST is already active */

#if defined(VTSS_ARCH_SERVAL)
  // This attribute has most likely never been set to TRUE in Serval-1.
  // So in porting to Serval-2, this should be safe to ignore.
  p->forward = FALSE;
#endif

#endif
}

//ok
void demo_voe_dm_cfg(vtss_oam_voe_dm_conf_t* p, BOOL enable_dual_ended, BOOL enable_single_ended)
{
  p->enable_dmm = enable_single_ended; /**< TRUE => process DMM+DMR in hardware */
  p->enable_1dm = enable_dual_ended;   /**< TRUE => process 1DM in hardware */
  p->copy_1dm_to_cpu = FALSE;          /**< TRUE => send all 1DM PDUs to CPU */
  p->copy_dmm_to_cpu = TRUE;          /**< TRUE => send all DMM PDUs to CPU */
  p->copy_dmr_to_cpu = FALSE;          /**< TRUE => send all DMR PDUs to CPU */

  p->count_as_selected = FALSE; /**< TRUE => count frames as selected */
  p->count_as_data = FALSE;     /**< TRUE => count frames as data for LM */

#if defined(VTSS_ARCH_SERVAL)
  p->forward_1dm = FALSE;     /**< TRUE => forward all OAM frames of this type to the extraction destination */
  p->forward_dmm = FALSE;     /**< TRUE => forward all OAM frames of this type to the extraction destination */
  p->forward_dmr = FALSE;     /**< TRUE => forward all OAM frames of this type to the extraction destination */
#endif

#if defined(VTSS_ARCH_JAGUAR_2_CE)
  p->extern_ts = FALSE;       /**< TRUE => external device updates the DMM/DMR/1DM timestamp information */
#endif
}

//ok
void demo_voe_lt_cfg(vtss_oam_voe_lt_conf_t *p, BOOL enable)
{
  p->enable = enable;           /**< TRUE => process LB in hardware */
  p->copy_ltm_to_cpu = FALSE;   /**< TRUE => send all LTM PDUs to CPU */
  p->copy_ltr_to_cpu = FALSE;   /**< TRUE => send all LTR PDUs to CPU */
  p->count_as_selected = FALSE; /**< TRUE => count frames as selected */
  p->count_as_data = FALSE;     /**< TRUE => count frames as data for LM */

#if defined(VTSS_ARCH_SERVAL)
  p->forward_ltm = FALSE;       /**< TRUE => forward all OAM frames of this type to the extraction destination */
  p->forward_ltr = FALSE;       /**< TRUE => forward all OAM frames of this type to the extraction destination */
#endif
}

void demo_voe_proc_cfg(vtss_oam_proc_conf_t* p, u8 mel)
{
  p->meg_level = (u32)mel;
  p->dmac_check_type = VTSS_OAM_DMAC_CHECK_NONE;//VTSS_OAM_DMAC_CHECK_BOTH;//VTSS_OAM_DMAC_CHECK_UNICAST;
  p->ccm_check_only = FALSE;//TRUE;

  // --- This flag is a modifier of the following 'copy_on_*' so that only the next occurence
  //     that match a certain 'copy_on_*' will go to the CPU
  p->copy_next_only = FALSE; // FALSE: No modification to 'copy_on_*'
                             // TRUE: hit once for each 'copy_on_*'


  p->copy_on_ccm_err               = FALSE; 
  p->copy_on_mel_too_low_err       = FALSE;


#ifdef VTSS_ARCH_SERVAL
  // --- For Serval-1, enabling this will copy CCM PDU to the CPU, if it contain
  //     more than one, i.e. more than the mandatory CCM TLV (see Y.1731, fig 9.2.1)
  //     This will be the case, if the CCM PDU contail TLVs for Port Status and/or 
  //     Interface Status. If that information is needed, then those CCM PDU
  //     should be copied to the CPU, i.e. setting the attribute below to TRUE.
  //
  p->copy_on_ccm_more_than_one_tlv = FALSE; // Only if copy_next_only = TRUE in Serval-1
#endif
#ifdef VTSS_ARCH_JAGUAR_2_CE
  // --- In Jaguar2 this is different. If a Port Status TLV is present in the CCM PDU, 
  //     then HW will extract that. Likewise goes for Interface Status TLV.
  //     The status is read with vtss_oam_ccm_status_get().
  //
  //     In order to find out if it is necessary to read the CCM status, the function
  //     vtss_oam_voe_event_poll(, voe_idx, &mask) can be called.
  //     If mask & VTSS_M_VOP_VOE_STAT_INTR_STICKY_TLV_PORT_STATUS_STICKY is set, then
  //     Port Status has changed.
  //     Similar if mask & VTSS_M_VOP_VOE_STAT_INTR_STICKY_TLV_INTERFACE_STATUS_STICKY
  //     is set, then Interface Status for that VOE has changed.
  //
  //     Interrupt can be enabled for these values if polling is not wanted. This is done
  //     with vtss_oam_voe_event_enable(, voe_idx, mask, TRUE) where 'mask' is
  //     VTSS_M_VOP_VOE_STAT_INTR_ENA_TLV_PORT_STATUS_INTR_ENA and/or
  //     VTSS_M_VOP_VOE_STAT_INTR_ENA_TLV_INTERFACE_STATUS_INTR_ENA
  //     (see vtss_jaguar2c_regs_vop.h) depending on what you want.
  //
  //     If you want to receive CCM PDUs with other TLVs than the 3 mentioned, then you
  //     can enable tht with:
  //     p->copy_on_ccm_tlv_nonzero = TRUE;
  //
  //     If you set p->copy_next_valid_ccm = TRUE, then the next received CCM PDU
  //     is sent to the CPU, and only that PDU. This is a "hit me once" operation.
  //     If you want CCM PDUs sent periodically to the CPU, then set
  //     p->auto_copy_next_valid_ccm = TRUE. 
  //     The intervall with which this will be done is configured with 
  //     vtss_oam_vop_conf_set( , &cfg), when C.auto_copy_next_valid_ccm select which
  //     timer out of two to use. The timeout value of these timers are set with
  //     cfg.auto_copy_period[0|1]
#endif


  p->copy_on_dmac_err              = FALSE; // Do not honour copy_next_only in Serval-1

  // --- There are many more copy_on_*, auto_copy_* and copy_next_* attributes in p->X
 
}

//  rc = vtss_oam_voe_conf_get(inst, voe_idx, &voe_cfg);




static u8 voe_unimac[6]={0x00,0x01,0xc1,0x00,0x00,0x22};
// This configure the common fields of 'vtss_oam_voe_conf_t'
//
#ifdef VTSS_ARCH_JAGUAR_2_CE
void demo_voe_common_cfg(vtss_oam_voe_conf_t *p, vtss_oam_mep_type_t updown)
{
  // (1) --- The 'voe_type' has been set when the VOE was allocated. So do not touch this field.
  //         The attribute specify whether the VOE is a service- or a port VOE.

  // (2) --- Set VOE MAC address to 00:01:c1:00:00:22.
  //
  memcpy(p->unicast_mac.addr, voe_unimac, 6);

  // (3) --- Set MEP type, i.e. VTSS_OAM_DOWNMEP, VTSS_OAM_UPMEP or OAM_MIP.
  //
  p->mep_type = updown;
  p->mepid = 0x0101;

  // (4) --- Service to path, i.e., let Service VOE point at Path VOE
  //         If enabled the svc_to_path_idx* must be set appropriately.
  //         The 'svc_to_path_idx_w' in Serval-1 is equvivalent to
  //         'svc_to_path_idx' in J2 Rev C architecture.
  //
  p->svc_to_path = FALSE;
  if (p->svc_to_path) {       // Is is only necessary to the "pointers" is svc_to_path is true.
#ifdef VTSS_ARCH_SERVAL
    p->svc_to_path_idx_w = 0;
    p->svc_to_path_idx_p = 0; // Only necessary is protection is used.
#endif
#if defined(VTSS_ARCH_JAGUAR_2_CE)
    p->svc_to_path_idx=0;
#endif
  }

  // (5) --- Loop ISDX: If the VOE shall generate OAM responses due to OAM messages,
  //         the ISDX of the OAM response must be configured. At this point, i.e. when
  //         the VOE is initially created, this ISDX is not known yet since the MCE TX
  //         rule has not been created, Therefore this ISDX must be configured at a 
  //         later point.
  //         The Serval-1 'loop_isdx_w' and the Jaguar2, Rev C 'loop_isdx' are equvivalent.
  //         The 'loop_isdx_p' is only necessary if protection is used.
  //
#ifdef VTSS_ARCH_SERVAL
  p->loop_isdx_w = 0;
  p->loop_isdx_p = 0;
#endif
#ifdef VTSS_ARCH_JAGUAR_2_CE
  p->loop_isdx = 0;
#endif



#if 0 
  p->loop_clear_dp = FALSE;  /**< TRUE => Clear drop precedence on loop (LB, LM, DM) */
  p->block_mel_high = FALSE; /**< TRUE => Block OAM PDUs with MEG level higher than the MEP MEG level */
  p->block_data_rx = FALSE;  /**< TRUE => Block all RX frames not processed as OAM as well as OAM frames with MEG level higher than the MEP MEG level */
  p->block_data_tx = FALSE;  /**< TRUE => Block all TX frames not processed as OAM as well as OAM frames with MEG level higher than the MEP MEG level */
  p->sat_test = FALSE;       /**< TRUE => VOE is used for Service Acceptance Test (SAT) */
#endif

}
#endif

#ifdef VTSS_ARCH_SERVAL
void demo_voe_common_cfg(vtss_oam_voe_conf_t *p, vtss_oam_mep_type_t updown)
{

  // (1) --- The 'voe_type' has been set when the VOE was allocated. So do not touch this field.
  //         The attribute specify whether the VOE is a service- or a port VOE.

  // (2) --- Set VOE MAC address to 00:01:c1:00:00:22.
  //
  memcpy(p->unicast_mac.addr, voe_unimac, 6);

  // (3) --- Set MEP type, i.e. VTSS_OAM_DOWNMEP, VTSS_OAM_UPMEP or OAM_MIP.
  //
  p->mep_type = updown;

  // (4) --- 
  p->svc_to_path = FALSE;


#if 0
  p->svc_to_path_idx_w = 0;  /**< Service VOE only: Index of associated Path VOE, working */
  p->svc_to_path_idx_p = 0;  /**< Service VOE only: Index of associated Path VOE, protected */

  p->loop_isdx_w = 0;        /**< Loop (LB, DM) Service index, working */
  p->loop_isdx_p = 0;        /**< Loop (LB, DM) Service index, protected */

  p->loop_portidx_p = 0;     /**< Loop (LB, DM) port index, protected */

  p->sdlb_enable = FALSE;    /**< TRUE => Use another S-DLB (sdlb_idx) for OAM frames counted as such (not as data) */
  p->sdlb_idx = 0;           /**< Index of S-DLB to use */
#endif

}
#endif


#ifdef VTSS_ARCH_SERVAL
void demo_voe_upmep_cfg(vtss_oam_voe_up_mep_conf_t *p)
{
  p->discard_rx = FALSE;     /**< TRUE => all non-OAM RX data is discarded */
  p->loopback = FALSE;       /**< TRUE => loop OAM mode; FALSE => in-service mode */
  p->port = 0;                /**< Front port where up-MEP is located */
}
#endif

#else
// VTSS_FEATURE_OAM is not defined


#endif
