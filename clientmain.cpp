
#include <netdb.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_NAME_LEN_MAX 255

int main(int argc, char *argv[])
{
    char server_name[SERVER_NAME_LEN_MAX + 1] = { 0 };
    int server_port, socket_fd;
    struct hostent *server_host;
    struct sockaddr_in server_address;
    char message_received[300];


    char delim[]=":";
    char *Desthost=strtok(argv[1],delim);
    char *Destport=strtok(NULL,delim);

    /* Get server name from command line arguments or stdin. */
    strncpy(server_name, Desthost, SERVER_NAME_LEN_MAX);

    /* Get server port from command line arguments or stdin. */
    server_port =  atoi(Destport);

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
    if (connect(socket_fd, (struct sockaddr *)&server_address, sizeof server_address) == -1) {
        perror("connect");
        exit(1);
    }

    memset(message_received, 0x00, sizeof(message_received));
    read(socket_fd, message_received, sizeof(message_received));

    // Parse the message received from the server using \n as delimiter
    char *token = strtok(message_received, "\n");
    while (token != NULL)
    {
        // check if the incoming version is either 'TEXT TCP 1.0\n' or 'TEXT TCP 1.1\n'
        if (strcmp(token, "TEXT TCP 1.0") != 0 && strcmp(token, "TEXT TCP 1.1") != 0)
        {
            printf("Server: Error; Incoming Token = %s\n", token);
            break;
        } else {
            printf("Server: %s\n", token);
        }
        token = strtok(NULL, "\n");
    }
}