#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

void create_socket(int* fd)
{
    if ((*fd = socket(AF_UNIX, SOCK_STREAM, 0) < 0))
    {
        perror("Error: Cannot create socket.");
        exit(EXIT_FAILURE);
    }
}

void setup_socket(int* fd)
{
    // TODO Set up server address
}

void connect_socket(int* fd)
{
    // TODO Connect to server
}

int main(int argc,  char const* argv[])
{
    int main_socket;
    int *server_fd;
    create_socket(server_fd);
    setup_socket(server_fd);
    return 0;
}