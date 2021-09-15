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

#ifndef __DEMO_API__
#define __DEMO_API__

#include <vtss/appl/types.h>
#include <misc_api.h>
#include <conf_api.h>

/*
 * Values of vtss_rc specific for this module
 */
enum {
    DEMO_ERROR_GEN = MODULE_ERROR_START(VTSS_MODULE_ID_DEMO),
    DEMO_ERROR_SOMETHING,
    DEMO_ERROR_SOMETHING_ELSE,
};

#define THREAD_DEFAULT_PRIO 7

// For OAM demo example
//
#define DEMO_FLAG_CCM     (1<<0)
#define DEMO_FLAG_CCM_LM  (1<<1)
#define DEMO_FLAG_LM      (1<<2)
#define DEMO_FLAG_DM_DE   (1<<3)
#define DEMO_FLAG_DM_SE   (1<<4)
#define DEMO_FLAG_SL_DE   (1<<5)
#define DEMO_FLAG_SL_SE   (1<<6)
#define DEMO_FLAG_LB      (1<<7)
#define DEMO_FLAG_LT      (1<<8)
#define DEMO_FLAG_TST     (1<<9)

#define DEMO_ENABLE_CCM(m)    (m & DEMO_FLAG_CCM    ? TRUE : FALSE)
#define DEMO_ENABLE_CCM_LM(m) (m & DEMO_FLAG_CCM_LM ? TRUE : FALSE)
#define DEMO_ENABLE_LM(m)     (m & DEMO_FLAG_LM     ? TRUE : FALSE)
#define DEMO_ENABLE_DM_DE(m)  (m & DEMO_FLAG_DM_SE  ? TRUE : FALSE)
#define DEMO_ENABLE_DM_SE(m)  (m & DEMO_FLAG_DM_SE  ? TRUE : FALSE)

#define DEMO_ENABLE_DM(m)  (m & (DEMO_FLAG_DM_DE|DEMO_FLAG_DM_SE) ? TRUE : FALSE)

#define DEMO_ENABLE_SL_DE(m)  (m & DEMO_FLAG_SL_DE ? TRUE : FALSE)
#define DEMO_ENABLE_SL_SE(m)  (m & DEMO_FLAG_SL_SE ? TRUE : FALSE)
#define DEMO_ENABLE_LB(m)     (m & DEMO_FLAG_LB    ? TRUE : FALSE)
#define DEMO_ENABLE_LT(m)     (m & DEMO_FLAG_LT    ? TRUE : FALSE)
#define DEMO_ENABLE_TST(m)    (m & DEMO_FLAG_TST   ? TRUE : FALSE)


extern mesa_rc demo_init(vtss_init_data_t *data);
extern const char *demo_error_txt(mesa_rc error);

#ifdef __cplusplus
extern "C" {
#endif

extern void debug_forwarding_control(BOOL enable, int method);
extern void demo_msg_tx(vtss_isid_t isid);
extern mesa_rc demo_tx_frame(mesa_port_no_t port_no, mesa_mac_t dmac, int method, int swtch);

extern void demo_npi(BOOL enable, BOOL use_npi, mesa_port_no_t port_no);
extern void demo_evc_main(void);
extern void demo_acl_main(void);

extern void demo_evc_main_ece_clear(mesa_port_no_t port_no);
extern void demo_evc_main_ece_statistics(mesa_port_no_t port_no);

extern void demo_qce_create(void);
extern void demo_qce_config_key(void);

extern mesa_rc demo_shaper(u32 r);

extern void demo_qos_dump(void);

extern void demo_oam_down(int);
extern void demo_oam_up(int);

extern void demo_oam_tx(int type, int isdx, BOOL upmep);
extern void demo_options(void);
extern void demo_mce_add_print(BOOL all);

extern void demo_print_vop(void);

extern void demo_read_port_counters(u32 port);

extern void demo_debug_print(void);
extern void demo_dump_mac_table(void);
extern void demo_dump_mac_table2(void);
extern void demo_mac_del(void);
extern void demo_vlan_evc(void);
extern void demo_vlan_evc_counters(void);

extern void demo_packet_api(void);
extern void create_acl_main(void);
extern void demo_dump_fdma_cfg(void);

extern int demo_socket_client(mesa_ip_t ip_addr);
extern int demo_socket_server(mesa_ip_t ip_addr);

    
#ifdef __cplusplus
}
#endif



#endif
