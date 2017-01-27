#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define PORTNUMBER  9001 

double relativeBearing = 24.9;
double distance = 10.32;

int main(void)
{
  int n, s;
  socklen_t len;
  int max;
  int number;
  struct sockaddr_in name;

  // create the socket
  if ( (s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }

  memset(&name, 0, sizeof(struct sockaddr_in));
  name.sin_family = AF_INET;
  name.sin_port = htons(PORTNUMBER);
  len = sizeof(struct sockaddr_in);

  // listen on all network interfaces
  n = INADDR_ANY;
  memcpy(&name.sin_addr, &n, sizeof(long));

  int reuse = 1;
  if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0) {
    perror("setsockopt(SO_REUSEADDR)");
    exit(1);
  }

  if (setsockopt(s, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0) {
    perror("setsockopt(SO_REUSEPORT)");
    exit(1);
  }

  // bind socket the network interface
  if (bind(s, (struct sockaddr *) &name, len) < 0) {
    perror("bind");
    exit(1);
  }

  // listen for connections
  if (listen(s, 5) < 0) {
    perror("listen");
    exit(1);
  }

  while(1 == 1) {

    // block until get a request
    int ns = accept(s, (struct sockaddr *) &name, &len);

    if ( ns < 0 ) {
      perror("accept");
      exit(1);
    }

    char buffer[1024];
    int bufferLen = sprintf(buffer, 
	"relativeBearing=%.1f\ndistance=%.2f\n",
	relativeBearing, distance);

    // write response to client
    write(ns, buffer, bufferLen);

    close(ns);
  } 
  
  close(s);
  exit(0);
}

