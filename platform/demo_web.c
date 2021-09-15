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



/*
 * In order to activate this demo htlp code, insert the code:
 *
 * #if defined(VTSS_SW_OPTION_DEMO)
 *  Demo,demo.htm
 * #endif
 *
 * in the file vtss_appl/web/html/menu_default.txt.
 * Note the indention is important. Try e.g. also the
 * variant:
 *
 *
 * #if defined(VTSS_SW_OPTION_DEMO)
 *  Demo
 *   Submenu,demo.htm
 * #endif
 */



#include "web_api.h"
#include "os_file_api.h"
#include <dirent.h>
#include <unistd.h>


#ifdef VTSS_SW_OPTION_PRIV_LVL
#include "vtss_privilege_api.h"
#include "vtss_privilege_web_api.h"
#endif

#include "demo_trace.h"

#define VTSS_ALLOC_MODULE_ID VTSS_MODULE_ID_DEMO
#define VTSS_TRACE_MODULE_ID VTSS_MODULE_ID_DEMO


static u32 demo_number = 1234;
static u32 demo_enable = 0;


static i32 demo_handler_status(CYG_HTTPD_STATE *httpd_state)
{
    T_D("entry");

#ifdef VTSS_SW_OPTION_PRIV_LVL
    if (web_process_priv_lvl(httpd_state, VTSS_PRIV_LVL_CONFIG_TYPE, VTSS_MODULE_ID_DEMO)) {
        goto early_out;
    }
#endif


    switch (httpd_state->method) {


    // --- This case is run, when the associated web page
    //     is loaded or reloaded.
    case CYG_HTTPD_METHOD_GET: {
        int cnt;
        char str[32];
        char encoded_string[32];

	// --- Define your own encoding of the information you want to
	//     send to the browser. Here we encode our answer as
	//     "OK*9*1". This is just an accident. You can do it any way you like.
	//     The OK is here used to indicate to the html code, that this
	//     answer is okey.
        sprintf(str, "OK*%d*%d", demo_number, demo_enable);

        (void)cyg_httpd_start_chunked("html");
        cnt = cgi_escape(str, encoded_string);
        (void)cyg_httpd_write_chunked(encoded_string, cnt);
        (void)cyg_httpd_end_chunked();

	// --- See debug print out by giving the vcli command:
	//     > debug trace module level demo web debug
	T_DG(TRACE_GRP_WEB, "str=%s", str);
	T_DG(TRACE_GRP_WEB, "encoded_string=%s", str);

    }
    break;


    // --- This case is run, when <SAVE> is pressed on the demo html page
    case CYG_HTTPD_METHOD_POST: {
        const char  *var_p;
        size_t var_len;
        char str[10];


	// (1) --- Get value for demo_number
	bzero(str,sizeof(str)); // Just make sure we stop.
        var_p = cyg_httpd_form_varable_string(httpd_state, "demo_number", &var_len);


        if (var_p && var_len) {
            strncpy(str, var_p, MIN(sizeof(str) - 1, var_len));
        }

	//     So now str contains the value for demo_number.
	//     We just print it out in this demo, and save it.
	T_DG(TRACE_GRP_WEB, "demo_number=%s", str);
	demo_number = atoi(str);


	// (2) --- Get value for demo_enable
	bzero(str,sizeof(str));
        var_p = cyg_httpd_form_varable_string(httpd_state, "demo_enable", &var_len);


        if (var_p && var_len) {
            strncpy(str, var_p, MIN(sizeof(str) - 1, var_len));
        }

	// --- See debug print out by giving the vcli command:
	//     > debug trace module level demo web debug
	T_DG(TRACE_GRP_WEB, "demo_enable=%s", str);
	demo_enable = (var_len ? 1 : 0);
	

	// (3) --- Exercise: Put in your own number


        redirect(httpd_state, "/demo.htm");
    }
    break;

    default:
        break;
    }

early_out:

    return -1;
}




/****************************************************************************/
/*  HTTPD Table Entries                                                     */
/****************************************************************************/

CYG_HTTPD_HANDLER_TABLE_ENTRY(get_cb_demo,       "/config/demo_conf_status",      demo_handler_status);


/****************************************************************************/
/*  End of file.                                                            */
/****************************************************************************/
