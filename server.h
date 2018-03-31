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
#include <sys/uio.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "http_library.h"
#include <sys/sendfile.h>

#define CONNECTION_BACKLOG 10
#define BUFFER_SIZE 256
#define TRUE 1
#define FALSE 0

int server(int port,const char *path_root);

//this returns the file pointer for the specific path
// if there is no file it returns NULL
FILE *fetch_file(const char *path_root,const char *file_path);

//checks if the file extensions are valid MIME TYPES that this server is capable of retrieving.
//returns TRUE if valid, FALSE if not
int is_valid_extension(const char *file);
