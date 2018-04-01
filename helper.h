#pragma once

#include <iostream>
#include <sstream>
#include <vector>
#include <iterator>
#include <exception>
#include <string>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <unistd.h>
#include <dirent.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <pthread.h>
#include <map>

#define BUFFER_SIZE 64
#define CHANNEL_NAME_LENGTH 20

struct ChannelData {
    std::string name;
    std::vector<std::string> user;
    std::map<std::string, struct UserData> user2;
};

struct ServerData {
    std::string password;
    std::vector<ChannelData> channels;
} serverData;

struct UserData {
    std::string username;
    bool isOperator = false;
};

void sendMsg(int fd, std::string str);

void sendMsg(int fd, char *str);

template<typename T>
void splitStr(const std::string &s, char delim, T result);

std::vector<std::string> splitStr(std::string &s, char delim);

int findChannel(std::string str);

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

//find channel after removing the '#' on start of str
int findChannel(std::string str) {
    if (str[0] != '#') {
        return -1;
    }
    str = str.substr(1);
    for (unsigned int i = 0; i < serverData.channels.size(); ++i) {
        if (serverData.channels[i].name == str) {
            return i;
        }
    }
    return -1;
}