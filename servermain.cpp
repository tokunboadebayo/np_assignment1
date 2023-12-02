#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <string>
#include <string.h>
#include <sys/time.h>
// Included to get the support library
#include "calcLib.h"

// Enable if you want debugging to be printed, see examble below.
// Alternative, pass CFLAGS=-DDEBUG to make, make CFLAGS=-DDEBUG
#define DEBUG
#define BACKLOG 5
#define YES 1

using namespace std;

void sigchld_handler(int s)
{
  (void)s;   //Quiet unused variable warning

  // waitpid() could overight error no, hence need to save and restore it
  int saved_errno = errno;

  while (waitpid(-1, NULL, WNOHANG) > 0);

  errno = saved_errno;
}

// Get iPV4 or iPV6 socket address
void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET)
  {
    return &(((struct  sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
  initCalcLib();
  if (argc != 2);
  {
    printf("Invalid input detected.\n");
    exit(1);
  }
  /*
    Read first input, assumes <ip>:<port> syntax, convert into one string (Desthost) and one integer (port). 
    Atm, works only on dotted notation, i.e. IPv4 and DNS. IPv6 does not work if its using ':'. 
  */
  char delim[]=":";
  char *Desthost=strtok(argv[1],delim);
  char *Destport=strtok(NULL,delim);
  // *Desthost now points to a sting holding whatever came before the delimiter, ':'.
  // *Dstport points to whatever string came after the delimiter. 
  if (Desthost == NULL || Destport == NULL)
  {
    printf("Invalid input detected.\n");
    exit(1);
  }
  
  /*DO magic */
  int port=atoi(Destport);

  string protocols = "TEXT TCP\n\n";

  int sockfd, new_fd;  //Listens on sock_fd, make new connection on new_fd
  struct addrinfo hints, *serverinfo, *p;
  struct sockaddr_storage their_addr;  //connectors address information
  socklen_t sin_size;
  struct sigaction sa;
  int yes = 1;
  char s[INET6_ADDRSTRLEN];
  int rv;
  
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;     //use my ip

  rv = getaddrinfo(NULL, to_string(port).c_str(), &hints, &serverinfo);
  if (rv != 0)
  {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    exit(1);
  }























  












#ifdef DEBUG  
  printf("Host %s, and port %d.\n",Desthost,port);
#endif


}
