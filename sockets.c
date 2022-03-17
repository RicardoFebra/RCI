#include "Libs.h"

/******************************************************************
* Function: createsocket
*
* This function creates every type of socket that will be used
*
*
*
******************************************************************/
struct addrinfo *createsocket(int *fd,struct addrinfo *hints,char *socket_type,char **ip,char **port, char **tcp_id){

    struct addrinfo *res, *p;
    struct in_addr *addr_udp;
    int errcode=0, aux_int=0, nn=0;
    char *buffer=NULL;

    buffer=(char*)calloc(128,sizeof(char));

    if (strcmp(socket_type,"UDP")==0){
      (*fd)=socket(AF_INET,SOCK_DGRAM,0);//UDP socket
      if((*fd)==-1){printf("Error creating socket\n");/*error*/return NULL;}       
      (*hints).ai_family=AF_INET;//IPv4
      (*hints).ai_socktype=SOCK_DGRAM;//UDP socket
      (*hints).ai_flags=AI_CANONNAME;
      if((errcode=getaddrinfo(*ip,*port,hints,&res))!=0){
        fprintf(stderr,"error: getaddrinfo: %s\n",gai_strerror(errcode));
        free(buffer);
        return NULL;
      }
      else{
          for(p=res;p!=NULL;p=p->ai_next){
            addr_udp=&((struct sockaddr_in *)p->ai_addr)->sin_addr;
          }
        }

    }else if(strcmp(socket_type,"TCPclient")==0){
        (*fd)=socket(AF_INET,SOCK_STREAM,0); //TCP socket
        if((*fd)==-1){/*error*/return NULL; free(buffer);}
        (*hints).ai_family=AF_INET;//IPv4
        (*hints).ai_socktype=SOCK_STREAM;//TCP socket
        strcpy(buffer,*tcp_id);
        if((errcode=getaddrinfo(*ip,*port,hints,&res))!=0){ 
            aux_int=1;
            while(aux_int==1){   // in case he doesn't connect to the random line
              strcpy(*tcp_id, buffer); //buffer will function as an auxiliar
              strcpy(*tcp_id, &(buffer)[strlen(*ip)+strlen(*port)+2]); //to go to the next line from nodelist
              nn=sscanf(*tcp_id,"%s %s", *ip,*port);
              if((errcode=getaddrinfo(*ip,*port,hints,&res))!=0 && nn==2){ 
                //fprintf(stderr,"Letsreg:error: getaddrinfo: %s\n",gai_strerror(errcode));
                aux_int=1;
              }else if(errcode==0 && nn==2){ aux_int=3;}
                else if(nn!=2){
                  fprintf(stderr,"Letsreg:error: getaddrinfo: %s\nExiting...\n",gai_strerror(errcode));
                  free(buffer);
                  return NULL;
                }
            }
          }
    }else if(strcmp(socket_type,"TCPclientKnown")==0) //in 
    {
      (*fd)=socket(AF_INET,SOCK_STREAM,0); //TCP socket
        if((*fd)==-1){/*error*/return NULL; free(buffer);}
      (*hints).ai_family=AF_INET;//IPv4
      (*hints).ai_socktype=SOCK_STREAM;//TCP socket

      if((errcode=getaddrinfo(*ip,*port,hints,&res))!=0){/*error*/free(buffer);return NULL;}
    }
    else if(strcmp(socket_type,"TCPserver")==0){
        (*fd)=socket(AF_INET,SOCK_STREAM,0); //TCP socket
            if((*fd)==-1)/*error*/return NULL;
        (*hints).ai_family=AF_INET;//IPv4
        (*hints).ai_socktype=SOCK_STREAM;//TCP socket
        (*hints).ai_flags=AI_PASSIVE;//to function as a server

        if((errcode=getaddrinfo(NULL,*port,hints,&res))!=0){/*error*/free(buffer);return NULL;}   
        
    }
    free(buffer);
    return res;
}

void socket_exit(struct socketlist **socketlist,struct nodestree **nodestree,struct recovernode **recovernode,char**id,int socket, struct socket_out **socket_out){
  struct nodestree *aux_nodes,**aux_nodesdouble;
  struct socketlist *aux_list=NULL,**aux_listdouble;
  char **id_out;
  int i=0,j=0,z=0;
    
  (aux_nodes)=(*nodestree);
  while ((aux_nodes)!=NULL){
    for(;(aux_nodes)->socket!=socket && (aux_nodes)->next!=NULL;(aux_nodes)=(aux_nodes)->next){}
    if((aux_nodes)!=NULL){
      if((aux_nodes)->socket==socket)j++; //for above can break loop if aux_nodes->socket is diferent to socket
      (aux_nodes)=(aux_nodes)->next;
    }
  }
  id_out=(char**)malloc(j*sizeof(char*));
  (aux_nodes)=(*nodestree); //to star from the first position
  (*socket_out)->id_i=(int *)malloc(1*sizeof(int));
  *((*socket_out)->id_i)=j;
  while ((aux_nodes)!=NULL){
    for(;(aux_nodes)->socket!=socket && aux_nodes->next!=NULL;aux_nodes=(aux_nodes)->next){}    //inside the loop if we reach aux_nodes=NULL the for cant make the verification

    if((aux_nodes)!=NULL){
      if((aux_nodes)->socket==socket){
        id_out[i]=(char *)malloc((strlen((aux_nodes)->id)+2)*sizeof(char));
        strcpy(id_out[i],(aux_nodes)->id);
        aux_nodesdouble=&aux_nodes;
        removenodetriple(&aux_nodesdouble);
        i++;
      }
      if((aux_nodes)!=NULL)(aux_nodes)=(aux_nodes)->next;  //after removal (aux_nodes) maybe is NULL
    }
  }
  (aux_list)=(*socketlist);
  while (aux_list!=NULL){
    for(;aux_list->socket!=socket && aux_list->next!=NULL;(aux_list)=(aux_list)->next){}
    if((aux_list)!=NULL){
      if((aux_list)->socket==socket){ aux_listdouble=&aux_list; removesockettriple(&aux_listdouble);}
      if((aux_list)!=NULL)(aux_list)=(aux_list)->next; //after removal (aux_list) maybe is NULL
    }
  }
  (*socket_out)->id_out=(char **)malloc((*((*socket_out)->id_i))*sizeof(char*));
  *((*socket_out)->id_i)=i;
  for(z=0;z<i;z++){
    
    (*socket_out)->id_out[z]=(char*)malloc((strlen(id_out[z])+2)*sizeof(char));
    strcpy((*socket_out)->id_out[z],id_out[z]);
  } //dar free de id_out
}

int testarsocket(char teste[],int socket){
    struct sigaction act;
    ssize_t n, nw=0;
    n=strlen(teste);
    
    memset(&act,0,sizeof act);
    act.sa_handler=SIG_IGN;
    if(sigaction(SIGPIPE,&act,NULL)==-1)/*error*/exit(1);

    while(n>0 && nw!=-1){
        if((nw=write(socket,teste,n))<=0)/*error*/exit(1);
        n-=nw; teste+=nw;
    }
    return nw;
}

void closesockets(struct socketlist **nodeslist,int *socketserver, struct nodestree **nodestree){
  while((*nodeslist)!=NULL){
    close((*nodeslist)->socket);
    (*nodeslist)=(*nodeslist)->next;
  }
  free(*nodeslist);

  close(*socketserver);
  while((*nodestree)!=NULL){
    free((*nodestree)->id);
    (*nodestree)=(*nodestree)->next;
  }

  free((*nodestree));
}