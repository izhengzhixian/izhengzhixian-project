#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 4096
#define REMOTE_HOST "127.0.0.1"
#define REMOTE_PORT 1234


int main(void){
	struct sockaddr_in server ,client;
	char buffer[BUFFER_SIZE];
	memset(&client, 0, sizeof(client));
	client.sin_family = AF_INET;
	client.sin_addr.s_addr = htonl(INADDR_ANY);
	client.sin_port = htons(0);

	int client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket < 0){
		printf("Create Socket Failed!\n");
		exit(1);
	}
	/*
	if ( bind(client_socket, (struct sockaddr*)&client, \
				sizeof(client))){
		printf("Client Bind Port Failedk!\n");
		exit(1);
	}
	*/
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(REMOTE_PORT);
	inet_aton(REMOTE_HOST, &server.sin_addr);
	int server_length = sizeof(server);
	if( connect(client_socket, (struct sockaddr*)&server, server_length) ){
		printf("Can Not Connect To %s:%d!\n", REMOTE_HOST, REMOTE_PORT);
		exit(1);
	}

	while(1){
		printf(">> ");
		scanf("%s", buffer);
		send(client_socket, buffer, strlen(buffer), 0);
		int recv_len = recv(client_socket, buffer, BUFFER_SIZE-1, 0);
		if( recv_len == 0){
			close(client_socket);
			break;
		}
		buffer[recv_len] = 0;
		printf("%s\n", buffer);
	}

	return 0;
}
