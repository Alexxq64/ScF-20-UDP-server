#pragma once
#include "mysql.h"
#include <iostream>

bool openDB();
void insertNewClient(const std::string cName, const std::string IP, int port);
void getMessages();
void getAllClients();
bool isSignedUp(const std::string& name);
bool isSignedIn(const std::string& name);
std::string getSignedInClients();
void closeBD();
bool verifyPw(const std::string& name, const std::string& password);
void savePw(const std::string& name, const std::string& password);