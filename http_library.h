#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>

#define MAX_BUFF_SIZE 256
#define FILE_EXT_SIZE 10

#define SUCCESS 0
#define FAILURE -1

// struct definition
struct http_struct{
  int sock_fd;
  char *file_path;
  int response;

  // size of the file being sent
  size_t *content_length;
};

//function prototypes

//returns the file type to get
char *process_get_request(const char *s);

int send_response(struct http_struct *http_response);
