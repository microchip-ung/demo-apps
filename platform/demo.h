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
#ifndef __DEMO_H__
#define __DEMO_H__

#include <vtss_bip_buffer_api.h>
#include <critd_api.h>
#include <vtss_trace_api.h>

// --- The functions/mutex for enter- and exit a critical region takes different parameters
//     depending on whether VTSS_TRACE_ENABLED is defined.
//     Therefore it is recommended to define macros to hide this.
//
#if 1
#if VTSS_TRACE_ENABLED

#define DEMO_PLATFORM_CRIT_ENTER()  critd_enter(&demo_platform_crit, TRACE_GRP_CRIT, VTSS_TRACE_LVL_NOISE, __FILE__, __LINE__)
#define DEMO_PLATFORM_CRIT_EXIT()   critd_exit(&demo_platform_crit,  TRACE_GRP_CRIT, VTSS_TRACE_LVL_NOISE, __FILE__, __LINE__)
#define DEMO_PLATFORM_CRIT_ASSERT_LOCKED() critd_assert_locked(&demo_platform_crit, TRACE_GRP_CRIT,        __FILE__, __LINE__)

#else

#define DEMO_PLATFORM_CRIT_ENTER()  critd_enter(&demo_platform_crit, __FILE__, __LINE__)
#define DEMO_PLATFORM_CRIT_EXIT()   critd_exit(&demo_platform_crit, __FILE__, __LINE__)
#define DEMO_PLATFORM_CRIT_ASSERT_LOCKED()  critd_assert_locked(&demo_platform_crit, __FILE__, __LINE__)

#endif

#else

#define DEMO_PLATFORM_CRIT_ENTER()  do {}while(0)
#define DEMO_PLATFORM_CRIT_EXIT()   do {}while(0)
#define DEMO_PLATFORM_CRIT_ASSERT_LOCKED()  do {}while(0)

#endif

// --- Defined used in this example
//
#define DEMO_FLAG_START             (1<<0)
#define DEMO_FLAG_CONF_DEF          (1<<1)
#define DEMO_FLAG_ICFG_LOADING_PRE  (1<<2)
#define DEMO_FLAG_ICFG_LOADING_POST (1<<3)
#define DEMO_FLAG_SUSPEND_RESUME    (1<<4)
#define DEMO_FLAG_WARMSTART_QUERY   (1<<5)

#define DEMO_FLAG_PACKET_RX         (1<<6)

#define DEMO_FLAG_SHOW_DEMO         (1<<7)
#define DEMO_FLAG_OAM_TX            (1<<8)


extern vtss_bip_buffer_t  demo_rx_buffer;
extern critd_t            demo_platform_crit;
extern vtss_flag_t         demo_thread_flag;


#endif
