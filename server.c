#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#define SOCKET_PATH "/tmp/socket"
#define BUFFER_SIZE 1024

static int create_socket(void);

// Create server socket
static int create_socket(void) {
  int fd;
#ifdef SOCK_CLOEXEC
  fd = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0);
#else
  fd = socket(AF_UNIX, SOCK_STREAM, 0); // NOLINT(android-cloexec-socket)
#endif
  if (fd < 0) {
    perror("Error: Cannot create socket.");
    exit(EXIT_FAILURE);
  }
  printf("Socket created successfully.\n");
  return fd;
}

static void bind_socket(int fd) {
  struct sockaddr_un address;
  memset(&address, 0, sizeof(address));
  address.sun_family = AF_UNIX;
  strncpy(address.sun_path, SOCKET_PATH, sizeof(address.sun_path) - 1);
  address.sun_path[sizeof(address.sun_path) - 1] = '\0';

  if (bind(fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("Error: Cannot bind socket.");
    exit(EXIT_FAILURE);
  }
  printf("Socket bind successfully.\n");
}

static void listen_socket(int fd) {
  if (listen(fd, SOMAXCONN) < 0) {
    perror("Error: Cannot listen on socket.");
    close(fd);
    exit(EXIT_FAILURE);
  }
  printf("Socket listen successfully.\n");
}

static int accept_socket(int fd) {
  int client_fd;
  client_fd = accept(fd, NULL, NULL);
  if (client_fd < 0) {
    perror("Error: Cannot accept connection.");
    exit(EXIT_FAILURE);
  }
  printf("Socket accept successfully.\n");
  return client_fd;
}

int main(void) {
  int server_fd;

  server_fd = create_socket();
  bind_socket(server_fd);
  listen_socket(server_fd);
  while (1) {
    int client_fd;
    ssize_t bytes;
    char buffer[BUFFER_SIZE];

    client_fd = accept_socket(server_fd);
    bytes = read(client_fd, &buffer, BUFFER_SIZE);
    while (bytes > 0) {
      bytes = read(client_fd, &buffer, BUFFER_SIZE);
      printf("Received %zd bytes.\n", bytes);
      printf("Received from client: %s\n", buffer);
    }
  }
  close(server_fd);
  exit(EXIT_SUCCESS);
}
