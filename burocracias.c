#include "Libs.h"

void freememory(struct addrinfo **res, char **tcp_id, char **tcp_ip, char **tcp_port){
  
  freeaddrinfo(*res);
  free(*tcp_id);
  free(*tcp_ip); 
  free(*tcp_port);

}

void allocatemem(char ***memory, int lim, int numberchars){

  int i=0;
  
  *memory=(char**)calloc(lim,sizeof(char*));
  for(i=0;i<lim;i++)
  {
    (*memory)[i]=(char *)calloc(numberchars, sizeof(char));
  }
}

void newsocket(struct socketlist **list,int socket){
  struct socketlist *aux_list,*before;
  if((*list)==NULL){
    (*list)=(struct socketlist*)malloc(sizeof(struct socketlist));
    (*list)->socket=socket;
    (*list)->next=NULL;
    (*list)->before=NULL;
  }else {
    for(aux_list=(*list);aux_list->next!=NULL;aux_list=aux_list->next){}
    before=aux_list;
    (aux_list)->next=(struct socketlist*)malloc(sizeof(struct socketlist));
    (aux_list)=(aux_list)->next;
    (aux_list)->socket=socket;
    (aux_list)->next=NULL;
    aux_list->before=before;
  }
}

void removesockettriple(struct socketlist ***list){
  struct socketlist *auxnode,*before;
  if((**list)!=NULL){
    if((**list)->before==NULL && (**list)->next==NULL){
      free(**list);
      (**list)=NULL;
    }else if((**list)->before!=NULL && (**list)->next!=NULL){
      (auxnode)=(**list)->next;
      (before)=(**list)->before;
      free((**list));
      (before)->next=(auxnode);
    }else if((**list)->before==NULL && (**list)->next!=NULL){
      (auxnode)=(**list)->next;
      free(**list);
      (**list)=(auxnode);
      (**list)->before=NULL;
    }else if((**list)->before!=NULL && (**list)->next==NULL){
      (before)=(**list)->before;
      free(**list);
      **list=(before);
      (**list)->next=NULL;
    }
  }
}

void removesocketdouble(struct socketlist **list){
  struct socketlist *auxnode,*before;
  if((*list)!=NULL){
    if((*list)->before==NULL && (*list)->next==NULL){
      free(*list);
      (*list)=NULL;
    }else if((*list)->before!=NULL && (*list)->next!=NULL){
      (auxnode)=(*list)->next;
      (before)=(*list)->before;
      free((*list));
      (before)->next=(auxnode);
    }else if((*list)->before==NULL && (*list)->next!=NULL){
      (auxnode)=(*list)->next;
      free(*list);
      (*list)=(auxnode);
      (*list)->before=NULL;
    }else if((*list)->before!=NULL && (*list)->next==NULL){
      (before)=(*list)->before;
      free(*list);
      *list=(before);
      (*list)->next=NULL;
    }
  }
}
void newnodetriple(struct nodestree ***nodestree_triple,char *id,int socket){
    struct nodestree *before ,*aux_node;
    int aux_int=0;
    if((**nodestree_triple)==NULL){
      (**nodestree_triple)=(struct nodestree*)malloc(sizeof(struct nodestree));
      (**nodestree_triple)->id=(char *)calloc(sizeof(id)+1,sizeof(char));
      strcpy((**nodestree_triple)->id,id);
      (**nodestree_triple)->socket=socket;
      (**nodestree_triple)->before=NULL;
      (**nodestree_triple)->next=NULL;
    }else{
      for(aux_node=(**nodestree_triple);aux_node->next!=NULL;aux_node=aux_node->next){if(strcmp(aux_node->id,id)==0)aux_int=1;}
      if(aux_int==0){
        (aux_node)->next=(struct nodestree*)malloc(sizeof(struct nodestree));
        before=aux_node;
        (aux_node)=(aux_node)->next;
        (aux_node)->id=(char *)calloc(sizeof(id)+1,sizeof(char));
        strcpy((aux_node)->id,id);
        (aux_node)->socket=socket;
        (aux_node)->before=before;
        (aux_node)->next=NULL;
      }
    }
}
void newnodedouble(struct nodestree **nodestree_double,char *id,int socket){
    struct nodestree *before,*aux_node;
    int aux_int=0;
    if((*nodestree_double)==NULL){
      (*nodestree_double)=(struct nodestree*)malloc(sizeof(struct nodestree));
      (*nodestree_double)->id=(char *)calloc(sizeof(id)+1,sizeof(char));
      strcpy((*nodestree_double)->id,id);
      (*nodestree_double)->socket=socket;
      (*nodestree_double)->before=NULL;
      (*nodestree_double)->next=NULL;
    }else{
      for(aux_node=(*nodestree_double);aux_node->next!=NULL;aux_node=aux_node->next){if(strcmp(aux_node->id,id)==0)aux_int=1;}
      if(aux_int==0){
        aux_node->next=(struct nodestree*)malloc(sizeof(struct nodestree));
        before=aux_node;
        aux_node=aux_node->next;
        aux_node->id=(char *)calloc(sizeof(id)+1,sizeof(char));
        strcpy(aux_node->id,id);
        aux_node->socket=socket;
        aux_node->before=before;
        aux_node->next=NULL;
      }
    }
}
void removenodedouble(struct nodestree **node){
  struct nodestree *after,*before;
  if(*node!=NULL){
    if((*node)->before==NULL && (*node)->next==NULL){
      free((*node)->id);
      free(*node);
      (*node)=NULL;
    }else if((*node)->before!=NULL && (*node)->next!=NULL){
      before=(*node)->before;
      after=(*node)->next;
      free((*node)->id);
      free((*node));
      before->next=after;
    }else if ((*node)->before==NULL && (*node)->next!=NULL){
      (after)=(*node);
      free((*node)->id);
      free((*node));
      (*node)=after;
      (*node)->before=NULL;
    }else if ((*node)->before!=NULL && (*node)->next==NULL){
      (before)=(*node)->before;
      free((*node)->id);
      free(*node);
      *node=before;
      (*node)->next=NULL;
    }
  }
}
void removenodetriple(struct nodestree ***node){
  struct nodestree *after,*before;
  if((**node)!=NULL){
    if((**node)->before==NULL && (**node)->next==NULL){
      free((**node)->id);
      free(**node);
      (**node)=NULL;
    }else if((**node)->before!=NULL && (**node)->next!=NULL){
      before=(**node)->before;
      after=(**node)->next;
      free((**node)->id);
      free((**node));
      (**node)=before;
      (**node)->next=(after);
    }else if ((**node)->before==NULL && (**node)->next!=NULL){
      (after)=(**node)->next;
      free((**node)->id);
      free((**node));
      (**node)=after;
      (**node)->before=NULL;
    }else if ((**node)->before!=NULL && (**node)->next==NULL){
      (before)=(**node)->before;
      free((**node)->id);
      free(**node);
      before->next=NULL;
      (**node)=before;
    }
  }
}

