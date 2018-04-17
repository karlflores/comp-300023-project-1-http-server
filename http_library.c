#include "http_library.h"
// #include "server.h"

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
}

//sends the htpp response to the specified socket
int send_response(struct http_struct *http_response){

  // convert the struct into relavent data entries

  int sock_fd = http_response->sock_fd;
  char *file_path = http_response->file_path;
  int response = http_response->response;

  char message[MAX_BUFF_SIZE];
  int n;
  if(sock_fd < 0){
    //return FAILED ;
    return FAILURE;
  }

  //get file extension
  char file_path_cpy[strlen(file_path)+1];
  strcpy(file_path_cpy,file_path);

  char *file_ext;
  file_ext = strtok(file_path_cpy,".\n ");
  if(file_ext == NULL){
    return FAILURE;
  }
  //know that the entries after the '.' is the file extension
  file_ext = strtok(NULL,".\n ");

  if(response == 200){
    strcpy(message,"HTTP/1.0 200 OK\r\n");
    if( (n = write(sock_fd,message,strlen(message)))<0){
      perror("ERROR: could not write HTTP response\n");
      return FAILURE;
    }
  }else if(response == 404){
    strcpy(message,"HTTP/1.0 404\r\n");
    if((n = write(sock_fd,message,strlen(message))) < 0){
      perror("ERROR: could not write HTTP response\n");
      return FAILURE;
    }
    //success -- don't need to return the content type
    return SUCCESS;
  }else{
    //if not a 200 or 404 return error
    return FAILURE;
  }
  // at this point we know that it is not a 404 therefore must be a 200 request
  //send the MIME-type: content-type field

  //reset message buffer
  memset(message,0,sizeof(message));

  //determine the content type and send the appropriate HTTP response
  if(strcmp(file_ext,"html") == 0){
    strcpy(message,"Content-Type: text/html\r\n\r\n");

    if((n = write(sock_fd,message,strlen(message))) < 0){
      perror("ERROR: could not write HTTP response\n");
      return FAILURE;
    }

  }else if(strcmp(file_ext,"css") == 0){
    strcpy(message,"Content-Type: text/css\r\n\r\n");

    if((n = write(sock_fd,message,strlen(message))) < 0){
      perror("ERROR: could not write HTTP response\n");
      return FAILURE;
    }

  }else if(strcmp(file_ext,"js") == 0){
    strcpy(message,"Content-Type: text/javascript\r\n\r\n");

    if((n = write(sock_fd,message,strlen(message))) < 0){
      perror("ERROR: could not write HTTP response\n");
      return FAILURE;
    }

  }else if(strcmp(file_ext,"jpg") == 0){
    strcpy(message,"Content-Type: image/jpeg\r\n\r\n");

    if((n = write(sock_fd,message,strlen(message))) < 0){
      perror("ERROR: could not write HTTP response\n");
      return FAILURE;
    }

  }
  //return successs
  return SUCCESS;
}
