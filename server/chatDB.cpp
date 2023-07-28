#include "chatDB.h"
#include <string>

MYSQL mysql;
MYSQL_RES* res;
MYSQL_ROW row;

bool openDB() {

	// �������� ���������� ����������
	mysql_init(&mysql);
	if (&mysql == nullptr) {
		// ���� ���������� �� ������� � ������� ��������� �� ������
		std::cout << "Error: can't create MySQL-descriptor" << std::endl;
	}

	// ������������ � �������
	if (!mysql_real_connect(&mysql, "localhost", "Alex", "1234", "chat", NULL, NULL, 0)) {
		// ���� ��� ����������� ���������� ���������� � �� ������� ��������� �� ������
		std::cout << "Error: can't connect to database " << mysql_error(&mysql) << std::endl;
	}
	else {
		// ���� ���������� ������� ����������� ������� ����� � "Success!"
		std::cout << "Success!" << std::endl;
	}

	mysql_set_character_set(&mysql, "utf8");
	//������� ���������� �� ��������� �� ������, �� ��������� ��� latin1
	std::cout << "connection characterset: " << mysql_character_set_name(&mysql) << std::endl;

	SetConsoleOutputCP(CP_UTF8);
	getMessages();
	getAllClients();
	std::cout << std::endl << std::endl;
	getSignedInClients();

	system("Pause");

	return true;
}

void insertNewClient(const std::string cName, const std::string IP, int port) {
	// Insert new client data into the 'users' table in the database
	std::string insertQuery = "INSERT INTO users (username, ip, port) VALUES ('" + cName + "', '" + IP + "', " + std::to_string(port) + ")";
	mysql_query(&mysql, insertQuery.c_str());
}

void getMessages(){
	size_t i = 0;
	mysql_query(&mysql, "SELECT * FROM messages"); //������ ������ � �������

	SetConsoleOutputCP(CP_UTF8);
	//������� ��� ��� ���� � ���� ����� ����
	if (res = mysql_store_result(&mysql)) {
		while (row = mysql_fetch_row(res)) {
			for (i = 0; i < mysql_num_fields(res); i++) {
				std::cout << row[i] << "  ";
			}
			std::cout << std::endl;
		}
	}
	else
		std::cout << "������ MySql ����� " << mysql_error(&mysql);
}

std::string getSignedInClients() {
	size_t i = 0;
	std::string list = "There are in the chat now:  \n";
	mysql_query(&mysql, "SELECT username FROM users WHERE signed_in = 1"); //������ ������ � �������

	if (res = mysql_store_result(&mysql)) {
		while (row = mysql_fetch_row(res)) {
			list = list + row[0] + ", ";
		}
		list[list.length()-2] = '.';
	}
	else
		std::cout << "������ MySql ����� " << mysql_error(&mysql);
	std::cout << list << std::endl;
	return list;
}

void getAllClients(){
	size_t i = 0;
	mysql_query(&mysql, "SELECT * FROM users"); //������ ������ � �������

	if (res = mysql_store_result(&mysql)) {
		while (row = mysql_fetch_row(res)) {
			for (i = 0; i < mysql_num_fields(res); i++) {
				std::cout << row[i] << "  ";
			}
			std::cout << std::endl;
		}
	}
	else
		std::cout << "������ MySql ����� " << mysql_error(&mysql);
}

bool isSignedUp(const std:: string& name) {
	std::string query = "SELECT * FROM users WHERE username = '" + name + "'";
	if (mysql_query(&mysql, query.c_str()) == 0) {
		res = mysql_store_result(&mysql);
		if (res) {
			return mysql_num_rows(res) > 0;
		}
	}
	return false;
}

bool isSignedIn(const std::string& name) {
	std::string query = "SELECT * FROM users WHERE username = '" + name + "' AND signed_in = 1";
	if (mysql_query(&mysql, query.c_str()) == 0) {
		res = mysql_store_result(&mysql);
		if (res) {
			return mysql_num_rows(res) > 0;
		}
	}
	return false;
}


void closeBD()
{
	// ��������� ���������� � �������� ���� ������
	mysql_free_result(res);
	mysql_close(&mysql);
}

bool verifyPw(const std::string& name, const std::string& password) {
	std::string query = "SELECT password FROM users WHERE username = '" + name + "'";
	if (mysql_query(&mysql, query.c_str()) == 0) {
		res = mysql_store_result(&mysql);
		if (res) {
			if (mysql_num_rows(res) > 0) {
				// ���� ��� ������������ �������, ��������� ������
				MYSQL_ROW row = mysql_fetch_row(res);
				if (row[4] == password) {
					std::string query = "UPDATE users SET signed_in = 1 WHERE username = '" + name + "'";
					if (mysql_query(&mysql, query.c_str()) != 0) {
						std::cout << "������ ��� ��������� �������: " << mysql_error(&mysql) << std::endl;
					}
					return true; // ������ ���������
				}
			}
		}
	}
	return false; // ��� ������������ �� ������� ��� ������ �� ���������
}


void savePw(const std::string& name, const std::string& password) {
	std::string query = "UPDATE users SET password = '" + password + "' WHERE username = '" + name + "'";
	if (mysql_query(&mysql, query.c_str()) != 0) {
		std::cout << "������ ��� ���������� ������: " << mysql_error(&mysql) << std::endl;
	}
	else {
		std::string query = "UPDATE users SET signed_in = 1 WHERE username = '" + name + "'";
		if (mysql_query(&mysql, query.c_str()) != 0) {
			std::cout << "������ ��� ��������� �������: " << mysql_error(&mysql) << std::endl;
		}
	}
}



