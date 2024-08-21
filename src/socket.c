#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "event.h"

#define SOCKET_LEN 1024

static pthread_t p;

static int sockfd;
static char buffer[SOCKET_LEN];
static struct sockaddr_in addr_here, addr_there;

void *socket_loop(void *x) {
  (void)x;
  uint32_t len;
  int n;

  while (1) {
    len = sizeof(addr_there);
    n = recvfrom(sockfd, (char *)buffer, SOCKET_LEN, MSG_WAITALL,
                 (struct sockaddr *)&addr_there, &len);
    if (n == -1) {
      printf(">> SOCKET: receive fail\n");
    } else {
      buffer[n] = '\0';
      printf("\033[0;31m");
      printf("%s", buffer); // assuming incoming linebreak?
      printf("\033[0m");
      char *line = malloc((n + 1) * sizeof(char));
      strncpy(line, buffer, n);
      line[n] = '\0';
      union event_data *ev = event_data_new(EVENT_EXEC_CODE_LINE);
      ev->exec_code_line.line = line;
      event_post(ev);
    }
  }
}

void init_socket(int port) {
  int ret;

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    printf(">> SOCKET: init fail\n");
    exit(1);
  }

  do {
    memset(&addr_here, 0, sizeof(addr_here));
    addr_here.sin_family = AF_INET;
    addr_here.sin_addr.s_addr = INADDR_ANY;
    addr_here.sin_port = htons(port);
    ret = bind(sockfd, (const struct sockaddr *)&addr_here, sizeof(addr_here)) <
          0;
    if (ret)
      port++;
  } while (ret);

  if (pthread_create(&p, NULL, socket_loop, 0)) {
    printf(">> SOCKET: init fail pthread\n");
    exit(1);
  }
  #ifdef _GNU_SOURCE
  pthread_setname_np(p, "socket");
  #endif

  printf("socket\t\t%d\n", port);
}
