#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
  struct addrinfo hints;
  struct addrinfo *servers, *server;
  int status = 0;
  char ipstr[INET6_ADDRSTRLEN];
  memset(&hints, 0, sizeof hints);
  // we want an addrinfo that is either ipv4 or 6,
  // that is tcp and since we'll pass NULL to getaddrinfo,
  // and something that is `bind`-able
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  status = getaddrinfo(NULL, "80", &hints, &servers);
  if (status != 0 ) {
    fprintf(stderr, "gettarrinfo error: %s\n", gai_strerror(status));
    exit(1);
  }
  printf("Here is what we got:\n");

  for (server = servers; server !=NULL; server = server->ai_next) {
    char *ipver;
    void *addr;
    // yay weak typing
    if (server->ai_family == AF_INET) {
      struct sockaddr_in *ipv4 = (struct sockaddr_in *)server->ai_addr;
      addr = &(ipv4->sin_addr);
      ipver = "IPv4";
    }
    else {
      struct sockaddr_in6 *ipv6 = (struct sockaddr_in6*)server->ai_addr;
      addr = &(ipv6->sin6_addr);
      ipver = "IPv6";
    }
    inet_ntop(server->ai_family, addr, ipstr, sizeof ipstr);
    printf(" %s: %s\n", ipver, ipstr);
  }
  freeaddrinfo(servers);
  return 0;
}
