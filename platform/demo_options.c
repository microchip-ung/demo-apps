#include <stdio.h>
#include <vtss/api/options.h>
#include "demo_api.h"
void demo_options(void) {
#ifdef VTSS_OPT_FDMA_VER
   printf(" VTSS_OPT_FDMA_VER = %d\n", VTSS_OPT_FDMA_VER);
#else
   printf("!VTSS_OPT_FDMA_VER\n");
#endif
#ifdef VTSS_AFI_V1
   printf(" VTSS_AFI_V1 \n");
#endif
#ifdef VTSS_AFI_V2
   printf(" VTSS_AFI_V2 \n");
#endif
#ifdef VTSS_ARCH_CARACAL
   printf(" VTSS_ARCH_CARACAL \n");
#endif
#ifdef VTSS_ARCH_DAYTONA
   printf(" VTSS_ARCH_DAYTONA \n");
#endif
#ifdef VTSS_ARCH_JAGUAR_2
   printf(" VTSS_ARCH_JAGUAR_2 \n");
#endif
#ifdef VTSS_ARCH_JAGUAR_2_B
   printf(" VTSS_ARCH_JAGUAR_2_B \n");
#endif
#ifdef VTSS_ARCH_JAGUAR_2_C
   printf(" VTSS_ARCH_JAGUAR_2_C \n");
#endif
#ifdef VTSS_ARCH_JAGUAR_2_CE
   printf(" VTSS_ARCH_JAGUAR_2_CE \n");
#endif
#ifdef VTSS_ARCH_LEDA
   printf(" VTSS_ARCH_LEDA \n");
#endif
#ifdef VTSS_ARCH_LUTON26
   printf(" VTSS_ARCH_LUTON26 \n");
#endif
#ifdef VTSS_ARCH_SERVAL
   printf(" VTSS_ARCH_SERVAL \n");
#endif
#ifdef VTSS_ARCH_SERVAL_CE
   printf(" VTSS_ARCH_SERVAL_CE \n");
#endif
#ifdef VTSS_ARCH_SERVAL_CPU
   printf(" VTSS_ARCH_SERVAL_CPU \n");
#endif
#ifdef VTSS_ARCH_SERVAL_SME
   printf(" VTSS_ARCH_SERVAL_SME \n");
#endif
#ifdef VTSS_ARCH_SERVAL_T
   printf(" VTSS_ARCH_SERVAL_T \n");
#endif
#ifdef VTSS_ARCH_SEVILLE
   printf(" VTSS_ARCH_SEVILLE \n");
#endif
#ifdef VTSS_ARCH_SEVILLE_2
   printf(" VTSS_ARCH_SEVILLE_2 \n");
#endif
#ifdef VTSS_ARCH_SPARX_III
   printf(" VTSS_ARCH_SPARX_III \n");
#endif
#ifdef VTSS_CHIP_10G_PHY
   printf(" VTSS_CHIP_10G_PHY \n");
#endif
#ifdef VTSS_CHIP_CU_PHY
   printf(" VTSS_CHIP_CU_PHY \n");
#endif
#ifdef VTSS_DAYTONA_CHANNELS
   printf(" VTSS_DAYTONA_CHANNELS \n");
#endif
#ifdef VTSS_FEATURE_10G
   printf(" VTSS_FEATURE_10G \n");
#endif
#ifdef VTSS_FEATURE_10GBASE_KR
   printf(" VTSS_FEATURE_10GBASE_KR \n");
#endif
#ifdef VTSS_FEATURE_ACL
   printf(" VTSS_FEATURE_ACL \n");
#endif
#ifdef VTSS_FEATURE_ACL_V2
   printf(" VTSS_FEATURE_ACL_V2 \n");
#endif
#ifdef VTSS_FEATURE_AFI_FDMA
   printf(" VTSS_FEATURE_AFI_FDMA \n");
#endif
#ifdef VTSS_FEATURE_AFI_SWC
   printf(" VTSS_FEATURE_AFI_SWC \n");
#endif
#ifdef VTSS_FEATURE_AGGR_GLAG
   printf(" VTSS_FEATURE_AGGR_GLAG \n");
#endif
#ifdef VTSS_FEATURE_CLAUSE_37
   printf(" VTSS_FEATURE_CLAUSE_37 \n");
#endif
#ifdef VTSS_FEATURE_CLOCK
   printf(" VTSS_FEATURE_CLOCK \n");
#endif
#ifdef VTSS_FEATURE_DELAY_REQ_AUTO_RESP
   printf(" VTSS_FEATURE_DELAY_REQ_AUTO_RESP \n");
#endif
#ifdef VTSS_FEATURE_ECE_ACT_DEI_MODE
   printf(" VTSS_FEATURE_ECE_ACT_DEI_MODE \n");
#endif
#ifdef VTSS_FEATURE_ECE_ACT_DP
   printf(" VTSS_FEATURE_ECE_ACT_DP \n");
#endif
#ifdef VTSS_FEATURE_ECE_ACT_INNER_TAG
   printf(" VTSS_FEATURE_ECE_ACT_INNER_TAG \n");
#endif
#ifdef VTSS_FEATURE_ECE_ACT_OT_VID
   printf(" VTSS_FEATURE_ECE_ACT_OT_VID \n");
#endif
#ifdef VTSS_FEATURE_ECE_ACT_OUTER_TAG
   printf(" VTSS_FEATURE_ECE_ACT_OUTER_TAG \n");
#endif
#ifdef VTSS_FEATURE_ECE_ACT_PCP_DEI_PRES
   printf(" VTSS_FEATURE_ECE_ACT_PCP_DEI_PRES \n");
#endif
#ifdef VTSS_FEATURE_ECE_ACT_PCP_MODE
   printf(" VTSS_FEATURE_ECE_ACT_PCP_MODE \n");
#endif
#ifdef VTSS_FEATURE_ECE_ACT_POLICER
   printf(" VTSS_FEATURE_ECE_ACT_POLICER \n");
#endif
#ifdef VTSS_FEATURE_ECE_ACT_PRIO
   printf(" VTSS_FEATURE_ECE_ACT_PRIO \n");
#endif
#ifdef VTSS_FEATURE_ECE_ACT_RULE
   printf(" VTSS_FEATURE_ECE_ACT_RULE \n");
#endif
#ifdef VTSS_FEATURE_ECE_ACT_TX_LOOKUP
   printf(" VTSS_FEATURE_ECE_ACT_TX_LOOKUP \n");
#endif
#ifdef VTSS_FEATURE_ECE_KEY_DIP
   printf(" VTSS_FEATURE_ECE_KEY_DIP \n");
#endif
#ifdef VTSS_FEATURE_ECE_KEY_DMAC
   printf(" VTSS_FEATURE_ECE_KEY_DMAC \n");
#endif
#ifdef VTSS_FEATURE_ECE_KEY_DMAC_TYPE
   printf(" VTSS_FEATURE_ECE_KEY_DMAC_TYPE \n");
#endif
#ifdef VTSS_FEATURE_ECE_KEY_DPORT
   printf(" VTSS_FEATURE_ECE_KEY_DPORT \n");
#endif
#ifdef VTSS_FEATURE_ECE_KEY_ETYPE
   printf(" VTSS_FEATURE_ECE_KEY_ETYPE \n");
#endif
#ifdef VTSS_FEATURE_ECE_KEY_INNER_TAG
   printf(" VTSS_FEATURE_ECE_KEY_INNER_TAG \n");
#endif
#ifdef VTSS_FEATURE_ECE_KEY_IP_PROTO
   printf(" VTSS_FEATURE_ECE_KEY_IP_PROTO \n");
#endif
#ifdef VTSS_FEATURE_ECE_KEY_LLC
   printf(" VTSS_FEATURE_ECE_KEY_LLC \n");
#endif
#ifdef VTSS_FEATURE_ECE_KEY_SIP
   printf(" VTSS_FEATURE_ECE_KEY_SIP \n");
#endif
#ifdef VTSS_FEATURE_ECE_KEY_SMAC
   printf(" VTSS_FEATURE_ECE_KEY_SMAC \n");
#endif
#ifdef VTSS_FEATURE_ECE_KEY_SNAP
   printf(" VTSS_FEATURE_ECE_KEY_SNAP \n");
#endif
#ifdef VTSS_FEATURE_ECE_KEY_SPORT
   printf(" VTSS_FEATURE_ECE_KEY_SPORT \n");
#endif
#ifdef VTSS_FEATURE_EDC_FW_LOAD
   printf(" VTSS_FEATURE_EDC_FW_LOAD \n");
#endif
#ifdef VTSS_FEATURE_EEE
   printf(" VTSS_FEATURE_EEE \n");
#endif
#ifdef VTSS_FEATURE_EVC
   printf(" VTSS_FEATURE_EVC \n");
#endif
#ifdef VTSS_FEATURE_EVC_COSID_COUNTERS
   printf(" VTSS_FEATURE_EVC_COSID_COUNTERS \n");
#endif
#ifdef VTSS_FEATURE_EVC_COSID_POLICERS
   printf(" VTSS_FEATURE_EVC_COSID_POLICERS \n");
#endif
#ifdef VTSS_FEATURE_EVC_COUNTERS
   printf(" VTSS_FEATURE_EVC_COUNTERS \n");
#endif
#ifdef VTSS_FEATURE_EVC_POLICERS
   printf(" VTSS_FEATURE_EVC_POLICERS \n");
#endif
#ifdef VTSS_FEATURE_EVC_PORT
   printf(" VTSS_FEATURE_EVC_PORT \n");
#endif
#ifdef VTSS_FEATURE_EVC_PORT_EVC
   printf(" VTSS_FEATURE_EVC_PORT_EVC \n");
#endif
#ifdef VTSS_FEATURE_EXC_COL_CONT
   printf(" VTSS_FEATURE_EXC_COL_CONT \n");
#endif
#ifdef VTSS_FEATURE_E_TREE
   printf(" VTSS_FEATURE_E_TREE \n");
#endif
#ifdef VTSS_FEATURE_F2DF
   printf(" VTSS_FEATURE_F2DF \n");
#endif
#ifdef VTSS_FEATURE_FAN
   printf(" VTSS_FEATURE_FAN \n");
#endif
#ifdef VTSS_FEATURE_FDMA
   printf(" VTSS_FEATURE_FDMA \n");
#endif
#ifdef VTSS_FEATURE_GFP
   printf(" VTSS_FEATURE_GFP \n");
#endif
#ifdef VTSS_FEATURE_HQOS
   printf(" VTSS_FEATURE_HQOS \n");
#endif
#ifdef VTSS_FEATURE_I2C
   printf(" VTSS_FEATURE_I2C \n");
#endif
#ifdef VTSS_FEATURE_INTERRUPTS
   printf(" VTSS_FEATURE_INTERRUPTS \n");
#endif
#ifdef VTSS_FEATURE_IPV4_MC_SIP
   printf(" VTSS_FEATURE_IPV4_MC_SIP \n");
#endif
#ifdef VTSS_FEATURE_IPV6_MC_SIP
   printf(" VTSS_FEATURE_IPV6_MC_SIP \n");
#endif
#ifdef VTSS_FEATURE_IRQ_CONTROL
   printf(" VTSS_FEATURE_IRQ_CONTROL \n");
#endif
#ifdef VTSS_FEATURE_LAYER2
   printf(" VTSS_FEATURE_LAYER2 \n");
#endif
#ifdef VTSS_FEATURE_LED_POW_REDUC
   printf(" VTSS_FEATURE_LED_POW_REDUC \n");
#endif
#ifdef VTSS_FEATURE_MAC10G
   printf(" VTSS_FEATURE_MAC10G \n");
#endif
#ifdef VTSS_FEATURE_MAC_AGE_AUTO
   printf(" VTSS_FEATURE_MAC_AGE_AUTO \n");
#endif
#ifdef VTSS_FEATURE_MAC_CPU_QUEUE
   printf(" VTSS_FEATURE_MAC_CPU_QUEUE \n");
#endif
#ifdef VTSS_FEATURE_MCE_ACT_PRIO
   printf(" VTSS_FEATURE_MCE_ACT_PRIO \n");
#endif
#ifdef VTSS_FEATURE_MIRROR_CPU
   printf(" VTSS_FEATURE_MIRROR_CPU \n");
#endif
#ifdef VTSS_FEATURE_MISC
   printf(" VTSS_FEATURE_MISC \n");
#endif
#ifdef VTSS_FEATURE_MPLS
   printf(" VTSS_FEATURE_MPLS \n");
#endif
#ifdef VTSS_FEATURE_NPI
   printf(" VTSS_FEATURE_NPI \n");
#endif
#ifdef VTSS_FEATURE_OAM
   printf(" VTSS_FEATURE_OAM \n");
#endif
#ifdef VTSS_FEATURE_OHA
   printf(" VTSS_FEATURE_OHA \n");
#endif
#ifdef VTSS_FEATURE_OTN
   printf(" VTSS_FEATURE_OTN \n");
#endif
#ifdef VTSS_FEATURE_PACKET
   printf(" VTSS_FEATURE_PACKET \n");
#endif
#ifdef VTSS_FEATURE_PACKET_GROUPING
   printf(" VTSS_FEATURE_PACKET_GROUPING \n");
#endif
#ifdef VTSS_FEATURE_PACKET_PIPELINE_PT
   printf(" VTSS_FEATURE_PACKET_PIPELINE_PT \n");
#endif
#ifdef VTSS_FEATURE_PACKET_PORT_L2CP_REG
   printf(" VTSS_FEATURE_PACKET_PORT_L2CP_REG \n");
#endif
#ifdef VTSS_FEATURE_PACKET_PORT_REG
   printf(" VTSS_FEATURE_PACKET_PORT_REG \n");
#endif
#ifdef VTSS_FEATURE_PACKET_PORT_REG_DISCARD
   printf(" VTSS_FEATURE_PACKET_PORT_REG_DISCARD \n");
#endif
#ifdef VTSS_FEATURE_PACKET_RX
   printf(" VTSS_FEATURE_PACKET_RX \n");
#endif
#ifdef VTSS_FEATURE_PACKET_TX
   printf(" VTSS_FEATURE_PACKET_TX \n");
#endif
#ifdef VTSS_FEATURE_PCS_10GBASE_R
   printf(" VTSS_FEATURE_PCS_10GBASE_R \n");
#endif
#ifdef VTSS_FEATURE_PFC
   printf(" VTSS_FEATURE_PFC \n");
#endif
#ifdef VTSS_FEATURE_PHY_TIMESTAMP
   printf(" VTSS_FEATURE_PHY_TIMESTAMP \n");
#endif
#ifdef VTSS_FEATURE_PORT_CNT_BRIDGE
   printf(" VTSS_FEATURE_PORT_CNT_BRIDGE \n");
#endif
#ifdef VTSS_FEATURE_PORT_CNT_ETHER_LIKE
   printf(" VTSS_FEATURE_PORT_CNT_ETHER_LIKE \n");
#endif
#ifdef VTSS_FEATURE_PORT_CONTROL
   printf(" VTSS_FEATURE_PORT_CONTROL \n");
#endif
#ifdef VTSS_FEATURE_PORT_IFH
   printf(" VTSS_FEATURE_PORT_IFH \n");
#endif
#ifdef VTSS_FEATURE_PORT_MUX
   printf(" VTSS_FEATURE_PORT_MUX \n");
#endif
#ifdef VTSS_FEATURE_PTP_RS422
   printf(" VTSS_FEATURE_PTP_RS422 \n");
#endif
#ifdef VTSS_FEATURE_PVLAN
   printf(" VTSS_FEATURE_PVLAN \n");
#endif
#ifdef VTSS_FEATURE_QCL
   printf(" VTSS_FEATURE_QCL \n");
#endif
#ifdef VTSS_FEATURE_QCL_DMAC_DIP
   printf(" VTSS_FEATURE_QCL_DMAC_DIP \n");
#endif
#ifdef VTSS_FEATURE_QCL_KEY_DIP
   printf(" VTSS_FEATURE_QCL_KEY_DIP \n");
#endif
#ifdef VTSS_FEATURE_QCL_KEY_DMAC
   printf(" VTSS_FEATURE_QCL_KEY_DMAC \n");
#endif
#ifdef VTSS_FEATURE_QCL_KEY_INNER_TAG
   printf(" VTSS_FEATURE_QCL_KEY_INNER_TAG \n");
#endif
#ifdef VTSS_FEATURE_QCL_KEY_S_TAG
   printf(" VTSS_FEATURE_QCL_KEY_S_TAG \n");
#endif
#ifdef VTSS_FEATURE_QCL_KEY_TYPE
   printf(" VTSS_FEATURE_QCL_KEY_TYPE \n");
#endif
#ifdef VTSS_FEATURE_QCL_MAP_ACTION
   printf(" VTSS_FEATURE_QCL_MAP_ACTION \n");
#endif
#ifdef VTSS_FEATURE_QCL_PCP_DEI_ACTION
   printf(" VTSS_FEATURE_QCL_PCP_DEI_ACTION \n");
#endif
#ifdef VTSS_FEATURE_QCL_POLICY_ACTION
   printf(" VTSS_FEATURE_QCL_POLICY_ACTION \n");
#endif
#ifdef VTSS_FEATURE_QCL_V2
   printf(" VTSS_FEATURE_QCL_V2 \n");
#endif
#ifdef VTSS_FEATURE_QOS
   printf(" VTSS_FEATURE_QOS \n");
#endif
#ifdef VTSS_FEATURE_QOS_CLASSIFICATION_V2
   printf(" VTSS_FEATURE_QOS_CLASSIFICATION_V2 \n");
#endif
#ifdef VTSS_FEATURE_QOS_COSID_CLASSIFICATION
   printf(" VTSS_FEATURE_QOS_COSID_CLASSIFICATION \n");
#endif
#ifdef VTSS_FEATURE_QOS_CPU_PORT_SHAPER
   printf(" VTSS_FEATURE_QOS_CPU_PORT_SHAPER \n");
#endif
#ifdef VTSS_FEATURE_QOS_DSCP_CLASS_DP_AWARE
   printf(" VTSS_FEATURE_QOS_DSCP_CLASS_DP_AWARE \n");
#endif
#ifdef VTSS_FEATURE_QOS_DSCP_REMARK
   printf(" VTSS_FEATURE_QOS_DSCP_REMARK \n");
#endif
#ifdef VTSS_FEATURE_QOS_DSCP_REMARK_DP_AWARE
   printf(" VTSS_FEATURE_QOS_DSCP_REMARK_DP_AWARE \n");
#endif
#ifdef VTSS_FEATURE_QOS_DSCP_REMARK_V2
   printf(" VTSS_FEATURE_QOS_DSCP_REMARK_V2 \n");
#endif
#ifdef VTSS_FEATURE_QOS_EGRESS_MAP
   printf(" VTSS_FEATURE_QOS_EGRESS_MAP \n");
#endif
#ifdef VTSS_FEATURE_QOS_EGRESS_QUEUE_SHAPERS
   printf(" VTSS_FEATURE_QOS_EGRESS_QUEUE_SHAPERS \n");
#endif
#ifdef VTSS_FEATURE_QOS_EGRESS_QUEUE_SHAPERS_EB
   printf(" VTSS_FEATURE_QOS_EGRESS_QUEUE_SHAPERS_EB \n");
#endif
#ifdef VTSS_FEATURE_QOS_EGRESS_SHAPERS_DLB
   printf(" VTSS_FEATURE_QOS_EGRESS_SHAPERS_DLB \n");
#endif
#ifdef VTSS_FEATURE_QOS_EGRESS_SHAPERS_RT
   printf(" VTSS_FEATURE_QOS_EGRESS_SHAPERS_RT \n");
#endif
#ifdef VTSS_FEATURE_QOS_HSCH_LEAK_LISTS
   printf(" VTSS_FEATURE_QOS_HSCH_LEAK_LISTS \n");
#endif
#ifdef VTSS_FEATURE_QOS_INGRESS_MAP
   printf(" VTSS_FEATURE_QOS_INGRESS_MAP \n");
#endif
#ifdef VTSS_FEATURE_QOS_POLICER_BC_SWITCH
   printf(" VTSS_FEATURE_QOS_POLICER_BC_SWITCH \n");
#endif
#ifdef VTSS_FEATURE_QOS_POLICER_DLB
   printf(" VTSS_FEATURE_QOS_POLICER_DLB \n");
#endif
#ifdef VTSS_FEATURE_QOS_POLICER_MC_SWITCH
   printf(" VTSS_FEATURE_QOS_POLICER_MC_SWITCH \n");
#endif
#ifdef VTSS_FEATURE_QOS_POLICER_UC_SWITCH
   printf(" VTSS_FEATURE_QOS_POLICER_UC_SWITCH \n");
#endif
#ifdef VTSS_FEATURE_QOS_PORT_POLICER_EXT
   printf(" VTSS_FEATURE_QOS_PORT_POLICER_EXT \n");
#endif
#ifdef VTSS_FEATURE_QOS_PORT_POLICER_EXT_DPBL
   printf(" VTSS_FEATURE_QOS_PORT_POLICER_EXT_DPBL \n");
#endif
#ifdef VTSS_FEATURE_QOS_PORT_POLICER_EXT_FC
   printf(" VTSS_FEATURE_QOS_PORT_POLICER_EXT_FC \n");
#endif
#ifdef VTSS_FEATURE_QOS_PORT_POLICER_EXT_FPS
   printf(" VTSS_FEATURE_QOS_PORT_POLICER_EXT_FPS \n");
#endif
#ifdef VTSS_FEATURE_QOS_PORT_POLICER_EXT_TTM_V2
   printf(" VTSS_FEATURE_QOS_PORT_POLICER_EXT_TTM_V2 \n");
#endif
#ifdef VTSS_FEATURE_QOS_QUEUE_POLICER
   printf(" VTSS_FEATURE_QOS_QUEUE_POLICER \n");
#endif
#ifdef VTSS_FEATURE_QOS_QUEUE_TX
   printf(" VTSS_FEATURE_QOS_QUEUE_TX \n");
#endif
#ifdef VTSS_FEATURE_QOS_SCHEDULER_DWRR_CNT
   printf(" VTSS_FEATURE_QOS_SCHEDULER_DWRR_CNT \n");
#endif
#ifdef VTSS_FEATURE_QOS_SCHEDULER_V2
   printf(" VTSS_FEATURE_QOS_SCHEDULER_V2 \n");
#endif
#ifdef VTSS_FEATURE_QOS_TAG_REMARK_V2
   printf(" VTSS_FEATURE_QOS_TAG_REMARK_V2 \n");
#endif
#ifdef VTSS_FEATURE_QOS_WRED_V2
   printf(" VTSS_FEATURE_QOS_WRED_V2 \n");
#endif
#ifdef VTSS_FEATURE_QOS_WRED_V3
   printf(" VTSS_FEATURE_QOS_WRED_V3 \n");
#endif
#ifdef VTSS_FEATURE_RAB
   printf(" VTSS_FEATURE_RAB \n");
#endif
#ifdef VTSS_FEATURE_SD10G65
   printf(" VTSS_FEATURE_SD10G65 \n");
#endif
#ifdef VTSS_FEATURE_SERDES_MACRO_SETTINGS
   printf(" VTSS_FEATURE_SERDES_MACRO_SETTINGS \n");
#endif
#ifdef VTSS_FEATURE_SERIAL_GPIO
   printf(" VTSS_FEATURE_SERIAL_GPIO \n");
#endif
#ifdef VTSS_FEATURE_SFLOW
   printf(" VTSS_FEATURE_SFLOW \n");
#endif
#ifdef VTSS_FEATURE_SYNCE
   printf(" VTSS_FEATURE_SYNCE \n");
#endif
#ifdef VTSS_FEATURE_SYNCE_10G
   printf(" VTSS_FEATURE_SYNCE_10G \n");
#endif
#ifdef VTSS_FEATURE_SYNC_ANN_AUTO_TRANSMIT
   printf(" VTSS_FEATURE_SYNC_ANN_AUTO_TRANSMIT \n");
#endif
#ifdef VTSS_FEATURE_TEMP_SENSOR
   printf(" VTSS_FEATURE_TEMP_SENSOR \n");
#endif
#ifdef VTSS_FEATURE_TIMESTAMP
   printf(" VTSS_FEATURE_TIMESTAMP \n");
#endif
#ifdef VTSS_FEATURE_TIMESTAMP_ASYMMETRY_COMP
   printf(" VTSS_FEATURE_TIMESTAMP_ASYMMETRY_COMP \n");
#endif
#ifdef VTSS_FEATURE_TIMESTAMP_LATENCY_COMP
   printf(" VTSS_FEATURE_TIMESTAMP_LATENCY_COMP \n");
#endif
#ifdef VTSS_FEATURE_TIMESTAMP_ONE_STEP
   printf(" VTSS_FEATURE_TIMESTAMP_ONE_STEP \n");
#endif
#ifdef VTSS_FEATURE_TIMESTAMP_ORG_TIME
   printf(" VTSS_FEATURE_TIMESTAMP_ORG_TIME \n");
#endif
#ifdef VTSS_FEATURE_TIMESTAMP_P2P_DELAY_COMP
   printf(" VTSS_FEATURE_TIMESTAMP_P2P_DELAY_COMP \n");
#endif
#ifdef VTSS_FEATURE_UPI
   printf(" VTSS_FEATURE_UPI \n");
#endif
#ifdef VTSS_FEATURE_VCAP
   printf(" VTSS_FEATURE_VCAP \n");
#endif
#ifdef VTSS_FEATURE_VCL
   printf(" VTSS_FEATURE_VCL \n");
#endif
#ifdef VTSS_FEATURE_VLAN_COUNTERS
   printf(" VTSS_FEATURE_VLAN_COUNTERS \n");
#endif
#ifdef VTSS_FEATURE_VLAN_PORT_V2
   printf(" VTSS_FEATURE_VLAN_PORT_V2 \n");
#endif
#ifdef VTSS_FEATURE_VLAN_SVL
   printf(" VTSS_FEATURE_VLAN_SVL \n");
#endif
#ifdef VTSS_FEATURE_VLAN_TRANSLATION
   printf(" VTSS_FEATURE_VLAN_TRANSLATION \n");
#endif
#ifdef VTSS_FEATURE_VLAN_TX_TAG
   printf(" VTSS_FEATURE_VLAN_TX_TAG \n");
#endif
#ifdef VTSS_FEATURE_VSTAX
   printf(" VTSS_FEATURE_VSTAX \n");
#endif
#ifdef VTSS_FEATURE_VSTAX_V2
   printf(" VTSS_FEATURE_VSTAX_V2 \n");
#endif
#ifdef VTSS_FEATURE_WARM_START
   printf(" VTSS_FEATURE_WARM_START \n");
#endif
#ifdef VTSS_FEATURE_WIS
   printf(" VTSS_FEATURE_WIS \n");
#endif
#ifdef VTSS_FEATURE_XAUI
   printf(" VTSS_FEATURE_XAUI \n");
#endif
#ifdef VTSS_FEATURE_XFI
   printf(" VTSS_FEATURE_XFI \n");
#endif
#ifdef VTSS_LEDA_CHANNELS
   printf(" VTSS_LEDA_CHANNELS \n");
#endif
#ifdef VTSS_LEDA_INIT_INTERFACE
   printf(" VTSS_LEDA_INIT_INTERFACE \n");
#endif
#ifdef VTSS_OPT_AFI_OPTIMIZE_FOR_TIMERS
   printf(" VTSS_OPT_AFI_OPTIMIZE_FOR_TIMERS \n");
#endif
#ifdef VTSS_OPT_FDMA
   printf(" VTSS_OPT_FDMA = %d\n", VTSS_OPT_FDMA);
#endif
#ifdef VTSS_OPT_FDMA_DEBUG
   printf(" VTSS_OPT_FDMA_DEBUG \n");
#endif
#ifdef VTSS_OPT_FDMA_IRQ_CONTEXT
   printf(" VTSS_OPT_FDMA_IRQ_CONTEXT \n");
#endif
#ifdef VTSS_OPT_FDMA_VER
   printf(" VTSS_OPT_FDMA_VER \n");
#endif
#ifdef VTSS_OPT_PCIE_ACCESS
   printf(" VTSS_OPT_PCIE_ACCESS \n");
#endif
#ifdef VTSS_OPT_PORT_COUNT
   printf(" VTSS_OPT_PORT_COUNT \n");
#endif
#ifdef VTSS_OPT_TRACE
   printf(" VTSS_OPT_TRACE \n");
#endif
#ifdef VTSS_OPT_VAUI_EQ_CTRL
   printf(" VTSS_OPT_VAUI_EQ_CTRL \n");
#endif
#ifdef VTSS_OPT_VCORE_III
   printf(" VTSS_OPT_VCORE_III \n");
#endif
#ifdef VTSS_PHY_OPT_VERIPHY
   printf(" VTSS_PHY_OPT_VERIPHY \n");
#endif
#ifdef _VTSS_OPTIONS_H_
   printf(" _VTSS_OPTIONS_H_ \n");
#endif
#ifndef VTSS_AFI_V1
   printf("! VTSS_AFI_V1 \n");
#endif
#ifndef VTSS_AFI_V2
   printf("! VTSS_AFI_V2 \n");
#endif
#ifndef VTSS_ARCH_CARACAL
   printf("! VTSS_ARCH_CARACAL \n");
#endif
#ifndef VTSS_ARCH_DAYTONA
   printf("! VTSS_ARCH_DAYTONA \n");
#endif
#ifndef VTSS_ARCH_JAGUAR_2
   printf("! VTSS_ARCH_JAGUAR_2 \n");
#endif
#ifndef VTSS_ARCH_JAGUAR_2_B
   printf("! VTSS_ARCH_JAGUAR_2_B \n");
#endif
#ifndef VTSS_ARCH_JAGUAR_2_C
   printf("! VTSS_ARCH_JAGUAR_2_C \n");
#endif
#ifndef VTSS_ARCH_JAGUAR_2_CE
   printf("! VTSS_ARCH_JAGUAR_2_CE \n");
#endif
#ifndef VTSS_ARCH_LEDA
   printf("! VTSS_ARCH_LEDA \n");
#endif
#ifndef VTSS_ARCH_LUTON26
   printf("! VTSS_ARCH_LUTON26 \n");
#endif
#ifndef VTSS_ARCH_SERVAL
   printf("! VTSS_ARCH_SERVAL \n");
#endif
#ifndef VTSS_ARCH_SERVAL_CE
   printf("! VTSS_ARCH_SERVAL_CE \n");
#endif
#ifndef VTSS_ARCH_SERVAL_CPU
   printf("! VTSS_ARCH_SERVAL_CPU \n");
#endif
#ifndef VTSS_ARCH_SERVAL_SME
   printf("! VTSS_ARCH_SERVAL_SME \n");
#endif
#ifndef VTSS_ARCH_SERVAL_T
   printf("! VTSS_ARCH_SERVAL_T \n");
#endif
#ifndef VTSS_ARCH_SEVILLE
   printf("! VTSS_ARCH_SEVILLE \n");
#endif
#ifndef VTSS_ARCH_SEVILLE_2
   printf("! VTSS_ARCH_SEVILLE_2 \n");
#endif
#ifndef VTSS_ARCH_SPARX_III
   printf("! VTSS_ARCH_SPARX_III \n");
#endif
#ifndef VTSS_CHIP_10G_PHY
   printf("! VTSS_CHIP_10G_PHY \n");
#endif
#ifndef VTSS_CHIP_CU_PHY
   printf("! VTSS_CHIP_CU_PHY \n");
#endif
#ifndef VTSS_DAYTONA_CHANNELS
   printf("! VTSS_DAYTONA_CHANNELS \n");
#endif
#ifndef VTSS_FEATURE_10G
   printf("! VTSS_FEATURE_10G \n");
#endif
#ifndef VTSS_FEATURE_10GBASE_KR
   printf("! VTSS_FEATURE_10GBASE_KR \n");
#endif
#ifndef VTSS_FEATURE_ACL
   printf("! VTSS_FEATURE_ACL \n");
#endif
#ifndef VTSS_FEATURE_ACL_V2
   printf("! VTSS_FEATURE_ACL_V2 \n");
#endif
#ifndef VTSS_FEATURE_AFI_FDMA
   printf("! VTSS_FEATURE_AFI_FDMA \n");
#endif
#ifndef VTSS_FEATURE_AFI_SWC
   printf("! VTSS_FEATURE_AFI_SWC \n");
#endif
#ifndef VTSS_FEATURE_AGGR_GLAG
   printf("! VTSS_FEATURE_AGGR_GLAG \n");
#endif
#ifndef VTSS_FEATURE_CLAUSE_37
   printf("! VTSS_FEATURE_CLAUSE_37 \n");
#endif
#ifndef VTSS_FEATURE_CLOCK
   printf("! VTSS_FEATURE_CLOCK \n");
#endif
#ifndef VTSS_FEATURE_DELAY_REQ_AUTO_RESP
   printf("! VTSS_FEATURE_DELAY_REQ_AUTO_RESP \n");
#endif
#ifndef VTSS_FEATURE_ECE_ACT_DEI_MODE
   printf("! VTSS_FEATURE_ECE_ACT_DEI_MODE \n");
#endif
#ifndef VTSS_FEATURE_ECE_ACT_DP
   printf("! VTSS_FEATURE_ECE_ACT_DP \n");
#endif
#ifndef VTSS_FEATURE_ECE_ACT_INNER_TAG
   printf("! VTSS_FEATURE_ECE_ACT_INNER_TAG \n");
#endif
#ifndef VTSS_FEATURE_ECE_ACT_OT_VID
   printf("! VTSS_FEATURE_ECE_ACT_OT_VID \n");
#endif
#ifndef VTSS_FEATURE_ECE_ACT_OUTER_TAG
   printf("! VTSS_FEATURE_ECE_ACT_OUTER_TAG \n");
#endif
#ifndef VTSS_FEATURE_ECE_ACT_PCP_DEI_PRES
   printf("! VTSS_FEATURE_ECE_ACT_PCP_DEI_PRES \n");
#endif
#ifndef VTSS_FEATURE_ECE_ACT_PCP_MODE
   printf("! VTSS_FEATURE_ECE_ACT_PCP_MODE \n");
#endif
#ifndef VTSS_FEATURE_ECE_ACT_POLICER
   printf("! VTSS_FEATURE_ECE_ACT_POLICER \n");
#endif
#ifndef VTSS_FEATURE_ECE_ACT_PRIO
   printf("! VTSS_FEATURE_ECE_ACT_PRIO \n");
#endif
#ifndef VTSS_FEATURE_ECE_ACT_RULE
   printf("! VTSS_FEATURE_ECE_ACT_RULE \n");
#endif
#ifndef VTSS_FEATURE_ECE_ACT_TX_LOOKUP
   printf("! VTSS_FEATURE_ECE_ACT_TX_LOOKUP \n");
#endif
#ifndef VTSS_FEATURE_ECE_KEY_DIP
   printf("! VTSS_FEATURE_ECE_KEY_DIP \n");
#endif
#ifndef VTSS_FEATURE_ECE_KEY_DMAC
   printf("! VTSS_FEATURE_ECE_KEY_DMAC \n");
#endif
#ifndef VTSS_FEATURE_ECE_KEY_DMAC_TYPE
   printf("! VTSS_FEATURE_ECE_KEY_DMAC_TYPE \n");
#endif
#ifndef VTSS_FEATURE_ECE_KEY_DPORT
   printf("! VTSS_FEATURE_ECE_KEY_DPORT \n");
#endif
#ifndef VTSS_FEATURE_ECE_KEY_ETYPE
   printf("! VTSS_FEATURE_ECE_KEY_ETYPE \n");
#endif
#ifndef VTSS_FEATURE_ECE_KEY_INNER_TAG
   printf("! VTSS_FEATURE_ECE_KEY_INNER_TAG \n");
#endif
#ifndef VTSS_FEATURE_ECE_KEY_IP_PROTO
   printf("! VTSS_FEATURE_ECE_KEY_IP_PROTO \n");
#endif
#ifndef VTSS_FEATURE_ECE_KEY_LLC
   printf("! VTSS_FEATURE_ECE_KEY_LLC \n");
#endif
#ifndef VTSS_FEATURE_ECE_KEY_SIP
   printf("! VTSS_FEATURE_ECE_KEY_SIP \n");
#endif
#ifndef VTSS_FEATURE_ECE_KEY_SMAC
   printf("! VTSS_FEATURE_ECE_KEY_SMAC \n");
#endif
#ifndef VTSS_FEATURE_ECE_KEY_SNAP
   printf("! VTSS_FEATURE_ECE_KEY_SNAP \n");
#endif
#ifndef VTSS_FEATURE_ECE_KEY_SPORT
   printf("! VTSS_FEATURE_ECE_KEY_SPORT \n");
#endif
#ifndef VTSS_FEATURE_EDC_FW_LOAD
   printf("! VTSS_FEATURE_EDC_FW_LOAD \n");
#endif
#ifndef VTSS_FEATURE_EEE
   printf("! VTSS_FEATURE_EEE \n");
#endif
#ifndef VTSS_FEATURE_EVC
   printf("! VTSS_FEATURE_EVC \n");
#endif
#ifndef VTSS_FEATURE_EVC_COSID_COUNTERS
   printf("! VTSS_FEATURE_EVC_COSID_COUNTERS \n");
#endif
#ifndef VTSS_FEATURE_EVC_COSID_POLICERS
   printf("! VTSS_FEATURE_EVC_COSID_POLICERS \n");
#endif
#ifndef VTSS_FEATURE_EVC_COUNTERS
   printf("! VTSS_FEATURE_EVC_COUNTERS \n");
#endif
#ifndef VTSS_FEATURE_EVC_POLICERS
   printf("! VTSS_FEATURE_EVC_POLICERS \n");
#endif
#ifndef VTSS_FEATURE_EVC_PORT
   printf("! VTSS_FEATURE_EVC_PORT \n");
#endif
#ifndef VTSS_FEATURE_EVC_PORT_EVC
   printf("! VTSS_FEATURE_EVC_PORT_EVC \n");
#endif
#ifndef VTSS_FEATURE_EXC_COL_CONT
   printf("! VTSS_FEATURE_EXC_COL_CONT \n");
#endif
#ifndef VTSS_FEATURE_E_TREE
   printf("! VTSS_FEATURE_E_TREE \n");
#endif
#ifndef VTSS_FEATURE_F2DF
   printf("! VTSS_FEATURE_F2DF \n");
#endif
#ifndef VTSS_FEATURE_FAN
   printf("! VTSS_FEATURE_FAN \n");
#endif
#ifndef VTSS_FEATURE_FDMA
   printf("! VTSS_FEATURE_FDMA \n");
#endif
#ifndef VTSS_FEATURE_GFP
   printf("! VTSS_FEATURE_GFP \n");
#endif
#ifndef VTSS_FEATURE_HQOS
   printf("! VTSS_FEATURE_HQOS \n");
#endif
#ifndef VTSS_FEATURE_I2C
   printf("! VTSS_FEATURE_I2C \n");
#endif
#ifndef VTSS_FEATURE_INTERRUPTS
   printf("! VTSS_FEATURE_INTERRUPTS \n");
#endif
#ifndef VTSS_FEATURE_IPV4_MC_SIP
   printf("! VTSS_FEATURE_IPV4_MC_SIP \n");
#endif
#ifndef VTSS_FEATURE_IPV6_MC_SIP
   printf("! VTSS_FEATURE_IPV6_MC_SIP \n");
#endif
#ifndef VTSS_FEATURE_IRQ_CONTROL
   printf("! VTSS_FEATURE_IRQ_CONTROL \n");
#endif
#ifndef VTSS_FEATURE_LAYER2
   printf("! VTSS_FEATURE_LAYER2 \n");
#endif
#ifndef VTSS_FEATURE_LED_POW_REDUC
   printf("! VTSS_FEATURE_LED_POW_REDUC \n");
#endif
#ifndef VTSS_FEATURE_MAC10G
   printf("! VTSS_FEATURE_MAC10G \n");
#endif
#ifndef VTSS_FEATURE_MAC_AGE_AUTO
   printf("! VTSS_FEATURE_MAC_AGE_AUTO \n");
#endif
#ifndef VTSS_FEATURE_MAC_CPU_QUEUE
   printf("! VTSS_FEATURE_MAC_CPU_QUEUE \n");
#endif
#ifndef VTSS_FEATURE_MCE_ACT_PRIO
   printf("! VTSS_FEATURE_MCE_ACT_PRIO \n");
#endif
#ifndef VTSS_FEATURE_MIRROR_CPU
   printf("! VTSS_FEATURE_MIRROR_CPU \n");
#endif
#ifndef VTSS_FEATURE_MISC
   printf("! VTSS_FEATURE_MISC \n");
#endif
#ifndef VTSS_FEATURE_MPLS
   printf("! VTSS_FEATURE_MPLS \n");
#endif
#ifndef VTSS_FEATURE_NPI
   printf("! VTSS_FEATURE_NPI \n");
#endif
#ifndef VTSS_FEATURE_OAM
   printf("! VTSS_FEATURE_OAM \n");
#endif
#ifndef VTSS_FEATURE_OHA
   printf("! VTSS_FEATURE_OHA \n");
#endif
#ifndef VTSS_FEATURE_OTN
   printf("! VTSS_FEATURE_OTN \n");
#endif
#ifndef VTSS_FEATURE_PACKET
   printf("! VTSS_FEATURE_PACKET \n");
#endif
#ifndef VTSS_FEATURE_PACKET_GROUPING
   printf("! VTSS_FEATURE_PACKET_GROUPING \n");
#endif
#ifndef VTSS_FEATURE_PACKET_PIPELINE_PT
   printf("! VTSS_FEATURE_PACKET_PIPELINE_PT \n");
#endif
#ifndef VTSS_FEATURE_PACKET_PORT_L2CP_REG
   printf("! VTSS_FEATURE_PACKET_PORT_L2CP_REG \n");
#endif
#ifndef VTSS_FEATURE_PACKET_PORT_REG
   printf("! VTSS_FEATURE_PACKET_PORT_REG \n");
#endif
#ifndef VTSS_FEATURE_PACKET_PORT_REG_DISCARD
   printf("! VTSS_FEATURE_PACKET_PORT_REG_DISCARD \n");
#endif
#ifndef VTSS_FEATURE_PACKET_RX
   printf("! VTSS_FEATURE_PACKET_RX \n");
#endif
#ifndef VTSS_FEATURE_PACKET_TX
   printf("! VTSS_FEATURE_PACKET_TX \n");
#endif
#ifndef VTSS_FEATURE_PCS_10GBASE_R
   printf("! VTSS_FEATURE_PCS_10GBASE_R \n");
#endif
#ifndef VTSS_FEATURE_PFC
   printf("! VTSS_FEATURE_PFC \n");
#endif
#ifndef VTSS_FEATURE_PHY_TIMESTAMP
   printf("! VTSS_FEATURE_PHY_TIMESTAMP \n");
#endif
#ifndef VTSS_FEATURE_PORT_CNT_BRIDGE
   printf("! VTSS_FEATURE_PORT_CNT_BRIDGE \n");
#endif
#ifndef VTSS_FEATURE_PORT_CNT_ETHER_LIKE
   printf("! VTSS_FEATURE_PORT_CNT_ETHER_LIKE \n");
#endif
#ifndef VTSS_FEATURE_PORT_CONTROL
   printf("! VTSS_FEATURE_PORT_CONTROL \n");
#endif
#ifndef VTSS_FEATURE_PORT_IFH
   printf("! VTSS_FEATURE_PORT_IFH \n");
#endif
#ifndef VTSS_FEATURE_PORT_MUX
   printf("! VTSS_FEATURE_PORT_MUX \n");
#endif
#ifndef VTSS_FEATURE_PTP_RS422
   printf("! VTSS_FEATURE_PTP_RS422 \n");
#endif
#ifndef VTSS_FEATURE_PVLAN
   printf("! VTSS_FEATURE_PVLAN \n");
#endif
#ifndef VTSS_FEATURE_QCL
   printf("! VTSS_FEATURE_QCL \n");
#endif
#ifndef VTSS_FEATURE_QCL_DMAC_DIP
   printf("! VTSS_FEATURE_QCL_DMAC_DIP \n");
#endif
#ifndef VTSS_FEATURE_QCL_KEY_DIP
   printf("! VTSS_FEATURE_QCL_KEY_DIP \n");
#endif
#ifndef VTSS_FEATURE_QCL_KEY_DMAC
   printf("! VTSS_FEATURE_QCL_KEY_DMAC \n");
#endif
#ifndef VTSS_FEATURE_QCL_KEY_INNER_TAG
   printf("! VTSS_FEATURE_QCL_KEY_INNER_TAG \n");
#endif
#ifndef VTSS_FEATURE_QCL_KEY_S_TAG
   printf("! VTSS_FEATURE_QCL_KEY_S_TAG \n");
#endif
#ifndef VTSS_FEATURE_QCL_KEY_TYPE
   printf("! VTSS_FEATURE_QCL_KEY_TYPE \n");
#endif
#ifndef VTSS_FEATURE_QCL_MAP_ACTION
   printf("! VTSS_FEATURE_QCL_MAP_ACTION \n");
#endif
#ifndef VTSS_FEATURE_QCL_PCP_DEI_ACTION
   printf("! VTSS_FEATURE_QCL_PCP_DEI_ACTION \n");
#endif
#ifndef VTSS_FEATURE_QCL_POLICY_ACTION
   printf("! VTSS_FEATURE_QCL_POLICY_ACTION \n");
#endif
#ifndef VTSS_FEATURE_QCL_V2
   printf("! VTSS_FEATURE_QCL_V2 \n");
#endif
#ifndef VTSS_FEATURE_QOS
   printf("! VTSS_FEATURE_QOS \n");
#endif
#ifndef VTSS_FEATURE_QOS_CLASSIFICATION_V2
   printf("! VTSS_FEATURE_QOS_CLASSIFICATION_V2 \n");
#endif
#ifndef VTSS_FEATURE_QOS_COSID_CLASSIFICATION
   printf("! VTSS_FEATURE_QOS_COSID_CLASSIFICATION \n");
#endif
#ifndef VTSS_FEATURE_QOS_CPU_PORT_SHAPER
   printf("! VTSS_FEATURE_QOS_CPU_PORT_SHAPER \n");
#endif
#ifndef VTSS_FEATURE_QOS_DSCP_CLASS_DP_AWARE
   printf("! VTSS_FEATURE_QOS_DSCP_CLASS_DP_AWARE \n");
#endif
#ifndef VTSS_FEATURE_QOS_DSCP_REMARK
   printf("! VTSS_FEATURE_QOS_DSCP_REMARK \n");
#endif
#ifndef VTSS_FEATURE_QOS_DSCP_REMARK_DP_AWARE
   printf("! VTSS_FEATURE_QOS_DSCP_REMARK_DP_AWARE \n");
#endif
#ifndef VTSS_FEATURE_QOS_DSCP_REMARK_V2
   printf("! VTSS_FEATURE_QOS_DSCP_REMARK_V2 \n");
#endif
#ifndef VTSS_FEATURE_QOS_EGRESS_MAP
   printf("! VTSS_FEATURE_QOS_EGRESS_MAP \n");
#endif
#ifndef VTSS_FEATURE_QOS_EGRESS_QUEUE_SHAPERS
   printf("! VTSS_FEATURE_QOS_EGRESS_QUEUE_SHAPERS \n");
#endif
#ifndef VTSS_FEATURE_QOS_EGRESS_QUEUE_SHAPERS_EB
   printf("! VTSS_FEATURE_QOS_EGRESS_QUEUE_SHAPERS_EB \n");
#endif
#ifndef VTSS_FEATURE_QOS_EGRESS_SHAPERS_DLB
   printf("! VTSS_FEATURE_QOS_EGRESS_SHAPERS_DLB \n");
#endif
#ifndef VTSS_FEATURE_QOS_EGRESS_SHAPERS_RT
   printf("! VTSS_FEATURE_QOS_EGRESS_SHAPERS_RT \n");
#endif
#ifndef VTSS_FEATURE_QOS_HSCH_LEAK_LISTS
   printf("! VTSS_FEATURE_QOS_HSCH_LEAK_LISTS \n");
#endif
#ifndef VTSS_FEATURE_QOS_INGRESS_MAP
   printf("! VTSS_FEATURE_QOS_INGRESS_MAP \n");
#endif
#ifndef VTSS_FEATURE_QOS_POLICER_BC_SWITCH
   printf("! VTSS_FEATURE_QOS_POLICER_BC_SWITCH \n");
#endif
#ifndef VTSS_FEATURE_QOS_POLICER_DLB
   printf("! VTSS_FEATURE_QOS_POLICER_DLB \n");
#endif
#ifndef VTSS_FEATURE_QOS_POLICER_MC_SWITCH
   printf("! VTSS_FEATURE_QOS_POLICER_MC_SWITCH \n");
#endif
#ifndef VTSS_FEATURE_QOS_POLICER_UC_SWITCH
   printf("! VTSS_FEATURE_QOS_POLICER_UC_SWITCH \n");
#endif
#ifndef VTSS_FEATURE_QOS_PORT_POLICER_EXT
   printf("! VTSS_FEATURE_QOS_PORT_POLICER_EXT \n");
#endif
#ifndef VTSS_FEATURE_QOS_PORT_POLICER_EXT_DPBL
   printf("! VTSS_FEATURE_QOS_PORT_POLICER_EXT_DPBL \n");
#endif
#ifndef VTSS_FEATURE_QOS_PORT_POLICER_EXT_FC
   printf("! VTSS_FEATURE_QOS_PORT_POLICER_EXT_FC \n");
#endif
#ifndef VTSS_FEATURE_QOS_PORT_POLICER_EXT_FPS
   printf("! VTSS_FEATURE_QOS_PORT_POLICER_EXT_FPS \n");
#endif
#ifndef VTSS_FEATURE_QOS_PORT_POLICER_EXT_TTM_V2
   printf("! VTSS_FEATURE_QOS_PORT_POLICER_EXT_TTM_V2 \n");
#endif
#ifndef VTSS_FEATURE_QOS_QUEUE_POLICER
   printf("! VTSS_FEATURE_QOS_QUEUE_POLICER \n");
#endif
#ifndef VTSS_FEATURE_QOS_QUEUE_TX
   printf("! VTSS_FEATURE_QOS_QUEUE_TX \n");
#endif
#ifndef VTSS_FEATURE_QOS_SCHEDULER_DWRR_CNT
   printf("! VTSS_FEATURE_QOS_SCHEDULER_DWRR_CNT \n");
#endif
#ifndef VTSS_FEATURE_QOS_SCHEDULER_V2
   printf("! VTSS_FEATURE_QOS_SCHEDULER_V2 \n");
#endif
#ifndef VTSS_FEATURE_QOS_TAG_REMARK_V2
   printf("! VTSS_FEATURE_QOS_TAG_REMARK_V2 \n");
#endif
#ifndef VTSS_FEATURE_QOS_WRED_V2
   printf("! VTSS_FEATURE_QOS_WRED_V2 \n");
#endif
#ifndef VTSS_FEATURE_QOS_WRED_V3
   printf("! VTSS_FEATURE_QOS_WRED_V3 \n");
#endif
#ifndef VTSS_FEATURE_RAB
   printf("! VTSS_FEATURE_RAB \n");
#endif
#ifndef VTSS_FEATURE_SD10G65
   printf("! VTSS_FEATURE_SD10G65 \n");
#endif
#ifndef VTSS_FEATURE_SERDES_MACRO_SETTINGS
   printf("! VTSS_FEATURE_SERDES_MACRO_SETTINGS \n");
#endif
#ifndef VTSS_FEATURE_SERIAL_GPIO
   printf("! VTSS_FEATURE_SERIAL_GPIO \n");
#endif
#ifndef VTSS_FEATURE_SFLOW
   printf("! VTSS_FEATURE_SFLOW \n");
#endif
#ifndef VTSS_FEATURE_SYNCE
   printf("! VTSS_FEATURE_SYNCE \n");
#endif
#ifndef VTSS_FEATURE_SYNCE_10G
   printf("! VTSS_FEATURE_SYNCE_10G \n");
#endif
#ifndef VTSS_FEATURE_SYNC_ANN_AUTO_TRANSMIT
   printf("! VTSS_FEATURE_SYNC_ANN_AUTO_TRANSMIT \n");
#endif
#ifndef VTSS_FEATURE_TEMP_SENSOR
   printf("! VTSS_FEATURE_TEMP_SENSOR \n");
#endif
#ifndef VTSS_FEATURE_TIMESTAMP
   printf("! VTSS_FEATURE_TIMESTAMP \n");
#endif
#ifndef VTSS_FEATURE_TIMESTAMP_ASYMMETRY_COMP
   printf("! VTSS_FEATURE_TIMESTAMP_ASYMMETRY_COMP \n");
#endif
#ifndef VTSS_FEATURE_TIMESTAMP_LATENCY_COMP
   printf("! VTSS_FEATURE_TIMESTAMP_LATENCY_COMP \n");
#endif
#ifndef VTSS_FEATURE_TIMESTAMP_ONE_STEP
   printf("! VTSS_FEATURE_TIMESTAMP_ONE_STEP \n");
#endif
#ifndef VTSS_FEATURE_TIMESTAMP_ORG_TIME
   printf("! VTSS_FEATURE_TIMESTAMP_ORG_TIME \n");
#endif
#ifndef VTSS_FEATURE_TIMESTAMP_P2P_DELAY_COMP
   printf("! VTSS_FEATURE_TIMESTAMP_P2P_DELAY_COMP \n");
#endif
#ifndef VTSS_FEATURE_UPI
   printf("! VTSS_FEATURE_UPI \n");
#endif
#ifndef VTSS_FEATURE_VCAP
   printf("! VTSS_FEATURE_VCAP \n");
#endif
#ifndef VTSS_FEATURE_VCL
   printf("! VTSS_FEATURE_VCL \n");
#endif
#ifndef VTSS_FEATURE_VLAN_COUNTERS
   printf("! VTSS_FEATURE_VLAN_COUNTERS \n");
#endif
#ifndef VTSS_FEATURE_VLAN_PORT_V2
   printf("! VTSS_FEATURE_VLAN_PORT_V2 \n");
#endif
#ifndef VTSS_FEATURE_VLAN_SVL
   printf("! VTSS_FEATURE_VLAN_SVL \n");
#endif
#ifndef VTSS_FEATURE_VLAN_TRANSLATION
   printf("! VTSS_FEATURE_VLAN_TRANSLATION \n");
#endif
#ifndef VTSS_FEATURE_VLAN_TX_TAG
   printf("! VTSS_FEATURE_VLAN_TX_TAG \n");
#endif
#ifndef VTSS_FEATURE_VSTAX
   printf("! VTSS_FEATURE_VSTAX \n");
#endif
#ifndef VTSS_FEATURE_VSTAX_V2
   printf("! VTSS_FEATURE_VSTAX_V2 \n");
#endif
#ifndef VTSS_FEATURE_WARM_START
   printf("! VTSS_FEATURE_WARM_START \n");
#endif
#ifndef VTSS_FEATURE_WIS
   printf("! VTSS_FEATURE_WIS \n");
#endif
#ifndef VTSS_FEATURE_XAUI
   printf("! VTSS_FEATURE_XAUI \n");
#endif
#ifndef VTSS_FEATURE_XFI
   printf("! VTSS_FEATURE_XFI \n");
#endif
#ifndef VTSS_LEDA_CHANNELS
   printf("! VTSS_LEDA_CHANNELS \n");
#endif
#ifndef VTSS_LEDA_INIT_INTERFACE
   printf("! VTSS_LEDA_INIT_INTERFACE \n");
#endif
#ifndef VTSS_OPT_AFI_OPTIMIZE_FOR_TIMERS
   printf("! VTSS_OPT_AFI_OPTIMIZE_FOR_TIMERS \n");
#endif
#ifndef VTSS_OPT_FDMA
   printf("! VTSS_OPT_FDMA \n");
#endif
#ifndef VTSS_OPT_FDMA_DEBUG
   printf("! VTSS_OPT_FDMA_DEBUG \n");
#endif
#ifndef VTSS_OPT_FDMA_IRQ_CONTEXT
   printf("! VTSS_OPT_FDMA_IRQ_CONTEXT \n");
#endif
#ifndef VTSS_OPT_FDMA_VER
   printf("! VTSS_OPT_FDMA_VER \n");
#endif
#ifndef VTSS_OPT_PCIE_ACCESS
   printf("! VTSS_OPT_PCIE_ACCESS \n");
#endif
#ifndef VTSS_OPT_PORT_COUNT
   printf("! VTSS_OPT_PORT_COUNT \n");
#endif
#ifndef VTSS_OPT_TRACE
   printf("! VTSS_OPT_TRACE \n");
#endif
#ifndef VTSS_OPT_VAUI_EQ_CTRL
   printf("! VTSS_OPT_VAUI_EQ_CTRL \n");
#endif
#ifndef VTSS_OPT_VCORE_III
   printf("! VTSS_OPT_VCORE_III \n");
#endif
#ifndef VTSS_PHY_OPT_VERIPHY
   printf("! VTSS_PHY_OPT_VERIPHY \n");
#endif
#ifndef _VTSS_OPTIONS_H_
   printf("! _VTSS_OPTIONS_H_ \n");
#endif
}
