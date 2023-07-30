#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "chatDB.h"

#define PORT 12345

SOCKET serverSocket;
sockaddr_in serverAddress;
sockaddr_in clientAddress;
char buffer[1024];
int bytesReceived;
char command;
std::string sender;
std::string receiver;
std::string text;
std::string currentClientName;

std::string getIP(const sockaddr_in& socketAddress);

int getPort(const sockaddr_in& socketAddress);

sockaddr_in getAddress(const std::string& IP, int port);

bool sendMessage(const std::string& text, const sockaddr_in& address);

bool startServer();

void getMessage();

void registerClient(const std::string& cName);

bool checkClient(const std::string& checkName);

//void clientsList();

void handleMessage();


