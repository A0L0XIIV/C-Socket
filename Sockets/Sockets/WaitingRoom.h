#include <iostream>
#include <vector>
#include <windows.h> // for sleep()

#include "Player.h"

using namespace std;

class WaitingRoom {
private:
	int numOfClients{ 0 };
	vector<Player> clients;
public:
	WaitingRoom();
	void Loading(bool wait);
	void PlayersList();
	void PlayerJoined(Player p);
	void PlayerLeft(Player p);
};

void WaitingRoom::Loading(bool wait) {
	cout << '-' << flush;
	while(wait) {
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
		cout << "|" << i << ". " << clients[i].getName;
		for (int j = 0; j < clients[i].getName.length(); j++)
			cout << ' ';
		cout << '|' << endl;
	}
	cout << "*------------------------------*" << endl;
}

void WaitingRoom::PlayerJoined(Player p) {
	numOfClients++;
	clients.push_back(p);
}

void WaitingRoom::PlayerLeft(Player p) {
	numOfClients--;
	clients.erase(p);
}