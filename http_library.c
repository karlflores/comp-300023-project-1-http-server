#include "http_library.h"
#include "server.h"
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

//sends the response to the socket
int send_response(int sock_df,const char *file_path,int response){
  char message[MAX_BUFF_SIZE];
  int n;
  if(sock_df < 0){
    //return FAILED ;
    return -1;
  }

  //get file extension
  char file_path_cpy[strlen(file_path)+1];
  strcpy(file_path_cpy,file_path);

  char *file_ext;
  file_ext = strtok(file_path_cpy,".\n ");
  if(file_ext == NULL){
    return -1;
  }
  //know that the entries after the '.' is the file extension
  file_ext = strtok(NULL,".\n ");

  if(response == 200){
    strcpy(message,"HTTP/1.0 200 OK\r\n");
    if( (n = write(sock_df,message,strlen(message)))<0){
      perror("ERROR: could not write HTTP response\n");
      return -1;
    }
  }else if(response == 404){
    strcpy(message,"HTTP/1.0 404\r\n");
    if((n = write(sock_df,message,strlen(message))) < 0){
      perror("ERROR: could not write HTTP response\n");
      return -1;
    }
    //success -- don't need to return the content type
    return 0;
  }else{
    //if not a 200 or 404 return error
    return -1;
  }

  //send the content type

  //reset message buffer
  memset(message,0,sizeof(message));

  //determine the content type
  if(strcmp(file_ext,"html") == 0){
    strcpy(message,"Content-Type: text/html\r\n\r\n");

    if((n = write(sock_df,message,strlen(message))) < 0){
      perror("ERROR: could not write HTTP response\n");
      return -1;
    }

  }else if(strcmp(file_ext,"css") == 0){
    strcpy(message,"Content-Type: text/css\r\n\r\n");

    if((n = write(sock_df,message,strlen(message))) < 0){
      perror("ERROR: could not write HTTP response\n");
      return -1;
    }

  }else if(strcmp(file_ext,"js") == 0){
    strcpy(message,"Content-Type: text/javascript\r\n\r\n");

    if((n = write(sock_df,message,strlen(message))) < 0){
      perror("ERROR: could not write HTTP response\n");
      return -1;
    }

  }else if(strcmp(file_ext,"jpg") == 0){
    strcpy(message,"Content-Type: image/jpeg\r\n\r\n");

    if((n = write(sock_df,message,strlen(message))) < 0){
      perror("ERROR: could not write HTTP response\n");
      return -1;
    }

  }
  //return successs
  return 0;
}
