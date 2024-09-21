#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/socket"
#define BUFFER_SIZE 1024

// Create server socket
static int create_socket(void) {
  int fd;
#ifdef SOCK_CLOEXEC
  fd = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0);
#else
  fd = socket(AF_UNIX, SOCK_STREAM, 0);
#endif
  if (fd < 0) {
    perror("Error: Cannot create socket.");
    exit(EXIT_FAILURE);
  }
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
}

static void listen_socket(int fd) {
  if (listen(fd, SOMAXCONN) < 0) {
    perror("Error: Cannot listen on socket.");
    close(fd);
    exit(EXIT_FAILURE);
  }
}

static int accept_socket(int fd) {
  ssize_t bytes;
  int new_fd;
  char buffer[BUFFER_SIZE];

  while (1) {
    printf("Waiting for connection...\n");
    new_fd = accept(fd, NULL, NULL);
    printf("Connection accepted.\n");
    while ((bytes = read(new_fd, &buffer, BUFFER_SIZE) > 0)) {
      // TODO parse and handle the bytes
      printf("Received %zd bytes.\n", bytes);
    }
  }
}

int main(void) {
  int server_fd;
  server_fd = create_socket();
  bind_socket(server_fd);
  listen_socket(server_fd);
  accept_socket(server_fd);

  exit(EXIT_SUCCESS);
}
