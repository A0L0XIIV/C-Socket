/*Baran Kaya*/

/**************************
* Basic socket program	  *
* Server - Client		  *
* Local IP				  *
**************************/

#include "stdafx.h"

#include <iostream>
#include <cstdlib>
#include <winsock2.h>
#include <ws2tcpip.h>

//WinSock Documentation
//https://docs.microsoft.com/en-us/windows/desktop/winsock

//The WSAStartup function is called to initiate use of WS2_32.dll.
#pragma comment(lib, "Ws2_32.lib")

//Port to run
#define DEFAULT_PORT "27015"

//Data buffer length
#define DEFAULT_BUFLEN 512

using namespace std;

struct addrinfo *result = NULL, 
				*ptr = NULL, 
				hints;

int main(int argc, char *argv[])
{
	WSADATA wsaData;

	// Result of the steps & for checking the errors.
	int iResult;

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


	// 1. Create a socket for client
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the local address and port to be used by the client
	iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		cout << "Getaddrinfo\t\t\tFAILED: " << iResult << endl;
		WSACleanup();
		return 1;
	}

	// SOCKET object for the client to connect to server
	SOCKET ConnectSocket = INVALID_SOCKET;

	ptr = result;

	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	if (ConnectSocket == INVALID_SOCKET) {
		cout << "Connect Socket\t\tFAILED: " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
	else {
		cout << "ListenSocket\t\t\tSUCCESS" << endl;
	}



	// 2. Connect to a socket

	// Connect to server
	iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}
	else {
		cout << "Server Connection\t\t\tSUCCESS" << endl;
	}

	// Should really try the next address returned by getaddrinfo
	// if the connect call failed
	// But for this simple example we just free the resources
	// returned by getaddrinfo and print an error message

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		cout << "Server Connection\t\t\tFAILED" << endl;
		WSACleanup();
		return 1;
	}


	// 3. Recieve&Send data
	int recvbuflen = DEFAULT_BUFLEN;
	char recvbuf[DEFAULT_BUFLEN];
	char sendbuf[15] = "this is a test";
	//char *sendbuf = "this is a test";

	// Send an initial buffer
	iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		cout << "Send \t\t\t\t\tFAILED: " << WSAGetLastError() << endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	cout << "Bytes Sent: " << iResult << endl;

	// shutdown the connection for sending since no more data will be sent
	// the client can still use the ConnectSocket for receiving data
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		cout << "Send Shutdown\t\t\tFAILED: " << WSAGetLastError() << endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}
	else {
		cout << "Send Shutdown\t\t\tSUCCESS" << endl;
	}

	// Receive data until the server closes the connection
	do {
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
			cout << "Bytes received: " << iResult << endl;
		else if (iResult == 0)
			cout << "Connection closed" << endl;
		else
			cout << "Recieve\t\t\t\tFAILED: " << WSAGetLastError() << endl;
	} while (iResult > 0);


	// 6. Disconnect the server-shutdown the connection
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		cout << "Shutdown\t\t\t\tFAILED: " << WSAGetLastError() << endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}
	else {
		cout << "Shutdown\t\t\t\tSUCCESS" << endl;
	}

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();


	return 0;
}

