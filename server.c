#include "server.h"
//this returns the
int server(int port,const char *path_root){

  //struct addrinfo hints, *server_info;
  struct protoent *protocol_struct;
  //get the transport protocol
  protocol_struct = getprotobyname("tcp");

  //socket file descriptor
  int socket_fd;

  socklen_t client_addr_len;

  //sever address structre
  struct sockaddr_in server_addr, client_addr;
  memset(&server_addr,0,sizeof(server_addr));
  memset(&client_addr,0,sizeof(client_addr));
  server_addr.sin_family = AF_INET;
  //turn the port to machine neutral format
  server_addr.sin_port = htons(port);
  //connect to any address
  server_addr.sin_addr.s_addr = INADDR_ANY;

  //set the hints struct
  //memset(&hints,0,sizeof(hints));
  //hints.ai_family = AF_UNSPEC;
  //hinst.ai_socktype = SOCK_STREAM;
  //hinst.ai_flags = AI_PASSIVE;

  //pthread declaration and pthread_attr declaration
  pthread_t pt_ids[NUM_THREADS];
  pthread_attr_t pt_attr;


  //create the TCP socket
  socket_fd = socket(AF_INET, SOCK_STREAM,protocol_struct->p_proto);
  //fprintf(stdout,"%d\n",socket_fd);

  //bind the socket to the specific port
  if(bind(socket_fd,(struct sockaddr*)&server_addr,sizeof(server_addr)) < 0){
    perror("ERROR: Unable to bind socket\n");
    close(socket_fd);
    exit(EXIT_FAILURE);
  }

  //we know that the bind is successful
  //listen on the port, put incoming connections into a queue of length
  //CONNECTION_BACKLOG
  listen(socket_fd,CONNECTION_BACKLOG);
  //printf("LISTENING ON SOCKET\n");
  // introduce pthreads on the accept command since it returns a new socket_fd
  // descriptor to communicate on -- each connection will be a new request
  // process each new request on another pthread

  //lets just process one request for now -- add the implementation of multi-
  //threading later

  client_addr_len = sizeof(client_addr);

  //create the client struct for the runner function
  struct client_send_t client;
  client.conn_fd = socket_fd;
  client.client_addr = client_addr;
  client.path_root = (char*)malloc(sizeof(char)*(strlen(path_root)+1));
  strcpy(client.path_root,path_root);
  client.client_addr_len = client_addr_len;

  //accept connections until user terminates the server
  while(TRUE){

    for(int i = 0;i<NUM_THREADS;i++){
      //initialise the pthread attributes
      pthread_attr_init(&pt_attr);

      int pt = pthread_create(&pt_ids[i],&pt_attr,client_accept_runner,(void*)&client);
      if(pt !=0){
        perror("ERROR: could not create thread\n");
        exit(EXIT_FAILURE);
      }
      printf("THREAD %d Created - status: %d\n",i,pt);
    }

    //wait for the thread to finish processing

    for(int i = 0;i<NUM_THREADS;i++){
      int pt = pthread_join(pt_ids[i],NULL);
      if(pt!=0){
        perror("ERROR: unable to join thread.\n");
      }
      printf("THREADS %d JOINED - status: %d\n",i,pt);
    }
}
  //close the socket
  close(socket_fd);

  //free the allocated memory
  free(client.path_root);

  return 0;
}

int is_valid_extension(const char *file){
  char buffer[BUFFER_SIZE];
  strcpy(buffer,file);

  //tokenise the file based on '.'
  char *token = strtok(buffer,".\n ");
  if(token== NULL){
    return FALSE;
  }
  //tokenise again to get the filetype
  token = strtok(NULL,".\n");
  //printf("FILE EXT: %s",token);

  // check if the token is a valid file extension
  if(strcmp(token,"js")==0){
    return TRUE;
  }else if(strcmp(token,"html")==0){
    return TRUE;
  }else if(strcmp(token,"jpg")==0){
    return TRUE;
  }else if(strcmp(token,"css")==0){
    return TRUE;
  }else{
    return FALSE;
  }


}

