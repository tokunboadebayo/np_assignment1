#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
#include "calcLib.h"

using namespace std;

int main(int argc, char *argv[]){
  if(argc != 2){
    printf("Invalid input, please enter correct arguements\n");
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
  char myAddress[20];
  char *myAdd = myAddress;

  struct sockaddr_in local_sin;
  socklen_t local_sinlen = sizeof(local_sin);
  getsockname(sock,(struct sockaddr*)&local_sin, &local_sinlen);
  inet_ntop(local_sin.sin_family, &local_sin.sin_addr, myAddress, sizeof(myAddress));
  myAdd = myAddress;

  #ifdef DEBUG 
  printf("Connected to %s:%d local %s:%d\n", Desthost, port, myAdd, ntohs(local_sin.sin_port));
  #endif

  // Talk to server
  char buf[1000];    /* Buffer to accept response from server*/
  
  memset(buf, 0, sizeof(buf));
  int bytesReceived = recv(sock, &buf, sizeof(buf), 0);
  printf("%s", buf);

  string ok = "OK\n";

  if(string(buf).find("TEXT TCP") != string::npos){
    int sendRes = send(sock, ok.c_str(), ok.length(), 0);
    if(sendRes == -1){
      printf("Could not send\n");
      exit(1);
    }
    printf("OK\n");    // Send 'Ok' to server to show that TCP protocol is
  }
  else{
    printf("Protocol is unsupported");
    close(sock);
    exit(1);
  }

  memset(buf, 0, sizeof(buf));
  bytesReceived = recv(sock, buf, sizeof(buf), 0);
  //printf("%s", buf);
  char *calc = strtok(buf, " ");
  char *num1 = strtok(NULL, " ");
  char *num2 = strtok(NULL, " ");
  int i1 = 0, i2 = 0, iresult = 0;     // for storing integer values and result
  float f1 = 0, f2 = 0, fresult = 0;   // for storing float values and result
  string result = "";
  printf("ASSIGNMENT: %s %s %s", calc, num1, num2);  // Receive the assignment from server

  //Compute the integer results
  if(string(calc).at(0)!='f'){
    i1 = stoi(num1);
    i2 = stoi(num2);
    if(string(calc) == "add"){
      iresult = i1+i2;
    }
    else if(string(calc) == "sub"){
      iresult = i1-i2;
    }
    else if(string(calc) == "mul"){
      iresult = i1*i2;
    }
    else if(string(calc) == "div"){
      iresult = i1/i2;
    }
  //printf("%8.8g\n",fresult);
  result = to_string(iresult);
  }
  //Compute floating point results
    else{
      f1 = stod(num1);
      f2 = stod(num2);
      if(string(calc) == "fadd"){
        fresult = f1+f2;
      }
      else if(string(calc) == "fsub"){
        fresult = f1-f2;
      }
      else if(string(calc) == "fmul"){
        fresult = f1*f2;
      }
      else if(string(calc) == "fdiv"){
        fresult = f1/f2;
      }
    //printf("%8.8g\n",fresult);
      result = to_string(fresult);
    }
    string result2 = result;
    result += '\n';

    //Send reusults to server

    int sendRes = send(sock, result.c_str(), result.length(), 0);
    if(sendRes == -1){
      printf("Unable to send result\n");
      return 1;
    }

    #ifdef DEBUG
    printf("The calculated result is: %s", result.c_str());
    #endif

    //Get status (OK or ERROR)
    memset(buf, 0, sizeof(buf));
    bytesReceived = recv(sock, buf, sizeof(buf), 0);
    if(string(buf).find("OK") != string::npos){
      printf("OK (my result = %s)\n", result2.c_str());
    }
    else{
      printf("%s", buf);
    }

    //Close Socket
    close(sock);

    return 0;
}
