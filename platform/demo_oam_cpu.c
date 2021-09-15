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

static const vtss_inst_t inst=NULL;



static const  u8 test_frame[2*6 + 4 + 2 + 10 + 2] = { 0x00, 0x01, 0xc1, 0x00, 0x11, 0x22,  
                                             0x00, 0x01, 0xc1, 0x00, 0x11, 0x33,
                                             0x81, 0x00, 0x64, 0xd2,
                                             0x00, 0x0a,
                                             0,1,2,3,4,5,6,7,8,9,
                                             0,0};
static const  u8 test_frame2[2*6  + 2 + 10 + 2] = { 0x00, 0x01, 0xc1, 0x00, 0x11, 0x22,  
                                             0x00, 0x01, 0xc1, 0x00, 0x11, 0x33,
                                             0x00, 0x0a,
                                             0,1,2,3,4,5,6,7,8,9,
                                             0,0};

static u32 test_length = 2*6 + 4 + 2 + 10;
static u32 test_length2 = 2*6 + 2 + 10;

extern void demo_dump_packet(int len, const u8 *p);


vtss_rc demo_tx_LBM(vtss_isdx_t isdx)
{
  vtss_rc rc;

  u8                   frame;
  u32                  length;

  vtss_packet_tx_info_t info;
  vtss_packet_tx_ifh_t ifh;


  // (1) --- Build the IFH header from 'info'
  //
  bzero(&info, sizeof(info));

  info.oam_type = VTSS_PACKET_OAM_TYPE_NONE;//VTSS_PACKET_OAM_TYPE_LBM;
  info.isdx = isdx;
  info.isdx_dont_use = FALSE;

  info.tag.tpid = 0x8100;
  info.tag.pcp  = 3;
  info.tag.dei  = FALSE;
  info.tag.vid  = 1234;

  info.masquerade_port = VTSS_PORT_NO_NONE;
  info.dst_port_mask = 0x3ff;
  info.afi_id = VTSS_AFI_ID_NONE;


  bzero(&ifh, sizeof(ifh));
  ifh.length = VTSS_PACKET_TX_IFH_MAX;
 
  rc = vtss_packet_tx_hdr_encode(inst, &info, (u8*)&ifh.ifh[0], &ifh.length);
  if (rc) return rc;

  printf("IFH:\n");
  demo_dump_packet(ifh.length, (u8*)&ifh.ifh[0]);

  printf("FRAME:\n");
  demo_dump_packet(test_length, test_frame);


  rc = vtss_packet_tx_frame(inst, &ifh, test_frame, test_length);
  if (rc) return rc;

  return rc;
}

vtss_rc demo_tx_LBM2(vtss_isdx_t isdx)
{
  vtss_rc rc;

  rc = vtss_packet_tx_frame_port(inst, 0, test_frame2, test_length2);

  return rc;
}


void demo_oam_tx()
{
  vtss_rc rc;

  rc = demo_tx_LBM2(514);
  if (rc) printf("Error: demo_tx_LBM rc=%d\n", rc);
}

#if 0
typedef struct {
  BOOL  switch_frm;
  u64   dst_port_mask;
  vtss_vlan_tag_t tag;
  vtss_prio_t     cos;

  vtss_packet_ptp_action_t ptp_action;
  u8 ptp_id;
  u32 ptp_timestamp;

  vtss_packet_oam_type_t oam_type;
  vtss_isdx_t isdx;
  BOOL isdx_dont_use;
  vtss_dp_level_t dp;
  vtss_port_no_t masquerade_port;

  // No used in serval-1
  u32 frm_len;
  u8 aggr_code;
  vtss_cosid_t cosid;
  u16 vsi;
  u32 latch_timestamp;
  u32 pdu_offset;

  vtss_afi_id_t afi_id; //VTSS_AFI_ID_NONE

} vtss_packet_tx_info_t;
#endif

void demo_dump_packet(int len, const u8 *p)
{
    int i;
#define BLINE 14
    for (i = 0; i < (len < BLINE ? len : BLINE); ++i) {
        printf("%2.2x ", p[i]);
    }

    for (i = BLINE; i < len; ++i) {
        if ( (i - BLINE) % 16 == 0 ) {
            printf("\n");
        }
        printf("%2.2x ", p[i]);
    }
    printf("\n");
}

extern int torben_tx;


extern "C" {
void demo_vtss_packet_tx_frame(const vtss_packet_tx_ifh_t *const ifh,
                               const u8                   *const frame,
                               const u32                   length)
{
  printf("IFH:\n");
  demo_dump_packet(ifh->length, (u8*)&ifh->ifh[0]);

  printf("FRAME:\n");
  demo_dump_packet(length, frame);
}
}
