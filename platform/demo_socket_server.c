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


static char buffer[256];

int demo_socket_server(mesa_ip_t ip_addr)
{
  int sockfd, connfd, rc;
  struct sockaddr_in servaddr;

  // (1) --- Create an TCP/IP socket
  //
  sockfd = vtss_socket(AF_INET, SOCK_STREAM, 0);
  if (-1==sockfd) {
    T_WG(TRACE_GRP_SOCKET, "Failed to open socket: %s\n", strerror(errno));
    return -1;
  }

  // (2) --- Bind to socket
  //
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(4321);
#if 1
  servaddr.sin_addr.s_addr = ip_addr;

#else // The UNIX way if ip_addr is a char*
  rc = inet_pton(AF_INET, ip_addr, (char*)&servaddr.sin_addr);
  if (rc!=1) {
    T_WG(TRACE_GRP_SOCKET, "Invalid IP address %\n", ip_addr);
    return -2;
  }
#endif
  
  rc = bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
  if (-1==rc) {
    T_WG(TRACE_GRP_SOCKET, "Failed to bind: %s\n", strerror(errno));
    return -2;
  }

  // (3) --- listen
  rc = listen(sockfd, 10);
  if (-1==rc) {
    T_WG(TRACE_GRP_SOCKET, "Failed to listen: %s\n", strerror(errno));
    return -2;
  }

  printf("Accept...\n");
  // (4) --- Accept
  //
  {
    connfd = vtss_accept(sockfd, (struct sockaddr*)NULL, NULL);
    if (-1==connfd) {
      T_WG(TRACE_GRP_SOCKET, "Failed to listen: %s\n", strerror(errno));
      return -2;
    }

    T_IG(TRACE_GRP_SOCKET, "2; rc=%d\n",rc );

    snprintf(buffer, sizeof(buffer), "Hello from remote end\n");

    rc = write(connfd, buffer, strlen(buffer));
    T_IG(TRACE_GRP_SOCKET, "3; rc=%d\n",rc );
    if (-1==rc) {
      T_WG(TRACE_GRP_SOCKET, "Failed to listen: %s\n", strerror(errno));
      return -2;
    }

    rc = close(connfd);
    T_IG(TRACE_GRP_SOCKET, "4; rc=%d\n",rc );
    if (-1==rc) {
      T_WG(TRACE_GRP_SOCKET, "Failed to listen: %s\n", strerror(errno));
      return -2;
    }
  }

  rc = close(sockfd);

  return 0;
}
