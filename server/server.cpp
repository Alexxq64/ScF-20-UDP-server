#include "server.h"
#include "chatDB.h"


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

    if (bind(serverSocket, (const sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
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
    bytesReceived = recvfrom(serverSocket, buffer, sizeof(buffer), 0, (sockaddr*)&clientAddress, &clientAddressSize);
    if (bytesReceived == SOCKET_ERROR) {
        std::cerr << "Failed to receive message" << std::endl;
    }
    std::cout << "Received from " << getIP(clientAddress) << " : " << getPort(clientAddress) << " ---> " << buffer << std::endl;
}

void registerClient(const std::string& cName) {
    currentClientName = cName;
    if (isSignedUp(cName)) {
                sendMessageTo("C:", clientAddress); // Instruct the client to verify the password
    }
    else {
        insertNewClient(cName, getIP(clientAddress), getPort(clientAddress));
        sendMessageTo("P:", clientAddress); // Instruct the client to enter the password
    }
}


bool checkClient(const std::string& checkName) {
    currentClientName = checkName;
    std::string text = "E";
        if (isSignedIn(checkName)) {
            std::cout << checkName << " is in the chat now" << std::endl;
            text = text + ":" + getIP(clientAddress) + ":" + std::to_string(getPort(clientAddress));
            sendMessageTo(text, clientAddress);
            return true;
        }
        if (isSignedUp(checkName)) {
            std::cout << checkName << " is in the list, but now is not in the chat" << std::endl;
            sendMessageTo("A", clientAddress);
            return true;
        }
    std::cout << checkName << " is not in the list" << std::endl;
    sendMessageTo("N:", clientAddress);
    return false;
}

//void clientsList() {
    //std::string list = "L";
        //sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
        //std::unique_ptr<sql::Connection> con(driver->connect("tcp://hostname:port", "username", "password"));
        //con->setSchema("mySQL_chat");

        //// Execute the query to retrieve signed-in clients
        //std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("SELECT username FROM users WHERE signed_in = 1"));
        //std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        // Process the query result
        //while (res->next()) {
        //    std::string username = res->getString("username");
        //    list += ":" + username;
        //}

        //// Close the statement and connection
        //pstmt->close();

    //list += getSignedInClients();
//    sendMessageTo("L:" + getSignedInClients(), clientAddress);
//}

void handleMessage() {
    command = buffer[0];
    text = "";
    text.append(&buffer[2]);
    switch (command) {
    case 'R':
        registerClient(text);
        break;
    case 'C':
        checkClient(text);
        break;
    case 'V':
        if (!verifyPw(currentClientName, text)) {
            sendMessageTo("B:", clientAddress);
        }
        break;
    case 'S':
        savePw(currentClientName, text);
        break;
    case 'L':
        sendMessageTo("L:" + getSignedInClients(), clientAddress);
        break;
    case 'M':
        std::cout << "M:" << text << std::endl;
        break;
    default:
        break;
    }
}

int main() {
    if (!openDB()) return -1;
    if (!startServer()) return -1;

    while (true) {
        getMessage();
        handleMessage();
    }

    WSACleanup();

    closeBD();

    return 0;
}
