#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/* You will to add includes here */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

// Included to get the support library
#include "calcLib.h"

// Enable if you want debugging to be printed, see examble below.
// Alternative, pass CFLAGS=-DDEBUG to make, make CFLAGS=-DDEBUG
#define DEBUG
#define BUFFER_SIZE 256


using namespace std;


int main(int argc, char *argv[]){
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <ip>:<port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
  
  /*
    Read first input, assumes <ip>:<port> syntax, convert into one string (Desthost) and one integer (port). 
     Atm, works only on dotted notation, i.e. IPv4 and DNS. IPv6 does not work if its using ':'. 
  */
    char delim[] = ":";
    char *Desthost = strtok(argv[1], delim);
    if (Desthost == NULL) {
        fprintf(stderr, "Invalid address\n");
        exit(EXIT_FAILURE);
    }

    char *Destport = strtok(NULL, delim);
    if (Destport == NULL) {
        fprintf(stderr, "Invalid port\n");
        exit(EXIT_FAILURE);
    }
  // *Desthost now points to a sting holding whatever came before the delimiter, ':'.
  // *Dstport points to whatever string came after the delimiter. 

  /* Do magic */
  int port = atoi(Destport);
    if (port == 0) {
        fprintf(stderr, "Invalid port\n");
        exit(EXIT_FAILURE);
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
#ifdef DEBUG  
  printf("Host %s, and port %d.\n",Desthost,port);
#endif


}