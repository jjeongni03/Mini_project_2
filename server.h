#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXLINE 1024
#define BACKLOG 10
#define MAX_CLIENTS 10

extern char *EXIT_STRING;
extern int client_sockets[MAX_CLIENTS];
extern char client_names[MAX_CLIENTS][50];

// 클라이언트 소켓에 해당하는 클라이언트 이름을 반환하는 함수
char* get_client_name(int fd);

// 모든 클라이언트에게 메시지를 전송하는 함수
void broadcast_message(char *message, int sender_fd);

#endif
