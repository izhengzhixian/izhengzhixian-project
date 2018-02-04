#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include"modules/define.h"
#include"modules/extend.h"

#define Z_PORT 1234
#define Z_HOST INADDR_ANY
#define Z_QUEUE_NUM 5
#define BUFFER_SIZE 4096
#define MAX_SOCKET_NUM 64

typedef struct thread_socket{
	pthread_t thread_id;
	struct sockaddr_in sock_in;
	int sock_des;
}TS_socket;

TS_socket ts_list[MAX_SOCKET_NUM];
TS_socket * get_thread_socket(void)
{
	int i;
	for(i=0; i < MAX_SOCKET_NUM; i++){
		if(! ts_list[i].sock_des)
			return ts_list + i;
	}
	return NULL;
}


char * zcal(char * str);


void * new_thread(void * new_arg){
	TS_socket * ts_sock_p = (TS_socket*)new_arg;
	int client_socket = ts_sock_p->sock_des;
	char * buffer = malloc(BUFFER_SIZE);
	if(! buffer){
		printf("malloc error!\n");
		exit(1);
	}
	int length;
	while(1){
		length = recv(client_socket, buffer, BUFFER_SIZE-1, 0);
		if (length == 0){
			close(client_socket);
			free(buffer);
			break;
		}
		buffer[length] = 0;
		char *ret_str = zcal(buffer);
		send(client_socket, ret_str, strlen(ret_str), 0);
		free(ret_str);
	}
	printf("%s:%d 断开\n", 
			inet_ntoa(ts_sock_p->sock_in.sin_addr), 
			ntohs(ts_sock_p->sock_in.sin_port));
	ts_sock_p->sock_des = 0;
}

int main() {   

	struct sockaddr_in server;
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(Z_HOST);
	server.sin_port = htons(Z_PORT);

	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket < 0){
		printf("Create socket Failed!\n");
		exit(1);
	}

	int opt = 1;
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	if (bind(server_socket, (struct sockaddr*)&server,  sizeof(server)) ){
		printf("Server Bind Port: %d Failed!\n", Z_PORT);
		exit(1);
	}

	if ( listen(server_socket, Z_QUEUE_NUM) ){
		printf("Server Listen Failed!\n");
		exit(1);
	}

	int client_length = sizeof(struct sockaddr_in);
	while(1){
		TS_socket * ts_sock_p  = get_thread_socket();
		if (! ts_sock_p){
			printf("Create thread over %d\n", MAX_SOCKET_NUM);
			exit(1);
		}
		ts_sock_p->sock_des = accept(server_socket,
				(struct sockaddr*)&(ts_sock_p->sock_in),
			   	(socklen_t*)&client_length);
		if ( ts_sock_p->sock_des < 0){
			printf("Server Accept Failed!\n");
			printf("%s\n", strerror(errno));
			exit(1);
		}
		printf("%s:%d 连接\n", 
				inet_ntoa(ts_sock_p->sock_in.sin_addr), 
				ntohs(ts_sock_p->sock_in.sin_port));
		if( pthread_create(&ts_sock_p->thread_id, NULL, 
					new_thread, ts_sock_p)){
			printf("pthread_create error!\n");
			exit(1);
		}
	}
	close(server_socket);
	return 0;
}
