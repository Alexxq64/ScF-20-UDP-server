#pragma once
#include "mysql.h"
#include <iostream>
#include <sstream>
#include <ctime>
#include <iomanip>
//#include <mysql_driver.h>
//#include <mysql_connection.h>

bool openDB();
void clearChat();
void fixAddress(std::string cName, std::string IP, int port);
void insertNewClient(const std::string cName, const std::string IP, int port);
void getMessages();
void getAllClients();
bool isSignedUp(const std::string& name);
bool isSignedIn(const std::string& name);
std::string getSignedInClients();
void closeBD();
bool verifyPw(const std::string& name, const std::string& password);
void savePw(const std::string& name, const std::string& password);
std::string getIPfromDB(std::string checkName);
int getPortFromDB(std::string checkName);
void saveMessageIntoDB(std::string sender, std::string receiver, std::string text);
int getClientID(std::string client);
std::string getNow();
std::string getMessageHistory(std::string currentClientName);
std::string getNameByID(int id);
std::string getNameByAddress(const std::string IP, int port);
//void createDatabaseIfNotExists(const std::string& dbName, const std::string& host, const std::string& user, const std::string& password);