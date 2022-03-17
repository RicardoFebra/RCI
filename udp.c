#include "Libs.h"

/************************************************************************************************
 * Function communication 
 * 
 * This function deals with the communication between the node and the machine "tejo"
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * Returns true or false 
 *
 * 
 *************************************************************************************************/

bool communication(char RegOrUnreg[], char MessageGiven[],socklen_t addrlen, struct sockaddr *addr1, struct addrinfo *res, char *argv[], char name_network[], int fd, char **tcp_id, char **tcp_ip,char **tcp_port){

  char message[300],buffer[127], *ptr;
  ssize_t n;
  int nn;
  fd_set IsIt;
  struct timeval *t;
  char aux_char[20]="NODESLIST ";

    t=(struct timeval *)calloc(1,sizeof(struct timeval));
    t->tv_sec=10;
    t->tv_usec=1;
    

    if (strcmp(RegOrUnreg,"REG")==0 || strcmp(RegOrUnreg,"UNREG")==0 ){ 
      sprintf(message, "%s %s %s %s",RegOrUnreg, name_network, argv[1], argv[2]); 
      n=sendto(fd,message,strlen(message), 0, res->ai_addr, res->ai_addrlen);
      if(n==-1){
        printf("%s", strerror(errno));
        free(t);
        /*error*/return false;
      }
    }else{
      sprintf(message, "%s %s",RegOrUnreg, name_network);  
      n=sendto(fd,message,strlen(message), 0, res->ai_addr, res->ai_addrlen);
      if(n==-1){
        printf("Couldn't send to server\n");
        free(t);
        /*error*/return false;
      }
    }

    FD_ZERO(&IsIt);
    FD_SET(0,&IsIt);
    FD_SET(fd,&IsIt);

    n=select(fd+1,&IsIt,NULL, NULL, t); //checks what file descriptor is ready to be read, integer "n" will be 0 if the timeout expired before any file descriptors became ready.
    if (n==0){
      printf("communication select timeout achieved\n\n");
      free(t);
      return false;
    }else if (FD_ISSET(fd,&IsIt) &&  (strcmp(RegOrUnreg,"REG")==0 || strcmp(RegOrUnreg,"UNREG")==0)){ //to register or unregister the node from the server
        addrlen=sizeof(*addr1);
        n=recvfrom(fd,message,200,0,addr1,&addrlen); //
        if(n==-1){
          printf("Couldn't receive from UDP serve\n");
          free(t); /*error*/
        return false;
      }
    }else if(FD_ISSET(fd,&IsIt) && (strcmp(RegOrUnreg,"REG")!=0 && strcmp(RegOrUnreg,"UNREG")!=0)){ //this will alow, for example, to receive the nodes from the server 
      addrlen=sizeof(*addr1);
      n=recvfrom(fd,message,200,0,addr1,&addrlen); //
      strcat(aux_char,name_network);
      strcat(aux_char,"\n\0");
      printf("%s %s\n", message,aux_char);
      if (strcmp(message,aux_char)==0 || n==-1){
        free(t);
        return false;
      }

      if(chooserandomline(&tcp_id,&tcp_ip,&tcp_port, message)==false){
        free(t);
        return false;
      }
      //nn=sscanf(message,"%s %s", buffer, aux_char);
      //strcpy(*tcp_id, &(message)[strlen(buffer)+strlen(aux_char)+2]);*/
    
    }else if(FD_ISSET(0,&IsIt)){
      while (1)
      {
        printf("communication: stdin\n\n");
        fgets(buffer, 127, stdin);
        if (strcmp(buffer,"exit")==0){
          free(t);
          return false;
        }
        else{ printf("You can only exit\n");}
      }
    }
    
    message[n]='\0';
    printf("%s", message);
    if (strcmp(MessageGiven,"\0")!=0){ //
      if(strcmp(message, MessageGiven)==0){ //If 
        printf("The operation %s was successful!\n", RegOrUnreg);
        free(t);

        return true;
      }
    }else{
      free(t);
      return true;
    }
    return false;
}

bool checkport(char *arg, int size) //check if the port number is valid
{
  int i = 0;

  if(size>5) /*error*/ return false; //a number smaller than 65535 has size 5 or less

  if(arg[0]=='-'){  //tests if it's a negative number
    return false;
  } 

  for (i = 0; i < size; i++) //checks if every character of the string is an integer
  {
    if( arg[i]<'0' || arg[i]>'9') return false;
  }
  if(strcmp(arg,"0")==0 || strcmp(arg,"00")==0 || strcmp(arg,"000")==0 || strcmp(arg,"0000")==0 || strcmp(arg,"00000")==0)  return false; // port can't be 0
  if(strcmp(arg,"65535")>0) return false;
  return true;
}

bool chooserandomline(char ***tcp_id, char ***tcp_ip, char ***tcp_port, char buffer[]){
    int i=0, nn=2, numberline=0, randline=0;
    char aux_string[128], *ptr;
    time_t t;

    
    srand((unsigned) time(&t));
    if(sscanf(buffer, "%s%s%s%s", *(*tcp_ip),*(*tcp_port),aux_string,aux_string)==4){
      buffer=buffer+strlen(*(*tcp_ip))+strlen(*(*tcp_port))+1; //jump first two words
      ptr=buffer;
      while(nn==2){
        nn=sscanf(ptr,"%s %s", *(*tcp_ip),*(*tcp_port));
        if(nn==2){
          numberline++;
          ptr=ptr+strlen(*(*tcp_ip))+strlen(*(*tcp_port))+2;
        }
      }
      randline=rand() % numberline; // to choose the random line
      ptr=buffer;
      nn=2;
      while(nn==2){
        nn=sscanf(ptr,"%s %s", *(*tcp_ip),*(*tcp_port));
        if(nn==2){ 
          ptr=ptr+strlen(*(*tcp_ip))+strlen(*(*tcp_port))+2;
          if(ptr[0]=='\t' || ptr[0]=='\n' || ptr[0]==' ') ptr++;
          if(i==randline){ strcpy(*(*tcp_id),*(*tcp_ip)); strcat(*(*tcp_id)," "); strcat(*(*tcp_id),*(*tcp_port));  break;}
          i++;
        }
      }
      
      return true;
    }
    return false;    
  }
