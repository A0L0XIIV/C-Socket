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
#include <thread>	// For threads
#include <list>		// Linked list

#include "Map.h"
#include "WaitingRoom.h"
#include "Player.h"

// WinSock Documentation
// https://docs.microsoft.com/en-us/windows/desktop/winsock

// The WSAStartup function is called to initiate use of WS2_32.dll.
#pragma comment(lib, "Ws2_32.lib")

using namespace std;

struct addrinfo *result = NULL;
struct addrinfo hints;

void ClientAccept(const SOCKET& ListenSocket, list<pair<SOCKET, Player*>>& ClientSocketsList, static char* st_recvbuf);
void ClientHandle(SOCKET ClientSocket, list<pair<SOCKET, Player*>>& ClientSocketsList, static char * st_recvbuf);

// Compares Client Socket Linked List's first pair (SOCKET) to given SOCKET
struct socketPairCompare {
	socketPairCompare(SOCKET const& s) : _s(s) { }

	bool operator () (pair<SOCKET, Player*> const& p)
	{
		return (p.first == _s);
	}

	SOCKET _s;
};

int main()
{
	WSADATA wsaData;

	// Result of the steps & for checking the errors.
	int iResult = 0;
	// Port to run
	const char* DEFAULT_PORT = "27015";
	// Data buffer length
	const size_t DEFAULT_BUFLEN = 512;
	// Max client number
	const size_t MAX_CLIENT = 10;
	size_t clientCounter = 0;

	// SOCKET object for the server to listen&send for client connections
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET NewClientSocket = INVALID_SOCKET;
	list<pair<SOCKET, Player*>> ClientSocketsList;

	// Recieve variables
	static char st_recvbuf[DEFAULT_BUFLEN] = {};
	int iSendResult = 0;
	
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
		cout << "Listen\t\t\t\tFAILED: " << WSAGetLastError() << endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	else {
		cout << "Listen\t\t\t\tSUCCESS" << endl;
	}


	// 4: 5, 6. Create and execute the a new thread for each client
	thread clientAcceptThread(ClientAccept, std::ref(ListenSocket), std::ref(ClientSocketsList), st_recvbuf);
	// Without detach, recv function doesn't work in threads
	clientAcceptThread.detach();

	string input;
	while (input != "0") {
		getline(std::cin, input);
	};

	// Send shutdown to all clients and close all sockets
	for (pair<SOCKET, Player*> client : ClientSocketsList) {
		// 6. Disconnect the server-shutdown the connection
		iResult = shutdown(client.first, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			cout << "Shutdown " << client.first << "\t\tFAILED" << WSAGetLastError() << endl;
			closesocket(client.first);
			WSACleanup();
			return 1;
		}
		else {
			cout << "Shutdown " << client.first << "\t\tSUCCESS" << endl;
		}

		// Close socket
		closesocket(client.first);
	}
	// 7. Cleanup
	WSACleanup();

    return 0;
}

// Accept Clients thread function
void ClientAccept(const SOCKET& ListenSocket, list<pair<SOCKET, Player*>>& ClientSocketsList, static char * st_recvbuf) {
	SOCKET NewClientSocket = 0;
	size_t clientCounter = 0;
	const size_t MAX_CLIENT = 10;

	sockaddr_in from;
	int fromlen = sizeof(from);

	// Accept new clients in the infinite loop
	while (1) {
		NewClientSocket = accept(ListenSocket, (struct sockaddr*)&from, &fromlen);
		// New client arrived
		if (NewClientSocket != INVALID_SOCKET && clientCounter <= MAX_CLIENT) {
			// Create and execute the a new thread for each client
			thread clientHandleThread (ClientHandle, NewClientSocket, std::ref(ClientSocketsList), st_recvbuf);
			// Without detach, recv function doesn't work in threads
			clientHandleThread.detach();
			// Increase counter
			++clientCounter;
		}
		// No new clients or max number of clients: do nothing
		else {
			// Update client counter, some clients might left the server
			clientCounter = ClientSocketsList.size();
			// Sleep for a second
			Sleep(250);
			cout << "\rWaiting for new clients\t\t" << flush;
			cout << "\b\\" << flush;
			Sleep(250);
			cout << "\rWaiting for new clients.\t" << flush;
			cout << "\b|" << flush;
			Sleep(250);
			cout << "\rWaiting for new clients..\t" << flush;
			cout << "\b/" << flush;
			Sleep(250);
			cout << "\rWaiting for new clients...\t" << flush;
			cout << "\b-" << flush;
			// Try again
			continue;
		}
	}
	return;
}


