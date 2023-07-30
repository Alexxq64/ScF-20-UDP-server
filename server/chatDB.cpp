#include "chatDB.h"
#include <string>

MYSQL mysql;
MYSQL_RES* res;
MYSQL_ROW row;

bool openDB() {
	// it's assumed that the datebase already exists 
	// it contains 2 tables
	// users: user_id, username, ip, port, signed_in
	// messges: message_id, sender_id, receiver_id, message_text, send_date

	// Get connection descriptor
	mysql_init(&mysql);
	if (&mysql == nullptr) {
		// If it is not got display an error message
		std::cout << "Error: can't create MySQL-descriptor" << std::endl;
	}

	// Connect to server
	if (!mysql_real_connect(&mysql, "localhost", "Alex", "1234", "chat", NULL, NULL, 0)) {
		// If no connect display an error message
		std::cout << "Error: can't connect to database " << mysql_error(&mysql) << std::endl;
	}

	mysql_set_character_set(&mysql, "utf8");
	// Check coding
	std::cout << "connection characterset: " << mysql_character_set_name(&mysql) << std::endl;

	SetConsoleOutputCP(CP_UTF8);
	getMessages();
	std::cout << std::endl;
	clearChat();
	getAllClients();
	std::cout << std::endl << std::endl;
	getSignedInClients();

	//system("Pause");

	return true;
}


// tried to create a database in case of its absence
// it didn't work out yet

//void createDatabaseIfNotExists(const std::string& dbName, const std::string& host, const std::string& user, const std::string& password) {
//	try {
//		sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
//		std::unique_ptr<sql::Connection> con(driver->connect(host, user, password));
//		con->setSchema("mysql");
//
//		std::stringstream createDbQuery;
//		createDbQuery << "CREATE DATABASE IF NOT EXISTS " << dbName;
//		std::unique_ptr<sql::Statement> stmt(con->createStatement());
//		stmt->execute(createDbQuery.str());
//
//		con->setSchema(dbName);
//
//		std::string createUsersTableQuery =
//			"CREATE TABLE IF NOT EXISTS users ("
//			"user_id INT AUTO_INCREMENT PRIMARY KEY,"
//			"username VARCHAR(255) NOT NULL,"
//			"ip VARCHAR(45) NOT NULL,"
//			"port INT NOT NULL,"
//			"password VARCHAR(255) NOT NULL,"
//			"signed_in TINYINT(1) DEFAULT 0"
//			") ENGINE=InnoDB";
//
//		std::string createMessagesTableQuery =
//			"CREATE TABLE IF NOT EXISTS messages ("
//			"message_id INT AUTO_INCREMENT PRIMARY KEY,"
//			"sender_id INT NOT NULL,"
//			"receiver_id INT NOT NULL,"
//			"message_text TEXT NOT NULL,"
//			"send_date TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,"
//			"FOREIGN KEY (sender_id) REFERENCES users (user_id) ON DELETE CASCADE,"
//			"FOREIGN KEY (receiver_id) REFERENCES users (user_id) ON DELETE CASCADE"
//			") ENGINE=InnoDB";
//
//		stmt->execute(createUsersTableQuery);
//		stmt->execute(createMessagesTableQuery);
//
//		std::cout << "Database " << dbName << " with required tables has been created/verified." << std::endl;
//	}
//	catch (sql::SQLException& e) {
//		std::cout << "MySQL Error: " << e.what() << std::endl;
//	}
//}





void clearChat(){
	std::stringstream query;
	query << "UPDATE users SET signed_in = 0";
	if (mysql_query(&mysql, query.str().c_str()) != 0) {
		std::cout << "Ошибка при изменении статуса: " << mysql_error(&mysql) << std::endl;
	}
}

