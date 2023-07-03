/*
there are a lot of ideas to improve this chat
add forwarding messages through the server
save history of messages that go through the server
save all clients in a data base
registration with password
remove clients from chat when quitting
make private list for each client in data base
add chat with choosed client in separate console to avoid multiple input of repeated info
add thread for server to work with its console
add message to all
profile editing
add color to output
split source files to main, source.h and .cpp, and cross-depended files 
*/


#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <vector>
#include <map>
#include <string>

#define PORT 12345

#pragma comment(lib, "ws2_32.lib")

struct Client {
	std::string name;
	std::string ip;
	int port;

	void print(std::string position = "Name") const {
		std::cout << position << ": " << name << "\t\tIP : " << ip << "\tPort : " << port << std::endl;
	}

};

std::vector<Client> clients;

SOCKET serverSocket;
SOCKADDR_IN serverAddress;
SOCKADDR_IN clientAddress;
char buffer[1024];
int bytesReceived;
char command;
std::string text;

std::string getIP(const SOCKADDR_IN& socketAddress) {
	return std::string(inet_ntoa(socketAddress.sin_addr));
}

int getPort(const SOCKADDR_IN& socketAddress) {
	return ntohs(socketAddress.sin_port);
}

SOCKADDR_IN getAddress(std::string IP, int port) {
	SOCKADDR_IN address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(IP.c_str());
	address.sin_port = htons(port);
	return address;
}



bool sendMessageTo(std::string text, SOCKADDR_IN address) {
	if (sendto(serverSocket, text.c_str(), text.size(), 0, (SOCKADDR*)&address, sizeof(address)) == SOCKET_ERROR) {
		return false;
	}
	std::cout << "      Sent to " << getIP(address) << " : " << getPort(address) << " ---> " << text << std::endl;
	return true;
}

bool startServer() {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "Failed to initialize winsock" << std::endl;
		return false;
	}
	serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (serverSocket == INVALID_SOCKET) {
		std::cerr << "Failed to create socket" << std::endl;
		WSACleanup();
		return false;
	}
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(PORT);
	if (bind(serverSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
		std::cerr << "Failed to bind socket" << std::endl;
		closesocket(serverSocket);
		WSACleanup();
		return false;
	}
	std::cout << "Server started on " << getIP(serverAddress) << " : " << getPort(serverAddress) << std::endl;
	return true;
}

void getMessage() {
	memset(buffer, 0, sizeof(buffer));
	int clientAddressSize = sizeof(clientAddress);
	bytesReceived = recvfrom(serverSocket, buffer, sizeof(buffer), 0, (SOCKADDR*)&clientAddress, &clientAddressSize);
	if (bytesReceived == SOCKET_ERROR) {
		std::cerr << "Failed to receive message" << std::endl;
	}
	std::cout << "Received from " << getIP(clientAddress) << " : " << getPort(clientAddress) << " ---> " << buffer << std::endl;
}

void registerNewClient(std::string newName) {
	Client newClient;
	newClient.name = newName;
	newClient.ip = getIP(clientAddress);
	newClient.port = getPort(clientAddress);
	std::cout << "New client: " << newClient.name << std::endl;
	clients.push_back(newClient);
	for (const auto& c : clients) {
		c.print("	");
	}
	std::cout << std::endl;
}

bool checkClient(std::string checkName) {
	std::string text = "E";
	for (const auto& c : clients) {
		if (checkName == c.name) {
			std::cout << checkName << " is in the list" << std::endl;
			text = text + ":" + c.ip + ":" + std::to_string(c.port);
			sendMessageTo(text, clientAddress);
			return true;
		}
	}
	std::cout << checkName << " is not in the list" << std::endl;
	sendMessageTo("N:", clientAddress);
	return false;
}

void clientsList() {
	std::string list = "L";
	std::cout << text << std::endl;
	for (const auto& c : clients) {
		list += ":" + c.name;
	}
	list += ".";
	sendMessageTo(list, clientAddress);
}



void handleMessage() {
	command = buffer[0];
	text = "";
	text.append(&buffer[2]);
	switch (command){
	case 'R':
		registerNewClient(text);
		break;
	case 'C':
		checkClient(text);
		break;
	case 'L':
		clientsList();
		break;
	case 'M':
		std::cout << "M:" << text << std::endl;
		break;
	default:
		break;
	}
}

int main() {

	if (!startServer()) return -1;

	while (true) {
		getMessage();
		handleMessage();
	}

	return 0;
}
