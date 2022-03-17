#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <ctype.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>

#ifndef ARGUMENTS_H
#define ARGUMENTS_H

struct recovernode
{
    char *ip;
    char *port;
};
struct socketlist{
    int socket;
    struct socketlist *next;
    struct socketlist *before;
};
struct nodestree{
    char *id;
    int socket;
    struct nodestree *next;
    struct nodestree *before;
};
struct socket_out{
    char **id_out;
    int *id_i;
};


/*---------------------------------UDP------------------------------------*/
bool checkport(char argv[], int size);
bool chooserandomline(char ***tcp_id, char ***tcp_ip, char ***tcp_port, char *buffer);
bool communication(char RegOrUnreg[], char MessageGiven[],socklen_t addrlen, struct sockaddr *addr1, struct addrinfo *res, char *argv[], char name_network[], int fd, char **tcp_id, char **tcp_ip,char **tcp_port);

/*---------------------------------TCP----------------------------*/
int trademsg(struct socketlist **socketlist, char *buffer,char ** tcp_id, struct nodestree **nodestree,struct nodestree **nodes_request,struct recovernode **recovernode, char *id,int socket_inuse,char **argv,char **my_objects, int indice, char ***cache, struct nodestree **nos_internos);
bool tcpconnectionclient(int socket_udp, struct addrinfo **res,char **argv,char *id);
bool tcpconnectionserver(int socket_udp, struct addrinfo **res);

/*-------------------------------Sockets-------------------------*/
int testarsocket(char teste[],int socket);
struct addrinfo *createsocket(int *fd,struct addrinfo *hints,char *socket_type,char **ip,char **port, char **tcp_id);
void socket_exit(struct socketlist **socketlist,struct nodestree **nodestree,struct recovernode **recovernode,char**id,int socket, struct socket_out **socket_out);
void closesockets(struct socketlist **nodeslist,int *socketserver, struct nodestree **nodestree);
/*---------------------------------Interface--------------------*/
int command(char cmd[]);
char *createobject(char *id, char *subname);
void showtopology(struct recovernode *recover, char *tcp_id);
void showrouting(struct nodestree *nodestree);
void showcache(char **cache);

/*----------------------------------Burocracia--------------------------------*/
void writeincache(char ****cache, char *object); 
void allocatemem(char ***memory, int lim, int numberchars);
void freememory(struct addrinfo **res, char **tcp_id, char **tcp_ip, char **tcp_port);
void trademsgerror(int error);
int numberofwords(char buffer[], char **protocols);
void newnodedouble(struct nodestree **nodestree_double,char *id,int socket);
void newnodetriple(struct nodestree ***nodestree_triple,char *id,int socket);
void newsocket(struct socketlist **list,int socket);
void removenodedouble(struct nodestree **node);
void removenodetriple(struct nodestree ***node);
void removesockettriple(struct socketlist ***list);
void removesocketdouble(struct socketlist **list);

#endif