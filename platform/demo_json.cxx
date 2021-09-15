/*
 Vitesse API software.

 Copyright (c) 2002-2014 Vitesse Semiconductor Corporation "Vitesse". All
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

#include "demo_serializer.hxx"
#include <vtss/basics/expose/json.hxx>

#define NS(N, P, D) static vtss::expose::json::NamespaceNode N(&P, D);

using namespace vtss;
using namespace vtss::json;
using namespace vtss::expose::json;
using namespace vtss::demo;


namespace vtss {

  void json_node_add(Node *node);

}

static NamespaceNode root("demo");


// Parent: vtss/demo -------------------------------------------------------------
NS(demo_somedata, root, "somedata");

#if 0
// You can add a number of leafs to the root node "demo".
NS(demo_somedata2, root, "somedata2");
#endif

// Parent: vtss/demo/somedata ------------------------------------------------------
static StructReadWrite<DemoConfig_t>  rw_demo_config(&demo_somedata, "something");


// --- Here this little demo json tree is hooked up into the JSON framwork something like this:
//
// vtss_rc demo_init(vtss_init_data_t *data)
// {
// 
//     switch (data->cmd) {
//     case INIT_CMD_INIT:
// 
//         demo_json_init();
// 
//         break;
//      ...
//     }
// 
//     return VTSS_RC_OK;
// }
//
// which is called from vtss_appl/main/main.c in the usual way.
//
void demo_json_init()
{
    json_node_add(&root);
}


// Notes:
//
// In order to test a JSON implementation the vson tool is recommended. This is available
// from:
// $ git clone https://github.com/vtss/json-rpc-util
// and require ruby to be installed. This is e.g. done on fedora with
// $ sudo dnf install ruby
// beside this you might need to do
// $ gem install net-http-digest_auth
//
// Run http://<IP-address>/json_spec and search for "demo" to see how this implementation
// turn up! It will be something like this:
//
//{
//    "types":[
//     ... 
//
//        {
//            "type-name":"demo_json_a_type_t",
//            "class":"Struct",
//            "description":"",
//            "encoding-type":"Object",
//            "elements":[
//                {
//                    "name":"SomeInterestingNumber",
//                    "type":"int32_t",
//                    "description":"Example of a number."
//                },
//                {
//                    "name":"SomeCoolString",
//                    "type":"char *",
//                    "description":"Example of a DisplayString."
//                }
//            ]
//        },
//     ...
//
//    ],
//    "groups":[
//     ...
//    ],
//    "methods":[
//     ...
//        {
//            "method-name":"demo.somedata.something.get",
//            "params":[],
//            "result":[
//                {
//                    "name":"config",
//                    "type":"demo_json_a_type_t"
//                }
//            ]
//        },
//        {
//            "method-name":"demo.somedata.something.set",
//            "params":[
//                {
//                    "name":"config",
//                    "type":"demo_json_a_type_t"
//                }
//            ],
//            "result":[]
//        },
//     ...
//    ]
//}
//
// In order to call these function do
//  $ ./vson -c -d <IP-address> update-spec
// which will download the json specification above to ~/.
//
// Run
//  $ ./vson -c grep demo
// which will list the methods in the spec that contain the string "demo".
// So in this case it will give:
//  demo.somedata.something.get
//  demo.somedata.something.set
//
// To get information on the get method run:
//
// ------------
// $ ./vson -c spec demo.somedata.something.get
// Method name:
//   demo.somedata.something.get
// Parameters:
//
// Results:
//   Result #0 {"name"=>"config", "type"=>"demo_json_a_type_t"}
//             {"type-name"=>"demo_json_a_type_t",
//              "class"=>"Struct",
//              "description"=>"",
//              "encoding-type"=>"Object",
//              "elements"=>[{"name"=>"SomeInterestingNumber",
//			      "type"=>"int32_t",
//			      "description"=>"Example of a number."},
//		             {"name"=>"SomeCoolString",
//			      "type"=>"char *",
//			      "description"=>"Example of a DisplayString."}]}
// ------------
//
// So this shows, that the get method take no input parameters since nothing is listed under "Parameters:"
// 
// Call the get method with:
//
// ------------
//  $ ./vson -c -d <IP-address> call demo.somedata.something.get
//  Calling demo.somedata.something.get:
//      SomeInterestingNumber:   1
//      SomeCoolString:        xxx
// ------------
//
// Try the same ting with the set method:

// Call the set method with:
//
// ------------
//  $ ./vson -c spec demo.somedata.something.set
//  Method name:
//    demo.somedata.something.set
//  Parameters:
//    Param #0 {"name"=>"config", "type"=>"demo_json_a_type_t"}
//             {"type-name"=>"demo_json_a_type_t",
//              "class"=>"Struct",
//              "description"=>"",
//              "encoding-type"=>"Object",
//              "elements"=>[{"name"=>"SomeInterestingNumber",
//                            "type"=>"int32_t",
//                            "description"=>"Example of a number."},
//                           {"name"=>"SomeCoolString",
//                            "type"=>"char *",
//                            "description"=>"Example of a DisplayString."}
//                          ]
//             }
// 
// Results:
// ------------
//
// From this we see, that the parameters to give in this set operation must be a JSON object. In the set operation
// below, this JSON object is given as the last argument in the '...'. The quotation marks '' are used in order for
// the shell to not change the string {"SomeInterestingNumber":"8","SomeCoolString":"xyz"}.
//
// ------------
//  $ ./vson -c -d <IP-address>  call demo.somedata.something.set '{"SomeInterestingNumber":"8", "SomeCoolString":"asd"}'
//  {"SomeInterestingNumber":"8", "SomeCoolString":"asd"}
//  Calling demo.somedata.something.set:
// ------------
//
//
