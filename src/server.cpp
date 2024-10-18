#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

int main()
{
    const char IP_SERV[] = "";
    const int PORT_NUM = 0;
    const short BUFF_SIZE = 1024;

    int erStat;

    in_addr ip_to_num;
    erStat = inet_pton(AF_INET, IP_SERV, &ip_to_num);

    if (erStat <= 0) {

        std::cout <<
        "Error in IP translation to special numeric format"
        << std:: endl;

        return 1;
    }

    WSADATA wsData;

    int erStat = WSAStartup(MAKEWORD(2, 2), &wsData);

    if (erStat != 0) {

        std::cout << "Error WinSock version initialization # ";
        std::cout << WSAGetLastError();
        return 1;
    }
    else {
        std::cout << "WinSock initialization is OK" << std::endl;
    }

    SOCKET ServSock = socket(AF_INET, SOCK_STREAM, 0);

    if (ServSock == INVALID_SOCKET) {

        std::cout << "Error initialization socket # "
        << WSAGetLastError() << std::endl;

        closesocket(ServSock);
        WSACleanup();
        return 1;
    }
    else {

        std::cout << "Server socket initialization is OK\n";
    }

    sockaddr_in servInfo;
    ZeroMemory(&servInfo, sizeof(servInfo));

    servInfo.sin_family = AF_INET;
    servInfo.sin_addr = ip_to_num;
    servInfo.sin_port = htons(1234);

    erStat = bind(ServSock, (sockaddr*)&servInfo, sizeof(servInfo));

    if (erStat != 0) {

        std::cout <<
        "Error Socket binding to server info. Error # " <<
        WSAGetLastError() << std::endl;

        closesocket(ServSock);
        WSACleanup();
        return 1;
    }
    else {

        std::cout <<
        "Binding socket to server info is OK" << std::endl;
    }

    erStat = listen(ServSock, SOMAXCONN);

    if (erStat != 0) {

        std::cout <<
        "Can't start to listen to. Error # " <<
        WSAGetLastError() << std::endl;

        closesocket(ServSock);
        WSACleanup();
        return 1;
    }
    else {

        std::cout << "Listening ..." << std::endl;
    }

    sockaddr_in clientInfo;
    ZeroMemory(&clientInfo, sizeof(clientInfo));

    int clientInfo_size = sizeof(clientInfo);

    SOCKET ClientConn = accept(ServSock, (sockaddr*)&clientInfo, &clientInfo_size);

    if (ClientConn == INVALID_SOCKET) {

        std::cout <<
        "Client detected, but can't connect to a client. Error # "
        << WSAGetLastError() << std::endl;

        closesocket(ServSock);
        closesocket(ClientConn);
        WSACleanup();
        return 1;
    }
    else {

        std::cout <<
        "Connection to a client established successfully"
        << std::endl;

        char clientIP[22];

        inet_ntop(AF_INET, &clientInfo.sin_addr, clientIP, INET_ADDRSTRLEN);

        std::cout <<
        "Client connected with IP addres" <<
        clientIP << std::endl;
    }

    std::vector<char> servBuff(BUFF_SIZE), clientBuff(BUFF_SIZE);
    short packet_size = 0;

    while (true) {

        packet_size = recv(ClientConn, servBuff.data(), servBuff.size(), 0);
        std::cout << "Client's message: " << servBuff.data() << std::endl;

        std::cout << "Your (host) message: ";
        fgets(clientBuff.data(), clientBuff.size(), stdin);

        if (clientBuff[0] == 'x' && clientBuff[1] == 'x' && clientBuff[2] == 'x') {

            shutdown(ClientConn, SD_BOTH);
            closesocket(ServSock);
            closesocket(ClientConn);
            WSACleanup();
            return 0;
        }

        packet_size = send(ClientConn, clientBuff.data(), clientBuff.size(), 0);

        if (packet_size == SOCKET_ERROR) {

            std::cout <<
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
