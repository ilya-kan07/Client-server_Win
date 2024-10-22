#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <vector>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

const char SERVER_IP[] = "127.0.0.1";
const short SERVER_PORT_NUM = 1234;
const short BUFF_SIZE = 1024;

void receiveMessages(SOCKET ClientSock) {

    vector <char> buffer(BUFF_SIZE);
    short packet_size = 0;

    while (true) {

        packet_size = recv(ClientSock, buffer.data(), buffer.size() - 1, 0);

        if (packet_size == SOCKET_ERROR || packet_size == 0) {

            cout << "Connection closed or error receiving message." << endl;
            break;
        }

        buffer[packet_size] = '\0';
        cout << "\nMessage from Server: " << buffer.data() << endl;
    }

    closesocket(ClientSock);
    WSACleanup();
    exit(0);
}

int main () {

    int erStat;

    in_addr ip_to_num;
    erStat = inet_pton(AF_INET, SERVER_IP, &ip_to_num);

    if (erStat <= 0) {

        cout << "Error in IP translation to special numeric format\n";
        return 1;
    }

    //WinSock initialization
    WSADATA wsData;

    erStat = WSAStartup(MAKEWORD(2, 2), &wsData);

    if (erStat != 0) {

        cout << "Error WinSock version initializaion #";
        cout << WSAGetLastError() << endl;
        return 1;
    }
    else {

        cout << "WinSock initializaion is OK" << endl;
    }

    //Socket initialization
    SOCKET ClientSock = socket(AF_INET, SOCK_STREAM, 0);

    if (ClientSock == INVALID_SOCKET) {

        cout << "Error initialization socket #";
        cout << WSAGetLastError() << endl;
        closesocket(ClientSock);
        WSACleanup();
    }
    else {

        cout << "Client socket initialization is OK" << endl;
    }

    //Establishing a connection to Server
    sockaddr_in servInfo;

    ZeroMemory(&servInfo, sizeof(servInfo));

    servInfo.sin_family = AF_INET;
    servInfo.sin_addr = ip_to_num;
    servInfo.sin_port = htons(SERVER_PORT_NUM);

    erStat = connect(ClientSock, (sockaddr*)&servInfo, sizeof(servInfo));

    if (erStat != 0) {

        cout << "Connection to server is FALIED. Error #";
        cout << WSAGetLastError() << endl;
        closesocket(ClientSock);
        WSACleanup();
        return 1;
    }
    else {

        cout << "Connection established SUCCESSFULY. ";
        cout << "Ready to send a message to Server" << endl;
    }

    // Starting a separate thread to receive messages from the server
    thread receiveThread(receiveMessages, ClientSock);
    receiveThread.detach();

    // Entering and sending messages to the server
    vector<char> clientBuff(BUFF_SIZE);
    short packet_size = 0;

    while (true) {

        cout << "Your message to Server: ";
        fgets(clientBuff.data(), clientBuff.size(), stdin);

        // Disconnect if Client send "xxx"
        if (clientBuff[0] == 'x' && clientBuff[1] == 'x' && clientBuff[2] == 'x') {
            shutdown(ClientSock, SD_BOTH);
            closesocket(ClientSock);
            WSACleanup();
            return 0;
        }

        packet_size = send(ClientSock, clientBuff.data(), strlen(clientBuff.data()), 0);

        if (packet_size == SOCKET_ERROR) {

            cout << "Can't receive message from Server. Error #";
            cout << WSAGetLastError() << endl;
            closesocket(ClientSock);
            WSACleanup();
            return 1;
        }
    }

    closesocket(ClientSock);
    WSACleanup();
    return 0;
}
