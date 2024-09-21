#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

// Create server socket
void create_socket(int& fd)
{
    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0) < 0))
    {
        perror("Error: Cannot create socket.");
        exit(EXIT_FAILURE);
    }
}

void setup_socket(int& fd)
{
    // TODO Set up server address
}

void bind_socket(int& fd)
{
    // TODO Bind address to server socket
    bind(fd, );
}

void listen_socket(int& fd)
{
    // TODO Listen for client connections
}

void accept_socket(int& fd)
{
    // TODO Accept client connections
}

int main(int argc,  char const* argv[])
{
    int main_socket;
    struct sockaddr_un address;
//    int *server_fd;
//    create_socket(*server_fd);
//    bind_socket(*server_fd);
    // TODO
    int server_fd;
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0) < 0))
    {
        perror("Error: Cannot create socket.");
        exit(EXIT_FAILURE);
    }

    memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) - 1);
    bind(server_fd, );

    exit(EXIT_SUCCESS);
}