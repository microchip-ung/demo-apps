static vtss_inst_t  inst = NULL;


static void  print_evc_counter(vtss_ece_id_t  ece_id,
                         vtss_port_no_t port_no,
                         vtss_evc_counters_t  *c)
{
  printf("For ece_id=%d, port_no=%d\n", ece_id, port_no);
  printf("  _______________frames_______bytes__\n");
  printf("  rx_green:  %10llu  %10llu\n",c->rx_green.frames,c->rx_green.bytes);
  printf("  rx_yellow: %10llu  %10llu\n",c->rx_yellow.frames,c->rx_yellow.bytes);
  printf("  rx_red:    %10llu  %10llu\n",c->rx_red.frames,c->rx_red.bytes);
  printf("  rx_discard:%10llu  %10llu\n",c->rx_discard.frames,c->rx_discard.bytes);
  printf("  tx_discard:%10llu  %10llu\n",c->tx_discard.frames,c->tx_discard.bytes);
  printf("  tx_green:  %10llu  %10llu\n",c->tx_green.frames,c->tx_green.bytes);
  printf("  tx_yellow: %10llu  %10llu\n",c->tx_yellow.frames,c->tx_yellow.bytes);
}


// Run this function in order to clear counter in step A: in the email. 
vtss_rc clear_ece_counter()
{
  vtss_rc rc;
  vtss_evc_id_t  evc_id = 0;
  vtss_port_no_t port_no;

  port_no = 0;
  rc = vtss_evc_counters_clear(inst, evc_id, port_no);
  if (rc) return rc;

  port_no = 1;
  rc = vtss_evc_counters_clear(inst, evc_id, port_no);

  return rc;
}


// Run this function in order to read counter in step B: in the email. 
vtss_rc get_ece_counter()
{
  vtss_rc rc;
  vtss_ece_id_t  ece_id = 1;
  vtss_port_no_t port_no;
  vtss_evc_counters_t  counters;


  port_no = 0;
  rc = vtss_ece_counters_get(inst, ece_id, port_no, &counters);
  if (rc) return rc;
  print_evc_counter(ece_id, port_no, &counters);

  port_no = 1;
  rc = vtss_ece_counters_get(inst, ece_id, port_no, &counters);
  if (rc) return rc;
  print_evc_counter(ece_id, port_no, &counters);

  return rc;
}
