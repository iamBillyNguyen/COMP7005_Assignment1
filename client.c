#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/socket"
// #define BUFFER_SIZE 1024
//   #define MESSAGE_IDENTIFIER "m:"
//    #define FILE_IDENTIFIER "f:"
//   #define INDETIFIER_ALLOCATED_SIZE 2
#define MESSAGE_LENGTH_ALLOCATED_SIZE 5
#define WRITE_MESSAGE_IDENTIFIER_SIZE 3
//    #define TEST_SIZE 13
//   #define ONE_DIGIT_IDENTIFIER 10
//   #define TWO_DIGITS_IDENTIFIER 100
//   #define THREE_DIGITS_IDENTIFIER 1000

static int create_socket(void);
static void display_help_message(int exit_code);
static void parse_arguments(int argc, char *argv[], int fd);
static void handle_write(int argc, char *argv[], int fd);
static void construct_write_message(char *argv[], size_t len,
                                    char *final_message);
// static void prepend_string(char *s, const char *t);

// static void handle_write(char *argv[], int fd);
//  static void handle_read(int argc, char *argv[], int fd);
//  static char *process_final_message_length(unsigned long length);
static void connect_socket(int fd);

int main(int argc, char *argv[]) {
  int client_fd;
  client_fd = create_socket();
  connect_socket(client_fd);
  parse_arguments(argc, argv, client_fd);

  close(client_fd);
  exit(EXIT_SUCCESS);
}

static void parse_arguments(int argc, char *argv[], int fd) {
  int opt;
  while ((opt = getopt(argc, argv, "hwr:")) != -1) {
    switch (opt) {
    case 'h':
      display_help_message(EXIT_SUCCESS);
      break;
    case 'w':
      handle_write(argc, argv, fd);
      break;
      //    case 'r':
      //      handle_arguments(argc, argv, 'r');
      //      break;
      //    case '?':
      //      display_help_message(EXIT_SUCCESS);
      //      break;
    default: // No arguments
      display_help_message(EXIT_FAILURE);
      break;
    }
  }
}

// TODO Fix
// static void prepend_string(char *s, const char *t) {
//  size_t len = strlen(t);
//  memmove(s + len, s, strlen(s) + 1);
//  memcpy(s, t, len);
//}

static void construct_write_message(char *argv[], size_t len,
                                    char *final_message) {
  char *write_message = malloc(len);
  char len_string[MESSAGE_LENGTH_ALLOCATED_SIZE];
  sprintf(len_string, "%zu", len);
  //  for (size_t i = 0; i < strlen(len_string); i++) {
  //    prepend_string(len_string, "0");
  //  }
  printf("total length as string: %s\n", len_string);
  strncpy(write_message, len_string, 4);
  strcat(write_message, ";");
  strcat(write_message, "w:");
  strcat(write_message, argv[2]);
  strcat(write_message, ":");
  strcat(argv[3], "\0");
  strcat(write_message, argv[3]);

  strncpy(final_message, write_message, strlen(write_message));
  //  printf("final message: %s\n", final_message);
  free(write_message);
}

static void handle_write(int argc, char *argv[], int fd) {
  char *final_message;
  size_t final_message_len;
  for (int i = 0; i < argc; i++) {
    printf("Argument found: %s\n", argv[i]);
  }

  if (argc < 4) {
    display_help_message(EXIT_FAILURE);
  }

  final_message_len = MESSAGE_LENGTH_ALLOCATED_SIZE // 	"total_length:"
                      + strlen(argv[2]) +
                      strlen(argv[3]) //	"f:message:file_location"
                      + WRITE_MESSAGE_IDENTIFIER_SIZE + 1; // 	"\0"
  final_message = malloc(final_message_len);
  construct_write_message(argv, final_message_len, final_message);
  write(fd, final_message, final_message_len);
  free(final_message);
}

// static void handle_read(int argc, char *argv[], int fd) {}

_Noreturn static void display_help_message(int exit_code) {
  fprintf(stderr, "Usage: \t./client [options] [target] ...\n");
  fputs(" \t./client -w <message> <filename>\n", stderr);
  fputs(" \t./client -r <filename>\n", stderr);
  fputs(" \t./client [-h]\n", stderr);
  fputs("Options:\n", stderr);
  fputs("\t-w MESSAGE FILE\t\tWrite message to file\n", stderr);
  fputs("\t-r FILE\t\t\tRead message from file\n", stderr);
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

  fd = connect(fd, (struct sockaddr *)&address, sizeof(address));
  if (fd < 0) {
    perror("Error: Cannot connect to socket.");
    exit(EXIT_FAILURE);
  }
}
