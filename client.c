#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/socket"
// #define BUFFER_SIZE 1024

static int create_socket(void);
static void display_help_message(int exit_code);
static void parse_arguments(int argc, char *argv[]);
static void handle_arguments(int argc, char *argv[]);
static void connect_socket(int fd);

int main(int argc, char *argv[]) {
  int client_fd;
  parse_arguments(argc, argv);
  client_fd = create_socket();
  connect_socket(client_fd);
  exit(EXIT_SUCCESS);
}

static void parse_arguments(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "hwir:")) != -1) {
    switch (opt) {
      // TODO Fix identical branches issue
    case 'h':
      printf("Found flag: %s\n", optarg);
      display_help_message(EXIT_SUCCESS);
      break;
    case 'w':
      printf("Found flag: %s\n", optarg);
      // TODO handle_arguments
      handle_arguments(argc, argv);
      break;
    case 'i':
      printf("Found flag: %s\n", optarg);
      // TODO handle_arguments
      handle_arguments(argc, argv);
      break;
    case 'r':
      printf("Found flag: %s\n", optarg);
      // TODO handle_arguments
      handle_arguments(argc, argv);
      break;
    case '?':
      printf("Found flag: %s\n", optarg);
      display_help_message(EXIT_SUCCESS);
      break;
    default: // No arguments
      printf("Found unofficial flag\n");
      display_help_message(EXIT_FAILURE);
      break;
    }
  }
}

static void handle_arguments(int argc, char *argv[]) {
  for (int i = 0; i < argc; i++) {
    printf("Argument found: %s\n", argv[i]);
  }
  argv[1][0] = 'f'; // To avoid "constant" issue
}

_Noreturn static void display_help_message(int exit_code) {
  fprintf(stderr, "Usage: \t./client [options] [target] ...\n");
  fputs(" \t./client -w [-i <filename>]\n", stderr);
  fputs(" \t./client -r\n", stderr);
  fputs(" \t./client [-h]\n", stderr);
  fputs("Options:\n", stderr);
  fputs("\t-w \t\t\tWrite message to file\n", stderr);
  fputs("\t\t\t\tBy default, write to ./output.txt\n", stderr);
  fputs("\t-i FILE \t\tFile to be written to\n", stderr);
  fputs("\t-r \t\t\tRead message from file\n", stderr);
  fputs("\t-h \t\t\tDisplay this help message\n", stderr);
  exit(exit_code);
}

static int create_socket(void) {
#ifdef SOCK_CLOEXEC
  int fd = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0);
#else
  int fd = socket(AF_UNIX, SOCK_STREAM, 0); // NOLINT(android-cloexec-socket)
#endif
  if (fd < 0) {
    perror("Error: Cannot create socket.");
    exit(EXIT_FAILURE);
  }
  return fd;
}

static void connect_socket(int fd) {
  struct sockaddr_un address;
  memset(&address, 0, sizeof(address));
  address.sun_family = AF_UNIX;
  strncpy(address.sun_path, SOCKET_PATH, sizeof(address.sun_path) - 1);
  address.sun_path[sizeof(address.sun_path) - 1] = '\0';

  if (connect(fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("Error: Cannot connect to socket.");
    close(fd);
    exit(EXIT_FAILURE);
  }
}
