#pragma once

#include "helper.h"


std::mutex mtx;

void handle_user(int fd, char *_response, int response_size, struct UserData *userdata);

void handle_user_USER(int fd, std::string response, struct UserData *userdata);

void handle_user_LIST(int fd, std::string response, struct UserData *userdata);

void handle_user_JOIN(int fd, std::string response, struct UserData *userdata);

void handle_user_PART(int fd, std::string response, struct UserData *userdata);

void handle_user_OPERATOR(int fd, std::string response, struct UserData *userdata);

void handle_user_KICK(int fd, std::string response, struct UserData *userdata);

void handle_user_PRIVMSG(int fd, std::string response, struct UserData *userdata);

void handle_user_QUIT(int fd, std::string response, struct UserData *userdata);

void handle_user_OTHER(int fd, std::string response, struct UserData *userdata);

void handle_user(int fd, char *_response, int response_size, struct UserData *userdata) {
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
        } else if (command == "LIST") {
            handle_user_LIST(fd, response, userdata);
        } else if (command == "JOIN") {
            handle_user_JOIN(fd, response, userdata);
        } else if (command == "PART") {
            handle_user_PART(fd, response, userdata);
        } else if (command == "OPERATOR") {
            handle_user_OPERATOR(fd, response, userdata);
        } else if (command == "KICK") {
            handle_user_KICK(fd, response, userdata);
        } else if (command == "PRIVMSG") {
            handle_user_PRIVMSG(fd, response, userdata);
        } else if (command == "QUIT") {
            handle_user_QUIT(fd, response, userdata);
        } else {
            handle_user_OTHER(fd, response, userdata);
        }
    }
    catch (const std::exception &e) {
        std::cerr << "Caught exception [" << e.what() << "]\n";
        sendMsg(fd, "Invalid command\n");
    }

}

//finished
void handle_user_USER(int fd, std::string response, struct UserData *userdata) {
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
    userdata->fd = fd;
    auto it = serverData.allUsers.find(strList[1]);
    if (it != serverData.allUsers.end()) {
        serverData.allUsers[strList[1]] = userdata;
        serverData.allUsers.erase(it);
        std::string tmp = "Change username to " + strList[1] + "\n";
        sendMsg(fd, tmp);
    } else {
        serverData.allUsers[strList[1]] = userdata;
        std::string tmp = "Welcome, " + strList[1] + "\n";
        sendMsg(fd, tmp);
    }
}

//finished
void handle_user_LIST(int fd, std::string response, struct UserData *userdata) {
    std::string tmp = "";
    if (response.find(' ') != std::string::npos) {//in case "list"
        std::vector<std::string> strList = splitStr(response, ' ');
        if (strList.size() > 1) {//in case "list "
            int channelNum = findChannel(strList[1]);
            if (channelNum >= 0) {//in case "list invalid"
                for (unsigned int i = 0; i < serverData.channels[channelNum].user.size(); ++i) {
                    tmp += serverData.channels[channelNum].user[i] + "\n";
                }
                sendMsg(fd, tmp);
                return;
            }
            sendMsg(fd, "Invalid channel\n");
            return;
        }
    }

    for (auto &channel : serverData.channels) {
        tmp += "* " + channel.name + "\n";
    }
    sendMsg(fd, tmp);
}

//finished
void handle_user_JOIN(int fd, std::string response, struct UserData *userdata) {
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
        serverData.channels.push_back(ch);
        channelNum = findChannel(strList[1]);
    } else {
        //add user into existing channel
        serverData.channels[channelNum].user.push_back(userdata->username);
    }

    std::ostringstream ss;
    ss << "Joined channel " << strList[1] << std::endl;
    sendMsg(fd, ss.str());

    for (const std::string &userSent : serverData.channels[channelNum].user) {
        // sent join message to all users in the channel
        auto it = serverData.allUsers.find(userSent);
        if (it != serverData.allUsers.end() && it->second->username != userdata->username) {
            std::ostringstream temp;
            //#netprog> justin joined the channel.
            temp << strList[1] << "> " << userdata->username << " joined the channel." << std::endl;
            sendMsg(it->second->fd, temp.str());
        }
    }
}

