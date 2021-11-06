#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

#include "event.h"

#define SOCKET_PORT	 11001
#define SOCKET_LEN   1024

static pthread_t p;

static int sockfd;
static char buffer[SOCKET_LEN];
static struct sockaddr_in addr_here, addr_there;

void *socket_loop(void *x) {
  (void)x;
  uint32_t len;
  int n;

  while(1) {
    len = sizeof(addr_there);
    n = recvfrom(sockfd, (char *)buffer, SOCKET_LEN,
        MSG_WAITALL, ( struct sockaddr *) &addr_there,
        &len);
    if(n == -1) {
      printf(">> SOCKET: receive fail\n");
    }
    else {
      buffer[n] = '\0';
      printf("\033[0;31m");
      printf("%s\n",buffer);
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

void init_socket(void) {
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		printf(">> SOCKET: init fail\n");
		exit(1);
	}
	
	memset(&addr_here, 0, sizeof(addr_here));
	addr_here.sin_family = AF_INET;
	addr_here.sin_addr.s_addr = INADDR_ANY;
	addr_here.sin_port = htons(SOCKET_PORT);
	
	if ( bind(sockfd, (const struct sockaddr *)&addr_here, sizeof(addr_here)) < 0 ) {
		printf(">> SOCKET: init fail bind\n");
		exit(1);
	}
	
  if (pthread_create(&p, NULL, socket_loop, 0)) {
    printf(">> SOCKET: init fail pthread\n");
    exit(1);
  }

  printf(">> SOCKET port 11001\n");
}


