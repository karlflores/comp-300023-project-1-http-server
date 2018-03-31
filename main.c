
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "server.h"
//main file for the server
int main(int argc, char **argv){

  //check if the args are correct
  if(argc != 3){
    perror("\\usage: .\\server [port_num] [path_to_web_root]\n");
    exit(EXIT_FAILURE);
  }

  //read in the path to the root and also the port number
  char path_root[strlen(argv[2])+1];
  int port_num = atoi(argv[1]);
  strcpy(path_root,argv[2]);

  //fprintf(stdout,"PORT NUM: %d\n",port_num);
  //fprintf(stdout,"PATH: %s\n",path_root);
  return 0;

}
