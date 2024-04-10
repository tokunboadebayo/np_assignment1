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

    /* Bind the client IP Address to server */
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    if (inet_pton(AF_INET, Desthost, &server.sin_addr) <= 0) {
        perror("inet_pton");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Listen and Connect to Server

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("connect");
        close(sock);
        exit(EXIT_FAILURE);
    }

    /* Buffer to accept response from server*/
    char buffer[BUFFER_SIZE];
    if (recv(sock, buffer, BUFFER_SIZE, 0) == -1) {
        perror("recv");
        close(sock);
        exit(EXIT_FAILURE);
    }

    int protocolSupported = 0;
    char *line = strtok(buffer, "\n");
    while (line != NULL) {
        if (strcmp(line, "TEXT TCP 1.0") == 0 || strcmp(line, "TEXT TCP 1.1") == 0) {
            protocolSupported = 1;
            break;
        }
        if (strcmp(line, "") == 0) {
            break;
        }
        line = strtok(NULL, "\n");
    }

    if (!protocolSupported) {
        fprintf(stderr, "Unsupported protocol\n");
        close(sock);
        exit(EXIT_FAILURE);
    }

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(sock, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received == -1) {
            perror("recv");
            break;
        } else if (bytes_received == 0) {
            break;
        }

        buffer[bytes_received] = '\0';
        printf("Server: %s\n", buffer);

        char operation[5];
        double f1, f2, fresult;
        int i1, i2, iresult;
        char response[BUFFER_SIZE];

        if (sscanf(buffer, "%s %lf %lf", operation, &f1, &f2) == 3) {
            if (strcmp(operation, "fadd") == 0) {
                fresult = f1 + f2;
            } else if (strcmp(operation, "fsub") == 0) {
                fresult = f1 - f2;
            } else if (strcmp(operation, "fmul") == 0) {
                fresult = f1 * f2;
            } else if (strcmp(operation, "fdiv") == 0) {
                fresult = f1 / f2;
            } else {
                fprintf(stderr, "Unknown operation '%s'\n", operation);
                close(sock);
                exit(EXIT_FAILURE);
            }
            snprintf(response, BUFFER_SIZE, "%.8g\n", fresult);
        
        } else if (sscanf(buffer, "%s %d %d", operation, &i1, &i2) == 3) {
            if (strcmp(operation, "add") == 0) {
                iresult = i1 + i2;
            } else if (strcmp(operation, "sub") == 0) {
                iresult = i1 - i2;
            } else if (strcmp(operation, "mul") == 0) {
                iresult = i1 * i2;
            } else if (strcmp(operation, "div") == 0) {
                if (i2 == 0) {
                    fprintf(stderr, "Division by zero error\n");
                    close(sock);
                    exit(EXIT_FAILURE);
                }
                iresult = i1 / i2;
            } else {
                fprintf(stderr, "Unknown operation '%s'\n", operation);
                close(sock);
                exit(EXIT_FAILURE);
            }
            snprintf(response, BUFFER_SIZE, "%d\n", iresult);
        } else {
            fprintf(stderr, "Failed to parse the command or unknown operation received: '%s'\n", buffer);
            close(sock);
            exit(EXIT_FAILURE);
        }

        if (send(sock, response, strlen(response), 0) == -1) {
            perror("send");
            close(sock);
            exit(EXIT_FAILURE);
        }

        memset(buffer, 0, BUFFER_SIZE);
        bytes_received = recv(sock, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received == -1) {
            perror("recv");
            break;
        } else if (bytes_received == 0) {
            break; 
        }
        buffer[bytes_received] = '\0';
        printf("Result: %s\n", buffer);
    }

    close(sock);
    return 0;
}