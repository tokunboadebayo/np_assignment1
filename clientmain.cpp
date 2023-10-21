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
  } else {
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

  /* TCP Socket */
  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
      error("socket:");
  }

  /* Bind the client IP Address to server */

  clientAddr.sin_family = AF_INET;
  clientAddr.sin_port = htons(port);
  inet_pton(AF_INET, ip_address.c_str(), &clientAddr.sin_addr);

  // Listen and Connect to Server
  int cliConn = connect(sockfd, (sockaddr*)&clientAddr, sizeof(clientAddr));
  // Display error on failed connection
  if (cliConn == -1)
  {
      error("connect:");
  }
  getsockname(sockfd, (struct sockaddr *)&clientAddr, &sa_len);
  printf("Connection established with %s local ip and port %s:%d\n", argv[1], inet_ntoa(clientAddr.sin_addr), (int)ntohs(clientAddr.sin_port));

  // Buffer to accept response from server
  char buffer[1024];
  char resBuff[1024];
  char opt[10];                // for storing operator
  double fnum1, fnum2, ftotal; // for storing float values and result
  int inum1, inum2, itotal;    // for storing integer values and result

  // Receive data from the server
  if (recv(sockfd, buffer, sizeof(buffer), 0))
  {
      error("Receive:");
  }
  #ifdef DEBUG
      printf(buffer);
  #endif
  if (strcmp(buffer, "TEXT TCP 1.0\n\n") == 0)
  {
      // Reset buffer array to null
      memset(buffer, '\0', sizeof(buffer));
      strcpy(buffer, "ok\n");  // Send 'Ok' to server to show that TCP protocol is accepted
      if (send(sockfd, buffer, sizeof(buffer), 0) < 0)
      {
          error("send:");
      }
      // Reset buffer array to null  
      memset(buffer, '\0', sizeof(buffer));
      // Receive the assignment from server
      if (recv(sockfd, buffer, sizeof(buffer), 0) < 0)
      {
        error("Assignment:");
      }
      printf("ASSIGNMENT: %s", buffer);

      // Get data based on provided 
      sscanf(buffer, "%s", opt);

      // Perform a check of the operator if it's a float
      if (opt[0] == 'f')
      {
          sscanf(buffer, "%s %lg %lg",opt, &fnum1, &fnum2);
          
          if (strcmp(opt, "fadd") == 0)
          {
              ftotal = fnum1 + fnum2;
          } else if (strcmp(opt, "fsub") == 0)
          {
              ftotal = fnum1 - fnum2;
          } else if (strcmp(opt, "fmul") == 0)
          {
              ftotal = fnum1 * fnum2;
          } else if (strcmp(opt, "fdiv") == 0)
          {
              ftotal = fnum1 / fnum2;
          } 
          
          // Reset buffer array to null
          memset(resBuff, '\0', sizeof(resBuff));
          // Construct the response as a string value
          sprintf(resBuff, "%8.8g\n",ftotal);
          // Send back the result to the server
          resBuff[strlen(resBuff) + 1] = '\0';      
      } else {
 			    sscanf(buffer, "%s %d %d",opt, &inum1, &inum2);

			    if (strcmp(opt, "add") == 0)
			    {
			      	itotal = inum1 + inum2;
		    	} else if (strcmp(opt, "sub") == 0)
		    	{
			      	itotal = inum1 - inum2;
		    	} else if (strcmp(opt, "mul") == 0)
			    {
				      itotal = inum1 * inum2;
          } else if (strcmp(opt, "div") == 0)
          {
              itotal = inum1 / inum2;
          }
          // Reset buffer array to null
          memset(resBuff, '\0', sizeof(resBuff));
          // Construct the result resonse as a string value
          sprintf(resBuff, "%d\n",itotal);
          // Send back the result to  the server
          resBuff[strlen(resBuff)+1] = '\0';
      }

      if (write(sockfd, resBuff, strlen(resBuff)) < 0)
      {
          error("send:");
      }

      // Reset buffer array to null
      memset(buffer, '\0', sizeof(buffer));
      // Receive result of assignment from server
      if (recv(sockfd, buffer, sizeof(buffer), 0) < 0)
      {
          error("recv:");
      }

      #ifdef DEBUG
          printf("Result is calculated as %s", resBuff);
      #endif
          printf("%s(myresult=%s)\n", strtok(buffer,"\n"), strtok(resBuff, "\n"));

    } else {
          close(sockfd);
    }
    return 0;
}     
