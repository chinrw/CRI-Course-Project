#pragma once
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <vector>
#include <pthread.h>

#define BUFFER_SIZE 64

struct ServerData {
	std::string password;
	std::vector<std::string> channels;
};


struct UserData {
	std::string username;
};

void sendMsg(int fd, char *str);
void handle_user(int fd, char* response, int response_size);
void handle_user_USER(int fd, std::string response);
void handle_user_LIST(int fd, std::string response);
void handle_user_JOIN(int fd, std::string response);
void handle_user_PART(int fd, std::string response);
void handle_user_OPERATOR(int fd, std::string response);
void handle_user_KICK(int fd, std::string response);
void handle_user_PRIVMSG(int fd, std::string response);
void handle_user_QUIT(int fd, std::string response);
void handle_user_OTHER(int fd, std::string response);

void sendMsg(int fd, char *str) {
	if (send(fd, str, strlen(str), 0) < 0) {
		fprintf(stderr, "Send Failed\n");
	}
}

void handle_user(int fd, char* _response, int response_size) {
	_response[strcspn(_response, "\n")] = 0;
	std::string response(_response);
	std::string command = response.substr(0, response.find(' '));

	if (command == "USER") {
		handle_user_USER(fd, command);
	}
	else if (command == "LIST") {
		handle_user_LIST(fd, command);
	}
	else if (command == "JOIN") {
		handle_user_JOIN(fd, command);
	}
	else if (command == "PART") {
		handle_user_PART(fd, command);
	}
	else if (command == "OPERATOR") {
		handle_user_OPERATOR(fd, command);
	}
	else if (command == "KICK") {
		handle_user_KICK(fd, command);
	}
	else if (command == "PRIVMSG") {
		handle_user_PRIVMSG(fd, command);
	}
	else if (command == "QUIT") {
		handle_user_QUIT(fd, command);
	}
	else {
		handle_user_OTHER(fd, command);
	}
}

void handle_user_USER(int fd, std::string response) {
}

void handle_user_LIST(int fd, std::string response) {
}

void handle_user_JOIN(int fd, std::string response) {
}

void handle_user_PART(int fd, std::string response) {
}

void handle_user_OPERATOR(int fd, std::string response) {
}

void handle_user_KICK(int fd, std::string response) {
}

void handle_user_PRIVMSG(int fd, std::string response) {
}

void handle_user_QUIT(int fd, std::string response) {
}

void handle_user_OTHER(int fd, std::string response) {
}

