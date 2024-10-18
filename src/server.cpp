#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

int main()
{
    const char IP_SERV[] = "127.0.0.1";
    const int PORT_NUM = 1234;
    const short BUFF_SIZE = 1024;

    int erStat;

    // IP in string format to numeric format
    in_addr ip_to_num;
    erStat = inet_pton(AF_INET, IP_SERV, &ip_to_num);

    if (erStat <= 0) {

        cout <<
            "Error in IP translation to special numeric format"
        << endl;

        return 1;
    }

    // WinSock initialization
    WSADATA wsData;

    erStat = WSAStartup(MAKEWORD(2, 2), &wsData);

    if (erStat != 0) {

        cout << "Error WinSock version initialization # ";
        cout << WSAGetLastError() << endl;
        return 1;
    }
    else {
        cout << "WinSock initialization is OK" << endl;
    }

    // Server socket initialization
    SOCKET ServSock = socket(AF_INET, SOCK_STREAM, 0);

    if (ServSock == INVALID_SOCKET) {

        cout << "Error initialization socket # "
        << WSAGetLastError() << endl;

        closesocket(ServSock);
        WSACleanup();
        return 1;
    }
    else {

        cout << "Server socket initialization is OK\n";
    }

    // Server socket binding
    sockaddr_in servInfo;
    ZeroMemory(&servInfo, sizeof(servInfo));

    servInfo.sin_family = AF_INET;
    servInfo.sin_addr = ip_to_num;
    servInfo.sin_port = htons(1234);

    erStat = bind(ServSock, (sockaddr*)&servInfo, sizeof(servInfo));

    if (erStat != 0) {

        cout << "Error Socket binding to server info. Error # "
        << WSAGetLastError() << endl;

        closesocket(ServSock);
        WSACleanup();
        return 1;
    }
    else {

        cout << "Binding socket to server info is OK" << endl;
    }

    // Starting to listen to any Clients
    erStat = listen(ServSock, SOMAXCONN);

    if (erStat != 0) {

        cout <<
        "Can't start to listen to. Error # "
        << WSAGetLastError() << endl;

        closesocket(ServSock);
        WSACleanup();
        return 1;
    }
    else {

        cout << "Listening ..." << endl;
    }

    // Client socket creation and acception in case of connection
    sockaddr_in clientInfo;
    ZeroMemory(&clientInfo, sizeof(clientInfo));

    int clientInfo_size = sizeof(clientInfo);

    SOCKET ClientConn = accept(ServSock, (sockaddr*)&clientInfo, &clientInfo_size);

    if (ClientConn == INVALID_SOCKET) {

        cout << "Client detected, but can't connect to a client. Error # "
        << WSAGetLastError() << endl;

        closesocket(ServSock);
        closesocket(ClientConn);
        WSACleanup();
        return 1;
    }
    else {

        cout << "Connection to a client established successfully\n";

        char clientIP[22];

        inet_ntop(AF_INET, &clientInfo.sin_addr, clientIP, INET_ADDRSTRLEN);

        cout <<
        "Client connected with IP addres"
        << clientIP << endl;
    }

    // Exchange text data between Server and Client
    vector<char> servBuff(BUFF_SIZE), clientBuff(BUFF_SIZE);
    short packet_size = 0;

    while (true) {

        packet_size = recv(ClientConn, servBuff.data(), servBuff.size(), 0);
        cout << "Client's message: " << servBuff.data() << endl;

        cout << "Your (host) message: ";
        fgets(clientBuff.data(), clientBuff.size(), stdin);

        // Disconnection if client send "xxx"
        if (clientBuff[0] == 'x' && clientBuff[1] == 'x' && clientBuff[2] == 'x') {

            shutdown(ClientConn, SD_BOTH);
            closesocket(ServSock);
            closesocket(ClientConn);
            WSACleanup();
            return 0;
        }

        packet_size = send(ClientConn, clientBuff.data(), clientBuff.size(), 0);

        if (packet_size == SOCKET_ERROR) {

            cout <<
            "Can't send message to Client. Error # "
            << WSAGetLastError() << std::endl;

            closesocket(ServSock);
            closesocket(ClientConn);
            WSACleanup();
            return 1;
        }
    }

    closesocket(ServSock);
    closesocket(ClientConn);
    WSACleanup();

    return 0;
}
