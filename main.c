#include "Libs.h"


int main(int argc, char **argv) {


  /*------------------------------ Initializations--------------------------------*/
int fd=0, length1=0,socket_aux,socket_tcp_client=-1,socket_tcp_server=-1, newfd, maxfd=0,nleft, nwritten, errcode=0, indice=0;  struct addrinfo hints_udp, hints_tcp_server,hints_tcp_client, *res_udp, *res_tcp_client=NULL,*res_tcp_server;
  char *boottcp=NULL, *ptr=NULL,**my_objects=NULL;;
  char *aux_char1,*aux_char2,cmd[64],*msg_received, *buffer, name_network[64], *id, *bootip=NULL, *tcp_id=NULL, *tcp_ip=NULL, *tcp_port=NULL,**cache;
  struct socket_out *socket_out;
  struct recovernode *recovernode;
  ssize_t n, nw;
  socklen_t addrlen;
  fd_set wait;
  struct timeval *t;
  struct sockaddr addr1, addr_tcp;
  int Ip1, Ip2, Ip3, Ip4, aux_int, i,j;  //will help determine if the Ip is correct
  enum {letsreg, connected} state;
  struct nodestree *nodestree, *aux_nodes,*nodes_request,*nos_internos;
  struct socketlist *socketlist, *aux_listsocket, *auxsocketlist;
  bool aux_bool=false;
  struct sigaction act;


/*-------------------------Aloccating and initializing variables---------------------------*/
aux_char1=(char *)calloc(127, sizeof(char));
aux_char2=(char *)calloc(127, sizeof(char));
tcp_id=(char *)calloc(127, sizeof(char));
tcp_ip=(char *)calloc(60, sizeof(char));
tcp_port=(char *)calloc(50, sizeof(char));
boottcp=(char *)calloc(30, sizeof(char));
bootip=(char *)calloc(40, sizeof(char));
ptr=(char *)calloc(128, sizeof(char));
buffer=(char *)calloc(128, sizeof(char));
id=(char *)calloc(80, sizeof(char));
nodestree=NULL;
my_objects=(char**)malloc(20*sizeof(char*)); //cria até 20 objectos

socketlist=NULL;
recovernode=NULL;
nos_internos=NULL;

nodes_request=NULL;

cache=(char**)calloc(2,sizeof(char*));
cache[0]=NULL;
cache[1]=NULL;

/*-------------------------SIGPIPE IGNORED--------------------------------*/
memset(&act,0,sizeof act);
act.sa_handler=SIG_IGN;
if(sigaction(SIGPIPE,&act,NULL)==-1)/*error*/exit(1);

/*-------------------------Validating inputs---------------------------*/
  if(argc!=5){
    fprintf(stderr,"Wrong number of arguments... \nThe arguments are: ndn <Ip> <TCP port> <Server Ip> <Server Port Number>!!\n");
    exit(0);
  }
  if((sscanf(argv[1], "%d.%d.%d.%d.", &Ip1,&Ip2,&Ip3,&Ip4)!=4) || Ip1>255 || Ip2>255 || Ip3>255||Ip4>255){ //checks if it's a possible IP
    fprintf(stderr, "Invalid IP, should have four integers separated by an <.> and each one can't be bigger than 255\n");
    exit(1);
  }
  if(checkport(argv[2],strlen(argv[2]))==false){
    fprintf(stderr, "Wrong Port Number!\nMust be an integer between 0 and 65535\n");
    exit(0);
  }
  if(strcmp(argv[3], "193.136.138.142")!=0 && strcmp(argv[3], "tejo.tecnico.ulisboa.pt")!=0){
    fprintf(stderr, "Wrong IP\n");
    exit(0);
  }
  strcpy(argv[3], "193.136.138.142");  //in case the client writes tejo.tecnico.ulisboa.pt 
  if(checkport(argv[4],strlen(argv[4]))==false){ 
    fprintf(stderr, "Wrong Port Number!\nMust be an integer between 0 and 65535\n");
    exit(0);
  }

/************************************************************************************************************/
/************************************************************************************************************/
  
  
  /*----------------------------------------Loop de espera por argumentos------------------------------*/
 
 
  state=letsreg; // first state  
  printf("letsreg_ndn> "); fflush(stdout);//prompt
  while(1){

    switch (state){

      case letsreg: // not registered yet
        if(fgets(buffer, 127, stdin)!=NULL) //reads the input arguments
          sscanf(buffer, "%s", cmd);  //to check which command was given
        FD_ZERO(&wait);
        switch(command(cmd))
        {
          case 0: //user writes an uknown command
            fprintf(stderr,"This isn't a valid command\nIt must be one of the following: join, create, get, id, show, leave or exit\n");
          break;
          
          case 1: //Join command written
            aux_int=sscanf(buffer, "%*s %s %s %s %s", name_network, id, bootip, boottcp);
            if(aux_int!=2 && aux_int!=4 ){
              fprintf(stderr,"Wrong number of arguments... \nArguments: <join> <net_id> <id> or <join> <net_id> <id> <bootip> <boottcp> \n"); 
              break;
            }
            memset(&hints_tcp_server,0,sizeof (hints_tcp_server));
            res_tcp_server=createsocket(&socket_tcp_server,&hints_tcp_server,"TCPserver",NULL,&argv[2], NULL);
              if (res_tcp_server==NULL){
              printf("Letsreg:error creating TCP socket para o primeiro cliente\n\n");
              exit(1);
            }
            if(tcpconnectionserver(socket_tcp_server,&res_tcp_server)==false){freeaddrinfo(res_tcp_server); close(socket_tcp_server); exit(1);}
            maxfd=socket_tcp_server;
            if(aux_int==2){

              memset(&hints_udp,0,sizeof (hints_udp));
              res_udp=createsocket(&fd,&hints_udp,"UDP",&argv[3],&argv[4], NULL);
              if (res_udp==NULL){
                printf("Inicialization: Error UDP socket");
                freememory(&res_udp,&tcp_id,&tcp_ip,&tcp_port);
                exit(1);
              }
              
              if(communication("NODES","\0", addrlen,&addr1,res_udp,argv,name_network,fd,&tcp_id, &tcp_ip, &tcp_port)==true){
                close(fd);

                memset(&hints_tcp_client,0,sizeof (hints_tcp_client));
                res_tcp_client=createsocket(&socket_tcp_client,&hints_tcp_client,"TCPclient", &tcp_ip, &tcp_port, &tcp_id);
                if (res_tcp_client==NULL){
                  printf("Letsreg:error creating TCP socket para o Servidor\n\n");
                  break;
                }
                if(tcpconnectionclient(socket_tcp_client,&res_tcp_client, argv,id)==false) /*error*/ exit(1);
                newsocket(&socketlist,socket_tcp_client);
                printf("Letsreg:A criar o primeiro socket para cliente, como servidor TCP\n\n");
              }else close(fd);

            }else if(aux_int==4){ //connecting to the node given in the user interface
              if(checkport(boottcp,strlen(boottcp))==false){ printf("Wrong Port Number!\nMust be an integer between 0 and 65535\n"); break;} //checks if it's a possible port
              if((sscanf(bootip, "%d.%d.%d.%d.", &Ip1,&Ip2,&Ip3,&Ip4)!=4) || Ip1>255 || Ip2>255 || Ip3>255||Ip4>255){ /*checks if it's a possible IP*/
                printf("Invalid IP, should have four integers separated by an <.> and each one can't be bigger than 255\n"); 
                break;
              }
              memset(&hints_tcp_client,0,sizeof (hints_tcp_client));
              res_tcp_client=createsocket(&socket_tcp_client,&hints_tcp_client,"TCPclientKnown", &bootip, &boottcp, NULL);
              if (res_tcp_client==NULL){
                printf("Letsreg:error creating TCP socket para o Servidor\n\n");
                break;
              }
              tcpconnectionclient(socket_tcp_client,&res_tcp_client,argv,id);
              newsocket(&socketlist,socket_tcp_client);
              printf("Letsreg:A criar o primeiro socket para cliente, como servidor TCP\n\n");
            }           

            memset(&hints_udp,0,sizeof (hints_udp));
            res_udp=createsocket(&fd,&hints_udp,"UDP",&argv[3],&argv[4], NULL);
            if (res_udp==NULL){
              printf("Inicialization: Error UDP socket");
              freememory(&res_udp,&tcp_id,&tcp_ip,&tcp_port);
              exit(1);
            }
            if(communication("REG","OKREG", addrlen,&addr1,res_udp,argv,name_network,fd,&tcp_id, NULL, NULL)==true){ 
              state=connected;
              close(fd);

            }else{
              printf("Letsreg:Something went wrong (you can try check if the port number input is correct). Try again from the beginning!!\n\n");
              freememory(&res_udp, &tcp_id,&tcp_ip, &tcp_port);
              exit(0);
            }
            close(fd);

          break;

          case 2: 
            printf("You haven't joined yet\n");
          break;
          case 3:
            printf("You haven't joined yet\n");
          break;
          case 4:
            printf("You haven't joined yet\n");            
          break;
          case 5: //Leave command written
            printf("There is no node connected, you can't remove something that doesn't exist\n");
          break;

          case 6: //Exit
            printf("Exiting...\n");
            freememory(&res_udp,&tcp_id,&tcp_ip, &tcp_port);
            exit(1);
          break;
        }
        
      break;

      case connected:
        FD_ZERO(&wait);
        FD_SET(0,&wait); 
        FD_SET(socket_tcp_server,&wait);
        for (aux_listsocket=socketlist;aux_listsocket!=NULL;aux_listsocket=aux_listsocket->next){
          FD_SET(aux_listsocket->socket,&wait);
          if(aux_listsocket->next==NULL) maxfd=aux_listsocket->socket;
        }
        
          t=(struct timeval *)calloc(1,sizeof(struct timeval)); //intialize timeval struct with limited time to use in select function
          t->tv_sec=20;
          t->tv_usec=1;
                
        n=select(maxfd+1,&wait,NULL, NULL,t); //checks what file descriptor is ready to read, integer "n" will be 0 if the timeout expired before any file descriptors became ready.
        if (n==0){
            free(t);
         }
        for (i=0;i<n;i++){
          if(FD_ISSET(0,&wait)){ //interface

            fgets(buffer, 127, stdin);
            sscanf(buffer, "%s %s %s", cmd,aux_char1,aux_char2);
            switch(command(cmd)){
              case 1: //join
                printf("You had already joined this server\n");
              break;
              case 2: //create
                my_objects[indice]=createobject(id,aux_char1); //send <id> from the input and the second argument from the last fgets which has the subname
                indice++;  //increments pointer for the next object        
              break;
              case 3: //get
                msg_received=(char*)calloc(128,sizeof(char));
                strcpy(msg_received,"INTEREST"); strcat(msg_received," "); strcat(msg_received,aux_char1);  //"get" therefore it has to send the message interest and the name we are looking for
                trademsg(&socketlist,msg_received,&tcp_id,&nodestree,&nodes_request,&recovernode,id,0,argv,my_objects,indice,&cache,&nos_internos);             
              break;  
              case 4: //show
                if( strcmp(aux_char1,"topology")==0){
                  showtopology(recovernode,tcp_id);
                }else if(strcmp(aux_char1,"routing")==0){
                  showrouting(nodestree);
                }else if(strcmp(aux_char1,"cache")==0){
                  showcache(cache);
                }
              break;
              case 5: //leave
                memset(&hints_udp,0,sizeof (hints_udp));
                res_udp=createsocket(&fd,&hints_udp,"UDP",&argv[3],&argv[4], NULL);
                if (res_udp==NULL){
                  printf("Inicialization: Error UDP socket");
                  freememory(&res_udp,&tcp_id,&tcp_ip,&tcp_port);
                  exit(1);
                }
                if(communication("UNREG","OKUNREG", addrlen,&addr1,res_udp,argv,name_network, fd,&tcp_id, NULL, NULL)==true){ state=letsreg;}
                closesockets(&socketlist,&socket_tcp_server, &nodestree); freeaddrinfo(res_tcp_server);freeaddrinfo(res_tcp_client); //close sockets and free memory
                close(fd);
              break;

              case 6: //Exit
                memset(&hints_udp,0,sizeof (hints_udp));
                res_udp=createsocket(&fd,&hints_udp,"UDP",&argv[3],&argv[4], NULL);
                if (res_udp==NULL){
                  printf("Inicialization: Error UDP socket");
                  freememory(&res_udp,&tcp_id,&tcp_ip,&tcp_port);
                  exit(1);
                }
                if(communication("UNREG","OKUNREG", addrlen,&addr1,res_udp,argv,name_network, fd,&tcp_id, NULL, NULL)==true){ state=letsreg;} //leaving therefore we can join again
                close(fd);
                printf("Exiting...\n");
                freememory(&res_udp, &tcp_id,&tcp_ip, &tcp_port);
                closesockets(&socketlist,&socket_tcp_server, &nodestree);
                freeaddrinfo(res_tcp_server); freeaddrinfo(res_tcp_client);
                exit(1);
              break;
            }
            FD_CLR(0,&wait);
          }else if(FD_ISSET(socket_tcp_server,&wait)){ //server ready to read
            addrlen=sizeof(addr_tcp);
            if((newfd=accept(socket_tcp_server,&addr_tcp,&addrlen))==-1){ 
              printf("travou no accept");
              /*error*/exit(1);
              }
            newsocket(&socketlist,newfd);
            for (aux_listsocket=socketlist;aux_listsocket->next!=NULL;aux_listsocket=aux_listsocket->next){}
            FD_CLR(socket_tcp_server,&wait);
          }else{           
            for (aux_listsocket=socketlist;aux_listsocket!=NULL;aux_listsocket=aux_listsocket->next){
              if(FD_ISSET(aux_listsocket->socket,&wait)){
                i++;
                msg_received=(char*)calloc(128,sizeof(char));
                nw=read(aux_listsocket->socket,msg_received,128);
                if(nw==-1 || nw==0)/*error*/{ //fecho de sessão
                  for(aux_nodes=nos_internos;aux_nodes!=NULL;aux_nodes->next){
                    if(aux_nodes->socket==aux_listsocket->socket)break;  } //there is only one socket inside nos_internos that is equal to the socket taht is in use. So no while function is used to run all the nos_internos list
                  removenodedouble(&aux_nodes);
                  socket_aux=aux_listsocket->socket;
                  FD_CLR(aux_listsocket->socket,&wait);
                  socket_out=(struct socket_out*)malloc(1*sizeof(socket_out));
                  socket_exit(&socketlist,&nodestree,&recovernode,&tcp_id,aux_listsocket->socket, &socket_out);
                  for(j=0;j<(*(socket_out->id_i));j++){
                    strcpy(msg_received," WITHDRAW ");strcat(msg_received,socket_out->id_out[j]);
                    trademsg(&socketlist,msg_received,&tcp_id,&nodestree,&nodes_request,&recovernode,id,0,argv,my_objects,indice, &cache,&nos_internos);
                  }
                  free(msg_received);msg_received=(char*)calloc(128,sizeof(char));  //msg_received filled above, the free is used so that there are no other caracters left by the previous messages
                  free(tcp_id);free(tcp_ip);free(tcp_port);
                  tcp_id=(char *)calloc(127, sizeof(char));tcp_ip=(char *)calloc(60, sizeof(char));tcp_port=(char *)calloc(50, sizeof(char));
                  if(recovernode!=NULL && aux_listsocket->socket==socket_tcp_client){
                    if(strcmp(recovernode->ip,argv[1])!=0 && strcmp(recovernode->port,argv[2])!=0){
                      strcpy(aux_char1,recovernode->ip);strcpy(aux_char2,recovernode->port);
                      free(recovernode->ip);free(recovernode->port);free(recovernode);recovernode=NULL;
                      memset(&hints_tcp_client,0,sizeof (hints_tcp_client));
                      res_tcp_client=createsocket(&socket_tcp_client,&hints_tcp_client,"TCPclientKnown", &aux_char1, &aux_char2, NULL);
                      if (res_tcp_client==NULL){
                        printf("Letsreg:error creating TCP socket para o Servidor\n\n");
                      }else{  //connection with new external neighbour made
                        tcpconnectionclient(socket_tcp_client,&res_tcp_client, argv,id);
                        newsocket(&socketlist,socket_tcp_client);
                        strcpy(tcp_ip,aux_char1);strcpy(tcp_port,aux_char2);strcpy(tcp_id,tcp_ip);strcat(tcp_id," ");strcat(tcp_id,tcp_port);
                      }
                    }else if(socketlist!=NULL){   //send new message to my internal neighbour that he is my new external neighbour, if socketlist==NULL there are no nodes in my network, so my node doesnt do nothing
                      free(recovernode->ip);free(recovernode->port);free(recovernode);recovernode=NULL;
                      strcpy(msg_received," NEW ");strcat(msg_received,argv[1]);strcat(msg_received," ");strcat(msg_received,argv[2]);
                      ptr=msg_received;
                      n=strlen(ptr);
                      while(n>0){
                        if((nw=write(nos_internos->socket,ptr,n))<=0)/*error*/exit(1);
                        n-=nw; ptr+=nw;
                      }
                      socket_tcp_client=nos_internos->socket;
                      strcpy(tcp_id,nos_internos->id);
                    }
                  }
                  close(socket_aux);
                }
                else{
                  auxsocketlist=aux_listsocket;
                  aux_int=trademsg(&socketlist,msg_received,&tcp_id,&nodestree,&nodes_request,&recovernode,id,auxsocketlist->socket,argv,my_objects,indice, &cache,&nos_internos);
                  FD_CLR(aux_listsocket->socket,&wait);
                }
                free(msg_received);
              }
              
            }
          }
        }
      break;   
    }
  }

   return 0;
}
  


