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

#include <vtss_api.h>
#include "demo_oam.h"

#ifdef VTSS_FEATURE_OAM

#define MEL         DEMO_OAM_MEG_LEVEL
#define VERSION     1

static u8 lmm_frame[18+4*4+1] = {0x00,0x01,0xc1,0xbe,0xef,0x11 , 0x00,0x01,0xc1,0xbe,0xef,0x22 , /* DMAC, SMAC */
                           0x81,0x00,0x00,0x00, /* VLAN TAG */
                           0x89,0x02,           /* Etype=0x8902=OAM */
                           (MEL << 5) | 1, 43,  0, 12,
                           0,0,0,0, /* TxFCf must be inserted by chip */
                           0,0,0,0, /* Reserved for RxFCf in LMR */
                           0,0,0,0, /* Reserved for TxFCb in LMR */
                           0        /* End TLV (0) */
}; 


static u8 ccm_lm_frame[18+10+48+4*4+1] = {0x00,0x01,0xc1,0xbe,0xef,0x11, // DMAC
                                          0x00,0x01,0xc1,0xbe,0xef,0x22, // SMAC
                                          0x81,0x00,0x00,0x00, /* VLAN TAG */
                                          0x89,0x02,                     // Etype=0x8902=OAM
                                          (MEL << 5) | 0/*version*/, 1,  // CCM
                                          0x04,                          // FLAG
                                          70,                            // TLV_OFFSET
                                          0,0,0,0,                       // Sequence number
                                          1,1,                           // MEP ID

                                          'a','a','a','a','a','a','a','a',
                                          'a','a','a','a','a','a','a','a',
                                          'a','a','a','a','a','a','a','a',
                                          'a','a','a','a','a','a','a','a',
                                          'a','a','a','a','a','a','a','a',
                                          'a','a','a','a','a','a','a','a',

                                          0,0,0,0,   /* TxFCf must be inserted by chip */
                                          0,0,0,0,   /* Reserved for RxFCb             */
                                          0,0,0,0, /* Reserved for TxFCb             */
                                          0,0,0,0,   /* Reserved                       */
                                          0          /* End TLV (0) */
}; 

static u8 ccm_lm_frame_up[18+10+48+4*4+1+4] = {0x00,0x01,0xc1,0xbe,0xef,0x11, // DMAC
                                          0x00,0x01,0xc1,0xbe,0xef,0x22, // SMAC
                                          0x81,0x00,0x04,0xd2, /* VLAN TAG */
                                          0x81,0x00,0x00,0x00, /* VLAN TAG */
                                          0x89,0x02,                     // Etype=0x8902=OAM
                                          (MEL << 5) | 0/*version*/, 1,  // CCM
                                          0x04,                          // FLAG
                                          70,                            // TLV_OFFSET
                                          0,0,0,0,                       // Sequence number
                                          1,1,                           // MEP ID

                                          'a','a','a','a','a','a','a','a',
                                          'a','a','a','a','a','a','a','a',
                                          'a','a','a','a','a','a','a','a',
                                          'a','a','a','a','a','a','a','a',
                                          'a','a','a','a','a','a','a','a',
                                          'a','a','a','a','a','a','a','a',

                                          0,0,0,0,   /* TxFCf must be inserted by chip */
                                          0,0,0,0,   /* Reserved for RxFCb             */
                                          0,0,0,0, /* Reserved for TxFCb             */
                                          0,0,0,0,   /* Reserved                       */
                                          0          /* End TLV (0) */
}; 

static u8 _1dm_frame[18+4+4+4+1] =        {0x00,0x01,0xc1,0xbe,0xef,0x11, // DMAC
                                          0x00,0x01,0xc1,0xbe,0xef,0x22, // SMAC
                                          0x81,0x00,0x00,0x00, /* VLAN TAG */
                                          0x89,0x02,                     // Etype=0x8902=OAM
                                          (MEL << 5) | 1/*version*/, 45, // 1DM
                                          0x00,                          // FLAG (on-demand=0,proactive=1)
                                          16,                            // TLV_OFFSET
                                          0,0,0,0,                       // TxTimeStampf
                                          0,0,0,0,                       // RxTimeStampf
                                          0          /* End TLV (0) */
}; 

