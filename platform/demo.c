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

//#include <vtss_types.h>
#define DEMO_LINUX
#include <vtss/appl/types.h>
#include <misc_api.h>
#include <conf_api.h>
/* #include <cyg/infra/diag.h> */
#ifdef VTSS_SW_OPTION_ICFG
#include <icfg_api.h>
#endif

#include "demo_api.h"
#include "demo.h"
#include "demo_trace.h"

#ifndef DEMO_LINUX
#define vtss_thread_t cyg_thread
#define vtss_handle_t  cyg_handle_t
#define vtss_flag_t  cyg_flag_t
#endif

#ifndef VTSS_THREAD_PRIO_DEFAULT
//#define VTSS_THREAD_PRIO_DEFAULT THREAD_DEFAULT_PRIO
#endif

extern void demo_register_l2proto(void); // From demo_l2proto.c
extern void demo_register_packet(void);  // From demo_packet.c
extern void demo_register_msg(void);     // From demo_msg.h
extern void demo_register_trace(void);   // From demo_trace.c
extern void demo_register_icfg(void);    // From demo_icfg.c
extern void demo_json_init();

extern "C" int demo_icli_cmd_register();


#define DEMO_RX_BUFFER_SIZE_BYTES   40000
vtss_bip_buffer_t  demo_rx_buffer;





// --- Some global variabled for flags, thread and semaphore/mutex
//

static vtss_handle_t  demo_thread_handle;
static vtss_thread_t  demo_thread_state;
#if 0
static char           demo_thread_stack[THREAD_DEFAULT_STACK_SIZE];
#define     sizeof(demo_thread_stack)
#else
static char           *demo_thread_stack=nullptr;
#define     sizeof_demo_thread_stack 0
#endif

vtss_flag_t   demo_thread_flag;
critd_t      demo_platform_crit;




// --- Thread in this demo.
//     It waits on the flags to change, and then print out which one.
//
static void demo_thread(vtss_addrword_t data)
{
    vtss_flag_value_t flags;
    /* vtss_handle_t H; */
    /* UINT16 ID; */
    /* vtss_thread_info I; */

    // --- Start with a clean buffer. Since the l2proto module
    //     used this too, see demo_l2proto.c, it is a critical region.
    DEMO_PLATFORM_CRIT_ENTER();
    vtss_bip_buffer_clear(&demo_rx_buffer);
    DEMO_PLATFORM_CRIT_EXIT();

    /* H = vtss_thread_self(); */
    /* ID = vtss_thread_get_id(H); */
    /* vtss_thread_get_info(H, ID, &I); */

    /* printf("Thread=%s\n", I.name); */


    for (;;) {

        flags = vtss_flag_wait(&demo_thread_flag, 0xfffffff, VTSS_FLAG_WAITMODE_OR_CLR);


        if (DEMO_FLAG_START & flags) {
            printf("Start event\n");
        }

        if (DEMO_FLAG_CONF_DEF & flags) {
            printf("Cond Def event\n");
        }

        if (DEMO_FLAG_ICFG_LOADING_PRE & flags) {
            printf("Configuration loading - pre\n");
        }

        if (DEMO_FLAG_ICFG_LOADING_POST & flags) {
            printf("Configuration loading - post\n");
        }

        if (DEMO_FLAG_SUSPEND_RESUME & flags) {
            printf("Suspent Resume event\n");
        }

        if (DEMO_FLAG_WARMSTART_QUERY & flags) {
            printf("Warmstart Query event\n");
        }

        if (DEMO_FLAG_PACKET_RX & flags) {
            u32 *buf;

            // --- Get buffers from the bip buffer until it is empty.
            do {
                int contiguous_block_size;

                // (1) --- Get next buffer
                DEMO_PLATFORM_CRIT_ENTER();
                buf = (u32 *)vtss_bip_buffer_get_contiguous_block(&demo_rx_buffer, &contiguous_block_size);
                DEMO_PLATFORM_CRIT_EXIT();

                // (2) --- if any buffer, then ...
                if (buf) {
                    u32 total_len = buf[0];

                    // ...do something useful...

                    // ...and release eventually.
                    DEMO_PLATFORM_CRIT_ENTER();
                    vtss_bip_buffer_decommit_block(&demo_rx_buffer, total_len);
                    DEMO_PLATFORM_CRIT_EXIT();
                }

            } while (buf);

            T_DG(TRACE_GRP_PACKET, "Received packet\n");
        }


        if (DEMO_FLAG_SHOW_DEMO & flags) {
	  printf("show demo event\n");
	}

        DEMO_PLATFORM_CRIT_ENTER();
        //   Critical region. Do what ever you have to do.
        DEMO_PLATFORM_CRIT_EXIT();


    }
}






