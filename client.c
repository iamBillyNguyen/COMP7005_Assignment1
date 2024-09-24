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
//   #define FINAL_LENGTH_ALLOCATED_SIZE 4
//    #define TEST_SIZE 13
//   #define ONE_DIGIT_IDENTIFIER 10
//   #define TWO_DIGITS_IDENTIFIER 100
//   #define THREE_DIGITS_IDENTIFIER 1000

static int create_socket(void);
static void display_help_message(int exit_code);
static void parse_arguments(int argc, char *argv[], int fd);
static void handle_arguments(int argc, char *argv[], int fd);
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
      handle_arguments(argc, argv, fd);
      //      handle_write(argv, fd);
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

static void handle_arguments(int argc, char *argv[], int fd) {
  for (int i = 0; i < argc; i++) {
    printf("Argument found: %s\n", argv[i]);
  }

  argv[2][strlen(argv[2])] = '\0';
  write(fd, argv[2], strlen(argv[2]));
  //  switch (opt) {
  //  	case 'w':
  //      if (argc < 5)
  //	  {
  //		display_help_message(EXIT_FAILURE);
  //      }
  //      break;
  //    case 'r':
  //      if (argc < 4)
  //      {
  //		display_help_message(EXIT_FAILURE);
  //      }
  //      break;
  //    default:
  //      display_help_message(EXIT_FAILURE);
  //      break;
  //  }
}

// static char *process_final_message_length(unsigned long length) {
//   char *result = malloc(FINAL_LENGTH_ALLOCATED_SIZE);
//   char *length_str;
//
//   if (length < ONE_DIGIT_IDENTIFIER) {
//     strncpy(result, "000", 3);
//     length_str = malloc(1);
//   }
//   if (length < TWO_DIGITS_IDENTIFIER) {
//     strncpy(result, "00", 2);
//     length_str = malloc(2);
//   }
//   if (length < THREE_DIGITS_IDENTIFIER) {
//     strncpy(result, "0", 1);
//     length_str = malloc(3);
//   } else {
//     length_str = malloc(4);
//   }
//   // int to string conversion
//   sprintf(length_str, "%d", (int)length);
//   strcat(result, length_str);
//   printf("Final message length is: %s\n", result);
//
//   free(length_str);
//   return result;
// }

// static void handle_write(char *argv[], int fd) {
//   size_t message_len = strlen(argv[2]) + INDETIFIER_ALLOCATED_SIZE;
//   char *message = malloc(message_len + 1);
//   size_t final_message_len = FINAL_LENGTH_ALLOCATED_SIZE + message_len + 1;
//   char *length_allocation = malloc(FINAL_LENGTH_ALLOCATED_SIZE);
//   char *final_message = malloc(final_message_len);
//
//   if (strlen(argv[2]) + 1 >
//       BUFFER_SIZE - FINAL_LENGTH_ALLOCATED_SIZE - INDETIFIER_ALLOCATED_SIZE)
//       {
//     perror("Error: Argument exceeds limit of 1024 characters");
//   }
//
//   // Constructing data (identifer + message)
//   strncpy(message, MESSAGE_IDENTIFIER, strlen(MESSAGE_IDENTIFIER));
//   strcat(message, argv[2]);
//   strcat(message, "\0");
//   // Constructing final message (length of data + data)
//   length_allocation = process_final_message_length(message_len + 1);
//   strncpy(final_message, length_allocation, FINAL_LENGTH_ALLOCATED_SIZE);
//   strcat(final_message, message);
//   printf("Final message is: %s\n", final_message);
//   // Write to socket
//   write(fd, final_message, final_message_len);
//
//   free(final_message);
//   free(length_allocation);
//   free(message);
//}

// static void handle_read(int argc, char *argv[], int fd) {}

_Noreturn static void display_help_message(int exit_code) {
  fprintf(stderr, "Usage: \t./client [options] [target] ...\n");
  fputs(" \t./client -w <message> -i <filename>\n", stderr);
  fputs(" \t./client -r -i <filename>\n", stderr);
  fputs(" \t./client [-h]\n", stderr);
  fputs("Options:\n", stderr);
  fputs("\t-w MESSAGE \t\tWrite message to file\n", stderr);
  fputs("\t-r \t\t\tRead message from file\n", stderr);
  fputs("\t-i FILE \t\tFile to be written to\n", stderr);
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