void fixAddress(std::string cName, std::string IP, int port) {
    std::stringstream query;
    query << "UPDATE users SET signed_in = 1, IP = '" << IP << "', port = " << port << " WHERE username = '" << cName << "'";
    if (mysql_query(&mysql, query.str().c_str()) != 0) {
        std::cout << "Ошибка при изменении статуса: " << mysql_error(&mysql) << std::endl;
    }
}

void insertNewClient(const std::string cName, const std::string IP, int port) {
	// Insert new client data into the 'users' table in the database
	std::string insertQuery = "INSERT INTO users (username, ip, port, password) VALUES ('" + cName + "', '" + IP + "', " + std::to_string(port) + ", '" + cName + "')";
	//std::cout << "Query to insert: " << std::endl << insertQuery.c_str() << std::endl;
	mysql_query(&mysql, insertQuery.c_str());
}

void getMessages(){
	size_t i = 0;
	mysql_query(&mysql, "SELECT * FROM messages"); //Делаем запрос к таблице

	SetConsoleOutputCP(CP_UTF8);
	//Выводим все что есть в базе через цикл
	if (res = mysql_store_result(&mysql)) {
		while (row = mysql_fetch_row(res)) {
			for (i = 0; i < mysql_num_fields(res); i++) {
				std::cout << row[i] << "  ";
				if (i == 0) std::cout << "\t";
			}
			std::cout << std::endl;
		}
	}
	else
		std::cout << "Ошибка MySql номер " << mysql_error(&mysql);
}

std::string getSignedInClients() {
	size_t i = 0;
	std::string list = "There are in the chat now:  \n";
	int thereAreInTheChatNow = 0;
	mysql_query(&mysql, "SELECT username FROM users WHERE signed_in = 1"); //Делаем запрос к таблице

	if (res = mysql_store_result(&mysql)) {
		while (row = mysql_fetch_row(res)) {
			thereAreInTheChatNow++;
			list = list + row[0] + ", ";
		}
		list[list.length()-2] = '.';
	}
	else
		std::cout << "Ошибка MySql номер " << mysql_error(&mysql);
	if (thereAreInTheChatNow == 0 ) list = "There is nobody in the chat now.";
	std::cout << list << std::endl << std::endl;
	return list;
}

void getAllClients(){
	size_t i = 0;
	mysql_query(&mysql, "SELECT * FROM users"); //Делаем запрос к таблице

	if (res = mysql_store_result(&mysql)) {
		while (row = mysql_fetch_row(res)) {
			for (i = 0; i < mysql_num_fields(res); i++) {
				std::cout << row[i] << "\t  ";
			}
			std::cout << std::endl;
		}
	}
	else
		std::cout << "Ошибка MySql номер " << mysql_error(&mysql);
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
	// Закрываем соединение с сервером базы данных
	mysql_free_result(res);
	mysql_close(&mysql);
}

bool verifyPw(const std::string& name, const std::string& password) {
	std::string query = "SELECT password FROM users WHERE username = '" + name + "'";
	if (mysql_query(&mysql, query.c_str()) == 0) {
		res = mysql_store_result(&mysql);
		if (res) {
			if (mysql_num_rows(res) > 0) {
				// Если имя пользователя найдено, проверяем пароль
				MYSQL_ROW row = mysql_fetch_row(res);
				if (row[0] == password) {
					std::string query = "UPDATE users SET signed_in = 1 WHERE username = '" + name + "'";
					if (mysql_query(&mysql, query.c_str()) != 0) {
						std::cout << "Ошибка при изменении статуса: " << mysql_error(&mysql) << std::endl;
					}
					return true; // Пароль совпадает
				}
			}
		}
	}
	return false; // Имя пользователя не найдено или пароль не совпадает
}


void savePw(const std::string& name, const std::string& password) {
	std::string query = "UPDATE users SET password = '" + password + "' WHERE username = '" + name + "'";
	if (mysql_query(&mysql, query.c_str()) != 0) {
		std::cout << "Ошибка при сохранении пароля: " << mysql_error(&mysql) << std::endl;
	}
	else {
		std::string query = "UPDATE users SET signed_in = 1 WHERE username = '" + name + "'";
		if (mysql_query(&mysql, query.c_str()) != 0) {
			std::cout << "Ошибка при изменении статуса: " << mysql_error(&mysql) << std::endl;
		}
	}
}

