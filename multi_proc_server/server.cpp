#include <iostream>
#include <winsock2.h>

#pragma comment(lib,"ws2_32.lib") // Winsock Library
#pragma warning(disable: 4996)

// Задаем порт для прослушивания входящих соединений
const int LISTEN_PORT = 8000;

int main() {
    // Инициализируем библиотеку сокетов
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return 1;
    }

    // Создаем сокет для прослушивания входящих соединений
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        std::cerr << "socket failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Задаем параметры соединения
    sockaddr_in listenAddr;
    listenAddr.sin_family = AF_INET;
    listenAddr.sin_port = htons(LISTEN_PORT);
    listenAddr.sin_addr.s_addr = INADDR_ANY;

    // Привязываем сокет к адресу и порту
    result = bind(listenSocket, (sockaddr*)&listenAddr, sizeof(listenAddr));
    if (result == SOCKET_ERROR) {
        std::cerr << "bind failed: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // Начинаем прослушивать входящие соединения
    result = listen(listenSocket, SOMAXCONN);
    if (result == SOCKET_ERROR) {
        std::cerr << "listen failed: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server is listening on port " << LISTEN_PORT << std::endl;

    // Принимаем входящие соединения и обрабатываем сообщения
    while (true) {
        // Ожидаем входящее соединение
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        SOCKET clientSocket = accept(listenSocket, (sockaddr*)&clientAddr, &clientAddrSize);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "accept failed: " << WSAGetLastError() << std::endl;
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }

        std::cout << "Client connected: " << inet_ntoa(clientAddr.sin_addr) << std::endl;

        // Принимаем сообщения от клиента и выводим на экран
        char buffer[1024];
        while (true) {
            result = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (result == SOCKET_ERROR) {
                std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
                closesocket(clientSocket);
                break;
            }
            else if (result == 0) {
                std::cout << "Client disconnected" << std::endl;
                closesocket(clientSocket);
                break;
            }
            else {
                buffer[result] = '\0';
                std::cout << "Received message: " << buffer << std::endl;
            }
        }
    }
    closesocket(listenSocket);
    WSACleanup();
    return 0;
}
