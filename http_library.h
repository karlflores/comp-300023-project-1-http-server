#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>


#define MAX_BUFF_SIZE 256
#define FILE_EXT_SIZE 10
//function prototypes

//returns the file type to get
char *process_get_request(const char *s);

int send_response(int sock_df,const char *file_path,int response, int len);
