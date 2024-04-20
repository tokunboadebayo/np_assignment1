

#include <netdb.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_NAME_LEN_MAX 255

#define DEBUG 0

int main(int argc, char *argv[])
{
    char server_name[SERVER_NAME_LEN_MAX + 1] = {0};
    int server_port, socket_fd;


    char message_received[300];


    char delim[] = ":";
    char *Desthost = strtok(argv[1], delim);
    char *Destport = strtok(nullptr, delim);

    // Check if the input is correct
    if (Desthost == nullptr || Destport == nullptr) {
        printf("Usage: %s <ip>:<port>\n", argv[0]);
        exit(1);
    }

    /* Get server name from command line arguments or stdin. */
    strncpy(server_name, Desthost, SERVER_NAME_LEN_MAX);

    /* Get server port from command line arguments or stdin. */
    server_port = atoi(Destport);

    /* Print IP or Host and Port number */
    // Host 127.0.0.1, and port 5000.

    printf("Host %s, and port %d.\n", server_name, server_port);

    struct addrinfo hints, *server_info, *p;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // Support both IPv4 and IPv6
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo hints, *server_info, *p;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // Support both IPv4 and IPv6
    hints.ai_socktype = SOCK_STREAM;

     int status = getaddrinfo(server_name, Destport, &hints, &server_info);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return 1;
    }

    // Extract the first IP address
    struct sockaddr_storage server_addr;
    for (p = server_info; p != NULL; p = p->ai_next) {
        if (p->ai_family == AF_INET || p->ai_family == AF_INET6) {
            memcpy(&server_addr, p->ai_addr, p->ai_addrlen);
            break;
        }
    }

     if (p == NULL) {
        fprintf(stderr, "Failed to extract IP address\n");
        return 2;
    }

    // Convert the IP address to a string and print it
    char ipstr[INET6_ADDRSTRLEN];
    void *addr;
    if (server_addr.ss_family == AF_INET) {
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)&server_addr;
        addr = &(ipv4->sin_addr);
    } else {
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)&server_addr;
        addr = &(ipv6->sin6_addr);
    }

    inet_ntop(server_addr.ss_family, addr, ipstr, sizeof ipstr);
    printf("Connecting to %s\n", ipstr);

    /* Create TCP socket. */
    if ((socket_fd = socket(server_host->h_addrtype, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    /* Connect to socket with server address. */
    if (connect(socket_fd, (struct sockaddr *) &server_address, sizeof server_address) == -1) {
        perror("connect");
        exit(1);
    }

    memset(message_received, 0x00, sizeof(message_received));
    ssize_t  read_status = read(socket_fd, message_received, sizeof(message_received));
    if (read_status == -1) {
        perror("read");
        exit(1);
    }

    // Parse the message received from the server using \n as delimiter
    bool isSupportedProtocol = false;
    char *token = strtok(message_received, "\n");
    while (token != NULL) {
        // check if the incoming version is either 'TEXT TCP 1.0\n' or 'TEXT TCP 1.1\n'
        if (strcmp(token, "TEXT TCP 1.0") != 0 && strcmp(token, "TEXT TCP 1.1") != 0) {
            printf("Server: Error; Incoming Token = %s\n", token);
            break;
        } else {
#if DEBUG
            printf("Server: %s\n", token);
#endif
            isSupportedProtocol = true;
        }
        token = strtok(NULL, "\n");
    }

    if (!isSupportedProtocol) {
        printf("Server: Error; Unsupported Protocol\n");
        exit(1);
    }

    // Send the Response to the
    char response[100];
    memset(response,0x00, sizeof(response));
    sprintf(response,"%s","OK\n");
    int send_status = send(socket_fd, response, strlen(response), 0);
    if (send_status == -1) {
        perror("send");
        exit(1);
    }

    // Expect the server to send a challenge to solve with the syntax '<OPERATION> <VALUE1> <VALUE2>\n'.
    memset(message_received, 0x00, sizeof(message_received));
    read_status = read(socket_fd, message_received, sizeof(message_received));
    if (read_status == -1) {
        perror("read");
        exit(1);
    }

    // print the challenge
    printf("ASSIGNMENT: %s", message_received);


    // Parse the message received from the server.
    char *operation = strtok(message_received, " ");

    // Check if it is floating or interger based
    bool isfloat = operation[0] == 'f';

    double f1, f2 = 0.0;
    int i1, i2 = 0;

    if (isfloat) {
        // parse the floating point values
        f1 = atof(strtok(NULL, " "));
        f2 = atof(strtok(NULL, "\n"));
    } else {
        // parse the integer values
        i1 = atoi(strtok(NULL, " "));
        i2 = atoi(strtok(NULL, "\n"));
    }

    // Perform the operation
    double result = 0.0;
    if (isfloat) {
        if (strcmp(operation, "fadd") == 0) {
            result = f1 + f2;
        } else if (strcmp(operation, "fsub") == 0) {
            result = f1 - f2;
        } else if (strcmp(operation, "fmul") == 0) {
            result = f1 * f2;
        } else if (strcmp(operation, "fdiv") == 0) {
            result = f1 / f2;
        }
    } else {
        if (strcmp(operation, "add") == 0) {
            result = i1 + i2;
        } else if (strcmp(operation, "sub") == 0) {
            result = i1 - i2;
        } else if (strcmp(operation, "mul") == 0) {
            result = i1 * i2;
        } else if (strcmp(operation, "div") == 0) {
            result = i1 / i2;
        }
    }

    // Send the result back to the server
    memset(response, 0x00, sizeof(response));
    if (isfloat) {
        sprintf(response, "%8.8g\n", result);
    } else {
        sprintf(response, "%d\n", (int) result);
    }

    // print the response
#if DEBUG
    printf("Client: %s", response);
#endif

    // Send the Response to the Server
    send_status = send(socket_fd, response, strlen(response), 0);
    if (send_status == -1) {
        perror("send");
        exit(1);
    }

    // read the final response from the server
    memset(message_received, 0x00, sizeof(message_received));
    read_status = read(socket_fd, message_received, sizeof(message_received));
    if (read_status == -1) {
        perror("read");
        exit(1);
    }

    // print the final response
    printf("%s", message_received);


    close(socket_fd);
    return 0;
}