#include "http_library.h"

//returns the file_path to the file that needs to be fetched
char *process_get_request(const char *request){
  /* assume that the get request is in the format
    GET file_path HTTP/VERSION
  */
  //copy string to a buffer
  char buffer[MAX_BUFF_SIZE];
  strcpy(buffer,request);

  //tokenise the string
  char *token;
  token = strtok(buffer," ");

  //check if the first token is GET, if not return NULL to indicate an error
  if(strcmp(token,"GET")!=0){
    return NULL;
  }
  token = strtok(NULL," ");
  //if it is a get request get the filepath
  //fprintf(stdout,"GET REQUEST IS: %s\n",token);

  //allocate memory to store the filepath
  char *file_path = (char*)malloc(sizeof(char)*(strlen(token)+1));
  //get filepath from the token
  strcpy(file_path,token);
  return file_path;

  //remember to free the file_path once finished
//
}