/*
 *  We'll call this software option for: VTSS_SW_OPTION_DEMO
 *  Changes to make, in order to include this option:
 *
 *  1): In vtss_appl/main/main.c search for xxrp and add
 *      the demo module in a similar fashion.
 *
 *  2):
 *
 */

#ifdef DEMO_OAM
mesa_rc demo_oam_tx_init(vtss_init_data_t *data);
#endif

mesa_rc demo_init(vtss_init_data_t *data)
{

    switch (data->cmd) {

    case INIT_CMD_INIT:
        // --- This section is run once at boot time from vtss_appl/main/main.c::vtss_user_start()
        //     and before the scheduler is started - so do not use printf here!
        //     Everything that needs to be created at boot time goes here, e.g.:
        //     1) Thread creation
        //     2) creation/init of flags
        //     3) creation/init of semaphoreds
        //     4) etc


        // --- Create trace functionality
        demo_register_trace();

        // --- Create the rx buffer for received PDUs (master only)
        if ( ! vtss_bip_buffer_init(&demo_rx_buffer, DEMO_RX_BUFFER_SIZE_BYTES) )  {
            T_EG(TRACE_GRP_PLATFORM, "Unable to create rx buffer");
        }


        // --- Create some flags for signaling
        vtss_flag_init(&demo_thread_flag);

        // --- Create a semaphore/mutex. Note it is taken initially.
        //     So it has to be exit. We do it here immediately.
        critd_init(&demo_platform_crit, "demo_platform", VTSS_MODULE_ID_DEMO, CRITD_TYPE_MUTEX);
	//        DEMO_PLATFORM_CRIT_EXIT();

        // --- Create a thread
        vtss_thread_create(VTSS_THREAD_PRIO_DEFAULT,
                          demo_thread,
                          0,
                          "demo thread",
                          demo_thread_stack,
                          sizeof_demo_thread_stack,
                          &demo_thread_handle,
                          &demo_thread_state);


	// We have á dedicated json example
	//        demo_json_init();

	// --- Register ICLI commands
	demo_icli_cmd_register();

        break;

    case INIT_CMD_START:
      //        vtss_thread_resume(demo_thread_handle);

        vtss_flag_setbits(&demo_thread_flag, DEMO_FLAG_START);

        // --- Register on some commonly used modules
#if 1
        demo_register_l2proto();
        demo_register_packet();
        demo_register_msg();
        demo_register_icfg();
#endif

        break;

    case INIT_CMD_CONF_DEF:
        vtss_flag_setbits(&demo_thread_flag, DEMO_FLAG_CONF_DEF);
        break;

    case INIT_CMD_ICFG_LOADING_PRE:
        vtss_flag_setbits(&demo_thread_flag, DEMO_FLAG_ICFG_LOADING_PRE);
        break;

    case INIT_CMD_ICFG_LOADING_POST:
        vtss_flag_setbits(&demo_thread_flag, DEMO_FLAG_ICFG_LOADING_POST);
        break;

    case INIT_CMD_SUSPEND_RESUME:
        vtss_flag_setbits(&demo_thread_flag, DEMO_FLAG_SUSPEND_RESUME);
        break;

    case INIT_CMD_WARMSTART_QUERY:
        vtss_flag_setbits(&demo_thread_flag, DEMO_FLAG_WARMSTART_QUERY);
        break;

    default:
        break;
    }


    // --- Just a similar init function else where.
#ifdef DEMO_OAM
    demo_oam_tx_init(data);
#endif

    return VTSS_RC_OK;
}


const char *demo_error_txt(mesa_rc error)
{
  switch (error) {
  case DEMO_ERROR_GEN: return "DEMO_ERROR_GEN";
  case DEMO_ERROR_SOMETHING: return "DEMO_ERROR_SOMETHING";
  case DEMO_ERROR_SOMETHING_ELSE: return "DEMO_ERROR_SOMETHING_ELSE";
  }

  return "DEMO: Unknown error code";
}