int numberofwords(char buffer[], char **protocols){

  char aux_char[30];
  int wrd=0, i=0;
/*----------------------------Checks if any word has been received and increments the pointer */
  
  sscanf(buffer,"%s",aux_char);
  while (buffer[0]==' ' || buffer[0]=='\n' ||buffer[0]=='\t' ){buffer++;}//passar os espaços à frente
  for (i=0;i<=6;i++){
    if(strcmp(aux_char,protocols[i])==0)
    wrd++;
  }
  buffer=buffer+strlen(aux_char); //to go through the first word
  /*----------------------------counts the number of words the server received -------------------------------------*/

  while(strcmp(&buffer[0],"\0")!=0){
        if( buffer[0]==' ' || buffer[0]=='\n' || buffer[0]=='\t'){
          while (buffer[0]==' ' || buffer[0]=='\n' ||buffer[0]=='\t' ){buffer++;}//passar os espaços à frente
          if(sscanf(buffer,"%s",aux_char)==1){
            if(strcmp(aux_char,protocols[0])==0){wrd++;}else if(strcmp(aux_char,protocols[1])==0){wrd++;}
            else if(strcmp(aux_char,protocols[2])==0){wrd++;}else if(strcmp(aux_char,protocols[3])==0){wrd++;}
            else if(strcmp(aux_char,protocols[4])==0){wrd++;}else if(strcmp(aux_char,protocols[5])==0){wrd++;}
            else if(strcmp(aux_char,protocols[6])==0){wrd++;}
            buffer=buffer+strlen(aux_char);
          }  
        }else{
          buffer=buffer+strlen(aux_char);
        }
  }
  return wrd;
}
void writeincache(char ****cache, char *object){
   //cada cache tem apenas 2 espaços e a política é LRU
  
  if((*(*cache))[0]==NULL){ //significa  cache vazia
    (*(*cache))[0]=(char*)malloc((strlen(object)+1)*sizeof(char));
    strcpy((*(*cache))[0], object);
  }
  else if((*(*cache))[1]==NULL && strcmp((*(*cache))[0],object)!=0){ //significa que só a metade da cache está cheia
  
    (*(*cache))[1]=(char*)malloc((strlen(object)+1)*sizeof(char));
    strcpy((*(*cache))[1], object);
  }
  else{ //cache cheia
    if(strcmp((*(*cache))[0],object)==0 || strcmp((*(*cache))[1],object)==0){ //já está escrito na cache
      return;
    }
    free((*(*cache))[0]); //libertar cache 0 pois está cheia
    (*(*cache))[0]=(char*)malloc((strlen((*(*cache))[1])+1)*sizeof(char));
    strcpy((*(*cache))[0],(*(*cache))[1]);
    free((*(*cache))[1]);
    (*(*cache))[1]=(char*)malloc((strlen(object)+1)*sizeof(char));
    strcpy((*(*cache))[1],object);
  }

}