// Individual Client Handle thread function
void ClientHandle(SOCKET ClientSocket, list<pair<SOCKET, Player*>>& ClientSocketsList, static char* st_recvbuf) {
	const size_t DEFAULT_BUFLEN = 512;
	// Recieve variables
	int iRecvResult = 0;
	char sendbuf[DEFAULT_BUFLEN] = {};
	// Send variables
	int iSendResult = 0;
	char recvbuf[DEFAULT_BUFLEN] = {};
	// Create a new player object
	string playerName = "S_" + to_string(ClientSocket);
	Player* clientPlayer = new Player(playerName, 'O');
	

	// 4. Accept connection - Accept a client socket
	if (ClientSocket == INVALID_SOCKET) {
		cout << "Accept Client " << ClientSocket << "\t\tFAILED" << WSAGetLastError() << endl;
		closesocket(ClientSocket);
		WSACleanup();
		return;
	}
	else {
		cout << "Accept Client " << ClientSocket << "\t\tSUCCESS" << endl;
		// Add Socket in the client sockets list
		ClientSocketsList.push_back(pair<SOCKET, Player*>(ClientSocket, clientPlayer));
	}

	
	// 5. Recieve & Send data - Receive until the peer shuts down the connection
	do {
		// Clear the recieve buffer everytime
		memset(&recvbuf[0], 0, sizeof(recvbuf));
		// Get messages from the client socket
		iRecvResult = recv(ClientSocket, recvbuf, DEFAULT_BUFLEN, 0);

		// Message recieved from the client
		if (iRecvResult > 0) {
			// Print message to server console
			cout << iRecvResult << " bytes received from (" << ClientSocket << ") - " << playerName << ", message: ";
			for (int i = 0; i < iRecvResult; ++i)
				cout << recvbuf[i];
			cout << endl << flush;

			// Player set operation (First message from the client
			if (recvbuf[0] == '!') {
				// Char array to string
				string msg(recvbuf);
				// Get player name and symbol
				size_t playerNamePos = msg.find("!PlayerName=");
				size_t playerSymbolPos = msg.find("!PlayerSymbol=");
				string setPlayerSetMsg = "";

				// Check if message is valid or not
				if (playerNamePos != string::npos && playerSymbolPos != string::npos) {
					// Set Player name
					string playerName = msg.substr(playerNamePos + 12, playerSymbolPos - 12);
					clientPlayer->setName(playerName);
					cout << "Player Name for " << ClientSocket << " setted: " << playerName << endl;

					// Set Player symbol
					string playerSymbol = msg.substr(playerSymbolPos + 14);
					clientPlayer->setSymbol(playerSymbol[0]);
					cout << "Player Symbol " << ClientSocket << " setted: " << playerSymbol[0] << endl;

					// Send Success message to client
					setPlayerSetMsg = "!PlayerSetSuccess";
				}
				else {
					// Send Fail message to client
					setPlayerSetMsg = "!PlayerSetFail";
				}
				// String to char array
				strcpy_s(sendbuf, setPlayerSetMsg.c_str());
				// Send response message to client
				send(ClientSocket, sendbuf, iRecvResult, 0);
				// Update the local nama variable
				playerName = clientPlayer->getName();
			}

			// Normal message
			else {
				// Add client/player name in front of the message
				string msgWithName = playerName + ": " + string(recvbuf);
				// String to char array
				strcpy_s(sendbuf, msgWithName.c_str());

				// Echo the buffer (message) to all clients
				for (pair<SOCKET, Player*> otherClient : ClientSocketsList) {
					// Do not send the message to source client
					if (otherClient.first == ClientSocket) continue;

					// Send message to other clients
					iSendResult = send(otherClient.first, sendbuf, msgWithName.length(), 0);

					// Send error
					if (iSendResult == SOCKET_ERROR) {
						cout << "Send failed to (" << otherClient.first << ") - "<< otherClient.second->getName() << ": " << WSAGetLastError() << endl;
						// Close socket and clean up
						closesocket(otherClient.first);
						WSACleanup();
						cout << "Socket " << ClientSocket << " closed." << endl;
						// Find the socket inside the vector
						auto toErease = std::find(ClientSocketsList.begin(), ClientSocketsList.end(), otherClient);
						// And then erase if found
						if (toErease != ClientSocketsList.end()) {
							// Delete from the clients vector
							ClientSocketsList.erase(toErease);
						}
						// Kill the thread
						return;
					}
					// Send successful
					else
						cout << "Bytes sent to (" << otherClient.first << ") - " << otherClient.second->getName() << ": " << iSendResult << endl << flush;
				}
			}
		}

		// Client left the server OR Data recieve failed
		else {
			// Cleint left
			if (iRecvResult == 0) {
				cout << "Player (" << ClientSocket << ") - " << playerName << " stopped the connection." << endl << flush;
			}
			// Recieve failed
			else {
				cout << "Recieve from (" << ClientSocket << ") - " << playerName << " failed: " << WSAGetLastError() << endl;
			}

			// Find the socket inside the linked-list
			auto toErease = std::find_if(ClientSocketsList.begin(), ClientSocketsList.end(), socketPairCompare(ClientSocket));
			// And then erase if found
			if (toErease != ClientSocketsList.end()) {
				// Delete from the clients vector
				ClientSocketsList.erase(toErease);
			}
			// Print number of players
			cout << "Current number of players: " << ClientSocketsList.size() << endl;

			// Generate a left the server message
			string leftMessage = "Player_" + playerName + "_has_left_the_server.";
			// String to char array
			strcpy_s(sendbuf, leftMessage.c_str());
			// Echo the left player to all clients
			for (pair<SOCKET, Player*> otherClient : ClientSocketsList)
				send(otherClient.first, sendbuf, leftMessage.length(), 0);
			
			// End the loop
			break;
		}
	} while (iRecvResult > 0);


	// 7. Close socket and Cleanup
	closesocket(ClientSocket);
	WSACleanup();
	return;
}
