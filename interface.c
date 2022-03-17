#include "Libs.h"

int command(char cmd[])
{ //function that determines which command was given

  if (strcmp(cmd, "join") == 0)
    return 1;

  if (strcmp(cmd, "create") == 0)
    return 2;

  if (strcmp(cmd, "get") == 0)
    return 3;

  if (strcmp(cmd, "show") == 0)
    return 4;

  if (strcmp(cmd, "leave") == 0)
    return 5;

  if (strcmp(cmd, "exit") == 0)
    return 6;

  return 0;
}
char *createobject(char *id, char *subname){
  char *object;
  int numberchar=0;

  numberchar=strlen(id)+strlen(subname);
  object=(char*)malloc((numberchar+2)*sizeof(char));
  strcpy(object,id); 
  strcat(object,"."); 
  strcat(object,subname);
  return object;
}

/**************************************************************************************************
 * Function showtopology
 *  
 * This function has the objective: display the contacts of external and recuperation nodes 
 * 
 * 
 * 
 **************************************************************************************************/
void showtopology(struct recovernode *recover, char *tcp_id){
    char *tcp_port,*tcp_ip;
    tcp_ip=(char *)malloc(strlen(tcp_id)*sizeof(char));
    tcp_port=(char *)malloc(strlen(tcp_id)*sizeof(char));
    sscanf(tcp_id,"%s %s",tcp_ip,tcp_port);
    printf("--------------------Topology------------------\n\n");
    if(recover==NULL){printf("The net only has one node then it doesn't have external and recuperation node\n"); return;}
    printf("The recuperation node has the following values: IP)%s  Port)%s\n", recover->ip, recover->port);
    printf("The external node has the following values: IP)%s  Port)%s\n\n", tcp_ip, tcp_port);
    free(tcp_port);free(tcp_ip);
    printf("-----------------------------------------------\n\n");
}

/**************************************************************************************************
 * Function showrouting
 *  
 * This function has the objective: display the "tabela de expedição" of the node
 * 
 * 
 * 
 * *********************************************************************************************/
void showrouting(struct nodestree *nodestree){
  printf("---------------Expedition table----------------\n\n");
    
  	while (nodestree!=NULL)
    {
     printf("Node: id-%s socket-%d\n", nodestree->id,nodestree->socket);
     nodestree=nodestree->next;
    }
  printf("------------------------------------------------\n\n");

  
}
/**************************************************************************************************
 * Function showcache
 *  
 * This function has the objective: display the "tabela de expedição" of the node
 * 
 * 
 * 
 * *********************************************************************************************/

void showcache(char **cache){
  printf("-----------------The cache------------------\n");
  if(cache[0]==NULL){
    printf("Cache is empty\n");
  }
  else if(cache[1]==NULL){ printf("The cache has: %s\n", cache[0]);}
  else{ printf("The cache has %s %s\n", cache[0], cache[1]);}
  printf("---------------------------------------------\n");
}