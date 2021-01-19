#pragma once

#include "Player.h"

#include <vector>

class WaitingRoom {
private:
	int numOfClients{ 0 };
	std::vector<Player*> clients;
public:
	WaitingRoom();
	void Loading(bool wait);
	void PlayersList();
	void PlayerJoined(Player* p);
	void PlayerLeft(Player* p);
};