#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define ERROR_IDENTIFIER_SIZE 6
#define SUCCESS_IDENTIFIER_SIZE 8

static int create_socket(void);
static void display_help_message(int exit_code);
static void parse_arguments(int argc, char *argv[], int fd);
static void process_request(int argc, char *argv[], int fd);
_Noreturn static void process_response(int fd);
static void connect_socket(int fd, const char *path) {
  struct sockaddr_un address;
  memset(&address, 0, sizeof(address));
  address.sun_family = AF_UNIX;
  strncpy(address.sun_path, path, sizeof(address.sun_path) - 1);
  address.sun_path[sizeof(address.sun_path) - 1] = '\0';

  fd = connect(fd, (struct sockaddr *)&address, sizeof(address));
  if (fd < 0) {
    perror("Error: Cannot connect to socket.");
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char *argv[]) {
  int client_fd;
  client_fd = create_socket();
  parse_arguments(argc, argv, client_fd);
  process_response(client_fd);

  close(client_fd);
  fflush(stdout);
  exit(EXIT_SUCCESS);
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

static void process_request(int argc, char *argv[], int fd) {
  printf("Request:\n");
  for (int i = 0; i < argc; i++) {
    printf("%s ", argv[i]);
  }
  connect_socket(fd, argv[1]);
  strcat(argv[2], "\0");
  if (write(fd, argv[2], strlen(argv[2]) + 1) < 0) {
    perror("Error: Cannot write to socket.");
  }
  printf("\n");
}

_Noreturn static void display_help_message(int exit_code) {
  fprintf(stderr, "Usage: \t./client [target] ...\n");
  fputs(" \t./client <SOCKET_PATH> <FILE>\n", stderr);
  fputs(" \t./client [-h]\n", stderr);
  fputs("Options:\n", stderr);
  fputs("\t<SOCKET_PATH>\t\tPath to socket (maximum 1024 characters)\n",
        stderr);
  fputs("\t<FILE>\t\t\tPath to file and filename (maximum 1024 "
        "characters)\n",
        stderr);
  fputs("\t-h \t\t\tDisplay this help message\n", stderr);
  exit(exit_code);
}

static void parse_arguments(int argc, char *argv[], int fd) {
  int opt;

  if (argc < 3 || argc > 3) {
    display_help_message(EXIT_FAILURE);
  }

  if (strlen(argv[1]) > BUFFER_SIZE || strlen(argv[2]) > BUFFER_SIZE) {
    display_help_message(EXIT_FAILURE);
  }

  while ((opt = getopt(argc, argv, "h:")) != -1) {
    if (opt == 'h') {
      display_help_message(EXIT_SUCCESS);
    } else // No arguments
    {
      display_help_message(EXIT_FAILURE);
    }
  }
  process_request(argc, argv, fd);
}

_Noreturn static void process_response(int fd) {
  printf("Response:\n");
  while (1) {
    ssize_t bytes;
    char buffer[BUFFER_SIZE + 1];
    char error[ERROR_IDENTIFIER_SIZE + 1];
    char success[SUCCESS_IDENTIFIER_SIZE + 1];

    bytes = recv(fd, &buffer, BUFFER_SIZE, 0);
    printf("%s", buffer);

    memcpy(error, buffer, ERROR_IDENTIFIER_SIZE);
    error[ERROR_IDENTIFIER_SIZE] = '\0';
    memcpy(success, buffer, SUCCESS_IDENTIFIER_SIZE);
    success[SUCCESS_IDENTIFIER_SIZE] = '\0';

    if (strcmp(error, "\nERROR\0") == 0) {
      printf("\nExiting with error\n");
      exit(EXIT_SUCCESS);
    }
    if (strcmp(success, "\nSUCCESS\0") == 0) {
      printf("\nExiting with success\n");
      exit(EXIT_SUCCESS);
    }
    memset(buffer, 0, BUFFER_SIZE);
    memset(error, 0, ERROR_IDENTIFIER_SIZE + 1);
    memset(success, 0, SUCCESS_IDENTIFIER_SIZE + 1);
    fflush(stdout);
    while (bytes > 0) {
      bytes = recv(fd, &buffer, BUFFER_SIZE, 0);
      printf("%s", buffer);

      memcpy(error, buffer, ERROR_IDENTIFIER_SIZE);
      error[ERROR_IDENTIFIER_SIZE] = '\0';
      memcpy(success, buffer, SUCCESS_IDENTIFIER_SIZE);
      success[SUCCESS_IDENTIFIER_SIZE] = '\0';

      if (strcmp(error, "\nERROR\0") == 0) {
        printf("\nExiting with error\n");
        exit(EXIT_SUCCESS);
      }
      if (strcmp(success, "\nSUCCESS\0") == 0) {
        printf("\nExiting with success\n");
        exit(EXIT_SUCCESS);
      }
      memset(buffer, 0, BUFFER_SIZE);
      fflush(stdout);
    }
    if (bytes < 0)
    {
      perror("Error: Cannot read from socket.");
    }
  }
}
