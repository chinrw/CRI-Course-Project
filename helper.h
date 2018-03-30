#pragma once
#include <iostream>
#include <sstream>
#include <vector>
#include <iterator>
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
#include <pthread.h>

#define BUFFER_SIZE 64

struct ServerData {
	std::string password;
	std::vector<std::string> channels;
} serverdata;


struct UserData {
	std::string username;
};

void sendMsg(int fd, std::string str);
void sendMsg(int fd, char *str);
template<typename T> void splitStr(const std::string &s, char delim, T result);
std::vector<std::string> splitStr(std::string &s, char delim);
void handle_user(int fd, char* response, int response_size, struct UserData* userdata);
void handle_user_USER(int fd, std::string response, struct UserData* userdata);
void handle_user_LIST(int fd, std::string response, struct UserData* userdata);
void handle_user_JOIN(int fd, std::string response, struct UserData* userdata);
void handle_user_PART(int fd, std::string response, struct UserData* userdata);
void handle_user_OPERATOR(int fd, std::string response, struct UserData* userdata);
void handle_user_KICK(int fd, std::string response, struct UserData* userdata);
void handle_user_PRIVMSG(int fd, std::string response, struct UserData* userdata);
void handle_user_QUIT(int fd, std::string response, struct UserData* userdata);
void handle_user_OTHER(int fd, std::string response, struct UserData* userdata);

void sendMsg(int fd, std::string str) {
	if (send(fd, str.c_str(), str.length(), 0) < 0) {
		fprintf(stderr, "Send Failed\n");
	}
}

void sendMsg(int fd, const char *str) {
	if (send(fd, str, strlen(str), 0) < 0) {
		fprintf(stderr, "Send Failed\n");
	}
}

template<typename T>
void splitStr(const std::string &s, char delim, T result) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		*(result++) = item;
	}
}

std::vector<std::string> splitStr(std::string &s, char delim) {
	std::vector<std::string> elems;
	splitStr(s, delim, std::back_inserter(elems));
	return elems;
}

void handle_user(int fd, char* _response, int response_size, struct UserData* userdata) {
	_response[strcspn(_response, "\n")] = 0;
	std::string response(_response);
	std::string command = response.substr(0, response.find(' '));
	if (userdata->username == "" && response.find("USER") == std::string::npos) {
		sendMsg(fd, "Invalid command, please identify yourself with USER.\n");
		return;
	}

	if (command == "USER") {
		handle_user_USER(fd, response, userdata);
	}
	else if (command == "LIST") {
		handle_user_LIST(fd, response, userdata);
	}
	else if (command == "JOIN") {
		handle_user_JOIN(fd, response, userdata);
	}
	else if (command == "PART") {
		handle_user_PART(fd, response, userdata);
	}
	else if (command == "OPERATOR") {
		handle_user_OPERATOR(fd, response, userdata);
	}
	else if (command == "KICK") {
		handle_user_KICK(fd, response, userdata);
	}
	else if (command == "PRIVMSG") {
		handle_user_PRIVMSG(fd, response, userdata);
	}
	else if (command == "QUIT") {
		handle_user_QUIT(fd, response, userdata);
	}
	else {
		handle_user_OTHER(fd, response, userdata);
	}
}

void handle_user_USER(int fd, std::string response, struct UserData* userdata) {
	std::vector<std::string> strList = splitStr(response, ' ');
	userdata->username = strList[1];
	std::string tmp = "Welcome, " + strList[1] + "\n";
	sendMsg(fd, tmp);
}

void handle_user_LIST(int fd, std::string response, struct UserData* userdata) {
	std::string tmp = "";
	for (int i = 0; i < serverdata.channels.size(); ++i) {
		tmp.append("* ");
		tmp.append(serverdata.channels[i]);
		tmp.append("\n");
	}
	sendMsg(fd, tmp);
}

void handle_user_JOIN(int fd, std::string response, struct UserData* userdata) {
}

void handle_user_PART(int fd, std::string response, struct UserData* userdata) {
}

void handle_user_OPERATOR(int fd, std::string response, struct UserData* userdata) {
}

void handle_user_KICK(int fd, std::string response, struct UserData* userdata) {
}

void handle_user_PRIVMSG(int fd, std::string response, struct UserData* userdata) {
}

void handle_user_QUIT(int fd, std::string response, struct UserData* userdata) {
}

void handle_user_OTHER(int fd, std::string response, struct UserData* userdata) {
	sendMsg(fd, "Invalid command\n");
}

