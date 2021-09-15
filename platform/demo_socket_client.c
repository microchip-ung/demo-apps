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



static char recvline[256];

int demo_socket_client(mesa_ip_t ip_addr)
{
  int sockfd, rc;
  struct sockaddr_in servaddr;

  // (1) --- Create an TCP/IP socket
  //
  sockfd = vtss_socket(AF_INET, SOCK_STREAM, 0);
  if (-1==sockfd) {
      T_WG(TRACE_GRP_SOCKET, "Failed to open socket: %s\n", strerror(errno));
    return -1;
  }

  // (2) --- Connect to server
  //
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(4321);
#if 1
  servaddr.sin_addr.s_addr = htonl(ip_addr);

#else // The UNIX way if ip_addr is a char*
  rc = inet_pton(AF_INET, ip_addr, (char*)&servaddr.sin_addr);
  if (rc!=1) {
    T_WG(TRACE_GRP_SOCKET, "Invalid IP address %s\n", ip_addr);
    return -2;
#endif

  rc = connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
  if (-1==rc) {
    T_WG(TRACE_GRP_SOCKET, "Failed to open socket: %s\n", strerror(errno));
    return -2;
  }

  T_DG(TRACE_GRP_SOCKET, "Client connect succeeded\n");

  // (3) --- Read
  //
  rc = read(sockfd, recvline, sizeof(recvline));
  if (rc>0) recvline[rc]=0;
  printf("%d %s", rc, recvline);

  T_NG(TRACE_GRP_SOCKET, "%d %s", rc, recvline);
  
  close(sockfd);
  
  return 0;
}