//finished
void handle_user_PART(int fd, std::string response, struct UserData *userdata) {
    if (response.find(' ') == std::string::npos) {//in case "PART"
        sendMsg(fd, "Invalid command\n");
        return;
    }
    std::vector<std::string> strList = splitStr(response, ' ');
    if (strList.size() == 1) { // remove from all channel
        for (unsigned int i = 0; i < serverData.channels.size(); ++i) {
            for (unsigned int j = 0; j < serverData.channels[i].user.size(); ++j) {
                if (serverData.channels[i].user[j] == userdata->username) {
                    serverData.channels[i].user.erase(serverData.channels[i].user.begin() + j);
                    break;
                }
            }
        }
    } else {
        int channelNum = findChannel(strList[1]);
        if (channelNum == -1) {
            sendMsg(fd, "Invalid command\n");
            return;
        } else {
            mtx.lock();
            for (unsigned int j = 0; j < serverData.channels[channelNum].user.size(); ++j) {
                if (serverData.channels[channelNum].user[j] == userdata->username) {
                    serverData.channels[channelNum].user.erase(serverData.channels[channelNum].user.begin() + j);
                    break;
                }
            }
            mtx.unlock();
        }
    }

}

//finished
void handle_user_OPERATOR(int fd, std::string response, struct UserData *userdata) {
    if (response.find(' ') == std::string::npos) {//in case "OPERATOR"
        sendMsg(fd, "Invalid command\n");
        return;
    }
    std::vector<std::string> strList = splitStr(response, ' ');
    if (strList.size() < 2) {//in case "OPERATOR "
        sendMsg(fd, "Invalid command\n");
        return;
    }
    if (strList[1] == serverData.password) {
        //validate status
        userdata->isOperator = true;
        sendMsg(fd, "OPERATOR status bestowed.\n");
    } else {
        sendMsg(fd, "Invalid OPERATOR command.\n");
    }
}

void handle_user_KICK(int fd, std::string response, struct UserData *userdata) {
    if (response.find(' ') == std::string::npos) {//in case "KICK"
        sendMsg(fd, "Invalid command\n");
        return;
    }
    std::vector<std::string> strList = splitStr(response, ' ');
    if (strList.size() < 3) {//in case "OPERATOR "
        sendMsg(fd, "Invalid command\n");
        return;
    }
    if (!userdata->isOperator) {
        sendMsg(fd, "You are not Operator\n");
        return;
    }
    std::string kickUser = strList[2];

    int channelNum = findChannel(strList[1]);
    if (channelNum == -1) {
        sendMsg(fd, "Channel not found \n");
        return;
    }
    auto it = std::find(serverData.channels[channelNum].user.begin(),
                        serverData.channels[channelNum].user.end(), kickUser);

    if (it != serverData.channels[channelNum].user.end()) {
        std::ostringstream temp;
        temp << strList[1] << "> " << kickUser << " has been kicked from the channel." << std::endl;
        channelBoardCast(strList[1], temp.str(), userdata);
        mtx.lock();
        serverData.channels[channelNum].user.erase(it);
        mtx.unlock();
    } else {
        sendMsg(fd, "KICK: User not found \n");
    }
}

void handle_user_PRIVMSG(int fd, std::string response, struct UserData *userdata) {
    if (response.find(' ') == std::string::npos) {//in case "JOIN"
        sendMsg(fd, "Invalid command\n");
        return;
    }
    std::vector<std::string> strList = splitStr(response, ' ');

    if (strList.size() < 3) {//in case "PRIVMSG "
        sendMsg(fd, "Invalid command\n");
        return;
    }

    if (strList[2].size() > 512) {
        // too long message
        sendMsg(fd, "Invalid command\n");
        return;
    }

    if (strList[1][0] == '#') {
        // send message to the whole channel
        int channelNum = findChannel(strList[1]);
        if (channelNum == -1) {
            sendMsg(fd, "Channel not found \n");
            return;
        }
        std::string message = strList[1] + "> " + userdata->username + ":" + strList[2] + "\n";
        for (auto &userSent : serverData.channels[channelNum].user) {
            sendMsg(serverData.allUsers[userSent]->fd, message);
        }
    } else {
        // send message to one user
        auto it = serverData.allUsers.find(strList[1]);
        if (it != serverData.allUsers.end()) {
            std::string message = userdata->username + "> " + strList[2] + "\n";
            sendMsg(it->second->fd, message);
        } else {
            sendMsg(fd, "User not found \n");
        }
    }
}

//finished
void handle_user_QUIT(int fd, std::string response, struct UserData *userdata) {
    //remove from all channels
    for (auto &channel : serverData.channels) {
        for (unsigned int j = 0; j < channel.user.size(); ++j) {
            if (channel.user[j] == userdata->username) {
                channel.user.erase(channel.user.begin() + j);
                break;
            }
        }
    }
    delete (userdata);
    close(fd);
}

//finished
void handle_user_OTHER(int fd, std::string response, struct UserData *userdata) {
    sendMsg(fd, "Invalid command\n");
}