std::string getIPfromDB(std::string checkName) {
	std::string query = "SELECT ip FROM users WHERE username = '" + checkName + "'";
	mysql_query(&mysql, query.c_str());
		res = mysql_store_result(&mysql);
		return mysql_fetch_row(res)[0];
}

int getPortFromDB(std::string checkName){
	std::string query = "SELECT port FROM users WHERE username = '" + checkName + "'";
	mysql_query(&mysql, query.c_str());
	res = mysql_store_result(&mysql);
	return atoi (mysql_fetch_row(res)[0]);
}


int getClientID(std::string client){
	std::string query = "SELECT user_id FROM users WHERE username = '" + client + "'";
	mysql_query(&mysql, query.c_str());
	res = mysql_store_result(&mysql);
	return atoi(mysql_fetch_row(res)[0]);
}

std::string getNow() {
	std::time_t now = std::time(nullptr);
	std::tm tm;
	localtime_s(&tm, &now);

	std::stringstream ss;
	ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
	return ss.str();
}


void saveMessageIntoDB(std::string sender, std::string receiver, std::string message) {
	std::string insertQuery = "INSERT INTO messages (send_date, sender_id, receiver_id, message_text) VALUES ('"
		+ getNow() + "', " + std::to_string(getClientID(sender)) + ", " + std::to_string(getClientID(receiver)) + ", '" + message + "')";
	//std::cout << "Query to insert: " << std::endl << insertQuery.c_str() << std::endl;
	mysql_query(&mysql, insertQuery.c_str());
}

std::string getMessageHistory(std::string currentClientName) {
	std::stringstream query;
	query << "SELECT sender_id, receiver_id, send_date, message_text FROM messages WHERE sender_id = '"
		<< getClientID(currentClientName) << "' OR receiver_id = '" << getClientID(currentClientName) << "'";
	std::cout << query.str().c_str() << std::endl;
	if (mysql_query(&mysql, query.str().c_str()) == 0) {
		mysql_set_character_set(&mysql, "utf8");
		MYSQL_RES* result = mysql_store_result(&mysql);
		if (result) {
			std::stringstream messageHistory;
			MYSQL_ROW row;
			while ((row = mysql_fetch_row(result))) {
				std::string sender = getNameByID(atoi(row[0]));
				std::string receiver = getNameByID(atoi(row[1]));
				std::string sendDate = row[2];
				std::string messageText = row[3];

				if (sender == currentClientName) {
					messageHistory << ">> " << receiver << " " << sendDate << " " << messageText << "\n";
				}
				else {
					messageHistory << "<< " << sender << " " << sendDate << " " << messageText << "\n";
				}
			}

			mysql_free_result(result);
			return messageHistory.str();
		}
	}

	return "Error fetching message history.";
}

std::string getNameByID(int id) {
	std::stringstream query;
	query << "SELECT username FROM users WHERE user_id = " << id;

	if (mysql_query(&mysql, query.str().c_str()) == 0) {
		MYSQL_RES* result = mysql_store_result(&mysql);
		if (result) {
			MYSQL_ROW row = mysql_fetch_row(result);
			if (row) {
				std::string username = row[0];
				mysql_free_result(result);
				return username;
			}
		}
	}

	return "User not found.";
}

std::string getNameByAddress(const std::string IP, int port){
	std::string query = "SELECT username FROM users WHERE ip = '" + IP + "'" + " AND port = " + std::to_string(port);
	std::cout << "Query: " << std::endl << query.c_str() << std::endl;
	mysql_query(&mysql, query.c_str());
	res = mysql_store_result(&mysql);
	return mysql_fetch_row(res)[0];
}
