#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define BACKLOG 10
#define BUFFSIZE 1000

int main(int argc, char *argv[])
{
  struct addrinfo hints;
  struct addrinfo *servers, *server;
  int status = 0;
  int yes = 1;
  memset(&hints, 0, sizeof hints);
  // we want an addrinfo that is either ipv4 or 6,
  // that is tcp and since we'll pass NULL to getaddrinfo,
  // and something that is `bind`-able
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  status = getaddrinfo(NULL, "2333", &hints, &servers);
  if (status != 0 ) {
    fprintf(stderr, "gettarrinfo error: %s\n", gai_strerror(status));
    exit(1);
  }
  printf("Here is what we got:\n");
  int s;
  for (server = servers; server !=NULL; server = server->ai_next) {
    if ((s = socket(server->ai_family, server->ai_socktype, server->ai_protocol)) < 0){
      perror("server: could not open a socket");
      continue;
      }
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){ // we want to be able to bind even if socket is in TIME_WAIT
      perror("Failed setting socket options");
      continue;
    }
    if (bind(s, server->ai_addr, server->ai_addrlen) == -1){
      perror("binding to port 2333");
      close(s);
      continue;
    }
    break;

  }
  freeaddrinfo(servers);


  // open a socket on this addrinfo
  printf("socket fd: %d\n", s);
  if (listen(s, BACKLOG) == -1){
    perror("Fatal error while trying to listen");
    exit(1);
  }
  printf("listening\n");

  struct sockaddr_storage * their_addr;
  socklen_t addr_size = sizeof their_addr;
  int new_fd = accept(s, (struct sockaddr *) &their_addr, &addr_size);
  char buff[BUFFSIZE];
  // assuming the request got here in one piece
  // quite a bold assumption
  recv(new_fd, buff, BUFFSIZE, 0);
  printf("%s", buff);

  if(strncmp((const char *)buff, "GET", 3) != 0){
    char * notimplemented = "HTTP/1.1 501 Not Implemented\r\n\r\n";
    send(new_fd, (const char *) notimplemented, strlen(notimplemented), 0);
  }
  else {
    char * ok = "HTTP/1.1 200 OK\r\n\r\n";
    send(new_fd, ok, strlen(ok), 0);
  }
  close(new_fd);
  close(s);
  return 0;
}
