#include "Libs.h"

bool tcpconnectionclient(int socket_tcp, struct addrinfo **res_tcp,char **argv,char *id){

  int errcode=0, n, nw;
  char buffer[20], *ptr;

  errcode=connect(socket_tcp, (*res_tcp)->ai_addr, (*res_tcp)->ai_addrlen);
  if(errcode==-1){
    fprintf(stderr,"Couldn't connect with TCP server.\n");
    return false;
  }else{
    strcpy(buffer,"NEW ");strcat(buffer,argv[1]);strcat(buffer," ");strcat(buffer,argv[2]);
    ptr=buffer;
    n=strlen(ptr);
    while(n>0){
      if((nw=write(socket_tcp,ptr,n))<=0){/*error*/exit(1);}
      n-=nw; ptr+=nw;
    }
    strcpy(buffer," ADVERTISE ");strcat(buffer,id);
    ptr=buffer;
    n=strlen(ptr);
    while(n>0){
      if((nw=write(socket_tcp,ptr,n))<=0){/*error*/exit(1);}
      n-=nw; ptr+=nw;
    }
  } 
}

bool tcpconnectionserver(int socket_tcp, struct addrinfo **res_tcp){

  if(bind(socket_tcp,(*res_tcp)->ai_addr,(*res_tcp)->ai_addrlen)==-1){//error
    fprintf(stderr,"Couldn't bind.\n");
    return false;
  }
  if(listen(socket_tcp,5)==-1){
    //error
    fprintf(stderr,"Couldn't get address info, error in listen. Exiting...\n");
    return false;
  }
  return true;
}

