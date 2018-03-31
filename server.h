#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <inttypes.h>

#define CONNECTION_BACKLOG 10
#define BUFFER_SIZE 256

int server(int port,const char *path_root);

//this returns the file pointer for the specific path
// if there is no file it returns NULL
FILE *fetch_file(const char *path_root,const char *file_path);

int send_response(int status);
