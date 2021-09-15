#include "demo_api.h"


static const mesa_inst_t                  inst= NULL;

// Demo frame to be sent, vlan-tag = PCP=3, DEI=0, VID=1234
//
#define TEST_LENGTH (2*6 + 4 + 2 + 10 + 2)
static u32 test_length = TEST_LENGTH - 2;
static const  u8 test_frame[TEST_LENGTH] = { 0x00, 0x01, 0xc1, 0x00, 0x11, 0x22,  
                                             0x00, 0x01, 0xc1, 0x00, 0x11, 0x33,
                                             0x81, 0x00, 0x64, 0xd2,
                                             0x00, 0x0a,
                                             0,1,2,3,4,5,6,7,8,9,
                                             0,0};


static mesa_rc demo_packet_api_(void)
{
  mesa_rc rc;
  mesa_packet_tx_info_t info;
  //  mesa_packet_tx_ifh_t ifh;


  // (1) --- Just initialyze the info structure
  //
  rc = mesa_packet_tx_info_init(inst, &info);
  if (rc) return rc;

  
  // (2) --- Configure the 'info' structure. The 'vtss_packet_tx_info_t'
  //         type contain a lot of fields of which not all are relevant for Luton.

  // This field set the BYPASS field in the CPU Injection Header, see datasheet table 97.
  // If TRUE, then BYPASS is 0. If FALSE, then BYPASS is 1.
  //
  info.switch_frm = TRUE;


  // If the frame is not supposed to be switched, i.e. info.switch_frm = FALSE,
  // then analyzer is bypassed. Therefore we'll have to provide some information,
  // which do not matter if the frame is going to be switched.
  //
  if (!info.switch_frm) {
    info.dst_port_mask = 0x3ff;

    info.tag.tpid = 0x8100; /**< Tag Protocol Identifier */
    info.tag.pcp = 0;       /**< Priority Code Point     */
    info.tag.dei = FALSE;   /**< Drop Eligible Indicator */
    info.tag.vid = 1;       /**< VLAN Identifier         */
    
    info.cos = 0; // 0,...,7
  
    info.ptp_action = MESA_PACKET_PTP_ACTION_NONE;
    info.ptp_id = 0;
    info.ptp_timestamp = 0;
  }
  

  // (2) -- Build the IFH (Internal Frame Header) from the 'info' information.
  //
//  rc = mesa_packet_tx_hdr_compile(inst, &info, &ifh);
//  if (rc) return rc;

  // (3) --- Transmit frame
  //
  rc = mesa_packet_tx_frame(inst, &info, test_frame, test_length);
  if (rc) return rc;

  return rc;
}


void demo_packet_api(void)
{
  mesa_rc rc;

  rc = demo_packet_api_();
  if (rc) printf("Error: rc=%d\n", rc);
}  

// The function below can also be used to send frames and will basically
// used the methods used in the example about, but just wrapped.
//
#if 0
  mesa_rc vtss_packet_tx_frame_vlan(const mesa_inst_t  inst,
                                  const vtss_vid_t   vid,
                                  const u8           *const frame,
                                  const u32          length);

  mesa_rc vtss_packet_tx_frame_port_vlan(const mesa_inst_t     inst,
                                       const vtss_port_no_t  port_no,
                                       const vtss_vid_t      vid,
                                       const u8              *const frame,
                                       const u32             length);

  mesa_rc vtss_packet_tx_frame_port(const mesa_inst_t     inst,
                                  const vtss_port_no_t  port_no,
                                  const u8              *const frame,
                                  const u32             length);

  mesa_rc vtss_packet_tx_hdr_encode(const mesa_inst_t                  inst,
                                  const vtss_packet_tx_info_t *const info,
                                        u8                    *const bin_hdr,
                                        u32                   *const bin_hdr_len);

  mesa_rc vtss_packet_tx_hdr_compile(const mesa_inst_t                  inst,
                                   const vtss_packet_tx_info_t *const info,
                                         vtss_packet_tx_ifh_t  *const ifh);
#endif
