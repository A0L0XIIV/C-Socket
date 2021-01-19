#pragma once

#include<string>

class Player {
private:
	std::string name{ "Player" };
	char symbol{ 'o' };
public:
	Player() = default;
	Player(std::string Name, char sym) { name = Name; symbol = sym; };
	std::string getName() { return name; };
	char getSymbol() { return symbol; };
	void setName(std::string Name) { name = Name; };
	void setSymbol(char sym) { symbol = sym; };
}; 