static u8 dmm_frame[18+4+4+4+1] =        {0x00,0x01,0xc1,0xbe,0xef,0x11, // DMAC
                                          0x00,0x01,0xc1,0xbe,0xef,0x22, // SMAC
                                          0x81,0x00,0x00,0x00, /* VLAN TAG */
                                          0x89,0x02,                     // Etype=0x8902=OAM
                                          (MEL << 5) | 1/*version*/, 47, // 1DM
                                          0x00,                          // FLAG (on-demand=0,proactive=1)
                                          16,                            // TLV_OFFSET
                                          0,0,0,0,                       // TxTimeStampf
                                          0,0,0,0,                       // RxTimeStampf
                                          0          /* End TLV (0) */
}; 


#define PDU_OFFSET1 (2*6 + 1*4 + 2)
#define PDU_OFFSET2 (2*6 + 2*4 + 2)
#define M(s) s, sizeof(s)

struct frame_list frame_list[2][13] = {
  { // --- For DownMEP injection
    {  VTSS_PACKET_OAM_TYPE_NONE,    "NONE"   , PDU_OFFSET1, 0, 0},
    {  VTSS_PACKET_OAM_TYPE_CCM,     "CCM"    , PDU_OFFSET1, 0, 0},
    {  VTSS_PACKET_OAM_TYPE_CCM_LM,  "CMM-LM" , PDU_OFFSET1, M(ccm_lm_frame)},
    {  VTSS_PACKET_OAM_TYPE_LBM,     "LBM"    , PDU_OFFSET1, 0, 0},
    {  VTSS_PACKET_OAM_TYPE_LBR,     "LBR"    , PDU_OFFSET1, 0, 0},
    {  VTSS_PACKET_OAM_TYPE_LMM,     "LMM"    , PDU_OFFSET1, M(lmm_frame)},
    {  VTSS_PACKET_OAM_TYPE_LMR,     "LMR"    , PDU_OFFSET1, 0, 0},
    {  VTSS_PACKET_OAM_TYPE_DMM,     "DMM"    , PDU_OFFSET1, 0, 0},
    {  VTSS_PACKET_OAM_TYPE_DMR,     "DMR"    , PDU_OFFSET1, 0, 0},
    {  VTSS_PACKET_OAM_TYPE_1DM,     "1DM"    , PDU_OFFSET1, M(_1dm_frame)},
    {  VTSS_PACKET_OAM_TYPE_LTM,     "LTM"    , PDU_OFFSET1, 0, 0},
    {  VTSS_PACKET_OAM_TYPE_LTR,     "LTR"    , PDU_OFFSET1, 0, 0},
    {  VTSS_PACKET_OAM_TYPE_GENERIC, "GENERIC", PDU_OFFSET1, 0, 0}
  },
  { // --- For UpMEP injection
    {  VTSS_PACKET_OAM_TYPE_NONE,    "NONE"   , PDU_OFFSET2, 0, 0},
    {  VTSS_PACKET_OAM_TYPE_CCM,     "CCM"    , PDU_OFFSET2, 0, 0},
    {  VTSS_PACKET_OAM_TYPE_CCM_LM,  "CMM-LM" , PDU_OFFSET2, M(ccm_lm_frame_up)},
    {  VTSS_PACKET_OAM_TYPE_LBM,     "LBM"    , PDU_OFFSET2, 0, 0},
    {  VTSS_PACKET_OAM_TYPE_LBR,     "LBR"    , PDU_OFFSET2, 0, 0},
    {  VTSS_PACKET_OAM_TYPE_LMM,     "LMM"    , PDU_OFFSET2, M(lmm_frame)},
    {  VTSS_PACKET_OAM_TYPE_LMR,     "LMR"    , PDU_OFFSET2, 0, 0},
    {  VTSS_PACKET_OAM_TYPE_DMM,     "DMM"    , PDU_OFFSET2, 0, 0},
    {  VTSS_PACKET_OAM_TYPE_DMR,     "DMR"    , PDU_OFFSET2, 0, 0},
    {  VTSS_PACKET_OAM_TYPE_1DM,     "1DM"    , PDU_OFFSET2, M(_1dm_frame)},
    {  VTSS_PACKET_OAM_TYPE_LTM,     "LTM"    , PDU_OFFSET2, 0, 0},
    {  VTSS_PACKET_OAM_TYPE_LTR,     "LTR"    , PDU_OFFSET2, 0, 0},
    {  VTSS_PACKET_OAM_TYPE_GENERIC, "GENERIC", PDU_OFFSET2, 0, 0}
  }
};

#else
// VTSS_FEATURE_OAM is not defined
#endif
