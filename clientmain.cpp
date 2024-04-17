
#include <netdb.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_NAME_LEN_MAX 255

#define DEBUG 1

int main(int argc, char *argv[])
{
    char server_name[SERVER_NAME_LEN_MAX + 1] = {0};
    int server_port, socket_fd;
    struct hostent *server_host;
    struct sockaddr_in server_address;
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

    /* Get server host from server name. */
    server_host = gethostbyname(server_name);

    /* Initialise IPv4 server address with server host. */
    memset(&server_address, 0, sizeof server_address);
    server_address.sin_family = server_host->h_addrtype;
    server_address.sin_port = htons(server_port);
    memcpy(&server_address.sin_addr.s_addr, server_host->h_addr, server_host->h_length);

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
    read(socket_fd, message_received, sizeof(message_received));

    // Parse the message received from the server using \n as delimiter
    char *token = strtok(message_received, "\n");
    while (token != NULL) {
        // check if the incoming version is either 'TEXT TCP 1.0\n' or 'TEXT TCP 1.1\n'
        if (strcmp(token, "TEXT TCP 1.0") != 0 && strcmp(token, "TEXT TCP 1.1") != 0) {
            printf("Server: Error; Incoming Token = %s\n", token);
            break;
        } else {
            printf("Server: %s\n", token);
        }
        token = strtok(NULL, "\n");
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
    int read_status = read(socket_fd, message_received, sizeof(message_received));
    if (read_status == -1) {
        perror("read");
        exit(1);
    }

    // print the challenge
#ifdef DEBUG
    printf("Server: %s\n", message_received);
#endif

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
    double result;
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
#ifdef DEBUG
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
#ifdef DEBUG
    printf("Server: %s\n", message_received);
#endif

    close(socket_fd);
    return 0;
}