#include "WaitingRoom.h"

#include <iostream>
#include <vector>
#include <windows.h> // for sleep()

using namespace std;

void WaitingRoom::Loading(bool wait) {
	cout << '-' << flush;
	while (wait) {
		Sleep(1000);
		cout << "\b\\" << flush;
		Sleep(1000);
		cout << "\b|" << flush;
		Sleep(1000);
		cout << "\b/" << flush;
		Sleep(1000);
		cout << "\b-" << flush;
	}
}

void WaitingRoom::PlayersList() {
	cout << "Waiting Room" << endl << endl;

	cout << "*------------------------------*" << endl;
	for (int i = 0; i < clients.size(); i++) {
		cout << "|" << i << ". " << clients[i]->getName();
		for (int j = 0; j < clients[i]->getName().length(); j++)
			cout << ' ';
		cout << '|' << endl;
	}
	cout << "*------------------------------*" << endl;
}

void WaitingRoom::PlayerJoined(Player* p) {
	numOfClients++;
	clients.push_back(p);
	return;
}

void WaitingRoom::PlayerLeft(Player* p) {
	// Find the player inside the vector
	auto toErease = std::find(clients.begin(), clients.end(), p);

	// And then erase if found
	if (toErease != clients.end()) {
		// Delete the pointer from the memory
		delete* toErease;
		// Delete from the clients vector
		clients.erase(toErease);
		// Decrease the number of clients
		numOfClients--;
	}
	return;
}