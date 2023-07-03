#define _WINSOCK_DEPRECATED_NO_WARNINGS

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
typedef int SOCKET;
const int INVALID_SOCKET = -1;
const int SOCKET_ERROR = -1;
#define closesocket(s) close(s)
#endif

#include <iostream>
#include <vector>
#include <map>
#include <string>

#define PORT 12345

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
sockaddr_in serverAddress;
sockaddr_in clientAddress;
char buffer[1024];
int bytesReceived;
char command;
std::string text;

std::string getIP(const sockaddr_in& socketAddress) {
    char* ip = inet_ntoa(socketAddress.sin_addr);
    if (ip)
        return std::string(ip);
    return "";
}

int getPort(const sockaddr_in& socketAddress) {
    return ntohs(socketAddress.sin_port);
}

sockaddr_in getAddress(const std::string& IP, int port) {
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(IP.c_str());
    address.sin_port = htons(port);
    return address;
}

bool sendMessageTo(const std::string& text, const sockaddr_in& address) {
    if (sendto(serverSocket, text.c_str(), text.size(), 0, (const sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        return false;
    }
    std::cout << "      Sent to " << getIP(address) << " : " << getPort(address) << " ---> " << text << std::endl;
    return true;
}

bool startServer() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize winsock" << std::endl;
        return false;
    }
#endif

    serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket" << std::endl;
#ifdef _WIN32
        WSACleanup();
#endif
        return false;
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    if (bind(serverSocket, (const sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Failed to bind socket" << std::endl;
        closesocket(serverSocket);
#ifdef _WIN32
        WSACleanup();
#endif
        return false;
    }

    std::cout << "Server started on " << getIP(serverAddress) << " : " << getPort(serverAddress) << std::endl;
    return true;
}

void getMessage() {
    memset(buffer, 0, sizeof(buffer));
#ifdef _WIN32
    int clientAddressSize = sizeof(clientAddress);
#else
    socklen_t clientAddressSize = sizeof(clientAddress);
#endif
    bytesReceived = recvfrom(serverSocket, buffer, sizeof(buffer), 0, (sockaddr*)&clientAddress, &clientAddressSize);
    if (bytesReceived == SOCKET_ERROR) {
        std::cerr << "Failed to receive message" << std::endl;
    }
    std::cout << "Received from " << getIP(clientAddress) << " : " << getPort(clientAddress) << " ---> " << buffer << std::endl;
}
void registerNewClient(const std::string& newName) {
    Client newClient;
    newClient.name = newName;
    newClient.ip = getIP(clientAddress);
    newClient.port = getPort(clientAddress);
    std::cout << "New client: " << newClient.name << std::endl;
    clients.push_back(newClient);
    for (const auto& c : clients) {
        c.print("    ");
    }
    std::cout << std::endl;
}

bool checkClient(const std::string& checkName) {
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
    switch (command) {
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

#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}
