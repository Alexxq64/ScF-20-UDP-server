#include "console.h"

void consoleThread() {
    bool toStop = false;
    std::string command;

    Logger logger("log.txt"); // ������� ������ Logger

    while (!toStop) {
        std::cout << "\nEnter a command (or 'exit' to quit): ";
        std::getline(std::cin, command);

        if (command == "exit") {
            std::cout << "Bye! See you later!";
            toStop = true;
        }
        else if (command == "log") {
                std::cout << logger.getLastLog() << std::endl;
            //std::vector<std::string> logs = logger.getLastNLogs(10); // �������� ��������� 10 �����
            //for (const auto& log : logs) {
            //    std::cout << log << std::endl;
            }
        else if (command == "test") {
            std::cout << "------    Test    --------" << std::endl;
        }
        // ������ ������ ��������� ������
    }
}