#include <iostream>
#include <vector>
#include <windows.h> // for sleep()

using namespace std;

class WaitingRoom {
private:
	int numOfPlayers{ 0 };
	vector<string> players;
public:
	WaitingRoom() = default;
	void Loading(bool wait);
	void PlayersList();
	void PlayerJoined(string playerName);
	void PlayerLeft(string playerName);
};

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
	for (int i = 0; i < players.size(); i++) {
		cout << "|" << i << ". " << players[i];
		for (int j = 0; j < players[i].length(); j++)
			cout << ' ';
		cout << '|' << endl;
	}
	cout << "*------------------------------*" << endl;
}

void WaitingRoom::PlayerJoined(string playerName) {
	numOfPlayers++;
	players.push_back(playerName);
}

void WaitingRoom::PlayerLeft(string playerName) {
	numOfPlayers--;
	auto playIte = find(players.begin(), players.end(), playerName) - players.begin(); 
	players.erase(playIte);
}