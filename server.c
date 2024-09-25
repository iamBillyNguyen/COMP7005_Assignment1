#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

static int create_socket(void);
static void parse_arguments(int argc, char *argv[], int fd);
_Noreturn static void process_client_request(int fd);
// static char* read_data(int fd, char* filename);
static void display_help_message(int exit_code);

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

static void bind_socket(int fd, const char *path) {
  struct sockaddr_un address;
  memset(&address, 0, sizeof(address));
  address.sun_family = AF_UNIX;
  strncpy(address.sun_path, path, sizeof(address.sun_path) - 1);
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

_Noreturn static void display_help_message(int exit_code) {
  fprintf(stderr, "Usage: \t./server [target]\n");
  fputs(" \t./server <SOCKET_PATH>\n", stderr);
  fputs(" \t./server [-h]\n", stderr);
  fputs("Options:\n", stderr);
  fputs("\tSOCKET_PATH\t\tPath to create and link socket to\n", stderr);
  fputs("\t-h \t\t\tDisplay this help message\n", stderr);
  exit(exit_code);
}

static void parse_arguments(int argc, char *argv[], int fd) {
  int opt;
  if (argc < 2 || argc > 2) {
    display_help_message(EXIT_FAILURE);
  }

  while ((opt = getopt(argc, argv, "h:")) != -1) {
    if (opt == 'h') {
      display_help_message(EXIT_SUCCESS);
    } else {
      display_help_message(EXIT_FAILURE);
    }
  }
  unlink(argv[1]);
  bind_socket(fd, argv[1]);
  listen_socket(fd);
}

_Noreturn static void process_client_request(int fd) {
  while (1) {
    int client_fd;
    ssize_t bytes;
    char buffer[BUFFER_SIZE];

    client_fd = accept_socket(fd);
    bytes = read(client_fd, &buffer, BUFFER_SIZE);
    while (bytes > 0) {
      FILE *file;

      printf("Received from client: %s\n", buffer);
      file = fopen(buffer, "re");
      memset(buffer, 0, BUFFER_SIZE);
      printf("fopen is %s\n", (file == NULL) ? "NULL" : "SUCCESS");
      if (file == NULL) {
        printf("Reponse: ERROR opening file.");
        bytes = write(client_fd, "\nERROR - Fail to open file.\0",
                      strlen("\nERROR - Fail to open file.\0"));
        if (bytes < 0) {
          perror("Error: Cannot write to file.");
        }
      } else {
        while (fgets(buffer, sizeof(buffer), file) != NULL) {
          printf("%s", buffer);
          bytes = send(client_fd, buffer, strlen(buffer), 0);
          memset(buffer, 0, BUFFER_SIZE);
        }
        fclose(file);
        write(client_fd, "\nSUCCESS - File read successfully.\0",
              strlen("\nSUCCESS - File read successfully.\0"));
        printf("\n");
        printf("Reponse: File read successfully.\n");
      }
      break;
    }
    if (bytes < 0) {
      perror("Error: Cannot read from socket.");
    }
  }
}

int main(int argc, char *argv[]) {
  int server_fd;

  server_fd = create_socket();
  parse_arguments(argc, argv, server_fd);
  process_client_request(server_fd);

  close(server_fd);
  exit(EXIT_SUCCESS);
}
