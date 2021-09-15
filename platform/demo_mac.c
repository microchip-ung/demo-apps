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

#include <vtss/appl/types.h>
#include <vtss_api.h>
/* #include <cyg/infra/diag.h> */

#include "demo_api.h"
#include "demo.h"
#include "demo_trace.h"

static const vtss_inst_t       inst=NULL;

static void demo_mac_inc(vtss_mac_table_entry_t *const e)
{
  int i;

  for (i=5; i>-1; --i)
    if (++(e->vid_mac.mac.addr[i]) != 0) break;
}

static void demo_mac_dec(vtss_mac_table_entry_t *const e)
{
  int i;

  for (i=5; i>-1; --i)
    if ((e->vid_mac.mac.addr[i])-- != 0) break;
}

static bool demo_mac_equal(const vtss_mac_table_entry_t *const e, const vtss_mac_table_entry_t *const f)
{

  if (e->vid_mac.vid != f->vid_mac.vid) return FALSE;
  if (memcmp(&e->vid_mac.mac.addr[0], &f->vid_mac.mac.addr[0], 6)) return FALSE;
#if 0
  if (e->destination != f->destination) return FALSE;
  if (e->locked != f->locked) return FALSE;
  if (e->copy_to_cpu != f->copy_to_cpu) return FALSE;
#endif
  return TRUE;
}



static void demo_mac_print(int j, const vtss_mac_table_entry_t *const e)
{
  int i;

  printf("%5d: mac=%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x vid=%u ", j, 
         e->vid_mac.mac.addr[0], e->vid_mac.mac.addr[1], e->vid_mac.mac.addr[2],
         e->vid_mac.mac.addr[3], e->vid_mac.mac.addr[4], e->vid_mac.mac.addr[5],
         e->vid_mac.vid );

  printf("destination port = ");
  for (i=0; i<VTSS_PORT_ARRAY_SIZE; ++i) {
    if (e->destination[i])
      printf("1");
    else
      printf(".");
  }

  if (e->locked)
    printf(" locked");
  else
    printf(" ......");

  if (e->copy_to_cpu)
    printf(" cpu");
  else
    printf(" ...");

  printf("\n");

/*     BOOL           copy_to_cpu;                        /\**< CPU copy flag *\/ */
/*     BOOL           locked;                             /\**< Locked/static flag *\/ */
/*     BOOL           aged;                               /\**< Age flag *\/ */
/* #if defined(VTSS_FEATURE_MAC_CPU_QUEUE) */
/*     vtss_packet_rx_queue_t cpu_queue;                  /\**< CPU queue *\/ */
/* #endif /\* VTSS_FEATURE_MAC_CPU_QUEUE *\/ */

/* #if defined(VTSS_FEATURE_VSTAX_V2) */
/*     struct { */
/*         BOOL               enable;       /\**< Use (UPSID, UPSPN) when adding entry *\/ */
/*         BOOL               remote_entry; /\**< Local or remote entry when getting entry  *\/ */
/*         vtss_vstax_upsid_t upsid;        /\**< UPS identifier  *\/ */
/*         vtss_vstax_upspn_t upspn;        /\**< Logical port within UPS *\/     */
/*     } vstax2; /\**< Unit/port identification *\/ */
/* #endif /\* VTSS_FEATURE_VSTAX_V2 *\/ */

}

void demo_dump_mac_table2(void)
{
  vtss_rc rc;
  int i=0;

  vtss_vid_mac_t vid_mac;
  vtss_mac_table_entry_t entry;

  bzero(&vid_mac, sizeof(vid_mac));

  do {
    rc = vtss_mac_table_get_next(inst, &vid_mac, &entry);
    if (rc) break;

    demo_mac_print(i+1, &entry);

    memcpy(&vid_mac, &entry.vid_mac, sizeof(vid_mac));
  } while (++i<8192);

}


void demo_dump_mac_table(void)
{
  vtss_rc rc;
  int i=1, ii=1, iii=9999;
  int k0, k=0;
  vtss_vid_mac_t vid_mac;
  vtss_mac_table_entry_t entry[2];

  bzero(&vid_mac, sizeof(vid_mac));

  rc = vtss_mac_table_get_next(inst, &vid_mac, &entry[k]);
  if (rc) return;
  memcpy(&vid_mac, &entry[k].vid_mac, sizeof(vid_mac));

  demo_mac_print(i, &entry[k]);

  do {
    k0=k;
    k=k?0:1;
    ++i;

    rc = vtss_mac_table_get_next(inst, &vid_mac, &entry[k]);
    if (rc) break;
    iii=i;
    demo_mac_inc(&entry[k0]);

    if (!demo_mac_equal(&entry[k], &entry[k0])) {

      demo_mac_dec(&entry[k0]);
      if (i-1 != ii) demo_mac_print(i-1, &entry[k0]);

      demo_mac_print(i, &entry[k]);
      ii = i;
    }

    memcpy(&vid_mac, &entry[k].vid_mac, sizeof(vid_mac));
  } while (i<0x2000*4+10);

  if (iii!=ii)  demo_mac_print(iii, &entry[k0]);

}

static  vtss_vid_mac_t vid_mac_dt[] = {
  {1,{0x33,0x33,0x00,0x00,0x00,0x01}},
  {1,{0x33,0x33,0x00,0x00,0x00,0x02}},
  {1,{0x33,0x33,0xff,0x00,0xd0,0x70}},
  {1,{0xff,0xff,0xff,0xff,0xff,0xff}},
  {1,{0x00,0x01,0xc1,0x00,0x8e,0x00}}
};

void demo_mac_del(void)
{
  int i;
  vtss_rc rc;
  vtss_vid_mac_t vid_mac;

  for (i=0; i<5; ++i) {
    rc = vtss_mac_table_del(inst, &vid_mac_dt[i]);
  }

}
