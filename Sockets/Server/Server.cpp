/*Baran Kaya*/

/**************************
* Basic Windows socket    *
* Server - Client		  *
* Local IP				  *
**************************/

//#include "stdafx.h"

#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <cstdlib>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "Map.h"
#include "WaitingRoom.h"
#include "Player.h"

// WinSock Documentation
// https://docs.microsoft.com/en-us/windows/desktop/winsock

// The WSAStartup function is called to initiate use of WS2_32.dll.
#pragma comment(lib, "Ws2_32.lib")

// Port to run
#define DEFAULT_PORT "27015"

// Data buffer length
#define DEFAULT_BUFLEN 512

using namespace std;

struct addrinfo *result = NULL;
struct addrinfo hints;

int main()
{
	WSADATA wsaData;

	// Result of the steps & for checking the errors.
	int iResult;

	// SOCKET object for the server to listen&send for client connections
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	// Recieve variables
	char recvbuf[DEFAULT_BUFLEN];
	int iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;
	
	// 0. Initialize Winsock
	try {
		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	}
	catch (exception& e) {
		cout << "WSAStartup exception: " << e.what() << endl;
	}
	if (iResult != 0) {
		cout << "WSAStartup\t\t\tFAILED: " << iResult << endl;
		return 1;
	}
	else {
		cout << "WSAStartup\t\t\tSUCCESS" << endl;
	}


	// 1. Create a socket for server
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		cout << "Getaddrinfo\t\t\tFAILED: " << iResult << endl;
		WSACleanup();
		return 1;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		cout << "Listen Socket\t\tFAILED: " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
	else {
		cout << "ListenSocket\t\t\tSUCCESS" << endl;
	}


	// 2. Bind a socket - Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		cout << "Bind\t\t\t\tFAILED: " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	else {
		cout << "Bind\t\t\t\tSUCCESS" << endl;
	}

	// addr. info returned by the getaddrinfo function is no longer needed
	freeaddrinfo(result);

	// 3. Listen on socket
	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		cout << "Listen\t\t\t\tSUCCESS: " << WSAGetLastError() << endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	else {
		cout << "Listen\t\t\t\tSUCCESS" << endl;
	}


	// 4. Accept connection - Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		cout << "Accept Client\t\t\tFAILED" << WSAGetLastError() << endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	else {
		cout << "Accept Client\t\t\tSUCCESS" << endl;
		Player *player1 = new Player("Client 1", 'O');
	}


	// 5. Recieve&Send data - Receive until the peer shuts down the connection
	do {
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			cout << "Bytes received: " << iResult << endl << "Recieved Message: ";
			for(int i = 0; i < iResult; ++i)
				cout << recvbuf[i];
			cout << endl << flush;

			// Echo the buffer back to the sender
			iSendResult = send(ClientSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				cout << "Send failed: " << WSAGetLastError() << endl;
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			printf("Bytes sent: %d\n", iSendResult);
		}
		else if (iResult == 0) {
			cout << "Connection closing..." << endl;
			Sleep(1000);
			cout << "3" << endl << flush;
			Sleep(1000);
			cout << "2" << endl << flush;
			Sleep(1000);
			cout << "1" << endl << flush;
			Sleep(1000);
		}
		else {
			cout << "Recieve failed: " << WSAGetLastError() << endl;
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}
	} while (iResult > 0);


	// 6. Disconnect the server-shutdown the connection
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		cout << "Shutdown\t\t\tFAILED" << WSAGetLastError() << endl;
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}
	else {
		cout << "Shutdown\t\t\tSUCCESS" << endl;
	}

	// 7. Cleanup
	closesocket(ClientSocket);
	WSACleanup();

    return 0;
}

