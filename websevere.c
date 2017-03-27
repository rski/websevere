#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <unistd.h>

#define BACKLOG 10
#define BUFFSIZE 1024

#define NOTIMPLEMENTED "HTTP/1.1 501 Not Implemented\r\n\r\n"
#define OK "HTTP/1.1 200 OK\r\n\r\n"
#define NOTFOUND "HTTP/1.1 404 Not Found\r\n\r\n"
#define FORBIDDEN "HTTP/1.1 403 Forbidden\r\n\r\n"

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
  int new_fd;
  char buff[BUFFSIZE];
  char file_to_serve[BUFFSIZE];

  while(1){
    new_fd = accept(s, (struct sockaddr *) &their_addr, &addr_size);

    // assuming the request got here in one piece
    // quite a bold assumption
    recv(new_fd, buff, BUFFSIZE, 0);
    printf("%s", buff);
    if(strncmp((const char *)buff, "GET", 3) != 0){
      send(new_fd, (const char *) NOTIMPLEMENTED, strlen(NOTIMPLEMENTED), 0);
    }
    else {
      //check if the file is there
      strtok(buff, " ");
      char * path = strtok(NULL, " ");
      printf("%s", path);
      if (strlen(path) == 1 && strncmp((const char *) path, "/", 1) == 0){
        memcpy(file_to_serve, "index.html\0", strlen("index.html")+1);
      }
      else {
        sprintf(file_to_serve, ".%s", path);
      }

      printf("file to serve: %s\n", file_to_serve);
      if (access(file_to_serve, F_OK) != 0){
        perror("Error while looking for file to serve.\n");
        send(new_fd, NOTFOUND, strlen(NOTFOUND), 0);
        close(new_fd);
        continue;
      }
      if (access(file_to_serve, R_OK) !=0){
        perror("File to serve seems to not be readable.\n");
        send(new_fd, FORBIDDEN, strlen(FORBIDDEN), 0);
        close(new_fd);
        continue;
      }
      FILE * file;
      file = fopen(file_to_serve, "r");
      char filebuff[BUFFSIZE];
      int nread = fread(filebuff, 1, sizeof filebuff, file);
      char sendpagebuff[strlen(OK) + nread];

      memcpy(sendpagebuff, OK, strlen(OK));
      memcpy(sendpagebuff+strlen(OK), filebuff, nread);

      send(new_fd, sendpagebuff, strlen(OK) + nread, 0);
    }
    close(new_fd);
  }
  close(s);
  return 0;
}
