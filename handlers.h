#pragma once
#include "helper.h"

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

void handle_user(int fd, char* _response, int response_size, struct UserData* userdata) {
	_response[strcspn(_response, "\n")] = 0;
	std::string response(_response);
	std::string command = response.substr(0, response.find(' '));
	if (userdata->username == "" && response.find("USER") == std::string::npos) {
		sendMsg(fd, "Invalid command, please identify yourself with USER.\n");
		return;
	}

	try {
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
	catch (const std::exception& e) {
		std::cerr << "Caught exception [" << e.what() << "]\n";
		sendMsg(fd, "Invalid command\n");
	}

}

//finished
void handle_user_USER(int fd, std::string response, struct UserData* userdata) {
	if (response.find(' ') == std::string::npos) {//in case "USER"
		sendMsg(fd, "Invalid command\n");
		return;
	}
	std::vector<std::string> strList = splitStr(response, ' ');
	if (strList.size() < 2) {//in case "USER "
		sendMsg(fd, "Invalid command\n");
		return;
	}
	userdata->username = strList[1];
	std::string tmp = "Welcome, " + strList[1] + "\n";
	sendMsg(fd, tmp);
}

//finished
void handle_user_LIST(int fd, std::string response, struct UserData* userdata) {
	std::string tmp = "";
	if (response.find(' ') != std::string::npos) {//in case "list"
		std::vector<std::string> strList = splitStr(response, ' ');
		if (strList.size() > 1) {//in case "list "
			int channelNum = findChannel(strList[1]);
			if (channelNum >= 0) {//in case "list invalid"
				for (unsigned int i = 0; i < serverdata.channels[channelNum].user.size(); ++i) {
					tmp += serverdata.channels[channelNum].user[i] + "\n";
				}
				sendMsg(fd, tmp);
				return;
			}
			sendMsg(fd, "Invalid channel\n");
			return;
		}
	}

	for (unsigned int i = 0; i < serverdata.channels.size(); ++i) {
		tmp += "* " + serverdata.channels[i].name + "\n";
	}
	sendMsg(fd, tmp);
}

//finished
void handle_user_JOIN(int fd, std::string response, struct UserData* userdata) {
	if (response.find(' ') == std::string::npos) {//in case "JOIN"
		sendMsg(fd, "Invalid command\n");
		return;
	}
	std::vector<std::string> strList = splitStr(response, ' ');
	if (strList.size() < 2) {//in case "JOIN "
		sendMsg(fd, "Invalid command\n");
		return;
	}
	int channelNum = findChannel(strList[1]);
	if (strList[1][0] == '#' && channelNum == -1) {
		//add new channel and add user into channel
		struct ChannelData ch;
		ch.name = strList[1].substr(1);
		ch.user.push_back(userdata->username);
		serverdata.channels.push_back(ch);
	}
	else {
		//add user into existing channel
		serverdata.channels[channelNum].user.push_back(userdata->username);
	}
}

//finished
void handle_user_PART(int fd, std::string response, struct UserData* userdata) {
	if (response.find(' ') == std::string::npos) {//in case "PART"
		sendMsg(fd, "Invalid command\n");
		return;
	}
	std::vector<std::string> strList = splitStr(response, ' ');
	if (strList.size() == 1) {
		for (unsigned int i = 0; i < serverdata.channels.size(); ++i) {
			for (unsigned int j = 0; j < serverdata.channels[i].user.size(); ++j) {
				if (serverdata.channels[i].user[j] == userdata->username) {
					serverdata.channels[i].user.erase(serverdata.channels[i].user.begin() + j);
					break;
				}
			}
		}
	}
	else {
		int channelNum = findChannel(strList[1]);
		if (channelNum == -1) {
			sendMsg(fd, "Invalid command\n");
			return;
		}
		else {
			for (unsigned int j = 0; j < serverdata.channels[channelNum].user.size(); ++j) {
				if (serverdata.channels[channelNum].user[j] == userdata->username) {
					serverdata.channels[channelNum].user.erase(serverdata.channels[channelNum].user.begin() + j);
					break;
				}
			}
		}
	}

}

//finished
void handle_user_OPERATOR(int fd, std::string response, struct UserData* userdata) {
	if (response.find(' ') == std::string::npos) {//in case "OPERATOR"
		sendMsg(fd, "Invalid command\n");
		return;
	}
	std::vector<std::string> strList = splitStr(response, ' ');
	if (strList.size() < 2) {//in case "OPERATOR "
		sendMsg(fd, "Invalid command\n");
		return;
	}
	if (strList[1] == serverdata.password) {
		//validate status
		userdata->isOperator = true;
		sendMsg(fd, "OPERATOR status bestowed.\n");
	}
	else {
		sendMsg(fd, "Invalid OPERATOR command.\n");
	}
}

void handle_user_KICK(int fd, std::string response, struct UserData* userdata) {
}

void handle_user_PRIVMSG(int fd, std::string response, struct UserData* userdata) {
}

//finished
void handle_user_QUIT(int fd, std::string response, struct UserData* userdata) {
	//remove from all channels
	for (unsigned int i = 0; i < serverdata.channels.size(); ++i) {
		for (unsigned int j = 0; j < serverdata.channels[i].user.size(); ++j) {
			if (serverdata.channels[i].user[j] == userdata->username) {
				serverdata.channels[i].user.erase(serverdata.channels[i].user.begin() + j);
				break;
			}
		}
	}
	close(fd);
}

//finished
void handle_user_OTHER(int fd, std::string response, struct UserData* userdata) {
	sendMsg(fd, "Invalid command\n");
}