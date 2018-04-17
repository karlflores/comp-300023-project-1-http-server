#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#define MAX_BUFF_SIZE 256
#define FILE_EXT_SIZE 10

// struct definition
struct http_struct{
  int sock_fd;
  char *file_path;
  int response;
};

//function prototypes

//returns the file type to get
char *process_get_request(const char *s);

// int send_response(int sock_fd,const char *file_path,int response);

int send_response(struct http_struct *http_response);
