#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <inttypes.h>
#include <sys/uio.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>

#include "http_library.h"

#define CONNECTION_BACKLOG 10
#define BUFFER_SIZE 256
#define TRUE 1
#define FALSE 0
#define NUM_THREADS 10

// struct containing the information to the thread worker call
struct client_send_t {
  int conn_fd;
  struct sockaddr_in client_addr;
  char *path_root;
  socklen_t client_addr_len;
};

int server(int port,const char *path_root);

//checks if the file extensions are valid MIME TYPES that this server is
//capable of retrieving.
//returns TRUE if valid, FALSE if not
int is_valid_extension(const char *file);

//multithreading function -- processes client connections
void *client_accept_send(void *client_struct);

//build full file path from root path and file path
char *build_full_path(const char *path_root,const char *file_path);

// Own function for sending a file through the socket
int send_file(int client_sock_fd, int file_fd, size_t file_size);

int is_valid_file_path(const char *file_path);
