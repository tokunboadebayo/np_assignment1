#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <iostream>

/* The socket libraries */
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

using namespace std;

bool isValidIPv4(const char *ipAdd)
{
    unsigned int a,b,c,d;
    return (sscanf(ipAdd, "%d.%d.%d.%d", &a,&b,&c,&d) == 4);
}

// Enable if you want debugging to be printed, see examble below.
// Alternative, pass CFLAGS=-DDEBUG to make, make CFLAGS=-DDEBUG
// #define DEBUG

/*
 * error-wrapper for perror
 */
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

// Included to get the support library
//#include <calcLib.h>

int main(int argc, char *argv[])
{
    int port, sockfd;
    struct sockaddr_in clientAddr;
    string ip_address;

    unsigned int sa_len = sizeof(clientAddr);

    if (argc !=2)
    {
		    fprintf(stderr, "usage: %s <ip>:<port>\n", argv[0]);
		    exit(0);
    }
    
  /*
    Read first input, assumes <ip>:<port> syntax, convert into one string (Desthost) and one integer (port). 
     Atm, works only on dotted notation, i.e. IPv4 and DNS. IPv6 does not work if its using ':'. 
  */
 // extract the ip and port from the given argument
  char delim[]=":";
  char *Desthost=strtok(argv[1],delim);
  char *Destport=strtok(NULL,delim);
  // *Desthost now points to a sting holding whatever came before the delimiter, ':'.
  // *Dstport points to whatever string came after the delimiter. 

  /* Do magic */
  port = atoi(Destport);

  if (isValidIPv4(Desthost))
  {
      string ss1(Desthost);
      ip_address = ss1;
  } else
  {
      hostent *record = gethostbyname(Desthost);
      if(record == NULL)
      {
          printf("%s unable to be resolved\n", Desthost);
          exit(1);
      }
      in_addr *address = (in_addr *)record->h_addr;
      ip_address = inet_ntoa(* address);
  }
  
  #ifdef DEBUG 
	printf("Host: %s, Port: %d\n", ip_address.c_str(), port);
  #endif
}