char *build_full_path(const char *path_root,const char *file_path){

    int final_filepath_len = strlen(file_path) + strlen(path_root);

    char *full_filepath = (char*)malloc(sizeof(char)*(final_filepath_len+1));
    int path_index = 0;

    for(int i = 0;i<strlen(path_root);i++,path_index++){
      full_filepath[path_index] = path_root[i];
    }
    for(int i = 0;i<strlen(file_path);i++,path_index++){
      full_filepath[path_index] = file_path[i];
    }
    full_filepath[path_index] = '\0';
    fprintf(stderr,"%s\n",full_filepath);
    return full_filepath;
}
//function for pthread processing
//this handles the incoming connection requests by a thread
void *client_accept_runner(void *client_struct){
  struct client_send_t *client;
  client = (struct client_send_t*)client_struct;

  //get the contents of the strcut
  int socket_fd = client->conn_fd;
  struct sockaddr_in client_addr = client->client_addr;
  socklen_t client_addr_len = client->client_addr_len;
  char *path_root = client->path_root;

  char buffer[BUFFER_SIZE];
  memset(buffer,0,sizeof(buffer));

  int client_sockfd, n, file_send_fd;

  if((client_sockfd = accept(socket_fd,(struct sockaddr*)&client_addr,
    &client_addr_len)) < 0){
      //then there is an error
      perror("ERROR: Could not accept connection\n");
      close(socket_fd);
      pthread_exit(0);
  }
  printf("ACCEPTED MESSAGE FROM CLIENT %d\n",client_sockfd);

  //know that if we get here, then we have accepted a connection

  if((n = read(client_sockfd,buffer,255)) <0){
    perror("ERROR: Could not read from socket");
    close(client_sockfd);
    pthread_exit(0);
  }

  fprintf(stdout,"REQUEST: %s\n",buffer);

  //get the file path
  char *file_path = process_get_request(buffer);
  //printf("FILE PATH: %s : VALID: %d\n",file_path,is_valid_extension(file_path));


  //process the file path
  char *full_filepath = build_full_path(path_root,file_path);
  //printf("FINAL PATH: %s",full_filepath);

  //open the file
  if(is_valid_extension(file_path)== FALSE){
    perror("ERROR: file extension is not valid\n");
    close(client_sockfd);
    pthread_exit(0);
  }
  //open the required file
  if((file_send_fd = open(full_filepath,O_RDONLY)) < 0){
    perror("ERROR: could not find file -- need to send 404\n");
    //send 404 response then exit the pthread;
    n = send_response(client_sockfd,file_path,404);
    if(n!= 0){
      perror("ERROR: could not send HTTP/1.0 response.\n");
    }
    close(client_sockfd);
    pthread_exit(0);
  }

  //if we get to this stage, the file does exist, therefore we can send it
  //over the socket

  //send the HTTP request first
  n = send_response(client_sockfd,file_path,200);
  if(n!= 0){
    perror("ERROR: could not send HTTP/1.0 response.\n");
  }
  //get the size of the file using stat
  //create the stat struct and use stat
  struct stat file_stat;
  stat(full_filepath,&file_stat);

  //send the file over the socket. need to implement non sendfile

  //replace this with write/read -- need to make a function to read a file
  //in chunks and send through the socket
  if((n = sendfile(client_sockfd,file_send_fd,NULL,file_stat.st_size)) < 0){
    //error has occured sending the file
    perror("ERROR: could not send file\n");
    close(client_sockfd);
    pthread_exit(0);
  }

  //if we get down here we have done all we needed and we can exit the
  //pthread;
  close(file_send_fd);
  close(client_sockfd);

  //free allocated memory
  free(file_path);
  free(full_filepath);
  pthread_exit(0);
}
/*
//send the file over the socket in chunks
int send_file(const int dest_fd,const int src_df, ssize_t src_size){
  ssize_t bytes_read = 0;
  ssize_t bytes_write = 0;
  ssize_t curr_read = 0;
  ssize_t curr_write = 0;
  //set the buffer up
  char buffer[MAX_BUFF_SIZE];
  //clear the buffer
  memset(buffer,0,sizeof(buffer));
  while(bytes_read<src_size){
    //read into the buffer
    curr_read = read(src_df,buffer,MAX_BUFF_SIZE);
    if(curr_read == 0){
      //did not read any bytes
    }else if(curr_read < 0){
      perror("ERROR: could not read into buffer.\n");
      return -1;
    }
    bytes_read+=curr_read;
  }

  //read into the buffer




}
*/
