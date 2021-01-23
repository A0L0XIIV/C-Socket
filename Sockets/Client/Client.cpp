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

// Recieve and send threads
void recieveDataFromServer(SOCKET ServerSocket, atomic<bool>& isSocketOpen);
void sendDataToServer(SOCKET ServerSocket, atomic<bool>& isSocketOpen);

int main(int argc, char *argv[]) {
	// PLAYER SELECTION OPERATIONS
	string name = "";
	string symbol = "";
	string connectNowChar = "";
	bool connectNow = false;

	cout << "Welcome to the X." << endl;
	cout << "Please enter your name: ";
	getline(std::cin, name);
	cout << "Please enter a character for your player (Ex: O, X): ";
	getline(std::cin, symbol);
	while (!connectNow) {
		cout << "Do you want to connect to the server now? (y/n): ";
		cin >> connectNowChar;
		if (connectNowChar == "y" || connectNowChar == "Y")
			connectNow = true;
		else if (connectNowChar == "n" || connectNowChar == "N")
			return 0;
	}

	// WINSOCK OPERATIONS
	WSADATA wsaData;

	// Result of the steps & for checking the errors.
	int iResult;

	// SOCKET object for the client to connect to server
	SOCKET ConnectSocket = INVALID_SOCKET;


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


	// 3. Recieve & Send data
	char recvbuf[DEFAULT_BUFLEN] = {};
	char sendbuf[DEFAULT_BUFLEN] = {};

	// Set player name
	string setPlayerName = "!PlayerName=" + name + "!PlayerSymbol=" + symbol;
	// String to char array
	strcpy_s(sendbuf, setPlayerName.c_str());
	// Send an set player name buffer
	iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		cout << "Set Player Name\t\t\tFAILED: " << WSAGetLastError() << endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// Check if the palyer name set was succesfull
	string checkPlayerSetMsg = "!PlayerSetSuccess";
	// Recieve set player success message from the server
	iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
	if (iResult > 0 && recvbuf == checkPlayerSetMsg) {
		cout << "Set Player Name\t\t\tSUCCESS" << endl << flush;
	}
	else {
		cout << "Set Player Name\t\t\tFAILED: " << recvbuf << endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// Atomic variables for checking socket's status
	atomic<bool> isSocketOpen(true);
	// Create a new thread for reciving messages from the server
	thread* read = new thread(recieveDataFromServer, ConnectSocket, ref(isSocketOpen));
	// Create a new thread for sending messages to the server
	thread* write = new thread(sendDataToServer, ConnectSocket, ref(isSocketOpen));

	//read->detach();
	write->detach();
	read->join();
	//write->join();

	return 0;
}

void recieveDataFromServer(SOCKET ServerSocket, atomic<bool>& isSocketOpen) {
	// Result of the steps & for checking the errors.
	int iResult;
	// 3. Recieve data
	char recvbuf[DEFAULT_BUFLEN] = {};

	// 5. Receive data until the server closes the connection
	do {
		// Sleep 100ms
		Sleep(100);
		// Recieve messages
		iResult = recv(ServerSocket, recvbuf, DEFAULT_BUFLEN, 0);
		if (iResult > 0) {
			//cout << iResult << " bytes received from " << ServerSocket << ", message: ";
			for (int i = 0; i < iResult; ++i) {
				recvbuf[i] != '_' ? cout << recvbuf[i] : cout << ' ';
			}
			cout << endl << flush;
		}
		else if (iResult == 0) {
			cout << "Connection closed!" << endl;
		}
		else if (!isSocketOpen) {
			break;
		}
		else
			cout << "Recieve\t\t\tFAILED: " << WSAGetLastError() << endl;
	} while (isSocketOpen && iResult > 0);

	return;
}


void sendDataToServer(SOCKET ServerSocket, atomic<bool>& isSocketOpen) {
	// Result of the steps & for checking the errors.
	int iResult;
	// 3. Send data
	char sendbuf[DEFAULT_BUFLEN] = {};

	string input;
	cout << "Enter 0 for exiting. Enter your message: " << endl << flush;

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
			cout << "Send\t\t\t\t\tFAILED: " << WSAGetLastError() << endl;
			closesocket(ServerSocket);
			WSACleanup();
			return;
		}
		// Print sent bytes
		cout << "Bytes Sent: " << iResult << endl;
	}

	// 6. Disconnect the server-shutdown the connection
	cout << "Connection closing..." << flush;
	Sleep(1000);
	cout << "\b\b\b in 3" << flush;
	Sleep(1000);
	cout << "\b2" << flush;
	Sleep(1000);
	cout << "\b1" << flush;
	Sleep(1000);
	cout << "\b0" << endl << flush;

	iResult = shutdown(ServerSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		cout << "Shutdown\t\t\tFAILED: " << WSAGetLastError() << endl;
		closesocket(ServerSocket);
		WSACleanup();
		return;
	}
	else {
		cout << "Shutdown\t\t\tSUCCESS" << endl;
	}

	// Update socket status so that read thread can finish
	isSocketOpen = false;

	// 7. Cleanup
	closesocket(ServerSocket);
	WSACleanup();

	return;
}