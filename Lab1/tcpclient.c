#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

#define PORT "58001"

int fd, errcode;
ssize_t n;
socklen_t addrlen;
struct addrinfo hints, *res;
struct sockaddr_in addr;
char buffer[128];

int main(void)
{
  fd = socket(AF_INET, SOCK_STREAM, 0); // UDP socket
  if (fd == -1)                         /*error*/
    exit(1);

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;       // IPv4
  hints.ai_socktype = SOCK_STREAM; // UDP socket

  errcode = getaddrinfo("localhost", PORT, &hints, &res);
  if (errcode != 0) /*error*/
    exit(1);

  n = connect(fd, res->ai_addr, res->ai_addrlen);
  if (n == -1) /*error*/
    exit(1);

  n = write(fd, "Hello!\n", 7);
  if (n == -1) /*error*/
    exit(1);

  n = read(fd, buffer, 128);
  if (n == -1) /*error*/
    exit(1);

  write(1, "echo: ", 6);
  write(1, buffer, n);

  freeaddrinfo(res);
  close(fd);
  return 0;
}