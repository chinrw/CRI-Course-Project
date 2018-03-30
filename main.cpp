#define _CRT_SECURE_NO_WARNINGS
#include "helper.h"

void *TCP_connection(void *arg);

int main(int argc, char* argv[]) {
	printf("Started server\n");
	fflush(stdout);
	if (argc != 1 && argc != 2) {
		fprintf(stderr, "ERROR: wrong number of arguments[%d]\n", argc);
		fflush(stdout);
		return -1;
	}
	if (argc == 2) {
		std::string tmp = argv[1];
		serverdata.password = tmp.substr(tmp.find("--opt-pass=") + strlen("--opt-pass="));
		printf("Server password is [%s]\n", serverdata.password.c_str());
	}

	/*establish structure*/
	int tcp_socket = socket(PF_INET, SOCK_STREAM, 0);
	if (tcp_socket < 0) {
		fprintf(stderr, "ERROR: socket() failed\n");
		fflush(stdout);
		return -1;
	}
	struct sockaddr_in tcp_server, tcp_client, my_address;
	tcp_server.sin_family = PF_INET;
	tcp_server.sin_addr.s_addr = INADDR_ANY;
	tcp_server.sin_port = htons(0);
	if (bind(tcp_socket, (struct sockaddr *) &tcp_server, sizeof(tcp_server)) < 0) {
		fprintf(stderr, "ERROR: bind() tcp failed\n");
		fflush(stdout);
		return -1;
	}
	if (listen(tcp_socket, 5) < 0) {
		fprintf(stderr, "ERROR: listen() tcp failed\n");
		fflush(stdout);
		return -1;
	}
	unsigned int sizeOfsockaddr = sizeof(tcp_server);
	getsockname(tcp_socket, (struct sockaddr *) &my_address, &sizeOfsockaddr);

	printf("Listening for TCP connections on port: %d\n", ntohs(my_address.sin_port));
	fflush(stdout);
	/*establish structure*/

	fd_set readfds;
	while (1) {
		FD_ZERO(&readfds);
		FD_SET(tcp_socket, &readfds);
		int n = select(FD_SETSIZE, &readfds, NULL, NULL, NULL);
		if (n < 0) {
			fprintf(stderr, "ERROR: select() failed\n");
			perror("select()");
			fflush(stdout);
			return -1;
		}
		else if (n == 0) { continue; }
		if (FD_ISSET(tcp_socket, &readfds)) {//incoming TCP connection
			int new_socket = accept(tcp_socket, (struct sockaddr *) &tcp_client, (socklen_t *)&sizeOfsockaddr);
			printf("Rcvd incoming TCP connection from: [%s] on port[%d]\n",
				inet_ntoa((struct in_addr) tcp_client.sin_addr), tcp_client.sin_port);
			fflush(stdout);
			pthread_t tid;
			if (pthread_create(&tid, NULL, TCP_connection, &new_socket) != 0) {
				fprintf(stderr, "ERROR: pthread_create() failed\n");
				fflush(stdout);
				return -1;
			}
		}
	}
	return 0;
}

void *TCP_connection(void *arg) {
	pthread_detach(pthread_self());
	int *arg_ptr = (int *)arg;
	int fd = (*arg_ptr);
	char buffer[BUFFER_SIZE];
	struct UserData* userdata = new UserData;

	while (1) {
		int n = recv(fd, buffer, BUFFER_SIZE - 1, 0);
		if (n < 0) {
			fprintf(stderr, "ERROR: recv() failed");
			fflush(stdout);
			return NULL;
		}
		else if (n == 0) {
			printf("[Thread %u] Client disconnected\n", (unsigned int)pthread_self());
			fflush(stdout);
			close(fd);
			return NULL;
		}
		else {
			buffer[n] = '\0';
			handle_user(fd, buffer, n, userdata);
		}
	}
	return NULL;
}
