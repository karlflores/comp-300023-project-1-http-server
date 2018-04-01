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
//strcture for client_accept_send
struct client_send_t {
  int conn_fd;
  struct sockaddr_in client_addr;
  char *path_root;
  socklen_t client_addr_len;
};

int server(int port,const char *path_root);

//this returns the file pointer for the specific path
// if there is no file it returns NULL
FILE *fetch_file(const char *path_root,const char *file_path);

//checks if the file extensions are valid MIME TYPES that this server is capable of retrieving.
//returns TRUE if valid, FALSE if not
int is_valid_extension(const char *file);

void *client_accept_runner(void *client_struct);

char *build_full_path(const char *path_root,const char *file_path);

//send the file over the socket in chunks
int send_file(const int dest_fd,const int src_fd, const ssize_t src_size);
