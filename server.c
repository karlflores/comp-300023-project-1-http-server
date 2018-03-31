#include "server.h"
//this returns the
int server(int port,const char *path_root){

  //struct addrinfo hints, *server_info;
  struct protoent *protocol_struct;
  //get the transport protocol
  protocol_struct = getprotobyname("tcp");

  //socket file descriptor
  int socket_fd, new_sockfd;

  int n;

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

  // buffer declaration
  char buffer[BUFFER_SIZE];
  //set the hints struct
  //memset(&hints,0,sizeof(hints));
  //hints.ai_family = AF_UNSPEC;
  //hinst.ai_socktype = SOCK_STREAM;
  //hinst.ai_flags = AI_PASSIVE;

  //create the TCP socket
  socket_fd = socket(AF_INET, SOCK_STREAM,protocol_struct->p_proto);
  fprintf(stdout,"%d\n",socket_fd);

  //bind the socket to the specific port
  if(bind(socket_fd,(struct sockaddr*)&server_addr,sizeof(server_addr)) < 0){
    perror("ERROR: Unable to bind socket\n");
    exit(EXIT_FAILURE);
  }

  //we know that the bind is successful
  //listen on the port, put incoming connections into a queue of length
  //CONNECTION_BACKLOG
  listen(socket_fd,CONNECTION_BACKLOG);
  printf("LISTENING ON SOCKET\n");
  // introduce pthreads on the accept command since it returns a new socket_fd
  // descriptor to communicate on -- each connection will be a new request
  // process each new request on another pthread

  //lets just process one request for now -- add the implementation of multi-
  //threading later

  client_addr_len = sizeof(client_addr);

  if((new_sockfd = accept(socket_fd,(struct sockaddr*)&client_addr,
  &client_addr_len)) < 0){
    //then there is an error
    perror("ERROR: Could not accept connection\n");
    exit(EXIT_FAILURE);
  }
  printf("ACCEPTED MESSAGE\n");

  //know that if we get here, then we have accepted a connection
  memset(&buffer,0,sizeof(buffer));
  if((n = read(new_sockfd,buffer,255)) <0){
    perror("ERROR: Could not read from socket");
    exit(EXIT_FAILURE);
  }

  fprintf(stdout,"REQUEST: %s\n",buffer);

  //write the acknowledgement HEADER FILE;
  if((n = write(new_sockfd,"REQUEST RECIEVED\n",18)< 0)){
    perror("ERROR: Could not write to socket\n");
    exit(EXIT_FAILURE);
  }

  //close the socket
  close(socket_fd);


  return 0;
}
