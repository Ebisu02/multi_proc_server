#include <iostream>
#include <winsock2.h>
#include <string>

#pragma comment(lib,"ws2_32.lib") // Winsock Library
#pragma warning(disable: 4996)

// Задаем адрес и порт сервера
const char* SERVER_ADDRESS = "127.0.0.1";
const int SERVER_PORT = 8000;

int main() {
    // Инициализируем библиотеку сокетов
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return 1;
    }

    // Создаем сокет
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "socket failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Задаем параметры соединения
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);

    // Подключаемся к серверу
    result = connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (result == SOCKET_ERROR) {
        std::cerr << "connect failed: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Задаем количество и задержку отправки сообщений
    int n_messages = 10;
    const int delay = 1; // в секундах

    std::cout << "Enter an 'i' to continue: ";
    std::cin >> n_messages;

    // Отправляем сообщения на сервер
    for (int i = 1; i <= n_messages; ++i) {
        // Формируем сообщение
        std::string message = std::to_string(i);

        // Отправляем сообщение на сервер
        result = send(clientSocket, message.c_str(), message.size(), 0);
        if (result == SOCKET_ERROR) {
            std::cerr << "send failed: " << WSAGetLastError() << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }

        std::cout << "Sended: " << i << "\n";

        // Задержка
        Sleep(delay * 1000);
    }

    // Закрываем соединение и освобождаем ресурсы
    shutdown(clientSocket, SD_SEND);
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}