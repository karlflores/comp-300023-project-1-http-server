#include "server.h"

// main server function -- this starts the server
int server(int port,const char *path_root){

  //struct addrinfo hints, *server_info;
  struct protoent *protocol_struct;
  //get the transport protocol
  protocol_struct = getprotobyname("tcp");

  //socket file descriptor
  int socket_fd;

  //server and client address structre -- set to zero
  struct sockaddr_in server_addr, client_addr;
  memset(&server_addr,0,sizeof(server_addr));
  memset(&client_addr,0,sizeof(client_addr));

  //set the length of the client address
  socklen_t client_addr_len;
  client_addr_len = sizeof(client_addr);

  //set attributes of server_addr to required fields
  server_addr.sin_family = AF_INET;
  //turn the port to machine neutral format
  server_addr.sin_port = htons(port);
  //connect to any address
  server_addr.sin_addr.s_addr = INADDR_ANY;

  //pthread declaration and pthread_attr declaration
  //create an array of pthread ids
  pthread_t pt_ids[NUM_THREADS];
  //create the pthread attr for pthread_create;
  pthread_attr_t pt_attr;

  //create the client struct for the runner function
  struct client_send_t client;

  //create the TCP socket
  socket_fd = socket(PF_INET, SOCK_STREAM,protocol_struct->p_proto);

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
  // introduce pthreads on the accept command since it returns a new socket_fd
  //accept will be called on the new pthread and it will block until it has
  //a connection to connect to

  //set the client_struct attributes for the pthread function call
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

      //create the pthreads and call the client accept function
      int pt = pthread_create(&pt_ids[i],&pt_attr,client_accept_runner,(void*)&client);

      //error checking on the thread that has been created
      if(pt !=0){
        perror("ERROR: could not create thread\n");
        exit(EXIT_FAILURE);
      }
      printf("THREAD %d Created - status: %lu\n",i,pt_ids[i]);
    }

    //wait for all of the threads to finish processing files
    for(int i = 0;i<NUM_THREADS;i++){
      int pt = pthread_join(pt_ids[i],NULL);
      //error checking for join
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

//returns true if the extension is valid according to the mime types of the
//server
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

//builds the full file path from the path_root and a file_path string
//returns an allocated memory space -- therefore need to free after use
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

//no need for mutex lock -- no race conditions as not accessing a shared
//variable -- open file in read_only -> returns a new file descriptor pointing
//to the same file.
void *client_accept_runner(void *client_struct){


  //check if client_struct points to null
  if(client_struct == NULL){
    perror("ERROR: client_accept_runner - NULL pointer argument\n");
    pthread_exit(0);
  }
  //typecase the client_struct to the relavent pointer type
  struct client_send_t *client;
  client = (struct client_send_t*)client_struct;

  //get the contents of the strcut in to seperate variables
  int socket_fd = client->conn_fd;
  struct sockaddr_in client_addr = client->client_addr;
  socklen_t client_addr_len = client->client_addr_len;
  char *path_root = client->path_root;

  // http response struct
  struct http_struct http_response;

  //initialise the buffer
  char buffer[BUFFER_SIZE];
  memset(buffer,0,sizeof(buffer));

  //client socket, error checking num, file that is to be sent fd initialisation
  int client_sockfd, n, file_send_fd;

  //accept a connection from the client -- block until a connection is present
  if((client_sockfd = accept(socket_fd,(struct sockaddr*)&client_addr,
    &client_addr_len)) < 0){
      //then there is an error
      perror("ERROR: Could not accept connection\n");
      close(socket_fd);

      //exit the thread
      pthread_exit(0);
  }
  // set the the socket file descriptor in the http struct
  http_response.sock_fd = client_sockfd;

  printf("ACCEPTED REQUEST FROM CLIENT %d\n",client_sockfd);

  //know that if we get here, then we have accepted a connection
  //read from socket into the buffer
  if((n = read(client_sockfd,buffer,255)) <0){
    perror("ERROR: Could not read from socket");
    close(client_sockfd);
    pthread_exit(0);
  }

  fprintf(stdout,"REQUEST: %s\n",buffer);

  //get the file path
  char *file_path = process_get_request(buffer);

  // set the file path fields and the sock file descriptor fields of the
  // http response struct -- these wont change during the call
  http_response.file_path = file_path;

  //process the file path
  char *full_filepath = build_full_path(path_root,file_path);

  if(is_valid_extension(file_path)== FALSE){
    perror("ERROR: file extension is not valid\n");
    //send 404 response if the file extension is not valid
    http_response.response = 404;
    n = send_response(&http_response);
    if(n!=0){
      //could not send response
      perror("ERROR: could not send HTTP response\n");
    }
    close(client_sockfd);
    pthread_exit(0);
  }

  //open the file -- check for errors too
  if((file_send_fd = open(full_filepath,O_RDONLY)) < 0){
    perror("ERROR: could not find file -- need to send 404\n");
    //send 404 response then exit the pthread;
    http_response.response = 404;

    if((n = send_response(&http_response))!=0){
      perror("ERROR: could not send HTTP/1.0 response.\n");
    }
    close(client_sockfd);
    pthread_exit(0);
  }

  //if we get to this stage, the file does exist, therefore we can send it
  //over the socket

  //send the HTTP request first -- send a 200 response
  http_response.response = 200;

  if((n = send_response(&http_response))<0){
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

  //sleep(1);
  pthread_exit(0);
}
