/*
 Copyright (c) 2002-2017 Microsemi Corporation "Microsemi". All Rights Reserved.

 Unpublished rights reserved under the copyright laws of the United States of
 America, other countries and international treaties. Permission to use, copy,
 store and modify, the software and its source code is granted but only in
 connection with products utilizing the Microsemi switch and PHY products.
 Permission is also granted for you to integrate into other products, disclose,
 transmit and distribute the software only in an absolute machine readable format
 (e.g. HEX file) and only in or with products utilizing the Microsemi switch and
 PHY products.  The source code of the software may not be disclosed, transmitted
 or distributed without the prior written permission of Microsemi.

 This copyright notice must appear in any copy, modification, disclosure,
 transmission or distribution of the software.  Microsemi retains all ownership,
 copyright, trade secret and proprietary rights in the software and its source code,
 including all modifications thereto.

 THIS SOFTWARE HAS BEEN PROVIDED "AS IS". MICROSEMI HEREBY DISCLAIMS ALL WARRANTIES
 OF ANY KIND WITH RESPECT TO THE SOFTWARE, WHETHER SUCH WARRANTIES ARE EXPRESS,
 IMPLIED, STATUTORY OR OTHERWISE INCLUDING, WITHOUT LIMITATION, WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR USE OR PURPOSE AND NON-INFRINGEMENT.
*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "demo_trace.h"
#include "demo_api.h"
#include <vtss_trace_api.h>

#define NUMBER_OF_THREADS 3

static vtss_handle_t  my_thread_handle[NUMBER_OF_THREADS];
static vtss_thread_t  my_thread_state[NUMBER_OF_THREADS];
static char           my_thread_stack[NUMBER_OF_THREADS][THREAD_DEFAULT_STACK_SIZE];


// --- Most of the time, you might want to create the thread you need at startup time,
//     look in demo.c and search for vtss_thread_create().
//
//     But threads can be created at will


static struct {
    char* name;
    vtss_addrword_t context;
} p[3] = {
    { "The1stThread", 0x11111111 },
    { "The2ndThread", 0x22222222 },
    { "The3rdThread", 0x33333333 }
};

void demo_thread(void)
{

    // (1) --- Create a thread resource. It must have a:
    //         - A priority. Here it is VTSS_THREAD_PRIO_DEFAULT.
    //         - A function like my_thread()
    //         - A 'stack', a 'handler' and a 'state'. All there must be
    //           allocated. Here they are static variables.
    //
    
    for (i=0; i<3; ++i) {
        vtss_thread_create(VTSS_THREAD_PRIO_DEFAULT,
                           my_thread,
                           context[i].user_value,
                           context[i].name,
                           my_thread_stack[i],
                           sizeof(my_thread_stack[0]),
                           &my_thread_handle[i],
                           &my_thread_state[i]);
    }



    
    // (2) --- The thread is not started yet, so we'll do it now
    //
    vtss_thread_resume(my_thread_handle);


    
    

}
