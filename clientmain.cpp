#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/* You will to add includes here */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <iostream>

// Enable if you want debugging to be printed, see examble below.
// Alternative, pass CFLAGS=-DDEBUG to make, make CFLAGS=-DDEBUG
#define DEBUG

// Included to get the support library
#include <calcLib.h>

using namespace std;

int main(int argc, char *argv[]){
  if(argc != 2){
    printf("Invalid input\n");
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

  if(Desthost == NULL || Destport == NULL){
    printf("Invalid input\n");
    exit(1);
  }

  // Converting port no to integer and host ip address to string
  int port = atoi(Destport);
  string ipAddr = Desthost;
  printf("Host %s, and port %d\n", Desthost, port);
  // Tcp socket
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock == -1){
    printf("Error unable to setup socket\n");
    exit(1);
  }

  /* Bind the client IP Address to server */
  struct sockaddr_in clientAddr;
  clientAddr.sin_family = AF_INET;
  clientAddr.sin_port = htons(port);
  inet_pton(AF_INET, ipAddr.c_str(), &clientAddr.sin_addr);

  // Listen and Connect to Server
  int connectRes = connect(sock, (sockaddr*)&clientAddr, sizeof(clientAddr));
  if(connectRes == -1){
    printf("Error unable to establish connection");
  }



#ifdef DEBUG 
  printf("Host %s, and port %d.\n",Desthost,port);
#endif

  
}