int trademsg(struct socketlist **socketlist, char *buffer,char **tcp_id, struct nodestree **nodestree,struct nodestree **nodes_request,struct recovernode **recovernode, char *id,int socket_inuse,char**argv,char **my_objects,int indice, char ***cache,struct nodestree **nos_internos){
  
  int i=0,j=0,k=0, wrd=0,socket_tcp_client,aux_int;
  bool aux_bool;
  char *ptr, **words;
  ssize_t nw,nread,nleft,n;
  char **protocols;
  char *arg1=NULL,*arg2=NULL,*arg3=NULL,*arg4=NULL,*aux_string=NULL,*ip_extern=NULL,*port_extern=NULL,*aux_char,*buffer1=NULL;
  struct socketlist *listrunner;
  struct addrinfo hints_tcp_client,*res_tcp_client;
  struct nodestree *nodesrunner,**nodesrunner_double;

/*----------------------Allocating memory and initializing variables-------------------------------*/
  
  allocatemem(&protocols,7,10);
  
  arg1=(char *)malloc(30*sizeof(char));
  arg2=(char *)malloc(30*sizeof(char));
  arg3=(char *)malloc(20*sizeof(char));
  aux_string=(char *)malloc(20*sizeof(char));
  buffer1=(char *)malloc(128*sizeof(char));

  strcpy(protocols[0],"NEW");
  strcpy(protocols[1],"EXTERN");
  strcpy(protocols[2],"ADVERTISE");
  strcpy(protocols[3],"WITHDRAW");
  strcpy(protocols[4],"INTEREST");
  strcpy(protocols[5],"DATA");
  strcpy(protocols[6],"NODATA");

/*----------------------------------------------------------------------*/

  wrd=numberofwords(buffer, protocols); //counts the number of words in each message received (NEW, EXTERN,...)

  words=(char**)malloc(wrd*sizeof(char*)); //allocate only the space needed
  aux_char=buffer;

   while (aux_char[0]==' ' || aux_char[0]=='\n' ||aux_char[0]=='\t' ){aux_char++;} //steps empty spaces, enters and tabs

  sscanf(aux_char,"%s",aux_string);
 
  for (j=0;j<8;j++){ // words[0] cointais all the message, words[1] will contain the msg received moved 2 or 3 arguments after (depends of which command words[0] has), etc
    if(j<7){   //j<7  to check if the message received has any words=protocols
      if(strcmp(aux_string,protocols[j])==0){
        words[i]=(char *)malloc((strlen(aux_char)+1)*sizeof(char));
        strcpy(words[i],aux_char); 
        aux_char=aux_char+strlen(aux_string); //skip arguments that arent equal to any of the protocols
        while (aux_char[0]==' ' || aux_char[0]=='\n' ||aux_char[0]=='\t' ){aux_char++;}//steps empty spaces, enters and tabs
        sscanf(aux_char,"%s",aux_string);
        i++; j=-1; //i++) increment to another word position | j=-1 to start the for with j=0
      }      
    }
    if(j==7 && aux_char[0]!='\0'){
      aux_char=aux_char+strlen(aux_string)+1; sscanf(aux_char,"%s",aux_string);
      j=-1;
    }else if (aux_char[0]=='\0') break;
  }

  /*--------------------------------------------------------------*/

  for (i=0;i<wrd;i++){
    if(sscanf(words[i],"%s",aux_string)==1){
      
      if(strcmp(aux_string,protocols[0])==0){
        /*strcat(arg1," ");strcat(arg1,arg2);
        newnodetriple(&nos_internos,arg1,socket_inuse);*/
        if((*recovernode)==NULL){
          sscanf(words[i],"%*s %s %s",arg1,arg2);
          strcpy((*tcp_id),arg1);strcat((*tcp_id)," ");strcat((*tcp_id),arg2);
        }
        strcpy(buffer1,"EXTERN ");
        strcat(buffer1,*tcp_id);
        ptr=buffer1;
        n=strlen(ptr);
        while(n>0){
          if((nw=write(socket_inuse,ptr,n))<=0)/*error*/exit(1);
          n-=nw; ptr+=nw;
        }
        for (nodesrunner=(*nodestree);(nodesrunner)!=NULL;(nodesrunner)=(nodesrunner)->next){//fazer advertise da minha tabela de expedição
          strcpy(buffer1," ADVERTISE ");
          strcat(buffer1,(nodesrunner)->id);
          ptr=buffer1;
          n=strlen(ptr);
          while(n>0){
            if((nw=write(socket_inuse,ptr,n))<=0)/*error*/exit(1);
            n-=nw; ptr+=nw;
          }
        }
        strcpy(buffer1," ADVERTISE ");
        strcat(buffer1,id);
        ptr=buffer1;
        n=strlen(ptr);
        while(n>0){
          if((nw=write(socket_inuse,ptr,n))<=0)/*error*/exit(1);
          n-=nw; ptr+=nw;
        }
        if((*recovernode)==NULL){
          strcpy(buffer1," NEW ");strcat(buffer1,argv[1]);strcat(buffer1," ");strcat(buffer1,argv[2]);
          ptr=buffer1;
          n=strlen(ptr);
          while(n>0){
            if((nw=write(socket_inuse,ptr,n))<=0)/*error*/exit(1);
            n-=nw; ptr+=nw;
          }
        }
      }if(strcmp(aux_string,protocols[1])==0){
        /*Guardar o vizinho de recuperação*/

        (*recovernode)=(struct recovernode*)malloc(1*sizeof(struct recovernode));
        sscanf(words[i],"%*s %s %s",arg1,arg2);
        aux_int=strlen(arg1);
        (*recovernode)->ip=(char*)malloc((aux_int+1)*sizeof(char));
        aux_int=strlen(arg2);
        (*recovernode)->port=(char*)malloc((aux_int+1)*sizeof(char));
        strcpy((*recovernode)->ip,arg1);
        strcpy((*recovernode)->port,arg2);
        
      }if(strcmp(aux_string,protocols[2])==0){
        sscanf(words[i],"%*s %s",arg1);
        
        if(strcmp(arg1,id)!=0){   //se o id recebido é diferente do meu então faço as ações necessarias
          aux_int=0;
          for (nodesrunner=(*nodestree);(nodesrunner)!=NULL;(nodesrunner)=(nodesrunner)->next){// If i allready have the id in the nodetree, the information is not saved
            if(strcmp(arg1,nodesrunner->id)==0)aux_int=1;}
          if(aux_int==0)newnodetriple(&nodestree,arg1,socket_inuse);
          strcpy(buffer1," ADVERTISE ");
          strcat(buffer1,arg1);
          for (listrunner=(*socketlist);listrunner!=NULL;listrunner=listrunner->next){  //Send my expedition table by advertise.
            if(listrunner->socket!=socket_inuse){
              ptr=buffer1;
              n=strlen(ptr);
              while(n>0){
                if((nw=write(listrunner->socket,ptr,n))<=0)/*error*/exit(1);
                n-=nw; ptr+=nw;
              }
            }
          }
        }
      }if(strcmp(aux_string,protocols[3])==0){
        sscanf(words[i],"%*s %s",arg1);
        strcpy(buffer1," WITHDRAW ");strcat(buffer1,arg1);

        (nodesrunner)=(*nodestree);
        if(nodesrunner!=NULL){
          while(nodesrunner->next!=NULL){
            for(;nodesrunner->next!=NULL && strcmp((nodesrunner)->id,arg1)!=0 ;(nodesrunner)=(nodesrunner)->next){} //nodesrunner->next!=NULL is with ->next because the other verification can give segmentation fault if nodesrunner=NULL
            if(strcmp((nodesrunner)->id,arg1)==0){
              nodesrunner_double=&nodesrunner;
              removenodetriple(&nodesrunner_double);
            }
            if(nodesrunner!=NULL && nodesrunner->next!=NULL){(nodesrunner)=(nodesrunner)->next;}  //after the removenode the nodesrunner can be NULL. Nodesrunner can go to the while with null value
          }if(nodesrunner!=NULL){if(strcmp((nodesrunner)->id,arg1)==0){nodesrunner_double=&nodesrunner;removenodetriple(&nodesrunner_double);}}
        }
        if((*socketlist)!=NULL){
        while((*socketlist)->before!=NULL){(*socketlist)=(*socketlist)->before;}}
        for (listrunner=(*socketlist);listrunner!=NULL;listrunner=listrunner->next){
          if(listrunner->socket!=socket_inuse){
            ptr=buffer1;
            n=strlen(ptr);
            while(n>0){
              if((nw=write(listrunner->socket,ptr,n))<=0){/*error*/exit(1);}
              n-=nw; ptr+=nw;
            }
          }
        }
      }
      if(strcmp(aux_string,protocols[4])==0){ //Interest
        n=50;
        free(arg1);
        free(arg2);
        arg1=(char *)calloc(40,sizeof(char)); //the name can be bigger than previous malloc (10)
        arg2=(char *)calloc(40,sizeof(char)); //the name can be bigger than previous malloc (10)
        sscanf(words[i],"%*s %s",arg1); //gets id.subname
        strcpy(arg2,arg1);
        aux_char=strtok(arg1,".");aux_char=arg1+strlen(aux_char)+1;
        nodesrunner=*nodestree;
        aux_int=0;
        if(nodesrunner!=NULL){
          while(nodesrunner->next!=NULL){
            //verifica se é um id possível (se está na tabela de expedição)
            if(strcmp(arg1,nodesrunner->id)==0){ aux_int=2; break;}//está na tabela
            if(nodesrunner->next!=NULL)(nodesrunner)=(nodesrunner)->next;
          }
          if(aux_int!=2){printf("Id não está na tabela de expedição\n");}
        }
        if(strcmp(id,arg1)==0){
          for(k=0;k<indice;k++){
            aux_int=0;
            if(strcmp(my_objects[k],arg2)==0){ //verifica se é um objecto criado pelo meu nó
              aux_int=1;
              break;
            }
          }
            if(aux_int==1){ //caso seja um onjecto criado pelo meu nó, enviar "DATA" ara o resto dos nós
              strcpy(buffer1," DATA ");strcat(buffer1,arg2);
              writeincache(&cache,arg2);
              ptr=buffer1;
              n=strlen(ptr);
              while(n>0){
                if((nw=write(socket_inuse,ptr,n))<=0){/*error*/exit(1);}
                n-=nw; ptr+=nw;
              }
            }
            else{ //caso o nome não exista, enviar "NO DATA" para o resto dos nós
              strcpy(buffer1," NODATA ");strcat(buffer1,arg2);
              ptr=buffer1;
              n=strlen(ptr);
              while(n>0){
                if((nw=write(socket_inuse,ptr,n))<=0)/*error*/exit(1);
                n-=nw; ptr+=nw;
              }
            }
        }else{
          for(i=0;i<2;i++){
            if((*cache)[i]!=NULL){
              if(strcmp((*cache)[i],arg2)==0){
                  strcpy(buffer1," DATA ");strcat(buffer1,arg2);
                  ptr=buffer1;
                  n=strlen(ptr);
                  if(socket_inuse==0){printf("O objeto já se encontra na cache do nó");//se o objeto já se encontra na cache depois do get name então imprime a cena
                  }else{
                    while(n>0){
                      if((nw=write(socket_inuse,ptr,n))<=0)/*error*/exit(1);
                        n-=nw; ptr+=nw;
                      }
                  }
                
              }
            }
          }
          if(n==50){
            strcpy(buffer1," INTEREST ");strcat(buffer1,arg2);
            
            for (nodesrunner=(*nodestree);nodesrunner!=NULL;nodesrunner=nodesrunner->next){
              if(strcmp(nodesrunner->id,arg1)==0){
                newnodetriple(&nodes_request,arg2,socket_inuse); //grava o socket de que enviou o INTEREST
                ptr=buffer1;
                n=strlen(ptr);
                while(n>0){
                  if((nw=write(nodesrunner->socket,ptr,n))<=0)/*error*/exit(1);
                  n-=nw; ptr+=nw;
                }
              }
            }
          } 
        }
      }if(strcmp(aux_string,protocols[5])==0){
        sscanf(words[i],"%*s %s",aux_string);
        writeincache(&cache,aux_string);
        strcpy(buffer1," DATA ");strcat(buffer1,aux_string);

        for(nodesrunner=(*nodes_request);nodesrunner->next!=NULL && strcmp(nodesrunner->id,aux_string)!=0;nodesrunner=nodesrunner->next){}
        if(strcmp(nodesrunner->id,aux_string)==0){
          ptr=buffer1;
          n=strlen(ptr);  
          while(n>0){  
            if((nw=write(nodesrunner->socket,ptr,n))<=0)/*error*/exit(1);
            n-=nw; ptr+=nw;
          }
        }
      }if(strcmp(aux_string,protocols[6])==0){
        sscanf(words[i],"%*s %s",aux_string);
        strcpy(buffer1," NODATA ");strcat(buffer1,aux_string);
        for(nodesrunner=(*nodes_request);strcmp(nodesrunner->id,aux_string)!=0;nodesrunner=nodesrunner->next){}
        ptr=buffer1;
        n=strlen(ptr);  
        while(n>0){  
          if((nw=write(nodesrunner->socket,ptr,n))<=0)/*error*/exit(1);
          n-=nw; ptr+=nw;
        }

      }
    }
  }
}