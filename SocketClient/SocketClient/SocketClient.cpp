#define WIN32_LEAN_AND_MEAN

#include <Windows.h> // Windows API
#include <iostream>
#include <WinSock2.h> //библиотека для работы с сокетами
#include <WS2tcpip.h> // библиотека для работы с протоколом TCP/IP

using namespace std;

int main() {
    WSADATA wsaData; //объявляем переменные
    ADDRINFO hints;
    ADDRINFO* addrResult;
    SOCKET ConnectSocket = INVALID_SOCKET; //сокет
    char recvBuffer[512]; //переменная для хранения данных от сервера

    const char* sendBuffer1 = "Hello from client 1";
    const char* sendBuffer2 = "Hello from client 2";

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData); //инициализация библиотеки
    if (result != 0) {
        cout << "WSAStartup failed with result: " << result << endl; //проверка
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints)); //очищение hints, данные для создания сокета
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP; 

    result = getaddrinfo("localhost", "666", &hints, &addrResult);//получение данных о сервере по имени и порту
    if (result != 0) {
        cout << "getaddrinfo failed with error: " << result << endl;//проверка
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol); //сокет с параметрами, которые мы узнаем из addResult
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Socket creation failed" << endl;//проверка
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);//подключение к серверу
    if (result == SOCKET_ERROR) {
        cout << "Unable to connect to server" << endl;//проверка
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    result = send(ConnectSocket, sendBuffer1, (int)strlen(sendBuffer1), 0);//отправляем данные на сервер(сокет, буффер, длина сообщения)
    if (result == SOCKET_ERROR) {
        cout << "Send failed, error: " << result << endl;//проверка
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }
    cout << "Sent: " << result << " bytes" << endl;

    result = send(ConnectSocket, sendBuffer2, (int)strlen(sendBuffer2), 0);//отправляем данные на сервер(сокет, буффер, длина сообщения)
    if (result == SOCKET_ERROR) {
        cout << "Send failed, error: " << result << endl;//проверка
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }
    cout << "Sent: " << result << " bytes" << endl;

    result = shutdown(ConnectSocket, SD_SEND);//закрываем отправку на сокете, оставляем прием
    if (result == SOCKET_ERROR) {
        cout << "Shutdown failed, error: " << result << endl;//проверка
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    do {
        ZeroMemory(recvBuffer, 512);//обнуляем буффер; цикл для приема данных от сервера, он будет работать, пока соединение не будет закрыто, либо пока не произойдет ошибка
        result = recv(ConnectSocket, recvBuffer, 512, 0);
        if (result > 0) {
            cout << "Received " << result << " bytes" << endl;
            cout << "Received data: " << recvBuffer << endl;
        }
        else if (result == 0) {
            cout << "Connection closed" << endl;//соединение закрыто
        }
        else {
            cout << "Recv failed, error: " << WSAGetLastError() << endl;//ошибка
        }
    } while (result > 0);

    closesocket(ConnectSocket);//закрытие сокета
    freeaddrinfo(addrResult);//освобождение ресурсов
    WSACleanup();//завершение программы
    return 0;
}
