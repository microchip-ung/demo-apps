This module shall be placed under vtss_appl, i.e., vtss_appl/demo/
 $ cd <webstax-source-root>/vtss_appl
 $ git clone git@github.com:microchip-ung/demo-appl demo
 $ cd demo
 $ git checkout 4.6

Preparation:
------------
Goto the build directory. If you have not already setup a link 'config.mk', then run
'make' which will provide you with a list of posibilities. Choose one, e.g.,
'tools/set_build_config.rb -c configs/smb_multi.mk -s smb_jr2_24.mfi'.

To make life easy, generate tags, i.e., 'make TAGS'. Then you have tags for the emacs
editor. That will be usefull in the following.


Build simple application:
-------------------------
1)   $ cp module_demo.in_simple ../../build/make/module_demo.in

1.5) $ cd to-top-of-sdk-tree
     $ patch -p1 < vtss_appl/demo/patch-to-simple.patch
     Applying the patch is the same as doing step 2-4 below.
     
2) If the line 'Custom/AddModules =' does not exist in config.mk, then add the line
'Custom/AddModules = demo'. If it does exist, then add 'demo' to the end of the
space seperated list, e.g., 'Custom/AddModules = xxx yyy demo'

3) Add VTSS_MODULE_ID_DEMO to vtss_appl/include/vtss/appl/vtss_module_id.h.
If you use tags in your editor, then search for VTSS_MODULE_ID_NONE.
You will have to edit 3 places. See the instructions just above
VTSS_MODULE_ID_NONE in this file.

4) Hook up the demo module with the Vitesse application, i.e., edit 
vtss_appl/main/main.c. Search for VTSS_SW_OPTION_HTTPS in that file,
and do something similar, where you just change the HTTPS with DEMO.
Three thing must be done:
 a) Include the demo_api.h if VTSS_SW_OPTION_DEMO is defined.
 b) Add the demo_init() function to the initfun[] list, if
    VTSS_SW_OPTION_DEMO is defined.
 c) Add the demo_error_txt() function to the error_txt() function,
    if VTSS_SW_OPTION_DEMO is defined.

5) In the build directory run 'make clobber'. This is because we have
change the make system. And then run 'make -j' or just 'make'.

6) Upload to code to the target. When it eventually boot, you should
be able to see some of the printf statements in the demo_simple.c


Build more advanced application:
--------------------------------
1) cp module_demo.in_advance ../../build/make/module_demo.in

1.5) $ cd to-top-of-sdk-tree
     $ patch -p1 < vtss_appl/demo/patch-to-simple.patch
     Applying the patch is the same as doing step 2-4 from the simple application + step 2,3 below.

Step 2-4 from the simpler application should be done now, if you have
not tried out the simple application, which you should have done.

2) The demo_icfg.c application need some defines in vtss_appl/icfg/icfg_api.h
in order to compile. Follow the instructions in the demo_icfg.c file.

3) All web for demo to vtss_appl/web/menu_default.cxx.

4) In the build directory run 'make clobber'. This is because we have
change the make system. And then run 'make -j' or just 'make'.

5) Upload to code to the target. When it eventually boot, you should
be able to see some of the printf statements in the demo.c
