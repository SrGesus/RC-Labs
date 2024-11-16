// getaddrinfo.c
//  Task 2
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(void)
{
  struct addrinfo hints, *res, *p;
  int errcode;
  char buffer[INET_ADDRSTRLEN];
  struct in_addr *addr;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET; // IPv4
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_CANONNAME;
  
  if ((errcode = getaddrinfo("Versailles", NULL, &hints, &res)) != 0)
    fprintf(stderr, "error: getaddrinfo: %s\n", gai_strerror(errcode));
  else
  {
    printf("canonical hostname: %s\n", res->ai_canonname);
    for (p = res; p != NULL; p = p->ai_next)
    {
      addr = &((struct sockaddr_in *)p->ai_addr)->sin_addr;
      printf("internet address: %s (%08lX)\n",
             inet_ntop(p->ai_family, addr, buffer, sizeof buffer),
             (long unsigned int)ntohl(addr->s_addr));
    }
    freeaddrinfo(res);
  }
  exit(0);
}