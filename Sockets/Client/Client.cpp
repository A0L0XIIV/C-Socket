/*Baran Kaya*/

/**************************
* Basic Windows socket	  *
* Server - Client		  *
* Local IP				  *
**************************/

//#include "stdafx.h"

#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <cstdlib>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>		// Thread
#include <algorithm>	// Transform space to underscore
#include <string>		// Getline

//WinSock Documentation
//https://docs.microsoft.com/en-us/windows/desktop/winsock

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

//Port to run
#define DEFAULT_PORT "27015"

//Data buffer length
#define DEFAULT_BUFLEN 512

using namespace std;

struct addrinfo *result = NULL, 
				*ptr = NULL, 
				hints;

// Recieve thread
void recieveDataFromServer(SOCKET ServerSocket);
void sendDataToServer(SOCKET ServerSocket);

int main(int argc, char *argv[])
{
	WSADATA wsaData;

	// Result of the steps & for checking the errors.
	int iResult;

	// SOCKET object for the client to connect to server
	SOCKET ConnectSocket = INVALID_SOCKET;

	// Validate the parameters
	/*if (argc != 2) {
		cout << "usage: " << argv[0] << " server-name" << endl;
		return 1;
	}*/

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

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
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

		// 2. Connect to server with socket
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		else {
			cout << "Server Connection\t\tSUCCESS" << endl;
			break;
		}
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
	char sendbuf[DEFAULT_BUFLEN] = {};
	//char* sendbuf = nullptr;

	// Create a new thread for reciving messages from the server
	thread* first = new thread(recieveDataFromServer, ConnectSocket);
	//first->detach();
	thread* second = new thread(sendDataToServer, ConnectSocket);
	//second->detach();
	first->join();
	//second->join();

	/*string input;

	while (input != "0") {
		cout << "Enter 0 for exiting. Message: ";
		cin >> skipws >> input;
		if (input == "0") break;
		//memset(&(sendbuf[0]), 0, DEFAULT_BUFLEN);
		strcpy_s(sendbuf, input.c_str());
		//sendbuf = &input[0];

		// Send an initial buffer
		iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
		if (iResult == SOCKET_ERROR) {
			cout << "Send \t\t\t\t\tFAILED: " << WSAGetLastError() << endl;
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}
		// Print sent bytes
		cout << "Bytes Sent: " << iResult << endl;

		// Recieve sent message back
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			cout << "Bytes received: " << iResult << endl << "Recieved Message: ";
			for (int i = 0; i < iResult; ++i)
				cout << recvbuf[i];
			cout << endl << flush;
		}
	}

	// shutdown the connection for sending since no more data will be sent
	// 4. the client can still use the ConnectSocket for receiving data
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

	// 5. Receive data until the server closes the connection
	do {
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
			cout << "Bytes received: " << iResult << endl;
		else if (iResult == 0) {
			cout << "Server closed!" << endl;
		}
		else
			cout << "Recieve\t\t\t\tFAILED: " << WSAGetLastError() << endl;
	} while (iResult > 0);


	// 6. Disconnect the server-shutdown the connection
	cout << "Connection closing..." << endl;
	Sleep(1000);
	cout << "3" << endl << flush;
	Sleep(1000);
	cout << "2" << endl << flush;
	Sleep(1000);
	cout << "1" << endl << flush;
	Sleep(1000);
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

	// 7. Cleanup
	closesocket(ConnectSocket);
	WSACleanup();

	return 0;*/
}

void recieveDataFromServer(SOCKET ServerSocket) {
	// Result of the steps & for checking the errors.
	int iResult;
	// 3. Recieve&Send data
	int recvbuflen = DEFAULT_BUFLEN;
	char recvbuf[DEFAULT_BUFLEN];
	char sendbuf[DEFAULT_BUFLEN] = {};

	// 5. Receive data until the server closes the connection
	do {
		// Sleep 100ms
		Sleep(100);
		// Recieve messages
		iResult = recv(ServerSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			//cout << iResult << " bytes received from " << ServerSocket << ", message: ";
			cout << ServerSocket << ": ";
			for (int i = 0; i < iResult; ++i) {
				recvbuf[i] != '_' ? cout << recvbuf[i] : cout << ' ';
			}
			cout << endl << flush;
		}
		else if (iResult == 0) {
			cout << "Server closed!" << endl;
		}
		else
			cout << "Recieve\t\t\t\tFAILED: " << WSAGetLastError() << endl;
	} while (iResult > 0 && ServerSocket != INVALID_SOCKET);

	return;
}

void sendDataToServer(SOCKET ServerSocket) {
	// Result of the steps & for checking the errors.
	int iResult;
	// 3. Recieve&Send data
	int recvbuflen = DEFAULT_BUFLEN;
	char sendbuf[DEFAULT_BUFLEN] = {};

	string input;
	cout << "Enter 0 for exiting. Message: " << endl << flush;

	while (input != "0") {
		//cin >> skipws >> input;
		std::getline(std::cin, input);

		if (input == "0") break;

		// Replace spaces with underscore
		transform(input.begin(), input.end(), input.begin(), [](char ch) {
			return ch == ' ' ? '_' : ch;
		});

		// String to char array
		strcpy_s(sendbuf, input.c_str());

		// Send an initial buffer
		iResult = send(ServerSocket, sendbuf, (int)strlen(sendbuf), 0);
		if (iResult == SOCKET_ERROR) {
			cout << "Send \t\t\t\t\tFAILED: " << WSAGetLastError() << endl;
			closesocket(ServerSocket);
			WSACleanup();
			return;
		}
		// Print sent bytes
		cout << "Bytes Sent: " << iResult << endl;
	}

	// shutdown the connection for sending since no more data will be sent
	// 4. the client can still use the ConnectSocket for receiving data
	iResult = shutdown(ServerSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		cout << "Send Shutdown\t\t\tFAILED: " << WSAGetLastError() << endl;
		closesocket(ServerSocket);
		WSACleanup();
		return;
	}
	else {
		cout << "Send Shutdown\t\t\tSUCCESS" << endl;
	}


	// 6. Disconnect the server-shutdown the connection
	cout << "Connection closing..." << endl;
	Sleep(1000);
	cout << "3" << endl << flush;
	Sleep(1000);
	cout << "2" << endl << flush;
	Sleep(1000);
	cout << "1" << endl << flush;
	Sleep(1000);
	iResult = shutdown(ServerSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		cout << "Shutdown\t\t\t\tFAILED: " << WSAGetLastError() << endl;
		closesocket(ServerSocket);
		WSACleanup();
		return;
	}
	else {
		cout << "Shutdown\t\t\t\tSUCCESS" << endl;
	}

	// 7. Cleanup
	closesocket(ServerSocket);
	WSACleanup();

	return;
}