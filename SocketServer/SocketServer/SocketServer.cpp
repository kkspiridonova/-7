#define WIN32_LEAN_AND_MEAN

#include <Windows.h> // Windows API
#include <iostream>
#include <WinSock2.h> //библиотека для работы с сокетами
#include <WS2tcpip.h> // библиотека для работы с протоколом TCP/IP

using namespace std;

int main() {
    WSADATA wsaData;//переменные
    ADDRINFO hints;
    ADDRINFO* addrResult;
    SOCKET ListenSocket = INVALID_SOCKET;//сокет для прослушивания входящих подключений
    SOCKET ConnectSocket = INVALID_SOCKET;//сокет для соединения с клиентом
    char recvBuffer[512];

    const char* sendBuffer = "Hello from server";//строка для тправки клиенту

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);//инициализация библиотеки
    if (result != 0) {
        cout << "WSAStartup failed with result: " << result << endl;//проверка
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));//очистка
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;//ожидание соединения

    result = getaddrinfo(NULL, "666", &hints, &addrResult);//принимает адрес и порт, указывает на сокет и протокол
    if (result != 0) {
        cout << "getaddrinfo failed with error: " << result << endl;//проверка
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);//создание сокета с параметрами, которые мы узнали раньше
    if (ListenSocket == INVALID_SOCKET) {
        cout << "Socket creation failed" << endl;//проверка
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);//привязка сокета к адресу
    if (result == SOCKET_ERROR) {
        cout << "Bind failed, error: " << result << endl;//проверка
        closesocket(ListenSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    result = listen(ListenSocket, SOMAXCONN);//прослушивание входящих подключений и создание нового сокета
    if (result == SOCKET_ERROR) {
        cout << "Listen failed, error: " << result << endl;//проверка
        closesocket(ListenSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    ConnectSocket = accept(ListenSocket, NULL, NULL);//принимает входящее подключение
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Accept failed, error: " << WSAGetLastError() << endl;//проверка
        closesocket(ListenSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    closesocket(ListenSocket);//закрытие прослушивания

    do {
        ZeroMemory(recvBuffer, 512);//цикл для чтения данных от клиента
        result = recv(ConnectSocket, recvBuffer, 512, 0);
        if (result > 0) {
            cout << "Received " << result << " bytes" << endl;
            cout << "Received data: " << recvBuffer << endl;

            result = send(ConnectSocket, sendBuffer, (int)strlen(sendBuffer), 0);//отправка данных клиенту
            if (result == SOCKET_ERROR) {
                cout << "Send failed, error: " << result << endl;
                closesocket(ConnectSocket);
                freeaddrinfo(addrResult);
                WSACleanup();
                return 1;
            }
        }
        else if (result == 0) {
            cout << "Connection closing" << endl;//закрытие соединения
        }
        else {
            cout << "Recv failed, error: " << WSAGetLastError() << endl;//ошибкка
            closesocket(ConnectSocket);
            freeaddrinfo(addrResult);
            WSACleanup();
            return 1;
        }
    } while (result > 0);

    result = shutdown(ConnectSocket, SD_SEND);//закрываем соединение с клиентом
    if (result == SOCKET_ERROR) {
        cout << "Shutdown failed, error: " << result << endl;//проверка
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    closesocket(ConnectSocket);//закрытие сокета
    freeaddrinfo(addrResult);//освобождение ресурсов
    WSACleanup();//завершение программы
    return 0;
}
